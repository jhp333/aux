//From windowsx.h
// but with WIn32++, this was modified to resemble the MFC interface.

#define xt2CPoint(xpos,ypos) uglyfcn((xpos),(ypos))

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

#define HANDLE_WM_PAINT(hwnd, wParam, lParam, fn) \
    ((fn)(), 0L)
#define HANDLE_WM_CTLCOLORSTATIC(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(HBRUSH)(fn)((HDC)(wParam), (HWND)(lParam), CTLCOLOR_STATIC)
#define HANDLE_WM_CLOSE(hwnd, wParam, lParam, fn) \
    ((fn)(), 0L)
#define HANDLE_WM_SIZE(hwnd, wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)
#define HANDLE_WM_DESTROY(hwnd, wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Cls_OnKeyDown(UINT vk, int cRepeat, UINT flags) */
#define HANDLE_WM_KEYDOWN(hwnd, wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam)), 0L)
/* void Cls_OnKeyUp(UINT vk, int cRepeat, UINT flags) */
#define HANDLE_WM_KEYUP(hwnd, wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam)), 0L)

/* void Cls_OnMouseMove(UINT keyFlags, CPoint cpt) */
#define HANDLE_WM_MOUSEMOVE(hwnd, wParam, lParam, fn) \
    ((fn)((UINT)(wParam), xt2CPoint((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam))), 0L)

/* void Cls_OnLButtonDown(UINT keyFlags, CPoint cpt) */
#define HANDLE_WM_LBUTTONDOWN(hwnd, wParam, lParam, fn) \
    ((fn)((UINT)(wParam), xt2CPoint((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam))), 0L)

/* void Cls_OnLButtonDown(UINT keyFlags, CPoint cpt) */
#define HANDLE_WM_LBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
    ((fn)((UINT)(wParam), xt2CPoint((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam))), 0L)

/* void Cls_OnLButtonUp(UINT keyFlags, CPoint cpt) */
#define HANDLE_WM_LBUTTONUP(hwnd, wParam, lParam, fn) \
    ((fn)((UINT)(wParam), xt2CPoint((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam))), 0L)

/* void Cls_OnRButtonDown(UINT keyFlags, CPoint cpt) */
#define HANDLE_WM_RBUTTONDOWN(hwnd, wParam, lParam, fn) \
    ((fn)((UINT)(wParam), xt2CPoint((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam))), 0L)

/* void Cls_OnRButtonDown(UINT keyFlags, CPoint cpt) */
#define HANDLE_WM_RBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
    ((fn)((UINT)(wParam), xt2CPoint((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam))), 0L)

/* void Cls_OnRButtonUp(UINT keyFlags, CPoint cpt) */
#define HANDLE_WM_RBUTTONUP(hwnd, wParam, lParam, fn) \
    ((fn)((UINT)(wParam), xt2CPoint((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam))), 0L)



/* void Cls_OnTimer(HWND hwnd, UINT id) */
#define HANDLE_WM_TIMER(hwnd, wParam, lParam, fn) \
    ((fn)((UINT)(wParam)), 0L)


// void OnSoundEvent(int index, int code) // code: 1 for done, -1 for error (for error, index indicates the error code)
#define HANDLE_WM__SOUND_EVENT(hwnd, wParam, lParam, fn) \
    ((fn)((int)lParam, (int)wParam), 0L)