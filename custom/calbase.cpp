#include "stdafx.h"
#include <math.h>
#include <windows.h> // for file HANDLE

using namespace std;

#include "calbase.h"
#include "supportFunctions.h"

CalcManager::CalcManager()
{
	Add(new cal_sin());
	Add(new cal_cos());
	Add(new cal_tan());
	Add(new cal_log());
	Add(new cal_log10());
	Add(new cal_sqrt());
	Add(new cal_abs());
	Add(new cal_plus());
	Add(new cal_mult());
	Add(new cal_div());
	Add(new cal_mod());
	Add(new cal_raiseby());
	Add(new cal_tone());
	Add(new cal_ramp());
	Add(new cal_tparamonly());
	Add(new cal_iir());
	Add(new cal_am());
	Add(new cal_fm());
	Add(new cal_db());
	Add(new cal_wave());
	Add(new cal_rand());
	Add(new cal_irand());
	Add(new cal_randperm());
	Add(new cal_readtxt());
	Add(new cal_filt());
	Add(new cal_length());
	Add(new cal_dur());
	Add(new cal_rms());
	Add(new cal_round());
	Add(new cal_cisim());
}

void calbase::getArguments(list<CEqSig *> arg, CEqSig *args, int &nArgs)
{
	list <CEqSig *>::iterator iter;
	for ( iter = arg.begin( ); iter != arg.end( ); iter++ )
	{
		CEqSig *p = *iter;
		args[nArgs++] = (CEqSig &)*p;
	}
}

double calbase::getTimeMarks(CEqSig& eqIn, string funcname, int paramID, double *tmarks)
{
	// Input: eqIn, funcname, paramID
	// Output: tmarks
	// return value: duration
	string str2throw;
	double dur;
	char szbuffer[256];
	tmarks[0]=0.;
	switch(eqIn.sig.nSamples)
	{
	case 0:
		dur = tmarks[1]=eqIn.sig.val;
		break;
	case 1:
		dur = tmarks[1]=eqIn.sig.buf[0];
		break;
	case 2:
		memcpy((void*)tmarks, (void*)eqIn.sig.buf, 2*sizeof(double));
		break;
	default:
		sprintf(szbuffer,"Argument %d (Time parameter) in %s must be a scalar or 2-element array.", paramID, funcname.c_str());
		throw szbuffer;
	}
	return tmarks[1];
}

CEqSig& cal_iir::calc(list<CEqSig *> arg)
{
	int type, kind, nMinArg, norder;
	double freqs[2], rippledB, stopbandFreqORAttenDB;
	char emsg[260], buf[128], errstr[256];
	string emsg1(" (signal, freq, [order=8], [kind=1], [dB_passband_ripple=0.5], [dB_stopband_atten=-40])");
	string emsg2(" (signal, freq1, freq2, [order=8], [kind=1], [dB_passband_ripple=0.5], [dB_stopband_atten=-40])");
	string emsg3("kind: 1 for Butterworth, 2 for Chebyshev, 3 for Elliptic");

	if  (name=="lpf") {	type=1; nMinArg = 2; sprintf(emsg, "%s%s\n%s", name.c_str(), emsg1.c_str(), emsg3.c_str()); }
	else if  (name=="bpf") { type=2; nMinArg = 3; sprintf(emsg, "%s%s\n%s", name.c_str(), emsg2.c_str(), emsg3.c_str()); }
	else if  (name=="hpf") { type=3; nMinArg = 2; sprintf(emsg, "%s%s\n%s", name.c_str(), emsg1.c_str(), emsg3.c_str()); }
	else				   { type=4; nMinArg = 3; sprintf(emsg, "%s%s", name.c_str(), emsg2); }

	body.sig.Reset(body.sig.GetFs()); 
	list <CEqSig *>::iterator iter;
	CEqSig args[7];
	int nArgs(0), nTotalArgs(nMinArg+4);
	if (arg.size()>7) throw "Too many arguments.";
	for ( iter = arg.begin( ); iter != arg.end( ); iter++ )
	{
		CEqSig *p = *iter;
		args[nArgs++] = (CEqSig &)*p;
	}
	if (nArgs<nMinArg)	
	{
		sprintf(buf, "Insufficient arguments. %s%s", name.c_str(), emsg);
		throw buf;
	}
	if (args[0].sig.GetLength()<=1) 
	{
		sprintf(buf, "First argument is the signal to filter. %s%s", name.c_str(), emsg);
		throw buf;
	}
	for (int i=1; i<nArgs; i++)
	{
		if (args[i].sig.GetLength()>1) 
		{
			sprintf(buf, "Error in parameter %d. %s%s", i, name.c_str(), emsg);
			throw buf;
		}
	}

	freqs[0] = args[1].sig.val;
	if (nMinArg==3) freqs[1] = args[2].sig.val;

	stopbandFreqORAttenDB = (nArgs>=nTotalArgs) ? args[nTotalArgs-1].sig.val : -40;
	rippledB = (nArgs>=nTotalArgs-1) ? args[nTotalArgs-2].sig.val : .5;
	kind =  (nArgs>=nTotalArgs-2) ? (int)args[nTotalArgs-3].sig.val : 1;
	norder = (nArgs>=nTotalArgs-3) ? (int)args[nTotalArgs-4].sig.val : 8;
	if (kind>3)
	{
		sprintf(buf, "Invalid kind\n%s", emsg);
		throw buf;
	}

	body.sig = args[0].sig;
	if (body.sig.IIR(kind, type, norder, freqs, rippledB, stopbandFreqORAttenDB, errstr)<0)
	{
		sprintf(buf, "Unknown error in source %s. %s", name, errstr);
		throw buf;
	}
	return 	body;
}

CEqSig& cal_tparamonly::calc(list<CEqSig *> arg)
{
	char buf[256];
	if (arg.size()!=1)
	{
		sprintf(buf, "%s requires one argument (time parameter).", name.c_str());
		throw buf;
	}
	body.sig.Reset(body.sig.GetFs()); 
	list <CEqSig *>::iterator args;
	args = arg.begin();
	CSignals xout(body.sig.GetFs());
	CEqSig *p1(*args);
	double tmark[2], dur;
	dur = getTimeMarks(*p1, name, 1, tmark);
	xout.Silence(tmark[0]); 
	if (name=="noise")
		body.sig.Noise(dur);
	else if (name=="gnoise")
		body.sig.Noise2(dur);
	else if (name=="silence")// name must be "silence"
		body.sig.Silence(dur);
	else // name must be "step"
		body.sig.DC(dur);
	body.sig.Insert(xout,0);
	return 	body;

}

CEqSig& cal_am::calc(list<CEqSig *> arg)
{
	char buf[256];
	body.sig.Reset(body.sig.GetFs()); 
	list <CEqSig *>::iterator iter;
	CEqSig args[4];
	if (arg.size()>4) throw "Too many arguments.";
	int nArgs(0), nTotalArgs(4);
	double amDepth(1.), initPhase(0.); 

	for ( iter = arg.begin( ); iter != arg.end( ); iter++ )
	{
		CEqSig *p = *iter;
		args[nArgs++] = (CEqSig &)*p;
	}

	if (arg.size()<2)
	{
		sprintf(buf, "%s requires at least 2 arguments.\n", name.c_str());
		strcat(buf, "am(signal,rate,[depth=1.0],[Init Phase=0.])");
		throw buf;
	}
	body.sig = args[0].sig; 
	for (int i=1; i<nArgs; i++)
	{
		if (args[i].sig.GetLength()>1) 
		{
			sprintf(buf, "Parameter %d must be scalar. %s", i, name);
			throw buf;
		}
	}
	
	if (nArgs==4) initPhase = args[3].sig.val;
	else if (nArgs==3) amDepth = args[2].sig.val;
	double modRate = args[1].sig.val;

	int nsamples = body.sig.GetLength();
	double fs=(double)body.sig.GetFs();
	int nzPoint = body.GetNonZeroPt();
	double *env = new double[nsamples];
	for (int i=nzPoint; i<nsamples; i++) 
		env[i] = (1.+amDepth*sin(2*PI*(modRate*(double)(i-nzPoint)/fs+initPhase-.25)))/(1.+amDepth);
	body.sig.Modulate(env,nsamples);
	delete[] env;
	return 	body;
}


CEqSig& cal_fm::calc(list<CEqSig *> arg)
{
	char buf[256];
	list <CEqSig *>::iterator iter;
	CEqSig args[5];
	if (arg.size()>5) throw "Too many arguments.";
	int nArgs(0), nTotalArgs(5);
	CSignals xout;
	double dur, initPhase(0.), tmark[2]; 

	for ( iter = arg.begin( ); iter != arg.end( ); iter++ )
	{
		CEqSig *p = *iter;
		args[nArgs++] = (CEqSig &)*p;
	}

	if (arg.size()<4)
	{
		sprintf(buf, "%s requires at least 4 arguments.\n", name.c_str());
		strcat(buf, "fm (freq1, freq2, mod_rate, time, [init_phase=0])");
		throw buf;
	}
	body.sig.Reset(body.sig.GetFs()); 

	if (args[0].sig.GetLength()>1 || args[1].sig.GetLength()>1)
	{
		sprintf(buf, "FREQUENCY MUST BE SCALAR.\n");
		strcat(buf, "fm (freq1, freq2, mod_rate, time, [init_phase=0])");
		throw buf;
	}

	double midFreq = (args[0].sig.val+args[1].sig.val)/2.;
	double width = fabs(args[1].sig.val-args[0].sig.val)/2.;
	double modRate = args[2].sig.val;
	if (nArgs==5) initPhase = args[4].sig.val;
	dur = getTimeMarks(args[3], name, 3, tmark);
	xout.Silence(tmark[0]); 
	if (modRate!=0.)
		body.sig.fm(midFreq, width, modRate, dur, initPhase-.25);
	else
	{
		double freqs[2];
		for (int i=0; i<2; i++) freqs[i] = args[i].sig.val;
		body.sig.Tone(freqs, dur);
	}
	body.sig.Insert(xout,0);
	return 	body;
}

CEqSig& cal_wave::calc(list<CEqSig *> arg)
{
	list <CEqSig *>::iterator iter;
	CSignals xout;
	char quot('"'), errStr[256];
	string fnamewithpath;
	CEqSig args[2];
	if (arg.size()>2) throw "Too many arguments.";

	double dur, tmark[2]; 
	tmark[0]=0.;

	int nArgs(0);
	for ( iter = arg.begin( ); iter != arg.end( ); iter++ )
	{
		CEqSig *p = *iter;
		args[nArgs++] = (CEqSig &)*p;
	}
	if (arg.size()>1)	
	{
		dur = getTimeMarks(args[1], name, 1, tmark);
		xout.Silence(tmark[0]); 
	}
	args[0].str = args[0].str.substr(1, string::npos);
	args[0].str = args[0].str.substr(0, args[0].str.length()-1);
	args[0].FileWithPath(args[0].str, fnamewithpath, "WAV");
	if (!body.sig.Wavread(fnamewithpath.c_str(), errStr)) {strcat (errStr, fnamewithpath.c_str());	throw errStr;}
	if (body.sig.GetFs()!=22050) if(!body.sig.Resample(22050,errStr))  {strcat (errStr, " "); strcat (errStr, fnamewithpath.c_str());	throw errStr;}
	if (arg.size()>1)	
	{
		if (tmark[1]<body.sig.GetDuration()) body.sig.Truncate(tmark[0], tmark[1]);
		body.sig.Insert(xout,0);
	}
	return body;
}

CEqSig& cal_readtxt::calc(list<CEqSig *> arg)
{
	list <CEqSig *>::iterator iter;
	CSignals xout;
	char errStr[256], estr[256], *buf;;
	HANDLE hFile;
	string fnamewithpath;
	int nArgs(0);
	CEqSig args[8];
	getArguments(arg, args, nArgs);
	if (arg.size()!=1) throw "Too many arguments.";
	char quot('"');
	args[0].str = args[0].str.substr(1, string::npos);
	args[0].str = args[0].str.substr(0, args[0].str.length()-1);
	args[0].FileWithPath(args[0].str, fnamewithpath, "txt");
	if ((hFile = CreateFile(fnamewithpath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ))== INVALID_HANDLE_VALUE)
	{
		sprintf(errStr, "Cannot read the file %s", fnamewithpath.c_str());
		throw errStr;
	}
	DWORD dw, fsize = GetFileSize(hFile, &dw);
	if (fsize==INVALID_FILE_SIZE) 	{
		sprintf(errStr, "Cannot get the size of file %s", fnamewithpath.c_str());
		CloseHandle(hFile);
		throw errStr;
	}
	buf = new char[fsize+1];
	if(!ReadFile (hFile, buf, fsize, &dw, NULL))	{
		sprintf(errStr, "File %s Reading Error, GetLastError=%d",fnamewithpath.c_str(), GetLastError());
		CloseHandle(hFile);
		throw errStr;
	}
	buf[dw]='\0';
	if (body.sig.Str2Sig(buf, estr)==NULL) {
		delete[] buf; 
		CloseHandle(hFile);
		sprintf(errStr, "Str2Sig Error. %s", estr);
		throw errStr;
	}
	delete[] buf; 
	CloseHandle(hFile);
	return body;
}

CEqSig& cal_irand::calc(list<CEqSig *> arg)
{
	//This generates a random number between 1 and n (inclusive)
	cal_rand temp;
	CEqSig out = temp.calc(arg);
	body.sig.val = (double)((int)out.sig.val+1);
	return body;
}

CEqSig& cal_randperm::calc(list<CEqSig *> arg)
{
	if (arg.size()!=1)	throw "MUST HAVE ONE ARGUMENT."; 
	int nArgs(0); 
	CEqSig args[1], xEq1, xEq2;
	getArguments(arg, args, nArgs);
	if (args[0].sig.val != (double)(int)args[0].sig.val) throw "The argument must be an integer.";
	if (args[0].sig.val <=0 ) throw "The argument must be a positive integer.";
	body.sig.Reset(0);
	int len = (int)args[0].sig.val;
	double hold;
	cal_irand tempobj;
	body.sig.UpdateBuffer(len);
	for (int i=0; i<len; i++) body.sig.buf[i] = (double)i;
	//swapping 100 times
	int repeat = (int)sqrt(len*100.);
	for (int i=0; i<repeat; i++)
	{
		xEq1 = tempobj.calc(arg);
		do
		{
			xEq2 = tempobj.calc(arg);
		} while (xEq2.sig.val==xEq1.sig.val);
		hold = body.sig.buf[(int)xEq2.sig.val-1];
		body.sig.buf[(int)xEq2.sig.val-1] = body.sig.buf[(int)xEq1.sig.val-1];
		body.sig.buf[(int)xEq1.sig.val-1] = hold;
	}
	return body;
}

CEqSig& cal_filt::calc(list<CEqSig *> arg)
{
	// If both args[1] and args[2] are 
	// array --> filter them
	// string --> get the coeff from mat file and filter them
	// if there's only one arg, FIR, the second argument should be just one.
	list <CEqSig *>::iterator iter;
	CSignals xout;
	char errStr[256], buf[256];
	string fnamewithpath;
	CEqSig args[3];
	if (arg.size()>3) throw "Too many arguments.";
	int nArgs(0);
	CSignals denom(0);
 	for ( iter = arg.begin( ); iter != arg.end( ); iter++ )
	{
		CEqSig *p = *iter;
		args[nArgs++] = (CEqSig &)*p;
	}
	strcpy(errStr, "filt(signal, ""Matlab file name"", ""filter variable name"")\n");
	strcat(errStr, "filt(signal, Numerator_array, [Denominator_array=1 for FIR])\n");
	if (nArgs<2) // 
	{
		sprintf(buf, "Too few arguments.\n");		strcat(errStr, buf);
		throw errStr;
	}
	if (args[0].sig.GetLength()<=1) 
	{
		sprintf(buf, "First arg is the signal to filter.\n");		strcat(errStr, buf);
		throw errStr;
	}
	body.sig = args[0].sig;
	if (nArgs==2) // second arg must be an array and add the third arg as one
	{
		if (args[1].sig.GetLength()<1) 
		{
			sprintf(buf, "Second arg should be the numerator array for filtering.\n");	strcat(errStr, buf);
			throw errStr;
		}
//		denom.val=1.;
	}
	else
	{
		if (args[1].sig.nSamples>1 && args[2].sig.nSamples>1 && args[1].sig.nSamples!=args[2].sig.nSamples)
		{
			sprintf(buf, "Second & Third arg should be the same length, if not constant.\n");	strcat(errStr, buf);
			throw errStr;
		}
		char quot('"');
		string str1(args[1].str), str2(args[2].str);
		if (str1.at(0)==quot || str1.at(str1.length()-1)==quot || str2.at(0)==quot || str2.at(str2.length()-1)==quot)
		{
			if (str1.at(0)!=quot || str1.at(str1.length()-1)!=quot || str2.at(0)!=quot || str2.at(str2.length()-1)!=quot)
			{
				sprintf(buf, "In order to retrieve mat file for filter coeff. Use "" for the file name and variable name.\n");	strcat(errStr, buf);
				throw errStr;
			}
			for (int i=1; i<3; i++)
			{
				args[i].str.erase(0,1);
				args[i].str.erase(args[i].str.length()-1,1);
			}
			args[1].FileWithPath(args[1].str, fnamewithpath, "mat");
			errStr[0]=0;
			if (!body.sig.Filter(fnamewithpath.c_str(), args[2].str.c_str(), errStr)) throw errStr;
			return body;
		}
	}
	int len = max(args[1].sig.nSamples, args[2].sig.nSamples);
	double *arr = new double[len];
	memset((void*)arr, 0, sizeof(double)*len);
	if (args[1].sig.nSamples==0) 
	{
		arr[0] = args[1].sig.val;
		body.sig.Filter(len, arr, args[2].sig.buf);
	}
	else if (args[1].sig.nSamples==1) 
	{
		arr[0] = args[1].sig.buf[0];
		body.sig.Filter(len, arr, args[2].sig.buf);
	}
	else if (args[2].sig.nSamples==0) 
	{
		arr[0] = args[2].sig.val;
		body.sig.Filter(len, args[1].sig.buf, arr);
	}
	else if (args[2].sig.nSamples==1) 
	{
		arr[0] = args[2].sig.buf[0];
		body.sig.Filter(len, args[1].sig.buf, arr);
	}
	else
		body.sig.Filter(len, args[1].sig.buf, args[2].sig.buf);
	delete[] arr;
	return body;
}

CEqSig& cal_rms::calc(list<CEqSig *> arg)
{
	char errStr[256];
	strcpy(errStr, "rms(signal)\n");
	strcat(errStr, "rms(signal, TargetRmsValue) or\n");
	strcat(errStr, "rms(Signal, Ref, Signal2RefRatio_dB)\n");
	if (arg.size()>3 || arg.size()==0) throw errStr;
	list <CEqSig *>::iterator iter;
	int nArgs(0);
	CEqSig args[3], tempEq, tempEq2;
 	for ( iter = arg.begin( ); iter != arg.end( ); iter++ )
	{
		CEqSig *p = *iter;
		args[nArgs++] = (CEqSig &)*p;
	}
	if (args[0].sig.GetLength()<=1) throw "The first argument must be a signal.";
	if (nArgs==1) // calculates the rms
	{
		body.sig.Reset(0);
		body.sig.val =  args[0].sig.LevelLT();
	}
	else
	{
		if (args[1].sig.GetLength()>1) 
		{
			list<CEqSig *> holder(arg);
			if (nArgs != 3) throw errStr;
			double targetSRR = args[2].sig.val;
			arg.pop_back(); arg.pop_back();
			tempEq = calc(arg); // First argument

			holder.pop_back(); holder.pop_front();
			tempEq2 = calc(holder); // Second argument

			double currentSRR = tempEq.sig.val - tempEq2.sig.val;
			body.sig = args[0].sig;
			body.sig *= exp((targetSRR - tempEq.sig.val + tempEq2.sig.val) * LOG10DIV20);
		}
		else
		{
			if (nArgs == 3) throw errStr;
			arg.pop_back();
			tempEq = calc(arg);
			double ssratio = args[1].sig.val - tempEq.sig.val - 0.000262;
			// the adjust value above is the calculated rms value of a full scale sinusoid (this is necessary to avoid the clipping of rms adjusted full scale sinusoid)
			body.sig = args[0].sig;
			body.sig *= exp(ssratio * LOG10DIV20);
		}

	}
	return body;
}

