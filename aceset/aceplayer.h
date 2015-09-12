#include <windows.h>
#include <StdString.h>
#include <commctrl.h>  // includes the common control header
#include "aceproc.h"
#include "audfret.h"
#include "audstr.h"
#include "resource_aceplayer.h"

typedef enum
{
	UNSPECIFIED = 0,
	SPRINT,
	L34_CIC3,
	L34_CIC4,
} InterfaceType;

#include "aceset.h"
#include <process.h>
#include "sigproc.h"
#include "flyplus.h"

using namespace std;

#define IDC_STATUSBAR 1010
#define WM__NICSTREAM	WM_APP+100
#define WM_CLEAN_CLOSE	WM_APP+101
#define WM_STREAM_END	WM_APP+102
#define WM__GETPARAM	WM_APP+110
#define WM__GETACE_HANDLE	WM_APP+111

#define MAX_WORDS_PIPEMSG	32
#define CONSOLE_PORT		1200

#define FS 16000
#define WAVNAME "lastPlayed.wav"
#define SIGNAL_INTERFACE_PIPENAME  "CochlearAudioSignalInterfacePipe"
#define ACEPLAYER_CONSOLE_PIPENAME "ACE_CONSOLE_PIPE"

#define INIFILE "aceplayer.ini"


void sprintfFloat(double f, int max_fp, CStdString& strOut);
void FileDlgInit(OPENFILENAME *ofn, const char *initDir);
BOOL FileOpenDlg (OPENFILENAME *ofn, HWND hwnd, LPSTR pstrFileName, LPSTR pstrTitleName, LPSTR szFilter, LPSTR lpstrDefExt);
BOOL FileSaveDlg (OPENFILENAME *ofn, HWND hwnd, LPSTR pstrFileName, LPSTR pstrTitleName, LPSTR szFilter, LPSTR lpstrDefExt);
BOOL CALLBACK SmallDlg (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK SetSpProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam);
void pipeThread (PVOID nsr);
void genProcessingString (char *out, size_t outsize, CParamACE *param, int nOutChan, double rmsofwavefile);
BOOL CALLBACK PipeLogProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam);


// For NICstream 1
int InitNICStream (HWND hDlg, const char *nicpath, char *errstr);
int NICStreamIt (const char *qFile, char *errstr);
void NICstreamCleanUp();
void OnStreamEnd ();
void FormatErrStr(int errcode, char *errmsg);

// For NIC2
int InitNIC2(HWND hDlg, const char *nicpath, const char *initializeStr, char *errstr);
int NIC2CleanUp();
int NIC2StreamIt (const char *streamFile, char *errstr, bool readandstream=0);
int NIC2StreamIt (ACESEQ *seq, double dur_powerframes, char *errstr, bool readandstream);

extern HINSTANCE hInst;
extern HWND hMainDlg, hPipeLog, hMATLABMSG, hPulseView;
extern unsigned int streamThreadID;