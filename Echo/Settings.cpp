// Settings.cpp : implementation file
//
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSettings dialog


CSettings::CSettings(CWndDlg* pParent /*=NULL*/)
	: CDialog(CSettings::IDD)
	, m_fs(22050)
{
	//{{AFX_DATA_INIT(CSettings)
	hPar = (CEchoMainWnd *)pParent;
	//}}AFX_DATA_INIT
}


void CSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettings)
	DDX_Text(pDX, IDC_FS, hPar->Fs);
	DDX_Text(pDX, IDC_UDFPATH, m_udfpath);
	// Log reporting
	DDX_Check(pDX, IDC_LOG_EMAIL, hPar->report.fUseEmail);
	DDX_Text(pDX, IDC_EMAIL_ADDRESS, hPar->report.email_address);
	DDX_Check(pDX, IDC_LOG_FTP, hPar->report.fUseFtp);
	DDX_Text(pDX, IDC_FTP_ADDRESS, hPar->report.ftp_address);
	DDX_Text(pDX, IDC_FTP_ID, hPar->report.ftp_id);
	DDX_Text(pDX, IDC_FTP_PASSWORD, hPar->report.ftp_password);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettings, CDialog)
	//{{AFX_MSG_MAP(CSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettings message handlers

void CSettings::OnOK() 
{
	CDialog::OnOK();	// includes UpdateData();
	if (m_udfpath.GetLength()>0)
		hPar->AstSig.SetPath("aux", m_udfpath);
	else
		hPar->AstSig.SetPath("aux", hPar->AppPath);

	// update INI file
	char iniFile[MAX_PATH], errStr[256];
	sprintf (iniFile, "%s%s", hPar->AppPath, INI_NAME);
	printfINI(errStr, iniFile, "SAMPLE RATE", "%d", hPar->Fs);
	printfINI(errStr, iniFile, "UDFPATH", "%s", m_udfpath);
	hPar->report.WriteINI(iniFile);
}

BOOL CSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	UpdateData(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}