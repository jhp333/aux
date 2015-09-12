#include "token.h"
#include "tokenDlg.h"
#include <time.h>

#define NUM_KEYS	15
#define FOPEN(NAME1,NAME2) \
if ((hFile = CreateFile (NAME1, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))==INVALID_HANDLE_VALUE)\
{	CStdString errmsg; GetLastErrorStr(errmsg); MessageBox (errmsg + NAME2, "Cannot create a temporary file for intermediate result", MB_ICONERROR); }\
if ((hFile2 = CreateFile (NAME2, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL))==INVALID_HANDLE_VALUE)\
{	CStdString errmsg; GetLastErrorStr(errmsg); MessageBox (errmsg + NAME2, "Cannot create a log file", MB_ICONERROR); }\

void uploadLogFile(void *fname);

int timeStamp ( char* dateStr, char* timeStr)
{
	if ( (dateStr==NULL) || (timeStr==NULL) )
		return 0;
    _strdate( dateStr );
    _strtime( timeStr );
	return 1;
}

void CTokenDlg::OnCommand(int idc, HWND hwndCtl, UINT event)
{
	static char logname[MAX_PATH], tempFilename[MAX_PATH];
	static HANDLE hFile, hFile2;
	static	UINT fileCreatedNumber;
	static int nCorrCum, nTotalsCum;

	char errstr[256], buf[256], fname[MAX_PATH], fullfname[MAX_PATH];
	DWORD gle, dw;
	CAstSig ord;
	CSignals order, dbclicked;
	string parse[3]; 
	CStdString resp, str2;
	LRESULT res;
	struct tm *newtime;

	int k, _fs, nCorr, nTotals;

	switch (idc)
	{
	case IDC_AUX:
		if (event==EN_KILLFOCUS)
		{
			if (strlen(tokManager.name)>0)
			{
				GetDlgItemText(IDC_AUX, buf, sizeof(buf));
				if (strlen(buf)>0) {	EnableDlgItem(hDlg, IDC_GO, 1); break;}
			}
			EnableDlgItem(hDlg, IDC_GO, 0); 
		}
		break;
	case IDC_STOP:
		EnableDlgItem(hDlg, IDC_GO, 1);
		EnableDlgItem(hDlg, IDC_STOP, 0);
		EnableDlgItem(hDlg, IDC_OPEN_SESSION, 1);
		EnableDlgItem(hDlg, IDC_FS, 1);
		EnableDlgItem(hDlg, IDC_SUBJ, 1);
		EnableDlgItem(hDlg, IDC_OPEN_DEFINITION, 1);
		EnableDlgItem(hDlg, IDC_AUX, 1);
		EnableDlgItem(hDlg, IDC_LEVEL, 1);
		EnableDlgItem(hDlg, IDC_NODELIST, 1);			
		EnableDlgItem(hDlg, IDC_RANDOMIZE, 1);			
		break;
	case IDC_GO:
		if (GetDlgItemText(IDC_SUBJ, resp)==0) {
			MessageBox ("Please type in subejct ID");
			break;
		}
		if (!fs_ready) {
			MessageBox ("Resampling required (or in progress).\nUpdate sample rate and retry or wait till it is complete.");
			break;
		}
		EnableDlgItem(hDlg, IDC_GO, 0);
		EnableDlgItem(hDlg, IDC_STOP, 1);
		::ShowWindow(GetDlgItem(IDC_STOP), SW_SHOW);
		EnableDlgItem(hDlg, IDC_CORRECT, 1);
		while(SendDlgItemMessage(IDC_SESSION_ORDER, LB_DELETESTRING, 0)!=LB_ERR) {};
		if (SendDlgItemMessage (IDC_RANDOMIZE, BM_GETCHECK, 0, 0)==BST_CHECKED)
			sprintf(buf, "randperm(%d)-1", tokManager.len);
		else
			sprintf(buf, "[1:%d]-1", tokManager.len);
		ord.SetNewScript(buf);
		order = ord.Compute();
		for (int j, i=0; i<tokManager.len; i++) 
		{
			j = tokManager.order[i] = (int)order.buf[i];
			_splitpath (tokManager.file[j].c_str(), NULL, NULL, fname, buf);
			sprintf(buf, "%d: %s ", i+1, fname);
			for (int m=0; m<tokManager.line[tokManager.order[i]].nItems; m++) 
			{
				str2.Format("%s ", tokManager.line[tokManager.order[i]].item[m].c_str());
				strcat(buf, str2.c_str());
			}
			SendDlgItemMessage(IDC_SESSION_ORDER, LB_ADDSTRING, 0, (LPARAM)buf);
		}
		fileCreatedNumber = GetTempFileName(AppPath, "token", 0, tempFilename);
		EnableDlgItem(hDlg, IDC_CORRECT, 0);
		nCorrCum = nTotalsCum = 0;
		trial = -1;
		//Record in the log file.
        __time64_t long_time;
        _time64( &long_time );		
		newtime = _localtime64( &long_time );
		sprintf(buf, "tokenlog%d-%d-%d.txt", newtime->tm_mon+1, newtime->tm_mday, newtime->tm_year+1900);
		FulfillFile(logname, AppPath, buf);
		OnCommand(IDC_NEXT, GetDlgItem(IDC_NEXT), BN_CLICKED);
		EnableDlgItem(hDlg, IDC_OPEN_SESSION, 0);
		EnableDlgItem(hDlg, IDC_FS, 0);
		EnableDlgItem(hDlg, IDC_SUBJ, 0);
		EnableDlgItem(hDlg, IDC_OPEN_DEFINITION, 0);
		EnableDlgItem(hDlg, IDC_AUX, 0);
		EnableDlgItem(hDlg, IDC_LEVEL, 0);
		EnableDlgItem(hDlg, IDC_NODELIST, 0);
		EnableDlgItem(hDlg, IDC_RANDOMIZE, 0);
		break;

	case IDC_CORRECT:
		EnableDlgItem(hDlg, IDC_NEXT, 1);
		k = tokManager.line[tokManager.order[trial]].nItems;
		if (SendDlgItemMessage(IDC_CORRECT, BM_GETCHECK)==BST_CHECKED) // "ALL" checked
			for (int i=0; i<k; i++)		SendDlgItemMessage(IDC_KEY1+i, BM_SETCHECK, BST_CHECKED);
		else
			for (int i=0; i<k; i++)		SendDlgItemMessage(IDC_KEY1+i, BM_SETCHECK, BST_UNCHECKED);
		break;

	case IDM_PIPEVIEW:
		ShowWindow (hPipeMsgViewDlg,1);
		break;

	case IDC_UPDATE:
		_fs = GetDlgItemInt (IDC_FS, &k);
		if (_fs*k==0)
			fs=0;
		else {
			fs = _fs;
			GetDlgItemText(IDC_SESSION_PATH, str2);
			OnSession(str2.c_str());
		}
		EnableDlgItem(hDlg, IDC_UPDATE, 0);
		break;

	case IDC_FS:
		if (event==EN_UPDATE)
		{
			_fs = GetDlgItemInt (IDC_FS, &k);
			if (fs!=_fs)
			{
				if (_fs*k==0)
					fs=0;
				else 
				{
					fs = _fs;
					EnableDlgItem(hDlg, IDC_UPDATE, 1);
					fs_ready = false;
				}
			}
		}
		break;

	case IDC_NEXT:
		EnableDlgItem(hDlg, IDC_NEXT, 0);
		SendDlgItemMessage(IDC_CORRECT, BM_SETCHECK, BST_UNCHECKED);
		trial++;
		SendDlgItemMessage(IDC_SESSION_ORDER, LB_SETCURSEL, trial);
		FOPEN(tempFilename, logname)
		SetFilePointer(hFile, NULL, NULL, FILE_END); SetFilePointer(hFile2, NULL, NULL, FILE_END);
		//For the first trial, prepare the header for log file
		if (trial==0)
			PrepareDataFileHeading(hFile, hFile2);
		else {	// evaluate the last response
			EvalResponse(hFile, hFile2, trial-1, nCorr, nTotals);
			nTotalsCum += nTotals;
			nCorrCum += nCorr;
			SetDlgItemInt(IDC_CORR_SOFAR, nCorrCum);
			SetDlgItemInt(IDC_TOTAL_TOKENS, nTotalsCum);
		}
		if (trial==tokManager.len)
		{
			CloseHandle(hFile); CloseHandle(hFile2);
			for (int i=0; i<nTotals; i++)		SendDlgItemMessage(IDC_KEY1+i, BM_SETCHECK, BST_UNCHECKED);
			for (int i=0; i<NUM_KEYS; i++) ::ShowWindow(GetDlgItem(IDC_KEY1+i), SW_HIDE);
			SendDlgItemMessage(IDC_CORRECT, BM_SETCHECK, BST_UNCHECKED);
			EnableDlgItem(hDlg, IDC_GO, 1);
			EnableDlgItem(hDlg, IDC_STOP, 0);
			EnableDlgItem(hDlg, IDC_OPEN_SESSION, 1);
			EnableDlgItem(hDlg, IDC_FS, 1);
			EnableDlgItem(hDlg, IDC_SUBJ, 1);
			EnableDlgItem(hDlg, IDC_OPEN_DEFINITION, 1);
			EnableDlgItem(hDlg, IDC_AUX, 1);
			EnableDlgItem(hDlg, IDC_LEVEL, 1);
			EnableDlgItem(hDlg, IDC_NODELIST, 1);
			EnableDlgItem(hDlg, IDC_RANDOMIZE, 1);
			SaveResult (tempFilename);
			break;
		}
		for (int i=0; i<NUM_KEYS; i++)  SendDlgItemMessage(IDC_KEY1+i, BM_SETCHECK, BST_UNCHECKED);
		SetDlgItemText(IDC_RESPONSE, "");
		for (int i=0; i<NUM_KEYS; i++) ::ShowWindow(GetDlgItem(IDC_KEY1+i), SW_HIDE);
		for (int i=0; i<tokManager.line[tokManager.order[trial]].nItems; i++)
		{
			::ShowWindow(GetDlgItem(IDC_KEY1+i), SW_SHOW);
			SetDlgItemText(IDC_KEY1+i, tokManager.line[tokManager.order[trial]].item[i].c_str());
		}
		::UpdateWindow(hDlg);
		if (!PlayStim(trial, errstr))
			MessageBox(errstr);
		{
			string LogStr = AstSig.GetTag("LOGSTR").string();
			sprintf(buf, "%d: %s\r\n", trial+1, LogStr.c_str());
		}
		WriteFile(hFile, buf, (DWORD)strlen(buf), &dw, NULL);
		WriteFile(hFile2, buf, (DWORD)strlen(buf), &dw, NULL);
		CloseHandle(hFile); CloseHandle(hFile2);
		EnableDlgItem(hDlg, IDC_CORRECT, 1);
		break;
	case IDC_SESSION_ORDER:
		if (event==LBN_DBLCLK)
		{
			if ((res = SendDlgItemMessage(IDC_SESSION_ORDER, LB_GETCURSEL))!=LB_ERR)
			{
				
				SendDlgItemMessage(IDC_SESSION_ORDER, LB_GETTEXT, (WPARAM)res, (LPARAM)buf);
				/*
				str2array(parse, 3, buf, " ");
				_splitpath (tokManager.file[0].c_str(), buf, zstr, fname, errstr);
				str2.Format("%s%s%s%s", buf, zstr, parse[1].c_str(), errstr);
				if (!dbclicked.Wavread(str2.c_str(), buf))
				{
					MessageBox(buf);
					break;
				}
				if (fs>0 && dbclicked.GetFs()!=fs) 
				{
					if (!dbclicked.Resample(fs, errstr))
					{ res=0; break; }
				}
				if (!PlayStim(&dbclicked, errstr))
				*/
				str2array(parse, 2, buf, ":");
				if (!PlayStim(atoi(parse[0].c_str())-1, errstr))
				{
					MessageBox(errstr);
					break;
				}
			}
		}
		break;
	case IDC_OPEN_SESSION:
		fname[0]='\0';fullfname[0]='\0';
		if (!fdlg.FileOpenDlg(fullfname, fname, "text file (*.TXT)\0*.txt\0", "txt"))
		{
			gle = GetLastError();
			if (gle!=0)
			{
				sprintf(errstr, "GetOpenFileName failed. code=%d", gle);
				MessageBox (errstr, "Token", MB_OK);	
			}
		}
		else
		{
			if (OnSession(fullfname))
				SetDlgItemText(IDC_SESSION_PATH, fullfname);
		}
		break;
	case IDC_OPEN_DEFINITION:
		fname[0]='\0';fullfname[0]='\0';
		if (!fdlg.FileOpenDlg(fullfname, fname, "text file (*.TXT)\0*.txt\0", "txt"))
		{
			gle = GetLastError();
			if (gle!=0)
			{
				sprintf(errstr, "GetOpenFileName failed. code=%d", gle);
				MessageBox (errstr, "Token", MB_OK);	
			}
		}
		else
		{
			SetDlgItemText(IDC_DEFINITION_PATH, fullfname);
			OnDef(fullfname);		
		}
		break;
	case IDM_DESTROY:
		SendMessage (WM_CLOSE);
		break;

	default:
		if (IDC_KEY1 <= idc && idc < IDC_KEY1 + NUM_KEYS) {
			EnableDlgItem(hDlg, IDC_NEXT, 1);
			for (k=0; k<NUM_KEYS; k++)
			{
				if (SendDlgItemMessage(IDC_KEY1+k, BM_GETCHECK)==BST_CHECKED)
				{
					EnableDlgItem(hDlg, IDC_CORRECT, 0);
					break;
				}
			}
			if (k==NUM_KEYS)
				EnableDlgItem(hDlg, IDC_CORRECT, 1);
		}
	}
}

int CTokenDlg::PlayStim(int idx, char *estr)
{
	static char prepInfo[MAX_PROCESSING_CONDITION_STRING] = "";
	char errbuf[256], fnamewpath[256], buf[256];
	int b, res(0);
	double level2, level3;
	CStdString cstrUdfPath;

	GetDlgItemText(IDC_UDFPATH, cstrUdfPath);
	AstSig.SetPath("", cstrUdfPath);
	AstSig.Reset(fs);

	int ix = tokManager.order[idx];

	CStdString AuxIn;
	AuxIn.AppendFormat("TRIAL_ID=%d\n", idx+1);
	AuxIn.AppendFormat("TOTAL_TRIALS=%d\n", tokManager.len);
	AuxIn.AppendFormat("TOTAL_TOKENS=%d\n", tokManager.line[ix].nItems);
	for (int i=0; i<tokManager.line[ix].nItems; ++i)
		AuxIn.AppendFormat("TOKEN{%d}=\"%s\"\n", i+1, tokManager.line[ix].item[i].c_str());
	AuxIn.AppendFormat("PRESET=\"%s\"\n", prepInfo);
	AuxIn.AppendFormat("TGNAME=\"%s\"\n", tokManager.file[ix].c_str());
	AstSig.SetTag("TG", tokManager.sig[ix]);
	AstSig.SetRef("TARGET", "TG");
	//evaluate the signal to present
	if (Prepare(AuxIn.c_str(), estr))
	{
		CStdString NodeName;
		if (GetDlgItemText (IDC_NODELIST, NodeName)==0)
		{
			SendDlgItemMessage(IDC_NODELIST, CB_SETCURSEL, (WPARAM)(LRESULT)SendDlgItemMessage(IDC_NODELIST, CB_FINDSTRINGEXACT, -1, (LPARAM)"."));
			NodeName = ".";
		}
		if (NodeName == ".")
			sprintf(fnamewpath, "%2.2s%s", AppPath, WAVNAME);
		else
			sprintf(fnamewpath, "\\\\%s%s", NodeName.c_str(), WAVNAME);
		//signal ready. 
		if (!AstSig.Sig.Wavwrite(fnamewpath, buf))
		{
			strcpy(estr, buf);
		}
		else
		{
			level = GetDlgItemDouble(hDlg, IDC_LEVEL, &b);
			if (!b)
			{
				strcpy(estr, "Error in reading Level");
			}
			else
			{
				level2 = level - LeveldBrms(fnamewpath, &level3, buf); 
				if (PreparePresent(level2, prepInfo, errbuf))
					res=1;
				else
					strcpy(estr, errbuf);
			}
		}
	}
	return res;
}

int CTokenDlg::PreparePresent(double level2, char *prepInfo, char *errstr)
{
	char fnamewpath[256], reply[512], PipeName[128], command[376];
	CStdString NodeName;
	string strbuffer[2];
	int res, nItems;
	DWORD nRead;
	reply[0]=0;
	if (GetDlgItemText (IDC_NODELIST, NodeName)==0)
	{
		SendDlgItemMessage(IDC_NODELIST, CB_SETCURSEL, (WPARAM)(LRESULT)SendDlgItemMessage(IDC_NODELIST, CB_FINDSTRINGEXACT, -1, (LPARAM)"."));
		NodeName = ".";
	}

	sprintf(PipeName, "\\\\%s\\pipe\\%s", NodeName.c_str(), SIGNAL_INTERFACE_PIPENAME);
	if (NodeName == ".")
		sprintf (command, "PREPARE %5.1f %2.2s%s", level2, AppPath, WAVNAME);
	else
		sprintf (command, "PREPARE %5.1f \\\\%s%s", level2, NodeName.c_str(), WAVNAME);
	DISP_PIPE_LOG("[out]",command);
	res = CallNamedPipe(PipeName, (LPVOID)(LPCTSTR)command, (DWORD)strlen(command)+1,
		prepInfo, MAX_PROCESSING_CONDITION_STRING, &nRead, NMPWAIT_WAIT_FOREVER);
	while (!res)
	{
		GetLastErrorStr(reply);
		sprintf (errstr, "Error in CallNamedPipe (PREPARE)\nmsg: %s", reply);
		if (MessageBox (errstr, "Retry or cancel? (you can still retry after changing settings)", MB_RETRYCANCEL)==IDRETRY)
		{
			res = CallNamedPipe(PipeName, (LPVOID)(LPCTSTR)command, (DWORD)strlen(command)+1,
					prepInfo, MAX_PROCESSING_CONDITION_STRING, &nRead, NMPWAIT_WAIT_FOREVER);
		}
		else
		{
			return 0;
		}
	}
	prepInfo[nRead]='\0';
	DISP_PIPE_LOG("[in]",prepInfo);	
	nItems=str2array(strbuffer, 2, prepInfo, " ");
	if (strbuffer[0] !="SUCCESS") 
	{		
		sprintf (errstr, "FAILURE in PREPARE: %s", prepInfo+strbuffer[0].length());
		return 0;
	}
	else
	{
		if (nItems==1)
			strcpy(prepInfo, ""); //????
		else
		{
			for (int i=0; i<=(int)strlen(prepInfo)-8; i +=8)
				strncpy (prepInfo+i, prepInfo+i+8, 8);
		}
	}

	strcpy(command, "PRESENT");

	DISP_PIPE_LOG("[out]",command);
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
	if (SendDlgItemMessage (IDC_ACOUSTIC, BM_GETCHECK)==BST_CHECKED)
	{
		if (NodeName == ".")
			sprintf(fnamewpath, "%2.2s%s", AppPath, WAVNAME);
		else
			sprintf(fnamewpath, "\\\\%s%s", NodeName.c_str(), WAVNAME);
		PlaySound(fnamewpath, hInst, SND_FILENAME | SND_ASYNC);
	}
	reply[nRead]='\0';
	DISP_PIPE_LOG("[in]",reply);
	nItems=str2array(strbuffer, 2, reply, " ");
	if (strbuffer[0] !="SUCCESS") 
	{		
		if(MessageBox ("Fail to PRESENT. Retry(Yes), or Abort(No)?", reply, MB_YESNO)==IDYES)
		{
			return PreparePresent(level2, prepInfo, errstr);
		}
		else
		{
			strcpy(errstr, "Present failed by presenter");
			return 0;
		}
	}
	return 1;
}

void CTokenDlg::EvalResponse(HANDLE hFile, HANDLE hFile2, int trial, int &nCorr, int &nTotals)
{
	DWORD dw;
	char buf[256];
	CStdString str, str2;
	string rcum("");
	nCorr=0;
	nTotals = tokManager.line[tokManager.order[trial]].nItems;
	for (int i=0; i<nTotals; i++)
	{
		if (SendDlgItemMessage(IDC_KEY1+i, BM_GETCHECK)==BST_CHECKED)
		{
			nCorr++;
			sprintf(buf, "%s (O) ", tokManager.line[tokManager.order[trial]].item[i].c_str());
		}
		else
			sprintf(buf, "%s (X) ", tokManager.line[tokManager.order[trial]].item[i].c_str());
		rcum += buf;
	}
	GetDlgItemText(IDC_RESPONSE, str);
	if (str.GetLength()>0)		str2.Format("%s --> %s", rcum.c_str(), str);
	else str2 = rcum.c_str();

	sprintf(buf, "%d of %d correct: %s\r\n", nCorr, nTotals, str2.c_str());
	WriteFile(hFile, buf, (DWORD)strlen(buf), &dw, NULL);
	WriteFile(hFile2, buf, (DWORD)strlen(buf), &dw, NULL);
}

int CTokenDlg::PrepareDataFileHeading(HANDLE hFile, HANDLE hFile2)
{
	CStdString str, str2;
	DWORD dw;
	char buf[9000], date[128], time[128];
	sprintf (buf, "=========================================\r\n");
	WriteFile(hFile, buf, (DWORD)strlen(buf), &dw, NULL);
	WriteFile(hFile2, buf, (DWORD)strlen(buf), &dw, NULL);
	timeStamp(date, time);
	sprintf (buf, "[INFO] %s %s, %s, %s\r\n", applname, versionString, date, time);
	WriteFile(hFile, buf, (DWORD)strlen(buf), &dw, NULL);
	WriteFile(hFile2, buf, (DWORD)strlen(buf), &dw, NULL);
	GetDlgItemText(IDC_SUBJ, str);
	sprintf (buf, "[SUBJECT] %s\r\n", str.c_str());
	WriteFile(hFile, buf, (DWORD)strlen(buf), &dw, NULL);
	WriteFile(hFile2, buf, (DWORD)strlen(buf), &dw, NULL);
	GetDlgItemText(IDC_AUX, str);
	sprintf (buf, "[AUX SCRIPTS] %s\r\n", str.c_str());
	WriteFile(hFile, buf, (DWORD)strlen(buf), &dw, NULL);
	WriteFile(hFile2, buf, (DWORD)strlen(buf), &dw, NULL);
	GetDlgItemText(IDC_LEVEL, str);
	sprintf (buf, "[LEVEL] %s\r\n", str.c_str());
	WriteFile(hFile, buf, (DWORD)strlen(buf), &dw, NULL);
	WriteFile(hFile2, buf, (DWORD)strlen(buf), &dw, NULL);
	GetDlgItemText(IDC_SESSION_PATH, str);
	GetDlgItemText(IDC_SESSION_NAME, str2);
	sprintf (buf, "[SESSION] %s: %s\r\n", str.c_str(), str2.c_str());
	WriteFile(hFile, buf, (DWORD)strlen(buf), &dw, NULL);
	WriteFile(hFile2, buf, (DWORD)strlen(buf), &dw, NULL);
	sprintf (buf, "[DETAILS]\r\n");
	WriteFile(hFile, buf, (DWORD)strlen(buf), &dw, NULL);
	WriteFile(hFile2, buf, (DWORD)strlen(buf), &dw, NULL);
	return 1;
}

int CTokenDlg::SaveResult(char *tempname)
{
	static char fname[MAX_PATH], fullfname[MAX_PATH];
	char *tempbuf, buf[256], errstr[256], date[128], time[128];
	int nCorr, nTotals;
	double score;
	DWORD gle, dw;
	HANDLE hFile, hFile2;
	if (!fdlg.FileSaveDlg(fullfname, fname, "text file (*.TXT)\0*.txt\0", "txt"))
	{
		gle = GetLastError();
		if (gle!=0)
		{
			sprintf(errstr, "FileSaveDlg failed. code=%d", gle);
			MessageBox (errstr, "Token", MB_OK);	
		}
	}
	else
	{
		if ((hFile = CreateFile (fullfname, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL))==INVALID_HANDLE_VALUE)
		{
			gle = GetLastError();
			sprintf(errstr, "CreateFile failed. code=%d", gle);
			MessageBox (errstr, "Token", MB_OK);	
		}
		else
		{
			if ((hFile2 = CreateFile (tempname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))==INVALID_HANDLE_VALUE)
			{
				gle = GetLastError();
				sprintf(errstr, "Error in CreateFile %s. code=%d", tempname, gle);
				MessageBox (errstr, "Token", MB_OK);	
			}
			else
			{
				gle = GetFileSize(hFile2, &dw);
				if (dw) 
				{
					sprintf(errstr, "File size too large");
					MessageBox (errstr, "Token", MB_OK);	
				}
				tempbuf = new char[gle+1];
				ReadFile(hFile2, tempbuf, gle, &dw, NULL);
				tempbuf[dw]=0;
				CloseHandle(hFile2);

				char *filename1 = "LastSession.log";
				remove(filename1);
				rename(tempname, filename1);
				_beginthread(uploadLogFile, 0, filename1);

				SetFilePointer(hFile, NULL, NULL, FILE_END);
				WriteFile(hFile, tempbuf, (DWORD)strlen(tempbuf), &dw, NULL);
				delete[] tempbuf;
				timeStamp(date, time);
				sprintf (buf, "[TIMEMARK] %s %s\r\n", date, time);
				WriteFile(hFile, buf, (DWORD)strlen(buf), &dw, NULL);
				nCorr = GetDlgItemInt(IDC_CORR_SOFAR);
				nTotals = GetDlgItemInt(IDC_TOTAL_TOKENS);
				score = (double)nCorr/ (double)nTotals * 100.;
				sprintf (buf, "[SCORE] %5.2f%% (%d of %d)\r\n", score, nCorr, nTotals);
				WriteFile(hFile, buf, (DWORD)strlen(buf), &dw, NULL);
				CloseHandle(hFile);
			}
		}
	}
	return 1;
}