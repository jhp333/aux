#include "auxgv.h"
#include "auxgvDlg.h"

HWND hPlotDlgCurrent;

int FileDlg(CFileDlg fdlg, char *fullfname, char *fname, int idc)
{
	switch (idc)
	{
	case IDC_OPEN:
		return fdlg.FileOpenDlg(fullfname, fname, "text file (*.TXT)\0*.txt\0", "txt");
		break;
	case IDC_SAVE:
		return fdlg.FileSaveDlg(fullfname, fname, "text file (*.TXT)\0*.txt\0", "txt");
		break;
	}
	return 0;
}
CAuxDlg::CAuxDlg(void)
{
}

CAuxDlg::~CAuxDlg(void)
{
}

BOOL CAuxDlg::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
	char errstr[256], buf[256];
	string readstr;

	AstSig.SetPath("wav", AppPath);
	AstSig.SetPath("aux", AppPath);
	AstSig.SetPath("txt", AppPath);
	sprintf(buf, "%s%s", AppPath, INIFILE);
	readINI(buf, errstr);
/*
	eFont = CreateFont(16,0,0,0, FW_MEDIUM, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		FIXED_PITCH | FF_MODERN, FONTS_DEF);
*/
	GetModuleFileName(hInst, buf, sizeof(buf));
	getVersionStringAndUpdateTitle (hDlg, buf, VerStr);

	return 1;
}

void CAuxDlg::OnSize(UINT state, int cx, int cy)
{
	int sbarWidth[2];
	::MoveWindow (hStatusbar, 0, cy - 10, cx,	 cy, TRUE);
	sbarWidth [0] = cx / 2;
	sbarWidth [1] = -1;
	::SendMessage (hStatusbar, SB_SETPARTS, 2, (LPARAM)sbarWidth);
}


void CAuxDlg::OnClose()
{
	::DestroyWindow(hDlg);
}

void CAuxDlg::OnDestroy()
{
	char errstr[256], buf[256], msg[256];
	sprintf(buf, "%s%s", AppPath, INIFILE);
	if(!writeINI(buf, errstr))
	{
		sprintf(msg, "Exiting.... %s (%s)", errstr, buf);
		MessageBox(msg);
	}
	for (int i=0; i<nFigureWnd; i++) delete figwnd[i];
	delete[] figwnd;
	PostQuitMessage(0);
}

bool CAuxDlg::PrepareSig()
{
	try {
		AstSig.Reset(fs);
		AstSig.SetNewScript(m_auxstr.c_str());
		Sig = AstSig.Compute();
		Sig.MakeChainless();
	} catch (const char *errmsg) {
		MessageBox(errmsg, "Error in AUX script", MB_ICONERROR);
		return false;
	}
	return true;
}


int CAuxDlg::readINI(char *fname, char *estr)
{
	char errStr[256];
	string strRead;
	int val;
	double dval;
	int res = ReadINI (errStr, fname, "SAMPLE RATE", strRead);
	if (res>0 && sscanf(strRead.c_str(), "%d", &val)!=EOF && val >10)	fs = val;
	else																fs = DEFAULT_FS;
	res = ReadINI (errStr, fname, "PLAYBACK BLOCK SIZE MILLISEC", strRead);
	if (res>0 && sscanf(strRead.c_str(), "%f", &dval)!=EOF && dval >10.)	block = val;
	else																block = DEFAULT_BLOCK_SIZE;

	if (ReadINI (errStr, fname, "AUX", strRead)>=0)
		m_auxstr = strRead; 
	else if (ReadINI (errStr, fname, "VAR", strRead)>=0)	{
		m_auxstr = strRead;
		if (ReadINI (errStr, fname, "SIGNAL", strRead)>=0) {
			m_auxstr += "\r\n" + strRead;
			printfINI (errStr, fname, "SIGNAL", "");
		}
		printfINI (errStr, fname, "VAR", "");
	} else {
		strcpy(estr, errStr);
		return 0;
	}
	SetDlgItemText(IDC_AUX, m_auxstr.c_str());
	return 1;
}

int CAuxDlg::writeINI(char *fname, char *estr)
{
	char errStr[256];
	CStdString in;
	if (!printfINI (errStr, fname, "SAMPLE RATE", "%d", fs)) {strcpy(estr, errStr); return 0;}
	if (!printfINI (errStr, fname, "PLAYBACK BLOCK SIZE MILLISEC", "%f", block)) {strcpy(estr, errStr); return 0;}
	GetDlgItemText(IDC_AUX, in);
	if (!printfINI (errStr, fname, "AUX", "%s", in.c_str())) {strcpy(estr, errStr); return 0;}
	return 1;
}

void CAuxDlg::OnCommand(int idc, HWND hwndCtl, UINT event)
{
	char errstr[256], fname[MAX_PATH], fullfname[MAX_PATH];
	DWORD gle;
	INT_PTR res;
	CEqSig ord;
	CSignals dbclicked;
	CStdString in, sigstr;
	string parse[3]; 
	switch (idc)
	{
	case IDC_MAKE_PLOT:
		GetDlgItemText(IDC_AUX, in);
		m_auxstr = in.c_str();
		if (!PrepareSig())
			break;
		if (Sig.GetFs() <= 2 || Sig.CalculateTotalDuration()== 0) {
			char buf[1000];
			if (Sig.CalculateTotalDuration()== 0)
				break;//strcpy(buf, "null");
			else if (Sig.IsScalar()) {
				sprintf(buf, "%f", Sig.value());
			} else if (Sig.GetFs() == 2) {
				strcpy(buf, "String \"");
				Sig.getString(buf+strlen(buf), sizeof(buf)/sizeof(*buf)-(int)strlen(buf));
				strcat(buf, "\"");
			} else if (Sig.GetLength() > 0) {
				int n = Sig.GetLength();
				sprintf(buf, "vector (%d)\n", n);
				for (int i=0; i<min(n,10); ++i)
					sprintf(buf+strlen(buf), "%f\n", Sig.buf[i]);
				if (n>10)
					strcat(buf, "...");
			}
			MessageBox(buf, "Result", MB_ICONINFORMATION);
			break;
		}
		Sig.MakeChainless();
		// get the last line into sigstr.
		for (size_t pp,p=in.length()+1; sigstr.length()==0; ) {
			pp = p;
			p=in.rfind("\r\n", p-1);
			if (p == CStdString::npos) {
				sigstr = in.substr(0, pp);
				break;
			}
			sigstr = in.substr(p+2, pp-p-2);
		}
		if (int len = Sig.GetLength())
		{
			RECT rt = {0, 0, 600, 400};
			HANDLE fig = OpenFigure(&rt, Sig, 0, block);
			HANDLE ax = AddAxis (fig, .08, .1, .86, .8);
			AddText(fig, sigstr, .02, .02, 0, 0);
			double *x = new double[len];
			for (int i=0; i<len; i++)
				x[i] = (double)i/Sig.GetFs();
			PlotDouble(ax, len, x, Sig.buf);
			if (Sig.GetNextChan())
				PlotDouble(ax, len, x, Sig.GetNextChan()->buf, RGB(200,0,50));
			SetRange(ax, 'y', -1, 1);
			SetTick(ax, 'y', 0, 1, 4);
			SetTick(ax, 'x', 0, x[len-1]/10, 1, 0, "%6.3f");
			delete[] x;
			hAccel = GetAccel(curFig = fig);
			hPlotDlgCurrent = GetHWND_PlotDlg(curFig);

			//CFigure **oldList = new CFigure *[++nFigureWnd];
			//for (int i=0; i<nFigureWnd-1; i++) oldList[i] = figwnd[i];
			//oldList[nFigureWnd-1] = curFig;
			//delete[] figwnd;
			//figwnd = oldList;
		}
		break;

	case ID_SETFS:
		res = DialogBoxParam(hInst, "setfsdlg", hDlg, setfsproc, (LPARAM)&fs);
		if (res>0 && res !=fs)
			fs = (int)res;
		break;

	case ID_ABOUT:
		{
			CStdString msg, title = "AUX Viewer ";
			msg = title + VerStr;
			msg += "\n\nCopyright (C) 2010 Bomjun Kwon, Ph.D.";
			msg += "\nLicensed under the Academic Free License version 3.0";
			MessageBox(msg, "About " + title, MB_OK | MB_TOPMOST | MB_ICONINFORMATION);
		}
		break;

	case IDCANCEL:
	case IDM_DESTROY:
		SendMessage (WM_CLOSE);
		break;
	case IDC_OPEN:
	case IDC_SAVE:
		fname[0]='\0';fullfname[0]='\0';
		if (!FileDlg(fdlg, fullfname, fname, idc))
		{
			gle = GetLastError();
			if (gle!=0)
			{
				sprintf(errstr, "GetOpenFileName failed. code=%d", gle);
				MessageBox (errstr, "AUX viewer", MB_OK);	
			}
		}
		else
		{
			SetDlgItemText(IDC_FNAME, fname);
			OnFile(fullfname, idc);
		}
		break;
	}
}

void CAuxDlg::OnFile(char *fname, int idc)
{
	char errstr[256];
	switch (idc)
	{
	case IDC_OPEN:
		if (!readINI(fname, errstr))
			MessageBox (errstr);
		break;
	case IDC_SAVE:
		if (!writeINI(fname, errstr))
			MessageBox (errstr);
		break;
	}
}


HBRUSH CAuxDlg::OnCtlColor(HDC hdc, HWND hCtrl, int id)
{
	COLORREF col2, col = RGB(190,230, 130);
	int idc;
	col2 = GetSysColor (COLOR_BTNFACE);
	idc = (int)::GetWindowLong (hCtrl, GWL_ID);
	switch (idc)
	{
	case IDC_AUX:
		//SelectObject (hdc, eFont);
		SetBkColor (hdc, col);
		return (HBRUSH) CreateSolidBrush (col);
		break;
	default:
		SetBkColor (hdc, col2);
	}
	return (HBRUSH) CreateSolidBrush (col2);
}

HBRUSH CAuxDlg::OnCtlColorStatic(HDC hdc, HWND hCtrl, int id)
{
	return OnCtlColor(hdc, hCtrl, id);
}

HBRUSH CAuxDlg::OnCtlColorEdit(HDC hdc, HWND hCtrl, int id)
{
	return OnCtlColor(hdc, hCtrl, id);
}