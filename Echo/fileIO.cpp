#include "Echo.h"
#include "EchoMainWnd.h"

int CEchoMainWnd::ReadSessFile (char* errStr)
{
	string strbuf;
	int rc;

	if ((rc=ReadINI(errStr, sessFileNameWPath, "NAME", strbuf)) >= 0)
		triml(strbuf, " ");
	else if (rc == -2 /* file open error */ || rc == -3 /* file read error */)
		return -1;
	// else go on ignoring error (heading not found, etc.)
	
	strcpy(ss->sessionName, strbuf.c_str());
	SCANSTR2("DIRECTORY", strbuf); triml(strbuf, " ");
	strcpy(ss->wavePath,  strbuf.c_str());
	if (ss->wavePath[strlen(ss->wavePath)-1]!='\\')
		strcat(ss->wavePath, "\\");
	SCANSTR2("NAME RULE", strbuf); triml(strbuf, " ");
	strcpy(ss->nameRule, strbuf.c_str());
	SCANSTR2("TOKENS", strbuf); triml(strbuf, " ");
	ss->nToken = str2array (ss->token, 24, strbuf.c_str(), " ") ;
	for (int i=0; i<MAX_NUM_TOKEN; i++)
		memset(ss->tokenExample[i], 0, MAX_LEN_TOKEN_EXAMPLE);
	if (SCANSTR2("TOKEN EXAMPLES",  strbuf)>0)
	{
		triml(strbuf, " ");
		str2array (ss->tokenExample, 24, strbuf.c_str(), " ") ;
	}
	else
		for (int i=0; i<MAX_NUM_TOKEN; i++) ss->tokenExample[i][0]=0;

	return isConsonantGeneral(); //ss and consTesting are updated.
// returns 1 for consonant testing 0 for general
}

 
int CEchoMainWnd::WriteSessFile (char* errStr)
{
	int i;
	char buf[512];

	GetDlgItemText (IDC_SESSION_NAME, buf, sizeof(buf));
	strcpy(ss->sessionName, buf);
	CHEC(printfINI(errStr, sessFileNameWPath, "NAME", "%s\r\n", ss->sessionName))
	CHEC(printfINI(errStr, sessFileNameWPath, "DIRECTORY", "%s\r\n", ss->wavePath))
	CHEC(printfINI(errStr, sessFileNameWPath, "NAME RULE", "%s\r\n", ss->nameRule))
	buf[0]='\0';
	for (i=0; i<ss->nToken; i++)
	{
		strcat (buf, ss->token[i]);
		strcat (buf, " ");
	}
	CHEC(printfINI(errStr, sessFileNameWPath, "TOKENS", "%s\r\n", buf))
	return 0;
}

int CEchoMainWnd::isConsonantGeneral()
{ // global variables affected: ss and consTesting
	char buffer[256], buf[256], *temp[2];
	int i, j;
	char codedTokensBuf[MAX_NUM_TOKEN][MAX_LEN_TOKEN];
	char *codedTokens[MAX_NUM_TOKEN];

	// Finding out whether the tokens from the file are for consonant testing or not.
	temp[0] = buffer; temp[1]=buf;
	for (i=0; i<MAX_NUM_TOKEN; i++)
	{
		codedTokens[i]=codedTokensBuf[i];
		LoadString (hInst, IDS_TOKEN1+i, codedTokens[i], 64);
		//token only (taking out the example)
		str2strarray (temp, 2, codedTokens[i], " ");
		strcpy(codedTokens[i], temp[0]);
	}
	for (i=0; i<ss->nToken; i++)
	{
		int comp=1;
		for (j=0; j<NUM_CODED_TOKENS; j++)
			comp *= strcmp(ss->token[i], codedTokens[j]);
		if (comp!=0)
		{
			consTesting = 0;
			return 0;
		}
	}
	consTesting = 1;
	return 1;
}

int CEchoMainWnd::ReadINI_UpdateScreen()
{
	string strbuf;
	char iniFile[512], errStr[256];
	int i, res, len=512;

	sprintf (iniFile, "%s%s", AppPath, INI_NAME);
	// if session file indiated by the "speechtesting.ini" file exists, the other parameters ([SESSION NAME], [WAVE PATH], [NAME RULE], [TOKEN]) are ignored
	// if the file doesn't exist, retrieve the parameters (if available), otherwise, just ignore and proceed the application.
	SCANSTR("SESSION FILE", strbuf)
	{
		strcpy(sessFileNameWPath, strbuf.c_str());
		trimLeft(sessFileNameWPath, " ");
		res = ReadSessFile (errStr);
		if (res==1) // consonant testing
			SendDlgItemMessage (IDC_CONS, BM_CLICK);
		else if (res==0)
			SendDlgItemMessage (IDC_GENERAL, BM_CLICK);
		else // -1.... ReadWriteINI failed.
		{
			if (strbuf.length()>0)
				MessageBox (errStr, "Cannot read");
			return 1;
		}
	}
	SCANSTR("NODE", strbuf)
	{
		triml(strbuf, ' ');
		SetDlgItemText (IDC_NODELIST, strbuf.c_str());
	}
	SCANITEM("LEVEL", "%d", i)
	{
		SetDlgItemInt (IDC_LEVEL, i, 1);  //SIGNED
		SCANSTR("ADDITIONAL PROC", strbuf) {
			SetDlgItemText (IDC_PROC, strbuf.c_str());
		}
	}
	SCANITEM("ACOUSTIC PRESENTATION", "%d", i)
		SendDlgItemMessage (IDC_ACOUSTIC, BM_SETCHECK, (i) ? BST_CHECKED:BST_UNCHECKED);
	SCANITEM("FEEDBACK", "%d", i)
		SendDlgItemMessage (IDC_FEEDBACK, BM_SETCHECK, (i) ? BST_CHECKED:BST_UNCHECKED);
	SCANITEM("SINGLE RESPONSE", "%d", i)
		SendDlgItemMessage (IDC_1RESPONSE, BM_SETCHECK, (i) ? BST_CHECKED:BST_UNCHECKED);
	SCANITEM("ALLOW SAME TOKEN", "%d", i)
		SendDlgItemMessage (IDC_ALLOWSAMETOKEN, BM_SETCHECK, (i) ? BST_CHECKED:BST_UNCHECKED);
	SCANSTR("RESULT PATH", strbuf)
		strcpy(resultPath, strbuf.c_str());
	SCANITEM("SAMPLE RATE", "%d", i)
		Fs = i;
	SCANSTR("UDFPATH", strbuf)
		AstSig.SetPath("aux", strbuf.c_str());
	report.ReadINI(iniFile);
	if (ss->nToken>0)
		SendMessage(WM__ORG_SESS_SCREEN, (WPARAM)sessFileNameWPath, PREV_SESSION_LOADED);
	return 1;
}

void CEchoMainWnd::GetDefaultParameter(char *ItemName)
{
	if (!strcmp(ItemName,"LEVEL")) SetDlgItemInt (IDC_LEVEL, 70, 1); 
	else if (!strcmp(ItemName,"ACOUSTIC PRESENTATION"))		SendDlgItemMessage (IDC_ACOUSTIC, BM_SETCHECK, BST_CHECKED);
	else if (!strcmp(ItemName,"FEEDBACK"))		SendDlgItemMessage (IDC_FEEDBACK, BM_SETCHECK, BST_CHECKED);
	else if (!strcmp(ItemName,"SINGLE RESPONSE"))		SendDlgItemMessage (IDC_1RESPONSE, BM_SETCHECK, BST_UNCHECKED);
	else if (!strcmp(ItemName,"ALLOW SAME TOKEN"))		SendDlgItemMessage (IDC_ALLOWSAMEWAV, BM_SETCHECK, BST_UNCHECKED);
	else if (!strcmp(ItemName,"NODE")) SetDlgItemText (IDC_NODELIST, ".");
	else if (!strcmp(ItemName,"SAMPLE RATE")) Fs = 16000;
	else if (!strcmp(ItemName,"UDFPATH"))	AstSig.SetPath("aux", AppPath);
}

int CEchoMainWnd::UpdateINI(char *ssfname)
{
	CString buf;
	char errStr[256], iniFile[MAX_PATH];
	int i;
	sprintf (iniFile, "%s%s", AppPath, INI_NAME);
	if (ssfname[0]=='\0')
		return 0;
	else
		CHEC(printfINI(errStr, iniFile, "SESSION FILE", "%s", ssfname))
	GetDlgItemText (IDC_NODELIST, buf);
	CHEC(printfINI(errStr, iniFile, "NODE", "%s", buf))
	GetDlgItemText (IDC_LEVEL, buf); 
	CHEC(printfINI(errStr, iniFile, "LEVEL", "%s", buf))
	GetDlgItemText (IDC_PROC, buf); 
	CHEC(printfINI(errStr, iniFile, "ADDITIONAL PROC", "%s", buf))
	i = (SendDlgItemMessage (IDC_ACOUSTIC, BM_GETCHECK) == BST_CHECKED);
	CHEC(printfINI(errStr, iniFile, "ACOUSTIC PRESENTATION", "%d", i))
	i = (SendDlgItemMessage (IDC_FEEDBACK, BM_GETCHECK) == BST_CHECKED);
	CHEC(printfINI(errStr, iniFile, "FEEDBACK", "%d", i))
	i = (SendDlgItemMessage (IDC_1RESPONSE, BM_GETCHECK) == BST_CHECKED);
	CHEC(printfINI(errStr, iniFile, "SINGLE RESPONSE", "%d", i))
	i = (SendDlgItemMessage (IDC_ALLOWSAMETOKEN, BM_GETCHECK) == BST_CHECKED);
	CHEC(printfINI(errStr, iniFile, "ALLOW SAME TOKEN", "%d", i))
	CHEC(printfINI(errStr, iniFile, "RESULT PATH", "%s", resultPath))
	return 1;
}