#include "aceplayer.h"
#include "aceplayerdlg.h"
#include <stdlib.h>
#include <process.h>
#include "msgCrack_aceplayer.h"

HINSTANCE hInst;
unsigned int streamThreadID;
HWND hPipeLog, hMATLABMSG, hPulseView;
HWND hMainDlg;

CWndDlg wnd;
CACEplayerDlg aceMain;

BOOL CALLBACK PipeLogProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	switch (umsg)
	{
	case WM_INITDIALOG:
		return 1;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			ShowWindow(hDlg, SW_HIDE);
			break;
		}
		break;
	default:
		return 0;
	}
	return 1;
}


BOOL CALLBACK aceplayerDlg (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	switch (umsg)
	{
	chHANDLE_DLGMSG (hDlg, WM_INITDIALOG, wnd.OnInitDialog);
	chHANDLE_DLGMSG (hDlg, WM_SIZE, aceMain.OnSize);
	chHANDLE_DLGMSG (hDlg, WM_CLOSE, aceMain.OnClose);
	chHANDLE_DLGMSG (hDlg, WM_DESTROY, aceMain.OnDestroy);
	chHANDLE_DLGMSG (hDlg, WM_COMMAND, aceMain.OnCommand);
	chHANDLE_DLGMSG (hDlg, WM_FL_ARRIVED, aceMain.OnFlyArrived);
	chHANDLE_DLGMSG (hDlg, WM_FL_CLOSE, aceMain.OnFlyClosed);
	chHANDLE_DLGMSG (hDlg, WM_FL_CONNECTED, aceMain.OnFlyConnected);
	default:
		return FALSE;
	}
	return TRUE;
}

int  WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
	MSG         msg ;

	hInst = hInstance;
//	InitCommonControls();

	wnd.hDlg = CreateDialog (hInst, MAKEINTRESOURCE(IDD_ACE_PLAYER), NULL, aceplayerDlg);

	hMainDlg = wnd.hDlg;
	aceMain.hDlg = wnd.hDlg ;
	aceMain.OnInitDialog(NULL, NULL);
	ShowWindow(aceMain.hDlg, iCmdShow);
	UpdateWindow (aceMain.hDlg);

	HANDLE h = LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, 0);
	SetClassLongPtr (hMainDlg, GCL_HICON, (LONG)(LONG_PTR)h);

	while (GetMessage (&msg, NULL, 0, 0))
	{ 
		if ( !IsDialogMessage (aceMain.hDlg, &msg) )
		{
			TranslateMessage (&msg) ;
			DispatchMessage (&msg) ;
		}
	}
	return (int)msg.wParam ;
}


