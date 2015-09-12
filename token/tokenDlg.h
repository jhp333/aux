#pragma once
#include "FileDlg.h"
#include "toak.h"

class CTokenDlg : public CWndDlg
{
public:
	CTokenDlg(void);
	~CTokenDlg(void);

	CAstSig AstSig;
	CFileDlg fdlg;
	string sigDef;
	string varDef;
	int trial;
	double level;
	char hostname[128];
	char AppPath[MAX_PATH], applname[MAX_PATH], versionString[64]; 
	HFONT hFontBigTokenView;

	int fs;
	bool fs_ready;
	int nTokens;
	CToak tokManager;

	HWND hPipeMsgViewDlg;
	HINSTANCE hInst;

	int SaveResult(char *tempname);
	int PrepareDataFileHeading(HANDLE hFile, HANDLE hFile2);
	void CreateMyTooltip (HWND hWnd2Display);

	BOOL OnInitDialog(HWND hwndFocus, LPARAM lParam);
	BOOL OnInitDialog2(HWND hwndFocus, LPARAM lParam);
	void OnCommand(int idc, HWND hwndCtl, UINT event);
	void OnSize(UINT state, int cx, int cy);
	void OnClose();
	void OnDestroy();
	HBRUSH OnCtlColorStatic(HDC hdc, HWND hCtrl, int);
	int OnSession(const char *fname);
	int OnDef(const char *fname);
	void WriteDefFile(const char *fname);
	void EvalResponse(HANDLE hFile, HANDLE hFile2, int trial, int &nCorr, int &nTotals);

	int PreparePresent(double level2, char *prepInfo, char *errstr);

	void OnFlyArrived(WORD command, WORD len, void* inBuffer);
	void OnFlyClosed();
	void OnFlyConnected(char *hostname, char* ipa);

	HWND hLog, hStatusbar, hSpDlg;
	int sbarWidth[2];
	char PipeName[64];

//	int Update_last_settings(int update, char *errStr);
	bool Prepare(const char *AuxInputDef, char *estr);
	void MakeTag(CSignals *sig, string varsymbol);
	int PlayStim(int idx, char *estr);
};

extern CTokenDlg tokenMain;