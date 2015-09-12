#include "stdafx.h"
#if !defined(AFX_SETTINGS_H__ED519019_7251_4E86_989C_C76F97D7BA53__INCLUDED_)
#define AFX_SETTINGS_H__ED519019_7251_4E86_989C_C76F97D7BA53__INCLUDED_
#include "Echo.h"
#include "EchoMainWnd.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Settings.h : header file
//
#define WM__CAL_PLAYBACK WM_APP+100

/////////////////////////////////////////////////////////////////////////////
// CSettings dialog

class CSettings : public CDialog
{
// Construction
public:
	CEchoMainWnd *hPar;
	CSettings(CWndDlg* pParent = NULL);   // standard constructor
	void OnEnKillfocusCalLevel();

// Dialog Data
	//{{AFX_DATA(CSettings)
	enum { IDD = IDD_SETTINGS };
	CString m_udfpath;
	int		m_fs;
	// Log reporting
	BOOL   m_log_email;
	BOOL   m_log_ftp;
	CString m_email_address;
	CString m_ftp_address;
	CString m_ftp_id;
	CString m_ftp_password;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSettings)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnPlayStop();
	afx_msg void OnKillFocusVol1();
	afx_msg void OnKillFocusVol2();
	//}}AFX_MSG
	afx_msg void OnChangeDev(UINT nID, bool more=true);
	afx_msg void OnKillfocusCalLevel(UINT nID);
	afx_msg LRESULT onCalPlaybackCB(WPARAM wp, LPARAM lp);
DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedCopyVol1();
	afx_msg void OnBnClickedCopyVol2();
private:
	afx_msg void OnBnClickedUpdateCal();
	CSliderCtrl m_volSlider1;
	CSliderCtrl m_volSlider2;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
private:
	void OnKillFocusVol(int idc);
	afx_msg void OnCbnSelchangeProcedurePlay();
	afx_msg void OnCbnSelchangeSingleplay();
	CButton m_cont;
	afx_msg void OnBnClickedUseAsController();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGS_H__ED519019_7251_4E86_989C_C76F97D7BA53__INCLUDED_)
