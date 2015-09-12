// psyconDlg.cpp : implementation file
//

#include "stdafx.h"
#include "psycon.h"
#include "psyconDlg.h"
#include "settings.h"
#include "mmsystem.h"
#include <process.h>
#include <math.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CPsyconApp theApp;

#define HALFDONE			16
#define DUALDONE			 8
#define SINGLEDONE			 1
#define ADDLONGBUF			1010
#define STOPPED				0
#define NOT_STOPPED			1119

#define FONTS_DEF "MS Reference Sans Serif"

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString st = "Psycon ";
	st += theApp.VerStr;
	SetDlgItemText(IDC_STATIC_PSYCON, st);
	return TRUE;
}
// CPsyconDlg dialog

IMPLEMENT_DYNAMIC(CPsyconDlg, CDialog)

CPsyconDlg::CPsyconDlg(CWnd* pParent /*=NULL*/)
: CDialog(CPsyconDlg::IDD, pParent)
, sgen0(&AstSig), sgen1(&AstSig) 
, m_figProc(NULL), m_axProc(NULL), trial(-1), nIntervals(0), destroyedWind(0), ascontroller(false)
, report("Psycon")
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_PSYCON);
	m_strRef = _T("");
	m_strCont = _T("");
	m_strTagDef = m_strTagDef2 = _T("");
	m_controller_node = _T("");
	intervalDelay = 0;
	trialDelay = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	fs = 22050;
	variable=.5;	
	toggle=0; // toggle: 0 for primary sequence 1 for secondary
	saveDetails=0;
	m_bClose = 0;
	scores = (double*)calloc(1, sizeof(double));
	PresentingIDOrder = (char*)calloc(1,1);
	cumTrials = new short[1];
	cumCorr = new short[1];
	intervalsig = new CSignals[1];

	//Initialize random
	srand((unsigned int)time(NULL) ^ (unsigned int)GetCurrentThreadId());

	memset((void*)banchor, 0, 8);
	SigEditFont.CreateFont(16,7,0,0, FW_MEDIUM, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		FIXED_PITCH | FF_MODERN, FONTS_DEF);
}

CPsyconDlg::~CPsyconDlg()
{
	delete[] m_figProc;
	SigEditFont.DeleteObject();
	delete[] devCaptions;
	free(scores);
	free(PresentingIDOrder);
	for (int i=0; i<theApp.nPlotWindows; i++)	delete[] m_figView[i];
	delete[] intervalsig;
	delete[] cumTrials;
	delete[] cumCorr;
}

void CPsyconDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPsyconDlg)
	DDX_Control(pDX, IDC_SIGNAL_REF, m_edit4plot2);
	DDX_Control(pDX, IDC_SIGNAL_CON, m_edit4plot1);
	DDX_Text(pDX, IDC_SIGNAL_REF, m_strRef);
	DDX_Text(pDX, IDC_SIGNAL_CON, m_strCont);
	DDX_Text(pDX, IDC_SIGNAL_TAG, m_strTagDef);
	DDX_Text(pDX, IDC_SIGNAL_TAG2, m_strTagDef2);
	DDX_Text(pDX, IDC_NUM_INTERVALS, nIntervals);
	DDX_Text(pDX, IDC_ANCHOR_INT, m_anchorStr);
	DDX_Control(pDX, IDC_LOG, m_logEdit);
	DDX_Control(pDX, IDC_TASKTAB, m_tab);
	DDX_Check(pDX, IDC_FEEDBACK, feedback);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SIGNAL_TAG, m_edit_tags);
	DDX_Control(pDX, IDC_SIGNAL_TAG2, m_edit_tags2);
}


BEGIN_MESSAGE_MAP(CPsyconDlg, CDialog)
	//{{AFX_MSG_MAP(CPsyconDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_TIMER()
	ON_EN_UPDATE(IDC_TALK, OnUpdateTalk)
	ON_EN_CHANGE(IDC_SIGNAL_CON, OnChangeSignalCon)
	ON_EN_CHANGE(IDC_SIGNAL_REF, OnChangeSignalRef)
	ON_WM_PAINT()
	ON_COMMAND(IDM_FILE_OPENSTIM, OnFileOpenStim)
	ON_COMMAND(IDM_FILE_SAVESTIM, OnFileSaveStim)
	ON_COMMAND(IDM_FILE_SETTINGS, OnFileSettings)
	ON_COMMAND(IDM_FILE_EXIT, OnFileExit)
	ON_COMMAND(IDM_HELP_WHATSNEW, OnHelpWhatsNew)
	ON_COMMAND(IDM_HELP_ABOUT, OnHelpAbout)
//	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_MESSAGE (WM_FL_CONNECTED, OnFlyConnected)
	ON_MESSAGE (WM_FL_CLOSE, OnFlyClosed)
	ON_MESSAGE (WM_FL_ARRIVED, OnFlyArrived)
	ON_MESSAGE (WM_FL_NETWORK_ERR, OnFlyErr)
	ON_MESSAGE (WM__NEXT_INTERVAL, OnNext_Interval)
	ON_MESSAGE (WM__SELCHANGE, OnSelChange)
//	ON_NOTIFY(TCN_SELCHANGE, IDC_TASKTAB, &CPsyconDlg::OnTcnSelchangeTasktab)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_GO, &CPsyconDlg::OnBnClickedGo)
	ON_BN_CLICKED(IDC_STOP, &CPsyconDlg::OnBnClickedStop)
	ON_EN_CHANGE(IDC_SIGNAL_TAG, &CPsyconDlg::OnEnChangeSignalTag)
	ON_WM_GETDLGCODE()
	ON_WM_KILLFOCUS()
//	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_NUM_INTERVALS, &CPsyconDlg::OnEnChangeNumIntervals)
END_MESSAGE_MAP()


// CPsyconDlg message handlers

int CALLBACK  proc(CONST LOGFONT *lplf, CONST  TEXTMETRIC *lptm, DWORD dwType, LPARAM lpData)
{
	static int count(0);
	return count++;
}

void CPsyconDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

BOOL CPsyconDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// set MENU here
	menu0.LoadMenu(IDR_MENU1);
	SetMenu(&menu0);

	// TODO:  Add extra initialization here
	char procname[64];
	GetCurrentProcInfo(theApp.AppPath, procname, theApp.VerStr);
	getVersionStringAndUpdateTitle(m_hWnd, procname, theApp.VerStr);

	sbar.Create	(WS_CHILD|WS_VISIBLE, CRect(1,2,1,2), this, 1000);
	int rtEdges[] = {170, 220, -1};
	sbar.SetParts(3, rtEdges);

	// TODO: Add extra initialization here
	char remotePC[1024];
	int i;
	CString s;

	// To check font availability (unfinishied)
	i = EnumFonts(::GetDC(m_hWnd), FONTS_DEF, proc, NULL);

	ToolTipCtrl.Create(this);
	ToolTipCtrl.SetMaxTipWidth(200); // for multiline messages
	ToolTipCtrl.AddTool(GetDlgItem(IDC_SIGNAL_TAG), _T("To be computed once upon 'Go' or F5"));
	ToolTipCtrl.AddTool(GetDlgItem(IDC_SIGNAL_TAG2), _T("To be updated on each trial"));
 	ToolTipCtrl.AddTool(GetDlgItem(IDC_SIGNAL_REF), _T(""));
	ToolTipCtrl.AddTool(GetDlgItem(IDC_SIGNAL_CON), _T(""));

	dlgs[0] = m_child1 = new CAdaptiveDlg(this);
	dlgs[1] = m_child2 = new CConstantStim(this);
	dlgs[2] = m_child3 = new CSliderAdj(this);
	dlgs[3] = m_child4 = new CCustomScript(this);

	m_tab.InitDialogs(dlgs, sizeof(dlgs)/sizeof(dlgs[0]), this);

	ReadCalibrationFileUpdate();


	s.Format("%sremotePCs.ini", theApp.AppPath);
	if (!PickRemotePC(NULL, remotePC, s)) 
	{
		s.Format("No connection to subject", remotePC);
		sbar.SetText(s, 0, 0);
	}
	else
	{
		s.Format("Connection to %s", remotePC);
		sbar.SetText(s, 0, 0);
		sbar.SetText("waiting", 1, 0);
		i=initfly (m_hWnd, remotePC, -1, 1); // client
	}

	AstSig.SetPath("", theApp.AppPath);
	AstSig.SetTag("v", CSignals(variable));
	AstSig.SetTag("n", CSignals((double)trial+2));

	CString fnameWpath;
	fnameWpath.Format("%s%s", theApp.AppPath, INI_FILE_NAME);

	if (!_ReadINI(fnameWpath))
	{
		sprintf(remotePC,"%s--invalid format for stim/procedure defintion",fnameWpath);
		MessageBox (remotePC);
	}

	SetTimer(IDT_INIT_TABS, 500, NULL);

	//Get the number of sound devices
	WAVEOUTCAPS woc;
	short temp[8];
	char err[256];
	for (i=0; i<8; i++)		temp[i]=0;
	//if ((i=PlayBufAsynch16(0, temp, 8, 1, 44100, 0, NULL, 0, err))!=MMSYSERR_NOERROR)
	//	waveOutGetErrorText (i, err, sizeof(err));
	/* Getting the number of sound device. */
	nDev = waveOutGetNumDevs();
	if (nDev==0)	MessageBox ("Sound devices not found. Sound will not be played.");
	devCaptions = new CString[nDev];
	for (i=0; i<nDev; i++)
	{
		waveOutGetDevCaps(i, &woc, sizeof(woc));
		devCaptions[i] = woc.szPname;
	}

	m_edit4plot1.SetFont(&SigEditFont);
	m_edit4plot2.SetFont(&SigEditFont);
	m_edit_tags.SetFont(&SigEditFont);
	m_edit_tags2.SetFont(&SigEditFont);

	string buf;
	ReadINI(err,fnameWpath,"LAST VERSION",buf);
	if (buf != theApp.VerStr) {
		OnHelpWhatsNew();
		printfINI(err, fnameWpath, "LAST VERSION", "%s", theApp.VerStr);
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPsyconDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

int CPsyconDlg::WriteINI(const char *fnameWpath)
{
	UpdateData();
	m_child1->UpdateDataPlus();
	m_child2->UpdateData();
	m_child3->UpdateDataPlus();
	m_child4->UpdateData();

	CString s;
	char errStr[128], buf[64], buf2[64];
	CHEC(printfINI(errStr, fnameWpath, "FS", "%d", fs))
	CHEC(printfINI(errStr, fnameWpath, "DEF_TASK_ID", "%d", taskID))
	CHEC(printfINI(errStr, fnameWpath, "INTERVALDELAY", "%d", intervalDelay))
	CHEC(printfINI(errStr, fnameWpath, "TRIALDELAY", "%d", trialDelay))
	CHEC(printfINI(errStr, fnameWpath, "PLAY ROUTING", "%d %d", singlePlay, procPlay))
	CHEC(printfINI(errStr, fnameWpath, "FEEDBACK", "%d", feedback))
	CHEC(printfINI(errStr, fnameWpath, "INSTRUCTIONS", "%s\r\n%s", instruction[0], instruction[1]))
	CHEC(printfINI(errStr, fnameWpath, "NUM_INTERVALS", "%d", nIntervals))
	CHEC(printfINI(errStr, fnameWpath, "ANCHOR_INTERVALS", "%s", m_anchorStr))
	CHEC(printfINI(errStr, fnameWpath, "TRIAL-BY-TRIAL SAVE", "%d", saveDetails))

	CHEC(printfINI(errStr, fnameWpath, "DOWN_UP", "%d %d", m_child1->adap.down, m_child1->adap.up))
	sprintfFloat(m_child1->adap.stepsize[0], 6, buf, sizeof(buf));
	sprintfFloat(m_child1->adap.stepsize[1], 6, buf2, sizeof(buf2));
	CHEC(printfINI(errStr, fnameWpath, "STEPSIZES", "%s %s", buf, buf2))
	CHEC(printfINI(errStr, fnameWpath, "FACTOR", "%d", m_child1->adap.factor))
	CHEC(printfINI(errStr, fnameWpath, "DESCENDING", "%d", m_child1->adap.descending))
	CHEC(printfINI(errStr, fnameWpath, "REVERSALS", "%d %d", m_child1->adap.reversal[0], m_child1->adap.reversal[1]))
	CHEC(printfINI(errStr, fnameWpath, "FINAL_REV_4_AVG", "%d", m_child1->adap.finalRev4Avg))
	sprintfFloat(m_child1->m_crossing_limit, 5, buf, sizeof(buf));
	CHEC(printfINI(errStr, fnameWpath, "NO CROSSING LIMIT", "%s", buf))
	sprintfFloat(m_child1->adap.initialPt , 6, buf, sizeof(buf));
	CHEC(printfINI(errStr, fnameWpath, "INITIAL VALUE", "%s", buf))
	m_child2->GetDlgItemText(IDC_IND_VAR_STR, s);
	CHEC(printfINI(errStr, fnameWpath, "INSTRUCTION1", "%s", m_child1->m_instruction))

	CHEC(printfINI(errStr, fnameWpath, "INDEPENDENT VAR", "%s", s))
	CHEC(printfINI(errStr, fnameWpath, "REPEATS FOR EACH VAR", "%d", m_child2->nTrials))
	m_child3->GetDlgItemText(IDC_INIT_VAL, m_child3->initValStr);
	CHEC(printfINI(errStr, fnameWpath, "INSTRUCTION2", "%s", m_child2->m_instruction))

	CHEC(printfINI(errStr, fnameWpath, "INITIAL ADJUSTMENT VALUE", "%s", m_child3->initValStr))
	sprintfFloat(m_child3->m_step_big, 6, buf, sizeof(buf));
	sprintfFloat(m_child3->m_step_small, 6, buf2, sizeof(buf2));
	CHEC(printfINI(errStr, fnameWpath, "ADJUSTMENT STEPSIZES (BIG SMALL)", "%s %s",buf,buf2))
	CHEC(printfINI(errStr, fnameWpath, "INSTRUCTION3", "%s", m_child3->m_instruction))

	CHEC(printfINI(errStr, fnameWpath, "VAR", "%s", m_strTagDef))
	CHEC(printfINI(errStr, fnameWpath, "VAR2", "%s", m_strTagDef2))
	CHEC(printfINI(errStr, fnameWpath, "SIGNAL", "%s", m_strRef))
	CHEC(printfINI(errStr, fnameWpath, "SIGNAL2", "%s", m_strCont))
	CHEC(printfINI(errStr, fnameWpath, "AS CONTROLLER", "%d", ascontroller))
	CHEC(printfINI(errStr, fnameWpath, "NODE", "%s", m_controller_node))
	CHEC(printfINI(errStr, fnameWpath, "CONTROLLER LEVEL", "%f", m_controller_level))
	CHEC(printfINI(errStr, fnameWpath, "UDFPATH", "%s", m_udfpath))

	CHEC(printfINI(errStr, fnameWpath, "CUSTOM SCRIPT", "%s", m_child4->m_Script))

	// Log reporting
	if (report.fUseEmail)
		CHEC(printfINI(errStr, fnameWpath, "LOG EMAIL ADDRESS", "%s", report.email_address))
	else
		CHEC(printfINI(errStr, fnameWpath, "LOG EMAIL ADDRESS", ""))
	if (report.fUseFtp) {
		CHEC(printfINI(errStr, fnameWpath, "LOG FTP ADDRESS", "%s", report.ftp_address))
		CHEC(printfINI(errStr, fnameWpath, "LOG FTP ID", "%s", report.ftp_id))
		CHEC(printfINI(errStr, fnameWpath, "LOG FTP PASSWORD", "%s", report.ftp_password))
	} else
		CHEC(printfINI(errStr, fnameWpath, "LOG FTP ADDRESS", ""))
	return 1;
}

int CPsyconDlg::_ReadINI(const char *fnameWpath)
{
	char errStr[128];
	CString errstr;
	string buf;
	
	if (sscanfINI(errStr,fnameWpath,"FS","%d",&fs)<=0)
		fs = 22050;
	if (sscanfINI(errStr,fnameWpath,"DEF_TASK_ID","%d",&taskID)<=0)
		taskID = 0;
	if (sscanfINI(errStr,fnameWpath,"INTERVALDELAY","%d",&intervalDelay)<=0)
		intervalDelay=400;
	if (sscanfINI(errStr,fnameWpath,"TRIALDELAY","%d",&trialDelay)<=0)
		trialDelay=400;
	if (sscanfINI(errStr,fnameWpath,"PLAY ROUTING","%d %d",&singlePlay,&procPlay)<=0)
		singlePlay = procPlay = 0;

	if (ReadINI(errStr,fnameWpath,"VAR",buf)<0)
		m_strTagDef = "";
	else
		m_strTagDef = buf.c_str();
	if (ReadINI(errStr,fnameWpath,"VAR2",buf)<0)
		m_strTagDef2 = "";
	else
		m_strTagDef2 = buf.c_str();
	if (ReadINI(errStr,fnameWpath,"SIGNAL",buf)<0)
		m_strRef = "";
	else
		m_strRef = buf.c_str();
	if (ReadINI(errStr,fnameWpath,"SIGNAL2",buf)<0)
		m_strCont = "";
	else
		m_strCont = buf.c_str();
	if (sscanfINI(errStr,fnameWpath,"AS CONTROLLER","%d",&ascontroller)<=0)
		ascontroller = 0;
	if (ReadINI(errStr,fnameWpath,"NODE",buf)<0)
		m_controller_node = ".";
	else {
		m_controller_node = buf.c_str();
		m_controller_node.Trim();
	}
	if (sscanfINI(errStr,fnameWpath,"CONTROLLER LEVEL","%lf",&m_controller_level)<=0)
		m_controller_level=60;
	if (ReadINI(errStr,fnameWpath,"UDFPATH",buf)<0) {
		m_udfpath = "";
		AstSig.SetPath("aux", theApp.AppPath);
	} else {
		m_udfpath = buf.c_str();
		AstSig.SetPath("aux", m_udfpath);
	}
	m_child4->m_Script = (ReadINI(errStr, fnameWpath, "CUSTOM SCRIPT", buf) < 0) ? "" : buf.c_str();

	// Log reporting
	char buf2[256];
	if (ReadItemFromINI(errStr, fnameWpath, "LOG EMAIL ADDRESS", buf2, sizeof(buf))>0) {
		report.fUseEmail = true;
		report.email_address = buf2;
	} else
		report.fUseEmail = false;
	if (ReadItemFromINI(errStr, fnameWpath, "LOG FTP ADDRESS", buf2, sizeof(buf))>0) {
		report.fUseFtp = true;
		report.ftp_address = buf2;
		if (ReadItemFromINI(errStr, fnameWpath, "LOG FTP ID", buf2, sizeof(buf))>0)
			report.ftp_id = buf2;
		if (ReadItemFromINI(errStr, fnameWpath, "LOG FTP PASSWORD", buf2, sizeof(buf))>0)
			report.ftp_password = buf2;
	} else
		report.fUseFtp = false;

	if (ReadINI(errStr, fnameWpath, "INSTRUCTIONS", buf)<0) {
		instruction[0]="Listen...";
		instruction[1]="Ask the operator what to do."; 
	} else {
		str2array(instruction, 2, buf,"\r\n");
		instruction[0].TrimLeft(" \t");
	}
	if (ReadItemFromINI(errStr, fnameWpath, "INSTRUCTION1", buf2, 256)<0)
		m_child1->m_instruction = "";
	else {
		trimLeft(buf2, " ");
		trimRight(buf2, "\r\n");
		trimRight(buf2, " ");
		m_child1->m_instruction = buf2;
	}
	if (ReadItemFromINI(errStr, fnameWpath, "INSTRUCTION2", buf2, 256)<0)
		m_child2->m_instruction = "";
	else {
		trimLeft(buf2, " ");
		trimRight(buf2, "\r\n");
		trimRight(buf2, " ");
		m_child2->m_instruction = buf2;
	}
	if (ReadItemFromINI(errStr, fnameWpath, "INSTRUCTION3", buf2, 256)<0)
		m_child3->m_instruction = "";
	else {
		trimLeft(buf2, " ");
		trimRight(buf2, "\r\n");
		trimRight(buf2, " ");
		m_child3->m_instruction = buf2;
	}

	if (sscanfINI(errStr,fnameWpath,"TRIAL-BY-TRIAL SAVE","%d",&saveDetails)<=0)
		saveDetails=0;
	if (sscanfINI(errStr,fnameWpath,"NUM_INTERVALS","%d",&nIntervals)<=0)
		nIntervals=2;
	UpdateText4SingleInt();
	sscanfINI(errStr,fnameWpath,"NO CROSSING LIMIT","%lf", &m_child1->m_crossing_limit); 
	if (ReadINI(errStr,fnameWpath,"ANCHOR_INTERVALS",buf)>=0) m_anchorStr = buf.c_str();
	if (sscanfINI(errStr,fnameWpath,"DOWN_UP","%d %d",&m_child1->adap.down,&m_child1->adap.up)<=0) {
		m_child1->adap.down = 2;
		m_child1->adap.up = 1;
	}
	if (sscanfINI(errStr,fnameWpath,"FEEDBACK","%d",&feedback)<=0)
		feedback = 1;
	if (sscanfINI(errStr,fnameWpath,"STEPSIZES","%lf %lf",&m_child1->adap.stepsize[0],&m_child1->adap.stepsize[1])<=0) {
		m_child1->adap.stepsize[0] = 7;
		m_child1->adap.stepsize[1] = 2;
	}
	if (sscanfINI(errStr,fnameWpath,"REVERSALS","%d %d",&m_child1->adap.reversal[0],&m_child1->adap.reversal[1])<=0) {
		m_child1->adap.reversal[0] = 6;
		m_child1->adap.reversal[1] = 8;
	}
	if (sscanfINI(errStr,fnameWpath,"FINAL_REV_4_AVG","%d",&m_child1->adap.finalRev4Avg)<=0)
		m_child1->adap.finalRev4Avg = 4;
	if (sscanfINI(errStr,fnameWpath,"INITIAL VALUE","%lf",&m_child1->adap.initialPt)<=0)
		m_child1->adap.initialPt = 30;
	if (sscanfINI(errStr,fnameWpath,"FACTOR","%d",&m_child1->adap.factor)<=0)
		m_child1->adap.factor = false;
	if (sscanfINI(errStr,fnameWpath,"DESCENDING","%d",&m_child1->adap.descending)<=0)
		m_child1->adap.descending = 1;
	if (m_child1->adap.descending!=1) m_child1->adap.descending=0; // if something other than 0 or 1 is in the file, it is presumed to be 0.
	if (ReadINI(errStr,fnameWpath,"INDEPENDENT VAR",buf)<0)
		m_child2->varlistStr="";
	else
		m_child2->varlistStr=buf.c_str();
	if (sscanfINI(errStr,fnameWpath,"REPEATS FOR EACH VAR","%d",&m_child2->nTrials)<=0)
		m_child2->nTrials = 5;
	if (m_child2->nTrials == 0) m_child2->nTrials = 1;
	if (ReadINI(errStr,fnameWpath,"INITIAL ADJUSTMENT VALUE",buf)<0)
		m_child3->initValStr = "30";
	else
		m_child3->initValStr = buf.c_str();
	if (sscanfINI(errStr,fnameWpath,"ADJUSTMENT STEPSIZES (BIG SMALL)","%lf %lf",&m_child3->m_step_big,&m_child3->m_step_small)<=0) {
		m_child3->m_step_big = 10.;
		m_child3->m_step_small = 3.;
	}

	if (sscanfINI(errStr,fnameWpath,"ADAP_SESSION LIMIT","%d",&m_child1->adap.m_limit)<=0)
		m_child1->adap.m_limit = 99;

	UpdateData(0);
	m_child1->UpdateDataPlus(0);
	m_child2->UpdateData(0);
	m_child3->UpdateDataPlus(0);
	m_child4->UpdateData(0);
	SetTimer(IDT_INIT_TABS, 500, NULL);
	return 1;
}

void CPsyconDlg::UpdateText4SingleInt()
{
	CString st;
	if (nIntervals==1)
	{
		st.LoadString(IDS_STRING105);
		SetDlgItemText(IDC_STATIC8, st);
		ToolTipCtrl.UpdateTipText(_T("Result should be a string\nTo be written in session log"), GetDlgItem(IDC_SIGNAL_REF));
		st.LoadString(IDS_STRING106);
		SetDlgItemText(IDC_STATIC9, st);
		ToolTipCtrl.UpdateTipText("Use v for variable, n for trial#\nPress F5 to view signal", GetDlgItem(IDC_SIGNAL_CON));
	}
	else
	{
		st.LoadString(IDS_STRING103);
		SetDlgItemText(IDC_STATIC8, st);
		ToolTipCtrl.UpdateTipText(_T("Press F5 to view signal"), GetDlgItem(IDC_SIGNAL_REF));
		st.LoadString(IDS_STRING104);
		SetDlgItemText(IDC_STATIC9, st);
		ToolTipCtrl.UpdateTipText("Use v for variable\nPress F5 to view signal", GetDlgItem(IDC_SIGNAL_CON));
	}
}

BOOL CPsyconDlg::DestroyWindow() 
{
	CString fnameWpath;
	if (!destroyedWind)
	{
		UpdateData();
		m_child1->UpdateDataPlus();
		m_child2->UpdateData();
		m_child3->UpdateDataPlus();
		fnameWpath.Format("%s%s", theApp.AppPath, INI_FILE_NAME);
		WriteINI(fnameWpath);
		closefly();
		CloseFigure(0);
		for (int i=0; i<MAX_RUNTIME_DLLS; i++)
			FreeLibrary(theApp.hRunTimeDlls[i]);
		destroyedWind = 1;
	}
	return CDialog::DestroyWindow();
}

int CPsyconDlg::MakeNextAnswer(int nInt)
{
	int res, j(0), ints[8];
	for (int i=0; i<nIntervals; i++) 
		if (!banchor[i]) 
			ints[j++] = i;
	ASSERT(j==nInt);
	if (j==0)
	{
		MessageBox ("Number of intervals must be greater than zero. Procedure must be aborted.");
		return 0;
	}
	else 
		res = rand() % j;
	return ints[res];
}

void CPsyconDlg::OnFileOpenStim()
{
	CString szFilter("Signal/Session Files (*.txt; *.sig)|*.txt; *.sig|All Files (*.*)|*.*||");
	CFileDialog dllg(1, "txt", NULL, OFN_HIDEREADONLY, szFilter);
	if (dllg.DoModal() == IDOK)
		SigFile(1, dllg.GetFileName());
}

void CPsyconDlg::OnFileSaveStim()
{
	CString szFilter("Signal/Session Files (*.txt)|*.txt|All Files (*.*)|*.*||");
	CFileDialog dllg(0, "txt", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	dllg.DoModal();
	SigFile(0, dllg.GetFileName());
}

void CPsyconDlg::OnFileSettings()
{
	CSettings dlg (this);
	dlg.DoModal();
}

void CPsyconDlg::OnFileExit()
{
	DestroyWindow();
}

void CPsyconDlg::OnHelpWhatsNew()
{
	CString msg, title = "What's new in ";
	title += theApp.VerStr;
	msg = "sigproc v1.76";
	msg += "\n* Experimental release. Not for public use.";
	msg += "\n* Psycon_response is replaced with Subject Response.";
	MessageBox(msg, title, MB_OK | MB_TOPMOST | MB_ICONINFORMATION);
}

void CPsyconDlg::OnHelpAbout()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CPsyconDlg::OnTimer(UINT nIDEvent) 
{
	//If you need any REPETITIVE (such as time-ticking), move KillTimer inside.
	KillTimer(nIDEvent);
	switch(nIDEvent)
	{
	case JUST_CONNECTED:
		FER(flySendText (FL_RUNDLL, TS_DLL_NAME))
		break;
	case IDT_TRIAL_DELAYS:
		interval=0;
		switch(taskID)
		{
		case 0:
		case 1:
			PostMessage(WM__NEXT_INTERVAL, answer==interval ? CONTROLLED:REFERENCE);
			break;
		case 2:
				PostMessage(WM__NEXT_INTERVAL, REFERENCE);
			break;
		}
		break;
	case IDT_INTERVAL_DELAYS:
		if (taskID==2) // slider
		{
			if (++interval==1)
			{
				PostMessage(WM__NEXT_INTERVAL, CONTROLLED);
			}
			else
			{
				FER(flySendText (FL_END_OF_PRSENTATION, ""))
				interval=0;
			}
		}
		else
		{
			if (++interval<nIntervals)
				PostMessage(WM__NEXT_INTERVAL, answer==interval ? CONTROLLED:REFERENCE);
		}
		break;
	case IDT_INIT_TABS:
		m_tab.SetCurSel(taskID);
		m_tab.ActivateTabDialogs(taskID);
		OnSelChange(taskID);
		break;
	}
	CDialog::OnTimer(nIDEvent);
}


void CPsyconDlg::OnUpdateTalk() 
{
	CString s;
	GetDlgItemText(IDC_TALK, s);
	FER(flySendText(FL_TEXT, s));
}

void CPsyconDlg::DoneSession(CString buf2Print, int param)
{
	double jnd, jnd2, std, std2;
	int id;
	CStdioFile fl;
	CString buf, buf2, st, filename="";
	static CString longBuf(""); 
	char zbuf[32], zbuf1[32], bbuf[65535];
	char szFilter[] = "Text Files (*.txt)|*.txt|All Files (*.*)|*.*||";

	if (param==ADDLONGBUF)	
	{		longBuf += buf2Print;		return;		}
	EnableID (this, IDC_TASKTAB, 1);
	EnableID (this, IDC_GO, 1);
	EnableID (this, IDC_STOP, 0);
	EnableID (this, IDC_SIGNAL_TAG, 1);
	EnableID (this, IDC_SIGNAL_TAG2, 1);
	EnableID (this, IDC_SIGNAL_REF, 1);
	EnableID (this, IDC_SIGNAL_CON, 1);
	EnableID (this, IDC_NUM_INTERVALS, 1);
	EnableID (this, IDC_ANCHOR_INT, 1);
	CFileDialog dlg (0, "txt", NULL, OFN_HIDEREADONLY, szFilter);
	if (param!=STOPPED)	flySendText (FL_DONE, "Session Done.");
	if (saveDetails || param==STOPPED || (m_strRef.GetLength()>0 && nIntervals==1) ) // for single-interval, speech identification
	{
		if (fl.Open (INTERMEDIATE_FILE_NAME, CFile::modeRead | CFile::typeBinary )) {
			id = fl.Read(bbuf, 65535);
			bbuf[id] = 0;
			longBuf += "\r\n";
			longBuf += bbuf;
			fl.Close();
		}
	}

	switch (taskID) {
	case 0:
		if (param!=STOPPED)
		{
			jnd = getmean(m_child1->adap.finalRev4Avg, thresholdCand[0]+m_child1->adap.reversal[0]+m_child1->adap.reversal[1]-m_child1->adap.finalRev4Avg, &std);
			if (m_child1->adap.dualSeries)
			{
				jnd2 = getmean(m_child1->adap.finalRev4Avg, thresholdCand[1]+m_child1->adap.reversal[0]+m_child1->adap.reversal[1]-m_child1->adap.finalRev4Avg, &std2);
				buf.Format ("Testing complete. Converged values=%5.1f, %5.1f", jnd, jnd2);
				buf2.Format("Results:\r\nSeries 1:%5.1f, std=%5.2f\r\nSeries 2: %5.1f, std=%5.2f\r\n", jnd, std, jnd2, std2);
			}
			else
			{
				buf.Format ("Testing complete. Converged value=%5.1f", jnd);
				buf2.Format("Result=%5.1f, std=%5.2f\r\n", jnd, std);
			}
			MessageBox (buf);
		}
		EditPrintf (m_logEdit.m_hWnd, buf2);
		m_child1->GetDlgItemText (IDC_INIT_VAL, buf);
		double c;
		memcpy((void*)&c, (void*)banchor, sizeof(banchor));
		if (c==0.)
			st.Format("%d-I, %ddown %dup, beginPt: %s, ", nIntervals, m_child1->adap.down, m_child1->adap.up, buf);
		else
		{
			GetDlgItemText (IDC_ANCHOR_INT, buf2);
			st.Format("%d-I (anchors: %s), %ddown %dup, beginPt: %s, ", nIntervals, buf2, m_child1->adap.down, m_child1->adap.up, buf);
		}
		longBuf += st;
		sprintfFloat(m_child1->adap.stepsize[0], 3, zbuf, 32);
		sprintfFloat(m_child1->adap.stepsize[1], 3, zbuf1, 32);
		st.Format("steps:%s %s, revs:%d %d\r\n", zbuf, zbuf1, m_child1->adap.reversal[0], m_child1->adap.reversal[1]);
		longBuf += st;
		if (param!=STOPPED)
			st.Format("Result(last %d revs) = %5.1f (std=%5.2f)\r\n", m_child1->adap.finalRev4Avg, jnd, std);
		else
			st.Format("Session Aborted\r\n");
		longBuf += st;
		if (m_child1->adap.dualSeries && param!=STOPPED)
		{
			st.Format("Result(last %d revs) = %5.1f\r\n", m_child1->adap2.finalRev4Avg, jnd2);
			longBuf += st;
		}
		break;

	case 1:
		MessageBox (buf2Print);
		longBuf += "\r\n" + buf2Print;
		break;

	case 2:
		sprintfFloat(variable, 3, zbuf, 32);
		st.Format("Session done. Adjusted value:%s\r\n", zbuf);
		MessageBox (st);
		longBuf += st;
		break;
	}

	if (taskID!=2 && trial>0 && MessageBox ("Keep the session?", 0, MB_YESNO)==IDYES) {
		report.filename = "LastSession.log";
		remove(report.filename);
		SaveLogFile(report.filename, longBuf);
		if (report.fUseEmail || report.fUseFtp)
			_beginthread(uploadLogFile, 0, (void*)&report);

		if (dlg.DoModal()==IDOK) {
			filename = dlg.GetPathName();
			SaveLogFile(filename, longBuf);
		}
	}

	longBuf="";
	trial=-1;
}


int CPsyconDlg::SaveLogFile(CString& fname, CString& string2save)
{
	if (fname.GetLength()==0) return 0;

	CTime t = CTime::GetCurrentTime();
	CString subj, inst, str;
	CString s = t.Format( "%H:%M:%S, %B %d, %Y" );
	str.Format("psycon %s %s", theApp.VerStr, s);

	HANDLE hFile;
	DWORD dw;

	if ( (hFile = ::CreateFile (fname, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
				OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ))== INVALID_HANDLE_VALUE)
	{
		errstr.Format("Cannot open %s", fname);
		return -1; //file not found or something like that
	}
	::SetFilePointer(hFile, 0, NULL, FILE_END);
	str += "\r\n"; // time
	if(!::WriteFile (hFile, str, str.GetLength(), &dw, NULL))
	{
		errstr.Format("Cannot write to file %s", fname);
		return -2;
	}
	GetDlgItemText (IDC_SUBJ, subj);
	CString st;
	GetDlgItemText(IDC_SIGNAL_TAG, st);
	str.Format("Subject:%s\r\nTags:\r\n%s\r\nStandard:\r\n%s\r\n", subj, st, m_strRef);
	if(!::WriteFile (hFile, str, str.GetLength(), &dw, NULL))
	{
		errstr.Format("Cannot write to file %s", fname);		return -2;
	}
	if (m_strTagDef2.GetLength()>0)
	{
		str.Format("Tags (trial-by-trial):\r\n%s\r\n", m_strTagDef2);
		if(!::WriteFile (hFile, str, str.GetLength(), &dw, NULL))
		{
			errstr.Format("Cannot write to file %s", fname);		return -2;
		}
	}
	str.Format("Control:\r\n%s\r\n", m_strCont);
	if(!::WriteFile (hFile, str, str.GetLength(), &dw, NULL))
	{
		errstr.Format("Cannot write to file %s", fname);
		return -2;
	}
	switch(taskID) {
	case 0:
		inst.Format("Instructions:%s\r\n%s\r\n", instruction[0], m_child1->m_instruction);
		break;
	case 1:
		inst.Format("Instructions:%s\r\n%s\r\n", instruction[0], m_child2->m_instruction);
		break;
	case 2:
		inst.Format("Instructions:%s\r\n", m_child3->m_instruction);
		break;
	}
	str = string2save + inst;
	str += "===========================\r\n";
	if(!::WriteFile (hFile, str, str.GetLength(), &dw, NULL))
	{
		errstr.Format("Cannot write to file %s", fname);
		return -2;
	}
	if(!::CloseHandle(hFile))
	{
		errstr.Format("Cannot close file %s", fname);
		return -3;
	}
	errstr = "no error";
	return 0;
}

LRESULT CPsyconDlg::OnFlyConnected(WPARAM code, LPARAM lParam)
{
	sbar.SetText("ON", 1, 0);
	SetTimer(JUST_CONNECTED, 500, NULL);
	return 0;
}

LRESULT CPsyconDlg::OnFlyClosed(WPARAM code, LPARAM lParam)
{
	sbar.SetText("OFF", 1, 0);
	return 0;
}

LRESULT CPsyconDlg::OnFlyErr(WPARAM code, LPARAM lParam)
{
	sbar.SetText("Error", 1, 0);
	return 0;
}

int CPsyconDlg::AdapProceed(int corrThis)
{
	static CAdaptive *obj=&m_child1->adap; // toggles between m_child1->adap and m_child1->adap2
	static int onceDual;
	int res;
	double val = var.get();
	if (corrThis<0)
		onceDual = m_child1->adap.dualSeries;
	if (m_child1->adap.dualSeries)
	{
		if (toggle) ++trial;
		toggle = !toggle;
		obj = toggle ? &m_child1->adap2 : &m_child1->adap;
		if (res = obj->Proceed (corrThis, &val)) 
		{
			toggle = !toggle;
			obj = toggle ? &m_child1->adap2 : &m_child1->adap;
			var.set(-9999.9);
			corr.set(8);
			val = var.get();
			if (res = obj->Proceed (corr.get(), &val))  return DUALDONE;
			variable = val;
			m_child1->adap.dualSeries=0;
			corr.SetLength(1);
			var.SetLength(1);
			varHistory[toggle][trial] = val;
			return HALFDONE;
		}
		varHistory[toggle][trial] = val;
	}
	else
	{
		++trial;
		if (res = obj->Proceed (corrThis, &val)) 
		{
			if (onceDual)		return DUALDONE;
			else				return SINGLEDONE;
		}
		varHistory[toggle][trial] = val;
	}
	variable = val; // variable for the next trial is updated here.
	return res;
}

LRESULT CPsyconDlg::OnFlyArrived(WPARAM code, LPARAM lParam)
{
	int res, response;
	static int clipped;
	static double rangex1, rangex2, rangey1, rangey2;
	static bool domainv; // used to check if v has crossed the limit (ref value) in adaptive procedure
	static bool crossed; 
	bool curCorr;
	double adj, adj_big, adj_small;
	CString zbuf, buf, temp[2];
	CStdioFile fl;
	char buf2[32], strbuf[64];

	if (dlgs[taskID]->OnFlyArrived(code, lParam) != 1)
		return 1;

	switch (LOWORD(code))
	{
	case FL_STOP:
		OnBnClickedStop();
		break;

	case FL_BEGIN:
		stopped=0;
		if(taskID==0)
		{
			memset((void*)varHistory, 0, sizeof(varHistory));
			m_child1->adap.InitAdaptive (corrHist[0], thresholdCand[0]);
			if (m_child1->adap.dualSeries)
			{
				toggle = 1;
				m_child1->adap2.InitAdaptive (corrHist[1], thresholdCand[1]);
				corr.SetLength(2);
				corr.set(-2); corr.set(-1);
				var.SetLength(2);
				var.set(m_child1->adap.initialPt); 
				var.set(m_child1->adap2.initialPt);
			}
			else
			{
				toggle = 0;
				corr.SetLength(1);
				corr.set(-1);
				var.SetLength(1);
				var.set(m_child1->adap.initialPt); 
			}
			AdapProceed (corr.get());
			if (m_child1->m_crossing_limit != QNAN) domainv = variable > m_child1->m_crossing_limit;
			PlotHistory(varHistory[toggle]);
		}
		else // taskID will be 1
		{
			sprintf(strbuf, "%s ", tagIDs[0].c_str());
			if (m_child2->nVars>1)	strcat(strbuf, tagIDs[1].c_str()); 
			m_child2->GenLogStr(buf, strbuf, cumCorr, cumTrials);

			m_child2->m_crv->m_res_log = buf;
			m_child2->m_crv->SetDlgItemText(IDC_RESULT_LOG, buf);
			++trial;
		}
		answer = MakeNextAnswer(nIntLessAnc);
		if (Prepare(-1)) {	// for the first trial, always recalc (regarldess of recalc checkbutton)
			res=SetTimer(IDT_TRIAL_DELAYS, trialDelay, NULL);
		} else
			DoneSession();
		break;

	case FL_RESPONSE:
		memcpy((void*)&response, (void*)lParam, HIWORD(code));
		if (nIntLessAnc>1)
			curCorr = (answer==response);
		else
			curCorr = !response;
		if (toggle)
			curCorr = !curCorr;
		corr.set(curCorr);
		if (curCorr)
		{	zbuf = "Correct"; buf = "  O\r\n";	}
		else
		{	zbuf = "Incorrect"; buf = "  X\r\n";	}
		if (feedback)
			flySendText (FL_FEEDBACK, zbuf);

		if (nIntLessAnc>1)
			zbuf.Format("%s[%d]%s", trial_line_log, response+1, buf);
		else
			zbuf.Format("%s%s", trial_line_log, buf);
		fl.Open(INTERMEDIATE_FILE_NAME, CFile::modeNoTruncate | CFile::modeWrite | CFile::typeBinary );
		fl.SeekToEnd();
		fl.WriteString(zbuf);
		fl.Close();

		EditPrintf (m_logEdit.m_hWnd, buf);
		if (taskID==0)
		{
			if (trial>m_child1->adap.m_limit)
			{
				stopped=1;
				FER(flySendText (FL_STOP, "")) 
				DoneSession();
				break;
			}
			res = AdapProceed (corr.get());
			if (m_child1->m_crossing_limit != QNAN)
			{
				// This is good only for single procedure, check (and modify) for double staircase
				bool ch = variable > m_child1->m_crossing_limit;
				if (ch!=domainv) { 
					variable = m_child1->m_crossing_limit; m_child1->adap.warning=true;
					buf.Format( "Warning: The variable limit reached in trial %d\n", trial+1);
					m_child1->InvalidateRect(NULL);
					m_child1->SetDlgItemTextA(IDC_WARNING_STATIC, "WARNING! The variable has reached");
					DoneSession(buf, ADDLONGBUF);
				}
			}
			if (res==DUALDONE)				m_child1->adap.dualSeries=1;
			if (res==DUALDONE || res==SINGLEDONE)
			{
				DoneSession("", NOT_STOPPED);
			}
			else if (res==-1)
			{
				MessageBox ("Reached the boundary. Session aborted.");
				DoneSession();
			}
			else // continue
			{
				answer = MakeNextAnswer(nIntLessAnc);
				if (Prepare(-1)) {
					if (m_child1->adap.dualSeries) {
						if (m_axProc->nLines==2)
							m_axProc->DeleteLine(0);
					} else
						m_axProc->DeleteLine(m_axProc->nLines-1);
					PlotHistory(varHistory[toggle]);
					SetTimer(IDT_TRIAL_DELAYS, trialDelay, NULL);
				} else
					DoneSession();
			}
		}
		else
		{
			cumTrials[PresentingIDOrder[trial]]++;
			cumCorr[PresentingIDOrder[trial]] += curCorr;
			scores[PresentingIDOrder[trial]] = (double) cumCorr[PresentingIDOrder[trial]] / (double)cumTrials[PresentingIDOrder[trial]] * 100.;
			ClearEditPrintf(m_child2->m_crv->m_const_log.m_hWnd);

			strcpy(strbuf,"");
			strcat(strbuf, tagIDs[0].c_str()); strcat(strbuf," ");
			if (m_child2->nVars>1)	strcat(strbuf, tagIDs[1].c_str()); 
			m_child2->GenLogStr(buf, strbuf, cumCorr, cumTrials);

			m_child2->m_crv->m_res_log = buf;
			m_child2->m_crv->SetDlgItemText(IDC_RESULT_LOG, buf);
//			respHist[trial] = response+1; //what is this?
//			m_axProc->DeleteLine(-1);
//			m_axProc->plot(nConstSteps, values, scores, RGB(40, 150, 200), (char)2, 1);
			if (trial==0)
			{
				rangex1 = -.5;		rangex2 = nConstSteps -.5;
				rangey1 = 0;		rangey2 = 110.;
				m_axProc->setRange('x', rangex1,rangex2);
				m_axProc->setRange('y', rangey1,rangey2);
				m_axProc->setTick('x',0., 1., 0);
				m_axProc->setTick('y',0., 110., 10);
				m_axProc->xtick.offset=1.;
			}
			if (trial<nTrials-1)
			{
				++trial;
				answer = MakeNextAnswer(nIntLessAnc);
				if (Prepare(-1))
					SetTimer(IDT_TRIAL_DELAYS, trialDelay, NULL);
				else
					DoneSession();
			}
			else
			{
				m_axProc->DeleteLine(-1);
				m_axProc->plot(nConstSteps, scores, RGB(40, 150, 200), (char)2, 1);
				//LOOK HERE above to change the x-axis
				strcpy(strbuf,"");
				strcat(strbuf, tagIDs[0].c_str()); strcat(strbuf," ");
				if (m_child2->nVars>1)	strcat(strbuf, tagIDs[1].c_str()); 
				m_child2->GenLogStr(buf, strbuf, cumCorr, cumTrials);
				DoneSession(buf, NOT_STOPPED);
			}
		}
		break;

	case FL_SKIP:
		if (feedback)				flySendText (FL_FEEDBACK, "Skip");

		zbuf.Format("%s  -\r\n", trial_line_log);
		fl.Open(INTERMEDIATE_FILE_NAME, CFile::modeNoTruncate | CFile::modeWrite | CFile::typeBinary );
		fl.SeekToEnd();
		fl.WriteString(zbuf);
		fl.Close();
		
		EditPrintf (m_logEdit.m_hWnd, "  -\r\n");

		if (trial>m_child1->adap.m_limit) {
			stopped=1;
			FER(flySendText (FL_STOP, "")) 
			DoneSession();
			break;
		}
		++trial;
		varHistory[toggle][trial] = varHistory[toggle][trial-1];
		answer = MakeNextAnswer(nIntLessAnc);
		if (Prepare(-1)) {
			if (m_child1->adap.dualSeries) {
				if (m_axProc->nLines==2)
					m_axProc->DeleteLine(0);
			} else
				m_axProc->DeleteLine(m_axProc->nLines-1);
			PlotHistory(varHistory[toggle]);
			SetTimer(IDT_TRIAL_DELAYS, trialDelay, NULL);
		} else
			DoneSession();
		break;

	case FL_PLAY: 
		trial++;
		stopped=0;
		if (Prepare(-1)) {
			SetTimer(IDT_TRIAL_DELAYS, trialDelay, NULL);
			playedItem=-1;
			sprintfFloat(variable, 3, buf2, 32);
			m_child3->SetDlgItemText (IDC_VAL, buf2);
		} else {
			stopped=1;
			trial--;
			DoneSession();
			break;
		}
		break;

	case FL_ADJUST: 
		adj_big = GetDlgItemDouble (m_child3->m_hWnd, IDC_STEP_BIG, &res);
		adj_small = GetDlgItemDouble (m_child3->m_hWnd, IDC_STEP_SMALL, &res);
		strcpy(buf2, (char*)lParam);
		if (buf2[0]=='+')
		{
			if (!strncmp(&buf2[1], "BIG", 3))
				adj = adj_big;
			else if (!strncmp(&buf2[1], "SMA", 3))
				adj = adj_small;
			else
			{
				buf.Format("buf2=%s..invalid FL_ADJUST", buf2);
				MessageBox (buf2);
				break;
			}
		}
		else if (buf2[0]=='-')
		{
			if (!strncmp(&buf2[1], "BIG", 3))
				adj = -adj_big;
			else if (!strncmp(&buf2[1], "SMA", 3))
				adj = -adj_small;
			else
			{
				buf.Format("buf2=%s..invalid FL_ADJUST", buf2);
				MessageBox (buf2);
				break;
			}
		}
		else
		{
			MessageBox ("Invalid FL_ADJUST message","", MB_OK);
			break;
		}
		variable += adj;
		break;

	case FL_DONE:
		EnableID (m_child3, IDC_INIT_VAL, 1);
		EnableID (m_child3, IDC_STEP_BIG, 1);
		EnableID (m_child3, IDC_STEP_SMALL, 1);
		DoneSession("", NOT_STOPPED);
		break;
	}
	return 0;
}

LRESULT CPsyconDlg::OnNext_Interval(WPARAM wp, LPARAM lp)
{
	static double rangex1, rangex2, rangey1, rangey2, varHistory[256];
	char varstr[16];
	FER(sendfly (FL_ABOUT2PRESENT, "", 0))
	switch(taskID) {
	case 0:
	case 1:
		if (interval==0)
		{
			if (taskID==0)
				sprintfFloat(variable, 6, varstr,  sizeof(varstr));
			else {
				sprintfFloat(AstSig.GetTag(tagIDs[0].c_str()).value(), 3, varstr, sizeof(varstr));
				if (m_child2->nVars > 1) {	// assume that nVars == 2
					strcat(varstr, "|");
					sprintfFloat(AstSig.GetTag(tagIDs[1].c_str()).value(), 3, varstr+strlen(varstr), sizeof(varstr)-strlen(varstr));
				}
			}
			strcat(varstr, " ");
			if (nIntLessAnc>1) {
				trial_line_log.Format("%d: ", trial+1);
				for (int i=0; i<nIntervals; i++)
					trial_line_log += (answer==i) ? varstr : "ref ";
			} else {
				//how do I use the result of the above compute down here to record the trial-by-trial stuff?
				//also ask jh how to set up the cell array.
				trial_line_log.Format("%d: %s %s", trial+1, sgen0.Sig.string().c_str(), varstr);
			}

			EditPrintf (m_logEdit.m_hWnd, trial_line_log);
		}
		break;
	case 2:
/*		varHistory[trial] = variable;
		m_axProc->DeleteLine(-1);
		m_axProc->plot(trial+1, varHistory, RGB(0, 255, 100), (char)2, 0);
		if (trial==0)
		{
			rangex1 = 0.;		rangex2 = 10.;
			rangey1 = variable + m_child3->m_range1;	rangey2 = variable + m_child3->m_range2;
		}
		else
		{
			if (trial>rangex2-1)		rangex2 += 5;
			if (variable > rangey2)			rangey2 += m_child3->m_range2;
			else if (variable < rangey1)		rangey1 -=  m_child3->m_range1;
		}
		m_axProc->setRange('x', rangex1,rangex2);
		m_axProc->setRange('y', rangey1,rangey2);
		m_axProc->setTick('x', 0., 5., 0);
		m_axProc->setTick('y', 0, m_child3->m_step*2, 0);
*/		break;
	}
	Present(interval);
	return 0;
}

int CPsyconDlg::PrepareTags()
{
	UpdateData();
	try {
		AstSig.Reset(fs).SetNewScript(m_strTagDef).Compute();
	} catch (const char *errmsg) {
		MessageBox(errmsg, "Error in Variable Definitions");
		return 0;
	}
	return 1;
}

int CPsyconDlg::Prepare(int id)
{ 
	int nInt;

	switch (id)
	{
	case -1:
		AstSig.SetTag("n", CSignals((double)trial+1));	
		if (taskID==1)
		{// to check irregularities here and record in the log file (if trial==nTrials-1, don't do it---the last one that won't be used)
			CString buf;
			if (trial>nTrials-1 || trial<-1)
			{ buf.Format("trial=%d\r\n", trial);
			  EditPrintf (m_logEdit.m_hWnd, buf);	//RecordTrialByTrial(buf);
			}
			// This is where value selection for independent variables is done in each trial from presentingIDorder;
			// currentInd: the index in the order of m_child2->var from CConstantStim::ProcessList()
			int currentInd = PresentingIDOrder[trial];

			if (m_child2->nVars>1) {	// assume that it is 2
				AstSig.SetTag(tagIDs[0].c_str(), m_child2->var.buf[currentInd * m_child2->nVars]);
				AstSig.SetTag(tagIDs[1].c_str(), m_child2->var.buf[currentInd * m_child2->nVars+1]);
			} else
				AstSig.SetTag(tagIDs[0].c_str(), m_child2->var.buf[currentInd]);
			if (currentInd>=m_child2->var.nSamples || currentInd<0) // check this m_child2->var.nSamples
			{ buf.Format("currentInd=%d, variable=%lf\r\n", currentInd, variable);
			  EditPrintf (m_logEdit.m_hWnd, buf);	//RecordTrialByTrial(buf);
			}
		} else {
			if (taskID==2 && trial==-1) m_child3->UpdateDataPlus(); // updating variable to be used below
			var.set(variable);
			AstSig.SetTag("v", CSignals(variable));
		}
		nInt = (taskID==2) ? 2 : nIntervals;
		try {
			AstSig.SetNewScript(m_strTagDef2).Compute();
		} catch (char *errmsg) {
			MessageBox(errmsg, "Error in Variables Per Trial", MB_ICONQUESTION);
			return 0;
		}		
		if (!(Prepare(0) && Prepare(1)))
			return 0;
		if (nIntervals>1) {
			for (int i=0; i<nInt; i++)
				intervalsig[i] = (i==answer) ? sgen1.Sig : sgen0.Sig;
		} else
			intervalsig[0] = sgen1.Sig;
		break;
	case 0:
		try {
			sgen0.SetNewScript(m_strRef).Compute();
		} catch (const char *errmsg) {
			if (nIntervals>1)
				MessageBox(errmsg, "Error in Reference Signal Definition", MB_ICONQUESTION);
			else
				MessageBox(errmsg, "Error in Trial-by-trial Record", MB_ICONQUESTION);
			return 0;
		}
		break;
	case 1:
		try {
			sgen1.SetNewScript(m_strCont).Compute();
		} catch (const char *errmsg) {
			MessageBox(errmsg, "Error in Variable Signal Definition", MB_ICONQUESTION);
			return 0;
		}
		break;
	default:
		return 0;
	}
	return 1;
}

#define no_acousticPlay 0
#define PREPAREPATH	"ResearchAudioSignals"

void CPsyconDlg::Present(int playID)
{
	char fname[256], fnamePath[256], estr[256];
	char prepInfo[49660];
	double level2, dummy;
	if (ascontroller)
	{
		Prepare((playID==answer)?1:0);	// doing this again to process #PIPE(). 6/28/2012 jhpark
		//In this version, the wave file name is hard-coded as "psycon.wav"
		sprintf (fname, "%s", "psycon.wav");
		if (m_controller_node.GetLength() > 0 && m_controller_node != ".")
			sprintf (fnamePath, "\\\\%s\\%s\\%s", m_controller_node, PREPAREPATH, fname);
		else
			sprintf (fnamePath, "%2.2s\\%s\\%s", theApp.AppPath, PREPAREPATH, fname);
		if (!intervalsig[playID].Wavwrite(fnamePath, estr))
		{	MessageBox (estr); return;	}
		level2 = m_controller_level - LeveldBrms(fnamePath, &dummy, estr);
		if (!PreparePresent("", fname, no_acousticPlay, level2, prepInfo, sizeof(prepInfo), estr))
		{	MessageBox (estr); return;	}
	}
	else
	if(!intervalsig[playID].PlayArray(procPlay, estr))
	{	MessageBox (estr); return;	}
	FER(sendfly (FL_ABOUT2STOP, "", 0));
	int i = SetTimer(IDT_INTERVAL_DELAYS, intervalDelay, NULL);
}

void CPsyconDlg::Plot(int idc) 
{
	UpdateData();
	if(theApp.nPlotWindows==MAX_PLOTWND)
	{
		::CloseFigure(0);
		theApp.nPlotWindows = 0;
	}
	int id = idc==IDC_SIGNAL_REF ? 0:1;

	if (trial==-1)
	{  // if plotting out of the procedure
		//When plotting a signal, it always sets recalc to true
		if (PrepareTags() != 1)
			return;
		if (taskID == 1 && id == 1 && m_child2->ProcessList()<0)
			return;
		try {
			AstSig.SetNewScript(m_strTagDef2).Compute();
		} catch (char *errmsg) {
			MessageBox(errmsg, "Error in Variables Per Trial", MB_ICONQUESTION);
			return;
		}		
		if (!Prepare(id))
			return;
	}

	CSignals &sig = (id) ? sgen1.Sig : sgen0.Sig;
	sig.MakeChainless();
	if (int len = sig.GetLength())
	{
		CRect rt(0, 0, 600, 400);
		CFigure *fig = OpenFigure(&rt, sig, singlePlay);
		CAxis *ax = AddAxis (fig, .08, .1, .86, .8);
		fig->AddText(id==0?m_strRef:m_strCont, CPosition(.02, .02, 0, 0));
		double *x = new double[len];
		for (int i=0; i<len; i++)
			x[i] = (double)i/sig.GetFs();
		ax->plot(len, x, sig.buf);
		if (sig.GetNextChan())
			ax->plot(len, x, sig.GetNextChan()->buf, RGB(20,235,0));
		ax->setRange('y', -1, 1);
		ax->setTick('x', 0, x[len-1]/10, 1, 0, "%6.3f");
		ax->setTick('y', 0, 1, 4);
		delete[] x;
		hAccel = GetAccel(curFig = fig);
		hViewPlotDlg = GetHWND_PlotDlg(curFig);

		m_figView[theApp.nPlotWindows] = fig;
		m_axWav[theApp.nPlotWindows] = ax;
		theApp.nPlotWindows++;
	}
}

void CPsyconDlg::PlotHistory(double *ydata)
{ 
	static double rangex1=0, rangex2=-1, rangey1, rangey2;
	int height;
	COLORREF col;
	col = toggle? RGB(140, 200, 0) : RGB(0, 200, 140);
	CLine *pbj = m_axProc->plot(trial+1, ydata, col, (char)2, 1);
	if ( rangex1 > rangex2 ) // If the range has not been initialized  
	{
		rangex1 = 0.;		rangex2 = 10.;
		rangey1 = variable - 5;
		rangey2 = variable + 5;
	}
	else
	{
		if (trial>rangex2-1)				rangex2 += 10;
		if (variable > rangey2)
			rangey2 = variable + .3 * ( variable - rangey1 );
		else if (variable < rangey1)		
			rangey1 = variable - .3 * ( rangey2 - variable );
	}
	m_axProc->setRange('x',rangex1,rangex2);
	m_axProc->setRange('y',rangey1,rangey2);
	m_axProc->setTick('x',0.,5., 0);
	height = (int) (rangey2 - rangey1 +.5);
	m_axProc->setTick('y',rangey1, rangey1+height, 9);
}


void CPsyconDlg::OnEnChangeSignalTag()
{
}

void CPsyconDlg::OnChangeSignalCon() 
{
}

void CPsyconDlg::OnChangeSignalRef() 
{
}

LRESULT CPsyconDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch(message)
	{
	case WM_COMMAND:
		if (wParam==IDOK || wParam==IDCANCEL)
		{
			if (m_bClose==FALSE)			return NULL;
		}
		break;
	case WM_GETDLGCODE:
		return DLGC_WANTALLKEYS;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CPsyconDlg::OnClose() 
{
	m_bClose = TRUE;
	CDialog::OnClose();
}

int CPsyconDlg::ReadCalibrationFileUpdate(void)
{
	CString fnameWpath;
	int res1, res2;
	char buf[32], errStr[256];
	double maxlevel, slope, xx, yy;
	fnameWpath.Format("%s%s", theApp.AppPath, INI_FILE_NAME);
	res1 = sscanfINI(errStr,fnameWpath,"MAX LEVEL(dB)", "%lf",&xx);
	res2 = sscanfINI(errStr,fnameWpath,"dB per Volume_tick", "%lf",&yy);
	if (res1<=0 || res2<=0)
	{	MessageBox ("No previous calibration data. Use relative level (i.e., 0 dB = Full scale)", errStr); xx=0.;}
	maxlevel = xx ;
	slope = yy;
	theApp.m_maxlevel = maxlevel;
	dBperVoltick = slope;
	sprintfFloat (theApp.m_maxlevel, 2, buf, sizeof(buf));
	SetDlgItemText(IDC_MAX_LEVEL, buf);
	return 1;
}

LRESULT CPsyconDlg::OnSelChange(WPARAM wp, LPARAM lparam)
{
	taskID=(int)wp;
	switch (taskID)
	{
	case 0:
		variable = m_child1->adap.initialPt;
		break;
	case 1:
		variable = 0;//m_child2->m_initVal;
		break;
	case 2:
		m_child3->UpdateDataPlus(); //variable is computed here
		break;
	}
	m_child2->m_crv->ShowWindow((taskID == 1)?SW_SHOW:SW_HIDE); // show crv only when Constan Stim
	AstSig.SetTag("v", CSignals(variable));
	return 1;
}

void CPsyconDlg::OnBnClickedGo()
{
	if (taskID != 3) {	// except for custom procedure
		if (PrepareTags() != 1)
			return;

		if (m_strRef.GetLength()==0 || m_strCont.GetLength()==0) 
		{ MessageBox ("Stimulus not defined."); return ;}

		CStdioFile fl;
		if (!fl.Open(INTERMEDIATE_FILE_NAME, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary )) {
			MessageBox ("Cannot create temporary file.");
			return ;
		}
		fl.Close();

		m_child1->adap.warning=false;
		m_child1->InvalidateRect(NULL);
		m_child1->SetDlgItemTextA(IDC_WARNING_STATIC, "Warning when v reaches or crosses");
		if (!UpdateData()) return;
		if (m_child1->adap.dualSeries && nIntervals!=2) 
		{
			MessageBox ("Only 2AFC is allowed for dual-series adaptive procedure.");
			return;
		}
		ClearEditPrintf(m_logEdit.m_hWnd);
		if (m_figProc==NULL)
		{
			CRect rt;
			rt.SetRect(450, 310, 750, 570);
			m_figProc = OpenChildFigure(&rt, CSignals(AstSig.Sig.GetFs())); 
			m_axProc = AddAxis(m_figProc, .17, .1, .75, .8);
		}
		else
		{
			m_axProc->DeleteLine(0);
			m_axProc->setRange('x', 0., -1.);
			m_axProc->setRange('y', 0., -1.);
		}
	}

	CString st, str;
	if (taskID < 2) {	// adaptive & constant stim.
		int res, check, count, cx[8];
		char buffer[256]; 

		res = GetDlgItemInt (IDC_NUM_INTERVALS, &check);
		if (!check) {MessageBox ("Invalid Number of Intervals."); return;}
		nIntervals = res;
		GetDlgItemText (IDC_ANCHOR_INT, st);
		memset((void*)banchor, 0, 8);
		if (st.GetLength()>0)
		{
			count=str2array(cx, 8, st, " ,");
			for (int k=0; k<count; k++) 
				if (cx[k]*(cx[k]-nIntervals-1)>=0) 
					{	MessageBox ("Anchored interval should be between 1 and N",""); return;	}
			for (int k=0; k<count; k++) banchor[cx[k]-1] = true;
		}
		nIntLessAnc = 0;
		for (int i=0; i<nIntervals; i++)	if (!banchor[i]) nIntLessAnc++;
		LONG sentobj = MAKELONG((WORD)nIntervals, (WORD)nIntLessAnc);
		flySendBin (FL_INIT, &sentobj, sizeof(LONG));
		buffer[0]=0;
		for (int i=0; i<nIntervals; i++)
		{
			if (banchor[i])	
			{
				st.Format("%d ", i);
				strcat(buffer, st);
			}
		}
		if (strlen(buffer)>0)	flySendText (FL_INIT2, buffer);
	}

	switch(taskID)
	{
	case 0:
		if (!m_child1->UpdateDataPlus()) return;
		delete[] intervalsig;
		intervalsig = new CSignals[nIntervals];
		FER(flySendText(FL_INSTRUCTION, instruction[0]))
		FER(flySendText(FL_INSTRUCTION, m_child1->m_instruction))
		break;

	case 1:
		if (m_child2->ProcessList()<0) return;
		if (m_child2->nTrials <= 0) {
			MessageBox("Repeat# should be a positive integer.");
			return;
		}
		nConstSteps = m_child2->nSteps;
		nTrials = m_child2->nTrials * nConstSteps;

		delete[] intervalsig;
		intervalsig = new CSignals[nIntervals];
		//de-alloc and alloc 
		delete[] PresentingIDOrder; PresentingIDOrder = new char[nTrials];
		delete[] scores; scores = new double[nConstSteps];		memset((void*)scores, 0, sizeof(double)*nConstSteps);
		delete[] cumTrials; cumTrials = new short[nConstSteps];	memset((void*)cumTrials, 0, sizeof(short)*nConstSteps);
		delete[] cumCorr; cumCorr = new short[nConstSteps];		memset((void*)cumCorr, 0, sizeof(short)*nConstSteps);

		if (m_child2->nVars>0) // if a local tag (a tag used for ConstantStim procedure) was defined 
			for (int i=0; i<m_child2->nVars; i++) 
				tagIDs[i] = m_child2->varNames[i];
		else
			return;

		for (int i=0; i<nConstSteps; i++)
		{
			for (int j=0; j<m_child2->nTrials; j++)
				PresentingIDOrder[i*m_child2->nTrials+j]=i;
		}

		perm (PresentingIDOrder, nTrials);
		FER(flySendText(FL_INSTRUCTION, instruction[0]))
		FER(flySendText(FL_INSTRUCTION, m_child2->m_instruction))
		break;

	case 2:
		if (!m_child3->UpdateDataPlus()) return;
		answer = 1;
		delete[] intervalsig;
		intervalsig = new CSignals[2];

		m_child3->GetDlgItemText(IDC_INIT_VAL, str);
		st.Format("Initial value string:%s, initial value used:%lf\r\n", str, variable);
		DoneSession(st,ADDLONGBUF);
		FER(flySendText(FL_INIT_SLIDER, ""))
		FER(flySendText(FL_INSTRUCTION, m_child2->m_instruction))
		EnableID (m_child3, IDC_INIT_VAL, 0);
		EnableID (m_child3, IDC_STEP_BIG, 0);
		EnableID (m_child3, IDC_STEP_SMALL, 0);
		break;
	}

	if (dlgs[taskID]->OnBnClickedGo() != 1)
		return;

	EnableID (this, IDC_TASKTAB, 0);
	EnableID (this, IDC_GO, 0);
	EnableID (this, IDC_STOP, 1);
	EnableID (this, IDC_SIGNAL_TAG, 0);
	EnableID (this, IDC_SIGNAL_TAG2, 0);
	EnableID (this, IDC_SIGNAL_REF, 0);
	EnableID (this, IDC_SIGNAL_CON, 0);
	EnableID (this, IDC_NUM_INTERVALS, 0);
	EnableID (this, IDC_ANCHOR_INT, 0);
}

void CPsyconDlg::OnBnClickedStop()
{
	if (MessageBox ("Abort the session?", 0, MB_YESNO)!=IDYES) {
		flySendText(FL_RESUME, "");
		return;
	}
	if (dlgs[taskID]->OnBnClickedStop() != 1)
		return;
	flySendText(FL_STOP, "");

	stopped=1;
	EnableID (this, IDC_TASKTAB, 1);
	EnableID (this, IDC_GO, 1);
	EnableID (this, IDC_STOP, 0);
	if (taskID==2)
	{
		EnableID (m_child3, IDC_INIT_VAL, 1);
		EnableID (m_child3, IDC_STEP_BIG, 1);
		EnableID (m_child3, IDC_STEP_SMALL, 1);
	}
	else
	{
		DoneSession();
		switch(taskID)
		{
		case 0:
			break;
		case 1:
			nTrials = m_child2->nTrials*nConstSteps; //What is this?
			break;
		}
	}
	trial=-1;

	EnableID (this, IDC_SIGNAL_TAG, 1);
	EnableID (this, IDC_SIGNAL_TAG2, 1);
	EnableID (this, IDC_SIGNAL_REF, 1);
	EnableID (this, IDC_SIGNAL_CON, 1);
	EnableID (this, IDC_NUM_INTERVALS, 1);
	EnableID (this, IDC_ANCHOR_INT, 1);
}


UINT CPsyconDlg::OnGetDlgCode()
{
	// TODO: Add your message handler code here and/or call default
	return DLGC_WANTALLKEYS;

	//return CDialog::OnGetDlgCode();
}

void CPsyconDlg::OnKillFocus(CWnd* pNewWnd)
{
	CDialog::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here
}

#define ADD_BREAKER(STR) if (strlen(STR)!=0) s += (STR + '\x1f'); else s += " \x1f";

void CPsyconDlg::SigFileOld(int read, LPCSTR fname)
{ // this is necessary to read/write the old-style (prior to version 1.7 and earlier) *.sig files.
	CString s, st;
	CStdioFile fl;
	int res, type; //type : 0 for adaptive, 1 for constant, 2 for adjustment
	if (read)
	{
		if (fl.Open(fname, CFile::modeReadWrite | CFile::typeBinary)) 
		{
			CFileStatus stat;
			char *pbuffer;
			CString temp[7];
			if (fl.GetStatus(stat))
			{
				pbuffer = new char[(UINT)stat.m_size+1];
				UINT dww = fl.Read(pbuffer,(UINT)stat.m_size);
				pbuffer[dww]='\0';
				res = str2array(temp, 7, pbuffer, "\x1f");
				delete[] pbuffer;
				m_strRef = temp[0];
				m_strCont = temp[1];
				m_strTagDef = temp[2];
				if (res<7) // earlier version (only adaptive parameters)
				{
					m_child1->SetDlgItemText(IDC_INIT_VAL, temp[3]);
					m_child1->SetDlgItemText(IDC_STEP1, temp[4]);
					m_child1->SetDlgItemText(IDC_STEP2, temp[5]);
				}
				else
				{
					sscanf(temp[3],"%d",&type);
					switch (type)
					{
					case 0:
						m_child1->SetDlgItemText(IDC_INIT_VAL, temp[4]);
						m_child1->SetDlgItemText(IDC_STEP1, temp[5]);
						m_child1->SetDlgItemText(IDC_STEP2, temp[6]);
						break;
					case 1:
						m_child2->SetDlgItemText(IDC_BEGIN_VAL, temp[4]);
						m_child2->SetDlgItemText(IDC_STEP, temp[5]);
						m_child2->SetDlgItemText(IDC_HOWMANY, temp[6]);
						break;
					case 2:
						m_child3->SetDlgItemText(IDC_INIT_VAL, temp[4]);
						m_child3->SetDlgItemText(IDC_STEP_BIG, temp[5]);
						m_child3->SetDlgItemText(IDC_STEP_SMALL, temp[6]);
						break;

					}
				}
			}
			fl.Close();
		}
		UpdateData(0);
		m_child1->UpdateDataPlus(0);
		m_child2->UpdateData(0);
		m_child3->UpdateDataPlus(0);
	}
	else
	{
		UpdateData();
		m_child1->UpdateDataPlus();
		m_child2->UpdateData();
		m_child3->UpdateDataPlus();

		if (m_strRef.GetLength()!=0) s = m_strRef + '\x1f'; else s = " \x1f";
		ADD_BREAKER(m_strCont)
		ADD_BREAKER(m_strTagDef)
		st.Format("%d", taskID);
		ADD_BREAKER(st)
		switch(taskID)
		{
		case 0:
			m_child1->GetDlgItemText(IDC_INIT_VAL, st);
			ADD_BREAKER(st)
			m_child1->GetDlgItemText(IDC_STEP1, st);
			ADD_BREAKER(st)
			m_child1->GetDlgItemText(IDC_STEP2, st);
			ADD_BREAKER(st)
			break;
		case 1:
			m_child2->GetDlgItemText(IDC_BEGIN_VAL, st);
			ADD_BREAKER(st)
			m_child2->GetDlgItemText(IDC_STEP, st);
			ADD_BREAKER(st)
			m_child2->GetDlgItemText(IDC_HOWMANY, st);
			ADD_BREAKER(st)
			break;
		case 2:
			m_child3->GetDlgItemText(IDC_INIT_VAL, st);
			ADD_BREAKER(st)
			m_child3->GetDlgItemText(IDC_STEP_BIG, st);
			ADD_BREAKER(st)
			m_child3->GetDlgItemText(IDC_STEP_SMALL, st);
			ADD_BREAKER(st)
			break;
		}

		if (!fl.Open(fname, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) MessageBox ("Last signal expression not saved");
		else
		{
			fl.WriteString(s);
			fl.Close();
		}
	}
}

void CPsyconDlg::SigFile(int read, LPCSTR fname)
{
	char drive[128], pathh[256], fn[128], ext[128];
	_splitpath(fname, drive, pathh, fn, ext);
	if (!stricmp(ext, ".sig")) // if the extension is sig
		SigFileOld(read, fname);
	else
	{
		if (read) 
		{
			if (!_ReadINI(fname))
			{
				sprintf(pathh,"%s--invalid format for stim/procedure defintion",fname);
				MessageBox (pathh);
			}
		}
		else WriteINI(fname);
	}
}
BOOL CPsyconDlg::PreTranslateMessage(MSG* pMsg)
{
	ToolTipCtrl.RelayEvent(pMsg);
	return 0;
	int n = ::IsDialogMessage(hViewPlotDlg, pMsg);
	if (pMsg->message==0x100)
		int j=0;
	if (theApp.nPlotWindows==0 || !n || !::TranslateAccelerator(hViewPlotDlg, hAccel, pMsg))
		return CDialog::PreTranslateMessage(pMsg);
	else
	{
		return 1;
	}
}
void CPsyconDlg::OnEnChangeNumIntervals()
{
	int ch;
	int res = GetDlgItemInt(IDC_NUM_INTERVALS, &ch);
	if (ch)
	{
		nIntervals = res;
		UpdateText4SingleInt();
	}
}
