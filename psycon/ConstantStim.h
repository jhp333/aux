#pragma once
#include "Constant_resview.h"

// CConstantStim dialog
class CPsyconDlg;

class CConstantStim : public CProcDialog
{
	DECLARE_DYNAMIC(CConstantStim)

public:
	CPsyconDlg* hPar;
	void GenLogStr(CString& str, string tagStrSpaceDelim, short *cumCorr, short *cumTrials);
	void UpdateEndValue();
	int ProcessList();
	CConstantStim(CPsyconDlg* pParent = NULL);
	virtual ~CConstantStim();

// Dialog Data
	enum { IDD = IDD_CONSTANT };
	int getIDD() { return IDD; }
	const char *getName() { return "Constant"; }
	CConstant_resview *m_crv;
	CString m_instruction;
	CString	varlistStr;
	int		nIntervals;
	CSignals var;
	int		nTrials;
	int		nSteps;
	int		nVars;
	string	*varNames;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnKillfocusIndVarStr();
public:
	virtual BOOL OnInitDialog();
};
