#include "Echo.h"

#define DISPLAY(X,Y) \
{	if (swt==NULL) EditPrintf (hDisplay, X, Y); \
	else fprintf(swt, X, Y); }

void defineFeature (int *v, int *m, int *p, char *token);
void GetConfMtx (int *confMtx, int *presented, int *responded, int numTrials, int *featureDefTable);
int countFeatures (int *featureDefTable, int len);
double CalculateFeatureTransmission (int *confMtx, int *featureDefTable);

int CountSameElements(int *x, int *y, int len);
int mod (int big, int divider);

int nOutputCharsBeforeNote;
char displayedStrWOcommnet[8192];

int GetFeatureScore(HWND hDlg, char *featureScoreOut, const char c, int * presented, int * responded, int numTrials)
{
	int res, i, j, k, feat, confMtx[576], featureDefTable[24];
	memset((void*)featureDefTable, -1, 24*sizeof(int));
	memset((void*)confMtx, 0, 576*sizeof(int));
	for (i=0; i < ss.Ntokens; i++)
	{ 
		switch (c)
		{
		case 'v':
			defineFeature (&feat, &j, &k, ss.token[i]);
			break;
		case 'm':
			defineFeature (&j, &feat, &k, ss.token[i]);
			break;
		case 'p':
			defineFeature (&j, &k, &feat, ss.token[i]);
			break;
		default:
			MessageBox (hDlg, "GetFeatureScore MUST have v, m, or p.", "", MB_OK);
			return -1;
		}
		if (feat==-1)
			MessageBox (hDlg, "Error in defineFeature", "", MB_OK);
		featureDefTable[i] = feat;
	}
	res = countFeatures (featureDefTable, ss.Ntokens);
	if (res>1)
	{
		GetConfMtx (confMtx, presented, responded, numTrials, featureDefTable);
		sprintf (featureScoreOut, "%6.2f", CalculateFeatureTransmission (confMtx, featureDefTable));
	}
	else
		sprintf (featureScoreOut, "undetermined");

	return res;
}

void DisplaySummary(HWND hDlg, char *header, FILE *swt, char **pr, char **rp, int numTrials, int firstend)
{ 
	// firstend is even number for the first one (begin time marked), multiple of 3 for the last one (end time marked)
	// (if both first and last, it should be 6.)
	// For display on screen swt is NULL, all the other saves it to the file.
	
	// nCorr_multi is ignored if single presentation
	int i, j, cumCorr, confMtx[576], nItems;
	int *presented, *responded, featureDefTable[24];
	char buf[4096], scoreStr[64], buf1[1024], buf2[64], str[8192*2], **parse;
	HWND hPar = GetParent(hDlg);
	HWND hDisplay = GetDlgItem(hDlg, IDC_SUMMARY);

	cumCorr=0;
	memset((void*)confMtx, 0, 576*sizeof(int));
	presented = (int*)calloc(numTrials*nConc, sizeof(int));
	responded = (int*)calloc(numTrials*nConc, sizeof(int));
	for (i=0; i<numTrials;i++)
	{
		for (j=0; j<nConc;j++)
		{
			presented[i*nConc+j] = MainAppWnd->wavID2tokenID(pr[j][i]);
			responded[i*nConc+j] = rp[j][i];
		}
		cumCorr += CountSameElements(presented+i*nConc, responded+i*nConc, nConc);
	}

	if (nConc==1)
	{
		sprintf (scoreStr, "[OVERALL SCORE] %5.1f%% (%d out of %d)", 
			(double)cumCorr / (double) numTrials * 100., cumCorr, numTrials);
	}
	else
	{
		sprintf (scoreStr, "[OVERALL SCORE] %5.1f%% (%d out of %d(%d tokens in %d trials))", 
			(double)cumCorr/(double)numTrials/(double)nConc*100., (int)cumCorr, numTrials*nConc, nConc, numTrials);
	}
	// header
	if (mod(firstend,2)==0) // even 
		DISPLAY ("%s", sessionBeganTime);
	// header (including presenter processing info)---if too long, display up to 4096 on screen (save the whole in the file)
	if (strlen(header)>4096 && swt==NULL)
	{
		strncpy(buf, header, 4095);
		buf[4095]=0;
		EditPrintf (hDisplay, "%s\r\n", buf);
	}
	else
		DISPLAY ("%s\r\n", preparedCondition);
	// overall score
	DISPLAY ("%s\r\n", scoreStr);
	GetDlgItemText (hDlg, IDC_SUMMARY, str, sizeof(str));
	nOutputCharsBeforeNote = strlen(str);

	// Add note2save.... (only for file saving)
	GetDlgItemText (hDlg, IDC_NOTE2SAVE, buf1, sizeof(buf1));
	if (strlen(buf1)>0 && swt!=NULL)
		fprintf (swt, "%s\r\n", buf1);

	// Confusion matrix (and feature trans) for single consonant, presented-responded log for everything else
	if (nConc==1)
	{
		// for overall confusion mtx 
		for (i=0; i<ss.Ntokens; i++)
			featureDefTable[i] = i;

		GetConfMtx (confMtx, presented, responded, numTrials, featureDefTable);
		strcpy(str,"[CONFUSION MATRIX]\r\n     ");
		for (i=0; i<ss.Ntokens; i++)
		{
			sprintf (buf1, "%s ", ss.token[i]);
			strcat (str, buf1);
		}
		strcat (str, "\r\n");
		EditPrintf (hDisplay, str);

		sprintf (str,"");
		for (i=0; i<ss.Ntokens; i++)
		{
			sprintf (buf1, "%s : ", ss.token[i]);
			for (j=0; j<ss.Ntokens; j++)
			{
				sprintf (buf2, "%d ", confMtx[i*ss.Ntokens+j]);
				strcat (buf1, buf2);
			}
			strcat (buf1, "\r\n");
			strcat(str, buf1);
		}
		DISPLAY ("%s", str);
	}
	else if (swt!=NULL)
	{
		DISPLAY ("%s", "[PRESENTED-RESPONDED]\r\n");
		//Retrieve response log from IDC_RESPONSELOG
		GetDlgItemText (hPar, IDC_RESPONSELOG, str, sizeof(str));
		nItems = countDeliminators(str, "\r\n");
		parse = (char**)calloc(nItems, sizeof(char**));
		for (i=0; i<nItems; i++)
			parse[i] = (char*)calloc(nItems, sizeof(char**));
		str2strarray(parse, nItems, str, "\r\n");
		for (i=0; i<nItems; i++)
		{
			fprintf(swt, "%s, ", parse[i]);
			free(parse[i]);
		}
		free(parse);
		fprintf(swt, "\r\n", parse[i]);
	}
	// end of overall

	if (nConc==1 && consTesting)
	{
		GetFeatureScore(hDlg, buf, 'v', presented, responded, numTrials);
		DISPLAY ("voicing = %s\r\n", buf);
		GetFeatureScore(hDlg, buf, 'm', presented, responded, numTrials);
		DISPLAY ("manner = %s\r\n", buf);
		GetFeatureScore(hDlg, buf, 'p', presented, responded, numTrials);
		DISPLAY ("place = %s\r\n\r\n", buf);
	}
	timeStamp (buf1, buf2);
	sprintf (str, "Session ends %s, %s\r\n=============================\r\n", buf1, buf2);
	if (mod(firstend,3)==0)
		DISPLAY ("%s", str);
	GetDlgItemText (hDlg, IDC_CONFMTX, displayedStrWOcommnet, sizeof(displayedStrWOcommnet));
	free(presented);
	free(responded);
}