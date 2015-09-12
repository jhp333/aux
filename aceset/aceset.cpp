#include <limits>
using namespace std;

HMODULE hDllModule;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	hDllModule = (HMODULE)hModule;
    return TRUE;
}

int UpdateData(HWND hDlg, CParamACE * param, bool data2screen)
{
	int i, res, trans, cum(0);
	float dres;
	CStdString str, str1, str2;

	if (data2screen)
	{
		if (param->level>0)		{ sprintfFloat(param->level, 1, str); SetDlgItemText(hDlg, IDC_LEVEL, str); }
		if (strlen(param->subj)>0) SetDlgItemText(hDlg, IDC_SUBJ_ID, param->subj); else return 0;
		if (strlen(param->mode)>0) SetDlgItemText(hDlg, IDC_MODE, param->mode); else return 0;
		if (param->pw[0] * param->pg != 0.) {
			sprintfFloat(param->pw[0], 1, str1); sprintfFloat(param->pg, 1, str2);
			str.Format("%s/%s", str1, str2);
			SetDlgItemText(hDlg, IDC_PW_PG, str);  }
		if (param->rate != 0.) {sprintfFloat(param->rate, 1, str); SetDlgItemText(hDlg, IDC_CH_RATE, str);  }
		if (param->numMax != 0) SetDlgItemInt(hDlg, IDC_NUMMAX, param->numMax, 0);
		if (param->Q != 0) SetDlgItemInt (hDlg, IDC_Q, param->Q, 0);
		// if selected and numBands are not consistent, selected has the priority.
		// (numBands is initialized to zero any time data set is read)
		for (i=0, param->numBands=0; i<22; i++) {
			SetDlgItemInt(hDlg, IDC_T0+i, (int)param->t[i], 0);
			SetDlgItemInt(hDlg, IDC_C0+i, (int)param->c[i], 0);
			sprintfFloat(param->gains[i], 1, str); SetDlgItemText(hDlg, IDC_GAIN0+i, str);
			if (param->selected[i]) {
				param->numBands++;	SendDlgItemMessage(hDlg, IDC_ELEC_CHECK0+i, BM_SETCHECK, BST_CHECKED, 0); }	
			else SendDlgItemMessage(hDlg, IDC_ELEC_CHECK0+i, BM_SETCHECK, BST_UNCHECKED, 0);
		}
		SetDlgItemInt(hDlg, IDC_NUMBANDS, param->numBands, 0);
		
		UpdateCutoffFreq (hDlg);
	}
	else
	{
		dres = (float)GetDlgItemDouble(hDlg, IDC_LEVEL, &trans);
		if (trans) param->level = dres;
		else { MessageBox (hDlg, "Invalid level", "ACEset", MB_OK); return 0;}
		res = GetDlgItemInt(hDlg, IDC_NUMMAX, &trans, 0);
		if (trans) param->numMax = res;
		else { MessageBox (hDlg, "Invalid nmber of Maxima", "ACEset", MB_OK); return 0;}
		res = GetDlgItemInt(hDlg, IDC_Q, &trans, 0);
		if (trans) param->Q = res;
		else { MessageBox (hDlg, "Invalid Q", "ACEset", MB_OK); return 0;}
		for (i=0, param->numBands=0; i<22; i++) {
			param->t[i] = GetDlgItemInt(hDlg, IDC_T0+i, &trans, 0);
			param->c[i] = GetDlgItemInt(hDlg, IDC_C0+i, &trans, 0);
			param->gains[i] = GetDlgItemDouble (hDlg, IDC_GAIN0+i);
			if (SendDlgItemMessage(hDlg, IDC_ELEC_CHECK0+i, BM_GETCHECK, 0, 0) == BST_CHECKED) 
			{	param->selected[i]=1; param->numMax++; }
			else
				param->selected[i]=0;
		}
	}
	return 1;
}

BOOL CALLBACK DlgProc(HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	static CParamACE * param;
	char fname[MAX_PATH], fullfname[MAX_PATH], errstr[128];
	WORD iCtrl;
	int q, num, trans1, trans2;
	switch (umsg)
	{
	case WM_INITDIALOG:
		FileDlgInit(NULL);
		param = (CParamACE *)lParam;
		UpdateData(hDlg, param, true);
		SetWindowLong (hDlg, GWL_USERDATA, (LONG)param);
		SendDlgItemMessage (hDlg, IDC_Q, CB_SETCURSEL, 0, 0);
		break;
	case WM_COMMAND:
		iCtrl = LOWORD(wParam);
		switch(iCtrl)
		{
		case IDC_OPEN_MAP:
			fname[0]='\0';fullfname[0]='\0';
			if (!FileOpenDlg(hDlg, fullfname, fname, "ACE MAP Files (*.ACE)\0*.ace\0", "ace"))
			{
				sprintf(errstr, "GetOpenFileName failed. code=%d", GetLastError());
				MessageBox (hDlg, errstr, "aceset::DlgProc", MB_OK);
			}
			else
			{
				if (!readACEPatientFile(fullfname, param))
				{
					SetDlgItemText (hDlg, IDC_MAP_FILE, fname);
					UpdateData(hDlg, param, true);
				}
				else
				{
					MessageBox(hDlg, errstr, "Error during opening subject-specific data", MB_OK);
					break;
				}
			}
			break;
		//case IDC_Q:
		//case IDC_NUMMAX:
		//	//Read changes from the screen and update param
		//	num = GetDlgItemInt (hDlg, IDC_NUMMAX, &trans1, 0);
		//	q = GetDlgItemInt (hDlg, IDC_Q, &trans2, 0);
		//	if (trans1*trans2)
		//	{	param->numMax = num; param->Q = q; }
		//	else
		//		MessageBox(hDlg, "Invalid Number of Max or Q", "ACESET", MB_OK);
		//	break;

		default:
			if (IsBetween(iCtrl, IDC_ELEC_CHECK0, IDC_ELEC_CHECK21)) UpdateCutoffFreq(hDlg);
			break;
		}
		break;
	default:
		return 0;
	}
	return 1;
}

ACESET_API HWND ACEChildDlg (HWND hParent, POINT position, CParamACE * param, char *errstr)
{
	HWND hDlg;
	RECT rt;
	if (hParent==NULL) {strcpy(errstr, "Null parent pointer."); return NULL;}
	if (param==NULL) {strcpy(errstr, "param pointer must have been allocated."); return NULL;}
	hDlg = CreateDialogParam(hDllModule, MAKEINTRESOURCE(IDD_ACESET), hParent, DlgProc, (LPARAM)param); 
	GetWindowRect(hDlg, &rt);
	MoveWindow(hDlg, position.x, position.y, rt.right-rt.left, rt.bottom-rt.top, TRUE);
	return hDlg;
}

ACESET_API CParamACE * GetParam(HWND hACEDlg, char *errstr)
{
	CParamACE *out;
	if (hACEDlg==NULL) { strcpy(errstr, "ACE Dlg not ready"); return NULL;}
	out = (CParamACE *)(LONG_PTR)GetWindowLong (hACEDlg, GWL_USERDATA);
	UpdateData(hACEDlg, out, 0); // update param
	return out;
}

ACESET_API double GetLevel(HWND hACEDlg)
{
	int trans;
	double level = GetDlgItemDouble(hACEDlg, IDC_LEVEL, &trans);
	if (!trans) 
	{
		MessageBox (hACEDlg, "Invalid level", "Aceset", MB_OK); 
		return ACESET_QNAN;
	}
	return level;
}

ACESET_API LRESULT ClickOpenMAP(HWND hACEDlg)
{
	return SendMessage(hACEDlg, WM_COMMAND, MAKELONG(IDC_OPEN_MAP, BN_CLICKED), (LPARAM)GetDlgItem(hACEDlg, IDC_OPEN_MAP) );
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
	return 1l;
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

