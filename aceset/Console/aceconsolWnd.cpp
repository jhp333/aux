// AceconsolWnd.cpp: implementation of the CAceconsolWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "console.h"
#include "AceconsolWnd.h"


#define UPDATEERROR {	lastError += strcat(errstr, "\r\n"); 		res=0;}
#define BLOCKSIZE	65400


char szLogFilter[] = "Log Files (*.LOG)\0*.log\0";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAceconsolWnd::CAceconsolWnd()
{
}

CAceconsolWnd::~CAceconsolWnd()
{
} 

void CAceconsolWnd::OnCommand(int idc, HWND hwndCtl, UINT event)
{
	CStdString msg, errStr;
	WORD dummy(99);
	switch(idc)
	{
	case IDC_RUN:
		GetDlgItemText(IDC_COMMANDS, msg);
		FER(flySendText(dummy, msg.c_str()))
		//if (!CallNamedPipe(PipeName, (LPVOID)(LPCTSTR)msg.c_str(), msg.GetLength()+1,
		//	reply, sizeof(reply), &nByte, NMPWAIT_WAIT_FOREVER) )
		//{
		//	CWndDlg::GetLastErrorStr(errStr);
		//	MessageBox ("CallNamedPipe failed", errStr.c_str(), MB_OK);
		//}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	CWndDlg::OnCommand(idc, hwndCtl, event);
}

BOOL CAceconsolWnd::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
	PipeName.Format("\\\\.\\pipe\\%s", ACEPLAYER_CONSOLE_PIPENAME);
	int i = initflySpecialPort(hDlg, "127.0.0.1", CONSOLE_PORT, -1, 1);
	return 1;
}

void CAceconsolWnd::OnFlyArrived(WORD command, WORD len, void* inBuffer)
{

}

void CAceconsolWnd::OnFlyClosed()
{

}

void CAceconsolWnd::OnFlyConnected(char *hostname, char* ipa)
{


}

