#include "aceplayer.h"
#include "aceplayerdlg.h"

BOOL CALLBACK PulseViewProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	switch (umsg)
	{
	case WM_INITDIALOG:
		return 1;
	case WM_PAINT:
		return 1;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			ShowWindow(hDlg, SW_HIDE);
			break;
		}
		break;
	default:
		return 0;
	}
	return 1;
}