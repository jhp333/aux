#include "Echo.h"
#include "EchoMainWnd.h"

char PipeName[128];

BOOL CALLBACK PresFailProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	int i, res;
	char buf[MAX_PATH];
	switch (umsg)
	{
	case WM_INITDIALOG:
		ShowWindow (hDlg, 0);
		res=0;
		for (i=0; res!=CB_ERR; i++)
		{
			res=SendDlgItemMessage (GetParent(hDlg), IDC_NODELIST, CB_GETLBTEXT, i, (LPARAM)buf);
			if (res!=CB_ERR)
				SendDlgItemMessage (hDlg, IDC_NODELIST, CB_ADDSTRING, 0, (LPARAM)buf);
		}
		i=::SendDlgItemMessage (GetParent(hDlg), IDC_NODELIST, CB_GETCURSEL, 0, 0);
		SendDlgItemMessage (hDlg, IDC_NODELIST, CB_SETCURSEL,	i, 0);
		GetDlgItemText (GetParent(hDlg), IDC_PRESENT_CONTROL, buf, sizeof(buf));
		SetDlgItemText (hDlg, IDC_PRESENT_CONTROL, buf);
		break;
	case WM_COMMAND:
		if (wParam==WM_DESTROY)			ShowWindow (hDlg, 0);
		else if (LOWORD(wParam)==IDOK)
		{
			GetDlgItemText (hDlg, IDC_NODELIST, buf, sizeof(buf));
			SetDlgItemText (GetParent(hDlg), IDC_NODELIST, buf);
			wsprintf(PipeName, "\\\\%s\\pipe\\%s", buf, INTERFACE_PIPENAME);
			GetDlgItemText (hDlg, IDC_PRESENT_CONTROL, buf, sizeof(buf));
			SetDlgItemText (GetParent(hDlg), IDC_PRESENT_CONTROL, buf);
			EndDialog(hDlg, 1);
		}
		else if (LOWORD(wParam)==IDCANCEL)		EndDialog(hDlg, 0);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

int CEchoMainWnd::ReadLineandWav(const char *str, char *errStr)
{
	char buf[MAX_PATH];
	CString AuxDef = "";
	int nItems = countDeliminators(str, " ");
	string *tokstr = new string[nItems];
	str2array(tokstr, nItems, str, " ");

	for (int i=0; i<nConc; i++)
	{
		//if (multi && nConc==1)	trimr(tokstr[i],"0123456789 ");
		FulfillFile (buf, ss->wavePath, tokstr[i].c_str());
		CString idx="";
		if (nConc!=1)
			idx.Format("%d", i+1);
		AuxDef += "TG" + idx + "=wave(\"" + CString(buf) + "\")\n";
		AuxDef += "TGNAME" + idx + "=\"" + CString(tokstr[i].c_str()) + "\"\n";
	}
	delete[] tokstr;
	try {
		CAstSig ast(AuxDef, &AstSig);
		ast.Compute();
	} catch (const char *errmsg) {
		MessageBox(errmsg, "AUX Definition");
		return 0;
	}
	return nConc;
}

bool CEchoMainWnd::PrepareAUX()
{
	CString longbuf;
	UINT res = GetDlgItemText(IDC_PROC, longbuf);
	if (res==0) longbuf = "TG";
	try {
		if (multi && hMulti->TrialVarNames.size()>0)
			hMulti->setNth(idAddProcs[trial], hMulti->TrialVarNames, hMulti->TrialVarValues);
		AstSig.SetNewScript(longbuf);
		Sig = AstSig.Compute();
	} catch (const char *errmsg) {
		MessageBox(errmsg, "AUX Processing");
		return false;
	}
	return true;
}

int CEchoMainWnd::PlayStim()
{
	char errbuf[256], buf[256];
	double level2, level3;
	CString NodeName, StimPathFile;

	//evaluate the signal to present
	if (!PrepareAUX())
		return 0;
	GetDlgItemText(IDC_NODELIST, NodeName);
	StimPathFile = ((NodeName == ".") ? CString(AppPath).Left(2) : "\\\\"+NodeName) + STIM_PATH_FILE;
	//signal ready. 
	if (Sig.Wavwrite(StimPathFile, buf) > 0) {
		int b;
		level = GetDlgItemDouble(hDlg, IDC_LEVEL, &b);
		if (b) {
			level2 = level - LeveldBrms(StimPathFile, &level3, buf); 
			if (PreparePresent(level2, errbuf))
				return 1;
		} else
			sprintf(errbuf, "Failed to get the level value.");
	} else {
		GetLastErrorStr(errbuf);
		sprintf(errbuf+strlen(errbuf), "Error in Wavwrite %s\n%s", StimPathFile, buf);
	}
	MessageBox(errbuf, "PlayStim");
	return 0;
}

int CEchoMainWnd::PreparePresent(double level2, char *errstr)
{
	char reply[512], PipeName[128], command[376];
	CString NodeName, StimPathFile;
	string strbuffer[2];
	int res, nItems;
	DWORD nRead;
	reply[0]=0;
	GetDlgItemText(IDC_NODELIST, NodeName);
	sprintf(PipeName, "\\\\%s\\pipe\\%s", NodeName, INTERFACE_PIPENAME);
	StimPathFile = ((NodeName == ".") ? CString(AppPath).Left(2) : "\\\\"+NodeName) + STIM_PATH_FILE;
	sprintf (command, "PREPARE %5.1f %s", level2, StimPathFile);
	DISP_PIPE_LOG("[out]",command);
	// temporary code for a secondary presenter. 9/1/2010 jhpark
		char PipeName2[128];
		sprintf(PipeName2, "\\\\%s\\pipe\\%s2", NodeName, INTERFACE_PIPENAME);
		res = CallNamedPipe(PipeName2, (LPVOID)(LPCTSTR)command, (DWORD)strlen(command)+1,
			preparedCondition, MAX_PROCESSING_CONDITION_STRING, &nRead, NMPWAIT_WAIT_FOREVER);
	// end temporary code
	res = CallNamedPipe(PipeName, (LPVOID)(LPCTSTR)command, (DWORD)strlen(command)+1,
		preparedCondition, MAX_PROCESSING_CONDITION_STRING, &nRead, NMPWAIT_WAIT_FOREVER);
	while (!res)
	{
		GetLastErrorStr(reply);
		sprintf (errstr, "Error in CallNamedPipe (PREPARE)\nmsg: %s", reply);
		if (MessageBox (errstr, "Retry or cancel? (you can still retry after changing settings)", MB_RETRYCANCEL)==IDRETRY)
			res = CallNamedPipe(PipeName, (LPVOID)(LPCTSTR)command, (DWORD)strlen(command)+1,
					preparedCondition, MAX_PROCESSING_CONDITION_STRING, &nRead, NMPWAIT_WAIT_FOREVER);
		else
			return 0;
	}
	preparedCondition[nRead]='\0';
	DISP_PIPE_LOG("[in]",preparedCondition);	
	nItems=str2array(strbuffer, 2, preparedCondition, " ");
	if (nItems>0 && strbuffer[0] =="SUCCESS") {
		if (nItems==1)
			preparedCondition[0] = '\0';
		else
			memmove(preparedCondition, preparedCondition+8, nRead-7);
	} else {		
		sprintf (errstr, "FAILURE during PREPARE --> %s", preparedCondition);
		return 0;
	}

	strcpy(command, "PRESENT");
	DISP_PIPE_LOG("[out]",command);
	// temporary code for a secondary presenter. 9/1/2010 jhpark
		res = CallNamedPipe(PipeName2, (LPVOID)(LPCTSTR)command, (DWORD)strlen(command)+1,
			preparedCondition, MAX_PROCESSING_CONDITION_STRING, &nRead, NMPWAIT_WAIT_FOREVER);
	// end temporary code
	res = CallNamedPipe(PipeName, (LPVOID)(LPCTSTR)command, (DWORD)strlen(command)+1,
		reply, sizeof(reply), &nRead, NMPWAIT_WAIT_FOREVER); 
	while (!res)
	{
		GetLastErrorStr(errstr);
		if(MessageBox ("CallNamedPipe failed(PRESENT). Retry(Yes), or Abort(No)?", errstr, MB_YESNO)==IDYES)
		{
			res = CallNamedPipe(PipeName, (LPVOID)(LPCTSTR)command, (DWORD)strlen(command)+1,
				reply, sizeof(reply), &nRead, NMPWAIT_WAIT_FOREVER);
		}
		else
		{
			return 0;
		}
	}
	reply[nRead]='\0';
	DISP_PIPE_LOG("[in]",reply);
	nItems=str2array(strbuffer, 2, reply, " ");
	if (nItems>0 && strbuffer[0] =="SUCCESS") {
		if (SendDlgItemMessage (IDC_ACOUSTIC, BM_GETCHECK)==BST_CHECKED) {
			CSignals x(StimPathFile);
			if (!x.PlayArray(errstr)) {
				sprintf(reply, "Playarray error: %s", errstr);
				MessageBox (reply);
			}
		}
	} else {		
		sprintf (errstr, "FAILURE during PRESENT --> %s", reply);
		return 0;
	}
	return 1;
}
