// ResultWnd.h: interface for the CResultWnd class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(CResultWnd_DEFINED)
#define CResultWnd_DEFINED

#if !defined(AFX_RESULTWND_H__41CDC850_E58F_4709_ABF2_AC2E721B066C__INCLUDED_)
#define AFX_RESULTWND_H__41CDC850_E58F_4709_ABF2_AC2E721B066C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndDlg.h" 

#include <afxwin.h>  
#include <stdio.h>  
#include "resource.h"


class CResultWnd  : public CWndDlg
{
public:
	int nOutputCharsBeforeNote;
	CString displayedStrWOcommnet;
	int saved;
	char FileNameWPath[512];
	void Save2File(char* fname);
	void OnCommand(int idc, HWND hwndCtl, UINT event);
	void OnClose();
	void outputSummary(FILE *fp, int *po, unsigned char **rp, int numTrials);
	void outputFooter(FILE *fp);
	BOOL OnInitDialog(HWND hwndFocus, LPARAM lParam);
	CResultWnd();
	virtual ~CResultWnd();
private:
	int GetFeatureScore(char *featureScoreOut, const char c, int * presented, int * responded, int numTrials);
	double CalculateFeatureTransmission (int *confMtx, int *featureDefTable);
	void GetConfMtx (int *confMtx, int *presented, int *responded, int numTrials, int *featureDefTable);
	void GetMappedTable (int *mappedTable, int *featureDefTable, int len);
	int countFeatures (int *featureDefTable, int len);
};

#endif // !defined(AFX_RESULTWND_H__41CDC850_E58F_4709_ABF2_AC2E721B066C__INCLUDED_)

#endif // !defined(CResultWnd_DEFINED)
