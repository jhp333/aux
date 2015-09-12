#include "stdafx.h"

char * FulfillFile (char *out, const char *path, const char * file)
{
	size_t len = strlen(path);
	if (len==0)		
		strcpy(out, file);
	else if (path[len-1]!='\\')
		sprintf(out, "%s\\%s", path, file);
	else
		sprintf(out, "%s%s", path, file);
	return out;
}

void GetLastErrorStr(char *errstr)
{
	char  *pstr, string[256];
	DWORD WindowsError, nchar;
	WindowsError = GetLastError();

	nchar = FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, WindowsError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							(LPTSTR) string, sizeof(string), NULL  );
	if (nchar == 0)
		MessageBox (NULL, "Failed to translate Windows error", "", MB_OK);
	else
	{
		pstr = strstr(string, "\r\n");
		pstr[0]=0;
		strcpy(errstr, string);
	}
	return;
}
