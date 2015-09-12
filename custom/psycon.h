// psycon.h : main header file for the PSYCON application
//

#if !defined(AFX_PSYCON_H__081B3769_5A31_40DC_B518_A16274879B5C__INCLUDED_)
#define AFX_PSYCON_H__081B3769_5A31_40DC_B518_A16274879B5C__INCLUDED_

#if _MSC_VER > 1000

#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "supportFunctions.h"
#include "flyplus.h"		
#include "audfret.h"
#include "audstr.h"
#include "protocol.h"		
#include "sigproc.h"
#include "precont.h"
#include "graphy.h"
#include <limits>

#define QNAN numeric_limits<long double>::quiet_NaN()
#define LOG10DIV20 0.1151292546497
#define INITVAL 1.2345e-100
#define CHAR_NUMERIC ".0123456789"
#define CHAR_NUMARITHETIC ".+-*/0123456789"


#define FER(x) {\
char linestr[256], estr[128]; int ecode = (x); \
if (ecode<0) { flyErrMsg(ecode, estr); wsprintf (linestr, "Error occurred at "#x", code=%d", ecode); MessageBox (linestr, estr, MB_OK);} }

#define CHEC(x) {if(!(x)) {errstr = errStr; return 0;}}

#define MAX_PLOTWND		32
#define MAX_BRICS		256
#define MAX_TRIALS		200
#define ANY_NUMBER_INT	999

#define WM__NEXT_INTERVAL		WM_APP+410
#define WM__GO					WM_APP+420
#define WM__STOP				WM_APP+430
#define WM__SELCHANGE			WM_APP+440

#define MAX_DEVICE_NUM		4

#define REFERENCE			0
#define CONTROLLED			1
#define ROVING_UNSET		1e10

#define FAILURE_DOUBLE			99999.999

#define LAST_SIG_STRINGS (LPCSTR)"last.sig"
#define INI_FILE_NAME (LPCSTR)"genTask.ini"
#define INTERMEDIATE_FILE_NAME (LPCSTR)"temp-trialxtrial.log"
#define MAX_RUNTIME_DLLS	8

#define JUST_CONNECTED		100
#define TS_DLL_NAME			"psycon_response.dll"


void sprintfFloat(double f, int max_fp, CString& strOut);
int DoesItHaveVariable(const CString& strIn);

/////////////////////////////////////////////////////////////////////////////
// CPsyconApp:
// See psycon.cpp for the implementation of this class
//

class CPsyconApp : public CWinApp
{
public:
	double	m_maxlevel;
	int mclInitialized;
	int nPlotWindows;
	char AppPath[MAX_PATH], VerStr[16];
	CPsyconApp();
	virtual ~CPsyconApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPsyconApp)
	public:
	virtual BOOL InitInstance();
	HMODULE hRunTimeDlls[MAX_RUNTIME_DLLS];

	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPsyconApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CPsyconApp theApp;

class CProcDialog : public CDialog
{
public:
	virtual int getIDD() = 0;
	virtual const char *getName() = 0;
	virtual int OnBnClickedGo() {return 1;}
	virtual LRESULT OnFlyArrived(WPARAM code, LPARAM lParam) {return 1;}
	virtual int OnBnClickedStop() {return 1;}
	virtual int DoneSession() {return 1;}
	CProcDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL) : CDialog(nIDTemplate, pParentWnd) {}
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PSYCON_H__081B3769_5A31_40DC_B518_A16274879B5C__INCLUDED_)
