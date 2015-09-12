#pragma once
#include "stdafx.h"  
#include <commctrl.h>  // includes the common control header
#include <process.h>
#include <htmlhelp.h>  // add htmlhelp.lib 
#include "protocol.h"
#include "flyplus.h"
#include "pipeCommunication.h"
#include "audfret.h"
#include "audstr.h"
#include "resource.h"
#include "sigproc.h"
#include "fer.h"
#include "CSession.h"
#include "msgCrack.h"

#include <string>
#include <map>
#include <vector>
using namespace std;
#include "OtherFunctions.h"

#define IDC_TOKEN		3000
#define COL_WAV_NOT_READY	RGB(200, 50, 100)
#define PREV_SESSION_LOADED						1111
#define	USR_MULTI			(WORD)944
#define MAX_BATCH_SIZE					8192
#define MAX_PROCESSING_CONDITION_STRING			32768
#define MAX_NUM_STIM	4
#define MAX_LEN_NODENAME	128
#define MAX_LEN_HEADING		128
#define MAX_NUM_TOKEN		24
#define MAX_LEN_TOKEN		16
#define MAX_LEN_TOKEN_EXAMPLE	64
#define MAX_LEN_WAVE_NAME   128
#define MAX_NUM_WAVE_FILES  1024
#define MAX_NUM_FILE_PER_TOKEN	64
#define MAX_NUM_CHARACTERS_PIPE_COMM	MAX_PROCESSING_CONDITION_STRING*2
#define SETPARAM		3400

#define NUM_CODED_TOKENS	IDS_TOKEN22-IDS_TOKEN1+1 // This must not exceed MAX_NUM_TOKEN. Needs update if the number of pre-selected tokens are changed.

#define ID_STATUSBAR		100
#define CHECK_TOKENS		200
#define READ_TOKENS			201
#define READ_TOKENS_GENERAL	202
#define dontcare		889874

//IDS_TOKEN1 THRU IDS_TOKEN22 MUST BE SEQUENTIAL
#define IDC_TOKEN_B			IDC_TOKEN+IDS_TOKEN1
#define IDC_TOKEN_TH		IDC_TOKEN+IDS_TOKEN22

#define WM__ORG_SESS_SCREEN			  WM_APP+100
#define WM__TOKENS					  WM_APP+101
#define WM__SESSION_DONE			  WM_APP+102
#define WM__PATH_OUTPUT				  WM_APP+103
#define WM__DISP_PIPE_LOG		 	  WM_APP+146
#define WM__WAV_LIST_READY		 	  WM_APP+150
#define WM__RUNBATCH			 	  WM_APP+151
#define WM__SAVE2FILE			 	  WM_APP+152
#define WM__OPEN_SESSION		 	  WM_APP+153

#define ERR_DECODING_INVALID_FORMAT		  -1
#define ERR_DECODING_PARENTH			  -2
#define ERR_DECODING_MUST_BE_TWO_NUMBERS		-3
#define ERR_DECODING_NUMBERS_MUST_BE_ASCENDING	-4
#define ERR_DECODING_NUMBERS_TOO_MANY		-5

#define INTERFACE_PIPENAME  "CochlearAudioSignalInterfacePipe"
#define  STIM_PATH_FILE "\\ResearchAudioSignals\\echostim.wav"
#define  NOISE_PATH_FILE "\\ResearchAudioSignals\\echonoisesource.wav"
#define INI_NAME "echo.ini"
#define LOG_FILE "echo.log"

#define CHEC(x) {if(!(x)) {return -1;}}
#define CHEC2(x) {if(!(x)) {free(buf); return -1;}}
#define CHEC3(X,Y) {int __temp_int = Y; if(!__temp_int) {free(buf); return -1;} else { X = max(X,__temp_int);}}

#define SCANSTR(NAME,VAR) if (ReadINI(errStr, iniFile,NAME,VAR)<=0) GetDefaultParameter(NAME); else
#define SCANSTR2(NAME,VAR) ReadINI(errStr, sessFileNameWPath,NAME,VAR)
#define SCANITEM(NAME,FORMAT,VAR) if (sscanfINI(errStr, iniFile,NAME,FORMAT,&VAR)<=0) GetDefaultParameter(NAME); else
#define SCANITEM2(NAME,FORMAT,VAR,VAR2) if (sscanfINI(errStr, iniFile,NAME,FORMAT,&VAR,&VAR2)<=0) GetDefaultParameter(NAME); else

#define DISP_PIPE_LOG(x,y) EditPrintf (::GetDlgItem(MainAppWnd.hPipeMsgViewDlg,IDC_MSG_LOG), "%s %s\r\n", (char*)x, (char*)y); 

int DecodeNameRuleAndGenerateList (char *ruleStr, map<int,string> &FileNameList, map<int,int> &wavID2tokenID, CSession *CSession);
void thread4pipe (PVOID h);
int randomizeList (int nConc, int **PreOrder, int nCombo);
BOOL CALLBACK  missFilesProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam);
BOOL PopResultFileSaveDlg (HWND, PSTR);
void PopFileInitialize (HWND hwnd);
BOOL SessionFileOpenDlg (HWND hwnd, PSTR pstrFileName, PSTR pstrTitleName);
BOOL SessionFileSaveDlg (HWND hwnd, PSTR pstrFileName, PSTR pstrTitleName);
BOOL ResultFileSaveDlg (HWND hwnd, PSTR pstrFileName);
BOOL CALLBACK  ResultProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK  pipe_msg_log_proc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK  batchProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK  multiProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam);
void getCurrentModulePath(char *moduleName, char *path);
BOOL CALLBACK PathViewProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam);
int timeStamp ( char* dateStr, char* timeStr);
int AddString2NodesList (HWND hDlg, char *nodeFile);
BOOL SendDll(HWND hwnd, char *dllname);
int isConsonantGeneral();
int UpdateINI(char *aPath, HWND hwnd, char *ssfname, char *resultPath);


