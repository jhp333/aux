#include "echotouch.h"

#define SET_TOKEN_STRING_DEFAULT_POS MoveWindow (hTokenStr, 0, sizeY/9, rt.right-rt.left, sizeY, FALSE);

extern int answer[];

int IsThiscorrectAnswer(int id, UCHAR *lastAns, int nConc)
{
	int i, x;
	x=1;
	for (i=0; i<nConc; i++)
		x *= id - (int)lastAns[i];
	return (x==0);
}

int DoesSameOneRepeat(int *x, int len)
{
	for (int i=0; i<len; i++)
		for (int j=i+1; j<len; j++)
			if (x[i]==x[j])
			return 1;
	return 0;
}

BOOL CALLBACK buttonProc (HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HFONT hFont1, hFont2, hFont3;
	static int sizeY, Clicked, lastClicked, nResponse;
	static HBRUSH hb_DlgBk, hb_DlgDisabled;
	static DWORD  dlgColor;
//	static char TokenStr[16];
	static HWND hpar;

	WORD id, DlgID;
	HDC hdc;
	RECT rt;
	int i;
	HWND h, hTokenStr, hExampleStr;
	PAINTSTRUCT ps;
	char tokenStr[64];

	// buttonEnabled is never used in a practice mode.

	switch (iMsg)
	{
	case WM_INITDIALOG:
		Clicked = -1;
		if (fontsize[0]==0) fontsize[0] = 45;
		if (fontsize[1]==0) fontsize[1] = 55;
		if (fontsize[2]==0) fontsize[2] = 23;
		hFont1 = CreateFont (fontsize[0], 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_ROMAN,
			"Arial");
		hFont3 = CreateFont (fontsize[1], 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_ROMAN,
			"Arial");
		hFont2 = CreateFont (fontsize[2], 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_ROMAN,
			"Times New Roman");
		hb_DlgBk = CreateSolidBrush (DIALOG_COLOR);
		hb_DlgDisabled = CreateSolidBrush (DIALOG_COLOR_DISALBED);
		fdBackOnNow = nResponse = 0;
		hpar = GetParent(hDlg);
		GetWindowRect (hDlg, &rt);
		sizeY = rt.bottom-rt.top;
		// Setting the range of token string 5/6 of height of the whole button from the top
		hTokenStr = GetDlgItem (hDlg, IDC_TOKEN);
		GetWindowRect (hTokenStr, &rt);
		SET_TOKEN_STRING_DEFAULT_POS
		
		// Setting the range of token string 1/6 of height of the whole button from the bottom
		hExampleStr = GetDlgItem (hDlg, IDC_EXAMPLE);
		GetWindowRect (hExampleStr, &rt);
		MoveWindow (hExampleStr, 0, 3*sizeY/5, rt.right-rt.left, sizeY, FALSE);		
		break;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		if (practiceMode)
		{
			GetDlgItemText (hDlg, IDC_TOKEN, tokenStr, 64);
			FER(flySendText (FL_PLAYTHIS, tokenStr));
			SendMessage (hpar, WM__SET_PRACTICE_INST, 0, 0);
		}
		else if (buttonEnabled && Clicked ==-1)
		{
			Clicked = (int)GetWindowLong (hDlg, GWL_ID);
			InvalidateRect(hDlg, NULL, TRUE);			UpdateWindow (hDlg);
		}
		break;

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		if (buttonEnabled && Clicked != -1)
		{  // Note: this line doesn't sendfly to remote immediately. Only accumulates "output buffer" and 
			// actually sends it when nConc clicks are acumulated.

			answer[nResponse++] = Clicked;
			Clicked = -1;
			if (fAllowSame!=1 && DoesSameOneRepeat(answer,nResponse))
			{
				SendMessage(GetParent(hDlg), WM__REDRAW_BUTTONS, 0, 0);
				nResponse=0;
			}
			if (nResponse>=nConc || fSingleResponse)
			{
				SendMessage(GetParent(hDlg), WM__REDRAW_BUTTONS, 0, 0);
				nResponse=0;
				buttonEnabled = 0;
				SetTimer (hDlg, CLICKED_RELEASE, 100, 0);
			}
		}
		break;

	case WM_CTLCOLORSTATIC:
		hExampleStr = GetDlgItem (hDlg, IDC_EXAMPLE);
		hTokenStr = GetDlgItem (hDlg, IDC_TOKEN);
		GetWindowRect (hTokenStr, &rt);
		SET_TOKEN_STRING_DEFAULT_POS

		hdc = (HDC)wParam;
		DlgID = (WORD)GetWindowLong (GetParent((HWND)lParam), GWL_ID);
		id = (WORD)GetWindowLong ((HWND)lParam, GWL_ID);
		strcpy(tokenStr, "");
		if (SessionProgress)
			SetBkColor ((HDC)wParam, DIALOG_COLOR);
		else
			SetBkColor ((HDC)wParam, DIALOG_COLOR_DISALBED);
		switch (id)
		{
		case IDC_TOKEN:
			SelectObject (hdc, hFont1);
			if (DlgID==Clicked)
				SetTextColor (hdc, COLOR_CLICKED);
			else if (IsThiscorrectAnswer(DlgID, correctAns,nConc) && fdBackOnNow)
			{
				SelectObject (hdc, hFont3);
				SetTextColor (hdc, COLOR_FEEDBACK);
				h = GetDlgItem (hDlg, IDC_TOKEN);
				GetWindowRect (h, &rt);
				MoveWindow (hTokenStr, 0, sizeY/8, rt.right-rt.left, 5*sizeY/6, FALSE);
			}
			else
			{
				SetTextColor (hdc, RGB(0, 30, 50));
			}
			break;
		case IDC_EXAMPLE:
			SelectObject (hdc, hFont2);
			SetTextColor (hdc, RGB(100, 80, 64));
			break;
		}
		if (SessionProgress)
			return (BOOL) hb_DlgBk;
		else
			return (BOOL) hb_DlgDisabled;

	case WM_TIMER:
		KillTimer (hDlg, wParam);
		switch LOWORD(wParam)
		{
		case CLICKED_RELEASE:
			InvalidateRect(hDlg, NULL, TRUE);			UpdateWindow (hDlg);
			// If feedback is on,
			// Upon the button losing its focus (100 ms after the actual button release),
			// the button corresponding to correctAns is turned on and 
			// another Settimer is called to turn it off.
			if (feedBackOn) {
				fdBackOnNow=1;
				SendMessage (GetParent(hDlg), WM__FEEDBACKDISPLAY, 0, 0);
			} else
				SetTimer (GetParent(hDlg), SEND_AFTER_FEEDBACK_OFF, 0, 0);

			break;

		case TURNOFF_FEEDBACK:
			for (i=0; i<MAX_NUM_STIM; i++)
				correctAns[i] = -1;
			fdBackOnNow = 0;
			InvalidateRect(hDlg, NULL, TRUE);			UpdateWindow (hDlg);
			break;
		}
		break;
	
	case WM_CTLCOLORDLG :
		dlgColor = DIALOG_COLOR;
		if (SessionProgress)
			return (BOOL)hb_DlgBk ;
		else
			return (BOOL)hb_DlgDisabled;

	case WM_PAINT:
		hdc = BeginPaint (hDlg, &ps);
		EndPaint (hDlg, &ps);
		break;

	default:
		return FALSE;
	}
	return TRUE;
}
