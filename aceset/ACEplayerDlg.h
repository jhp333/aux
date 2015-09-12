#pragma once
#include "ACEsetDlg_base.h"
#include "FileDlg.h"

class CACEplayerDlg : public CWndDlg
{
public:
	CAstSig AstSig;
	CSignals Sig;

	CACEplayerDlg(void);
	~CACEplayerDlg(void);

	CFileDlg fdlg;
	int GetACEparams(char *errstr);
	int Prepare (ACESEQ *seq, const char *wavName, double& duration, CStdString &resStr);
	int Present (ACESEQ *seq, CStdString &resStr);
	int Set (CStdString *pstr, int nArg, CStdString &resStr);

	BOOL OnInitDialog(HWND hwndFocus, LPARAM lParam);
	void OnCommand(int idc, HWND hwndCtl, UINT event);
	void OnSize(UINT state, int cx, int cy);
	void OnClose();
	void OnDestroy();

	void OnFlyArrived(WORD command, WORD len, void* inBuffer);
	void OnFlyClosed();
	void OnFlyConnected(char *hostname, char* ipa);

	bool PrepareAUX();
	void swapAB();

	int nConsoles;
	CParamACE *param, *param2;
	char streamFile[MAX_PATH];
	int fMakeStreamFile;
	HWND hLog, hStatusbar, hSpDlg;
	int sbarWidth[2];
	double duration;
	double dur_powerframes;
	char PipeName[64];
	char AppPath[MAX_PATH];
	char nicPATH[MAX_PATH];
	char aceprocdllname[MAX_PATH];
	char mxacedllname[MAX_PATH];
	InterfaceType itype;
	CACEDlg hACEobj[2];

	int ReadSettings(char *errStr);
	void DisplayInitScreen();
	int UpdateINI(char *errStr);
	void DisplayPulses(ACESEQ *seq, const char* inputlevelstr);

	int (*GateProcess)(ACESEQ *, const char *, const char *, const char *, CParamACE *, char **, char **);
	int (*GetCutoffFreqs)(int, double*, double*);
	int (*FixCRCR)(char *);
	int (*InitMatlabLib)(const char *, char **);
	void (*CloseMatlabLib)();
	CParamACE* (*newCParamACE)();
};
