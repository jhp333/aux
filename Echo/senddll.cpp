#include "Echo.h"
#include "EchoMainWnd.h"

/*#include <windows.h>
#include <flyd.h>
#include "fer.h"
#include "protocol.h"
#include "resource.h"
*/

#define BLOCKSIZE	65400

BOOL CEchoMainWnd::SendDll(char *dllname)
{
	int filesize, i, cumRecv, res, loop=1;
	char outbuffer[BLOCKSIZE+64], buf[MAX_PATH];
	HANDLE hFile;
	DWORD dw;

	wsprintf (buf, "%s%s", AppPath, dllname);
	if ( (hFile = CreateFile(buf, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))==INVALID_HANDLE_VALUE)
	{
		i=GetLastError();
		MessageBox (hDlg, "dll not found.", dllname, MB_OK);
		return 0;
	}
	res=999;

	filesize = GetFileSize(hFile, &dw);
	wsprintf (outbuffer, "%8d%s", filesize, dllname);
	outbuffer[8+strlen(dllname)]='\0';
	FER(flySendText (FL_FILEINFO, outbuffer));

	cumRecv=0;
	while (loop && cumRecv < filesize)
	{
		if (ReadFile(hFile, outbuffer, BLOCKSIZE, &dw, NULL)==0)
		{
			wsprintf(outbuffer, "code=%d", GetLastError());
			MessageBox (hDlg, "ReadFile Error.", outbuffer, MB_OK);
			CloseHandle(hFile);
			return 0;
		}
		else
		{
			cumRecv += dw;
			res=flySendBin (FL_FILE, outbuffer, dw);
			wsprintf(buf, "this transmission:%d, cumulative:%d\r\n", res,cumRecv);
		}
	}
	CloseHandle(hFile);
	return 1;
}

