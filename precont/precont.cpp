// audfretDll.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "audfret.h"
#include "audstr.h"
#include "precont.h"
#include "resource.h"

#define SIGNAL_INTERFACE_PIPENAME  "CochlearAudioSignalInterfacePipe"
#define ACEPLAYER_CONSOLE_PIPENAME "ACE_CONSOLE_PIPE"
#define PREPAREPATH	"ResearchAudioSignals"

#define DISP_PIPE_LOG(x,y) \
	EditPrintf (hLog, "%s %s\r\n", (char*)x, (char*)y); 

HWND hLog(NULL);
HMODULE hMod;

BOOL CALLBACK  log_proc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	switch (umsg)
	{
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			ShowWindow (hDlg, 0);
			break;
		}
		break;
	case WM_DESTROY:
		hLog = NULL; //maybe this line is never processed??
		break;

	default:
		return 0;
	}
	return 1;
}

#ifndef LINK_STATIC_PRECONT
BOOL APIENTRY DllMain( HMODULE hModule, DWORD  fdwReason, LPVOID lpReserved )
{
	hMod = hModule;
	if (hLog==NULL) {
		HWND hLogWin = CreateDialog ((HINSTANCE)::GetModuleHandle(0), MAKEINTRESOURCE(IDD_LOG), NULL, log_proc);
		hLog = GetDlgItem(hLogWin, IDC_MSG_LOG4CONT);
	}
	if (fdwReason == DLL_PROCESS_DETACH) {
		DestroyWindow(hLog);
		hLog = NULL;
	}
    return TRUE;
}
#endif

EXP_PRECONT int PreparePresent(const char *_NodeName, const char *wavname, int acoustPlay, double level, char *prepInfo, size_t LenprepInfo, char *errstr)
{
	char fnamewpath[256], reply[512], PipeName[128], hostname[128], command[376];
	string NodeName, strbuffer[2];
	int res, nItems;
	DWORD nRead;

	if (hLog==NULL) {
		HWND hLogWin = CreateDialog ((HINSTANCE)::GetModuleHandle(0), MAKEINTRESOURCE(IDD_LOG), NULL, log_proc);
		hLog = GetDlgItem(hLogWin, IDC_MSG_LOG4CONT);
	}
	reply[0]=0;
	if (_NodeName[0]=='\0') NodeName = ".";
	else NodeName = _NodeName;

	sprintf(PipeName, "\\\\%s\\pipe\\%s", NodeName.c_str(), SIGNAL_INTERFACE_PIPENAME);
	if (NodeName == ".")
		sprintf (command, "PREPARE %5.1f \\%s\\%s", level, PREPAREPATH, wavname);
	else
	{
		const char *point = strchr(wavname, ':');
		gethostname(hostname, sizeof(hostname));
		sprintf (command, "PREPARE %5.1f \\\\%s%s", level, hostname, point+1);
	}
	DISP_PIPE_LOG("[out]",command);
	res = CallNamedPipe(PipeName, (LPVOID)(LPCTSTR)command, (DWORD)strlen(command)+1,
		prepInfo, (DWORD)LenprepInfo, &nRead, NMPWAIT_WAIT_FOREVER);
	while (!res)
	{
		GetLastErrorStr(reply);
		sprintf (errstr, "Error in CallNamedPipe (PREPARE)\nmsg: %s", reply);
		if (MessageBox (NULL, errstr, "Retry or cancel? (you can still retry after changing settings)", MB_RETRYCANCEL)==IDRETRY)
		{
			res = CallNamedPipe(PipeName, (LPVOID)(LPCTSTR)command, (DWORD)strlen(command)+1,
					prepInfo, (DWORD)LenprepInfo, &nRead, NMPWAIT_WAIT_FOREVER);
		}
		else
		{
			return 0;
		}
	}
	prepInfo[nRead]='\0';
	DISP_PIPE_LOG("[in]",prepInfo);	
	nItems=str2array(strbuffer, 2, prepInfo, " ");
	if (strbuffer[0] !="SUCCESS") 
	{		
		sprintf (errstr, "FAILURE in PREPARE: %s", prepInfo+strbuffer[0].length());
		return 0;
	}
	else
	{
		if (nItems==1)
			strcpy(prepInfo, ""); //????
		else
		{
			for (int i=0; i<=(int)strlen(prepInfo)-8; i +=8)
				strncpy (prepInfo+i, prepInfo+i+8, 8);
		}
	}

	strcpy(command, "PRESENT");

	DISP_PIPE_LOG("[out]",command);
	res = CallNamedPipe(PipeName, (LPVOID)(LPCTSTR)command, (DWORD)strlen(command)+1,
		reply, sizeof(reply), &nRead, NMPWAIT_WAIT_FOREVER); 
	while (!res)
	{
		GetLastErrorStr(errstr);
		if(MessageBox (NULL, "CallNamedPipe failed(PRESENT). Retry(Yes), or Abort(No)?", errstr, MB_YESNO)==IDYES)
		{
			res = CallNamedPipe(PipeName, (LPVOID)(LPCTSTR)command, (DWORD)strlen(command)+1,
				reply, sizeof(reply), &nRead, NMPWAIT_WAIT_FOREVER);
		}
		else
		{
			return 0;
		}
	}
	if (acoustPlay)
	{
		sprintf(fnamewpath, "\\%s\\%s", PREPAREPATH, wavname);
		PlaySound(fnamewpath, NULL, SND_FILENAME | SND_ASYNC);
	}
	reply[nRead]='\0';
	DISP_PIPE_LOG("[in]",reply);
	nItems=str2array(strbuffer, 2, reply, " ");
	if (strbuffer[0] !="SUCCESS") 
	{		
		if(MessageBox (NULL, "Fail to PRESENT. Retry(Yes), or Abort(No)?", reply, MB_YESNO)==IDYES)
		{
			return PreparePresent(_NodeName, wavname, acoustPlay, level, prepInfo, LenprepInfo, errstr);
		}
		else
		{
			strcpy(errstr, "Present failed by presenter");
			return 0;
		}
	}
	else
		return 1;
}