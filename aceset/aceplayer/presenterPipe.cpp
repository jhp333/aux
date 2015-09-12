#include "aceplayer.h"
#include "aceplayerdlg.h"

void genStringUsedElectrodes (char *out, CParamACE *param)
{
	int i(0), j(0);
	char str[256], buf[256];

	strcpy(str,"E("); strcpy(buf,"");
	for ( ; i<22; i++)
	{
		if (param->selected[i])
		{
			sprintf (buf, "%d,", 22-i);
			strcat (str, buf);
			j=1;
		}
	}
	if (j)
		str[strlen(str)-1]=0; // to get rid of the final comma
	strcpy(out, str);
	strcat (out, ") ");
}

void genStringGains (char *out, CParamACE *param)
{
	int i, j(0);
	char str[256], buf[256], buff[256];

	strcpy(str,"[Gain] ("); strcpy(buf,"");
	for (i=0; i<22; i++)
	{
		if (param->selected[i])
		{
			if (param->gains[i]!=0.) j=1;
			sprintfFloat(param->gains[i], 1, buf, sizeof(buf));
			sprintf(buff, "%s,",buf);
			strcat(str,buff);
		}
	}
	if (j)
	{
		str[strlen(str)-1]=0; // to get rid of the final comma
		strcpy(out, str);
		strcat (out, ") ");
	}
	else
		strcpy(out,"");
}

char *getPwStr(CParamACE *param, char *str) {
	int first = -1;
	str[0] = '\0';
	// check if pw values are all the same.
	for (int i=0; i<sizeof(param->pw)/sizeof(param->pw[0]); i++) {
		if (!param->selected[i])
			continue;
		if (first == -1)
			first = i;
		else if (param->pw[i] != param->pw[first]) {
			strcpy(str, "var");
			break;
		}
	}
	if (first == -1)	// no E selected, possible? 8/20/2010 jhpark
		strcpy(str, "??");
	else if (str[0] == '\0')
		sprintf(str, "%d", (int)param->pw[first]);
	return str;
}

void genprocstr(char *out, size_t outsize, CParamACE *param, double level)
{
	char str[256];
	CStdString cstr, cstr0;

	sprintfFloat(level, 1, cstr);
	sprintf(out, "%s, ACE, %s %s", param->subj, is_wACE(param)?"w":"", param->mode);
	if (!is_wACE(param))
		sprintf(out+strlen(out), ", %dHz", (int)param->rate);
	sprintf(out+strlen(out), ", %s/%d, level=%s dBSPL RMS, Q=%d, max=%d\r\n", getPwStr(param,str), (int) param->pg
		, cstr.c_str(), param->Q, param->numMax);
	cstr = "";
	for (int i=0; i<22; ++i) {
		if (param->selected[i]) {
			CStdString cstr1, cstr2, cstr3, cstr4, cstr5;
			sprintfFloat(param->t[i], 1, cstr1);
			sprintfFloat(param->c[i], 1, cstr2);
			sprintfFloat(param->gains[i], 1, cstr3);
			sprintfFloat(param->freqsL[i], 1, cstr4);
			sprintfFloat(param->freqsU[i], 1, cstr5);
			cstr0.Format("%-5d%-6s%-6s%6s%6s  %s\r\n", 22-i, cstr4, cstr5, cstr1, cstr2, cstr3);
			cstr += cstr0;
		}
	}
	if (strlen(param->addmlines)>0) {
		cstr0.Format("Additional p fields: %s\r\n", param->addmlines);
		cstr += cstr0;
	}
	if (strlen(out)+cstr.length()+1 > outsize) {
		strncat(out, cstr.c_str(), outsize - strlen(out) - 4);
		strcat(out, "...");
	} else
		strncat(out, cstr.c_str(), outsize - strlen(out) - 1);
}

void genProcessingString (char *out, size_t outsize, CParamACE *param, int nOutChan, double rmsofwavefile)
{
	double level;
	CParamACE *param2 = param->ptParam2;
	CStdString cstr;
	//oct 28-2010. BJ Kwon
	// the dB level in this out string will be the actual rms level of the presentation
	level = param->level + rmsofwavefile;
	genprocstr(out, outsize, param, level);

	if (nOutChan==2 && param2) {
		char *out2 = new char[outsize+1];
		genprocstr(out2, outsize, param2, level);
		strcat(out, out2);
		delete[] out2;
	}
}

int WaitForController (HANDLE *hPipe, char *errStr)
{
	if (!ConnectNamedPipe(*hPipe, NULL))	
	{
		if (GetLastError() != ERROR_PIPE_CONNECTED)  
		{ GetLastErrorStr(errStr);	return 0;	}
	}
	errStr[0]=0;
	return 1;
}

HANDLE InitPipe(HWND hDlg, char *PipeName, char *errStr)
{
	// This waits for a pipe initiation (blocks till a pipe connection or an error occurs).
	// Run this inside of a second thread.
	SECURITY_ATTRIBUTES	sa;
	SECURITY_DESCRIPTOR	sd;
	HANDLE hPipe;

	if (! InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
	{ 	strcpy (errStr, "ERROR in InitializeSecurityDescriptor()");	return NULL;	}

	if (!SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE))
	{ 	strcpy (errStr, "ERROR in SetSecurityDescriptorDacl()");	return NULL;	}

	sa.nLength = sizeof(sa);			// fill sa fields;  no inheritance needed
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = &sd;
	hPipe = CreateNamedPipe( PipeName, PIPE_ACCESS_DUPLEX, 
		PIPE_TYPE_MESSAGE  | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		1, _MAX_PATH +100, _MAX_PATH +100, 5000, &sa);
	
	if (hPipe == INVALID_HANDLE_VALUE)
	{	GetLastErrorStr(errStr); strcat (errStr, "  [error in CreateNamedPipe]"); return NULL;	}

	if (!WaitForController (&hPipe, errStr))
	{	GetLastErrorStr(errStr); strcat (errStr, "  [error in WaitForController]"); return NULL;	}
	
	return hPipe;
}

void ProcessParsedString (HANDLE hPipe, CStdString *pstr, int nArg)
{
	static CStdString wavName("");
	static char streamFile[MAX_PATH];
	static InterfaceType itype(UNSPECIFIED);
	static CACEplayerDlg *hMainWndDlg;
	static ACESEQ seq; 

	CStdString outStr;
	double dummy;
	char buf[MAX_PATH];
	DWORD dw;
	int i, res;

	if (itype==UNSPECIFIED)
	{
		hMainWndDlg = (CACEplayerDlg *) (LONG_PTR)GetWindowLong (hMainDlg, GWL_USERDATA);
		itype = (InterfaceType)hMainWndDlg->hACEobj[0].param->dwReserved;
		if (itype==SPRINT)	LoadString (hInst, IDS_STREAM_FNAME, buf, sizeof(buf));
		else if (itype==L34_CIC3 || itype==L34_CIC4)	LoadString (hInst, IDS_STREAM_FNAME2, buf, sizeof(buf));
		else MessageBox(hMainDlg, "Check itype", "", MB_OK);
		FulfillFile(streamFile, hMainWndDlg->AppPath, buf);
	}

	if (pstr[0]=="IDENTIFY") 
	{
		outStr = "SUCCESS ACE_PLAYER";
	}
	else if (pstr[0]=="PREPARE")
	{
		//As of Oct-28-2010, level input from the controller is no longer used in the presenter.
		for (wavName="", i=2; i<nArg; i++) wavName += pstr[i]; // check file names containing blank character

		res=hMainWndDlg->Prepare(&seq, wavName.c_str(), dummy, outStr);
	}
	else if (pstr[0]=="PRESENT") 
	{
		res = hMainWndDlg->Present(&seq, outStr);
	}
	else if (pstr[0]=="SET") 
	{
		if (!(res = hMainWndDlg->Set(pstr, nArg, outStr))) { MessageBox (hMainDlg, outStr, "", MB_OK); return ; }
	}
	else
	{
		MessageBox (hMainDlg, "Unknown command", "", MB_OK);
	}
	WriteFile (hPipe, outStr.c_str(), outStr.GetLength()+1, &dw, NULL);
	SendMessage (GetDlgItem(hMainDlg, IDC_STATUSBAR), SB_SETTEXT, 1, (LPARAM)outStr.c_str());
	EditPrintf (GetDlgItem(hPipeLog, IDC_MSG), "(outgoing) %s\r\n", outStr.c_str());
}

void pipeThread (PVOID nsr)
{
	CStdString parsedMsg[MAX_WORDS_PIPEMSG];
	char *pipeNameStr = (char *)nsr;
	DWORD dw, ecode;
	HANDLE hPipe;
	int res, nArg;
	char PipeName[256], buffer[MAX_PATH], errstr[MAX_PATH], inBuf[MAX_PATH*4], inBuf2[MAX_PATH*4];
	HWND hDlg = hMainDlg;

	wsprintf(PipeName, "\\\\.\\pipe\\%s", pipeNameStr);

	if ((hPipe=InitPipe(hDlg, PipeName, errstr))==NULL)
	{ MessageBox (hDlg, errstr, "pipeThread", MB_OK);		_endthread();		return;	}

	_beginthread (pipeThread, 0, (void*)ACEPLAYER_CONSOLE_PIPENAME);

	while (1)
	{
		if (!ConnectNamedPipe (hPipe, NULL))
		{
			if (GetLastError() != ERROR_PIPE_CONNECTED)
			{	GetLastErrorStr(errstr);
				MessageBox (hDlg, "ERROR in ConnectNamedPipe()", errstr, MB_OK);
				_endthread();				return;			}
		}
		res = ReadFile (hPipe, inBuf, sizeof(inBuf), &dw, NULL);	inBuf[dw]='\0';
		if (!res)
		{
			 
			if ((ecode=GetLastError())==234)  /*More data is available.*/
			{
				ReadFile (hPipe, inBuf2, sizeof(inBuf), &dw, NULL);
				strcat(inBuf, inBuf2);
			}
			else
			{
				sprintf(buffer, "Erorr code=%d", ecode);
				MessageBox (hDlg, "ReadFile fails for pipe communication", buffer, MB_OK);
				break;
			}
		}
		nArg = str2array (parsedMsg, MAX_WORDS_PIPEMSG, inBuf, " ");
		SendMessage (GetDlgItem(hMainDlg, IDC_STATUSBAR), SB_SETTEXT, 1, (LPARAM)inBuf);
		EditPrintf (GetDlgItem(hPipeLog, IDC_MSG), "(incoming) %s\r\n", inBuf);
		ProcessParsedString (hPipe, parsedMsg, nArg);
		DisconnectNamedPipe(hPipe); // For Stateless named pipe connection, turn this line on.
		FlushFileBuffers(hPipe);
	}
}

