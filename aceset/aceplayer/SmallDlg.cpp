#include "aceplayer.h"

BOOL CALLBACK SmallDlg (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	static char *strPt;
	switch (umsg)
	{
	case WM_INITDIALOG:
		strPt = (char*)lParam;
		SetWindowText(GetDlgItem(hDlg, IDC_STATIC_DIRECTION), (char*)lParam);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			GetWindowText(GetDlgItem(hDlg, IDC_EDIT_GET), strPt, 512);
			EndDialog(hDlg, IDOK);
		}
		else if (LOWORD(wParam) == IDCANCEL) 
			EndDialog(hDlg, IDCANCEL);
		break;
	default:
		return 0;
	}
	return 1;
}