#include "token.h"
#include "tokendlg.h"
#include <stdlib.h>
#include <process.h>
#include "msgCrack_seacrm.h"

HINSTANCE hInst;
unsigned int streamThreadID;
bool EmulMode(false);
HWND hPipeLog;
HWND hMainDlg;
char ApplicationPath[MAX_PATH];

CWndDlg wnd;
CTokenDlg tokenMain;

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


BOOL CALLBACK tokenDlg (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	switch (umsg)
	{
	chHANDLE_DLGMSG (hDlg, WM_INITDIALOG, tokenMain.OnInitDialog);
	chHANDLE_DLGMSG (hDlg, WM_SIZE, tokenMain.OnSize);
	chHANDLE_DLGMSG (hDlg, WM_CLOSE, tokenMain.OnClose);
	chHANDLE_DLGMSG (hDlg, WM_DESTROY, tokenMain.OnDestroy);
	chHANDLE_DLGMSG (hDlg, WM_COMMAND, tokenMain.OnCommand);
	chHANDLE_DLGMSG (hDlg, WM_FL_ARRIVED, tokenMain.OnFlyArrived);
	chHANDLE_DLGMSG (hDlg, WM_FL_CLOSE, tokenMain.OnFlyClosed);
	chHANDLE_DLGMSG (hDlg, WM_FL_CONNECTED, tokenMain.OnFlyConnected);
	chHANDLE_DLGMSG (hDlg, WM_CTLCOLORSTATIC, tokenMain.OnCtlColorStatic);
	default:
		return FALSE;
	}
	return TRUE;
}

int  WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
	MSG         msg ;
	HACCEL hAccel;

	tokenMain.hInst = hInst = hInstance;
//	InitCommonControls();

	hMainDlg = wnd.hDlg = CreateDialog (hInst, MAKEINTRESOURCE(IDD_TAG_DIALOG), NULL, tokenDlg);
	HANDLE h = LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, 0);
	SetClassLongPtr (hMainDlg, GCL_HICON, (LONG)(LONG_PTR)h);
	tokenMain.hDlg = wnd.hDlg ;
	tokenMain.OnInitDialog2(NULL, NULL);
	ShowWindow(tokenMain.hDlg, iCmdShow);
	UpdateWindow (tokenMain.hDlg);
	hAccel = LoadAccelerators (hInst, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	while (GetMessage (&msg, NULL, 0, 0))
	{ 
		if ( tokenMain.hPipeMsgViewDlg==NULL || !IsDialogMessage (tokenMain.hPipeMsgViewDlg, &msg) )
		{
			 if (!TranslateAccelerator (tokenMain.hDlg, hAccel, &msg) && !IsDialogMessage(tokenMain.hDlg, &msg))
			 {
				 TranslateMessage (&msg) ;
				 DispatchMessage (&msg) ;
			 }
		}
	}
	return (int)msg.wParam ;
}


