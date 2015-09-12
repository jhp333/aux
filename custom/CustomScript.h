#pragma once

// CCustomScript dialog
class CPsyconDlg;

class CCustomScript : public CProcDialog
{
	DECLARE_DYNAMIC(CCustomScript)

public:
	CPsyconDlg *hPar;
	CCustomScript(CPsyconDlg* pParent = NULL);
	virtual ~CCustomScript();
	virtual BOOL OnInitDialog();
	void OnBnClickedExtEditor();
	int OnBnClickedGo();
	LRESULT OnFlyArrived(WPARAM code, LPARAM lParam);
	int OnBnClickedStop();
	int DoneSession();
	int killProcedure();

// Dialog Data
	enum { IDD = IDD_CUSTOM };
	int getIDD() { return IDD; }
	const char *getName() { return "Custom"; }
	CAstSig AstSig;
	CString m_Script;
	HANDLE hEvent;
	int LastResponse;
	CSignals LastRespSig;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:
};
