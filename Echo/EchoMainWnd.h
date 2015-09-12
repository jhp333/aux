// EchoMainWnd.h: interface for the CEchoMainWnd class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "batch.h"
#include "ResultWnd.h"
#include "multiProc.h"
#include "sigproc.h"
#include "LogReport.h"
#include <map>
#include <vector>

#define TS_DLL_NAME		"echotouch.dll"
#define REQUIRED_TS_DLL_VERSION		"1.2"
#define JUST_CONNECTED	100

class CEchoMainWnd : public CWndDlg
{
public:
	CAstSig AstSig;
	CSignals Sig;
	int isConsonantGeneral();
	int ReadINI_UpdateScreen();
	int UpdateINI(char *ssfname);
	int ReadSessFile (char* errStr);
	int WriteSessFile (char* errStr);
	void CreateTokensNShow();
	void CreateMyTooltip (HWND hWnd2Display);
	int nConc, nTrials, Fs;
	int consTesting;
	char sessFileNameWPath[512], sessFileName[128], missingFiles[8192], versionString[16];
	map<int,string> waveNameList, pracPlayList, logPlayList;
	map<int,int> wavID2tokenID;
	vector<vector<int>> Combs;
	HANDLE hPipe;
	HWND hStatusbar, hRespLog, hGList ;
	int multi, nSpeakers, connected;
	unsigned char *respCollected[MAX_NUM_STIM], *idPresets, *idAddProcs;
	int *presentOrder;
	void CleanUpTestingSession();
	string sessionBeganTime, sessionSubjectID;
	char preparedCondition[MAX_PROCESSING_CONDITION_STRING+512];
	map<int,map<int,string>> PresenterInfos;
	char AppPath[_MAX_PATH];
	char currentDir[_MAX_PATH];
	char resultPath[_MAX_PATH];
	char resultFile[_MAX_PATH];
	HWND hPipeMsgViewDlg;
	HINSTANCE hInst;
	HWND hMissingDlg;
	CResultWnd *hResult;
	BATCH *batch;
	double level;
	CmultiProc *hMulti;
	CSession *ss;
	int cleanpracPlayList, trial, sbarWidth[2], *tokenList, acousticOn, feedBackOn, fSingleResponse, fAllowSameToken, nResponse, wavListReady;
	char file2save[256];
	LogReport report;

	int BeginSessionTransmission();
	bool PrepareAUX();
	int ReadLineandWav(const char *str, char *errStr);
	int PlayStim();
	int PreparePresent(double level2, char *errstr);
	void PlayNext();
	void NewSession(char* SessionFile);
	void RawPresentRespSequence2Selected(int m, int n, int *po, unsigned char **rp);
	void outputPresentedResponded(FILE *fp, int m, int n);
	void SessionDonePrint(int done, FILE *fp);
	void Tokens(int code);
	int	SeeFolder (int control, char *path);
	int GetNConcfromProcStr(char *errstr);
	void appendLine2File(char *filename, char *str);

	void GetDefaultParameter(char *ItemName);
	int FillDesireWavList (int control, int control2);
	int CheckWavFilesReadyAndDisplay (int control, map<int,string> &waveNameList, char *missingFiles);
	void DisplaySession(char* wParam, int code);
	void CopyEchoStimsIfNecessary(char* wavname);
	BOOL OnInitDialog(HWND hwndFocus, LPARAM lParam);
	void OnFlyConnected(char *hostname, char* ipa);
	void OnFlyClosed();
	void OnFlyArrived(WORD command, WORD len, void* inBuffer);
	HBRUSH OnCtlColorStatic(HDC hdc, HWND hCtrl, int);
	void OnSize(UINT state, int cx, int cy);
	void OnCommand(int idc, HWND hwndCtl, UINT event);
	void OnClose();
	void OnTimer(HWND hwnd, UINT id);
	CEchoMainWnd();
	virtual ~CEchoMainWnd();
private:
	int GenerateRandomList ();
	int genCombination_checkToken(vector<vector<int>> &combs, int n, int k);
};

extern CEchoMainWnd MainAppWnd;

