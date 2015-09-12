#include <windows.h>
#include <flyplus.h>
#include <math.h>
#include <commctrl.h>
#include "audfret.h"
#include "..\protocol.h"
#include "resource.h"

#include <vector>
using namespace std;

#define CENTERPTX     450
#define CENTERPTY     330

#define PROGRESSBAR_LEFT     410
#define PROGRESSBAR_WIDTH    600
#define PROGRESSBAR_TOP    700
#define PROGRESSBAR_HEIGHT   20
#define DIALOG_COLOR			RGB(50, 160, 255)
#define DIALOG_COLOR_DISALBED   RGB(200, 200, 180)
#define COLOR_RED				RGB(200, 30, 100)
#define COLOR_CLICKED			RGB(190, 230, 30)
#define COLOR_FEEDBACK			RGB(230, 50, 30)


#define IDC_DONE			1498
#define IDC_START			1499
#define IDC_INSTRUCTION		1001
#define IDC_LIVETEXT		1002
#define IDC_CORRECT			10033
#define IDC_RESP_BUTTON		1500
#define IDC_PROGRESS		4500
#define IDC_ANS_EDIT		784


#define CLICKED_RELEASE		20900
#define TURNOFF_FEEDBACK	20920
#define SEND_AFTER_FEEDBACK_OFF 20940

#define FEEDBACK_DURATION	600 // the duration in ms of feedback display

#define WM__FEEDBACKDISPLAY			(WM_APP+101)
#define WM__REDRAW_BUTTONS			(WM_APP+102)
#define WM__DISABLE_ALL				(WM_APP+103)
#define WM__SET_PRACTICE_INST		(WM_APP+104)

#define MAX_NUM_STIM	4

#define INI_FILE "echotouch.ini"

#define FER(x) {\
char linestr[256], estr[128]; int ecode = (x); \
if (ecode<0) { flyErrMsg(ecode, estr); wsprintf (linestr, "Error occurred at "#x", code=%d", ecode); MessageBox (NULL	, linestr, estr, MB_OK);} }

int IsThiscorrectAnswer (int id, int *lastAns, int nConc);
BOOL CALLBACK buttonProc (HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);

//extern HMODULE hDllModule;
//extern WNDPROC hOrgProc;
extern HINSTANCE hInst;
extern int buttonEnabled, nConc, practiceMode, fontsize[3];
extern int feedBackOn, fSingleResponse, fAllowSame, fdBackOnNow, SessionProgress;

extern UCHAR correctAns[MAX_NUM_STIM];
