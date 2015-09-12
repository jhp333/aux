#include "Echo.h"
#include "EchoMainWnd.h"

CWndDlg wnd;
CEchoMainWnd MainAppWnd;

BOOL CALLBACK  echoMainProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	switch (umsg)
	{
	chHANDLE_DLGMSG (hDlg, WM_INITDIALOG, wnd.OnInitDialog);
	chHANDLE_DLGMSG (hDlg, WM_TIMER, MainAppWnd.OnTimer);
	chHANDLE_DLGMSG (hDlg, WM_CTLCOLORSTATIC, MainAppWnd.OnCtlColorStatic);
	chHANDLE_DLGMSG (hDlg, WM_COMMAND, MainAppWnd.OnCommand);
	chHANDLE_DLGMSG (hDlg, WM_SIZE, MainAppWnd.OnSize);
	chHANDLE_DLGMSG (hDlg, WM_CLOSE, MainAppWnd.OnClose);
	chHANDLE_DLGMSG (hDlg, WM__OPEN_SESSION, MainAppWnd.NewSession);
	chHANDLE_DLGMSG (hDlg, WM__SESSION_DONE, MainAppWnd.SessionDonePrint);
	chHANDLE_DLGMSG (hDlg, WM__ORG_SESS_SCREEN, MainAppWnd.DisplaySession);
	chHANDLE_DLGMSG (hDlg, WM__TOKENS, MainAppWnd.Tokens);
	chHANDLE_DLGMSG (hDlg, WM_FL_ARRIVED, MainAppWnd.OnFlyArrived);
	chHANDLE_DLGMSG (hDlg, WM_FL_CLOSE, MainAppWnd.OnFlyClosed);
	chHANDLE_DLGMSG (hDlg, WM_FL_CONNECTED, MainAppWnd.OnFlyConnected);

	case WM_FL_NETWORK_ERR:
		MessageBox (hDlg, (char*)wParam, (char*)lParam, MB_OK);
		break;

	case WM__WAV_LIST_READY:
		MainAppWnd.wavListReady = (int)wParam;
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

int  WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
     MSG    msg ;
	 HACCEL hAccel;
	 LONG lg;

	 MainAppWnd.hInst = hInstance;

	 InitCommonControls();

	 wnd.hDlg = CreateDialog (MainAppWnd.hInst, "ECHO_MAIN", NULL, echoMainProc);
	 MainAppWnd.hDlg = wnd.hDlg ;
	 MainAppWnd.OnInitDialog(NULL, NULL);
	 lg = (LONG)LoadIcon (MainAppWnd.hInst, MAKEINTRESOURCE(IDI_ICON1));
	 SetClassLong (MainAppWnd.hDlg, GCL_HICON, lg);
	 ShowWindow(MainAppWnd.hDlg, iCmdShow);
	 hAccel = LoadAccelerators (MainAppWnd.hInst, MAKEINTRESOURCE(IDR_ACCELERATOR1));
	 GetCurrentDirectory (sizeof(MainAppWnd.currentDir), MainAppWnd.currentDir);
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
		MainAppWnd.MessageBox("MFC failed to initialize!");
	 while (GetMessage (&msg, NULL, 0, 0))
	 {
		 if ( MainAppWnd.hResult->hDlg==NULL || MainAppWnd.hMissingDlg==NULL || MainAppWnd.hPipeMsgViewDlg==NULL || MainAppWnd.batch->hDlg==NULL ||
			 ! IsDialogMessage(MainAppWnd.hMissingDlg, &msg) || 
			 ! IsDialogMessage(MainAppWnd.hPipeMsgViewDlg, &msg) || ! IsDialogMessage(MainAppWnd.batch->hDlg, &msg) )
		 {
			 if (!TranslateAccelerator (MainAppWnd.hDlg, hAccel, &msg) && !IsDialogMessage(MainAppWnd.hDlg, &msg) )
			 {
				 TranslateMessage (&msg) ;
				 DispatchMessage (&msg) ;
			 }
		 }
	 }
     return msg.wParam ;
}