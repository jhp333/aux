#include "stdafx.h"
#include "aceproc_internal.h"
#include "HELP.h"
#define LEN_ERR_STRING	4096*4096

int str2doublearray (double* x, int maxSize_x, const char* buf, char* deliminators);

static int err;
static char errStrMatlab[LEN_ERR_STRING];

extern char path_to_dll[_MAX_PATH];


static int ErrorHandler(const char*s)
{
	err=1;
	strcat (errStrMatlab, s);
	return strlen(errStrMatlab);
}

static int PrintHandler(const char *s)
{
	strcat (errStrMatlab, s);
	return strlen(errStrMatlab);
}

int CAceprocApp::initLib()
{
	mclInitializeApplication(NULL,0);
	return mxaceproc5InitializeWithHandlers(PrintHandler, ErrorHandler);
}

void CAceprocApp::closeLib()
{
	mxaceproc5Terminate();
}

int CAceprocApp::_numbands2Cutfreqs(double **freqs, int nBands)
{
	mxArray *xCutoff = NULL;
	err = 0;	errStr = errStrMatlab; strcpy(errStrMatlab,"");
	mlfAcefreqbound (1, &xCutoff, mxCreateDoubleScalar(nBands));
	if (!err)
		*freqs = mxGetPr (xCutoff);
	mxDestroyArray(xCutoff);
	return (!err);
}

int CAceprocApp::_FixCRCR_(char *str)
{ /*Error String from Matlab has only \n, not \r\n, therefore, 
	 it is not suitable to display as is. Need to convert \n to \r\n
	 Besides, often \n\n was used where a single \n would suffice. 
	 Therefore, a matlab function elimcrcr.m does the following two things:
	 1) convert \n to \r\n
	 2) replace \r\n\r\n with \r\n */
	mxArray *Y=NULL;
	mlfElimcrcr(1, &Y, mxCreateString(str));
	mxGetString(Y,str,2*strlen(str));
	mxDestroyArray(Y);
	return (!err);
}


int CAceprocApp::str2mxArray(mxArray **var, int nVar, CString str)
{ 
	// Anyone calling this function shall be responsible for destroying var[i]

	int i,j, res,k,loop,indicesBrOpen[64], indicesBrClosed[64];
	int nBrOpen, nBrClosed;
	CString work, workNext;
	mxArray *Out;
	CString s = str; // making a duplicate copy
	j=0;
	k=0;
	res=0;
	loop=1;
	if (s[0]=='[')
		indicesBrOpen[k++]=0;
	while(res!=-1)
	{
		res = s.Find('[',res+1);
		indicesBrOpen[k++]=res;
	}
	nBrOpen = k-1;
	indicesBrOpen[k-1]=s.GetLength();
	k=0;
	res=0;
	while(res!=-1)
	{
		res = s.Find(']',res+1);
		indicesBrClosed[k++]=res;
	}
	nBrClosed = k-1;
	if (nBrClosed!=nBrOpen)
		return 0;
	if (!CheckBracketOpenClosed(indicesBrOpen, indicesBrClosed, nBrClosed))
		return 0;
	for (i=0; i<nBrClosed+1; i++)
	{
		work = s.Left(indicesBrOpen[i]); //Before the bracket. Only used for the first loop.
		while (loop)
		{
			workNext=str2SinglemxArray(&Out, work);
			if(strlen(workNext)>0)
			{
				if (j>nVar-1)			return j;
				var[j++] = Out;
				work = workNext;
			}
			else
				loop = 0;
		}
		work = s.Mid(indicesBrOpen[i], indicesBrClosed[i]-indicesBrOpen[i]);
		if (str2ArrayOfmxArray(&Out, work)>0)
		{
			if (j>nVar-1)			return j;
			var[j++] = Out;
		}
		else
			return j;
		work = s.Mid(indicesBrClosed[i], indicesBrOpen[i+1]-indicesBrClosed[i]);
		loop=1;
		while (loop)
		{
			workNext=str2SinglemxArray(&Out, work);
			if(strlen(workNext)>0)
			{
				if (j>nVar-1)			return j;
				var[j++] = Out;
				work = workNext;
			}
			else
				loop = 0;
		}
	}
	return j;
}

int CAceprocApp::str2ArrayOfmxArray(mxArray **X, CString s)
{
	char *list[32];
	double array[32];
	int i,nItems;
	CString str=s;
	nItems = countDeliminators(str, " []");
	if (nItems==0)
		return 0;
	for (i=0;i<32;i++)
		list[i]=(char*)calloc(32,sizeof(char));
	nItems = str2strarray(list, 32, str, " []");
	for (i=0;i<nItems;i++)
		array[i] = atof(list[i]);
	*X = mxCreateDoubleMatrix (1,nItems, mxREAL); 
	memcpy(mxGetPr(*X), (void*)array, nItems*sizeof(double));
	for (i=0;i<32;i++)
		free(list[i]);
	return nItems;
}

CString CAceprocApp::str2SinglemxArray(mxArray **X, CString s)
{
	char *list[2], head[32], rest[32];
	int nItems, fd;
	CString str=s;
	list[0]= head; list[1]=rest;
	nItems = str2strarray(list, 2, str, " []");
	if (nItems==0)
		return "";
	if (IsCharAlpha(head[0]))
		*X = mxCreateString (head);
	else
		*X =mxCreateDoubleScalar(atof(head));
	fd = str.Find(rest);
	s.Delete(0,str.Find(rest));
	s +=" ";
	return s;
}


int CAceprocApp::CheckBracketOpenClosed(int *x, int *y, int k)
{
	int i;
	if (k==0)
		return 1;
	for (i=0;i<k-1;i++)
	{
		if (x[i]>=y[i])
			return 0;
	}
	if ( x[k] < y[k-1]) 
		return 0;
	return 1;
}

int CAceprocApp::Conditioner(char *qufname, ACESEQ *seqOut, char *str, int nVarargin)
{
	static mxArray *xY, *xPath2;
	mxArray *xP;
	mxArray *xAddStr, *xAddedArg[4];
	mwSize dim[2];
	int nChan, i, nArgs;
	double *rate;

	err = 0;	errStr = errStrMatlab; strcpy(errStrMatlab,"");
	nChan=0;
	for (i=0; i<22; i++)
		if (param.selected[i])
			nChan++;

	aceparam_c2mx(&param, &xP);
	mxDestroyArray(xY);

    dim[0] = 1;
	dim[1] = nVarargin;
	xAddStr = mxCreateCellArray(2,dim);
	nArgs = str2mxArray(xAddedArg, nVarargin, str);
	if (nArgs!=nVarargin)
	{
		err=1;
		strcpy(errStr, "Insufficient input arguments.");
	}
	else
	{
		mxSetCell(xAddStr, 0, xAddedArg[0]); //proc identifier string
		for (i=1; i<nVarargin; i++)
			mxSetCell(xAddStr, i, xAddedArg[i]);

		if (err)		errStr = errStrMatlab;

		xY=NULL;
		xPath2=NULL;
		mlfGenaceseq (2, &xY, &xPath2, mxCreateString(wavName), xP, mxCreateString(qufname), xAddStr);
	}
	rate = mxGetPr(xPath2);
	mxDestroyArray(xAddStr);
	mxDestroyArray(xP);
	mxDestroyArray(xPath2);
	if (err==1)
		return 0;
	else
	{
		mxArray2ACESEQ(seqOut, xY);
		return (int)(*rate+.5);
	}
}

int CAceprocApp::ACE(char *qufname, ACESEQ *seqOut)
{
	static mxArray *xY, *xPath2;
	mxArray *xP;
	int i, nChan;
	double *x;

	err = 0;	errStr = errStrMatlab; strcpy(errStrMatlab,"");
	nChan=0;
	for (i=0; i<22; i++)
		if (param.selected[i])
			nChan++;

	aceparam_c2mx (&param, &xP);
	mxDestroyArray(xY);
	xY=NULL; xPath2 = NULL;
	mlfGenaceseq (2, &xY, &xPath2, mxCreateString(wavName), xP, mxCreateString(qufname), NULL);
	mxDestroyArray(xP);
	mxDestroyArray(xPath2);
	if (err==1)
		return 0;
	else
	{
		x=mxGetPr(xPath2);
		mxArray2ACESEQ(seqOut, xY);
		return (int)*x+1;
	}
}

int CAceprocApp::mxArray2ACESEQ(ACESEQ *seq, mxArray *X)
{
	int i, total_num_of_elements, number_of_fields, indexFilled;
	const char  *field_name;

	indexFilled=0;
	total_num_of_elements = mxGetNumberOfElements(X);
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
			seq->length = mxGetM(mxGetFieldByNumber(X, 0, i));
		}
		else if (!strcmp(field_name, "modes"))
		{
			seq->mode = mxGetPr(mxGetFieldByNumber(X, 0, i));
			indexFilled++;
		}
		else if (!strcmp(field_name, "phase_widths"))
		{
			seq->pw = mxGetPr(mxGetFieldByNumber(X, 0, i));
			indexFilled++;
			seq->lenPW = mxGetM(mxGetFieldByNumber(X, 0, i));
		}
		else if (!strcmp(field_name, "phase_gaps"))
		{
			seq->pg = mxGetPr(mxGetFieldByNumber(X, 0, i));
			indexFilled++;
			seq->lenPG = mxGetM(mxGetFieldByNumber(X, 0, i));
		}
		else if (!strcmp(field_name, "periods"))
		{
			seq->period = mxGetPr(mxGetFieldByNumber(X, 0, i));
			indexFilled++;
		}
	}
	if (indexFilled>=5)
		return seq->length;
	else
		return -2;
}

double CAceprocApp::onoffConditioner(const char *qufnameCond, int e, int cl, int rate, int onoff, char **errstr)
{
	// returns the rate of conditioner (when turning on) or -1 (when turning off) if succeeded
	// returns 0 for failure
	static mxArray *xCondRate;
	double *out;
	err = 0;		errStr = errStrMatlab; 	strcpy(errStrMatlab,"");

	mxDestroyArray(xCondRate);
	xCondRate = NULL;
	mlfOnoffconditioner (1, &xCondRate, mxCreateString(qufnameCond), mxCreateDoubleScalar(e), mxCreateDoubleScalar(cl), mxCreateDoubleScalar(rate), mxCreateDoubleScalar(onoff));
	if (err==1)
	{
		*errstr = errStr;
		return 0;
	}
	else
	{
		out = mxGetPr(xCondRate);
		return *out;
	}
}


int CAceprocApp::_genconstim(const char *qufname, int *elec, int *cl, int nChan, double pw, double pg, double ChanRate, int nPulseChan, int condE, int condCL, char **errstr)
{
	// When conditioner is not used, condE and condCL must be zeros.
	mxArray *xElec, *xCl, *xQ;
	int i;
	double *delec = (double*)malloc(sizeof(double)*nChan);
	double *dcl = (double*)malloc(sizeof(double)*nChan);
	err = 0;		errStr = errStrMatlab; 	strcpy(errStrMatlab,"");
	xElec = mxCreateDoubleMatrix (nChan, 1, mxREAL);
	xCl = mxCreateDoubleMatrix (nChan, 1, mxREAL);
	for (i=0; i<nChan; i++)		delec[i]=(double)elec[i];
	mxSetPr(xElec, delec);
	for (i=0; i<nChan; i++)		dcl[i]=(double)cl[i];
	mxSetPr(xCl, dcl);
	xQ = NULL;
	mlfGenconstim (1, &xQ, mxCreateString(qufname), xElec, xCl, mxCreateDoubleScalar(pw), mxCreateDoubleScalar(pg), mxCreateDoubleScalar(ChanRate), mxCreateDoubleScalar(nPulseChan), mxCreateDoubleScalar(condE), mxCreateDoubleScalar(condCL));
 	mxDestroyArray(xQ);
	free(delec);
	free(dcl);

	if (err==1)
	{
		*errstr = errStr;
		return 0;
	}
	else
	{
		return 1;
	}
}
