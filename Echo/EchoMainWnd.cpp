// EchoMainMainAppWnd.cpp: implementation of the CEchoMainWnd class->
//
//////////////////////////////////////////////////////////////////////

#include "Echo.h"
#include "EchoMainWnd.h"
#include <time.h>

WNDPROC hOrgEditProc;


//Token ID is represented by UCHAR--assuming that number of tokens doesn't exceed 64.

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEchoMainWnd::CEchoMainWnd()
:nConc(1), multi(0), wavListReady(0)
, report("Echo")
{
	hParent = NULL;
	// Instantiation of child classes
	batch = new BATCH;
	hResult = new CResultWnd;
	hMulti = new CmultiProc; 
	ss = new CSession;
}

CEchoMainWnd::~CEchoMainWnd()
{

}

int CEchoMainWnd::BeginSessionTransmission()
{
	char buf[2048];
	int i, res;
	DWORD x;
	// FL_TOTAL_CURRENT_TRIALS sends total number of trials and current trials (making DWORD)
	x = MAKELONG((WORD)nTrials,(WORD)trial);
	FER0(flySendBin (FL_TOTAL_CURRENT_TRIALS, &x, sizeof(x)));
	FER0(flySendBin (FL_INIT_FEEDBACK, &feedBackOn, sizeof(feedBackOn)));
	FER0(flySendBin (FL_INIT_SINGLE_RESPONSE, &fSingleResponse, sizeof(fSingleResponse)));
	FER0(flySendBin (FL_INIT_ALLOW_SAME, &fAllowSameToken, sizeof(fAllowSameToken)));
	FER0(flySendBin (FL_INIT_NUMBER_TOKENS, &ss->nToken, sizeof(ss->nToken)));
	FER0(flySendBin (FL_INIT_NUMBER_STIM, &nConc, sizeof(nConc)));

	if (trial==0)
	{
		LoadString (hInst, IDS_SUBJ_INST1, buf, sizeof(buf));
		FER0(flySendText (FL_INIT_SESS, buf));
	}

	for (i=0; i< ss->nToken; i++)
	{
		sprintf (buf, "%s %s", ss->token[i], ss->tokenExample[i]);
		res=flySendText (FL_INIT_TOKEN_STRING, buf);
	}
	return res;
}

void CEchoMainWnd::PlayNext()
{
	UCHAR currentToken[MAX_NUM_STIM];
	char errstr[512] = "", buf[_MAX_PATH];
	string temp[2];
	int res, i;

	for (i=0; i<nConc; i++)
		currentToken[i] = wavID2tokenID[Combs[presentOrder[trial]][i]];
	SendDlgItemMessage(IDC_PLAYLIST, LB_GETTEXT, (WPARAM)trial, (LPARAM)buf);
	// FL_CORRECT_ANS sends the index of the correct answer (among the displayed buttons)
	flySendBin (FL_CORRECT_ANS, currentToken, sizeof(currentToken[0])*nConc);
	if (str2array(temp, 2, buf, ":")<2)
	{	MessageBox (errstr, "Invalid format for the list", MB_OK);	return;		}
	if (!ReadLineandWav(temp[1].c_str(), errstr)) 	{	MessageBox(errstr);		return;	}
	res = PlayStim();
	if (res!=1) // if returns 0, abort the session.
	{
		ShowWindow (GetDlgItem(IDC_RETRY), SW_SHOW);
		ShowWindow (GetDlgItem(IDC_BEGIN), SW_HIDE);
	}
	else
	{
		ShowWindow (GetDlgItem(IDC_RETRY), SW_HIDE);
		ShowWindow (GetDlgItem(IDC_BEGIN), SW_SHOW);

		if (multi)
			PresenterInfos[idPresets[trial]][idAddProcs[trial]] = preparedCondition;
		else
			PresenterInfos[0][0] = preparedCondition;

		trial++;
		LoadString (hInst, IDS_SUBJ_INST3, buf, sizeof(buf));
		flySendText (FL_ABOUT2STOP, buf);
	}
}


BOOL CALLBACK missFilesProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	switch (umsg)
	{
	case WM_INITDIALOG:
		ShowWindow (hDlg, 0);
		break;
	case WM_COMMAND:
		if (wParam==WM_DESTROY)
			ShowWindow (hDlg, 0);
		break;
	case WM_DESTROY:
		MainAppWnd.hMissingDlg= NULL;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

LRESULT DisplayConsSet(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	int i;
	switch (umsg)
	{
	case WM_SHOWWINDOW:
		for (i=0; i<MainAppWnd.ss->nToken; i++)
			EditPrintf(hwnd, "%s\n", MainAppWnd.ss->token[i]);
		break;
	}
	return CallWindowProc(hOrgEditProc, hwnd, umsg, wParam, lParam);
}

BOOL CEchoMainWnd::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
	char buf[2048], zbuf[512];

	GetModuleFileName(hInst, zbuf, sizeof(zbuf));
	getVersionStringAndUpdateTitle (hDlg, zbuf, versionString);
	getCurrentModulePath(buf, AppPath);

	AstSig.SetPath("", AppPath);

	AddString2NodesList (hDlg, "nodes.ini");
	SendDlgItemMessage (IDC_LEVEL, CB_ADDSTRING, 0L, (LPARAM)"70");
	SendDlgItemMessage (IDC_LEVEL, CB_ADDSTRING, 0L, (LPARAM)"80");
	SetDlgItemInt (IDC_LEVEL, 70, FALSE);
	SendDlgItemMessage (IDC_SINGLE, BM_SETCHECK, 1, 0);
	hRespLog = GetDlgItem(IDC_RESPONSELOG);
	hGList = GetDlgItem(IDC_GENERAL_LIST);

	PopFileInitialize (hDlg);
	ReadINI_UpdateScreen();

	hStatusbar = CreateWindow (STATUSCLASSNAME, "", WS_CHILD|WS_VISIBLE|WS_BORDER|SBS_SIZEGRIP,
		0, 0, 0, 0, hDlg, (HMENU)ID_STATUSBAR, hInst, NULL);
	::SendMessage (hStatusbar, SB_SETTEXT, 0, (LPARAM)"Connection to Subject PC: OFF");
	::SetWindowLong (GetDlgItem(IDC_WAVREADY), GWL_USERDATA, (LONG)missingFiles);

	// Creation of child windows
	hPipeMsgViewDlg = CreateDialog (hInst, "PIPE_COMM", hDlg, pipe_msg_log_proc);
	batch->hDlg = CreateDialog (hInst, "BATCH", hDlg, batchProc);
	batch->hParent = this;
	hMulti->hDlg = CreateDialog (hInst, MAKEINTRESOURCE(IDD_MULTIPLEX), hDlg, multiProc);
	::SetTimer(hMulti->hDlg, SETPARAM, 300, NULL);
	hMissingDlg = CreateDialog (hInst, "MISSINGFILES", hDlg, missFilesProc);

	// Retrieve previous setting and adjust consTesting
 	
	CheckRadioButton (hDlg, IDC_CONS, IDC_GENERAL, (consTesting==1) ? IDC_CONS:IDC_GENERAL);
	CreateTokensNShow();
	sprintf (zbuf, "%s%s", AppPath, "remoteid"); 
	PickRemotePC(NULL, buf, zbuf);
	FER(initfly (hDlg, buf, -1, true));  // Client connection
	CreateMyTooltip(GetDlgItem(IDC_PROC));
	// Subclassing IDC_GENERAL_LIST so that everytime it is shown, it displays the previously-defined token set.
	hOrgEditProc = (WNDPROC)::SetWindowLong (GetDlgItem(IDC_GENERAL_LIST), GWL_WNDPROC, (LONG)DisplayConsSet);

	MoveWindow (hStatusbar, 0, 0, 0, 0, TRUE);
	sbarWidth [0] = 300;
	sbarWidth [1] = -1;
	::SendMessage (hStatusbar, SB_SETPARTS, 2, (LPARAM)sbarWidth);
	time_t tme = time(NULL);
	srand( (unsigned int) tme ); // for randperm
	trial = -1;
	return 1;
}

void CEchoMainWnd::OnFlyConnected(char *hostname, char* ipa)
{
	::SendMessage (hStatusbar, SB_SETTEXT, 0, (LPARAM)"Connection to Subject PC: ON");
	if (!connected) 
	{
		SetTimer(JUST_CONNECTED, 500, NULL);
	}
	connected=1;
	

}

void CEchoMainWnd::OnFlyClosed()
{
	::SendMessage (hStatusbar, SB_SETTEXT, 1, (LPARAM)"");
	::SendMessage (hStatusbar, SB_SETTEXT, 0, (LPARAM)"Connection to Subject PC: OFF");
	connected=0;
}

void CEchoMainWnd::OnFlyArrived(WORD command, WORD len, void* inBuffer)
{
	UCHAR currentToken[MAX_NUM_STIM];
	int i, gap, response[MAX_NUM_STIM];
	char buf[2048], buf2[2048], errstr[256], zbuf[512], nameRuleStr[64];
	CString NodeName, StimPathFile;
	CSession pracSS;

	sprintf (buf, "command = %x\n\r", command);
	switch (command)
	{
	case FL_STARTSESSION:
		SendMessage(WM_FL_ARRIVED, FL_RESPONSE, (LPARAM)&i);
		break;

	case FL_RESPONSE:
		memcpy ((void*)&response[0], inBuffer, sizeof(response));
	
		SendDlgItemMessage (IDC_PLAYLIST, LB_SETCURSEL, trial, 0);
		if (trial>0)
		{
			EditPrintf (hRespLog, "- ");
			for (i=0; i<nConc; i++)
			{
				if (fSingleResponse && i>0) {	// when SingleResponse, only response[0] is valid.
					respCollected[i][trial-1]=-1;	// invalidate it, because 0 matches the first token.
				} else {
					respCollected[i][trial-1]=response[i];
					EditPrintf (hRespLog, "%s ", ss->token[response[i]]);
				}
			}
			EditPrintf (hRespLog, "\r\n");
		}
		if (trial==nTrials)
			PostMessage (WM__SESSION_DONE); 				
		else
		{
			if (multi)
			{
//				SetDlgItemText (IDC_ADDPROC, hMulti->AddProcs[idAddProcs[trial]]);
				if (strlen(hMulti->PresenterSettings[idPresets[trial]])>0)
				{
					SetDlgItemText (IDC_PRESENT_CONTROL, hMulti->PresenterSettings[idPresets[trial]]);
					SendMessage(WM_COMMAND, MAKELONG(IDC_PRESENTER_SET, BN_CLICKED));
				}
				UpdateWindow (hDlg); 
			}
			LoadString (hInst, IDS_SUBJ_INST2, zbuf, sizeof(zbuf));
			flySendText (FL_ABOUT2PRESENT, zbuf);
			for (i=0; i<nConc; i++)
			{
				currentToken[i] = wavID2tokenID[Combs[presentOrder[trial]][i]];
				EditPrintf (hRespLog, "%s ", ss->token[currentToken[i]]);
			}
			if (nConc>1)
			{
				gap= GetDlgItemInt (IDC_GAP, &i);
				sprintf (buf, "%s%s", ss->wavePath, waveNameList[Combs[presentOrder[trial]][0]].c_str());
				sprintf (buf2, "%s%s", ss->wavePath, waveNameList[Combs[presentOrder[trial]][1]].c_str());
				int ch = (SendDlgItemMessage (IDC_STEREO, BM_GETCHECK)==BST_CHECKED) ? 1:0;			
				//if (!SignalPlusSignal (StimPathFile, buf, buf2, 0, gap, ch, errstr))
				//{
				//	MessageBox (errstr, "error in SignalPlusSignal()");
				//	break;
				//}
			}
			else
			{
				append_path_file(buf, ss->wavePath, waveNameList[Combs[presentOrder[trial]][0]].c_str());
				GetDlgItemText(IDC_NODELIST, NodeName);
				StimPathFile = ((NodeName == ".") ? CString(AppPath).Left(2) : "\\\\"+NodeName) + STIM_PATH_FILE;
				if (CopyFile (buf, StimPathFile, 0)==0)
				{
					MessageBox (StimPathFile + " <- " + buf, "Could not copy");
					break;
				}
			}
			//CopyEchoStimsIfNecessary(buf);
			PlayNext ();
			// Custom output. By jhpark 8/31/2011
			CSignals SigOutStr = AstSig.GetTag("OUTSTR");
			if (SigOutStr.GetLength() > 0) {
				if (multi)
					sprintf(buf, "%d:[%d %d] ", trial, idPresets[trial-1], idAddProcs[trial-1]);
				else
					sprintf (buf, "%d: ", trial);
				SigOutStr.getString(buf2, sizeof(buf2));
				strcat (buf, buf2);
				SendDlgItemMessage(IDC_PLAYLIST, LB_DELETESTRING, trial-1, 0);
				SendDlgItemMessage(IDC_PLAYLIST, LB_INSERTSTRING, trial-1, (LPARAM)buf);
				if (trial == 1) {
					appendLine2File(LOG_FILE, "--------------------------");
					appendLine2File(LOG_FILE, "Modified list:");
				}
				appendLine2File(LOG_FILE, buf);
			}
			CSignals SigLogStr = AstSig.GetTag("LOGSTR");
			SigLogStr.getString(buf2, sizeof(buf2));
			logPlayList[trial-1] = buf2;
		}
		break;

	case FL_DONE:
		EnableDlgItem (hDlg, IDC_PRACTICE, TRUE);
		EnableDlgItem (hDlg, IDC_BEGIN, TRUE);
		if(!cleanpracPlayList)
		cleanpracPlayList=1;
		break;

	case FL_PLAYTHIS:
		pracSS = *ss;
		pracSS.nToken = 1;
		strcpy(pracSS.token[0], (char*)inBuffer);
		pracSS.token[0][len]=0;
		GetDlgItemText (IDC_NAMERULE, nameRuleStr, 64);
		pracSS.nCombo=DecodeNameRuleAndGenerateList (nameRuleStr, pracPlayList, map<int,int>(), &pracSS);
		LoadString (hInst, IDS_SUBJ_INST6, zbuf, sizeof(buf));
		flySendText (FL_ABOUT2PRESENT, zbuf);
		if (nConc>1)
		{
			MessageBox (errstr, "Cannot practice in a dual mode.");
			break;
		}
		StimPathFile = AppPath + CString("practice.wav");
		for (i=0; i<pracSS.nCombo; i++)
		{
			append_path_file(buf, ss->wavePath, pracPlayList[i].c_str());
			if (i==0) {
				CSignals tsig;
				if (!tsig.Wavread(buf, errstr)) {
					MessageBox(errstr, "Wavread");
					break;
				}
				if (tsig.GetFs()!=Fs && !tsig.Resample(Fs,errstr)) {
					MessageBox(errstr, "Resample");
					break;
				}
				if (!tsig.Wavwrite(StimPathFile, errstr)) {
					MessageBox(errstr, "Wavwrite");
					break;
				}
			} else {
				if (!WavCat(StimPathFile, buf, 500., errstr))
				{
					MessageBox (errstr, "Cannot concatenate wave files");
					break;
				}
			}
		}
		CString AuxDef;
		AuxDef = "TG=wave(\"" + StimPathFile + "\")\n";
		AuxDef += "TGNAME=\"" + CString("practice.wav") + "\"\n";
		try {
			CAstSig ast(AuxDef, &AstSig);
			ast.Compute();
		} catch (const char *errmsg) {
			MessageBox(errmsg, "AUX Definition");
			break;
		}
		if (!PlayStim())
			break;
		LoadString (hInst, IDS_SUBJ_INST5, zbuf, sizeof(buf));
		flySendText (FL_ABOUT2STOP, zbuf);
	}

}

void CEchoMainWnd::OnTimer(HWND hwnd, UINT id)
{
	KillTimer(id);
	if (id==JUST_CONNECTED) 
	{
		FER(flySendText (FL_RUNDLL, TS_DLL_NAME))
		if (trial >= 0)	BeginSessionTransmission();
	}
}

HBRUSH CEchoMainWnd::OnCtlColorStatic(HDC hdc, HWND hCtrl, int)
{
	DWORD col;
	int idc;
	col = GetSysColor (COLOR_BTNFACE);
	idc = (int)::GetWindowLong (hCtrl, GWL_ID);
	switch (idc)
	{
	case IDC_WAVREADY:
		if (!wavListReady)
			SetTextColor (hdc, COL_WAV_NOT_READY);
		break;
	}
	SetBkColor (hdc, col);
	return (HBRUSH) CreateSolidBrush (col);
}

void CEchoMainWnd::OnSize(UINT state, int cx, int cy)
{
	MoveWindow (hStatusbar, 0, cy - 10, cx,	 cy, TRUE);
	sbarWidth [0] = cx / 2;
	sbarWidth [1] = -1;
	::SendMessage (hStatusbar, SB_SETPARTS, 2, (LPARAM)sbarWidth);
}

void CEchoMainWnd::NewSession(char* SessionFile)
{
	int res;
	char errstr[128];
	res = ReadSessFile (errstr);
	if (res!=1 && res!=0)
	{
		MessageBox (errstr, "ReadSessFile fails");
		return;
	}
	if (res==1) // consonant testing
		SendDlgItemMessage (IDC_CONS, BM_CLICK);
	else // (res==0)
		SendDlgItemMessage (IDC_GENERAL, BM_CLICK);
	PostMessage(WM__ORG_SESS_SCREEN);
	SendMessage (WM_COMMAND, IDC_STOP);
	SetDlgItemText (IDC_PLAYLIST, "");
}


void CEchoMainWnd::OnClose()
{
	char buf[2048], zbuf[512], fname[128], ext[16];
	if (trial!=-1 && MessageBox("A session is in progress.\nAre you sure you want to quit?", "Echo", MB_YESNO | MB_ICONWARNING)!=IDYES)
		return;
	closefly();
	_splitpath (sessFileNameWPath, zbuf, buf, fname, ext);
	sprintf (resultPath, "%s%s", zbuf, buf);
	UpdateINI(sessFileNameWPath);
	DestroyWindow (hPipeMsgViewDlg);
	PostQuitMessage(0);
	PresenterInfos.clear();

	delete batch;
	delete hResult;
	delete hMulti; 
	delete ss;
}

void CEchoMainWnd::RawPresentRespSequence2Selected(int m, int n, int *po, unsigned char **rp)
{
	int i, j;
	int cum=0;

	for (i=0; i<nTrials; i++)
	{
		if (idPresets[i]==m && idAddProcs[i]==n)
		{
			po[cum] = presentOrder[i];
			for (j=0; j<nConc; j++)
				rp[j][cum] = respCollected[j][i];
			cum++;
		}
	}
}

void CEchoMainWnd::outputPresentedResponded(FILE *fp, int m, int n)
{
	if (!fp) return;

	fprintf(fp, "%s", "[PRESENTED-RESPONDED]\r\n");
	for (int i=0; i<nTrials; ++i) {
		if (!multi || idPresets[i]==m && idAddProcs[i]==n) {
			fprintf(fp, "%d: ", i+1);
			if (logPlayList[i].length() > 0)
				fprintf(fp, "%s ", logPlayList[i].c_str());
			else
				for (int j=0; j<nConc; ++j)
					fprintf(fp, "%s ", waveNameList[Combs[presentOrder[i]][j]].c_str());
			fprintf(fp, " - ");
			for (int j=0; j<(fSingleResponse?1:nConc); ++j)
				fprintf(fp, " %s", ss->token[respCollected[j][i]]);
			fprintf(fp, "\r\n");
		}
	}
}

void CEchoMainWnd::SessionDonePrint(int done, FILE *fp)
{
	char buf[128];
	unsigned char *rp[MAX_NUM_STIM];
	int *pr;
	int i, j, org_nTrials ;
	bool fTmp = false;

	trial = -1;

	if (done)
	{
		LoadString (hInst, IDS_SUBJ_INST4, buf, sizeof(buf));
		flySendText (FL_DONE, buf);
		EnableDlgItem (hDlg, IDC_BEGIN, 1);
		EnableDlgItem (hDlg, IDC_STOP, 0);
		EnableDlgItem(hMulti->hDlg, IDC_GO, 1);
		hResult->hDlg = CreateDialog (hInst, "RESULT", hDlg, (DLGPROC)ResultProc);
	}

	if (fp == NULL) {
		fTmp = true;
		fp = fopen("session.log", "wb+");
		if (fp == NULL) {
			MessageBox ("file session.log open fails.");
			return;
		}
	}

		// header
	CString lev;
	GetDlgItemText (IDC_LEVEL, lev);
	fprintf(fp, "%s\r\n[SUBJECT ID] %s\r\n[TOKEN FORMAT] %s\r\n[PATH] %s\r\n[LEVEL] %sdB\r\n", sessionBeganTime.c_str(), sessionSubjectID.c_str(), ss->nameRule, ss->wavePath, lev);
	fprintf(fp, "[FEEDBACK] %s\r\n[SINGLE RESPONSE] %s\r\n[ALLOW SAME TOKEN] %s\r\n", feedBackOn?"On":"Off", fSingleResponse?"On":"Off", fAllowSameToken?"On":"Off");

	CString addprocStr;
	GetDlgItemText (IDC_PROC, addprocStr);
	fprintf(fp, "[AUX PROCESSING]\r\n%s\r\n", addprocStr);

	if (multi)
	{
		org_nTrials = nTrials/hMulti->nPresets/hMulti->nAddprocs;
		pr = new int[org_nTrials];
		for (i=0; i<nConc; i++)
			rp[i] = new unsigned char[org_nTrials];
		for (i=0; i<hMulti->nPresets; i++)
		{
			for (j=0; j<hMulti->nAddprocs; j++)
			{
				fprintf(fp, "\r\n[PRESENTER INFO] %s\r\n", PresenterInfos[i][j].c_str());
				fprintf(fp, "[MULTIPLE TASKS VAR] %s\r\n", hMulti->AddProcs[j]);

				RawPresentRespSequence2Selected(i, j, pr, rp); // from presentOrder, respCollected  to  pr, rp with i, j
				hResult->outputSummary(fp, pr, rp, org_nTrials);
				if (fp!=NULL)
					outputPresentedResponded(fp, i, j);
			}
		}
		delete[] pr;
		for (i=0; i<nConc; i++)
			delete[] rp[i];
	}
	else
	{
		fprintf(fp, "%s\r\n", PresenterInfos[0][0].c_str());

		hResult->outputSummary(fp, presentOrder, respCollected, nTrials);
		if (fp!=NULL)
			outputPresentedResponded(fp, 0, 0);
	}
	hResult->outputFooter(fp);

	if (batch->onoff && fTmp)
	{
		ShowWindow (hResult->hDlg, SW_SHOW);
		::PostMessage (hResult->hDlg, WM__SAVE2FILE, (WPARAM)batch->BEGIN_fname2save, 0);
		::PostMessage (batch->hDlg, WM__RUNBATCH, 0, 0);
	}
	if (fTmp)
		fclose(fp);
}

void CEchoMainWnd::Tokens(int code)
{// IDC_TOKEN_B THRU IDC_TOKEN_TH MUST BE SEQUENTIAL
	char *exampleStr;
	char tokenCheckButtonStr[128];
	char tokenStr[MAX_LEN_TOKEN];
	int i, j;
	for (i=0; i<24; i++)		strcpy(ss->tokenExample[i], "");
	switch (code)
	{
	case CHECK_TOKENS: //token and tokenString are filled here.
		for (i=IDC_TOKEN_B; i<=IDC_TOKEN_TH; i++)
		{
			GetDlgItemText (i, tokenCheckButtonStr, 64);
			memset (tokenStr, 0, MAX_LEN_TOKEN);
			exampleStr=NULL;
			exampleStr = strchr(tokenCheckButtonStr, ' ');
			if (exampleStr==NULL)
				strcpy (tokenStr, tokenCheckButtonStr);
			else
				strncpy (tokenStr, tokenCheckButtonStr, exampleStr-tokenCheckButtonStr);
			for (j=0; j<ss->nToken; j++)
			{
				if (strcmp(tokenStr, ss->token[j])==0)
				{
					::SendMessage (GetDlgItem(i), BM_SETCHECK, BST_CHECKED, 0);
					if (exampleStr!=NULL)
						strcpy(ss->tokenExample[j], exampleStr+1);
					j = ss->nToken+100;
				}
			}
			if (j < ss->nToken + 2)
				::SendMessage (GetDlgItem(i), BM_SETCHECK, BST_UNCHECKED, 0);
		}
		break;
	case READ_TOKENS:
		j=0;
		for (i=IDC_TOKEN_B; i<=IDC_TOKEN_TH; i++)
		{
			if ( ::SendMessage (GetDlgItem(i), BM_GETCHECK, 0, 0)==BST_CHECKED)
			{
				GetDlgItemText (i, tokenCheckButtonStr, 64);
				exampleStr=NULL;
				exampleStr = strchr(tokenCheckButtonStr, ' ');
				memset(ss->token[j], 0, MAX_LEN_TOKEN);
				if (exampleStr==NULL)
					strcpy (ss->token[j], tokenCheckButtonStr);
				else
				{
					strncpy (ss->token[j], tokenCheckButtonStr, exampleStr-tokenCheckButtonStr);
					strcpy (ss->tokenExample[j], exampleStr+1);
				}
				j++;
			}
		}
		ss->nToken = j;
		ss->nCombo = FillDesireWavList (IDC_WAVELIST2, IDC_NUM_COMBO);
		break;

	case READ_TOKENS_GENERAL:
		GetDlgItemText (IDC_GENERAL_LIST, tokenCheckButtonStr, sizeof(tokenCheckButtonStr));
		ss->nToken = str2strarray(ss->token, MAX_NUM_TOKEN, tokenCheckButtonStr, " \r\n\t");
		if (ss->nToken>0)
			PostMessage(WM__ORG_SESS_SCREEN);
		break;
	}
}

void CEchoMainWnd::DisplaySession(char* wParam, int code)
{
	int i, res;
	if (code==PREV_SESSION_LOADED)
		if (strlen((char*)wParam)>0)
			strcpy(sessFileNameWPath, (char*)wParam);
	i=SetDlgItemText (IDC_SESSION_FNAME, sessFileNameWPath);
	i=SetDlgItemText (IDC_SESSION_NAME, ss->sessionName);
	i=SetDlgItemText (IDC_WAV_DIRECTORY, ss->wavePath);
	i=SetDlgItemText (IDC_NAMERULE, ss->nameRule);
	if (strlen(ss->wavePath)>0)	
	{
		res = SeeFolder (IDC_WAVELIST, ss->wavePath);
		SetDlgItemInt (IDC_NUM_FOLDER_FILES, res);
		ss->nCombo = FillDesireWavList (IDC_WAVELIST2, IDC_NUM_COMBO);
	}
	
	if (consTesting)
		PostMessage (WM__TOKENS, 0, CHECK_TOKENS);
	else
	{
		ClearEditPrintf(hGList); 
		for (i=0; i<ss->nToken; i++)
			EditPrintf (hGList, "%s\r\n", ss->token[i]);
	}
	if (ss->nCombo>0)
	{
		CheckWavFilesReadyAndDisplay (IDC_WAVREADY, waveNameList, missingFiles);
		nSpeakers = ss->nCombo / ss->nToken;
	}
}

BOOL CALLBACK  echoTempProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	switch (umsg)
	{
	case WM_INITDIALOG:
		return 1;
	}
	return 0;
}


BOOL CALLBACK  pipe_msg_log_proc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	static char *inBuffer, *existingStr;
	static HWND hMsgHist;

	switch (umsg)
	{
	case WM_INITDIALOG:
		hMsgHist = GetDlgItem(hDlg, IDC_MSG_LOG);
		/* What are these? Never used, never freed. JP is commenting the following 2 lines out on 5/14/2010.
		existingStr = (char*)calloc(MAX_NUM_CHARACTERS_PIPE_COMM,1);
		inBuffer = (char*)calloc(MAX_PROCESSING_CONDITION_STRING+128,1);
		*/
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
		MainAppWnd.hPipeMsgViewDlg= NULL;
		break;

	default:
		return 0;
	}
	return 1;
}

void CEchoMainWnd::CleanUpTestingSession()
{
	free(presentOrder);
	for (int i=0; i<nConc; i++)
	{
		free(respCollected[i]);
	}
	if (idPresets) {
		delete[] idPresets;
		idPresets = NULL;
	}
	if (idAddProcs) {
		delete[] idAddProcs;
		idAddProcs = NULL;
	}
}

/*
int genCombination(vector<vector<int>> &combs, int n, int k)
{
	vector<int> comb;
	combs.clear();
    for (int i=0; i<k; ++i)
        comb.push_back(i);
    while (1) {
		combs.push_back(comb);
		int i;
        for (i=k-1; i>=0 && ++comb[i]>n-k+i; --i)
            ;
		if (comb[0] > n-k)
			break;
		for (++i; i<k; ++i)
            comb[i] = comb[i-1] + 1;
    }
    return 0;
}

int genMultiCombination(vector<vector<int>> &combs, int n, int k)
{
	vector<int> comb;
	combs.clear();
    for (int i=0; i<k; ++i)
        comb.push_back(0);
    while (1) {
		combs.push_back(comb);
		int i;
        for (i=k-1; i>=0 && ++comb[i]>n-1; --i)
            ;
		if (comb[0] > n-1)
			break;
		for (++i; i<k; ++i)
            comb[i] = comb[i-1];
    }
    return 0;
}
*/

int CEchoMainWnd::genCombination_checkToken(vector<vector<int>> &combs, int n, int k)
{
	vector<int> comb;
	combs.clear();
    for (int i=0; i<k; ++i)
		comb.push_back(0);
    while (1) {
		bool fDupToken = false;
		if (!fAllowSameToken) {
			fDupToken = false;
			for (int i=0; i<k; ++i)
				for (int j=i+1; j<k; ++j)
					if (wavID2tokenID[comb[i]] == wavID2tokenID[comb[j]])
						fDupToken = true;
		}
		if (!fDupToken)
			combs.push_back(comb);
		int i;
        for (i=k-1; i>=0 && ++comb[i]>n-1; --i)
            ;
		if (comb[0] > n-1)
			break;
		for (++i; i<k; ++i)
	        comb[i] = comb[i-1];
    }
    return 0;
}

int generatePresentOrders(int *orders, int length, int range)
{
	int cnt = 0;

	// randperm() produces a permuted series of integer array between 1 and nCombo (but only the first nTrials elements).
	if (randperm (orders, length, range)<0) {
		MainAppWnd.MessageBox ("un-initialized presentOrder", "randomizeList failed");
		return 0;
	}
	return 1;
}

int CEchoMainWnd::GenerateRandomList ()
{
	int i, j, k, org_nTrials;

	genCombination_checkToken(Combs, ss->nCombo, nConc);

	if (!multi) {
		if (!generatePresentOrders(presentOrder, nTrials, Combs.size()))
			return 0;
	} else {
		org_nTrials = nTrials / hMulti->nPresets / hMulti->nAddprocs;
		// First, make presentOrder (with org_nTrials length) with nPresets*nAddprocs (nTrials) times with randperm
		// and align it sequentially with idPresets and idAddProcs then permute the order again.
		// so that each presentation is balanced across idPresets and idAddProcs.

		int *tempOrders;
		for (k=0; k<hMulti->nPresets; k++)
			for (j=0; j<hMulti->nAddprocs; j++) {
				tempOrders = presentOrder+org_nTrials*(j+hMulti->nAddprocs*k);
				if (!generatePresentOrders(tempOrders, org_nTrials, Combs.size()))
					return 0;
			}
		// aligned indices
		for (i=0; i<hMulti->nPresets; i++) {
			memset(idPresets+org_nTrials*hMulti->nAddprocs*i, i, org_nTrials*hMulti->nAddprocs);
			for (j=0; j<hMulti->nAddprocs; j++)
				memset(idAddProcs+org_nTrials*(hMulti->nAddprocs*i+j), j, org_nTrials);
		}
		// permute presentOrder with idPresets and idAddProcs
		permute(presentOrder, idPresets, idAddProcs, nConc, nTrials);
	}
	return 1;
}

int	CEchoMainWnd::SeeFolder (int control, char *path)
{
	// returns the number of files in the list.
	WIN32_FIND_DATA fd;
	HANDLE hFind;
	HWND hEdit;
	int b, cum=0;
	char buf[128];
	__int64 fSize;

	strcpy(buf, path);

	hEdit = GetDlgItem(control);
	SetWindowText (hEdit, "");

	if (buf[strlen(buf)-1]!='\\')
		strcat(buf,"\\");
	strcat (buf, "*.*");
	hFind = FindFirstFile (buf, &fd);
	b = 1;

	while (hFind != INVALID_HANDLE_VALUE && b)
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			fSize = fd.nFileSizeLow + fd.nFileSizeHigh*0xffffffff;
			wsprintf (buf, "%16s%7d\r\n", fd.cFileName, fSize);
			EditPrintf (hEdit, buf);
			cum++;
		}
		b = FindNextFile (hFind, &fd);
	}
	return cum;
}

int CEchoMainWnd::FillDesireWavList (int control, int control2)
{
	int i, nList = DecodeNameRuleAndGenerateList (ss->nameRule, waveNameList, wavID2tokenID, ss);
	ClearEditPrintf(GetDlgItem(control));
	for (i=0; i<nList; i++)
		EditPrintf (GetDlgItem(control), "%s\r\n", waveNameList[i].c_str());

	SetDlgItemInt (control2, i, FALSE);
	return i; // Number of combos
}

int CEchoMainWnd::CheckWavFilesReadyAndDisplay (int control, map<int,string> &waveNameList, char *missingFiles)
{
	int i;
	WIN32_FIND_DATA fd;
	char buf1[256];

	strcpy(missingFiles, "");

	for (i=0; i<ss->nCombo; i++)
	{
		append_path_file(buf1, ss->wavePath, waveNameList[i].c_str());
		if (FindFirstFile (buf1, &fd)==INVALID_HANDLE_VALUE)
			strcat(missingFiles,(waveNameList[i]+"\r\n").c_str());
	}
	if (strlen(missingFiles)==0)
	{
		SendMessage (WM__WAV_LIST_READY, 1);
		SetDlgItemText (control, "All wav files exist.");
		return 1;
	}
	else
	{
		SendMessage (WM__WAV_LIST_READY, 0);
		SetDlgItemText (control, "NOT all wav files found. Click here details.");
		return 0;
	}
}

void CEchoMainWnd::CopyEchoStimsIfNecessary(char* wavname)
{
	char command[128], wavnamenoext[256], *temp[8], buf0[256], buf1[256];
	int nItems;
	temp[0] = buf0; temp[1] = buf1;
	size_t len = strlen(wavname);

	GetDlgItemText (IDC_PROC, command, 128);	
	for (int i=0; i<8; i++) temp[i] = new char[256];
	nItems=str2strarray(temp, 8, command, " ");
	strcpy(buf0, STIM_PATH_FILE);
	if (!_stricmp(temp[0],"WaveAdd"))
	{
		for (int i=1; i<nItems; i++)	
		{
			strcpy(buf1, buf0);
			if (len>5 && !stricmp(wavname+len-4,".wav"))
			{strncpy(wavnamenoext, wavname, len-4); wavnamenoext[len-4]=0;}
			else
				strcpy(wavnamenoext, wavname);
			if (temp[i][0]!='*')	
			{
				strcat(buf1, temp[i]);  strcat(buf1, ".wav");
				strcat(wavnamenoext,  temp[i]); strcat(wavnamenoext, ".wav");
				if (CopyFile(wavnamenoext, buf1, 0)==0)
				{
					MessageBox (wavnamenoext, "Could not copy", MB_OK);
					break;
				}
			}
		}
	}
	for (int i=0; i<8; i++) delete[] temp[i];
}

int CEchoMainWnd::GetNConcfromProcStr(char *errstr)
{
	CString str, div[2];
	int res(1), id, count, read, imax(-1);
	GetDlgItemText (IDC_PROC, str);

	count = countstr(str, "TG");
	bool *holder = new bool[count+1];
	for (int i=0; i<count; i++) holder[i] = false;

	//first find TG and if a numeric char follows right after that
	while ((id = str.Find("TG"))!=-1)
	{
		str = str.Mid(id+1);
		id = str2array(div, 2, str, " .+=-*/\r\n\t") - 2;
		div[0] = div[0].Mid(1);
		if (sscanf(div[0], "%d", &read)>0)
		{
			if (read<=0) { sprintf(errstr, "invalid: TG%d (must be positive)", read); res = 0; break;}
			if (read<=count)		holder[read-1]=true;
			else { sprintf(errstr, "TG%d but not all others were found", read); res = 0; break;}
			imax = max(imax, read);
		}
	}
	for (int i=0; res>0 && i<imax; i++)
	{
		if (!holder[i]) 
		{
			sprintf(errstr, "TG%d not found", i+1);
			res=0;
		}
	}
	if (res!=0) { errstr[0]=0; res = max(imax, 1);}
	delete[] holder;
	return res;
}

void CEchoMainWnd::appendLine2File(char *filename, char *str)
{
	HANDLE hFile;
	DWORD dw;
	int i;
	hFile = CreateFile (filename, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	i=SetFilePointer(hFile, NULL, NULL, FILE_END);
	i=WriteFile(hFile, str, strlen(str), &dw, NULL);
	i=WriteFile(hFile, "\r\n", 2, &dw, NULL);
	CloseHandle(hFile);
}