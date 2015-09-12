#pragma once
#include "FileDlg.h"
#include "auxgv.h"

class CAuxDlg : public CWndDlg
{
public:
	CSignals Sig;
	CAstSig AstSig;

	HFONT eFont;

	CAuxDlg(void);
	~CAuxDlg(void);
	HWND hStatusbar;
	HFONT hFontBigTokenView;
	BOOL OnInitDialog(HWND hwndFocus, LPARAM lParam);
	void OnCommand(int idc, HWND hwndCtl, UINT event);
	void OnSize(UINT state, int cx, int cy);
	void OnClose();
	void OnDestroy();
	HBRUSH OnCtlColorStatic(HDC hdc, HWND hCtrl, int id);
	HBRUSH OnCtlColorEdit(HDC hdc, HWND hCtrl, int id);
	HBRUSH OnCtlColor(HDC hdc, HWND hCtrl, int id);
	void OnFile(char *fname, int idc);
	string m_auxstr;
	void OnBnClickedMakePlot();
	bool PrepareSig();
	int writeINI(char *fname, char *estr);
	int readINI(char *fname, char *estr);
	HACCEL hAccel;
	CFileDlg fdlg;
	HANDLE curFig;
	HANDLE *figwnd;
	int nFigureWnd;
	int fs;
	double block; // block size for playback, in milliseconds
	char VerStr[32];
};

extern CAuxDlg auxMain;