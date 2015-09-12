#include "Echo.h"
#include "EchoMainWnd.h"

void CEchoMainWnd::CreateTokensNShow()
{
	int i;
	char buf[64];

	for (i=IDS_TOKEN1; i<=IDS_TOKEN22; i+=2)
	{
		LoadString (hInst, i, buf, sizeof(buf));
		CreateWindow ("button", buf, BS_AUTOCHECKBOX | WS_CHILD, 663, 6+10*i, 85, 15,
			hDlg, (HMENU)(IDC_TOKEN+i), hInst, NULL);
		LoadString (hInst, i+1, buf, sizeof(buf));
		CreateWindow ("button", buf, BS_AUTOCHECKBOX | WS_CHILD, 757, 6+10*i, 85, 15, 
			hDlg, (HMENU)(IDC_TOKEN+i+1), hInst, NULL);
	}
	if (SendDlgItemMessage (IDC_CONS, BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		for (i=IDS_TOKEN1; i<=IDS_TOKEN22; i++)
			ShowWindow (GetDlgItem(IDC_TOKEN+i), SW_SHOW); 
		ShowWindow (GetDlgItem(IDC_GENERAL_INST1), SW_HIDE); 
		ShowWindow (GetDlgItem(IDC_GENERAL_INST2), SW_HIDE); 
		ShowWindow (GetDlgItem(IDC_GENERAL_LIST), SW_HIDE); 
	}
}

void CEchoMainWnd::CreateMyTooltip (HWND hWnd2Display)
{
    HWND hwndTT; 
    TOOLINFO ti;// struct specifying info about tool in tooltip control
    unsigned int uid = 0;       
    char helpStr[2048];


    RECT rect;                  // for client area coordinates

	LoadString (hInst, IDS_HELPSTRING, helpStr, sizeof(helpStr));

    hwndTT = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,        
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        hWnd2Display, NULL, hInst, NULL);

    SetWindowPos(hwndTT, HWND_TOPMOST, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    /* GET COORDINATES OF THE MAIN CLIENT AREA */
    GetClientRect (hWnd2Display, &rect);
	
    /* INITIALIZE MEMBERS OF THE TOOLINFO STRUCTURE */
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS;
    ti.hwnd = hWnd2Display;
    ti.hinst = hInst;
    ti.uId = uid;
    ti.lpszText = helpStr;
    ti.rect.left = rect.left;    
    ti.rect.top = rect.top;
    ti.rect.right = rect.right;
    ti.rect.bottom = rect.bottom;
    
    /* SEND AN ADDTOOL MESSAGE TO THE TOOLTIP CONTROL WINDOW */
	::SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);	
	::SendMessage(hwndTT, TTM_SETMAXTIPWIDTH, 0, 300);
} 