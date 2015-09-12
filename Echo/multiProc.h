#if !defined(CmultiProc_DEFINED)
#define CmultiProc_DEFINED

#include <afxwin.h>
#include <vector>
#include "WndDlg.h" 

class CmultiProc : public CWndDlg
{
public:
	vector<string> TrialVarNames;
	CSignals TrialVarValues;
	void Alloc();
	char **PresenterSettings;	
	char **AddProcs;
	int nPresets;
	int nAddprocs;
	int *presetID;
	int *addprocID;
	int nTrials;

	int setNth(const int index, const vector<string> &varNames, const CSignals &varlist);
	int enumVarStr(const string varlistStr, vector<string> &varNames, CSignals &varlist);
	int ReadyProcessingStrings();
	BOOL OnInitDialog(HWND hwndFocus, LPARAM lParam);
	void OnCommand(int idc, HWND hwndCtl, UINT event);
	void OnDestroy();
	void OnTimer(HWND hwnd, UINT id);
	CmultiProc();
	~CmultiProc();
private:
	void CleanAllocced();
};

#endif 