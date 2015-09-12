// ResultWnd.cpp: implementation of the CResultWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "Echo.h"
#include "ResultWnd.h"
#include <math.h>
#include "EchoMainWnd.h"

#define DISPLAY(X,Y) \
{	if (swt==NULL) EditPrintf (hDisplay, X, Y);  else fprintf(swt, X, Y); }

void defineFeature (int *v, int *m, int *p, char *token);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CALLBACK  ResultProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	switch (umsg)
	{ 
	chHANDLE_DLGMSG (hDlg, WM_INITDIALOG, MainAppWnd.hResult->OnInitDialog);
	chHANDLE_DLGMSG (hDlg, WM_COMMAND, MainAppWnd.hResult->OnCommand);
	chHANDLE_DLGMSG (hDlg, WM_CLOSE, MainAppWnd.hResult->OnClose);
	chHANDLE_DLGMSG (hDlg, WM__SAVE2FILE, MainAppWnd.hResult->Save2File);
	default:		return FALSE;
	}
	return TRUE;
}


CResultWnd::CResultWnd()
{
	FileNameWPath[0]='\0';
}

CResultWnd::~CResultWnd()
{

}

void CResultWnd::Save2File(char* fname)
{
	FILE *fp = fopen("session.log", "wb");
	if (fp) {
		MainAppWnd.SessionDonePrint(0, fp);
		fclose(fp);
		MainAppWnd.report.Upload("session.log");
	} else
		MessageBox ("file session.log open fails.");

	fp = fopen(fname, "ab");
	if (fp) {
		MainAppWnd.SessionDonePrint(0, fp);
		if (fclose(fp) == 0) {
			saved = 1;
			DestroyWindow(hDlg);
		}
	} else
		MessageBox ("file open fails.", fname);
}

BOOL CResultWnd::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
	saved = 0;
	return 1;
}

void CResultWnd::OnCommand(int idc, HWND hwndCtl, UINT event) 
{
	CString str, note;
	switch (idc)
	{
	case IDC_NOTE2SAVE:
		if (event==EN_UPDATE)
		{
			GetDlgItemText(IDC_NOTE2SAVE, note);
			note = "\r\n" + note + "\r\n" + displayedStrWOcommnet.Mid(nOutputCharsBeforeNote);
			SendDlgItemMessage (IDC_SUMMARY, EM_SETSEL, (WPARAM) nOutputCharsBeforeNote, (LPARAM) -1) ;
			SendDlgItemMessage (IDC_SUMMARY, EM_REPLACESEL, FALSE, (LPARAM) note.GetBuffer()) ;
			SendDlgItemMessage (IDC_SUMMARY, EM_LINESCROLL, 0, 
			SendDlgItemMessage (IDC_SUMMARY, EM_GETLINECOUNT));
		}
		break;

	case IDC_SAVE2FILE:
		if (strlen(FileNameWPath)==0)
			sprintf(FileNameWPath, "%s*.txt", MainAppWnd.resultPath);
		if (ResultFileSaveDlg (hDlg, FileNameWPath))
			SendMessage( WM__SAVE2FILE, (WPARAM)FileNameWPath);
		break;

	case WM_DESTROY:
		DestroyWindow(hDlg);
		break;
	}
}

void CResultWnd::OnClose()
{
	if (!saved)
	{
		if (MessageBox ("You have not saved the result.", "Discard the result?",  MB_YESNO)==IDYES)
			DestroyWindow(hDlg);
		else 
			return;
	}
	hDlg = NULL;
	MainAppWnd.CleanUpTestingSession();
}


int CResultWnd::GetFeatureScore(char *featureScoreOut, const char c, int * presented, int * responded, int numTrials)
{
	int res, i, j, k, feat, confMtx[576], featureDefTable[24];
	memset((void*)featureDefTable, -1, 24*sizeof(int));
	memset((void*)confMtx, 0, 576*sizeof(int));
	for (i=0; i < MainAppWnd.ss->nToken; i++)
	{ 
		switch (c)
		{
		case 'v':
			defineFeature (&feat, &j, &k, MainAppWnd.ss->token[i]);
			break;
		case 'm':
			defineFeature (&j, &feat, &k, MainAppWnd.ss->token[i]);
			break;
		case 'p':
			defineFeature (&j, &k, &feat, MainAppWnd.ss->token[i]);
			break;
		default:
			MessageBox ("GetFeatureScore MUST have v, m, or p.");
			return -1;
		}
		if (feat==-1)
			MessageBox ("Error in defineFeature");
		featureDefTable[i] = feat;
	}
	res = countFeatures (featureDefTable, MainAppWnd.ss->nToken);
	if (res>1)
	{
		GetConfMtx (confMtx, presented, responded, numTrials, featureDefTable);
		sprintf (featureScoreOut, "%6.2f", CalculateFeatureTransmission (confMtx, featureDefTable));
	}
	else
		sprintf (featureScoreOut, "undetermined");

	return res;
}

void CResultWnd::outputSummary(FILE *fp, int *po, unsigned char **rp, int numTrials)
{ 
	// nCorr_multi is ignored if single presentation
	int i, j, cumCorr, confMtx[576];
	int nConc = MainAppWnd.nConc;
	int *presented, *responded, featureDefTable[24];
	char buf[4096]; //, scoreStr[64], buf1[1024], buf2[64], str[8192*2];

	cumCorr=0;
	memset((void*)confMtx, 0, 576*sizeof(int));
	presented = (int*)calloc(numTrials*nConc, sizeof(int));
	responded = (int*)calloc(numTrials*nConc, sizeof(int));
	for (i=0; i<numTrials;i++)
	{
		for (j=0; j<nConc;j++)
		{
			presented[i*nConc+j] = MainAppWnd.wavID2tokenID[MainAppWnd.Combs[po[i]][j]];
			responded[i*nConc+j] = rp[j][i];
		}
		cumCorr += CountSameElements(presented+i*nConc, responded+i*nConc, nConc);
	}

	// overall score
	if (nConc==1)
	{
		fprintf(fp, "[OVERALL SCORE] %5.1f%% (%d out of %d)\r\n",
			(double)cumCorr / (double) numTrials * 100., cumCorr, numTrials);
	}
	else
	{
		fprintf(fp, "[OVERALL SCORE] %5.1f%% (%d out of %d(%d tokens in %d trials))\r\n",
			(double)cumCorr/(double)numTrials/(double)nConc*100., (int)cumCorr, numTrials*nConc, nConc, numTrials);
	}

	// Confusion matrix (and feature trans) for single consonant, presented-responded log for everything else
	if (nConc==1)
	{
		// for overall confusion mtx 
		for (i=0; i<MainAppWnd.ss->nToken; i++)
			featureDefTable[i] = i;

		GetConfMtx (confMtx, presented, responded, numTrials, featureDefTable);
		fprintf(fp, "[CONFUSION MATRIX]\r\n     ");
		for (i=0; i<MainAppWnd.ss->nToken; i++)
			fprintf(fp, "%s ", MainAppWnd.ss->token[i]);
		fprintf(fp, "\r\n");

		for (i=0; i<MainAppWnd.ss->nToken; i++)
		{
			fprintf(fp, "%s : ", MainAppWnd.ss->token[i]);
			for (j=0; j<MainAppWnd.ss->nToken; j++)
				fprintf(fp, "%d ", confMtx[i*MainAppWnd.ss->nToken+j]);
			fprintf(fp, "\r\n");
		}
	}
	// end of overall

	if (nConc==1 && MainAppWnd.consTesting)
	{
		GetFeatureScore(buf, 'v', presented, responded, numTrials);
		fprintf(fp, "voicing = %s\r\n", buf);
		GetFeatureScore(buf, 'm', presented, responded, numTrials);
		fprintf(fp, "manner = %s\r\n", buf);
		GetFeatureScore(buf, 'p', presented, responded, numTrials);
		fprintf(fp, "place = %s\r\n\r\n", buf);
	}
	free(presented);
	free(responded);
}

void CResultWnd::outputFooter(FILE *fp)
{ 
	char buf1[64], buf2[64], str[1024];

	nOutputCharsBeforeNote = ftell(fp);
	// Add note2save.... (only for file saving)
	if (GetDlgItemText (IDC_NOTE2SAVE, str, sizeof(str))>0)
		fprintf (fp, "\r\n%s\r\n", str);

	timeStamp (buf1, buf2);
	fprintf (fp, "\r\nSession ends %s, %s\r\n=============================\r\n", buf1, buf2);

	long fsize = ftell(fp);
	char *buf = displayedStrWOcommnet.GetBufferSetLength(fsize+1);
	rewind(fp);
	fread(buf, 1, fsize, fp);
	buf[fsize] = '\0';
	SetDlgItemText(IDC_SUMMARY, buf);
}

int CResultWnd::countFeatures (int *featureDefTable, int len)
{
	// featureDefTable may contain non-consecutive numbers
	int i, res, temp[24];

	temp[0] = featureDefTable[0];
	res=1;

	for (i=1; i<len; i++)
	{
		if (find (temp, res, featureDefTable[i]) == 0 )
		{
			temp[res] = featureDefTable[i];
			res++;
		}
	}
	return res;
}

void CResultWnd::GetMappedTable (int *mappedTable, int *featureDefTable, int len)
{
	// This is necessary if featureDefTable does not begin with 0, otherwise
	// mappedTable is the same as featureDefTable
	int i, ind, cum = 0;

	mappedTable[0] = 0;
	for (i=1; i < len; i++)
	{
		ind = findWhere (featureDefTable, i, featureDefTable[i]);
		if (ind==-1) // if not found, add to mappedTable
			mappedTable[i] = ++cum;
		else
			mappedTable[i] = mappedTable[ind];
	}

}

void CResultWnd::GetConfMtx (int *confMtx, int *presented, int *responded, int numTrials, int *featureDefTable)
{
	// featureDefTable must be consecutive, ascending from zero with equal number with trlog.stim or trlog.resp

	int i, nFeatures, *mappedTable;

	nFeatures = countFeatures (featureDefTable, MainAppWnd.ss->nToken);
	mappedTable = (int*) calloc (MainAppWnd.ss->nToken, sizeof(int));

	GetMappedTable (mappedTable, featureDefTable, MainAppWnd.ss->nToken);

	for (i=0; i<numTrials; i++)
		++confMtx[mappedTable[presented[i]] * nFeatures + mappedTable[responded[i]]];
	free(mappedTable);
}

double CResultWnd::CalculateFeatureTransmission (int *confMtx, int *featureDefTable)
{
	double trans, maxTrans, tp;
	int i,j, nTrials, rowSum[8], colSum[8], nFeatures;
	// maximum number of feature scale is 8.... probably way more than enough

	nFeatures = countFeatures (featureDefTable, MainAppWnd.ss->nToken);

	for (i=0;i<8;i++)
	{
		rowSum[i] = 0;
		colSum[i] = 0;
	}
	for (i=0; i<nFeatures; i++)
		for (j=0; j<nFeatures; j++)
			rowSum[i] += confMtx[i*nFeatures+j];
	for (i=0; i<nFeatures; i++)
		for (j=0; j<nFeatures; j++)
			colSum[i] += confMtx[j*nFeatures+i];
	nTrials = 0;
	for (i=0; i<nFeatures; i++)
		nTrials += rowSum[i];
	trans = 0.;
	for (i=0; i<nFeatures; i++)
		for (j=0; j<nFeatures; j++)
		{
			if (confMtx[i*nFeatures+j]!=0)
			{
				tp = (double)(rowSum[i]*colSum[j])/(double)confMtx[i*nFeatures+j]/(double)nTrials;
				trans += (double)confMtx[i*nFeatures+j] / (double)nTrials * 
					log ( tp );// doesn't matter what the base of this log is, 
			}                  // because the ratio is returned later.
		}

	maxTrans = 0.;
	for (i=0; i<nFeatures; i++)
	{
		tp = (double) rowSum[i]/ (double) nTrials;
		maxTrans += (tp) * log (tp);
	}

	return trans / maxTrans;
}