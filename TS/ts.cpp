#include <windowS.h>
#include <stdio.h>
#include <flyplus.h>
#include "resource.h"
#include <audfret.h>

#define IDC_STAT    552
#define IDC_INFO    553

static LPCTSTR lpszAppName = "TouchScreen";
char comLine[128];
char IPA[64];
char dllname[128];
typedef HACCEL (* PFUN) (HWND);

HINSTANCE		hInst;
HACCEL			hAccel;

BOOL CALLBACK DlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	WORD evnt, ctrID; 
	switch (message)
	{
	case WM_COMMAND:
		ctrID = LOWORD(wParam);
		evnt = HIWORD(wParam);
		if (wParam==IDCANCEL || wParam==IDOK)
			ShowWindow(hDlg, SW_HIDE);
		break;
	default:
		return 0;
	}
	return 1;
}


LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HANDLE hFile;
	static HMODULE hMod;
	static HWND hStat, hInfo, hLogDlg;
	static int dlllaunched, FileWrittenDone, sizex, sizey, filesize, cumRecv, whichBitmap;
	static char *cline[2], ipa[64], dllname[512], fname[512], AppPath[1024];
	static HBITMAP hConnectionBall[3];
	static HFONT hFontStat;
	char buf[256], buf1[256], drv[8], dir[512], ext[512];
	int i, id;
	PFUN pf4SubClass;

	DWORD dw, col, bytesRecvd;
	WORD command;
	HDC hdc, hMem;
	PAINTSTRUCT ps;
	char *recvPt;
	switch (message)
	{
	case WM_CLOSE:
		i=0;
		break;

	case WM_NCDESTROY:
		i=IsWindow(hwnd);
		return 0;
	case WM_CREATE:
		recvPt = AppPath;
		GetCurrentProcInfo (AppPath, buf1, buf);
		getVersionStringAndUpdateTitle (hwnd, buf1, buf);
		hdc = GetDC(hwnd);
		sizex = GetSystemMetrics(SM_CXSCREEN);
		sizey = GetSystemMetrics(SM_CYSCREEN);
		if (strlen(comLine)==0)
		{
			dlllaunched=0;
		}
		else
		{
			strcpy(ipa,"");
			strcpy(dllname,"");
			cline[0] = ipa;
			cline[1] = dllname;
			str2strarray (cline,2,comLine," ");
			dlllaunched=1;
		}
		sprintf(buf1, "%s%s", AppPath, "remoteid");
		i=PickRemotePC(NULL, buf, buf1);
		if ((i=initfly (hwnd, buf, -1, false))!=FER_OK) 
		{	sprintf(buf, "code=%d", i);
			MessageBox (hwnd, buf, "", MB_OK);
		}
		hStat = CreateWindow ("STATIC","Waiting for operating application...", WS_CHILD|WS_VISIBLE, 700,5,160,45, hwnd, (HMENU)IDC_STAT, hInst, NULL);
		hInfo = CreateWindow ("STATIC","D_Click here for ERROR LOG", WS_CHILD | WS_VISIBLE | SS_NOTIFY, 870,0,250,23, hwnd, (HMENU)IDC_INFO, hInst, NULL);
		MoveWindow(hwnd, 0, 0, sizex, sizey, 0);
		hConnectionBall[0] = LoadBitmap (hInst, MAKEINTRESOURCE(IDB_BITMAP1)); // ready
		hConnectionBall[1] = LoadBitmap (hInst, MAKEINTRESOURCE(IDB_BITMAP2)); // on
		hConnectionBall[2] = LoadBitmap (hInst, MAKEINTRESOURCE(IDB_BITMAP3)); // off
		whichBitmap=0;
		hFontStat = CreateFont (12, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_ROMAN,
			"Arial");
		hLogDlg = CreateDialog (hInst, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, DlgProc);
		return 0;

	case WM_COMMAND:
		command = LOWORD(wParam);
		if (command==IDC_INFO)
		{
			if (HIWORD(wParam) == STN_DBLCLK)
				ShowWindow(hLogDlg, SW_SHOW);
		}
		return 0;

	case WM_CTLCOLORSTATIC:
		hdc = (HDC)wParam;
		col = GetSysColor (COLOR_WINDOW);
		id = (int)GetWindowLong ((HWND)lParam, GWL_ID);
		switch (id)
		{
		case IDC_STAT:
		case IDC_INFO:
			SelectObject (hdc, hFontStat);
			SetBkColor (hdc, col);
			break;
		}
		return (LRESULT) GetSysColorBrush  (COLOR_WINDOW);

	case WM_FL_CONNECTED:
//		recordFly ("flylog.txt",1);
		whichBitmap = 1;
		InvalidateRect (hwnd, NULL, FALSE);
		if (strlen(dllname)>0)
		{
			if ( (hMod = LoadLibrary(dllname))==NULL)
			{
				GetLastErrorStr(buf1);
				wsprintf (buf, "%s (%d).", buf1, GetLastError());
				MessageBox(hwnd, "LoadLibrary failed.", buf, MB_OK);
			}
			else
			{
				pf4SubClass = (PFUN) GetProcAddress((HMODULE)hMod, (LPCSTR)MAKELONG(1,0));
				hAccel = pf4SubClass(hwnd); // subclassing so that in the future all the message is routed to ProcessMsg() in the dll.
				wsprintf (buf, "%s Launched from a saved dll.", fname);
				SetWindowText (hStat, buf);
			}
		}
		return 0;
	case WM_FL_CLOSE:
		//The following lines caused crash of TS in Windows 7
		// that's probably because touch dll was loaded here but referenced somewhere later by a MESSAGE LOOP.
		// so just as a good practice, FreeLibrary is called whenever LoadLibrary is called. 05/23/2013 bjkwon
		//if(!FreeLibrary(hMod))
		//{	wsprintf (buf, "Error FreeLibrary(), code=%d.", GetLastError());		}
		FileWrittenDone=0;
		hAccel = NULL;
		wsprintf (buf, "Waiting...", fname, cumRecv);
		SetWindowText (hStat, buf);
		whichBitmap = 2;
		InvalidateRect (hwnd, NULL, FALSE);
		return 0;

	case WM_FL_NETWORK_ERR:
		strcpy(buf, (char*)lParam);
		strcat(buf, "  ");
		strcat(buf, (char*)wParam);
		strcat(buf, "\n");
		EditPrintf(GetDlgItem(hLogDlg, IDC_LOG), buf);
		return 0;

	case WM_FL_ARRIVED:
		command = LOWORD(wParam);
		switch(command)
		{
		case FL_FILEINFO: // format: 8 numeric character of filesize, filename 
			if (dlllaunched) return 0;
			recvPt = (char*)lParam;
			strncpy(buf1, recvPt,8);  buf1[8]='\0';
			filesize = atoi(buf1);
			strcpy(fname, recvPt+8);
			FileWrittenDone = 0;
			cumRecv = 0;
			if(!FreeLibrary(hMod))
			{	wsprintf (buf, "Error FreeLibrary(), code=%d.", GetLastError());		}
			if ( (hMod = LoadLibrary(fname))==NULL)
			{
				GetLastErrorStr(buf1);
				wsprintf (buf, "%s (%d).", buf1, GetLastError());
				MessageBox(hwnd, "LoadLibrary failed.", buf, MB_OK);
				break;
			}
			pf4SubClass = (PFUN) GetProcAddress((HMODULE)hMod, (LPCSTR)MAKELONG(1,0));
			hAccel = pf4SubClass(hwnd); // subclassing so that in the future all the message is routed to ProcessMsg() in the dll.
			wsprintf (buf, "%s launched.", fname);
			SetWindowText (hStat, buf);
			break;
		case FL_FILE:
			if (dlllaunched) return 0;
			if (FileWrittenDone)
			{
				MessageBox(hwnd, "File was done written","", MB_OK);
				break;
			}
			bytesRecvd =(DWORD)HIWORD(wParam);
			i=WriteFile(hFile, (void *)lParam, bytesRecvd, &dw, NULL);
			if (i==0 || dw!=(DWORD)bytesRecvd)
			{
				wsprintf (buf, "Bytes written so far %d", cumRecv);
				MessageBox(hwnd, "File writing error or \nNot all the data were written", buf, MB_OK);
				break;
			}
			cumRecv += bytesRecvd;
			wsprintf (buf, "File %s %d bytes written.", fname, cumRecv);
			SetWindowText (hStat, buf);
			if (cumRecv>=filesize)
			{
 				FileWrittenDone=1;
				i=CloseHandle(hFile);
				wsprintf (buf, "File creation success %s %d bytes.", fname, cumRecv);
				SetWindowText (hStat, buf);
				_splitpath(fname, drv, dir, buf, ext);
				if (!_stricmp(ext,".dll"))
				{
					if ( (hMod = LoadLibrary(AppPath))==NULL)
					{
						GetLastErrorStr(buf1);
						wsprintf (buf, "%s (%d).", buf1, GetLastError());
						MessageBox(hwnd, "LoadLibrary failed.", buf, MB_OK);
						break;
					}
					pf4SubClass = (PFUN) GetProcAddress((HMODULE)hMod, (LPCSTR)MAKELONG(1,0));
					hAccel = pf4SubClass(hwnd); // subclassing so that in the future all the message is routed to ProcessMsg() in the dll.
				}
				else if (!_stricmp(ext,".exe"))
				{
					STARTUPINFO si;
					PROCESS_INFORMATION pi;
					ZeroMemory(&si, sizeof(STARTUPINFO));
					ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
					si.cb = sizeof(STARTUPINFO);
					si.dwFlags = STARTF_USESHOWWINDOW;
					si.wShowWindow = SW_SHOWNORMAL;
					CreateProcess(AppPath, "", NULL, NULL, TRUE, 0, NULL, NULL,  &si, &pi);
				}
				else
				{
					MessageBox(hwnd, "Invalid filename", fname, MB_OK);
					break;				
				}
				wsprintf (buf, "%s Launched.", fname);
				SetWindowText (hStat, buf);
			}
			break;

		case FL_RUNDLL:
			if(!FreeLibrary(hMod))
			{	wsprintf (buf, "Error FreeLibrary(), code=%d.", GetLastError());		}
			strcpy(fname, (char*)lParam);
			if ( (hMod = LoadLibrary(fname))==NULL)
			{
				GetLastErrorStr(buf1);
				wsprintf (buf, "%s (%d).", buf1, GetLastError());
				MessageBox(hwnd, "LoadLibrary failed.", buf, MB_OK);
				break;
			}
			pf4SubClass = (PFUN) GetProcAddress((HMODULE)hMod, (LPCSTR)MAKELONG(1,0));
			hAccel = pf4SubClass(hwnd); // subclassing so that in the future all the message is routed to ProcessMsg() in the dll.
			wsprintf (buf, "%s Launched.", fname);
			SetWindowText (hStat, buf);
			break;
		}
		return 0;

	case WM_PAINT:
		hdc = BeginPaint (hwnd, &ps);
		hMem = CreateCompatibleDC(hdc);
		SelectObject (hMem, hConnectionBall[whichBitmap]);
		i=BitBlt (hdc, 20, 670, 50, 50, hMem, 0, 0, SRCCOPY);
		DeleteDC(hMem);
		EndPaint (hwnd, &ps);
		return 0;

	case WM_DESTROY:
//		DestroyWindow(hLogDlg);
		closefly();
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc( hwnd, message, wParam, lParam );
	}
	return DefWindowProc( hwnd, message, wParam, lParam );
}
 
int APIENTRY WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstace, LPSTR lpCmdLine, int nCmdShow)
{
	MSG			msg;
	WNDCLASS	wc;
	HWND		hWnd;

	if (!hPrevInstace)
	{
		wc.style			= CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc		= (WNDPROC) WndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= hInstance;
		wc.hIcon			= LoadIcon (hInstance, lpszAppName);
		wc.hCursor			= LoadCursor (NULL, IDC_ARROW);
		wc.hbrBackground	= (HBRUSH) (COLOR_WINDOW+1);
		wc.lpszMenuName		= lpszAppName;
		wc.lpszClassName	= lpszAppName;
		if (RegisterClass (&wc) == 0)
		{
			MessageBox (NULL, "RegisterClass() failed","Don't ask me why", MB_OK);
			return FALSE;
		}
	}
	hAccel = NULL;
	hInst = hInstance;
	strcpy(comLine,lpCmdLine);

	hWnd = CreateWindow (lpszAppName, "TouchScreen", WS_OVERLAPPEDWINDOW, 
						 CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL,
						 hInstance, NULL);
	if (hWnd == NULL) 		return FALSE;

	ShowWindow (hWnd, nCmdShow);

	while (GetMessage (&msg, NULL, 0, 0)>0 )
	{
		if (hAccel==NULL || !TranslateAccelerator(hWnd, hAccel, &msg) )
		{
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
	} 
	return (int)msg.wParam;
}

