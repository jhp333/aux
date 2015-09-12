#pragma once

#include "WndDlg.h"
#include "ACEsetDlg.h"

#include <limits>
using namespace std;

ACESET_API void OpenMAP(HWND hACEDlg, const char *mapfile, InterfaceType procEnum)
{
	SendMessage(hACEDlg, WM__OPEN_MAP, (WPARAM)mapfile, (LPARAM)procEnum);
}

ACESET_API LRESULT SetMax(HWND hACEDlg, int max)
{
	SetDlgItemInt(hACEDlg, IDC_NUMMAX, max, 1);
	return SendMessage(hACEDlg, WM_COMMAND, MAKELONG(IDC_NUMMAX, EN_SETFOCUS), (LPARAM)GetDlgItem(hACEDlg, IDC_NUMMAX));
}

ACESET_API LRESULT SetQ(HWND hACEDlg, int Q)
{
	SetDlgItemInt(hACEDlg, IDC_Q, Q, 1);
	return SendMessage(hACEDlg, WM_COMMAND, MAKELONG(IDC_Q, EN_SETFOCUS), (LPARAM)GetDlgItem(hACEDlg, IDC_Q));
}

ACESET_API LRESULT SelectElectrode(HWND hACEDlg, int electrode, int onoff)
{
	WPARAM CHECK = onoff ? BST_CHECKED : BST_UNCHECKED;
	return SendDlgItemMessage(hACEDlg, IDC_ELEC_CHECK0+22-electrode, BM_SETCHECK, CHECK, 0);
}

ACESET_API LRESULT ToggleElectrode(HWND hACEDlg, int electrode)
{
	int iCtrl = IDC_ELEC_CHECK0+22-electrode;
	WPARAM CHECK = (SendDlgItemMessage(hACEDlg, iCtrl, BM_GETCHECK, 0, 0)==BST_CHECKED)?
					BST_UNCHECKED : BST_CHECKED;
	SendDlgItemMessage(hACEDlg, iCtrl, BM_SETCHECK, CHECK, 0);
	return SendMessage(hACEDlg, WM_COMMAND, MAKELONG(iCtrl, BN_CLICKED), (LPARAM)GetDlgItem(hACEDlg, iCtrl) );
}

ACESET_API LRESULT SelectFreqLimits(HWND hACEDlg, double freq1, double freq2)
{
	if (freq1>=freq2) 
	{
		MessageBox (hACEDlg, "Upper limit must be greater than lower limit.", "SelectFreqLimits", MB_OK);
		return 0;
	}
	int idx1 = (int)((freq1-63.)/125. + .5)-1;
	int idx2 = (int)((freq2-63.)/125. + .5)-1;
	SendDlgItemMessage(hACEDlg, IDC_FREQ1, CB_SETCURSEL, idx1, 0);
	SendDlgItemMessage(hACEDlg, IDC_FREQ2, CB_SETCURSEL, idx2, 0);
	return SendMessage(hACEDlg, WM_COMMAND, MAKELONG(IDC_FREQ1, CBN_SELCHANGE), (LPARAM)GetDlgItem(hACEDlg, IDC_FREQ1) );
}

ACESET_API LRESULT GlobalAdjT(HWND hACEDlg, bool increment)
{
	if (increment)
		return SendMessage(hACEDlg, WM_COMMAND, MAKELONG(IDC_TUP, BN_CLICKED), (LPARAM)GetDlgItem(hACEDlg, IDC_TUP) );
	else
		return SendMessage(hACEDlg, WM_COMMAND, MAKELONG(IDC_TDN, BN_CLICKED), (LPARAM)GetDlgItem(hACEDlg, IDC_TDN) );
}

ACESET_API LRESULT GlobalAdjC(HWND hACEDlg, bool increment)
{
	if (increment)
		return SendMessage(hACEDlg, WM_COMMAND, MAKELONG(IDC_CUP, BN_CLICKED), (LPARAM)GetDlgItem(hACEDlg, IDC_CUP) );
	else
		return SendMessage(hACEDlg, WM_COMMAND, MAKELONG(IDC_CDN, BN_CLICKED), (LPARAM)GetDlgItem(hACEDlg, IDC_CDN) );
}

ACESET_API LRESULT GlobalAdjGain(HWND hACEDlg, bool increment)
{
	if (increment)
		return SendMessage(hACEDlg, WM_COMMAND, MAKELONG(IDC_GAINUP, BN_CLICKED), (LPARAM)GetDlgItem(hACEDlg, IDC_GAINUP) );
	else
		return SendMessage(hACEDlg, WM_COMMAND, MAKELONG(IDC_GAINDN, BN_CLICKED), (LPARAM)GetDlgItem(hACEDlg, IDC_GAINDN) );
}

