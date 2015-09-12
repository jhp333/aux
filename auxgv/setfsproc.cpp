#include "auxgv.h"
#include "SetFSProc.h"

#define		WM__INIT			WM_APP+100

CSetFS dlg;

CSetFS::CSetFS()
{

}

CSetFS::~CSetFS()
{

}

BOOL CSetFS::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
	int fs = *(int*)lParam;
	PostMessage(WM__INIT, (WPARAM)fs);
	return 1;
}

void CSetFS::InitDialog2(int fs)
{
	SetDlgItemInt(IDC_FS, fs);
}

void CSetFS::OnCommand(int idc, HWND hwndCtl, UINT event)
{
	int k, res;
	switch (idc)
	{
	case IDOK:
		res = GetDlgItemInt(IDC_FS, &k);
		if (k && res>10)
		{
			EndDialog(hDlg, res);
		}
		else
			MessageBox("Invalid sample rate");
		break;
	case WM_DESTROY:
		EndDialog(hDlg, 0);
		break;
	}
}

void CSetFS::OnClose()
{
	EndDialog(hDlg, 0);
}



BOOL CALLBACK  setfsproc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	dlg.hDlg = hDlg;
	switch (umsg)
	{
	chHANDLE_DLGMSG (hDlg, WM_COMMAND, dlg.OnCommand);
	chHANDLE_DLGMSG (hDlg, WM_INITDIALOG, dlg.OnInitDialog);
	chHANDLE_DLGMSG (hDlg, WM_DESTROY, dlg.OnClose);
	chHANDLE_DLGMSG (hDlg, WM__INIT, dlg.InitDialog2);
		break;
	default:		return 0;
	}
	return 1;
}
