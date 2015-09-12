#pragma once

// CSliderAdj dialog
class CPsyconDlg;

class CSliderAdj : public CProcDialog
{
	//DECLARE_DYNAMIC(CSliderAdj)

public:
	CPsyconDlg* hPar;
	BOOL UpdateDataPlus(BOOL bSaveAndValidate = TRUE);
	CSliderAdj(CPsyconDlg* pParent = NULL);
	virtual ~CSliderAdj();

// Dialog Data
	enum { IDD = IDD_SLIDER };
	int getIDD() { return IDD; }
	const char *getName() { return "Adjustment"; }

	double	val;
	CString initValStr;
	CString m_instruction;
	double	m_step_big;
	double	m_step_small;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnEnChangeInitVal();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
