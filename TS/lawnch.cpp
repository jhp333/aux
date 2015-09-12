#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <audfret.h>
#include "resource2.h"

#define AUDITORYPATH "c:\\Program Files\\Common Files\\auditorypro\\;"

// Lawnch version 2.0
// Usage: Lawnch (executable) path
// Executable should include extension. It can omit the extension only if path is not supplied, in which case path is AUDITORYPATH

int  WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
	STARTUPINFO sinfo;
	PROCESS_INFORMATION pinfo;
	DWORD dwres;
	size_t len = 10*MAX_PATH;
	char recoup[MAX_PATH], elsy[MAX_PATH];
	char errstr[MAX_PATH], *pathStr, tempath[MAX_PATH];
	size_t pathStrLen;
	char *pt;
	const char *auditoryPath = AUDITORYPATH;

	if (strlen(szCmdLine)==0)
	{
		LoadString (hInstance, IDS_STRING105, elsy, MAX_PATH);
		strcpy(errstr, elsy);
		LoadString (hInstance, IDS_STRING106, elsy, MAX_PATH);
		strcat(errstr, elsy);
		LoadString (hInstance, IDS_STRING107, elsy, MAX_PATH);
		strcat(errstr, elsy);
		LoadString (hInstance, IDS_STRING108, elsy, MAX_PATH);
		strcat(errstr, elsy);
		MessageBox (NULL, errstr, "lawnch", MB_OK);
		return 0;
	}
	pt = strchr(szCmdLine,'*');
	if (pt==NULL) // if no delimiter, use default path
	{
		strcpy(tempath, auditoryPath);
		strcpy(recoup, szCmdLine);
	}
	else
	{
		strcpy(tempath, pt+1);
		trimLeft(tempath, " ");
		strcat(tempath, ";");
		strncpy(recoup, szCmdLine, pt-szCmdLine-1);
		recoup[pt-szCmdLine-1]='\0';
	}
	pathStrLen = strlen(tempath);

	HANDLE h = LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, 0);
    SetClassLongPtr (NULL, GCL_HICON, (LONG)(LONG_PTR)h);
    ZeroMemory( &sinfo, sizeof(sinfo) );
    sinfo.cb = sizeof(sinfo);
    ZeroMemory( &pinfo, sizeof(pinfo) );

	pathStr = new char[len];
	strcpy(pathStr, tempath);
	dwres = GetEnvironmentVariable("PATH", pathStr+pathStrLen, (DWORD)(len-pathStrLen));
	while (dwres>len-3)
	{
		delete[] pathStr;
		pathStr = new char[len += MAX_PATH];
		dwres = GetEnvironmentVariable("PATH", pathStr+pathStrLen, (DWORD)(len-pathStrLen));
	}
	
	if (!SetEnvironmentVariable("PATH", pathStr))
	{
		sprintf(errstr, "SetEnvironmentVariable failed. code=%d", GetLastError());
		MessageBox (NULL, errstr, "lawnch", MB_OK);
		delete[] pathStr;
		return 0;
	}



	if (strlen(szCmdLine)>0)
	{
		if (!CreateProcess (NULL, recoup, NULL, NULL, FALSE, 0, NULL, NULL, &sinfo, &pinfo))
		MessageBox (NULL, "Error", "Cannot launch the process.", MB_OK);
	}
	delete[] pathStr;
	return 0;
}