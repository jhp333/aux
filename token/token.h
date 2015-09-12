#include <windows.h>
#include <commctrl.h>  // includes the common control header
#include "WndDlg.h"
#include "audfret.h"
#include "audstr.h"
#include "resource.h"
#include <process.h>
#include "sigproc.h"
#include "flyplus.h"
#include "toak.h"

using namespace std;

#define IDC_STATUSBAR 1010
#define WM__NICSTREAM	WM_APP+100
#define WM_CLEAN_CLOSE	WM_APP+101
#define WM_STREAM_END	WM_APP+102
#define WM__GETPARAM	WM_APP+110
#define WM__GETACE_HANDLE	WM_APP+111

#define MAX_WORDS_PIPEMSG	32
#define CONSOLE_PORT		1200
#define MAX_PROCESSING_CONDITION_STRING			65536

#define ININAME "token.ini"
#define WAVNAME "token.wav"
#define SIGNAL_INTERFACE_PIPENAME  "CochlearAudioSignalInterfacePipe"
#define ACEPLAYER_CONSOLE_PIPENAME "ACE_CONSOLE_PIPE"
#define PREPAREPATH	"ResearchAudioSignals"

#define DISP_PIPE_LOG(x,y) \
	EditPrintf (::GetDlgItem(tokenMain.hPipeMsgViewDlg,IDC_MSG_LOG), "%s %s\r\n", (char*)x, (char*)y); 

void FileDlgInit(OPENFILENAME *ofn, const char *initDir);
BOOL FileOpenDlg (OPENFILENAME *ofn, HWND hwnd, LPSTR pstrFileName, LPSTR pstrTitleName, LPSTR szFilter, LPSTR lpstrDefExt);
BOOL FileSaveDlg (OPENFILENAME *ofn, HWND hwnd, LPSTR pstrFileName, LPSTR pstrTitleName, LPSTR szFilter, LPSTR lpstrDefExt);
void pipeThread (PVOID nsr);
BOOL CALLBACK PipeLogProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam);

