#include "Echo.h"
#include "EchoMainWnd.h"

BATCH::BATCH()
:onoff(0)
{
}

BATCH::~BATCH()
{
}

void BATCH::OnDestroy()
{
	hDlg = NULL;
}

BOOL BATCH::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
	hPreviousBatch = GetDlgItem(IDC_PREV_BATCH);
	return 1;
}

		
void BATCH::OnRunbatch()
{
	char rest[MAX_BATCH_SIZE-128], buf2[MAX_BATCH_SIZE], command[128], *temp[2];
	int res;

	GetDlgItemText (IDC_CURRENT_BATCH, buf2, sizeof(buf2));
	temp[0] = command; temp[1] = rest;
	res = str2strarray(temp, 2, buf2, "\r\n");
	if (res>0)
	{
		EditPrintf (hPreviousBatch, "%s\r\n", command);
		SendDlgItemMessage (IDC_CURRENT_BATCH, EM_SETSEL, 0, -1);
		SendDlgItemMessage (IDC_CURRENT_BATCH, EM_REPLACESEL, 0, (LPARAM)"");
		if (res>1)
		{
			SetDlgItemText (IDC_CURRENT_BATCH, rest);
			SetDlgItemText (IDC_CURRENT_BATCH, strstr(buf2, rest));
		}
		execute (command, 1);
		UpdateWindow (hDlg);
	}
	else
	{
		MessageBox ("no more batch ready", "batch");
		ShowWindow (hDlg, SW_HIDE);
	}
}

void BATCH::OnCommand(int idc, HWND hwndCtl, UINT event)
{
	switch (idc)
	{
	case IDC_GO:
		onoff = 1;
		SendMessage (WM__RUNBATCH);
		break;
	case IDC_STOP:
		onoff = 0;
		break;
	case IDCANCEL:
		ShowWindow (hDlg, 0);
		break;
	}
}


void BATCH::execute (char *commandLine, int batchornot)
{// batchornot is necessary for SET without the actual batch
	int n, i, res;
	unsigned long nBytes;
	char *all[3], command[16], arg1[256], arg2[64];
	char NodeName[512], reply[128], PipeName[512], errstr[512];
	all[0] = command; all[1]=arg1; all[2]=arg2;

	n = str2strarray(all, 3, commandLine, " \t");
	if (!strcmp(command, "BEGIN"))
	{
		strcpy (NodeName, MainAppWnd.resultPath); 
		append_path_file (BEGIN_fname2save, NodeName, (char*)arg1);
		SendMessage (WM_COMMAND, IDC_BEGIN);
		return;
	}
	if (!strcmp(command, "LEVEL"))
	{
		::SetDlgItemText (hParent->hDlg, IDC_LEVEL, arg1);
	}
	else if (!strcmp(command, "FS"))
	{
		((CEchoMainWnd *)hParent)->Fs = atoi(arg1);
	}
	else if (!strcmp(command, "TRIALS"))
	{
		::SetDlgItemText (hParent->hDlg, IDC_NUM_TRIALS, arg1);
	}
	else if (!strcmp(command, "TALK"))
	{
		::SetDlgItemText (hParent->hDlg, IDC_TALK, strstr(commandLine, arg1));
	}
	else if (!strcmp(command, "WAIT"))
	{
		Sleep(atoi(arg1));
	}
	else if (!strcmp(command, "NUM_STIM"))
	{
		::SetDlgItemText (hParent->hDlg, IDC_NUM_STIM, arg1);
	}
	else if (!strcmp(command, "GAP"))
	{
		::SetDlgItemText (hParent->hDlg, IDC_GAP, arg1);
	}
	else if (!strcmp(command, "ADDPROC"))
	{
		::SetDlgItemText (hParent->hDlg, IDC_ADDPROC, strstr(commandLine, arg1));
	}
	else if (!strcmp(command, "NODE"))
	{
		::SetDlgItemText (hParent->hDlg, IDC_NODELIST, arg1);
	}
	else if (!strcmp(command, "CONNECT_PRESENTER"))
	{
		::SendMessage (::GetDlgItem(hParent->hDlg, IDC_CONNECT_PRESENTER), BM_CLICK, 0, 0);
	}
	else if (!strcmp(command, "OPEN_SESSION"))
	{
		::SendMessage (hParent->hDlg, WM__OPEN_SESSION, (WPARAM)arg1, 0);
	}
	else if (!strcmp(command, "SET"))
	{
		::GetDlgItemText (hParent->hDlg, IDC_NODELIST, NodeName, sizeof(NodeName));
		wsprintf(PipeName, "\\\\%s\\pipe\\%s", NodeName, "CochlearAudioSignalInterfacePipe");
		DISP_PIPE_LOG("[out]",commandLine);
		res = CallNamedPipe(PipeName, (LPVOID)(LPCTSTR)commandLine, strlen(commandLine)+1,
			reply, sizeof(reply), &nBytes, NMPWAIT_WAIT_FOREVER);
		if (!res)
		{
			i=GetLastError();
			GetLastErrorStr(errstr);
			sprintf (errstr, "CallNamedPipe failed. Experiment aborted.", errstr);
			return;
		}
		reply[nBytes]='\0';
		DISP_PIPE_LOG("[in]",reply);
	}
	if (batchornot)
		PostMessage (WM__RUNBATCH);
}

BOOL CALLBACK  batchProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	switch (umsg)
	{
	chHANDLE_DLGMSG (hDlg, WM_COMMAND, MainAppWnd.batch->OnCommand);
	chHANDLE_DLGMSG (hDlg, WM_INITDIALOG, MainAppWnd.batch->OnInitDialog);
	chHANDLE_DLGMSG (hDlg, WM_DESTROY, MainAppWnd.batch->OnDestroy);
	chHANDLE_DLGMSG (hDlg, WM__RUNBATCH, MainAppWnd.batch->OnRunbatch);
	default:		return 0;
	}
	return 1;
}
