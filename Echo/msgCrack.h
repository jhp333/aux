//From windowsx.h

#define     SetDlgMsgResult(hwnd, msg, result) (( \
        (msg) == WM_CTLCOLORMSGBOX      || \
        (msg) == WM_CTLCOLOREDIT        || \
        (msg) == WM_CTLCOLORLISTBOX     || \
        (msg) == WM_CTLCOLORBTN         || \
        (msg) == WM_CTLCOLORDLG         || \
        (msg) == WM_CTLCOLORSCROLLBAR   || \
        (msg) == WM_CTLCOLORSTATIC      || \
        (msg) == WM_COMPAREITEM         || \
        (msg) == WM_VKEYTOITEM          || \
        (msg) == WM_CHARTOITEM          || \
        (msg) == WM_QUERYDRAGICON       || \
        (msg) == WM_INITDIALOG             \
    ) ? (BOOL)(result) : (SetWindowLong((hwnd), DWL_MSGRESULT, (LPARAM)(LRESULT)(result)), TRUE))

#define chHANDLE_DLGMSG(hwnd, message, fn)                 \
   case (message): return (SetDlgMsgResult(hwnd, umsg,     \
      HANDLE_##message((hwnd), (wParam), (lParam), (fn))))

//Modified message handler.
#define HANDLE_WM_COMMAND(hwnd, wParam, lParam, fn) \
    ((fn)((int)(LOWORD(wParam)), (HWND)(lParam), (UINT)HIWORD(wParam)), 0L)
#define HANDLE_WM_INITDIALOG(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(BOOL)(fn)((HWND)(wParam), lParam)
#define HANDLE_WM_CTLCOLORSTATIC(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((HDC)(wParam), (HWND)(lParam), CTLCOLOR_STATIC)
#define HANDLE_WM_CLOSE(hwnd, wParam, lParam, fn) \
    ((fn)(), 0L)
#define HANDLE_WM_SIZE(hwnd, wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)
#define HANDLE_WM_DESTROY(hwnd, wParam, lParam, fn) \
    ((fn)(), 0L)

//void OnFlyConnected(HWND hDlg, char *hostname, char* ipa)
#define HANDLE_WM_FL_CONNECTED(hwnd, wParam, lParam, fn) \
    ((fn)((char*)wParam, (char*)lParam), 0L)

//void OnFlyClosed(HWND hDlg)
#define HANDLE_WM_FL_CLOSE(hwnd, wParam, lParam, fn) \
    ((fn)(), 0L)

//void OnFlyArrived(HWND hDlg, WORD code, WORD nBytes, void* inBuffer)
#define HANDLE_WM_FL_ARRIVED(hwnd, wParam, lParam, fn) \
    ((fn)(LOWORD(wParam), HIWORD(wParam), (void*)lParam), 0L)

//void NewSession(HWND hDlg, char* SessionFile)
#define HANDLE_WM__OPEN_SESSION(hwnd, wParam, lParam, fn) \
    ((fn)((char*)wParam), 0L)

//void SessionDonePrint(HWND hDlg, int done, FILE *fp)
#define HANDLE_WM__SESSION_DONE(hwnd, wParam, lParam, fn) \
    ((fn)(1L, (FILE*)NULL), 0L)

//void Tokens(HWND hDlg, int code)
#define HANDLE_WM__TOKENS(hwnd, wParam, lParam, fn) \
    ((fn)((int)lParam), 0L)

// void DisplaySession(HWND hDlg, char* wParam, int code)
#define HANDLE_WM__ORG_SESS_SCREEN(hwnd, wParam, lParam, fn) \
    ((fn)((char*)wParam, (int)lParam), 0L)

/* void Cls_OnTimer(HWND hwnd, UINT id) */
#define HANDLE_WM_TIMER(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (UINT)(wParam)), 0L)


#define HANDLE_WM__SEND_DLL(hwnd, wParam, lParam, fn) \
    ((fn)(), 0L)

#define HANDLE_WM__SAVE2FILE(hwnd, wParam, lParam, fn) ((fn)((char*)wParam), 0L)
#define HANDLE_WM__RUNBATCH(hwnd, wParam, lParam, fn) ((fn)(), 0L)