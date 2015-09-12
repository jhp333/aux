#pragma once

#include "Adaptive.h"

// CAdaptiveDlg dialog

#define IDT_TRIAL_DELAYS		2200
#define IDT_INTERVAL_DELAYS		2201
#define IDT_INIT_TABS			2202

class CPsyconDlg;

class CAdaptiveDlg : public CProcDialog
{
	DECLARE_DYNAMIC(CAdaptiveDlg)

public:
	void SyncAdap();
	void CheckValuesUpdateADscending();
	void InitAdaptive(CAdaptive *adapObj, int *corrHist, double* thrCand);
	BOOL UpdateDataPlus(BOOL bSaveAndValidate = TRUE);
	CPsyconDlg* hPar;
	CAdaptive adap;
	CAdaptive adap2;
	CAdaptiveDlg(CPsyconDlg* pParent = NULL);
	virtual ~CAdaptiveDlg();

// Dialog Data
	enum { IDD = IDD_ADAPTIVE };
	int getIDD() { return IDD; }
	const char *getName() { return "Adaptive"; }
	CButton	m_fdback_check;
	double m_crossing_limit;
	CString m_instruction;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	virtual BOOL OnInitDialog();
	afx_msg void OnUpdateInitVal();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeReversals2();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
