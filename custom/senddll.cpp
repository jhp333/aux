#include "stdafx.h"
#include "psycon.h"
#include "psyconDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define BLOCKSIZE	65400

BOOL CPsyconDlg::SendDll(CString dllname)
{
	int filesize, i, cumRecv, res, loop=1;
	char outbuffer[BLOCKSIZE+64], buf[MAX_PATH];
	HANDLE hFile;
	DWORD dw;

	wsprintf (buf, "%s%s", theApp.AppPath, dllname);
	if ( (hFile = ::CreateFile(buf, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))==INVALID_HANDLE_VALUE)
	{
		i=GetLastError();
		MessageBox ("dll not found.", dllname, MB_OK);
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
		if (::ReadFile(hFile, outbuffer, BLOCKSIZE, &dw, NULL)==0)
		{
			wsprintf(outbuffer, "code=%d", GetLastError());
			MessageBox ("ReadFile Error.", outbuffer, MB_OK);
			CloseHandle(hFile);
			return 0;
		}
		else
		{
			cumRecv += dw;
			res=flySendBin (FL_FILE, outbuffer, dw);
			wsprintf(buf, "this transmission:%d, cumulative:%d\r\n", res,cumRecv);
	//		WriteFile(hFile2, buf, strlen(buf), &dw, NULL);
		}
	}
	::CloseHandle(hFile);
	return 1;
}

