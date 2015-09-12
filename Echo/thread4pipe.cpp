#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include "pipeCommunication.h"
#include "Echo.h"
#include "EchoMainWnd.H"

void thread4pipe (PVOID h)
{
	HWND hDlg;
	char command[64], reply[128], errStr[128], buf[128], *paring[16], NodeName[128], PipeName[128];
	int i;
	DWORD nByte;
	

	hDlg = (HWND)h;

	GetDlgItemText (hDlg, IDC_NODELIST, NodeName, sizeof(NodeName));
	wsprintf(PipeName, "\\\\%s\\pipe\\%s", NodeName, INTERFACE_PIPENAME);

	strcpy (command, "IDENTIFY ");
	LoadString (MainAppWnd.hInst, IDS_APPLICATION_ID, buf, 5);
	strcat (command, buf);
	DISP_PIPE_LOG("[out]",command);
	if (!CallNamedPipe(PipeName, (LPVOID)(LPCTSTR)command, strlen(command)+1,
		reply, sizeof(reply), &nByte, NMPWAIT_WAIT_FOREVER) )
	{
		GetLastErrorStr(errStr);
		MessageBox (hDlg, "TransactNamedPipe failed", errStr, MB_OK);
		_endthread();
		return;
	}
	DISP_PIPE_LOG("[in]",reply);
	for (i=0; i<16; i++)
		paring[i] = (char*)calloc (128, sizeof(char));

	str2strarray (paring, 16, reply, " ");

	if (strcmp(paring[0], "SUCCESS")==0)
	{
		SetDlgItemText (hDlg, IDC_PRESENTER, (char*)&reply[strlen(paring[0])+1]);
	}
	else
	{
		MessageBox (hDlg, "IDENTIFY failed", &reply[strlen(paring[0])+1], MB_OK);
		for (i=0; i<16; i++)
			free(paring[i]);
		return;
	}
}


