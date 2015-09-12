#include "Echo.h"
#include "WndDlg.h" 

class CMainWnd : public CWndDlg
{
public:
	HWND hDlg;
	CSession ss;
	int cleanpracPlayList, trial, sbarWidth[2], *tokenList, acousticOn, feedBackOn, nResponse, wavListReady;
	char file2save[256];
	int DeleteTempFiles();
	int BeginSessionTransmission();
	void PlayNext();
	void NewSession(char* SessionFile);
	void MakeSummaryHeader(char *_headerBuf, int idPreset, int idAddproc);
	void RawPresentRespSequence2Selected(int m, int n, char **pr, char **rp);
	void SessionDonePrint(int done, FILE *fp);
	void Tokens(int code);
	void DisplaySession(char* wParam, int code);
	BOOL OnInitDialog(HWND hwndFocus, LPARAM lParam);
	void OnFlyConnected(char *hostname, char* ipa);
	void OnFlyClosed();
	void OnFlyArrived(WORD command, WORD len, void* inBuffer);
	void OnSendDll();
	HBRUSH OnCtlColorStatic(HDC hdc, HWND hCtrl, int);
	void OnSize(UINT state, int cx, int cy);
	void OnCommand(int idc, HWND hwndCtl, UINT event);
	void OnClose();
	CMainWnd();
	~CMainWnd();
};
