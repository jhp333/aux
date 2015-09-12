#pragma once
//using namespace std;

#include "sigplus_internal.h"
#include "WndDlg.h"

#define SIGPROCDLG_API __declspec(dllexport)

SIGPROCDLG_API int PrepareSignals(string varstr, string sigstr, CAstSig *allhere, char *errstr);

class CSigProcDlg :	public CWndDlg
{
public:
	SIGPROCDLG_API HWND Create(HWND hPar, POINT position, int fs);
//	SIGPROCDLG_API CACEDlg* GetThis();
	SIGPROCDLG_API int PrepareSignals();
	SIGPROCDLG_API int SetVar(const char *varname, const CSignals &signal);
	SIGPROCDLG_API int SetVar(const char *strIn);
	SIGPROCDLG_API void SetEditVar(const char *strIn);
	SIGPROCDLG_API void SetEditSig(const char *strIn);
	SIGPROCDLG_API void EnableEdits(int onoff);

	//BOOL OnInitDialog(HWND hwndFocus, LPARAM lParam);
	void OnCommand(int idc, HWND hwndCtl, UINT event);
	
	string errstr;
	string varstr;
	string sigstr;
	CAstSig *grp;
	SIGPROCDLG_API CSigProcDlg(void); 
	SIGPROCDLG_API ~CSigProcDlg(void);

private:
	bool PrepareDef();
	bool PrepareSig();

};