#include "sigplus_internal.h"
#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "rfftw.h"
#include "audstr.h"
#include <vector>

#include "samplerate.h"

#define CRIT  100. // Threshold for computing rms is above 100th of its max

#define RETURN_0_MSG(str) {	strcpy(errstr, str);		return 0;	}

extern double *COE[];

int ReadWavInfo (const char *fname, WAVEFORMATEX * pwf, int *ind, char *errstr);
int ReadWavData (const char *fname, void * outBuffer);
int sortWavDataBufferByChannel(double *sorted, void *wavData, int ch, int nSamples, WAVEFORMATEX *wFormat);
int wavBuffer2snd (int DevID, short *playBuffer, int length, int nChan, int fs, UINT userDefinedMsgID, HWND hApplWnd, int nProgReport, char *errstr);
int wav2snd (const char *fname, UINT userDefinedMsgID, HWND hApplWnd, int nProgReport, char *errstr);
void MakeWavInfoBlock (char *block, int fs, short nChan, int nSamples);


void bsortbytmark(CSignals **chains, int nItems)
{
// Bubble sort, ascending order
	int p, q;
	CSignals dummy;
	for (p = 0; p < nItems; p++)
	{
		for (q = 1; q <= p; q++)
		{
			if ((*chains[q-1]).tmark > (*chains[q]).tmark)
			{
				dummy = *chains[q-1];
				*chains[q-1] = *chains[q];
				*chains[q] = dummy;
			}
		}
	}
}

#define DOUBLE2SHORT(x)	((short)(max(min((x),1),-1)*(double)0x007fff))

int _double_to_24bit(double x)
{
	// This maps a double variable raning -1 to 1, to a short variable ranging -16388608 to 16388607.
	return (int)(max(min(x,1),-1)*MAX_24BIT-.5);
}

double _24bit_to_double(int x)
{ // converts a short variable into double in a scale of 16388608.
	return ((double)x+.5)/MAX_24BIT;
}

void _short_to_24bit(short* pshort, int* pint, int len)
{ // Assumes that memory blocks have been prepared.
	for (int i=0; i<len; i++) pint[i] = pshort[i]<<8;
}

void _24bit_to_short(int* pint, short* pshort, int len)
{ // Assumes that memory blocks have been prepared.
	for (int i=0; i<len; i++) pshort[i] = (short)(pint[i]>>8);
}

void _double_to_short(double* dint, short* pshort, int len)
{
	for (int i=0; i<len; i++) 
		pshort[i] = (short)(_double_to_24bit(dint[i]) >> 8);
}

int getmaxIndex (double *x, int len)
{
	int res=0;	
	double cmax=0.;
	for (int i=0; i<len; i++)	
	{
		if ( fabs(x[i]) > cmax )
			{ cmax = fabs(x[i]);	res = i; } 
	}
	return res;
}

void GetIndGreaterThanCrit(double *x, int len, double Crit, int *foundIndices)  
{ // equivalent to find command in Matlab
	int i,j=0;	
	for (i=0; i<len;i++)	
		if (x[i]>Crit)	
			foundIndices[j++] = i; 
		else if (x[i]<-Crit) 
			foundIndices[j++] = i;
}

double _getdB(double x)
{
	// 3 dB is added to make rms of full scale sinusoid 0 dB
	return 20*log10(x) + 3.0103;
}

double getrms (double *x, int len, int *indices) 
{
	int i;
	double cum=0.;
	if (indices==NULL)	// full computation of rms, no selection.
		for (i=0; i<len; i++) 
			cum += x[i]*x[i];
	else
		for (i=0; i<len && indices[i]!=-1; i++)
			cum += x[indices[i]]*x[indices[i]];
	cum /= i;	
	return sqrt(cum);
}

EXP_CS CSignals::CSignals()
:tmark(0), tfraction(0), nSamples(0), fs(1), BufSize(1), buf(new double[1]), next(NULL), chain(NULL)
{
}

EXP_CS CSignals::CSignals(int sampleRate)
:tmark(0), tfraction(0), nSamples(0), fs(max(sampleRate,1)), BufSize(1), buf(new double[1]), next(NULL), chain(NULL)
{
}

EXP_CS CSignals::CSignals(double value)
:tmark(0), tfraction(0), nSamples(0), fs(1), BufSize(1), buf(new double[1]), next(NULL), chain(NULL)
{
	SetValue(value);
}

EXP_CS CSignals::CSignals(const CSignals& src)
:nSamples(0), BufSize(1), buf(new double[1]), next(NULL), chain(NULL)
{
	*this = src;
}

EXP_CS CSignals::CSignals(const char* wavname)
:tmark(0), nSamples(0), fs(1), BufSize(1), buf(new double[1]), next(NULL), chain(NULL)
{
	char errstr[256];
	Wavread(wavname, errstr);
}

EXP_CS CSignals::~CSignals()
{
	if (buf) {
		delete[] buf; 
		buf=NULL;
	}
	if (chain) {
		delete chain;
		chain=NULL;
	}
	if (next) {
		delete next;
		next=NULL;
	}
}

EXP_CS CSignals& CSignals::Reset(int fs2set)	// Empty all data fields - sets nSamples to 0.
{
	if (fs2set)	// if fs2set == 0 (default), keep the current fs.
		fs = max(fs2set,1);	// not to allow minus.
	tmark = 0;
	tfraction = 0;
	nSamples = 0;
	if (next) {
		delete next;
		next=NULL;
	}
	if (chain) {
		delete chain;
		chain=NULL;
	}
	return *this;
}

EXP_CS CSignals& CSignals::UpdateBuffer(int length)	// Set nSamples. Re-allocate buf if necessary to accommodate new length.
{
	if (length < 0 || length == nSamples)
		return *this;
	if ((size_t)length > BufSize || (size_t)length < BufSize/10) {
		BufSize = (size_t)(length*1.1)+1;	// allocate extra space for growth.
		double *newbuf = new double[BufSize];
		if (nSamples>0)
			memcpy(newbuf, buf, sizeof(*buf)*min(nSamples,length));
		delete[] buf;
		buf = newbuf;
	}
	if (length > nSamples)
		memset(buf+nSamples, 0, sizeof(*buf)*(length-nSamples)); //initializing with zeros for the rest
	nSamples = length;
	return *this;
}

EXP_CS void CSignals::SetChain(CSignals *unit, double time_shifted)
{
	if (unit!=NULL)
	{
		if (nSamples>0)  delete[] buf;
		buf = NULL; 
		chain = unit;
	}
}

EXP_CS void CSignals::SetChain(double time_shifted)
{
	if (nSamples>0)
	{
		tmark = time_shifted;
		chain = this;
		buf = NULL;
	}
}

EXP_CS CSignals& CSignals::operator=(const CSignals& rhs)
{   // Make a deep copy only for buf, but not for sc, because sc is not necessarily dynamically allocated.
	// Thus, BE Extra careful when making writing an assignment statement about the scaling..
	if (this != &rhs) 
	{
		Reset(rhs.fs);
		tmark = rhs.tmark;
		tfraction = rhs.tfraction;
		if (rhs.nSamples>0) {
			UpdateBuffer(rhs.nSamples);
			memcpy(buf, rhs.buf, nSamples * sizeof(*buf));
		}
		if (rhs.chain) {
			chain = new CSignals;
			*chain = *rhs.chain;
		}
		SetNextChan(rhs.next);
	}
	return *this; // return self-reference so cascaded assignment works
}

void CSignals::SwapContents1node(CSignals &sec)
{	// swaps  fs, buf(shallow), chain(shallow), tmark, tfraction, BufSize, nSamples
	// *** Leaves "next" intact!!!
	CSignals tmp(fs);
	tmp.buf = buf, tmp.chain = chain, tmp.tmark = tmark, tmp.tfraction = tfraction, tmp.BufSize = BufSize, tmp.nSamples = nSamples;	// tmp = *this
	fs = sec.fs, buf = sec.buf, chain = sec.chain, tmark = sec.tmark, tfraction = sec.tfraction, BufSize = sec.BufSize, nSamples = sec.nSamples;	// *this = sec
	sec.fs = tmp.fs, sec.buf = tmp.buf, sec.chain = tmp.chain, sec.tmark = tmp.tmark, sec.tfraction = tmp.tfraction, sec.BufSize = tmp.BufSize, sec.nSamples = tmp.nSamples;	// sec = tmp
	// clean up, so that destructor don't destroy chain and buf which are being used by sec
	tmp.buf = NULL, tmp.chain = NULL, tmp.tmark = 0, tmp.tfraction = 0, tmp.BufSize = 0, tmp.nSamples = 0;
}

#define SIGNAL_OP(OP, CHAIN_FN) \
CSignals& CSignals::operator OP=(const double con) \
{ \
	for (int i=0; i<nSamples; i++) \
		buf[i] OP= con; \
	if (chain) \
		*chain OP= con; \
	return *this; \
} \
CSignals& CSignals::operator OP=(CSignals &sec) \
{ \
	if (IsSingle() && !sec.IsSingle()) \
		SwapContents1node(sec); \
	if (sec.next && !next) \
		next = new CSignals(*this); \
	if (sec.IsSingle()) { \
		if (IsSingle()) \
			buf[0] = value() OP sec.value(); \
		else \
			*this OP= sec.value(); \
	} else	/* now for two vectors */ \
		CHAIN_FN(&sec); \
	if (sec.next) \
		*next OP= *sec.next; \
	else if (next) \
		*next OP= sec; \
	return *this; \
} \
void CSignals::CHAIN_FN(CSignals *sec) \
{ \
	if (fs != sec->fs) { \
		if (fs>1 && sec->fs<=1) \
			sec->fs = fs; \
		else if (fs<=1 && sec->fs>1) \
			fs = sec->fs; \
		else \
			throw "The sampling rates of both operands must be the same."; \
	} \
	if (tmark > sec->tmark || (tmark == sec->tmark && nSamples < sec->nSamples)) \
		SwapContents1node(*sec); \
	/* now, (tmark <= sec->tmark) is always true */ \
	if (tmark + GetDuration() > sec->tmark)	{ \
		/* overlapping signals */ \
		int id0 = round((sec->tmark-tmark)/1000.*fs); \
		if (nSamples-id0 >= sec->nSamples) { \
			for (int i=0; i<sec->nSamples; i++) \
				buf[id0+i] OP= sec->buf[i]; \
			if (sec->chain) \
				CHAIN_FN(sec->chain); \
		} else { \
			/* sec->buf will get the new samples and be kept */ \
			for (int i=0; i<nSamples-id0; i++) \
				sec->buf[i] OP= buf[id0+i]; \
			UpdateBuffer(id0);	/* waste the rest (overlapping portion) */ \
			if (chain) { \
				sec->CHAIN_FN(chain); \
				delete chain; \
			} \
			chain = new CSignals(); \
			chain->SwapContents1node(*sec); \
		} \
	} else \
		if (chain) { \
			CSignals *p; \
			for (p=chain; p->chain && p->tmark + p->GetDuration() <= sec->tmark; p=p->chain) \
				; \
			p->CHAIN_FN(sec); \
		} else { \
			chain = new CSignals(); \
			chain->SwapContents1node(*sec);	/* swap, so sec will be empty and its destructor will not delete anything. */ \
		} \
}

SIGNAL_OP(+, PlusChain)
SIGNAL_OP(*, MultChain)



EXP_CS CSignals& CSignals::operator-=(CSignals &sec)
{
	return *this += -sec;
}
EXP_CS CSignals& CSignals::operator-(void)	// Unary minus
{
	if (IsScalar())
		SetValue(-value());
	else {
		for (int i=0; i<nSamples; i++)
			buf[i] = -buf[i];
		if (next)
			-*next;
		if (chain)
			-*chain;
	}
	return *this;
}

EXP_CS CSignals& CSignals::operator/=(double scaleFactor)
{
	return *this *= 1.0/scaleFactor;
}
EXP_CS CSignals& CSignals::operator/=(CSignals &scaleArray)
{
	return *this *= scaleArray.Reciprocal();
}
EXP_CS CSignals& CSignals::Reciprocal(void)
{
	if (IsScalar())
		SetValue(1.0/value());
	else {
		for (int i=0; i<nSamples; i++)
			buf[i] = 1.0/buf[i];
		if (next)
			next->Reciprocal();
		if (chain)
			chain->Reciprocal();
	}
	return *this;
}


EXP_CS CSignals& CSignals::operator<<=(const double delta)
{
	*this>>=(-delta);
	return *this;
}

EXP_CS CSignals& CSignals::operator>>=(const double delta)
{
	if (delta == 0)
		return *this;
	if (next)
		*next>>=delta;

	if (tmark+delta < 0) {
		// cut off negative time domain
		CSignals *p, *pp;
		double half_atom_time = 500./fs;
		for (pp=p=this; p && p->tmark+delta+p->GetDuration()<=half_atom_time; pp=p, p=p->chain)
			;
		if (p == NULL) {	// nothing in the range.
			Reset();
			return *this;
		}
		if (p != this) {
			pp->chain = NULL;	// to stop destruction at pp. This has to be done before SwapContents1node() for the cases of this==pp. jhpark 5/29/2012.
			SwapContents1node(*p);	// *p gets *this
			delete p;	// delete from head through pp
		}
		if (tmark+delta < 0) {	// patial cut-off
			Truncate(-(tmark+delta), GetDuration());
			tmark = -delta;	// in order to be zero later by tmark+=delta.
		}
	}
	tmark += delta;
	if (chain)
		*chain>>=delta;
	return *this;
}


EXP_CS CSignals& CSignals::operator<(const CSignals &sec)
{
	if (!IsScalar() || !sec.IsScalar())
		throw "The operands of operator '<' must be scalars.";
	SetValue(value() < sec.value());
	return *this;
}

EXP_CS CSignals& CSignals::operator>(const CSignals &sec)
{
	if (!IsScalar() || !sec.IsScalar())
		throw "The operands of operator '>' must be scalars.";
	SetValue(value() > sec.value());
	return *this;
}

EXP_CS CSignals& CSignals::operator<=(const CSignals &sec)
{
	if (!IsScalar() || !sec.IsScalar())
		throw "The operands of operator '<=' must be scalars.";
	SetValue(value() <= sec.value());
	return *this;
}

EXP_CS CSignals& CSignals::operator>=(const CSignals &sec)
{
	if (!IsScalar() || !sec.IsScalar())
		throw "The operands of operator '>=' must be scalars.";
	SetValue(value() >= sec.value());
	return *this;
}

EXP_CS bool CSignals::operator==(const CSignals &sec) const
{
	if (IsScalar() && sec.IsScalar())
		return (value() == sec.value());
	else if (!IsScalar() && !sec.IsScalar()) {
		if (nSamples != sec.nSamples)
			return false;
		if (memcmp(buf, sec.buf, nSamples*sizeof(*buf)) == 0) {
			if (chain || sec.chain) {
				if (chain && sec.chain) {
					if (*chain != *sec.chain)
						return false;
				} else
					return false;
			}
			if (next || sec.next) {
				if (next && sec.next) {
					if (*next != *sec.next)
						return false;
				} else
					return false;
			}
			return true;
		}
	}
	return false;
}

EXP_CS CSignals& CSignals::operator!()
{
	if (!IsScalar())
		throw "The operand of operator '!' must be scalars.";
	SetValue(!value());
	return *this;
}

EXP_CS CSignals& CSignals::operator&&(const CSignals &sec)
{
	if (!IsScalar() || !sec.IsScalar())
		throw "The operands of operator '&&' must be scalars.";
	SetValue(value() && sec.value());
	return *this;
}

EXP_CS CSignals& CSignals::operator||(const CSignals &sec)
{
	if (!IsScalar() || !sec.IsScalar())
		throw "The operands of operator '!=' must be scalars.";
	SetValue(value() || sec.value());
	return *this;
}
/*
EXP_CS void CSignals::AddChain(CSignals *sec)
{ // assume that nSamples>1 in both. 
	//MAKE SURE TO AVOID CIRCULAR CHAINING....IT WILL CREATE A STACK OVERFLOW

	if (chain==NULL) chain = sec;	// By jhpark 12/10/2009
	else chain->AddChain(sec);
}
*/

EXP_CS void CSignals::AddChain(CSignals &sec)
{ // assume that nSamples>1 in both. 
	//MAKE SURE TO AVOID CIRCULAR CHAINING....IT WILL CREATE A STACK OVERFLOW

	if (chain==NULL) 
	{
		chain = new CSignals;
		*chain = sec;
	}
	else chain->AddChain(sec);
}

CSignals * CSignals::ExtractDeepestChain(CSignals *dummy)
{
	// breaks and returns the deepest chain. Without dummy it would try to return local variable
	if (chain==NULL) return NULL;
	else if (chain->chain==NULL) return BreakChain(dummy);
	else return chain->ExtractDeepestChain(dummy);
}

CSignals * CSignals::BreakChain(CSignals *dummy)
{
	// returns the broken chain
	if (chain==NULL) return NULL;
	dummy = chain;
	chain = NULL;
	return dummy;
}

CSignals * CSignals::GetDeepestChain()
{
	if (chain==NULL) return NULL;
	else if (chain->chain==NULL) return chain;
	else return chain->GetDeepestChain();
}

int CSignals::GetHowmanyChains()
{
	int res(0);
	CSignals *xx, copy(*this), *chains;
	chains = new CSignals;
	do
	{
		xx = copy.ExtractDeepestChain(chains);
		res++;
		delete xx;
	} while (xx!=NULL);
	delete chains;
	return res;
}

void CSignals::Chain2Array(CSignals **chains, CSignals *forthis)
{
	int res(0);
	CSignals dummy, *out;
	do
	{
		out = ExtractDeepestChain(&dummy);
		if (out==NULL) { *forthis = *this; chains[res] = forthis; }
		else chains[res] = out; 
		res++;
	} while (out!=NULL);
	bsortbytmark(chains, res);
}

//void CSignals::SortMultiple()
//{
//
//}

double CSignals::CalculateTotalDuration()
{
	CSignals *p;
	for (p=this; p->chain; p=p->chain)
		;
	return p->tmark + p->GetDuration();
}

double CSignals::MakeChainless(double dur, int *pNewFS)
{ //This converts the null intervals of the signal to zero.
	if (pNewFS)
		fs = max(fs, *pNewFS);
	else
		fs = max(fs, 1);
	double totaldur = CalculateTotalDuration();	// doing this here as fs might have changed.
	double newdur = max(totaldur, dur);
	if (next) {
		newdur = next->MakeChainless(newdur, &fs);
		totaldur = CalculateTotalDuration();	// doing this again as fs might have changed.
	}
	if (pNewFS)
		*pNewFS = fs;
	if (!tmark && !chain && totaldur == newdur)	// already chainless && no padding required.
		return newdur;

	CSignals nsig(fs);
	nsig.UpdateBuffer(round(newdur/1000.*fs));
	for (CSignals *p=this; p; p=p->chain) {
		if (p->tmark + p->GetDuration() <= 0)
			continue;
		int iorg = (p->tmark < 0) ? round(-p->tmark/1000.*fs) : 0;
		int inew = (p->tmark > 0) ? round(p->tmark/1000.*fs) : 0;
		int cplen = p->GetLength()-iorg;
		if (inew+cplen > nsig.nSamples) {
			if (p->chain == NULL && inew+cplen == nsig.nSamples+1 && newdur/1000.*fs-nSamples > 0.499999)	// last chain, only 1 sample difference, possile rounding error.
				nsig.UpdateBuffer(nsig.nSamples+1);
			else
				throw "Internal error: Buffer overrun detected.";
		}
		memcpy(nsig.buf+inew, p->buf+iorg, cplen*sizeof(*buf));
	}
	SwapContents1node(nsig);	// *this gets the chainless signal, nsig gets the chained signal and will be destroyed soon.
	return newdur;
}

EXP_CS const CSignals& CSignals::operator+=(CSignals *yy)
{ // Concatenation of CSignals
  // yy might lose its value.
/* if *this is mono, insertee is mono, just do it 
			   mono,             stereo, take first chan of insertee,
			   stereo,           mono, apply insertee equally to both channels
			   stereo,           stereo, just do it   */

	// when this->fs doesn't seem valid, use insertee's fs.
	if (GetFs()<2)
		SetFs(yy->GetFs());

	CSignals *ptail = this;
	for (; ptail->chain; ptail=ptail->chain)	// get the tail of *this.
		;
	if (yy->chain || yy->tmark) {
		// when insertee has chain(s), chain it instead of copying. yy loses its value.
		CSignals *pNew = new CSignals();
		if (next && !yy->next)
			*pNew = *yy;	// make a copy, leaving the original intact for the next channel
		else
			pNew->SwapContents1node(*yy);		// pNew takes the insertee. As yy lost all its pointers, yy can be destroyed later without destroying any data it used to have.
		*pNew >>= ptail->tmark+ptail->GetDuration();	// to make the insertee the next chain(s) of ptail.
		ptail->chain = pNew;					// link it to *this.
	} else {
		// otherwise, just copy at the end.
		int nSamples0 = ptail->nSamples;
		ptail->UpdateBuffer(yy->nSamples + nSamples0);
		if (yy->nSamples>0)
			memcpy(ptail->buf+nSamples0, yy->buf, yy->nSamples*sizeof(*buf));
	}
	// when *this is stereo
	if (next) {
		if (yy->next)
			*next += yy->next;	// stereo += stereo
		else
			*next += yy;		// stereo += mono
	}
	return *this;
}


EXP_CS inline int CSignals::GetLength() // number of samples
{
	return nSamples;
}

EXP_CS double CSignals::GetDuration() // duration in ms.
{
	return (double)nSamples/fs*1000. + tfraction;
}

EXP_CS double CSignals::Levels(double *peak, int *index)
{
	// Returns rms (dB) re full scale
	int *foundindices;
	double rms, maxy;

	foundindices = new int[nSamples];
	memset ((void*)foundindices, 0xff, nSamples*sizeof(int));
	maxy = buf[getmaxIndex(buf, nSamples)];
	if (maxy<0.) maxy = -maxy;
	if (peak!=NULL)	*peak = _getdB(maxy);
	if (index!=NULL) *index = getmaxIndex(buf, nSamples);

	GetIndGreaterThanCrit (buf, nSamples, maxy/CRIT, foundindices);
	rms = _getdB(getrms(buf, nSamples, foundindices));
	delete[] foundindices;
	return rms;
}

EXP_CS double CSignals::LevelLT()
{
	// Long term rms level---- not excluding the points below the threshold as done in Levels()
	// This is normally what we call the long term rms, which is used for SNR computation.
	// But, I don't intend to use this whenever SNR is computed.... still when speech is added
	// to noise with some time delay... I'd like to exclude the silence portion of speech
	// so that the relative level is independent of the delay.
	// I am writing this only to compute the rms of gated (100%) noise.
	
	// Now ignores initial null. 12/29/2011 jhpark
	if (!chain)
		return _getdB(getrms(buf, nSamples, NULL));

	double cum = 0;
	for (CSignals *p=this; p; p=p->chain)
		for (int i=0; i<p->nSamples; ++i) 
			cum += p->buf[i] * p->buf[i];
	int n = round((CalculateTotalDuration()-tmark)/1000.*fs);
	return _getdB(sqrt(cum/n));
}

EXP_CS CSignals CSignals::LevelLTmc()
{
	CSignals res;
	int chan = 0;
	for (CSignals *p=this; p; ++chan, p=p->next) {
		res.UpdateBuffer(chan+1);
		res.buf[chan] = p->LevelLT();
	}
	return res;
}

EXP_CS double CSignals::Levels()
{
	return Levels((double*)NULL, (int*)NULL);
}

EXP_CS int CSignals::Wavread(const char *wavname, char *errstr)
{
	void *wavdataBuffer;
	WAVEFORMATEX wfx;
	int i, nFrames, nBytesSample, nBytesFrame;
	CSignals *sig[2], dumm;

	errstr[0] = '\0';
	nFrames = ReadWavInfo (wavname, &wfx, &i, errstr);
	if (nFrames<=0) {
		if (errstr[0] == NULL)
			strcpy(errstr, "Invalid wave header");
		sprintf(errstr+strlen(errstr), " - %s", wavname);
		return 0;
	}
	//if bits per sample is outside 8 of 24 bits, return error here.
	if ( wfx.wBitsPerSample<8 || wfx.wBitsPerSample>24) { strcpy(errstr, "Bits per sample must be between 8 and 24 for this release."); return NULL;}

	nBytesSample = wfx.wBitsPerSample / 8 ;
	nBytesFrame = nBytesSample * wfx.nChannels;
	wavdataBuffer = calloc (nFrames, nBytesFrame);
	sig[0] = this;
	for (i=0; i<wfx.nChannels && i<2; i++) //no more than 2 channels in this version
	{
		if (i>0) 
		{ sig[i] = next = new CSignals((int)wfx.nSamplesPerSec); } // no need to worry about cleaning this (taken care of by the destructor)
		else
		{
		sig[i]->fs = wfx.nSamplesPerSec;
		sig[i]->Reset(sig[i]->fs);
		}
		sig[i]->UpdateBuffer(nFrames);
		
		nFrames=ReadWavData (wavname, wavdataBuffer);
		// the wave raw data block is sorted into an integer array (regardless of wBitsPerSample) 
		sortWavDataBufferByChannel(sig[i]->buf, wavdataBuffer, i, nFrames, &wfx);
	}
	free(wavdataBuffer);
	if (errstr!=NULL)		strcpy(errstr, "");
	return 1;
}

#define WriteFile_CHECK(i,y) \
if (i==0 || (int)dww!=y) {wsprintf (errstr, "WriteFile failed. code=%d", GetLastError());	return 0; }


EXP_CS int CSignals::Wavwrite(const char *wavname, char *errstr)
{
	// For now, only 16-bit format is used. (Sep 2008)
	char block[44];
	HANDLE h;
	DWORD dww;
	short *sbuf;
	int r;

	if (fs <= 0) {
		strcpy(errstr, "Invalid sampling rate");
		return 0;
	}
	if ((h = CreateFile (wavname, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) )==INVALID_HANDLE_VALUE) {
		LPVOID lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0, NULL );
		sprintf(errstr, "Cannot create the file - %s - '%s'", lpMsgBuf, wavname);
		LocalFree(lpMsgBuf);
		return 0; // file not exist
	}  
	MakeChainless();
	if (next == NULL) {	// single channel
		MakeWavInfoBlock (block, fs, 1, nSamples);
		r = WriteFile (h, block, 44, &dww, NULL);
		WriteFile_CHECK(r,44)
		sbuf = new short[nSamples];
		for (int i=0; i<nSamples; i++)
			sbuf[i] = DOUBLE2SHORT(buf[i]);
		r = WriteFile (h, sbuf, nSamples*2, &dww, NULL);
		WriteFile_CHECK(r,nSamples*2)
	} else {	// two channel, stereo
		MakeWavInfoBlock (block, fs, 2, nSamples);
		r = WriteFile (h, block, 44, &dww, NULL);
		WriteFile_CHECK(r,44)
		sbuf = new short[nSamples*2];
		for (int i=0; i<nSamples; i++) {
			sbuf[i*2] = DOUBLE2SHORT(buf[i]);
			sbuf[i*2+1] = DOUBLE2SHORT(next->buf[i]);
		}
		r = WriteFile (h, sbuf, nSamples*4, &dww, NULL);
		WriteFile_CHECK(r,nSamples*4)
	}
	delete[] sbuf;
	CloseHandle(h);
	return 1;
}


EXP_CS inline double* CSignals::GetBuffer()
{
	return buf;
}


EXP_CS void *CSignals::PlayArray(char *errstr)
{
	return PlayArray(0, errstr);
}

EXP_CS void *CSignals::PlayArray(int DevID, char *errstr)
{ // returns a negative number if error occurrs
	return PlayArray(DevID, 0, NULL, 2, errstr); 
	// This is how you play in blocking mode (specify 2 for the nProgReport even though you are not utilizing any messaging back to hWnd.. This is just due to the way wavBuffer2snd is written in waves.cpp)
	// Jan 19, 2013. BJ Kwon
}

EXP_CS void *CSignals::PlayArray(int DevID, UINT userDefinedMsgID, HWND hApplWnd, double *block_dur_ms, char *errstr)
{// returns a negative number if error occurrs
 // This play the sound by specified block duration, generating event notification in every block
 // block_dur_ms is adjusted by the quantization of fs. Therefore, user should check if it has beend adjusted during this call.
 	int nSamples4Block = (int)(*block_dur_ms/(1000./(double)fs)+.5);
	*block_dur_ms = (double) nSamples4Block *1000. /(double)fs;
	double _nBlocks = (double)nSamples / nSamples4Block;
	int nBlocks = (int)_nBlocks;
	if (_nBlocks - (double)nBlocks > 0.1) nBlocks++;
	return PlayArray(DevID, userDefinedMsgID, hApplWnd, nBlocks, errstr);
}

EXP_CS void *CSignals::PlayArray(int DevID, UINT userDefinedMsgID, HWND hApplWnd, int nProgReport, char *errstr)
{// returns a negative number if error occurrs
	//For now this is only 16-bit playback (Sep 2008)
	int nChan;
	short *Buffer2Play;
	MakeChainless();
	errstr[0]='\0';
	if (next!=NULL)
	{
		double *buf2 = next->buf;
		Buffer2Play = new short[nSamples*2];
		for (int i=0; i<nSamples; ++i) {
			Buffer2Play[i*2] = (short)(_double_to_24bit(buf[i]) >> 8);
			Buffer2Play[i*2+1] = (short)(_double_to_24bit(buf2[i]) >> 8);
		}
		nChan=2;
	}
	else
	{
		Buffer2Play = new short[nSamples];
		_double_to_short(buf, Buffer2Play, nSamples);
		nChan=1;
	}
	return PlayBufAsynch16(DevID, Buffer2Play, nSamples, nChan, fs, userDefinedMsgID, hApplWnd, nProgReport, errstr);
}

EXP_CS CSignals& CSignals::Interp(const CSignals& gains, const CSignals& tmarks)
{
	if (gains.nSamples!=tmarks.nSamples)
		throw "The length of both arguments of interp( ) must be the same.";
	int id1(0), id2;
	double gain1(0.), slope;
	int newSampleCount = round(tmarks.buf[tmarks.nSamples-1]/1000.*fs);
	Reset();
	UpdateBuffer(newSampleCount);
	for (int i=0; i<tmarks.nSamples; i++)
	{
		id2 = round(tmarks.buf[i]/1000.*fs);
		slope = (gains.buf[i]-gain1) / (id2-id1);
		for (int j=id1; j<id2; j++) 
			buf[j] = slope * (j-id1) + gain1;
		gain1 = gains.buf[i];
		id1 = id2;
	}
	return *this;
}

EXP_CS CSignals& CSignals::Take(CSignals& out, int id1)
{ // if ending point is missed, it assumes the end of the whole duration.
	if (id1==0) {out = *this; return out;}
	return Take(out, id1, nSamples-1);
}

EXP_CS CSignals& CSignals::Take(CSignals& out, double begin_ms)
{ // if ending point is missed, it assumes the end of the whole duration.
	int id1 = round(begin_ms/1000.*fs);
	return Take(out, id1, nSamples-1);
}

EXP_CS CSignals& CSignals::Take(CSignals& out, int id1, int id2)
{
	id2 = min (nSamples, id2);
	if (id2<id1)		return *this;
	out.Reset();
	int nSamplesNeeded = id2-id1+1;
	out.UpdateBuffer(nSamplesNeeded);
	memcpy((void*)&out.buf[-min(0,id1)], (void*)&buf[max(0,id1)], sizeof(double)*(id2-max(0,id1)+1));
	if (next!=NULL) next->Take(*out.next, id1, id2);
	return out;
}

EXP_CS CSignals& CSignals::Take(CSignals& out, double begin_ms, double end_ms)
{ /* if id1 is negative, zero's are added before the wave form.
  if id2 is greater than current length, the end of current array will be taken.
	
// If there's an error, extracting is not happening and this pointer will be the output.
// Error will occur in one of the following:
	1) when id2 is the same or smaller than id1.
*/
	int id1 = round(begin_ms/1000.*fs);
	int id2 = round(end_ms/1000.*fs)-1;
	Take(out, id1, id2);
	out.tfraction = (end_ms-begin_ms) - (id2-id1+1)/(double)fs*1000.;
	return out;
}

EXP_CS CSignals& CSignals::Trim(double begin_ms, double end_ms)
{
	CSignals *pp, *p;
	
	if (begin_ms >= end_ms) {
		Reset();
		return *this;
	}
	// for subsequent channels
	if (next)
		next->Trim(begin_ms, end_ms);

	// excess segment
	for (p=pp=this; p && p->tmark<end_ms; pp=p,p=p->chain)
		;
	// now, p->tmark >= end_ms || p == null
	if (p == this) {	// nothing in the range
		CSignals *tnext = next;
		next = NULL;	// to preserve *next through Reset()
		Reset();
		next = tnext;
		return *this;
	} else if (p)	// get rid of p and the rest
		delete p;
	// last segment
	pp->chain = NULL;	// was p, which has been deleted.
	// discard the portion beyond end_ms
	int nSamplesNeeded = round((end_ms-pp->tmark)/1000.*fs);
	if (nSamplesNeeded < pp->nSamples) {
		pp->UpdateBuffer(nSamplesNeeded);
		pp->tfraction = (end_ms-pp->tmark) - nSamplesNeeded/(double)fs*1000.;
	}
	/* do we really need this? It can make problem with dur and tend function.
	else if (nSamplesNeeded > pp->nSamples) {
		// make a section of trailing null signal
		pp->chain = new CSignals(fs);
		pp->chain->tmark = end_ms;
	}
	*/
	// shift trimmed signal
	return *this>>=(-begin_ms);
}


EXP_CS void CSignals::Dramp(double dur_ms, int beginID)
{
	MakeChainless();

	if (next!=NULL) next->Dramp(dur_ms, beginID);
	// Cosine square ramping and damping of the array
	int nSamplesNeeded;
	double x, drampFs ;
	drampFs = 1.e3/(4.*dur_ms);
	nSamplesNeeded = round(dur_ms/1000.*fs); 
	nSamplesNeeded = min (nSamples, nSamplesNeeded);

	for (int i=0; i<nSamplesNeeded; i++)
	{
		x = sin(2*PI*drampFs*(double)i/(double)fs);
		buf[beginID+i] = x * x * buf[beginID+i];
		buf[nSamples-i-1] = x * x * buf[nSamples-i-1]; 
	}
}

EXP_CS double * CSignals::Modulate (double *env, int lenEnv)
{
	MakeChainless();

	if (next!=NULL) next->Modulate(env, lenEnv);
	// Modulate this object with env 
	// If lenEnv may be greater than nSamples, then the samples in env after nSamples are ignored.
	for (int i=0; i<nSamples && i<lenEnv; i++)	buf[i] *= env[i];
	return buf;
}

EXP_CS double * CSignals::Modulate (CSignals &secondsignal)
{ // Takes only first channel of secondsignal for modulation
	MakeChainless();

	if (next!=NULL) next->Modulate(secondsignal);
	// New length is determined by the shorter signal
	double *buf2 = secondsignal.GetBuffer();
	for (int i=0; i<nSamples && i<secondsignal.GetLength(); i++)
		buf[i] *= secondsignal.buf[i];
	return buf;
}

EXP_CS double * CSignals::Noise(double dur_ms)
{
	int nSamplesNeeded = round(dur_ms/1000.*fs);
	double *p = Noise(nSamplesNeeded);
	tfraction = dur_ms - nSamplesNeeded/(double)fs*1000.;
	return p;
}

EXP_CS double * CSignals::Noise(int nsamples)
{
	Reset();
	UpdateBuffer(nsamples); //allocate memory if necessary
	for (int i=0; i<nsamples; i++)
	{
		buf[i] = 2.*((double)rand()/(double)RAND_MAX-.5);
//		buf[i] = _double_to_24bit(x);
	}
	return buf;
}

EXP_CS double * CSignals::Noise2 (double dur_ms)
{ //Gaussian noise
	int nSamplesNeeded = round(dur_ms/1000.*fs); 
	double *p = Noise2(nSamplesNeeded);
	tfraction = dur_ms - nSamplesNeeded/(double)fs*1000.;
	return p;
}

EXP_CS double * CSignals::Noise2 (int nsamples)
{ //Gaussian noise
	double fac, r, v1, v2, sum(0.);
	Reset();
	UpdateBuffer(nsamples); //allocate memory if necessary
	for (int i=0; i<nsamples; i++)
	{
		if (i==5557) v1=10.;
		do {
			do {
					v1 = (2.0 * (double)rand()/(double)RAND_MAX ) - 1.0;
					v2 = (2.0 * (double)rand()/(double)RAND_MAX ) - 1.0;
					r = (v1*v1) + (v2*v2);
				} while (r >= 1.0);
			fac = sqrt(-2.0 * log(r) / r);
		} while (v2*fac>=1.0 || v2*fac<=-1.0);
		buf[i] = v2*fac;
		sum += v2*fac;
	}
	return buf;
}

EXP_CS double * CSignals::fm(double midFreq, double fmWidth, double fmRate, int nsamples, double beginFMPhase)
{   // beginFMPhase is to be set. (beginPhase is zero here ==> Its not so meaningful to set both of them)
	double t;
	Reset();
	UpdateBuffer(nsamples); //allocate memory if necessary
	for (int i=0; i<nsamples; i++)
	{
		t = (double)i/(double)fs;
		buf[i] = sin(2*PI*t*midFreq-fmWidth/fmRate*cos(2*PI*(fmRate*t+beginFMPhase)));
	}
	return buf;
}

EXP_CS double * CSignals::fm(double midFreq, double fmWidth, double fmRate, double dur_ms, double beginFMPhase)
{   
	int nSamplesNeeded = round(dur_ms/1000.*fs); 
	double *p = fm(midFreq, fmWidth, fmRate, nSamplesNeeded, beginFMPhase);
	tfraction = dur_ms - nSamplesNeeded/(double)fs*1000.;
	return p;
}


//beginPhase is normalized. 1.0 == full phase (2*PI)

EXP_CS double * CSignals::Tone(double *freq, int nsamples, double beginPhase)
{//Frequency glide: freq is a two-element array with beginning freq and ending freq
	double t, glidelta(freq[1] - freq[0]),  duration((double)nsamples/(double)fs);
	Reset();
	UpdateBuffer(nsamples); //allocate memory if necessary
	for (int i=0; i<nsamples; i++)
	{
		t = (double)i/(double)fs;
		buf[i] = sin(2*PI*t*(freq[0]+glidelta/2./duration*t+beginPhase)); // check the phase
	}
	return buf;
}

EXP_CS double * CSignals::Tone(double *freq, double dur_ms, double beginPhase)
{
	int nSamplesNeeded = round(dur_ms/1000.*fs); 
	double *p = Tone(freq, nSamplesNeeded, beginPhase);
	tfraction = dur_ms - nSamplesNeeded/(double)fs*1000.;
	return p;
}

EXP_CS double * CSignals::Tone(double freq, int nsamples, double beginPhase)
{
	Reset();
	UpdateBuffer(nsamples); //allocate memory if necessary
	for (int i=0; i<nsamples; i++)
		buf[i] = sin(2*PI*(freq*(double)i/(double)fs+beginPhase));
	return buf;
}

EXP_CS double * CSignals::Tone(double freq, double dur_ms, double beginPhase)
{
	int nSamplesNeeded = round(dur_ms/1000.*fs);
	double *p = Tone(freq, nSamplesNeeded, beginPhase);
	tfraction = dur_ms - nSamplesNeeded/(double)fs*1000.;
	return p;
}

EXP_CS double * CSignals::Truncate(double time_ms1, double time_ms2)
{ // Returns integer buffer pointer, to "extract" a signals object, use Take() member function
	int id1 = round(time_ms1/1000.*fs);
	int id2 = round(time_ms2/1000.*fs)-1;
	tfraction = (time_ms2-time_ms1) - (id2-id1+1)/(double)fs*1000.;
	return Truncate(id1, id2);
}

EXP_CS double * CSignals::Truncate(int id1, int id2)
{
	if (next!=NULL) next->Truncate(id1, id2);
	if (id1<0)			return buf;
	if (id1>id2) {
		nSamples = 0;
		return buf;
	}
	if (id2>nSamples-1) id2 = nSamples-1;

	nSamples = id2-id1+1;
	memmove(buf, buf+id1, nSamples*sizeof(*buf));
	return buf;
}

EXP_CS double * CSignals::Silence(double dur_ms)
{
	int nSamplesNeeded = round(dur_ms/1000.*fs); 
	double *p = Silence(nSamplesNeeded);
	tfraction = dur_ms - nSamplesNeeded/(double)fs*1000.;
	return p;
}

EXP_CS double * CSignals::Silence(int nsamples)
{
	Reset();
	UpdateBuffer(nsamples); //allocate memory if necessary
	if (nsamples < 0)
		return NULL;
	memset((void*)buf, 0, nsamples*sizeof(double));
	return buf;
}

EXP_CS double * CSignals::DC(double dur_ms)
{
	int nSamplesNeeded = round(dur_ms/1000.*fs); 
	double *p = DC(nSamplesNeeded);
	tfraction = dur_ms - nSamplesNeeded/(double)fs*1000.;
	return p;
}

EXP_CS double * CSignals::DC(int nsamples)
{
	Reset();
	UpdateBuffer(nsamples); //allocate memory if necessary
	for (int i=0; i<nsamples; i++) buf[i] = 1.;
	return buf;
}

EXP_CS CSignals CSignals::Abs()
{
	CSignals out(*this);
	for (int i=0; i<nSamples; i++) 
		if (buf[i]<0.) out.buf[i] *=-1.;
	return out;
}

EXP_CS void CSignals::Insert(CSignals& insertee, int id)
{
/* if *this is mono, insertee is mono, just do it 
               mono,             stereo, take first chan of insertee,
			   stereo,           mono, apply insertee equally to both channels
			   stereo,           stereo, just do it   */

	if (next!=NULL)			{
		if (insertee.next!=NULL) next->Insert(*insertee.next, id);
		else next->Insert(insertee, id);	}

	if (insertee.nSamples==0 || id<0) return;

	CSignals sig1, sig2;
	if (id==0)			sig1=insertee;
	else
	{
		Take(sig1, 0, id-1);
		sig1 += &insertee;
	}
	Take(sig2, id);
	sig1 += &sig2;
	*this = sig1;
}

EXP_CS double CSignals::Mean()
{
	double sum(0.);
	for (int i=0; i<nSamples; i++)		sum += buf[i];
	return sum/(double)nSamples;
}

EXP_CS double CSignals::Max(int &id)
{
	double localMax(-1.e-100);
	for (int i=0; i<nSamples; i++)
	{	if (buf[i] > localMax)
		{	localMax = buf[i];			id=i; }
	}
	return buf[id];
}

EXP_CS double CSignals::Max()
{
	int id;
	return Max(id);
}

EXP_CS double CSignals::Min(int &id)
{
	double localMin(1.e100);
	for (int i=0; i<nSamples; i++)
	{	if (buf[i] < localMin)
		{	localMin = buf[i];	id=i;		}
	}
	return buf[id];
}

EXP_CS double CSignals::Min()
{
	int id;
	return Min(id);
}

EXP_CS const CSignals& CSignals::Amplify(double dB)
{
	if (next!=NULL) next->Amplify(dB);
	return *this *= getexp(dB);
}

double *CSignals::MakeDoubleArray(double *DoubleArray)
{
	// DoubleArray must have been allocated properly with the proper (length nSamples)
	for(int i=0; i<nSamples; i++)
		DoubleArray[i] = ((double)buf[i]+.5)/MAX_24BIT;
	return DoubleArray;
}

void CSignals::SetBufferFromDoubleArray(double* scaledDoubleArray, int length)
{
	if (nSamples!=length) {Reset(); UpdateBuffer(length);}
	memcpy((void*)buf, (void*)scaledDoubleArray, length*sizeof(double));
}

#define FIRSIZE 225

EXP_CS double * CSignals::Resample(int newfs, char *errstr) // Revised in Dec09---noted for JHPARK
{
	MakeChainless();

	if (next!=NULL) next->Resample(newfs, errstr);

	SRC_DATA conv;
	vector<float> data_in, data_out;
	data_in.resize(nSamples);
	conv.data_in = &data_in[0];
	conv.src_ratio = (double)newfs/(double)fs;
	conv.input_frames = nSamples;
	conv.output_frames =  (long) (nSamples * (conv.src_ratio) + 2) ; // conv.src_ratio+1 should be right, but just in case 
	data_out.resize(conv.output_frames);
	conv.data_out = &data_out[0];
	for (int i=0; i<nSamples; i++) conv.data_in[i] = (float)buf[i];

	int res = src_simple(&conv, SRC_SINC_BEST_QUALITY, 1);
	if (!res)
	{
		//update nSamples and fs
		UpdateBuffer(conv.output_frames_gen);
		fs = newfs;
		for (int i=0; i<nSamples; i++) 
			buf[i] = conv.data_out[i];
	}
	else
	{
		strcpy(errstr, src_strerror(res));
		return 0;
	}
	return buf;
}

EXP_CS void CSignals::UpSample(int cc)
{
	MakeChainless();

	if (cc==1) return;
	if (next!=NULL) next->UpSample(cc);
	int newLength = nSamples * cc;
	CSignals temp(fs*cc);
	temp.UpdateBuffer(newLength);
	memset((void*)temp.buf, 0, sizeof(double)*newLength);
	for (int i=0; i<nSamples; i++)
		temp.buf[i*cc] = buf[i];
	*this = temp;
}

EXP_CS void CSignals::DownSample(int cc)
{
	MakeChainless();

	if (cc==1) return;
	if (next!=NULL) next->DownSample(cc);
	if (cc==1) return;
	int newLength = nSamples / cc;
	CSignals temp(fs/cc);
	temp.UpdateBuffer(newLength);
	for (int i=0; i<newLength; i++)
		temp.buf[i] = buf[cc*i];
	*this = temp;
}

EXP_CS int CSignals::Filter(const CSignals& num, const CSignals& den, char *errstr)
{
	int reqLeng = max(1, max(num.nSamples,den.nSamples) );
	Filter(reqLeng, num.buf, den.buf);
	return 1;
}

EXP_CS int CSignals::filtfilt(const CSignals& num, const CSignals& den, char *errstr)
{
	int reqLeng = max(1, max(num.nSamples,den.nSamples) );
	filtfilt(reqLeng, num.buf, den.buf);
	return 1;
}

EXP_CS void CSignals::Filter(int nTabs, double *num, double *den)
{
	if (next!=NULL) next->Filter(nTabs, num, den);
	double *out = new double[BufSize];
	if (den[0]!=0.)
		filter(nTabs, num, den, nSamples, buf, out);
	else
		filter(nTabs, num, nSamples, buf, out);
	if (buf) delete[] buf;
	buf = out;
}

EXP_CS void CSignals::Filter(int nTabs, double *num) // for FIR
{
	double den(0.);
	Filter(nTabs, num, &den);
}

EXP_CS void CSignals::filtfilt(int nTabs, double *num, double *den)
{
	MakeChainless();

	if (next!=NULL) next->filtfilt(nTabs, num, den);
	//Transient edges not handled, only zero-padded edges 
	CSignals temp(fs), temp2(fs), out(fs);
	int nfact = 3*(nTabs-1);
	temp.Silence(nfact);
	temp += this;
	temp2.Silence(nfact);
	temp += &temp2;
	temp.Filter(nTabs, num, den);
//	temp.ReverseTime();
//	temp.Filter(nTabs, num, den);
//	temp.ReverseTime();
	temp.Take(out, nfact, nfact+nSamples-1);
	*this = out;
}

EXP_CS void CSignals::filtfilt(int nTabs, double *num)
{
	double *den = new double[nTabs];
	memset((void*)den, 0, nTabs*sizeof(double));
	den[0] = 1.;
	filtfilt(nTabs, num, den);
	delete[] den;
}

EXP_CS void CSignals::ReverseTime()
{
	MakeChainless();

	if (next!=NULL) next->ReverseTime();
	CSignals temp(*this);
	double *tempBuf = temp.GetBuffer();
	for (int i=0; i<nSamples ; i++)
		tempBuf[nSamples-i-1]=buf[i];
	*this = temp;
}

void* CSignals::colonednums(double x1, double x2)
{
	return colonednums(x1, 1., x2);
}

void* CSignals::colonednums(double x1, double x2, double x3)
{
	CSignals out(-1); // double signal
	int nItems;
	nItems = max(1, (int)((x3-x1)/x2)+1);
	out.UpdateBuffer(nItems);
	for (int i=0; i<nItems; i++) out.buf[i] = x1 + (double)i*x2;
	*this = out;
	return buf;
}

#define RETURNNULL {	for (int i=0; i<nColons; i++) delete[] holder[i]; delete[] holder; return NULL;	}

int parse2array (const char *strIn, double **out, char *errstr)
{
	// out is allocated here and will be used for buf for a CSignals object in the calling function
	int nItems = countDeliminators(strIn, " ,\t\n\r");
	char **comb = new char*[nItems];
	for (int i=0; i<nItems; i++) comb[i] = new char[strlen(strIn)+1];
	if (str2strarray(comb, nItems, strIn, " ,\t\r\n")!=nItems)
	{ for (int i=0; i<nItems; i++) delete[] comb[i]; delete[] comb; if (errstr!=NULL) strcpy(errstr, "str2array malfunction in Str2Sig(<--parse2array)"); return 0; }
	*out = new double[nItems];
	for (int i=0; i<nItems; i++)
	{
		if (sscanf(comb[i], "%lf", *out+i)==EOF)
		{ delete[] out; if (errstr!=NULL) sprintf(errstr, "sscanf error with %s", comb[i]); for (int i=0; i<nItems; i++) delete[] comb[i]; delete[] comb; return 0; }
	}
	for (int i=0; i<nItems; i++) delete[] comb[i];
	delete[] comb; 
	return nItems;
}

EXP_CS double* CSignals::Str2Sig(const char * strIn, char *errstr)
{ 
	int estLength, minLength, nItems, nColons, nItems1, nItems2, nItems3, nItemsRest;
	char **holder;
	double nextBeginValue, *buffer, *buffer1, *buffer2, *buffer3;
	CSignals out(-1), temp(-1), x0(-1), x1(-1), x2(-1); 
	//first break down by colon
	nColons = countChar(strIn, ':', &estLength, &minLength);
	if (minLength==0) { if (errstr!=NULL) strcpy(errstr, "Two successive colons"); return NULL;}
	holder = new char*[nColons+1];
	for (int i=0; i<nColons; i++) holder[i] = new char[estLength+1];
	if (str2strarray(holder, nColons, strIn, ":")!=nColons) 
	{ if (errstr!=NULL) strcpy(errstr, "str2array malfunction in ReadArray"); RETURNNULL	}
	//breaking-down done
	//Now, we need to inspect whether this 2-colon or 3-colon.
	if (nColons==1) // no colon: string only has numbers with delimiters
	{
		if ((nItems = parse2array(holder[0], &buffer, errstr))==0) RETURNNULL 
		out.nSamples = nItems;
		out.buf = buffer; // direct manipulation of buf (discouraged in anywhere else)
	}
	else
		for (int i=0; i<nColons-1; i++) 
		{
			//Rule: If the string between two colons contains "anything" other than numeric and space (and period)
			//      only the first element after evaluation will be taken for the step (if the length==1 or 2) or 
			//      final value (if length>1)
			//	    But if it has only numbers separated by space, then parse accordingly.
			x0.Reset(1); x1.Reset(1); x2.Reset(1);
			if (i==0)
			{
				if ((nItems1 = parse2array(holder[i], &buffer1, errstr))==0) RETURNNULL
			}
			else
			{
				buffer1 = new double[nItems1 = 1];
				buffer1[0] = nextBeginValue;
			}
			if ((nItems2 = parse2array(holder[i+1], &buffer2, errstr))==0) RETURNNULL 

			x0.UpdateBuffer(nItems1-1);
			memcpy((void*)x0.buf, (void*)buffer1, sizeof(double)*(nItems1-1));
			if (nItems2==1 && i<nColons-2) // then do two-colon thing
			{
				if ((nItems3 = parse2array(holder[i+2], &buffer3, errstr))==0) RETURNNULL 
				x1.colonednums(buffer1[nItems1-1], *buffer2, buffer3[0]);
				nItemsRest = nItems3;
				buffer = buffer3;
				i++;
			}
			else
			{
				x1.colonednums(buffer1[nItems1-1], buffer2[0]);
				nItemsRest = nItems2;
				buffer = buffer2;
			}
			out += &x0;
			out += &x1;
			if (nItemsRest>2)
			{
				x2.UpdateBuffer(nItemsRest-2);
				memcpy((void*)x2.buf, (void*)&buffer[1], sizeof(double)*(nItemsRest-2));
				out += &x2;
			}
			if (nItemsRest>1)
				nextBeginValue = buffer[nItemsRest-1];
			else
			{
				if (i<nColons-1)
				{	sprintf(errstr, "invalid string %s", strIn); RETURNNULL	}
			}
			delete[] buffer1; delete[] buffer2; 
			if (nItems2==1 && i<nColons-2) delete[] buffer3; 
		}
	*this = out;
	return buf;
}


EXP_CS void CSignals::SetNextChan(CSignals *second)
{
	if (next) {
		delete next;
		next = NULL;
	}
	if (second) {
		next = new CSignals;
		*next = *second;
	}
}


EXP_CS int CSignals::IIR(int kind, int type, int order, double *freqs, double passRipple_dB, double stopFreqORAttenDB, char *errstr)
{// kind: 1 butterworth, 2 Chebyshev, 3 Elliptic
	// type: 1 lowpass, 2 bandpass, 3 highpass, 4 bandstop
	MakeChainless();

	errstr[0]=0;
	double *den = new double [2*order+1];
	double *num = new double [2*order+1];

	// To obtaine the filter coefficients, the object must be a scalar, the sample rate. 
	// Then on success of this call , this object has the buffer of a and b (den and num) in that order.
	if (IsScalar()) fs = (int)value();
	int res = design_iir(num, den, GetFs(), kind, type, order, freqs, passRipple_dB, stopFreqORAttenDB);
	if (res <= 0) {
		switch (res) {
		case -1:
			strcpy(errstr, "((kind <= 0) || (kind > 3))");
			break;
		case -2:
			strcpy(errstr, "((type <= 0) || (type > 4))");
			break;
		case -3:
			strcpy(errstr, "(n <= 0)");
			break;
		case -4:
			strcpy(errstr, "(fs <= 0)");
			break;
		case -5:
			strcpy(errstr, "Filter frequency is greater than Nyquist rate.");
			break;
		default:
			sprintf(errstr, "Unknown error, code=%d", res);
		}
	} else {
		res = 1;
		if (IsScalar()) {
			if (type & 1)
				UpdateBuffer(2*order+2);
			else
				UpdateBuffer(4*order+2);
			memcpy(buf, den, sizeof(*buf)*(nSamples/2));
			memcpy(buf+(nSamples/2), num, sizeof(*buf)*(nSamples/2));
		} else {
			if (type & 1)
				Filter(order+1, num, den);
			else
				Filter(2*order+1, num, den);
		}
	}
	delete[] den; delete[] num;
	return res;
}

std::string CSignals::string()
{
	int i;
	std::string out;
	out.resize(nSamples);
	for (i=0; i<nSamples && (char)buf[i]; i++)
		out[i] = (char)buf[i];
	out.resize(i);
	return out;
}

char *CSignals::getString(char *str, const int size)
{
	int i;
	for (i=0; i<nSamples && i<size-1; ++i)
		str[i] = (char)buf[i];
	str[i] = '\0';
	return str;
}


CSignals &CSignals::each(double (*fn)(double))
{
	for (CSignals *c=this; c; c=c->next)
		for (CSignals *p=c; p; p=p->next)
			for (int i=0; i<p->nSamples; ++i)
				p->buf[i] = fn(p->buf[i]);
	return *this;
}


CSignals &CSignals::transpose1()
{
	if (nSamples == 1)
		return *this;

	CSignals t(buf[0]);
	SwapContents1node(t);
	CSignals *p = this;
	for (int i=1; i<t.nSamples; ++i, p=p->next)
		p->next = new CSignals(t.buf[i]);
	return *this;
}


EXP_CS double *CSignals::Hamming(int len)
{
	if (len <= 0)
		throw "Length must be a positive integer.";
	Reset();
	UpdateBuffer(len);	// allocates memory if necessary
	for (int i=0; i<len; ++i)
		buf[i] = 0.54 - 0.46*cos(2.0*PI*i/(len-1.0));
	return buf;
}

EXP_CS double *CSignals::Blackman(int len, double alp)
{
	if (len <= 0)
		throw "Length must be a positive integer.";
	Reset();
	UpdateBuffer(len);	// allocates memory if necessary
	for (int i=0; i<len; ++i)
		buf[i] = (1-alp)/2 - 0.5*cos(2.0*PI*i/(len-1.0)) + alp/2*cos(4.0*PI*i/(len-1.0));
	return buf;
}

EXP_CS double * CSignals::FFT(int len)
{
	MakeChainless();
	len = min(nSamples,len);

	rfftw_plan plan = rfftw_create_plan(len, FFTW_FORWARD, FFTW_ESTIMATE | FFTW_OUT_OF_PLACE);
	double *out = new double[len];
	rfftw_one(plan, buf, out);
	rfftw_destroy_plan(plan);

	Reset();
	delete[] buf;
	buf = out;
	nSamples = len;
	BufSize = len;
	return buf;
}

EXP_CS double * CSignals::iFFT(void)
{
	int len = nSamples;
	rfftw_plan plan = rfftw_create_plan(len, FFTW_BACKWARD, FFTW_ESTIMATE | FFTW_OUT_OF_PLACE);
	double *out = new double[len];
	rfftw_one(plan, buf, out);
	rfftw_destroy_plan(plan);

	Reset();
	delete[] buf;
	buf = out;
	nSamples = len;
	BufSize = len;
	(*this) /= len;	// scale back down since the resulting array is scaled by len.
	return buf;
}


EXP_CS double * CSignals::cFFT(int len)
{
	MakeChainless();
	len = min(nSamples,len);

	fftw_complex *in = new fftw_complex[len];
	fftw_complex *out = new fftw_complex[len];
	memset(in, 0, len*sizeof(fftw_complex));

	fftw_plan plan = fftw_create_plan(len, FFTW_FORWARD, FFTW_ESTIMATE | FFTW_OUT_OF_PLACE);

	if (next && next->GetLength()>=len) {
		double *buf2 = next->buf;
		for (int i=0; i<len; ++i) {
			in[i].re = buf[i];
			in[i].im = buf2[i];
		}
	} else {
		for (int i=0; i<len; ++i)
			in[i].re = buf[i];
	}

	fftw_one(plan, in, out);
	fftw_destroy_plan(plan);

	if (next == NULL)
		next = new CSignals(fs);
	next->UpdateBuffer(len);
	double *buf2 = next->buf;
	for (int i=0; i<len; ++i) {
		buf[i] = out[i].re;
		buf2[i] = out[i].im;
	}

	delete[] in;
	delete[] out;

	return buf;
}

EXP_CS double * CSignals::icFFT(void)
{
	int len = nSamples;

	fftw_complex *in = new fftw_complex[len];
	fftw_complex *out = new fftw_complex[len];
	memset(in, 0, len*sizeof(fftw_complex));

	fftw_plan plan = fftw_create_plan(len, FFTW_BACKWARD, FFTW_ESTIMATE | FFTW_OUT_OF_PLACE);

	if (next && next->GetLength()>=len) {
		double *buf2 = next->buf;
		for (int i=0; i<len; ++i) {
			in[i].re = buf[i];
			in[i].im = buf2[i];
		}
	} else {
		for (int i=0; i<len; ++i)
			in[i].re = buf[i];
	}

	fftw_one(plan, in, out);
	fftw_destroy_plan(plan);

	if (next == NULL)
		next = new CSignals(fs);
	next->UpdateBuffer(len);
	double *buf2 = next->buf;
	for (int i=0; i<len; ++i) {
		buf[i] = out[i].re / len;	// scale back down since the resulting array is scaled by len.
		buf2[i] = out[i].im / len;
	}

	delete[] in;
	delete[] out;

	return buf;
}


EXP_CS double * CSignals::Hilbert(int len)
{
	if (len<1)
		return NULL;

	FFT(len);
	len = min(nSamples,len);

	fftw_complex *in = new fftw_complex[len];
	fftw_complex *out = new fftw_complex[len];
	memset(in, 0, len*sizeof(fftw_complex));

	fftw_plan plan = fftw_create_plan(len, FFTW_BACKWARD, FFTW_ESTIMATE | FFTW_OUT_OF_PLACE);

	// converting halfcomplex array to complex array
	in[0].re = buf[0];
	int half = len/2 + len%2;
	for (int i=1; i<half; ++i) {
		in[i].re = 2*buf[i];
		in[i].im = 2*buf[len-i];
	}
	if (len%2 == 0)	// len is even
		in[half].re = buf[half];
	// leave the rest zero

	// iFFT
	fftw_one(plan, in, out);
	fftw_destroy_plan(plan);

	for (int i=0; i<len; ++i)
		buf[i] = out[i].im / len;	// scale back down since the resulting array is scaled by len.

	delete[] in;
	delete[] out;

	return buf;
}