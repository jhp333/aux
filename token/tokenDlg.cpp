#include "token.h"
#include "tokenDlg.h"

//extern "C" char *sendpraat (void *display, const char *programName, long timeOut, const char *text);

void ResampleThread (PVOID h)
{
	char errstr[256];
	errstr[0]=0;
	CTokenDlg *hDlg = (CTokenDlg*)h;
	for (int i=0; i<hDlg->tokManager.len; i++) {
		if (hDlg->tokManager.sig[i].GetFs()!=hDlg->fs) {
			hDlg->tokManager.sig[i].Resample(hDlg->fs, errstr);
			if (strlen(errstr)>0) { 
				hDlg->MessageBox(errstr);
				::SendMessage (hDlg->hStatusbar, SB_SETTEXT, 1, (LPARAM)"Resampling incomplete. Retry with new fs.");
				return; 
			}
		}
	}
	::SendMessage (hDlg->hStatusbar, SB_SETTEXT, 1, (LPARAM)"Resampling DONE. Ready to start.");
	hDlg->AstSig.Reset(hDlg->fs);
	hDlg->fs_ready = true;
}

BOOL CALLBACK  pipe_msg_log_proc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	switch (umsg)
	{
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			ShowWindow (hDlg, 0);
			break;
		}
		break;
	case WM_DESTROY:
		tokenMain.hPipeMsgViewDlg= NULL;
		break;

	default:
		return 0;
	}
	return 1;
}


CTokenDlg::CTokenDlg(void)
:trial(0), fs(0)
{
	strcpy(PipeName,SIGNAL_INTERFACE_PIPENAME);
}

CTokenDlg::~CTokenDlg(void)
{
}

void getCurrentModulePath(char *moduleName, char *path)
{
 	char drive[16], dir[256], ext[8], buffer[256], buf[256];

 	GetModuleFileName((HINSTANCE)GetModuleHandle(NULL), buf, sizeof(buf));
 	_splitpath(buf, drive, dir, buffer, ext);
 	sprintf (moduleName, "%s%s", buffer, ext);
 	sprintf (path, "%s%s", drive, dir);
}

BOOL CTokenDlg::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
	CWndDlg::OnInitDialog(hwndFocus, lParam);

	getCurrentModulePath(applname, AppPath);
	getVersionStringAndUpdateTitle (hDlg, applname, versionString);

	hFontBigTokenView = CreateFont (50, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, ANSI_CHARSET, 
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_ROMAN,
			"Verdana");

	hPipeMsgViewDlg = CreateDialog (hInst, "PIPE_COMM", hDlg, pipe_msg_log_proc);
	fdlg.InitFileDlg(hDlg, "");

//char message [1000], fileName[256], *errorMessage;
//sprintf(fileName, "c:\\soundfiles\\sentences\\N&L 2009 sentences\\Training sentences\\ethrow.wav");
//sprintf (message, "Read from file... %s\nPlay", fileName);
//errorMessage = sendpraat (NULL, "praat", 1000, message);

	return 1;
}

BOOL CTokenDlg::OnInitDialog2(HWND hwndFocus, LPARAM lParam)
{
	int acoust, randomize;
	char zbuf[128], errstr[128];
	string readstr;
	hStatusbar = CreateWindow (STATUSCLASSNAME, "", WS_CHILD|WS_VISIBLE|WS_BORDER|SBS_SIZEGRIP,
		0, 0, 0, 0, hDlg, (HMENU)IDC_STATUSBAR, hInst, NULL);
	sbarWidth [0] = 200;
	sbarWidth [1] = -1;
	::SendMessage (hStatusbar, SB_SETPARTS, 2, (LPARAM)sbarWidth);
	::MoveWindow (hStatusbar, 0, 0, 0, 0, TRUE);
	::SendMessage (hStatusbar, SB_SETTEXT, 1, (LPARAM)".....");
	SendMessage (WM_SIZE, SIZE_RESTORED); 
	
	SetDlgItemText(IDC_CURRENT_STATIC, "");

	char iniFile[512];
	sprintf (iniFile, "%s%s", AppPath, ININAME);

	// This is for only during the developing stage.....
	if (ReadINI(errstr, iniFile, "SUBJECT", readstr)>0)
	{
		SetDlgItemText(IDC_SUBJ, readstr.c_str());
	}
	// END OF ----This is for only during the developing stage.....

	if (ReadINI(errstr, iniFile, "FS", readstr)>0)	
	{
		// sscanf(readstr.c_str(), "%d", &fs); // this shouldn't be here because it is to be with EN_UPDATE
		SetDlgItemText(IDC_FS, readstr.c_str()); //maybe make it multithread here so waiting won't occur?
	}
	if (ReadINI(errstr, iniFile, "SESSION", readstr)>0)
	{
		if (OnSession(readstr.c_str()))
			SetDlgItemText(IDC_SESSION_PATH, readstr.c_str());
		//read def file and update the screen
	}
	if (ReadINI(errstr, iniFile, "DEFINITION", readstr)>0)
	{
		//read session file and update the screen
		if (OnDef(readstr.c_str()))
			SetDlgItemText(IDC_DEFINITION_PATH, readstr.c_str());
		EnableDlgItem(hDlg, IDC_GO, 1);
	}
	if (ReadINI(errstr, iniFile, "UDF PATH", readstr)>0)		SetDlgItemText(IDC_UDFPATH, readstr.c_str());
	if (ReadINI(errstr, iniFile, "NODE", readstr)>0)		SetDlgItemText(IDC_NODELIST, readstr.c_str());
	if (readstr !=".") SendDlgItemMessage(IDC_NODELIST, CB_ADDSTRING, 0, (LPARAM)".");
	if (ReadINI(errstr, iniFile, "LEVEL", readstr)>0)		SetDlgItemText(IDC_LEVEL, readstr.c_str());
	if (ReadINI(errstr, iniFile, "ACOUST", readstr)>0)
	{
		if (sscanf(readstr.c_str(), "%d", &acoust)==1 && acoust == 1) SendDlgItemMessage (IDC_ACOUSTIC, BM_SETCHECK, BST_CHECKED);
		else SendDlgItemMessage (IDC_ACOUSTIC, BM_SETCHECK, BST_UNCHECKED);
	}
	randomize = 1;	// defaults to randomize
	if (ReadINI(errstr, iniFile, "RANDOMIZE", readstr)>0)
		sscanf(readstr.c_str(), "%d", &randomize);
	SendDlgItemMessage (IDC_RANDOMIZE, BM_SETCHECK, randomize ? BST_CHECKED : BST_UNCHECKED);
	GetModuleFileName(hInst, zbuf, sizeof(zbuf));
	getVersionStringAndUpdateTitle (hDlg, zbuf, versionString);

	CreateMyTooltip(GetDlgItem(IDC_FS));
	return 1;
}

void CTokenDlg::OnSize(UINT state, int cx, int cy)
{
	static int _cx, _cy;
	if (cx!=0)
	{	_cx = cx;  _cy = cy;	
		sbarWidth [0] = cx / 4;
		sbarWidth [1] = -1;
	}
	::MoveWindow (hStatusbar, 0, _cy - 10, _cx,	 _cy, TRUE);
	::SendMessage (hStatusbar, SB_SETPARTS, 2, (LPARAM)sbarWidth);
}


void CTokenDlg::OnClose()
{
	int acoust, randomize;
	char errstr[128];
	CStdString readstr;

	char iniFile[512];
	sprintf (iniFile, "%s%s", AppPath, ININAME);

	//	if (ReadINI(errstr, iniFile, "DEFINITION", readstr)>0)
//		WriteDefFile(readstr.c_str());
	// This is for only during the developing stage.....
	GetDlgItemText(IDC_SUBJ, readstr);
	if (!printfINI(errstr, iniFile, "SUBJECT", readstr.c_str())) MessageBox(errstr, iniFile); 
	// END OF ----This is for only during the developing stage.....
	GetDlgItemText(IDC_SESSION_PATH, readstr);
	if (!printfINI(errstr, iniFile, "SESSION", readstr.c_str())) MessageBox(errstr, iniFile); 
	GetDlgItemText(IDC_DEFINITION_PATH, readstr);
	if (!printfINI(errstr, iniFile, "DEFINITION", readstr.c_str())) MessageBox(errstr, iniFile); 
	GetDlgItemText(IDC_NODELIST, readstr);
	if (!printfINI(errstr, iniFile, "NODE", readstr.c_str())) MessageBox(errstr, iniFile); 
	GetDlgItemText(IDC_LEVEL, readstr);
	if (!printfINI(errstr, iniFile, "LEVEL", readstr.c_str())) MessageBox(errstr, iniFile); 
	GetDlgItemText(IDC_FS, readstr);
	if (!printfINI(errstr, iniFile, "FS", readstr.c_str())) MessageBox(errstr, iniFile); 
	GetDlgItemText(IDC_UDFPATH, readstr);
	if (!printfINI(errstr, iniFile, "UDF PATH", readstr.c_str())) MessageBox(errstr, iniFile); 
	if (SendDlgItemMessage (IDC_ACOUSTIC, BM_GETCHECK, 0, 0)==BST_CHECKED) acoust = 1;
	else acoust = 0;
	if (!printfINI(errstr, iniFile, "ACOUST", "%d", acoust)) MessageBox(errstr, iniFile); 
	if (SendDlgItemMessage (IDC_RANDOMIZE, BM_GETCHECK, 0, 0)==BST_CHECKED) randomize = 1;
	else randomize = 0;
	if (!printfINI(errstr, iniFile, "RANDOMIZE", "%d", randomize)) MessageBox(errstr, iniFile); 


	//DeleteFile ("*.tmp");  doesn't work
	WIN32_FIND_DATA fd;
	HANDLE h;
	int b(1);
	h = FindFirstFile("*.tmp", &fd);
	while (b && h!=INVALID_HANDLE_VALUE)
	{
		DeleteFile(fd.cFileName);
		b = FindNextFile (h, &fd);	
	}
	FindClose(h);

	DestroyWindow(hDlg);
}

void CTokenDlg::OnDestroy()
{
	PostQuitMessage(0);
}

int CTokenDlg::OnSession(const char *fname)
{
	char buf[256];
	string vern;
	uintptr_t hThread;
	if (tokManager.ReadSession(fname))
	{
		fs_ready = false;
		//Check fs and resample in the background if not the same.
		if (fs > 0) {
			LoadString (hInst, IDS_STRING109, buf, sizeof(buf));
			::SendMessage (hStatusbar, SB_SETTEXT, 1, (LPARAM)buf);
			hThread = _beginthread (ResampleThread, 0, (void*)this); 
		}
		while(SendDlgItemMessage(IDC_SESSION_VIEW, LB_DELETESTRING, 0)!=LB_ERR) {};
		SetDlgItemText(IDC_SESSION_NAME, tokManager.name);
		for (int i=0; i<tokManager.len; i++)
		{
			sprintf(buf, "%s", tokManager.file[i].c_str());
			SendDlgItemMessage(IDC_SESSION_VIEW, LB_ADDSTRING, 0, (LPARAM)buf);
		}
		return 1;
	}
	else
	{
		MessageBox (tokManager.errstr);
		return 0;
	}
}

int CTokenDlg::OnDef(const char *fname)
{
	char *readPt;
	CStdString estr;
	HANDLE hFile = CreateFile(fname, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD fsize, readCt;
	if (hFile!=INVALID_HANDLE_VALUE)
	{
		fsize = GetFileSize(hFile, NULL); 
		readPt = new char[fsize+10];
		ReadFile(hFile, readPt, fsize, &readCt, NULL);
		readPt[readCt]=0;
		SetDlgItemText(IDC_AUX, readPt);
		CloseHandle(hFile);
		varDef = readPt;
		delete[] readPt;
		trim(varDef,"\r\n");
		return 1;
	}
	else
	{
		GetLastErrorStr(estr);
		estr += " ";
		estr += fname;
		MessageBox (estr, "AUX File open error");
		return 0;
	}
}

void CTokenDlg::WriteDefFile(const char *fname)
{
	CStdString str;
	HANDLE hFile;
	DWORD writeCt;
	GetDlgItemText(IDC_AUX, str);
	trim(str,"\r\n");
	if (str!=varDef)
	{
		hFile = CreateFile(fname, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		WriteFile(hFile, str.c_str(), (DWORD)str.length()+1, &writeCt, NULL);
		CloseHandle(hFile);
	}
}

void CTokenDlg::OnFlyArrived(WORD command, WORD len, void* inBuffer)
{
}

void CTokenDlg::OnFlyClosed()
{
}

void CTokenDlg::OnFlyConnected(char *hostname, char* ipa)
{
}

bool CTokenDlg::Prepare(const char *AuxInputDef, char *estr)
{
	static CAstSig ast(&AstSig);
	try {
		ast.SetNewScript(AuxInputDef);
		ast.Compute();
	} catch (const char *errmsg) {
		sprintf(estr, "%s (error in AUX input variables)", errmsg);
		return false;
	}

	CStdString longbuf;
	GetDlgItemText(IDC_AUX, longbuf);
	varDef = longbuf.c_str();
	try {
		AstSig.SetNewScript(longbuf.c_str());
		AstSig.Compute();
	} catch (const char *errmsg) {
		sprintf(estr, "%s (error in AUX)", errmsg);
		return false;
	}
	//CSignals x=AstSig->GetTag("x");
	//CSignals xx=AstSig->GetTag("xx");
	//CSignals target=AstSig->GetTag("TARGET");
	//CSignals remn=AstSig->GetTag("remn");
	return true;
}

HBRUSH CTokenDlg::OnCtlColorStatic(HDC hdc, HWND hCtrl, int)
{
	COLORREF col2, col(RGB(200, 150, 0));
	int idc;
	col2 = GetSysColor (COLOR_BTNFACE);
	idc = (int)::GetWindowLong (hCtrl, GWL_ID);
	switch (idc)
	{
	case IDC_CURRENT_STATIC:
		SelectObject (hdc, hFontBigTokenView);
		SetTextColor (hdc, col);
		break;
	}
	SetBkColor (hdc, col2);
	return (HBRUSH) CreateSolidBrush (col2);
}

void CTokenDlg::CreateMyTooltip (HWND hWnd2Display)
{
    HWND hwndTT; 
    TOOLINFO ti;// struct specifying info about tool in tooltip control
    unsigned int uid = 0;       
    char helpStr[2048];

    RECT rect;                  // for client area coordinates

	LoadString (hInst, IDS_HELPSTRING1, helpStr, sizeof(helpStr));

    hwndTT = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,        
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        hWnd2Display, NULL, hInst, NULL);

    SetWindowPos(hwndTT, HWND_TOPMOST, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    /* GET COORDINATES OF THE MAIN CLIENT AREA */
    GetClientRect (hWnd2Display, &rect);
	
    /* INITIALIZE MEMBERS OF THE TOOLINFO STRUCTURE */
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS;
    ti.hwnd = hWnd2Display;
    ti.hinst = hInst;
    ti.uId = uid;
    ti.lpszText = helpStr;
    ti.rect.left = rect.left;    
    ti.rect.top = rect.top;
    ti.rect.right = rect.right;
    ti.rect.bottom = rect.bottom;
    
    /* SEND AN ADDTOOL MESSAGE TO THE TOOLTIP CONTROL WINDOW */
	::SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);	
	::SendMessage(hwndTT, TTM_SETMAXTIPWIDTH, 0, 300);
} 