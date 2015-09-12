#include "console.h"
#include "WndDlg.h"
#include "AceconsolWnd.h"
#include "msgCrack.h"


CWndDlg wnd;
CAceconsolWnd MainAppWnd;



BOOL CALLBACK  aceconsolMainProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	switch (umsg)
	{
	chHANDLE_DLGMSG (hDlg, WM_INITDIALOG, wnd.OnInitDialog);
	chHANDLE_DLGMSG (hDlg, WM_COMMAND, MainAppWnd.OnCommand);
	chHANDLE_DLGMSG (hDlg, WM_FL_ARRIVED, MainAppWnd.OnFlyArrived);
	chHANDLE_DLGMSG (hDlg, WM_FL_CLOSE, MainAppWnd.OnFlyClosed);
	chHANDLE_DLGMSG (hDlg, WM_FL_CONNECTED, MainAppWnd	.OnFlyConnected);
	default:
		return FALSE;
	}
	return TRUE;
}

int  WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
     MSG    msg ;
	 CWndDlg wnd2;
	 HANDLE h = LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, 0);
	 SetClassLongPtr (NULL, GCL_HICON, (LONG)(LONG_PTR)h);
	 wnd.hInst = hInstance;

	 wnd.hDlg = CreateDialog (hInstance, MAKEINTRESOURCE(IDD_CONSOLE), NULL, aceconsolMainProc);
	 MainAppWnd.hDlg = wnd.hDlg ;
	 MainAppWnd.OnInitDialog(NULL, NULL);
	 ShowWindow(MainAppWnd.hDlg, iCmdShow);
	 while (GetMessage (&msg, NULL, 0, 0))
	 {
		 if (!IsDialogMessage(MainAppWnd.hDlg, &msg) )
		 {
			 TranslateMessage (&msg) ;
			 DispatchMessage (&msg) ;
		 }
	 }
     return (int)msg.wParam ;
}

