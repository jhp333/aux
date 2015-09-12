// PsyconBaseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "psycon.h"
#include "PsyconBaseDlg.h"
#include "PsyconDlg.h"
#include "Tabber.h"
#include "Settings.h"

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


// CPsyconBaseDlg dialog

IMPLEMENT_DYNAMIC(CPsyconBaseDlg, CDialog)

CPsyconBaseDlg::CPsyconBaseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPsyconBaseDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_PSYCON);
}

CPsyconBaseDlg::~CPsyconBaseDlg()
{
}

void CPsyconBaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
}


BEGIN_MESSAGE_MAP(CPsyconBaseDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(IDM_FILE_SETTINGS, &CPsyconBaseDlg::OnFileSettings)
	ON_COMMAND(IDM_FILE_EXIT, &CPsyconBaseDlg::OnFileExit)
	ON_MESSAGE (WM__SELCHANGE, &CPsyconBaseDlg::OnSelChange)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CPsyconBaseDlg::OnTcnSelchangeTab)
END_MESSAGE_MAP()


// CPsyconBaseDlg message handlers

void CPsyconBaseDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

BOOL CPsyconBaseDlg::OnInitDialog()
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
	char procname[64], verstr[16];
	GetCurrentProcInfo(theApp.AppPath, procname, verstr);
	getVersionStringAndUpdateTitle(m_hWnd, procname, verstr);

	sbar.Create	(WS_CHILD|WS_VISIBLE, CRect(1,2,1,2), this, 1000);
	int rtEdges[] = {170, 220, -1};
	sbar.SetParts(3, rtEdges);


	dlgs[0] = new CPsyconDlg();
	int ID[1];
	ID[0]=IDD_PSYCON_DIALOG;
	m_tab.SetDlgs(dlgs, ID, 1);

	m_tab.InitDialogs(this);
	m_tab.InsertItem(0,"Test");
	m_tab.ActivateTabDialogs();


	//CRect rt;
	//rt.SetRect(0, 250, 787, 625);
	//CFigure * hFig = OpenChildFigure(&rt);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.


void CPsyconBaseDlg::OnPaint()
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

HCURSOR CPsyconBaseDlg::OnQueryDragIcon()
{
//	return (HCURSOR) m_hIcon; // This is from VC6 wizard.
	return CDialog::OnQueryDragIcon();   
}


BOOL CPsyconBaseDlg::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	dlgs[0]->DestroyWindow();

	return CDialog::DestroyWindow();
}

void CPsyconBaseDlg::OnFileSettings()
{
	CSettings dlg (this);
	dlg.DoModal();
}

void CPsyconBaseDlg::OnFileExit()
{
	DestroyWindow();
}

LRESULT CPsyconBaseDlg::OnSelChange(WPARAM wp, LPARAM lp)
{
	return 1;
}
LRESULT CPsyconBaseDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialog::WindowProc(message, wParam, lParam);
}

void CPsyconBaseDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
