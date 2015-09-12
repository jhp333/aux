// psycon.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "psycon.h"
#include "PsyconDlg.h"
#include "License.h"

int CheckLicense(char* errmsg);

#include "mclmcr.h" // for mclInitializeApplication

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPsyconApp

BEGIN_MESSAGE_MAP(CPsyconApp, CWinApp)
	//{{AFX_MSG_MAP(CPsyconApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPsyconApp construction

CPsyconApp::CPsyconApp()
: m_maxlevel(0.)
{
	AppPath[0]=0;
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CPsyconApp::~CPsyconApp()
{
	//if (hVOCOD_DLL!=NULL) ::FreeLibrary(hVOCOD_DLL);
	//hVOCOD_DLL = NULL;
	//if (hLibmcr!=NULL) ::FreeLibrary(hLibmcr);
	//hLibmcr = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPsyconApp object

CPsyconApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPsyconApp initialization

BOOL CPsyconApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CString str;
/*
	char errstr[128];

	int res = CheckLicense(errstr);
	if (res!=1)
	{
		str.Format("License Validation Failed. (%s)", errstr);
		AfxMessageBox(str);
		return 0;
	}
*/
	CPsyconDlg dlg;
//	mclInitialized=mclInitializeApplication(NULL,0);
//	if (!mclInitialized)
//		AfxMessageBox ("mclInitializeApplication failed...... No filtering available");

	nPlotWindows=0;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.

	return FALSE;
}

