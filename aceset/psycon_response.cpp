#include "psycon_response.h"
#include "fer.h"

void ShowButtons(HWND *hAdj, HWND h1, HWND h2, int showState)
{
	int i;
	for (i=0; i<4; i++)
		ShowWindow (hAdj[i], showState);
	ShowWindow (h1, showState);
	ShowWindow (h2, showState);
}

void EnableButtons(HWND *hAdj, HWND h1, HWND h2, int state)
{
	int i;
	for (i=0; i<4; i++)
		EnableWindow (hAdj[i], state);
	EnableWindow (h1, state);
	EnableWindow (h2, state);
}


WNDPROC OldSliderProc;
HINSTANCE hInst;
int stopped ;

int xPos[]={100, 350, 480, 620,};
int yPos = 350;
int butWidth[] = {180, 70, 70, 180,};
int butHeight[] = {130, 70, 70, 130,};

/* v 1.04 new protocol---FL_INIT2 
all string 
number of anchored intervals, then followed by anchored intervals 
examples:

0 --> no anchor
1 0 --> (the first one anchor)
1 2 --> (the last one anchor)
2 0 3 --> (the first and the last ones are the anchor)
1 --> error
*/



LRESULT CALLBACK WndProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int taskID, adjusted, playedItem, range_max, range_min, needMoreUpDown;
	static HWND hAdjButton[5], /*hSlider, hSliderVal,*/ hPlay, hDone /*hMore*/;
	static char versionstr[16], buf[1024], inst1[256], inst2[256];
	static HWND hbut[MAX_INTERVALS];
	static int ready4Respond, intvl, instID;
	static HBRUSH hBackgrnd;
	static HWND hFreak, hMsg, hFeed, hBegin, hMsgOperator, hCount, hCap;
	static HFONT hFont, hFont2, hFont3, hFont4;
	static HBITMAP hBitmap[14];

	int i, answer, j, id, count, nAnchor, anchor[8];

	WORD command;
	char cody[64], remotePC[256];

	HDC hdc;
	int bltx[]={30, 30, 30, 30, 25, 25,}, 
		 blty[]={25, 50, 50, 25, 25, 25,};
	COLORREF col;
	char AppPath[512], procname[64], verstr[16];

	static bool banchor[8];
	static int sizex, sizey, cintl, marginx, spacing, width, height, marginy, icount;

	switch (uMsg)
	{
	case WM_CREATE:
		hBitmap[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_CL_UP));
		hBitmap[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_CL_UP_SM));
		hBitmap[2] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_CL_DOWN_SM));
		hBitmap[3] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_CL_DOWN));
		hBitmap[4] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_PLAY));
		sizex = GetSystemMetrics (SM_CXSCREEN)-10;
		sizey = GetSystemMetrics (SM_CYSCREEN)-30;
		hMsg=CreateWindow ("Static", "", WS_CHILD|WS_VISIBLE, 100, 80, 800, 100, hwnd,
									(HMENU)ID_HMSG, hInst, NULL);
		hMsgOperator=CreateWindow ("Static", "", WS_CHILD|WS_VISIBLE, 100, 540, 800, 100, hwnd,
									(HMENU)ID_HMSG2, hInst, NULL);
		hFeed=CreateWindow ("Static", "", WS_CHILD, 100, 600, 390, 45, hwnd,
									(HMENU)ID_FEEDBACK, hInst, NULL);
		hCount = CreateWindow ("Static", "", WS_CHILD, 400, 335, 150, 150, hwnd,
									(HMENU)ID_COUNTER, hInst, NULL);
		hCap = CreateWindow ("Static", "Connection: OFF", WS_CHILD|WS_VISIBLE, 
			20, 8, 100, 25, hwnd, (HMENU)ID_CAP, hInst, NULL);
			// Creating the buttons
		hPlay = CreateWindow ("button", "", WS_CHILD | BS_BITMAP | BS_CENTER | BS_VCENTER, xPos[0]+(xPos[3]-xPos[0])/3, 
				yPos+160, butWidth[3]*2/3, butHeight[3]*11/15, hwnd, (HMENU)IDC_PLAY, hInst, NULL);
		SendMessage (hPlay, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap[4]);
		hDone= CreateWindow ("Button", "DONE", WS_CHILD|WS_DISABLED, xPos[2]+(xPos[3]-xPos[0])/3, yPos+160, butWidth[3]*4/9, butHeight[3]*33/60, hwnd,
									(HMENU)IDC_DONE, hInst, NULL);
		for (j=0; j<MAX_INTERVALS; j++)
			hbut[j] = CreateWindow ("Button", itoa(j+1, buf, 10), WS_CHILD|WS_DISABLED, 
				0, 0, 0, 0, hwnd, (HMENU)(BUTT1+j), hInst, NULL);
		hBegin = CreateWindow ("button", "Press to start", WS_CHILD|WS_DISABLED, sizex/3, sizey*3/5, sizex*35/100, sizey*35/100,
			hwnd, (HMENU)ID_HBEGIN, hInst, NULL);
		hFont = CreateFont (25, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_ROMAN,
			"Verdana");
		hFont2 = CreateFont (20, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_ROMAN,
			"Times New Roman");
		hFont3 = CreateFont (20, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_ROMAN,
			"Verdana");
		hFont4 = CreateFont (13, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_ROMAN,
			"Arial");

		hAdjButton[0] = CreateWindow ("button", "", WS_CHILD | BS_BITMAP | BS_CENTER | BS_VCENTER, xPos[0], yPos, butWidth[0], butHeight[0],
			hwnd, (HMENU)IDB_BITMAP_CL_UP, hInst, NULL);
		SendMessage (hAdjButton[0], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap[0]);
		hAdjButton[1] = CreateWindow ("button", "", WS_CHILD | BS_BITMAP | BS_CENTER | BS_VCENTER, xPos[1], yPos, butWidth[1], butHeight[1],
			hwnd, (HMENU)IDB_BITMAP_CL_UP_SM, hInst, NULL);
		SendMessage (hAdjButton[1], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap[1]);
		hAdjButton[2] = CreateWindow ("button", "", WS_CHILD | BS_BITMAP | BS_CENTER | BS_VCENTER, xPos[2], yPos, butWidth[2], butHeight[2],
			hwnd, (HMENU)IDB_BITMAP_CL_DOWN_SM, hInst, NULL);
		SendMessage (hAdjButton[2], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap[2]);
		hAdjButton[3] = CreateWindow ("button", "", WS_CHILD | BS_BITMAP | BS_CENTER | BS_VCENTER, xPos[3], yPos, butWidth[3], butHeight[3],
			hwnd, (HMENU)IDB_BITMAP_CL_DOWN, hInst, NULL);
		SendMessage (hAdjButton[3], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap[3]);
		hBackgrnd = CreateSolidBrush(GetSysColor (COLOR_WINDOW));
		ready4Respond = 0;
		GetCurrentProcInfo(AppPath, procname, verstr);
		getVersionStringAndUpdateTitle(hwnd, procname, verstr);

		i=PickRemotePC(NULL, remotePC, "remotePCs.ini");
		i=initfly (hwnd, remotePC, -1, 0);
		break;

	case WM_SIZE:
		MoveWindow(hwnd, 0, 0, sizex, sizey, 1);
		break;


	case WM_CTLCOLORSTATIC:
		hdc = (HDC)wParam;
		col = GetSysColor (COLOR_WINDOW);
		id = (int)GetWindowLong ((HWND)lParam, GWL_ID);
		switch (id)
		{
		case ID_FEEDBACK:
			SelectObject (hdc, hFont2);
			SetTextColor (hdc, RGB(113, 30, 225));
			SetBkColor (hdc, col);
			break;
		case ID_HMSG:
			SelectObject (hdc, hFont);
			SetTextColor (hdc, RGB(100, 80, 64));
			SetBkColor (hdc, col);
			break;
		case ID_HMSG2:
			SelectObject (hdc, hFont);
			SetTextColor (hdc, RGB(190, 30, 64));
			SetBkColor (hdc, col);
			break;
		case ID_CAP:
			SelectObject (hdc, hFont4 );
			SetBkColor (hdc, col);
			break;
		}
		return (LRESULT) hBackgrnd;

	case WM_FL_CONNECTED:
		SetWindowText (hCap, "Connection: On");
		break;
	case WM_FL_CLOSE:
		SetWindowText (hCap, "Connection: Off");
		for (j=0; j<cintl; j++)
			ShowWindow (hbut[j], SW_HIDE);
		ShowWindow(hBegin, SW_HIDE);
		ShowWindow(hCount, SW_HIDE);
		ShowWindow(hFeed, SW_HIDE); 
		ShowWindow(hMsg, SW_HIDE); 
		ShowWindow(hMsgOperator, SW_HIDE);
		ShowButtons(hAdjButton, hPlay, hDone, SW_HIDE);
		for (j=0; j<5; j++) DeleteObject(hBitmap[j]);

		break;
	case WM_FL_ARRIVED:
		command = LOWORD(wParam);
		switch (command)
		{
		case FL_INIT:
			taskID = 0;
			// Receiving the number of interval from O-PC
			memcpy((void*)&cintl, (void*)lParam, sizeof(int)); 

			// Calculating the width, spacing, etc... of the buttons
			width = 58 * sizex / cintl / 100;
			spacing = 30 * sizex /  cintl / 100;
			marginx = 3 * sizex / cintl / 10;
			height = 2 * sizey / 10;
			marginy = 35 * sizey / 100;

			intvl = stopped = 0; 
			// Arranging the buttons
			for (j=0; j<cintl; j++)
			{
				MoveWindow(hbut[j], marginx+j*(width+spacing), marginy, width, height, 0);
				ShowWindow(hbut[j], SW_SHOW);
			} 
			ShowWindow(hBegin, SW_SHOW);
			SetWindowText (hMsg, "Press 'begin' to start");
			ShowWindow (hMsg, SW_SHOW);
			ShowWindow (hCount, SW_HIDE);
			ShowWindow(hPlay, SW_HIDE);
			icount = 5;
			instID=0;
			memset((void*)banchor, 0, 8);
			break;
		case FL_INIT2:
			if ((count=str2array(anchor, 8, (char*)lParam, " "))<1) 
			{
				MessageBox (hwnd, (char*)lParam, "Invalid incoming string", MB_OK);
				break;
			}
			nAnchor = count; // the number of anchored intervals
			//Check what intervals are anchors
			for (j=0; j<nAnchor; j++) banchor[anchor[j]] = true;
			break;

		case FL_INIT_SLIDER:
			taskID = 2;
			ShowButtons(hAdjButton, hPlay, hDone, SW_SHOW);
			SetWindowText (hMsg, "Press ""PLAY"" button (or ""done"" when it's done)");
			break;
		case FL_ABOUT2PRESENT:
			if (taskID==0)
			{
				ready4Respond = 0; // Just to re-assure
				if (!stopped)
				{
					ShowWindow (hbut[intvl], SW_SHOW);
					if (intvl>0)
						ShowWindow (hbut[intvl-1], SW_HIDE);
				}
			}
			else // slider
			{
				EnableButtons (hAdjButton, hPlay, hDone, 0);
				if (playedItem==0)
					SetWindowText (hMsg, "Listen to SOUND 1...");
				else
					SetWindowText (hMsg, "Listen to SOUND 2...");
			}
			break;
		case FL_ABOUT2STOP:
			if (taskID==0)
			{
				intvl++;
				if (intvl==cintl) 
				{	
					for (i=0; i<cintl; i++) 
					{
						ShowWindow (hbut[i], SW_SHOW); 
						EnableWindow (hbut[i], !banchor[i]);
					}

					intvl = 0; 
					SetWindowText (hMsg, inst2);
					ready4Respond = 1;
				}
			}
			else // slider
			{
				EnableButtons (hAdjButton, NULL, NULL, 1);
				SetWindowText (hMsg, "");
				playedItem++;
			}
			break;

		case FL_END_OF_PRSENTATION:
			EnableWindow (hPlay, TRUE);
			EnableWindow (hDone, TRUE);
			SetWindowText (hMsg, inst1);
			break;

		case FL_TEXT:
			ShowWindow (hMsgOperator, SW_SHOW);
			SetWindowText (hMsgOperator, (char*)lParam);
			break;

		case FL_DONE:
			stopped=1;
			SetWindowText (hMsg, "Session done....");
			ShowWindow(hMsg, SW_SHOW);
			break;

		case FL_FEEDBACK:
			ShowWindow (hFeed, SW_SHOW);
			SetWindowText (hFeed, (char*)lParam);
			SetTimer (hwnd, FDBCKTIMER, 1500, NULL);
			break;
		case FL_STOP:
			PostMessage (hwnd, WM__STOPPED, 0, 1);
			break;
		case FL_RESUME:
			stopped=0;
			if (taskID==0 || taskID==1)
				for (j=0; j<cintl; j++)		ShowWindow (hbut[j], SW_SHOW); 
			else
				ShowButtons(hAdjButton, hPlay, hDone, SW_SHOW);
			break;

		case FL_COUNT:
			ShowWindow (hCount, SW_SHOW);
			SetWindowText (hMsg, "Wait for the next one....");
			SetWindowText (hCount, itoa(icount--, buf, 10));
			break;

		case FL_INSTRUCTION:
			if (instID==0)
			{
				strcpy(inst1, (char*)lParam);
				instID++;
			}
			else
			{
				strcpy(inst2, (char*)lParam);
				EnableWindow (hBegin, TRUE);
				instID=0;
			}
			break;
		}
		break;

	case WM_FL_SENT_OK:
		command = LOWORD(wParam);
		switch (command)
		{
		case FL_BEGIN:
		case FL_RESPONSE:
			for (j=0; j<cintl; j++)
				ShowWindow (hbut[j], SW_HIDE);
			SetWindowText (hMsg, inst1);
			break;
		}
		break ;

	case WM_FL_NETWORK_ERR:
		itoa(LOWORD(wParam), cody, 10);
		MessageBox(hwnd, (char*)lParam, cody, MB_OK);
		break ;

	case WM__STOPPED:
		stopped=1;
		intvl = 0;
		if ((int)lParam==1) // "panic" pressed
			strcpy(buf,"Stop pressed.");
		else // stopped by the operator
			strcpy(buf,"Stopped by the operator");
		SetWindowText (hMsg, buf);
		ShowWindow (hBegin, SW_HIDE); 
		for (j=0; j<cintl; j++)
			ShowWindow (hbut[j], SW_HIDE); 
		ShowButtons(hAdjButton, hPlay, hDone, SW_HIDE);
		break;

	case WM_COMMAND:
 		answer = LOWORD(wParam)-BUTT1;
		switch (LOWORD(wParam))
		{
		case IDC_PLAY:
			playedItem = 0;
			FER(flySendText (FL_PLAY, ""));
			break;
		case IDB_BITMAP_CL_UP:
			FER(flySendText (FL_ADJUST, "+BIG"));
			SendMessage (hwnd, WM_COMMAND, IDC_PLAY, 0);
			break;
		case IDB_BITMAP_CL_UP_SM:
			FER(flySendText (FL_ADJUST, "+SMALL"));
			SendMessage (hwnd, WM_COMMAND, IDC_PLAY, 0);
			break;
		case IDB_BITMAP_CL_DOWN:
			FER(flySendText (FL_ADJUST, "-BIG"));
			SendMessage (hwnd, WM_COMMAND, IDC_PLAY, 0);
			break;
		case IDB_BITMAP_CL_DOWN_SM:
			FER(flySendText (FL_ADJUST, "-SMALL"));
			SendMessage (hwnd, WM_COMMAND, IDC_PLAY, 0);
			break;
		case IDC_DONE:
			ShowButtons(hAdjButton, hPlay, hDone, SW_HIDE);
			SetWindowText (hMsg, "Done. Wait for the next trial.");
			FER(sendfly(FL_DONE, "", 0))
			EnableWindow (hDone, 0);
			needMoreUpDown = 0;
			break;

		case BUTT1:
		case BUTT2:
		case BUTT3:
		case BUTT4:
		case BUTT5:
			if(ready4Respond)
			{
				for (j=0; j<cintl; j++)
				{
					ShowWindow (hbut[j], SW_HIDE); 
					EnableWindow (hbut[j], 0);
				}
				FER(sendflyn (FL_RESPONSE, &answer, sizeof(int), 0))
			}
			ready4Respond = 0;
			break;
	
		case ID_HBEGIN:
			answer=10;
			FER(sendfly (FL_BEGIN, "", 0))
			ShowWindow(hBegin, SW_HIDE);
			SetWindowText (hMsg, inst1);
			SetTimer (hwnd, BEGINDELAY, 500, NULL);
			for (j=0; j<cintl; j++)
			{
				ShowWindow (hbut[j], SW_HIDE); 
				EnableWindow (hbut[j], 0);
			}
			break;

		case ID_RESPONSE_1:
			SendMessage (hwnd, WM_COMMAND, BUTT1, 0);
			break;
		case ID_RESPONSE_2:
			SendMessage (hwnd, WM_COMMAND, BUTT2, 0);
			break;
		case ID_RESPONSE_3:
			SendMessage (hwnd, WM_COMMAND, BUTT3, 0);
			break;
		case ID_RESPONSE_4:
			SendMessage (hwnd, WM_COMMAND, BUTT4, 0);
			break;
		case ID_STOP:
			FER(sendfly(FL_STOP, "", 0))
			SendMessage(hwnd, WM__STOPPED, 0, STOPPED_BY_USER);
			break;

		default:
			MessageBox(hwnd, "UNKNOWN", "ERROR", MB_OK);
			break;
		}
		break ;

	case WM_TIMER:
		switch (wParam)
		{
		case FDBCKTIMER:
			ShowWindow (hFeed, SW_HIDE);
			break;
		}
		break;

	case WM_DESTROY:
		closefly();
		PostQuitMessage(0);
		break;
	}
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

char szAppName[] = "psycon_response";
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
     {
     MSG         msg ;
     HWND        hwnd ;
     HACCEL      hAccel ;
     WNDCLASSEX  wndclass ;

	 hInst = hInstance;

     wndclass.cbSize        = sizeof (wndclass) ;
     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = NULL ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;
     wndclass.hIconSm       = NULL ;

     RegisterClassEx (&wndclass) ;
	 InitCommonControls();

     hwnd = CreateWindow (szAppName, "Response",
                          WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, szCmdLine) ;

     ShowWindow (hwnd, iCmdShow) ;
     UpdateWindow (hwnd) ; 

     hAccel = LoadAccelerators (hInstance, "ACCEL") ;

     while (GetMessage (&msg, NULL, 0, 0))
	 {
		if (!TranslateAccelerator (hwnd, hAccel, &msg))
		{
			TranslateMessage (&msg) ;
			DispatchMessage (&msg) ;
		}
	 }
     return msg.wParam ;
}
