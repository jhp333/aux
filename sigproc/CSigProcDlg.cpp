#pragma once

#include "sigprocdlg.h"
#include "resource.h"
#include "msgCrack.h"

CWndDlg wnd;
CSigProcDlg *ACEDlg;
extern HMODULE hDllModule;

BOOL CALLBACK  SigprocDlgProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	switch (umsg)
	{
	chHANDLE_DLGMSG (hDlg, WM_INITDIALOG, wnd.OnInitDialog);
	chHANDLE_DLGMSG (hDlg, WM_COMMAND, ACEDlg->OnCommand);
	default:
		return FALSE;
	}
	return TRUE;
}

CSigProcDlg::CSigProcDlg(void)
: errstr(""), varstr(""), sigstr(""), grp(NULL)
{
	
}

CSigProcDlg::~CSigProcDlg(void)
{
	if (grp!=NULL) delete grp;
}

HWND CSigProcDlg::Create(HWND hPar, POINT position, int fs)
{
	RECT rt;
	
	wnd.hDlg = CreateDialog(hDllModule, MAKEINTRESOURCE(IDD_SIGPROC), hPar, (DLGPROC)SigprocDlgProc); 
	ACEDlg = this;
	hDlg = wnd.hDlg ;

	OnInitDialog(NULL, NULL);
	GetWindowRect(hDlg, &rt);
	MoveWindow(hDlg, position.x, position.y, rt.right-rt.left, rt.bottom-rt.top, TRUE);

	grp = new CAstSig(fs);

	return hDlg;
}

void CSigProcDlg::OnCommand(int idc, HWND hwndCtl, UINT event)
{
	CStdString buf;
	switch(idc)
	{
	case IDC_VAR:
		if (event==EN_KILLFOCUS)
			if (GetDlgItemText(idc, buf)>0)	varstr = buf;
		break;
	case IDC_SIG:
		if (event==EN_KILLFOCUS)
			if (GetDlgItemText(idc, buf)>0)	sigstr = buf;
		break;
	}
}

SIGPROCDLG_API int CSigProcDlg::PrepareSignals()
{
	return (int)( PrepareDef() & PrepareSig());
}

bool CSigProcDlg::PrepareDef()
{
	try 
	{
		grp->SetNewScript(varstr.c_str());
		grp->Compute();
	} 
	catch (const char *errmsg) 
	{
		sformat(errstr, 512, "%s (error in variable def)", errmsg);
		return false;
	}
	return true;
}

bool CSigProcDlg::PrepareSig()
{
//	CAstSig ast(grp);
	try 
	{
		grp->SetNewScript(sigstr.c_str());
		grp->Compute();
	} 
	catch (const char *errmsg) 
	{
		sformat(errstr, 512, "%s (error in signal def)", errmsg);
		return false;
	}
	return true;
}

SIGPROCDLG_API int CSigProcDlg::SetVar(const char *varname, const CSignals &signal)
{
	grp->SetTag(varname, signal);
	return 1;
}

SIGPROCDLG_API int CSigProcDlg::SetVar(const char *strIn)
{
	char estr[256];
	try {
		grp->SetNewScript(strIn);
		grp->Compute();
		return 1;
	} catch (const char *errmsg) {
		sformat(errstr, 512, "%s (error processing-->%s)", errmsg, strIn);
		errstr = estr;
		return 0;
	}
}

SIGPROCDLG_API void CSigProcDlg::SetEditVar(const char *strIn)
{
	SetDlgItemText(IDC_VAR, strIn);
	varstr = strIn;
	UpdateWindow(hDlg);
}

SIGPROCDLG_API void CSigProcDlg::SetEditSig(const char *strIn)
{
	SetDlgItemText(IDC_SIG, strIn);
	sigstr = strIn;
	UpdateWindow(hDlg);
}

SIGPROCDLG_API void CSigProcDlg::EnableEdits(int onoff)
{
	EnableDlgItem(hDlg, IDC_VAR, onoff);
	EnableDlgItem(hDlg, IDC_SIG, onoff);
}