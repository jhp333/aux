#include "Echo.h"

char AppPath[MAX_PATH];

void getCurrentModulePath(char *moduleName, char *path)
{
 	char drive[16], dir[256], ext[8], buffer[256], buf[256];

 	GetModuleFileName((HINSTANCE)GetModuleHandle(NULL), buf, sizeof(buf));
 	_splitpath(buf, drive, dir, buffer, ext);
 	sprintf (moduleName, "%s%s", buffer, ext);
 	strcpy(buffer, buf);
 	sprintf (path, "%s%s", drive, dir);
}

