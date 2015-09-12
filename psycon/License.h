#pragma once


// CLicense dialog

class CLicense : public CDialog
{
	DECLARE_DYNAMIC(CLicense)

public:
	CStatusBarCtrl sbar;
	CLicense(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLicense();

// Dialog Data
	enum { IDD = IDD_LICENSE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
//	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnBnClickedCheck();
public:
	virtual BOOL OnInitDialog();
};
