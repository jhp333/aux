#include "echotouch.h"

#define WM__INIT_DISPLAY	WM_USER+110

HMODULE hDllModule;
WNDPROC hOrgProc;
HINSTANCE hInst;
int buttonEnabled, nConc, practiceMode, fontsize[3];
int feedBackOn, fSingleResponse, fAllowSame, fdBackOnNow, SessionProgress;
int answer[MAX_NUM_STIM];
UCHAR correctAns[MAX_NUM_STIM];

LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

__declspec (dllexport) HANDLE SubClass(HWND hwnd)
{
	hOrgProc = (WNDPROC)SetWindowLong (hwnd, GWL_WNDPROC, (LONG)WndProc);
	PostMessage(hwnd, WM__INIT_DISPLAY, 0, 0);
	return NULL;
}

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	hDllModule = (HMODULE)hModule;
	return TRUE;     
}

void Arrangebutton (int width, int height, int nTokens, HWND* hButton, int sizeY, double angle)
{
	double pi = 3.1415926535;
	int i;

	for (i=0; i<nTokens; i++)
	{
		MoveWindow (hButton[i], CENTERPTX-(int)(width*sin(i*2.*pi/(double)nTokens+angle*pi)), 
			CENTERPTY + (int) (height * cos(i*2.*pi/(double)nTokens+angle*pi)),
			sizeY*3/2, sizeY, TRUE);
		ShowWindow (hButton[i], SW_SHOW);
	}
}

void SetButtonText (HWND hbuttonDlg, char *tokenStr, char *tokenExample)
{
	SetDlgItemText (hbuttonDlg, IDC_TOKEN, tokenStr);
	SetDlgItemText (hbuttonDlg, IDC_EXAMPLE, tokenExample);
}

int AreTheseRightResponse(const char *buf, char **token, int nTokens, HWND* hbutton)
{
	char *bufHolder[4], buf0[128], buf1[128], buf2[128], buf3[128];
	bool loop;
	int i, j;
	bufHolder[0] = buf0; bufHolder[1] = buf1; bufHolder[2] = buf2; bufHolder[3] = buf3;
	int nTypedItems = str2strarray(bufHolder, 4, buf,  " \r\n");
	vector<int> sentAns(nConc);

	if (nTypedItems!=nConc) return 0;

	for (j=0; j<nConc; j++)
	{
		loop = true;
		for (i=0; i<nTokens && loop; i++)
		{
			if (!stricmp(bufHolder[j], token[i])) 
			{
				loop = false;
				sentAns[j] = i;
			}
		}
		if (loop)
			return 0;
	}
	//check if the same response is included
	if (!fAllowSame)
		for (j=0; j<nConc; j++)
			for (i=j+1; i<nConc; i++)
			{
				if (!strcmp(bufHolder[j], bufHolder[i]))
					return 0;
			}
	for (j=0; j<nConc; j++)
	{
		SendMessage(hbutton[sentAns[j]], WM_LBUTTONDOWN, 0, 0);
		SendMessage(hbutton[sentAns[j]], WM_LBUTTONUP, 0, 0);
	}
	return 1;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static char *token[24], *tokenExample[24], *temp[24], pracInst1[256], pracInst2[256];
	static HFONT hFont1, hFont2;
	static char IPaddress[20], verStr[20], instruction1[256], instruction2[256], beginInst[256], endInst[256] ;
	static int whichBitmap, nTrials, nTokens, curTok, cumCounter, trial;
	static HWND hAnsEdit, hInstruction, hLiveChat, hStart, hProgress, hDonePrac, hbutton[24];
	static RECT rcl;


	WNDPROC hh;
	char buf[2048]; 
	int sizex, sizey, i, id, idc;
	WORD command, event;
	DWORD dw;
	HDC hdc;
	COLORREF col;

	RECT rt;
	WINDOWPLACEMENT rp;

	switch (uMsg)
	{
	case WM__INIT_DISPLAY:
		InitCommonControls();
		for (i=0; i<MAX_NUM_STIM; i++)
			correctAns[i] = -1;
		memset((void*)fontsize, 0, 3*sizeof(int));
		sscanfINI(buf, INI_FILE, "FONT_SIZE_TOKEN", "%d", &fontsize[0]);
		sscanfINI(buf, INI_FILE, "FONT_SIZE_TOKEN_CORR", "%d", &fontsize[1]);
		sscanfINI(buf, INI_FILE, "FONT_SIZE_TOKEN_EXAMPLE", "%d", &fontsize[2]);
		for (i=0; i<24; i++) // maximum number of tokens (phonemes): 24
		{
			hbutton[i] = CreateDialog (hDllModule, MAKEINTRESOURCE(IDD_BUTTON1), hwnd, buttonProc);
			SetWindowLong (hbutton[i], GWL_ID, (LONG)i);
			ShowWindow (hbutton[i], SW_HIDE);
			token[i] = (char*) calloc (24, sizeof(char));
			tokenExample[i] = (char*) calloc (24, sizeof(char));
			temp[i] = (char*) calloc (24, sizeof(char));
		}
		hStart = CreateWindow ("button", "START", WS_CHILD, 900, 60, 100, 50, hwnd, (HMENU)IDC_START, hInst, NULL);
		hDonePrac = CreateWindow ("button", "Done with Practice", WS_CHILD, 850, 670, 150, 40, hwnd, (HMENU)IDC_DONE, hInst, NULL);
		hInstruction = CreateWindow ("static", "", WS_CHILD|WS_VISIBLE, 60, 680, 700, 20, hwnd, (HMENU)IDC_INSTRUCTION, hInst, NULL);
		hLiveChat = CreateWindow ("static", "", WS_CHILD|WS_VISIBLE, 60, 30, 870, 30, hwnd, (HMENU)IDC_LIVETEXT, hInst, NULL);
		hProgress = CreateWindowEx (0, PROGRESS_CLASS, "", WS_CHILD|WS_BORDER, 
			PROGRESSBAR_LEFT, PROGRESSBAR_TOP, PROGRESSBAR_WIDTH, PROGRESSBAR_HEIGHT, hwnd,
			(HMENU)IDC_PROGRESS, hInst, NULL);
		hAnsEdit = CreateWindow ("edit", "", WS_CHILD|WS_VISIBLE|ES_MULTILINE|WS_BORDER|ES_WANTRETURN, 800, 630, 180, 40, hwnd, (HMENU)IDC_ANS_EDIT, hInst, NULL);
		hFont1 = CreateFont (20, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_ROMAN,
			"Arial");
		hFont2 = CreateFont (30, 0, 0, 0, FW_NORMAL, 1, 0, 0, ANSI_CHARSET, 
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_ROMAN,
			"Arial");
		buttonEnabled = 0;
		SessionProgress = 0;
		practiceMode = 0;
		break;

	case WM_CTLCOLORSTATIC:
		hdc = (HDC)wParam;
		col = GetSysColor (COLOR_WINDOW);
		id = (int)GetWindowLong ((HWND)lParam, GWL_ID);
		switch (id)
		{
		case IDC_INSTRUCTION:
			SelectObject (hdc, hFont1);
			SetTextColor (hdc, RGB(0, 30, 200));
			SetBkColor (hdc, col);
			return (LRESULT) CreateSolidBrush (col);
		case IDC_LIVETEXT:
		case IDC_CORRECT:
			SelectObject (hdc, hFont2);
			SetTextColor (hdc, RGB(128, 128, 64));
			SetBkColor (hdc, col);
			return (LRESULT) CreateSolidBrush (col);
		}
		break;

	case WM_SIZE:
		sizex = GetSystemMetrics (SM_CXSCREEN);
		sizey = GetSystemMetrics (SM_CYSCREEN)-10;
		MoveWindow (hwnd, 0, 0, sizex, sizey, TRUE);
		break;

	case WM__REDRAW_BUTTONS:
		for (i=0; i<nTokens; i++)
			InvalidateRect(hbutton[i], NULL, TRUE);
		break;

	case WM__DISABLE_ALL:
		for (i=0; i<24; i++)
			EnableWindow (hbutton[i], FALSE);
		break;

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		PostMessage(hbutton[0], uMsg, wParam, lParam);
		break;

	case WM_FL_ARRIVED:
		command = LOWORD(wParam);
		switch (command)
		{
		case FL_INIT_FEEDBACK:
			memcpy ((void*)&feedBackOn, (void*)lParam, HIWORD(wParam));
			break;		
		case FL_INIT_SINGLE_RESPONSE:
			memcpy ((void*)&fSingleResponse, (void*)lParam, HIWORD(wParam));
			break;		
		case FL_INIT_ALLOW_SAME:
			memcpy ((void*)&fAllowSame, (void*)lParam, HIWORD(wParam));
			break;		
		case FL_INIT_NUMBER_STIM:
			memcpy ((void*)&nConc, (void*)lParam, HIWORD(wParam));
			break;
		case FL_TOTAL_CURRENT_TRIALS:
			memcpy ((void*)&dw, (void*)lParam, HIWORD(wParam));
			nTrials = LOWORD(dw);
			trial = HIWORD(dw);
			for (i=0; i<24; i++)
			{
				strcpy(tokenExample[i], "");
				strcpy(token[i], "");
			}
			ShowWindow (hProgress, SW_SHOW);
			SendMessage (hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, nTrials));
			SendMessage (hProgress, PBM_SETSTEP, 1, 0);
			SendMessage (hProgress, PBM_SETPOS, trial, 0);
			if (trial>0)
			{
				buttonEnabled = 1;
				for (i=0; i<24; i++)
					EnableWindow (hbutton[i], TRUE);
				SessionProgress = 1;
			}
			
			break;
		case FL_INIT_NUMBER_TOKENS:
			memcpy ((void*)&nTokens, (void*)lParam, HIWORD(wParam));
			curTok = cumCounter = 0;
			if (nTokens<=8)
				Arrangebutton(400,200, nTokens, hbutton, 100, 0);
			else if (nTokens<13)
			{
				Arrangebutton (400, 250, 8, hbutton, 100, 0);
				if (nTokens!=9)
					Arrangebutton (150, 120, nTokens-8, &hbutton[8], 100, .5);
				else
					Arrangebutton (50, 20, nTokens-8, &hbutton[8], 100, 0);
			}
			else if (nTokens < 17)
			{
				Arrangebutton (400, 250, nTokens-4, hbutton, 90, 0);
				Arrangebutton (150, 120, 4, &hbutton[nTokens-4], 90, 0);
			}
			else if (nTokens < 19)
			{
				Arrangebutton (400, 250, 12, hbutton, 80, 0);
				Arrangebutton (200, 120, nTokens-12, &hbutton[12], 80, 0);
			}
			else
			{
				Arrangebutton (400, 250, 14, hbutton, 80, 0);
				Arrangebutton (200, 120, nTokens-15, &hbutton[14], 80, 0);
				Arrangebutton (0, 0, 1, &hbutton[nTokens-1], 80, 0);
			}
			GetWindowRect (hbutton[0], &rt);
			rp.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement (hbutton[0], &rp);
			break;
		case FL_INIT_TOKEN_STRING:
			strcpy(buf, (char*)lParam);
			buf[HIWORD(wParam)]=0; // necessary to get rid of garbage when flooded
			i=str2strarray (temp, 8, buf, " ");
			if (i>1)
				strcpy(tokenExample[curTok],strstr(buf,temp[1]));
			strcpy(token[curTok],temp[0]);
			SetButtonText (hbutton[curTok], token[curTok], tokenExample[curTok]);
			curTok++;
			break;

		case FL_INIT_SESS:
			ShowWindow (hStart, SW_SHOW);
			ShowWindow (hAnsEdit, SW_SHOW);
			SetWindowText (hInstruction, (char*)lParam);
			ShowWindow (hInstruction, SW_SHOW);
			practiceMode = 0;
			break;

		case FL_INIT_PRAC:
			ShowWindow (hStart, SW_HIDE);
			ShowWindow (hDonePrac, SW_SHOW);
			ShowWindow (hInstruction, SW_SHOW);
			ShowWindow (hAnsEdit, SW_SHOW);
			practiceMode = 1;
			SetWindowText (hInstruction, (char*)lParam);
			for (i=0; i<24; i++)
				EnableWindow (hbutton[i], 1);
			break;

		case FL_INSTRUCTION:
			ShowWindow (hInstruction, SW_SHOW);
			SetWindowText (hInstruction, (char*)lParam);
			break;
			
		case FL_PRACTICE_INSTRUCTION:
			switch (cumCounter)
			{
			case 0:
				strcpy(pracInst1,(char*)lParam);
				pracInst1[HIWORD(wParam)]=0; // necessary to get rid of garbage when flooded
				cumCounter++;
				break;
			case 1:
				strcpy(pracInst2,(char*)lParam);
				pracInst2[HIWORD(wParam)]=0; // necessary to get rid of garbage when flooded
				cumCounter=0;
				SetWindowText (hInstruction, pracInst1);
				
				ShowWindow (hDonePrac, SW_SHOW);
				
				break;
			}
			break;

		case FL_ABOUT2PRESENT:
			ShowWindow (hStart, SW_HIDE);
			SetWindowText(hAnsEdit,"");
			SetWindowText (hInstruction, (char*)lParam);
			if (!practiceMode)
			{
				trial++;
				SendMessage (hProgress, PBM_SETPOS, trial, 0);
				for (i=0; i<24; i++)
					EnableWindow (hbutton[i], FALSE);
			}
			break;
		case FL_ABOUT2STOP:
			SetWindowText (hInstruction, (char*)lParam);
			if (!practiceMode)
			{
				buttonEnabled = 1;
				for (i=0; i<24; i++)
					EnableWindow (hbutton[i], TRUE);
			}
			break;

		case FL_CORRECT_ANS:
			memcpy((void*)correctAns, (void*)lParam, sizeof(correctAns[0])*nConc);
			break;

		case FL_DONE:
			SetWindowText (hInstruction, (char*)lParam);
			buttonEnabled = 0;
			for (i=0; i<nTokens; i++)
			{
				ShowWindow (hbutton[i], SW_HIDE);
				SetWindowText (hbutton[i], "");
				MoveWindow (hbutton[i], 0, 0, 0, 0, FALSE);
			}
			ShowWindow (hAnsEdit, SW_HIDE);
			SessionProgress = 0;
			for (i=0; i<nTokens; i++)
				InvalidateRect(hbutton[i], NULL, TRUE);
			ShowWindow (hProgress, SW_HIDE);
			break;

		case FL_TEXT:
			memcpy((void*)buf, (char*)lParam, HIWORD(wParam));
			buf[HIWORD(wParam)]=0; // necessary to get rid of garbage when flooded
			SetWindowText (hLiveChat, buf);
			break;

		case FL_SESSION_STOP:
			for (i=0; i<nTokens; i++)
			{
				ShowWindow (hbutton[i], SW_HIDE);
				SetWindowText (hbutton[i], "");
				MoveWindow (hbutton[i], 0, 0, 0, 0, FALSE);
			}
			ShowWindow (hStart, SW_HIDE);
			ShowWindow (hDonePrac, SW_HIDE);
			ShowWindow (hProgress, SW_HIDE);
			ShowWindow (hAnsEdit, SW_HIDE);

			SetWindowText (hInstruction, "");
			buttonEnabled = 0;
			SessionProgress = 0;
			for (i=0; i<nTokens; i++)
				InvalidateRect(hbutton[i], NULL, TRUE);
			break;
		}
		break;

	case WM_FL_NETWORK_ERR:
//		MessageBox (hwnd, (char*)wParam, (char*)lParam, MB_OK);
		break;

	case WM__FEEDBACKDISPLAY:
		for (i=0; i<nConc; i++)
		{
			InvalidateRect(hbutton[correctAns[i]], NULL, TRUE);		UpdateWindow (hbutton[correctAns[i]]);
			SetTimer (hbutton[correctAns[i]], TURNOFF_FEEDBACK, FEEDBACK_DURATION, NULL);
		}
		// When feedback is on, the app sendflies after the feedback displays is turned off.
		// (When feedback is off, the app sendflies 100 ms after after the last button is pressed.)
		SetTimer (hwnd, SEND_AFTER_FEEDBACK_OFF, FEEDBACK_DURATION, NULL);
		break;
		
	case WM_FL_CLOSE:
	case WM_CLOSE:
		SendMessage (hwnd, WM_FL_ARRIVED, FL_SESSION_STOP, 0);
		for (i=0; i<24; i++)
		{
			free(token[i]);
			free(tokenExample[i]);
			free(temp[i]);
			DestroyWindow(hbutton[i]);
		}
		DestroyWindow(hInstruction);
		DestroyWindow(hLiveChat);
		DestroyWindow(hStart); 
		DestroyWindow(hProgress); 
		DestroyWindow(hDonePrac);
		DestroyWindow(hAnsEdit);
		DeleteObject(hFont1);
		DeleteObject(hFont2);
		hh = (WNDPROC)SetWindowLong (hwnd, GWL_WNDPROC, (LONG)hOrgProc);
		PostMessage(hwnd, uMsg, wParam, lParam);	// This will be handled by hOrgProc later and current dll (this code) will be unloaded there.
		return 0;	// Must return here. Otherwise, the CallWindowProc() below will unload current dll and it causes a crash.(This code will be gone when CallWindowProc() returns.)

	case WM_TIMER:
		KillTimer (hwnd, wParam);
		switch LOWORD(wParam)
		{
		case SEND_AFTER_FEEDBACK_OFF:
			FER(i=flySendBin (FL_RESPONSE, answer, sizeof(answer)));
			break;
		}
		break;

	case WM_COMMAND:
		event = HIWORD(wParam);
 		idc = LOWORD(wParam);
		switch (idc)
		{
		case IDC_ANS_EDIT:
			if (event==EN_CHANGE)
			{
				GetWindowText(hAnsEdit, buf, sizeof(buf));
				int len = strlen(buf);
				if (len>1) {
					if (buf[len-1]=='\n')	{
						if (AreTheseRightResponse(buf, token, nTokens, hbutton)!=1)
							SendMessage(hAnsEdit, (UINT)EM_SETSEL, 0, (LPARAM)-1);
					}
				}
			}
			break;

		case IDC_START:
			buttonEnabled = 1;
			FER((i=flySendBin (FL_STARTSESSION, &idc, sizeof(idc))));
			SessionProgress = 1;
			for (i=0; i<nTokens; i++)
				InvalidateRect(hbutton[i], NULL, TRUE);
			break;

		case IDC_DONE:
			ShowWindow (hDonePrac, SW_HIDE);
			SendMessage(hwnd, WM_FL_ARRIVED, FL_DONE, 0);
			FER(flySendText (FL_DONE, "done practice"));
			practiceMode = 0;
			break;

		default:
			break;
		}
		break;
	}
    return CallWindowProc(hOrgProc, hwnd, uMsg, wParam, lParam);
}
