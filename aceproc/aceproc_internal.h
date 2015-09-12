// aceproc.h : main header file for the ACEPROC DLL
//
#include "stdafx.h"

#if !defined(AFX_ACEPROC_H__B73D236C_E395_4009_8F67_8E28B0DC681C__INCLUDED_)
#define AFX_ACEPROC_H__B73D236C_E395_4009_8F67_8E28B0DC681C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "HELP.h"	// Added by ClassView

#include "mxaceproc55.h"
#include "audfret.h"
#include "aceproc.h"


/////////////////////////////////////////////////////////////////////////////
// CAceprocApp
// See aceproc.cpp for the implementation of this class
//

class CAceprocApp : public CWinApp
{
public:
//	char path_to_dll[_MAX_PATH]; // Used in codes generated in Matlab compiler
	double onoffConditioner(const char *qufnameCond, int e, int cl, int rate, int onoff, char **errstr);
	int mxArray2ACESEQ(ACESEQ *seq, mxArray *X);
	int str2ArrayOfmxArray (mxArray **X, CString s);
	CString str2SinglemxArray(mxArray **X, CString s);
	int CheckBracketOpenClosed (int*x, int*y, int k);
	int str2mxArray(mxArray **var, int nVar, CString str);
	void closeLib();
	int _FixCRCR_(char*str);
	int _numbands2Cutfreqs(double **freqs, int nBands);
	int _genconstim(const char *qufname, int *elec, int *cl, int nChan, double pw, double pg, double ChanRate, int nPulseChan, int condE, int condCL, char **errstr);
	int Conditioner(char *qufname, ACESEQ *seq, char *str, int nVarargin);
	int ACE(char *qufname, ACESEQ *seqOut);
	void aceparam_c2mx(CParamACE *param, mxArray **xP);
	BOOL initLib();
	BOOL libInitialized;
	char* wavName;
	CParamACE param;
	char* errStr;
	int GateProcess(ACESEQ *seq, char *procStr, char *wavName, char *qufname, CParamACE *param, char **errStr);
	void ShowHelp();
	CHELP helpWindow;
	CAceprocApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAceprocApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAceprocApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACEPROC_H__B73D236C_E395_4009_8F67_8E28B0DC681C__INCLUDED_)
