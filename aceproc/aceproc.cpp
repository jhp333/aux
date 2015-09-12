// aceproc.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "mclmcrrt.h" // for matlab 2008a and later
//#include "mclmcr.h" // for matlab 2007b and earlier
#include <audfret.h>
#include "aceproc.h"
#include "audstr.h"
#include <math.h>

int FFT_band_bins(int *width, int nBands);

#define LEN_ERR_STRING	4096*4096
#define MAX_NUM_FIELDS 13

#ifdef _MANAGED
#pragma managed(push, off)
#endif

static int libInitialized=0;
static int err, stout;
static char errStrMatlab[LEN_ERR_STRING];
static char stdOutStrMatlab[LEN_ERR_STRING];
const char *fieldNames[MAX_NUM_FIELDS];
static char mxaceLibName[MAX_PATH];
char buf[512];
typedef bool (* FPINIT) (mclOutputHandlerFcn, mclOutputHandlerFcn);
typedef void (* FPEND) ();
typedef void (* FPACE) (int, mxArray**, mxArray**, mxArray*, mxArray*, mxArray*);
typedef void (* FPFREQ) (int, mxArray**, mxArray**, mxArray**, mxArray*, mxArray*);
typedef bool (*FPWAVLET)(int nargout, mxArray** out, mxArray** p, mxArray* in, mxArray* p_in1);
typedef void (* FPP) (int, mxArray**, mxArray*, mxArray*);
typedef void (* FPCIC4) (int, mxArray**, mxArray*);

HMODULE hLib;
FPFREQ fp_freq;
FPACE fp_ace;
FPWAVLET mlfWavletace;
FPP mlfShow_p;
FPCIC4 mlfTurn2cic4;

HINSTANCE hInst;

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, void *pv)
{
	hInst = hInstance;
	return 1;
}

static int Handler1(const char*s)
{
	err = 1;
	strcat (errStrMatlab, s);
	return (int)strlen(errStrMatlab);
}

static int Handler2(const char *s)
{
	stout = 1;
	strcat (stdOutStrMatlab, s);
	strcat (stdOutStrMatlab, "\n");
	return (int) strlen(stdOutStrMatlab);
}

int mxArray2ACESEQ(ACESEQ *seq, mxArray *X)
{
	int i, total_num_of_elements, number_of_fields, indexFilled;
	const char  *field_name;

	indexFilled=0;
	total_num_of_elements = (int)mxGetNumberOfElements(X);
	if (total_num_of_elements==0)
		return -1; // no element in X
	number_of_fields = mxGetNumberOfFields(X);
	for (i=0; i<number_of_fields; i++)
	{
		field_name = mxGetFieldNameByNumber(X, i);
		if (!strcmp(field_name, "electrodes"))
		{
			seq->ae = mxGetPr(mxGetFieldByNumber(X, 0, i));
			indexFilled++;
		}
		else if (!strcmp(field_name, "channels"))
		{
			seq->ae = mxGetPr(mxGetFieldByNumber(X, 0, i));
			indexFilled++;
		}
		else if (!strcmp(field_name, "current_levels"))
		{
			seq->cl = mxGetPr(mxGetFieldByNumber(X, 0, i));
			indexFilled++;
			seq->length = (int)mxGetM(mxGetFieldByNumber(X, 0, i));
		}
		else if (!strcmp(field_name, "modes"))
		{
			seq->mode = mxGetPr(mxGetFieldByNumber(X, 0, i));
			indexFilled++;
			seq->lenMode = (int)mxGetM(mxGetFieldByNumber(X, 0, i));
		}
		else if (!strcmp(field_name, "phase_widths"))
		{
			seq->pw = mxGetPr(mxGetFieldByNumber(X, 0, i));
			indexFilled++;
			seq->lenPW = (int)mxGetM(mxGetFieldByNumber(X, 0, i));
		}
		else if (!strcmp(field_name, "phase_gaps"))
		{
			seq->pg = mxGetPr(mxGetFieldByNumber(X, 0, i));
			indexFilled++;
			seq->lenPG = (int)mxGetM(mxGetFieldByNumber(X, 0, i));
		}
		else if (!strcmp(field_name, "periods"))
		{
			seq->period = mxGetPr(mxGetFieldByNumber(X, 0, i));
			indexFilled++;
			seq->lenPeriod = (int)mxGetM(mxGetFieldByNumber(X, 0, i));
		}
	}
	if (indexFilled>=5)
		return seq->length;
	else
		return -2;
}

int mxp2_aceparam (CParamACE * param, const mxArray * xP)
{ //                             out              in
	// Currently only the gate field is retrieved...no other field is necessary at this point (Mar 2011)
	// Presuming that memory for param has been allocated prior to this call.
	mxArray *xGateField = mxGetField (xP, 0, "gate");
	if (xGateField!=NULL)
	{
		mxArray *gateRateField = mxGetField (xGateField, 0, "rate");
		if (gateRateField!=NULL)
		{
			double * newGateRate = mxGetPr(gateRateField);
			param->gaterate = * newGateRate;
			return 1;
		}
	}
	return 0;
}


int aceparam_c2mx (CParamACE * param, mxArray ** xP)
{
	mxArray *xT, *xC, *xPW, *xGainsdB, *xElectrodes, *xP2;
	/* P must be cleared in the calling function after it is done.*/
	int mode4p, i(0), nFields(MAX_NUM_FIELDS), nBands(param->numBands);
	mwSize dim[]={1,1,};
	char dummy[64];

	if (param->ptParam2==NULL)		nFields--;

	double *elec = new double[nBands]; 
	double *t = new double[nBands]; 
	double *c = new double[nBands]; 
	double *gain = new double[nBands]; 
	double *pw = new double[nBands]; 
	for (int k(0), m=0; m<22; m++)
		if (param->selected[m])
		{
			elec[k] = 22-m;
			t[k] = param->t[m];
			c[k] = param->c[m];
			gain[k] = param->gains[m];
			pw[k++] = param->pw[m];
		}

	mode4p = param->mode2enum (dummy);

	xElectrodes = mxCreateDoubleMatrix (nBands,1,mxREAL);	memcpy(mxGetPr(xElectrodes),(void*)elec, nBands*sizeof(double));
	xC = mxCreateDoubleMatrix (nBands,1,mxREAL);	memcpy(mxGetPr(xC),(void*)c, nBands*sizeof(double));
	xT = mxCreateDoubleMatrix (nBands,1,mxREAL);	memcpy(mxGetPr(xT),(void*)t, nBands*sizeof(double));
	xPW = mxCreateDoubleMatrix (nBands,1,mxREAL);	memcpy(mxGetPr(xPW),(void*)pw, nBands*sizeof(double));
	xGainsdB = mxCreateDoubleMatrix (nBands,1,mxREAL);	memcpy(mxGetPr(xGainsdB),(void*)gain, nBands*sizeof(double));

	delete[] elec;
	delete[] t;
	delete[] c;
	delete[] gain;
	delete[] pw;

	*xP = mxCreateStructArray (2, dim, nFields, fieldNames);

	mxSetField (*xP, 0, fieldNames[i++], mxCreateDoubleScalar (param->rate));
	mxSetField (*xP, 0, fieldNames[i++], xElectrodes);
	mxSetField (*xP, 0, fieldNames[i++], mxCreateDoubleScalar (param->numMax));
	mxSetField (*xP, 0, fieldNames[i++], mxCreateDoubleScalar (param->level));
	mxSetField (*xP, 0, fieldNames[i++], mxCreateDoubleScalar (param->Q));
	mxSetField (*xP, 0, fieldNames[i++], xPW);
	mxSetField (*xP, 0, fieldNames[i++], mxCreateDoubleScalar (param->pg));
	mxSetField (*xP, 0, fieldNames[i++], mxCreateDoubleScalar (mode4p));
	mxSetField (*xP, 0, fieldNames[i++], xT);
	mxSetField (*xP, 0, fieldNames[i++], xC);
	mxSetField (*xP, 0, fieldNames[i++], xGainsdB);
	mxSetField (*xP, 0, fieldNames[i++], mxCreateString(param->addmlines));

	if (param->ptParam2!=NULL) 
	{
		aceparam_c2mx ((CParamACE*)(param->ptParam2), &xP2);
		mxSetField (*xP, 0, fieldNames[i], xP2);
		return 2;
	}
	return 1;
}


ACEPROC_EXP int GateProcess(ACESEQ * seqOut, const char *procStr, const char *wavname, const char *seqFilename, CParamACE *param, char **errstr, char **warnstr)
{
	// procStr is either "NICstream" "NIC2" or "WACE"
	static mxArray *xY, *xPout, *xDummy;
	mxArray *xP;
	
	if (!libInitialized)
	{
		*errstr = "GateProcess called before mxace initialized.";
		*warnstr = "";
		return 0;
	}
	err = 0;	stout = 0; 
	strcpy(errStrMatlab,""); strcpy(stdOutStrMatlab,"");
	int sum(0);
	for (int i=0; i<22; i++) sum += param->selected[i];
	if (param->numBands != sum)
	{
		*errstr = "Coding error (you shouldn't see this). param->numBands not equal to number of selected channels.";
		*warnstr = "";
		return 0;
	}
	int nMaps = aceparam_c2mx (param, &xP);
	mxDestroyArray(xY);
	mxDestroyArray(xPout);
	xY=NULL; xPout = NULL;
	if (param->pg<7.8) 		mlfTurn2cic4(1, &xP, xP);

	if (err)
	{
		*errstr = errStrMatlab;
	}
	else
	{
		//mlfShow_p (1, &xDummy, xP, mxCreateString("outs1.txt"));
		if (!stricmp(procStr,"NICstream") || (!stricmp(procStr,"NIC2")))
			fp_ace (2, &xY, &xPout, mxCreateString(wavname), xP, mxCreateString(seqFilename));
		else if (!stricmp(procStr,"wACE"))
			mlfWavletace (2, &xY, &xPout, mxCreateString(wavname), xP);
		else
		{
			err = 1;
			sprintf(errStrMatlab, "Unknown command: %s", procStr);
		}
		mxDestroyArray(xP); 
		*errstr = errStrMatlab;
		if (err==0)
		{
			//mlfShow_p (1, &xDummy, xPout, mxCreateString("outs2.txt"));
			mxp2_aceparam(param, xPout);
			mxArray2ACESEQ(seqOut, xY);
		}
	}
	*warnstr = stdOutStrMatlab;
	return (err==0);
}

ACEPROC_EXP int InitMatlabLib(const char* libname, char **errstr)
{
	// Returns 1 for success, 0 for mclInitializeApplication failure, -1 for mxaceproc8InitializeWithHandlers failure
	*errstr = "";
	if ( (hLib = LoadLibrary(libname))==NULL)
	{
		sprintf(buf, "%s.dll not found.", libname);
		*errstr = buf;
		return 0;
	}
	strcpy(mxaceLibName, libname);
	FPINIT fp_mxaceinit(NULL);
	char name[256], ext[256];
	_splitpath(libname, NULL, NULL, name, ext);
	sprintf(buf, "%sInitializeWithHandlers", name);

	if ((fp_mxaceinit = (FPINIT)GetProcAddress((HMODULE)hLib, buf))==NULL)
	{
		*errstr = "axace__InitializeWithHandlers not found."; return 0;
	}
	if (!libInitialized)
	{
		int res;
		strcpy(buf, "mlfFreqwidth");
		if ((fp_freq = (FPFREQ)GetProcAddress((HMODULE)hLib, buf))==NULL) 
		{
			*errstr = "mlfFreqwidth not found."; return 0;
		}
		if (!(res=mclInitializeApplication(NULL,0))) 
		{
			*errstr = "mclInitializeApplication Failed"; return 0;
		}
		strcpy(buf, "mlfWav2seq");
		if ((fp_ace = (FPACE)GetProcAddress((HMODULE)hLib, buf))==NULL)
		{
			*errstr = "mlfWav2seq not found."; return 0;
		}
		strcpy(buf, "mlfWavletace");
		if ((mlfWavletace = (FPWAVLET)GetProcAddress((HMODULE)hLib, buf))==NULL)
		{
			*errstr = "mlfWavletace not found."; return 0;
		}
		strcpy(buf, "mlfShow_p");
		if ((mlfShow_p = (FPP)GetProcAddress((HMODULE)hLib, buf))==NULL)
		{
			*errstr = "mlfShow_p not found."; return 0;
		}
		strcpy(buf, "mlfTurn2cic4");
		if ((mlfTurn2cic4 = (FPCIC4)GetProcAddress((HMODULE)hLib, buf))==NULL)
		{
			*errstr = "mlfTurn2cic4 not found."; return 0;
		}
		if (fp_mxaceinit(Handler1, Handler2))
			libInitialized=1;
		else
		{
			*errstr = "InitializeWithHandlers failed";
			return -1;
		}
	}
	fieldNames[0]="channel_stim_rate";
	fieldNames[1]="electrodes";
	fieldNames[2]="num_selected";
	fieldNames[3]="reference_SPL_dB";
	fieldNames[4]="Q";
	fieldNames[5]="phase_width";
	fieldNames[6]="phase_gap";
	fieldNames[7]="modes"; 
	fieldNames[8]="threshold_levels";
	fieldNames[9]="comfort_levels";
	fieldNames[10]="gains_dB";
	fieldNames[11]="addfieldstr";
	fieldNames[12]="p";

	return 1;
}

ACEPROC_EXP void CloseMatlabLib()
{
	FPEND fp(NULL);
	sprintf(buf, "%sTerminate", mxaceLibName);
	if ((fp = (FPEND)GetProcAddress((HMODULE)hLib, buf))!=NULL)		fp();
	FreeLibrary((HMODULE)hLib);
}

ACEPROC_EXP int GetCutoffFreqs(int nBands, double *freq_limits, double *freqs)
{ // in in out
	// freqs must be prepared by the caller (length : nBands + 1)

	int freqWidth[22];
	double *res, unitFreq(188.), unitWidth(125.);

	mxArray *xFreq_limits;
	static mxArray *xBins, *xCumBins, *xFreqBound;

	if (nBands<=0 || nBands>22) return 0;

	if (freq_limits[0]==188. && freq_limits[1]== 7938.)
	{
		FFT_band_bins(freqWidth, nBands);
		freqs[0] = unitFreq;
		for (int i=1; i<=nBands; i++)  freqs[i] = freqs[i-1] + (double)freqWidth[i-1] * unitWidth;
	}
	else
	{
		xFreq_limits = mxCreateDoubleMatrix (1,2, mxREAL);	memcpy(mxGetPr(xFreq_limits),(void*)freq_limits, 2*sizeof(double));
		// do some error check here!!!
		mxDestroyArray(xBins); xBins=NULL;
		mxDestroyArray(xCumBins); xCumBins = NULL;
		mxDestroyArray(xFreqBound); xFreqBound = NULL;
		fp_freq (3, &xBins, &xCumBins, &xFreqBound, mxCreateDoubleScalar(nBands), xFreq_limits);
		mxDestroyArray (xFreq_limits);
		res = mxGetPr(xFreqBound);
		memcpy((void*)freqs, (void*)res, (nBands+1)*sizeof(double));
	}
	return 1;
}


#ifdef _MANAGED
#pragma managed(pop)
#endif

