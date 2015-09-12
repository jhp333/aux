#pragma once
#include "afxwin.h"


// CConstant_resview dialog

class CConstant_resview : public CDialog
{
	DECLARE_DYNAMIC(CConstant_resview)

public:
	CConstant_resview(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConstant_resview();
	CWnd* hPar;

// Dialog Data
	enum { IDD = IDD_CONSTANT_LOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_const_log;
	CString m_res_log;
};
