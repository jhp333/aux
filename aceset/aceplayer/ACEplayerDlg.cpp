#include "aceplayer.h"
#include "ACEplayerDlg.h"

int strscanfINI (char *errstr, const char *fname, const char *heading, CStdString& outString);

void thread4mcr (PVOID h)
{
	CACEplayerDlg *hobj = (CACEplayerDlg *)h;
	char *buffer;

	if(hobj->InitMatlabLib(hobj->mxacedllname, &buffer)!=1)			
	{
		MessageBox(hobj->hDlg, buffer, "Error in InitMatlabLib", MB_OK);
		hobj->SendMessageA(WM_DESTROY);
	}
	else
	    SendMessage (hobj->hStatusbar, SB_SETTEXT, 1, (LPARAM)"READY FOR PROCESSING");

}

CACEplayerDlg::CACEplayerDlg(void)
:itype(UNSPECIFIED), nConsoles(0), param(NULL), param2(NULL)
{
	strcpy(PipeName,SIGNAL_INTERFACE_PIPENAME);
}

CACEplayerDlg::~CACEplayerDlg(void)
{
}

BOOL CACEplayerDlg::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
	uintptr_t hThread, hThread4mcr;
	POINT pt;
	char verStr[16], drive[4], dir[MAX_PATH], buf[MAX_PATH], ext[32], szbuffer[MAX_PATH];
	int i;
	char errstr[MAX_PATH];
	string str;

	GetModuleFileName (GetModuleHandle(NULL), szbuffer, sizeof(szbuffer));
	getVersionStringAndUpdateTitle(hDlg, szbuffer, verStr);
 	_splitpath(szbuffer, drive, dir, buf, ext);
	sprintf(AppPath,"%s%s", drive, dir);

	if (!ReadSettings(szbuffer)) {	// need this prior to CACEDlg::Init
		MessageBox (szbuffer, "Error while reading ini file", MB_ICONERROR);
		SendMessageA(WM_DESTROY);
		return 0;
	}

	if (!hACEobj[0].Init(aceprocdllname, buf))
	{
		MessageBox(buf, "Error in LoadLibrary", MB_ICONERROR);
		SendMessageA(WM_DESTROY);
		return 0;
	}
	hACEobj[1].Init(aceprocdllname, buf);

	pt.x=10; pt.y=0;
	hACEobj[0].Create(hDlg, pt);
	pt.x=670; pt.y=0;
	hACEobj[1].Create(hDlg, pt);

	DisplayInitScreen();

	InitMatlabLib = hACEobj[0].InitMatlabLib;
	CloseMatlabLib = hACEobj[0].CloseMatlabLib;
	FixCRCR = hACEobj[0].FixCRCR;
	GetCutoffFreqs = hACEobj[0].GetCutoffFreqs;
	GateProcess = hACEobj[0].GateProcess;
	newCParamACE = hACEobj[0].newCParamACE;

	hThread4mcr = _beginthread(thread4mcr, 0, (void*)this);

	hStatusbar = CreateWindow (STATUSCLASSNAME, "", WS_CHILD|WS_VISIBLE|WS_BORDER|SBS_SIZEGRIP,
		0, 0, 0, 0, hDlg, (HMENU)IDC_STATUSBAR, hInst, NULL);
	sbarWidth [0] = 200;
	sbarWidth [1] = -1;
	::SendMessage (hStatusbar, SB_SETPARTS, 2, (LPARAM)sbarWidth);
	::MoveWindow (hStatusbar, 0, 0, 0, 0, TRUE);
	::SendMessage (hStatusbar, SB_SETTEXT, 1, (LPARAM)"Wait for mcr intialization.....");

	switch(itype)
	{
	case SPRINT:
		if (!InitNICStream (hDlg, nicPATH, errstr)) {
			MessageBox(errstr, "NICstream Initialization error", MB_ICONERROR); 
			SendMessageA(WM_DESTROY);
			return 0;
		}
		::SendMessage (hStatusbar, SB_SETTEXT, 0, (LPARAM)"SPrint, CIC3");
		LoadString (hInst, IDS_STREAM_FNAME, szbuffer, sizeof(szbuffer));
		FulfillFile(streamFile, AppPath, szbuffer);
		break;
	case L34_CIC3:
	case L34_CIC4:
		FulfillFile(dir, AppPath, INIFILE);
		ReadINI(ext, dir, "PROCESSOR", str);
		if (!InitNIC2(hDlg, nicPATH, str.c_str(), errstr)) {
			MessageBox(errstr, "NIC2 Initialization error", MB_ICONERROR); 
			SendMessage(WM_DESTROY);
			return 0;
		}
		::SendMessage (hStatusbar, SB_SETTEXT, 0, (LPARAM)str.c_str());
		if (fMakeStreamFile) {
			LoadString (hInst, IDS_STREAM_FNAME2, szbuffer, sizeof(szbuffer));
			FulfillFile(streamFile, AppPath, szbuffer);
		} else
			streamFile = "";
		break;
	}
	
	hACEobj[0].param->dwReserved=(DWORD)itype;
	hACEobj[1].param->dwReserved=(DWORD)itype;
	hThread = _beginthread (pipeThread, 0, (void*)SIGNAL_INTERFACE_PIPENAME); 

	SendDlgItemMessage(IDC_MAPA, BM_SETCHECK, BST_CHECKED, 0);
	hPipeLog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_PIPEMSG), hDlg, (DLGPROC)PipeLogProc);
	hMATLABMSG = CreateDialog(hInst, MAKEINTRESOURCE(IDD_PIPEMSG), hDlg, (DLGPROC)PipeLogProc);
	hPulseView = CreateDialog(hInst, MAKEINTRESOURCE(IDD_PULSEVIEW), hDlg, (DLGPROC)PipeLogProc);

	SetWindowLong (GWL_USERDATA, (LONG)this);
	if ((i=initflySpecialPort (hDlg, "127.0.0.1", 1055, -1, false))!=FER_OK) 
	{	sprintf(buf, "initfly error", "code=%d", i);
		MessageBox (buf, "", MB_OK);
	}

	return 1;
}

void CACEplayerDlg::OnSize(UINT state, int cx, int cy)
{
	::MoveWindow (hStatusbar, 0, cy - 10, cx,	 cy, TRUE);
	sbarWidth [0] = cx / 2;
	sbarWidth [1] = -1;
	::SendMessage (hStatusbar, SB_SETPARTS, 2, (LPARAM)sbarWidth);
}

#define CHEC(x) {if(!(x)) {return 0;}}
#define CHEC2(x) {if(x==-1) {return 0;}}

int CACEplayerDlg::UpdateINI(char *errStr)
{
	CStdString cstr;
	char buf[MAX_PATH], fname[MAX_PATH], listname[MAX_PATH];
	int res, nItems;
	FulfillFile(fname, AppPath, INIFILE);
	CHEC(printfINI(errStr, fname, "WAVE PATH", "%s", fdlg.LastPath))
	res = BST_CHECKED==SendDlgItemMessage(IDC_ACOUSTIC_PLAY, BM_GETCHECK)? 1:0;
	CHEC(printfINI(errStr, fname, "ACOUST PRES", "%d", res))
	res = BST_CHECKED==SendDlgItemMessage(IDC_STAGGER, BM_GETCHECK)? 1:0;
	CHEC(printfINI(errStr, fname, "STAGGER", "%d", res))
	res = BST_CHECKED==SendDlgItemMessage(IDC_RADIO1, BM_GETCHECK)? 1:0;
	CHEC(printfINI(errStr, fname, "RMS SCALE", "%d", res))
	CHEC(printfINI(errStr, fname, "MAP A", "%s", hACEobj[0].MAPfilename))
	CHEC(printfINI(errStr, fname, "MAP B", "%s", hACEobj[1].MAPfilename))
	nItems = (int)SendDlgItemMessage(IDC_WAVE_LIST, LB_GETCOUNT);
	string str = "";
	for (int i=0; i<nItems; i++) {
		SendDlgItemMessage(IDC_WAVE_LIST, LB_GETTEXT, i, (LPARAM)listname);
		str += listname;
		str += "\r\n";
	}
	CHEC(printfINI(errStr, fname, "PLAY LIST", "%s", str.c_str()))
	sprintfFloat (dur_powerframes, 2, buf, sizeof(buf));
	CHEC(printfINI(errStr, fname, "POWER-UP FRAMES DURATION IN MS", "%s", buf))

	double tdbl = GetDlgItemDouble(hDlg, IDC_LEVEL, &res);
	if (res) {
		sprintfFloat (tdbl, 2, buf, sizeof(buf));
		CHEC(printfINI(errStr, fname, "LEVEL", "%s", buf))
	}
	GetDlgItemText(IDC_AUX, cstr);
	CHEC(printfINI(errStr, fname, "AUX PROCESSING", "%s", cstr.c_str()))

	hACEobj[0].Screen2Param();
	hACEobj[1].Screen2Param();
	CHEC(printfINI(errStr, fname, "ADDITIONAL M LINES A", "%s", hACEobj[0].param->addmlines))
	CHEC(printfINI(errStr, fname, "ADDITIONAL M LINES B", "%s", hACEobj[1].param->addmlines))
		
	return 1;
}

int CACEplayerDlg::ReadSettings(char *errStr)
{
	CStdString extra;
	string str("");
 	char fname[256], buffer[256];
	WIN32_FIND_DATA fd;
	errStr[0]=0;
	FulfillFile(fname, AppPath, INIFILE);
	// The following fields--PROCESSOR, NIC PATH, ACEPROC, MXACE--are only read from aceplayer.ini. 
	// The correct contents should be provided by a direct edit on aceplayer.ini. 
	// They will NOT be automatically updated in UpdateINI.
	if (ReadINI(errStr, fname, "PROCESSOR", str)>0)
	{
		extra = str;
		extra.MakeUpper();
		if (extra=="SPRINT") itype=SPRINT;
		else
		{
			if (extra.Left(9)=="L34-CIC3-") itype=L34_CIC3;
			else if (extra.Left(9)=="L34-CIC4-") itype=L34_CIC4;
			else 
			{
				itype=UNSPECIFIED;
				strcpy(errStr, "Invalid [PROCESSOR]");
				return 0;
			}
		}
	}
	else 		return 0;
	if (ReadINI(errStr, fname, "NIC PATH", str)>0)
	{
		FulfillFile(buffer, str.c_str(), "*.*");
		if (FindFirstFile(buffer, &fd)==INVALID_HANDLE_VALUE)	{
			strcpy(errStr, "Invalid NIC PATH");
			return 0;
		}
		strcpy(nicPATH, str.c_str());
	}
	else		return 0;
	if (ReadINI(errStr, fname, "ACEPROC", str)>0)
		strcpy(aceprocdllname, str.c_str());
	else		return 0;
	if (ReadINI(errStr, fname, "MXACE", str)>0)
		strcpy(mxacedllname, str.c_str());
	else return 0;

	fMakeStreamFile = 0;
	int ret = sscanfINI(errStr, fname, "MAKE STREAM FILE NIC2", "%d", &fMakeStreamFile);
	if (ret == -4)	// heading missing, create
		printfINI(errStr, fname, "MAKE STREAM FILE NIC2", "%d", 0);
	else if (ret < 0)
		return 0;
	return 1;
}


void CACEplayerDlg::DisplayInitScreen()
{
	int res, nItems;
	double dres;
	string str, str2, *lines;
	char drive[16], dir[256], ext[8], errStr[256], fname[256], buffer[256];
	FulfillFile(fname, AppPath, INIFILE);
	if (ReadINI(errStr, fname, "MAP A", str)>0)
	{
		OpenMAP(hACEobj[0].hDlg, str.c_str(), itype); 
	 	_splitpath(str.c_str(), drive, dir, buffer, ext);	str2 = (string)drive + dir;
		hACEobj[0].fdlg.InitFileDlg(hDlg, str2.c_str());
	}
	if (ReadINI(errStr, fname, "MAP B", str)>0)
	{
		OpenMAP(hACEobj[1].hDlg, str.c_str(), itype);  
	 	_splitpath(str.c_str(), drive, dir, buffer, ext);	str2 = (string)drive + dir;
		hACEobj[1].fdlg.InitFileDlg(hDlg, str2.c_str());
	}
	if(sscanfINI(errStr, fname, "ACOUST PRES", "%d", &res)>0)
		SendDlgItemMessage(IDC_ACOUSTIC_PLAY, BM_SETCHECK, res==1? BST_CHECKED:BST_UNCHECKED);
	if(sscanfINI(errStr, fname, "STAGGER", "%d", &res)>0)
		SendDlgItemMessage(IDC_STAGGER, BM_SETCHECK, res==1? BST_CHECKED:BST_UNCHECKED);
	if(sscanfINI(errStr, fname, "RMS SCALE", "%d", &res)>0)
		CheckRadioButton (hDlg, IDC_RADIO1, IDC_RADIO2, res ? IDC_RADIO1:IDC_RADIO2);

	if(sscanfINI(errStr, fname, "LEVEL", "%lf", &dres)<=0)
		dres = 70.0;	// default value
	CStdString cstr;
	sprintfFloat(dres, 1, cstr);
	SetDlgItemText(IDC_LEVEL, cstr);

	if(sscanfINI(errStr, fname, "POWER-UP FRAMES DURATION IN MS", "%lf", &dres)>0)
		dur_powerframes = dres ;
	else
		dur_powerframes = 200.;
	if (ReadINI(errStr, fname, "PLAY LIST", str)>0) {
		nItems = countDeliminators(str.c_str(), "\r\n");
		lines = new string[nItems];
		str2array(lines, nItems, str.c_str(), "\r\n");
		for (int i=0; i<nItems; i++)
			SendDlgItemMessage(IDC_WAVE_LIST, LB_ADDSTRING, 0, (LPARAM)lines[i].c_str());
		delete[] lines;
	}
	if (ReadINI(errStr, fname, "AUX PROCESSING", str)>0)
		SetDlgItemText(IDC_AUX, str.c_str());
	else
		SetDlgItemText(IDC_AUX, "");

	if (ReadINI(errStr, fname, "ADDITIONAL M LINES A", str)>0)
		hACEobj[0].SetAddMLines(str.c_str());
	else
		hACEobj[0].SetAddMLines("");
	if (ReadINI(errStr, fname, "ADDITIONAL M LINES B", str)>0)
		hACEobj[1].SetAddMLines(str.c_str());
	else
		hACEobj[1].SetAddMLines("");
	hACEobj[0].Param2Screen();
	hACEobj[1].Param2Screen();
}

void CACEplayerDlg::OnClose()
{
	char szbuffer[128];

	if (!UpdateINI(szbuffer))	MessageBox (szbuffer, "UpdateINI", MB_OK);

	if (itype==SPRINT)
		NICstreamCleanUp();
	else if (itype==L34_CIC3 || itype==L34_CIC4)
		if (!NIC2CleanUp()) MessageBox ("error", "NIC2CleanUp", MB_OK);
	CloseMatlabLib();
	DestroyWindow(hDlg);
	DestroyWindow(hSpDlg);
	DestroyWindow(hPipeLog);
	DestroyWindow(hMATLABMSG);
	DestroyWindow(hPulseView);
}

void CACEplayerDlg::OnDestroy()
{
	PostQuitMessage(0);
}

int CACEplayerDlg::Prepare (ACESEQ *seq, const char *wavName, double& duration, CStdString &resStr)
{	// out: seq, resStr
	// in: all the rest

	//Oct-28-2010. BJ Kwon
	//the level from the controller is ignored. It is determined only in aceplayer.

	CSignals wavdata, wavdata2;
	CStdString str;
	int res, nOutputChan;
	bool deletetempfile(false), hiace(false);
	const char *fullname;
	char buf[MAX_PATH*20], verstr[64], fname[MAX_PATH], fext[MAX_PATH], *errstr, *warnstr, errStr[512];

	resStr ="";

	if ((nOutputChan=GetACEparams(buf))==0) { resStr += buf; return 0;}

	int trans;
	param->level = GetDlgItemDouble(hDlg, IDC_LEVEL, &trans);
	if (!trans) {
		errstr="Invalid Presentation level";
		return 0;
	}

	if (strlen(param->subj)==0)
	{	resStr += "Presenter parameters not loaded."; return 0;}
	if (!wavdata.Wavread(wavName, buf))  { resStr += buf; return 0;}

	double sss = wavdata.LevelLT();
	double sss2 = wavdata.Levels();

	param->rms_scaled = (SendDlgItemMessage(IDC_RADIO1, BM_GETCHECK) == BST_CHECKED);
	if (param->rms_scaled)
		param->level -= wavdata.LevelLT();
	if (nOutputChan==2)	{
		param2->rms_scaled = param->rms_scaled;
		param2->level = param->level;
	}

	if (wavdata.GetNextChan()==NULL) // MONO file
		{
			if (param->ptParam2!=NULL) // multi processing. 2-ch wave file (with duplicate monos) is created.
			{
				if (strlen(param->ptParam2->subj)==0)
				{	resStr += "Presenter 2nd parameters not loaded."; return 0;}
				_splitpath (wavName, NULL, NULL, fname, fext);
				strcat(fname, fext);
				FulfillFile(str, AppPath, fname);
				fullname = str.c_str();
				wavdata2 = wavdata;
				wavdata.SetNextChan(&wavdata2);
				if (!wavdata.Wavwrite(fullname, buf)) { resStr += buf; return 0;}
				else deletetempfile=true;
			}
			else // single processing, no temporary wav file is generated
			{
				fullname = wavName;
			}
		}
	else // STEREO file
	{
		if (param->ptParam2!=NULL) // no temporary wav file is generated
		{
			fullname = wavName;
		}
		else // single processing, 2nd ch ignored
		{
			_splitpath (wavName, NULL, NULL, fname, fext);
			strcat(fname, fext);
			FulfillFile(str, AppPath, fname);
			fullname = str.c_str();
			wavdata.SetNextChan(NULL);
			if (!wavdata.Wavwrite(fullname, buf)) { resStr += buf; return 0;}
			else deletetempfile=true;
		}
	}
	if (is_wACE(param))
		strcpy(buf,W_ACE);
	else if (itype==SPRINT)		
	{
		strcpy(buf,"NICstream");
	}
	else if (itype==L34_CIC3 || itype==L34_CIC4)	strcpy(buf,"NIC2");
	res = GateProcess(seq, buf, fullname, streamFile.c_str(), param, &errstr, &warnstr);
	::SetDlgItemText(hMATLABMSG, IDC_MSG,"");
	if (strlen(warnstr)>0)
		EditPrintf (::GetDlgItem(hMATLABMSG, IDC_MSG),"Warning: %s", warnstr);
	if (strlen(errstr)>0)
		EditPrintf (::GetDlgItem(hMATLABMSG, IDC_MSG),"%s", errstr);
	if (deletetempfile) DeleteFile(fullname);
	resStr += errstr;
	if (res<=0) 
		return 0;
	else // success
	{
		GetWindowText(hDlg, verstr, sizeof(verstr));
		genProcessingString(buf, sizeof(buf), param, nOutputChan, wavdata.LevelLT());
		resStr.Format("SUCCESS %s, %s, %s\r\n%s", verstr, aceprocdllname, mxacedllname, buf);
		sprintfFloat(param->level+wavdata.LevelLT(), 1, buf, sizeof(buf));
		DisplayPulses(seq, buf);
	}
	duration = (double)wavdata.nSamples / (double)wavdata.GetFs();
	if (BST_CHECKED==SendDlgItemMessage(IDC_ACOUSTIC_PLAY, BM_GETCHECK))
		wavdata.PlayArray(0, WM_APP, hDlg, 1, errStr);
	return 1;
}



void printSeq(ACESEQ *seq)
{
	char buf[2048];
	FILE* fp=fopen("sdf.txt", "w");
	fprintf(fp,"New starts here-----\n");
	fprintf(fp,"Length of mode = %d, period = %d,\n",seq->lenMode, seq->lenPeriod);
	if (seq->lenMode>1 && seq->lenPeriod>1)
		for (int i=0; i<seq->length; i++)
			fprintf(fp, "%d\t%d\t%d\t%8.1f\n", (int)seq->ae[i], (int)seq->mode[i], (int)seq->cl[i], seq->period[i]);
	else if (seq->lenPeriod==1)
	{
		fprintf(fp,"period = %8.1f\n", *(seq->period));
		if (seq->lenMode>1)
		{
			for (int i=0; i<seq->length; i++)
				fprintf(fp, "%d\t%d\t%d\n", (int)seq->ae[i], (int)seq->mode[i], (int)seq->cl[i]);
		}
		else
		{
			fprintf(fp,"mode = %d\n", (int)*seq->mode);
			for (int i=0; i<seq->length; i++)
				fprintf(fp, "%d\t%d\n", (int)seq->ae[i], (int)seq->cl[i]);
		}
	}
	else 
	{
		sprintf(buf,"period = %8.1f\n", *seq->period);
		fprintf(fp,"period = %8.1f\n", *seq->period);
		for (int i=0; i<seq->length; i++)
		{
sprintf(buf,"%d\t%d\t%8.1f\n", (int)seq->ae[i], (int)seq->cl[i], seq->period[i]);
			fprintf(fp, "%d\t%d\t%8.1f\n", (int)seq->ae[i], (int)seq->cl[i], seq->period[i]);
		}
	}
	fclose(fp);
}

int CACEplayerDlg::Present(ACESEQ *seq, CStdString &resStr)
{
//	printSeq(seq); // for debuggin
	int res;
	char errstr[MAX_PATH];

	resStr.Format("Presenting... duration= %.1f s", duration);
	::SendMessage (hStatusbar, SB_SETTEXT, 1, (LPARAM)resStr.c_str());

	if (itype==SPRINT)
		res = NICStreamIt(streamFile, errstr);
	else // assuming L34
		res = NIC2StreamIt(seq, dur_powerframes, errstr, true);
//		res = NIC2StreamIt(streamFile, errstr, true);
	if (res) {
		resStr += "   done";
		::SendMessage (hStatusbar, SB_SETTEXT, 1, (LPARAM)resStr.c_str());
		resStr = "SUCCESS";
	} else
		resStr.Format("FAILURE %s", errstr); 
	return res;
}

void CACEplayerDlg::OnFlyArrived(WORD command, WORD len, void* inBuffer)
{
	int ch, cl1, cl2, nArg, nLines, res(1);
	double f1, f2;
	CStdString outStr, eachLine[16], cmd[16];
	nLines = str2array(eachLine, 16, (char*)inBuffer, "\r\n");
	for (int i=0; i<nLines; i++)
	{
		nArg = str2array(cmd, 16, eachLine[i], " \t");
		if (cmd[0]=="OPEN")
		{
			CACEDlg * hAceObj;
			if (nArg<3) { outStr="Two arguments required."; res=0;}
			else 
			{
				if (cmd[1]=="A") hAceObj = &hACEobj[0];
				else if (cmd[1]=="B") hAceObj = &hACEobj[1];
				else { outStr="1st argument must be A or B."; res=0;}
				hAceObj->OpenMap(cmd[2].c_str(), itype);
			}
		}
		else if (cmd[0]=="ATTENUATE")
		{
			if (nArg<3) { outStr="Two arguments required."; res=0;}
			else if (sscanf(cmd[1].c_str(), "%d", &cl1)!=1) { outStr="1st argument invalid."; res=0; }
			else if (sscanf(cmd[2].c_str(), "%d", &cl2)!=1) { outStr="2nd argument invalid."; res=0; }
			else
			{
				SetDlgItemInt(IDC_ATTEN_A, cl1);
				SetDlgItemInt(IDC_ATTEN_B, cl2);
			}
		}
		else if (cmd[0]=="MAP")
		{
			if (nArg<2) { outStr="One argument required (MAP A or B?)"; res=0; }
			else 
			{
				if (cmd[1]=="A") 
					OnCommand(IDC_MAPA, GetDlgItem(IDC_MAPA), BN_CLICKED);
				else if (cmd[1]=="B") 
					OnCommand(IDC_MAPB, GetDlgItem(IDC_MAPB), BN_CLICKED);
				else { outStr="Argument should be A or B."; res=0; }
			}
		}
		else if (cmd[0]=="PLAY")
		{
			if (nArg<2) { outStr="Index of item to play?"; res=0; }
			else 
			{
				if (sscanf(cmd[1].c_str(), "%d", &cl1)!=1) 
					{ outStr="invalid argument."; res=0; }
				else
				{
					SendDlgItemMessage(IDC_WAVE_LIST, LB_SETCURSEL, (WPARAM)cl1);
					OnCommand(IDC_WAVE_LIST, GetDlgItem(IDC_WAVE_LIST), LBN_DBLCLK);
				}
			}
		}
		else if (cmd[0]=="FREQ")
		{
			if (nArg<4) { outStr="THREE arguments required."; res=0;}
			else 
			{
				if (cmd[1]!="A" && cmd[1]!="B") 
				{ outStr="Argument should be A or B."; res=0; }
				else
				{
					if (sscanf(cmd[2].c_str(), "%lf", &f1)!=1 || sscanf(cmd[3].c_str(), "%lf", &f2)!=1)
						{ outStr="invalid argument."; res=0; }
					else
					{
						HWND hWnd = cmd[1]=="A" ? hACEobj[0].hDlg : hACEobj[1].hDlg;
						SelectFreqLimits(hWnd, f1, f2);
					}
				}
			}
		}
		else if (cmd[0]=="ELEC")
		{
			if (nArg<3) { outStr="Two arguments required."; res=0;}
			else 
			{
				if (cmd[1]!="A" && cmd[1]!="B") 
				{ outStr="Argument should be A or B."; res=0; }
				else
				{
					if (cmd[2]!="ON" && cmd[2]!="OFF") 
					{ outStr="Argument should be ON or OFF."; res=0; }
					else
					{
						if (sscanf(cmd[3].c_str(), "%d", &ch)!=1 || ch>22 || ch<0)
					{ outStr="invalid argument."; res=0; }
					else
					{
						HWND hWnd = cmd[1]=="A" ? hACEobj[0].hDlg : hACEobj[1].hDlg;
							int check = cmd[2]=="ON" ? 1:0;
							SelectElectrode(hWnd, ch, check);
						}
					}
				}
			}
		}
		else 
		{ outStr="Invalid console string"; res=0; }
	}
	if (!res) MessageBox (outStr);
}

void CACEplayerDlg::OnFlyClosed()
{
	nConsoles--;
}

void CACEplayerDlg::OnFlyConnected(char *hostname, char* ipa)
{
	nConsoles++;
}

#define DUMMY (WORD)0

int CACEplayerDlg::Set (CStdString *pstr, int nArg, CStdString &resStr)
{
	CStdString longbuf("");
	if (nArg==0) { resStr = "Empty pipe string"; return 0;}
	else if (pstr[0]=="SET")
	{
		if (nArg==1) { resStr = "SET must have more than one argument"; return 0;}
		else if (pstr[1]=="MAP")
		{
			if (nArg==2) { resStr = "SET MAP A or B?"; return 0;}
			else if (pstr[2]=="A")	OnCommand(IDC_MAPA, GetDlgItem(IDC_MAPA), BN_CLICKED);
			else if (pstr[2]=="B")	OnCommand(IDC_MAPB, GetDlgItem(IDC_MAPB), BN_CLICKED);
			else { resStr = "SET MAP must follow an argument A or B?"; return 0;}
		}
		else if (pstr[1]=="OPEN")
		{
			for (int i=1; i<nArg; i++)	longbuf += pstr[i] + " ";
			OnFlyArrived(DUMMY, (WORD)longbuf.length(), (void*)longbuf.c_str());
		}
		else if (pstr[1]=="MATCOM")
		{
			if (nArg==2) { resStr = "SET MATLABCOM A or B?"; return 0;}
			else 
			{
				CACEDlg * hAceObj;
				for (int i=3; i<nArg; i++)	longbuf += pstr[i] + " ";
				if (pstr[2]=="A")	hAceObj = &hACEobj[0];
				else if (pstr[2]=="B")	hAceObj = &hACEobj[1];
				else { resStr = "SET MATLABCOM must follow an argument A or B."; return 0;}
				hAceObj->SetAddMLines (longbuf.c_str());
				hAceObj->Param2Screen();
			}
		}
		else if (pstr[1]=="AUX")
		{
			for (int i=2; i<nArg; i++)	longbuf += pstr[i] + " ";
			SetDlgItemText(IDC_AUX, longbuf.c_str());
		}
	}
	return 1;
}

void CACEplayerDlg::swapAB()
{
	hACEobj[0].Screen2Param();
	hACEobj[1].Screen2Param();

	CParamACE *temp;
	temp = hACEobj[0].param;
	hACEobj[0].param = hACEobj[1].param;
	hACEobj[1].param = temp;

	char tbuf[MAX_PATH];
	strncpy(tbuf, hACEobj[0].MAPfilename, sizeof(tbuf));
	strncpy(hACEobj[0].MAPfilename, hACEobj[1].MAPfilename, sizeof(tbuf));
	strncpy(hACEobj[1].MAPfilename, tbuf, sizeof(tbuf));

	//CStdString tcstr;
	//tcstr = hACEobj[0].m_addmlines;
	//hACEobj[0].SetAddMLines(hACEobj[1].m_addmlines);
	//hACEobj[1].SetAddMLines(tcstr);

	hACEobj[0].Param2Screen();
	hACEobj[1].Param2Screen();
}