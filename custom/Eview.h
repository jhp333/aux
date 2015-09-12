#pragma once

#include "psycon.h"	

// CEview dialog

class CEview : public CDialog
{
	DECLARE_DYNAMIC(CEview)

public:
	HWND hExpression; // necessary to avoid accel processing in the Edit box 
	HWND hButt; // another variable used in  PreTranslateMessage
	HWND hViewPlotDlg;
	HACCEL hAccel;
	CFigure* curFig;
	CAxis * hAx[2];
	CFigure *hFig;
	CEview();
	virtual ~CEview();

// Dialog Data
	enum { IDD = IDD_EASYVIEW };

	virtual BOOL OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
//	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
public:
	afx_msg void OnBnClickedGenerate();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	afx_msg void OnEnKillfocusExpression();
public:
	afx_msg void OnStnClickedStaticPlot();
public:
	afx_msg void OnStnDblclickStaticPlot();
public:
	afx_msg void OnBnClickedPlot();
public:
	afx_msg void OnEnChangeExpression();
};
