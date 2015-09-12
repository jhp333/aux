#include "auxgv.h"
#include "auxgvdlg.h"
#include <stdlib.h>
#include <process.h>

HINSTANCE hInst;
HWND hAuxDlg;
char ApplicationPath[MAX_PATH];

CWndDlg wnd;
CAuxDlg auxMain;



BOOL CALLBACK auxDlg (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	switch (umsg)
	{
	chHANDLE_DLGMSG (hDlg, WM_INITDIALOG, wnd.OnInitDialog);
	chHANDLE_DLGMSG (hDlg, WM_SIZE, auxMain.OnSize);
	chHANDLE_DLGMSG (hDlg, WM_CLOSE, auxMain.OnClose);
	chHANDLE_DLGMSG (hDlg, WM_DESTROY, auxMain.OnDestroy);
	chHANDLE_DLGMSG (hDlg, WM_COMMAND, auxMain.OnCommand);
	chHANDLE_DLGMSG (hDlg, WM_CTLCOLORSTATIC, auxMain.OnCtlColorStatic);
	chHANDLE_DLGMSG (hDlg, WM_CTLCOLOREDIT, auxMain.OnCtlColorEdit);
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

	auxMain.hInst = hInst = hInstance;

	wnd.hDlg = CreateDialog (hInst, MAKEINTRESOURCE(IDD_AUXGV_DIALOG), NULL, auxDlg);
	hAuxDlg = wnd.hDlg;

	HANDLE h = LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, 0);
	SetClassLongPtr (hAuxDlg, GCL_HICON, (LONG)(LONG_PTR)h);
	auxMain.hDlg = wnd.hDlg ;
	strcpy(auxMain.AppPath, wnd.AppPath);
	auxMain.OnInitDialog(NULL, NULL);
	ShowWindow(auxMain.hDlg, iCmdShow);
	UpdateWindow (auxMain.hDlg);
	hAccel = LoadAccelerators (auxMain.hInst, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	while (GetMessage (&msg, NULL, 0, 0))
	{ 
		if (!TranslateAccelerator (auxMain.hDlg, hAccel, &msg) )
			if (!IsDialogMessage(auxMain.hDlg, &msg))
				{
					TranslateMessage (&msg) ;
					DispatchMessage (&msg) ;
				}
	}
	return (int)msg.wParam ;
}


