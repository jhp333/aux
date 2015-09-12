#pragma once
#include <list>
#include "eqsig.h"

#define LOG10DIV20 0.1151292546497

class calbase
{
public:
	CEqSig body;
	calbase(void) {n_names=0;}
	virtual ~calbase(void) {if (n_names>0) delete[] names;}
	string name;
	string *names;
	int n_names; // actual number of names minus one
	string GetName() {return name;}
	EXP_CS 	void getArguments(list<CEqSig *> arg, CEqSig *args, int &nArgs);
	virtual CEqSig& calc(list<CEqSig *> arg) {body.sig.Reset(body.sig.GetFs()); return body;}
	EXP_CS double getTimeMarks(CEqSig& eqIn, string funcname, int paramID, double *tmarks);
};

class CalcManager
{
	list<calbase *> flst;
	list<CEqSig *> arg;

public:
	CalcManager();
	virtual ~CalcManager(void)
	{
		list <calbase *>::iterator c1_Iter;
		for ( c1_Iter = flst.begin( ); c1_Iter != flst.end( ); c1_Iter++ )
		{
			calbase *p = *c1_Iter;
			delete p;
		}
	}

	int GetCount()			{	return (int)flst.size();}

	void Add(calbase* a)	{	flst.push_back(a);	}

	void AddArg(CEqSig *a)	{	arg.push_back(a);	}
	void ChangeLastArg(CEqSig *a)	{	arg.pop_back(); arg.push_back(a);	}
	void ClearArg()			{	arg.clear();	}

	CEqSig& Calc(const char* name, int fs=0)
	{
		list <calbase *>::iterator c1_Iter;
		calbase *p;
		char buf[256];
		size_t n = arg.size();
		
		for ( c1_Iter = flst.begin( ); c1_Iter != flst.end( ); c1_Iter++ )
		{
			p = *c1_Iter;
			if (p->n_names==0)
			{
				if (p->GetName()==name)
				{	
					p->body.sig.Reset(fs);
					p->calc(arg);
					return 		p->body;	}
			}
			else
			{
				for (int i=0; i<p->n_names; i++)
				{
					if (p->names[i]==name)
					{	
						p->body.sig.Reset(fs);
						p->name = name;
						p->calc(arg);
						return 		p->body;	}
				}
			}
		}
		sprintf(buf, "function %s not defined.", name);
		throw buf; // If you arrive on this line, it will crash.
	}
};

#define IN1OUT1(DERIVEDCLASSNAME, NAMEINC, NAMEINPSYCON) \
class DERIVEDCLASSNAME : public calbase	{public:	\
DERIVEDCLASSNAME(void) {name = #NAMEINPSYCON;} \
virtual ~DERIVEDCLASSNAME(void)	{} \
CEqSig& calc(list<CEqSig *> arg) \
{	char errStr[256]; body.sig.Reset(body.sig.GetFs()); \
	if (arg.size()!=1)	{ sprintf (errStr, "%s must have one argument.", name); throw errStr; }\
	else 	{ CEqSig  *inarg = arg.back(); if (inarg->len()<2) body.sig.val = NAMEINC(inarg->sig.value());	else { body.sig.UpdateBuffer(inarg->sig.GetLength()); for (int i=0; i<body.sig.GetLength(); i++) body.sig.buf[i] = NAMEINC(inarg->sig.buf[i]); }	} \
	return body; } };

#define IN1OUT1_EXPRESSION(DERIVEDCLASSNAME, NAMEINPSYCON, EXPRESSION) \
class DERIVEDCLASSNAME : public calbase	{public:	\
DERIVEDCLASSNAME(void) {name = #NAMEINPSYCON;} \
virtual ~DERIVEDCLASSNAME(void)	{} \
CEqSig& calc(list<CEqSig *> arg) \
{	char errStr[256]; \
	if (arg.size()!=1)	{ sprintf (errStr, "%s must have one argument.", name); throw errStr; }\
	else 	{ body.sig.Reset(0); CEqSig  *inarg = arg.back(); body.sig.val = EXPRESSION; }	\
	return body; } };

IN1OUT1(cal_sin,sin,sin)
IN1OUT1(cal_cos,cos,cos)
IN1OUT1(cal_tan,tan,tan)
IN1OUT1(cal_log,log,log)
IN1OUT1(cal_log10,log10,log10)
IN1OUT1(cal_sqrt,sqrt,sqrt)
IN1OUT1(cal_abs,fabs,abs)
IN1OUT1_EXPRESSION(cal_dur, dur, inarg->sig.GetDuration())
IN1OUT1_EXPRESSION(cal_length, length, inarg->sig.GetLength())
IN1OUT1_EXPRESSION(cal_rand, rand, (double)rand()/(double)RAND_MAX*inarg->sig.val)
IN1OUT1_EXPRESSION(cal_db, db, exp(inarg->sig.val * LOG10DIV20))

class cal_plus : public calbase
{ // + operation: argument 1 or less
	// 0 argument: return as is
	// 1 argument: add the argument to body.
public:
	cal_plus(void) {name = "+";}
	virtual ~cal_plus(void)	{}
	CEqSig& calc(list<CEqSig *> arg)
	{
		list <CEqSig *>::iterator iter;
		calbase::calc(arg); // to reset
		for ( iter = arg.begin( ); iter != arg.end( ); iter++ )
		{
			CEqSig *p = *iter;
			body.sig += p->sig;
		}
		return 	body;
	}
};

class cal_mult: public calbase
{
public:
	cal_mult(void) {name = "*";}
	virtual ~cal_mult(void)	{}
	CEqSig& calc(list<CEqSig *> arg)
	{
		list <CEqSig *>::iterator iter;
		body.sig.val=1.; // to reset
		for ( iter = arg.begin( ); iter != arg.end( ); iter++ )
		{
			CEqSig *p = *iter;
			body.sig *= p->sig;
		}
		return 	body;
	}
};

class cal_div: public calbase
{
public:
	cal_div(void) {name = "/";}
	virtual ~cal_div(void)	{}
	CEqSig& calc(list<CEqSig *> arg)
	{
		if (arg.size()!=2)	throw "MUST HAVE TWO ARGUMENTS."; 
		int nArgs(0); 
		CEqSig args[2];
		getArguments(arg, args, nArgs);
		body.sig = args[0].sig;
		body.sig /= args[1].sig;
		return 	body;
	}
};

class cal_mod: public calbase
{
public:
	cal_mod(void) {name = "mod";}
	virtual ~cal_mod(void)	{}
	CEqSig& calc(list<CEqSig *> arg)
	{
		body.sig.Reset(body.sig.GetFs()); 
		list <CEqSig *>::iterator arg1;
		list <CEqSig *>::iterator arg2;
		if (arg.size()!=2)	throw "MUST HAVE TWO ARGUMENTS."; 
		arg1 = arg.begin();
		arg2 = arg1++;

		CEqSig *p1(*arg2), *p2(*arg1) ;
		double x(p1->sig.val), y(p2->sig.val);
		body.sig.val = x - (double)((int)(x/y) * y) ;
		return 	body;
	}
};

class cal_raiseby: public calbase
{
public:
	cal_raiseby(void) {name = "^";}
	virtual ~cal_raiseby(void)	{}
	CEqSig& calc(list<CEqSig *> arg)
	{
		body.sig.Reset(body.sig.GetFs()); 
		list <CEqSig *>::iterator arg1;
		list <CEqSig *>::iterator arg2;
		if (arg.size()!=2)	throw "MUST HAVE TWO ARGUMENTS."; 
		arg1 = arg.begin();
		arg2 = arg1++;
		CEqSig *p1(*arg2), *p2(*arg1) ;
		double x(p1->sig.val), y(p2->sig.val);
		body.sig.val = exp(y*log(x));
		return 	body;
	}
};

class cal_tone: public calbase
{
public:
	cal_tone(void) {n_names=2; 	names = new string[2]; 	names[0] = "tone"; names[1] = "tone2";}
	virtual ~cal_tone(void)	{}
	CEqSig& calc(list<CEqSig *> arg)
	{
		char errStr[256];
		CSignals xout(body.sig.GetFs()); 
		body.sig.Reset(body.sig.GetFs()); 
		int nArgs(0);
		list <CEqSig *>::iterator iter;
		CEqSig args[3], tempEq, tempEq2;
 		for ( iter = arg.begin( ); iter != arg.end( ); iter++ )
		{
			CEqSig *p = *iter;
			args[nArgs++] = (CEqSig&)*p;
		}
		if ( (name=="tone" && nArgs!=2) || (name=="tone2" && nArgs!=3) ) 
		{
			strcpy(errStr, "tone(freq, duration or [begin duration]) or\n");
			strcat(errStr, "tone2(freq, duration or [begin duration], initial_phase_between_0_and_1)\n");
			throw errStr;
		}
		if (args[0].sig.GetLength()>1) throw "FREQUENCY MUST BE SCALAR."; 
		double initphase(0.), tmark[2], freq(args[0].sig.val);
		getTimeMarks(args[1], name, 2, tmark);
		if (tmark[1]<0.) throw "invalid time marker(negative duration)";
		if (nArgs==3) initphase = args[2].sig.val;
		xout.Silence(tmark[0]); 
		body.sig.Tone(freq, tmark[1], initphase);
		body.sig.Insert(xout,0);
		return 	body;
	}
};

class cal_fm: public calbase
{
public:
	cal_fm(void) {name = "fm";}
	virtual ~cal_fm(void)	{}
	CEqSig& calc(list<CEqSig *> arg);
};

class cal_am: public calbase
{
public:
	cal_am(void) {name = "am";}
	virtual ~cal_am(void)	{}
	CEqSig& calc(list<CEqSig *> arg);
};

class cal_wave: public calbase
{
public:
	cal_wave(void) {name = "wave";}
	virtual ~cal_wave(void)	{}
	CEqSig& calc(list<CEqSig *> arg);
};

class cal_ramp: public calbase
{
public:
	cal_ramp(void) {name = "ramp";}
	virtual ~cal_ramp(void)	{}
	CEqSig& calc(list<CEqSig *> arg)
	{
		if (arg.size()!=2)	throw "ramp MUST HAVE TWO ARGUMENTS."; 
		int nArgs(0); 
		CEqSig args[2];
		getArguments(arg, args, nArgs);
		body.sig.Reset(body.sig.GetFs()); 
		if (args[1].sig.GetLength()>1) throw "SECOND PARAMATER MUST BE SCALAR (ramping time in ms)."; 
		if (args[0].sig.GetLength()<=1) throw "FIRST PARAMATER MUST BE A SIGNAL TO RAMP."; 
		body.sig = args[0].sig;
		body.sig.Dramp(args[1].sig.val, args[0].GetNonZeroPt());
		return 	body;
	}
};

class cal_iir: public calbase
{
public:
	cal_iir(void) {n_names=4; 	names = new string[4]; 	names[0] = "lpf";	names[1] = "bpf";	names[2] = "hpf";	names[3] = "bsf";}
	virtual ~cal_iir(void)	{}
	CEqSig& calc(list<CEqSig *> arg);
};

class cal_rms: public calbase
{
public:
	cal_rms(void) {name = "rms";}
	virtual ~cal_rms(void)	{}
	CEqSig& calc(list<CEqSig *> arg);
};

class cal_tparamonly : public calbase
{
public:
	cal_tparamonly(void) {n_names=4; 	names = new string[n_names]; 	names[0] = "noise";	names[1] = "gnoise";	names[2] = "silence"; names[3] = "step"; }
	virtual ~cal_tparamonly(void)	{}
	CEqSig& calc(list<CEqSig *> arg);
};

class cal_irand: public cal_rand
{
public:
	cal_irand(void) {name = "irand";}
	virtual ~cal_irand(void)	{}
	virtual CEqSig& calc(list<CEqSig *> arg);
};

class cal_randperm: public cal_irand
{
public:
	cal_randperm(void) {name = "randperm";}
	virtual ~cal_randperm(void)	{}
	CEqSig& calc(list<CEqSig *> arg);
};


class cal_readtxt: public calbase
{
public:
	cal_readtxt(void) {name = "file";}
	virtual ~cal_readtxt(void)	{}
	CEqSig& calc(list<CEqSig *> arg);
};

class cal_filt: public calbase
{
public:
	cal_filt(void) {name = "filt";}
	virtual ~cal_filt(void)	{}
	CEqSig& calc(list<CEqSig *> arg);
};

class cal_cisim : public calbase
{
public:
	cal_cisim(void) {name = "cisim";}
	virtual ~cal_cisim(void)	{}
	CEqSig& calc(list<CEqSig *> arg);
};

class cal_round : public calbase
{
public:
	cal_round(void) {name = "round";}
	virtual ~cal_round(void)	{}
	CEqSig& calc(list<CEqSig *> arg)
	{
		int nArgs(0); 
		CEqSig args[8];
		getArguments(arg, args, nArgs);
		body.sig.Reset(body.sig.GetFs()); 
		if (nArgs!=1)	throw "round MUST HAVE ONE ARGUMENT."; 
		if (args[0].sig.nSamples>0)
		{
			body.sig.UpdateBuffer(args[0].sig.nSamples); 
			for (int i=0; i<body.sig.GetLength(); i++) 
				body.sig.buf[i] = (double)(int)(args[0].sig.buf[i]+.5);
		}
		else
			body.sig.val = (double)(int)(args[0].sig.val+.5);
		return 	body;
	}
};
