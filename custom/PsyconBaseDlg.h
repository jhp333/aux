#pragma once
#include "tabber.h"


// CPsyconBaseDlg dialog

class CPsyconBaseDlg : public CDialog
{
	DECLARE_DYNAMIC(CPsyconBaseDlg)

public:
	CDialog *dlgs[2];
	CMenu menu0;
	CStatusBarCtrl sbar;
	CPsyconBaseDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPsyconBaseDlg();

// Dialog Data
	enum { IDD = IDD_PSYCON_BASE };

protected:
	HICON m_hIcon;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	CTabber m_tab;
public:
	afx_msg void OnPaint();
public:
	afx_msg HCURSOR OnQueryDragIcon();
public:
	virtual BOOL DestroyWindow();
public:
	afx_msg void OnFileSettings();
	afx_msg void OnFileExit();
public:
//	afx_msg void OnEndSession(BOOL bEnding);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg LRESULT OnSelChange(WPARAM wp, LPARAM lp=0);
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
};
