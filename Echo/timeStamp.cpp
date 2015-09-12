#include <time.h>
#include "Echo.h"

int timeStamp ( char* dateStr, char* timeStr)
{
	if ( (dateStr==NULL) || (timeStr==NULL) )
		return 0;
    _strdate( dateStr );
    _strtime( timeStr );
	return 1;
}

int AddString2NodesList (HWND hDlg, char *nodeFile)
{
	// node names for presenter are retrieved from nodes.ini
	HANDLE hfile;
	char buf[2048], **nodes;
	int i, nItems;
	DWORD dw;
	SendDlgItemMessage (hDlg, IDC_NODELIST, CB_ADDSTRING, 0, (LPARAM)".");
	if ((hfile = CreateFile ("nodes.ini", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))!=INVALID_HANDLE_VALUE)
	{
		ReadFile (hfile, buf, sizeof(buf), &dw, NULL); buf[dw]=0;
		nItems = countDeliminators (buf, "\r\n");
		nodes = (char**)calloc(nItems, sizeof(char*));
		for (i=0; i<nItems; i++)
			nodes[i]=(char*)calloc(MAX_LEN_NODENAME, 1);
		i=str2strarray (nodes, nItems, buf, "\r\n");
		for (i=0; i<nItems; i++)
			SendDlgItemMessage (hDlg, IDC_NODELIST, CB_ADDSTRING, 0L, (LPARAM)nodes[i]);
		CloseHandle (hfile);
		for (i=0; i<nItems; i++) 
			free(nodes[i]);
		free(nodes);
		return 1;
	}
	else
		return 0;
}