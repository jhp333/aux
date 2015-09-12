// SetProcessorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "psynteg.h"
#include "psyntegDlg.h"
#include "SetProcessorDlg.h"


// CSetProcessorDlg dialog

IMPLEMENT_DYNAMIC(CSetProcessorDlg, CDialog)

CSetProcessorDlg::CSetProcessorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetProcessorDlg::IDD, pParent)
{

}

CSetProcessorDlg::~CSetProcessorDlg()
{
}

void CSetProcessorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetProcessorDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSetProcessorDlg::OnBnClickedOK)
	ON_BN_CLICKED(IDC_L34, &CSetProcessorDlg::OnBnClickedL34)
	ON_BN_CLICKED(IDC_SP12, &CSetProcessorDlg::OnBnClickedSP12)
	ON_BN_CLICKED(IDC_SPRINT, &CSetProcessorDlg::OnBnClickedSprint)
END_MESSAGE_MAP()


// CSetProcessorDlg message handlers

void CSetProcessorDlg::SetControls()
{
	char sprocessor[16], errstr[256];
	if (itype>=SPRINT_CPS) // nic1
	{
		EnableID(this, IDC_EMUL, 0);
		EnableID(this, IDC_CIC4, 0);
		EnableID(this, IDC_COMP_EDIT, 1);
		SendDlgItemMessage(IDC_SPRINT, BM_SETCHECK, BST_CHECKED);
		SendDlgItemMessage(IDC_L34, BM_SETCHECK, BST_UNCHECKED);
		SendDlgItemMessage(IDC_SP12, BM_SETCHECK, BST_UNCHECKED);
		SendDlgItemMessage(IDC_CIC3, BM_SETCHECK, BST_CHECKED);
		SendDlgItemMessage(IDC_CIC4, BM_SETCHECK, BST_UNCHECKED);
		SetDlgItemInt(IDC_COMP_EDIT, itype-SPRINT_CPS);
	}
	else if (itype==UNSPECIFIED) // error
	{
		strcpy(sprocessor, "L34-CIC3-0");
		if (!Registry(0, HKEY_LOCAL_MACHINE, REGISTRY_KEY4HW, REGISTRY_SUBKEY_4HW, "L34-CIC3-0", errstr))
			MessageBox (errstr, "Hardware info cannot be set. Application will crash and close.");
		::PostQuitMessage(0);
	}
	else // nic2
	{
		EnableID(this, IDC_CIC4, 1);
		EnableID(this, IDC_COMP_EDIT, 0);
		SendDlgItemMessage(IDC_SPRINT, BM_SETCHECK, BST_UNCHECKED);
		switch(itype)
		{
		case L34_CIC3:
		case L34_CIC4:
			SendDlgItemMessage(IDC_L34, BM_SETCHECK, BST_CHECKED);
			SendDlgItemMessage(IDC_SP12, BM_SETCHECK, BST_UNCHECKED);
			break;
		case SP12_CIC3:
		case SP12_CIC4:
			SendDlgItemMessage(IDC_L34, BM_SETCHECK, BST_UNCHECKED);
			SendDlgItemMessage(IDC_SP12, BM_SETCHECK, BST_CHECKED);
			break;
		}
		switch(itype)
		{
		case L34_CIC3:
		case SP12_CIC3:
			SendDlgItemMessage(IDC_CIC3, BM_SETCHECK, BST_CHECKED);
			SendDlgItemMessage(IDC_CIC4, BM_SETCHECK, BST_UNCHECKED);
			break;
		case L34_CIC4:
		case SP12_CIC4:
			SendDlgItemMessage(IDC_CIC3, BM_SETCHECK, BST_UNCHECKED);
			SendDlgItemMessage(IDC_CIC4, BM_SETCHECK, BST_CHECKED);
			break;
		}
		if (EmulMode) SendDlgItemMessage(IDC_EMUL, BM_SETCHECK, BST_CHECKED);
	}
}

int CSetProcessorDlg::GetControls()
{
	int ch, dum;
	if (SendDlgItemMessage(IDC_SPRINT, BM_GETCHECK) == BST_CHECKED)
	{
		dum = GetDlgItemInt(IDC_COMP_EDIT, &ch);
		if (!ch) 
		{
			MessageBox ("Com port number for PPS connection needs to be specified (for CPS, type 0)");
			return 0;
		}
		itype = (InterfaceType)((int)SPRINT_CPS + dum);
		EmulMode = false;
	}
	else if (SendDlgItemMessage(IDC_L34, BM_GETCHECK) == BST_CHECKED)
	{
		if (SendDlgItemMessage(IDC_CIC3, BM_GETCHECK) == BST_CHECKED)
			itype = L34_CIC3;
		else 
			itype = L34_CIC4;
		EmulMode = (SendDlgItemMessage(IDC_EMUL, BM_GETCHECK) == BST_CHECKED);
	}
	else if (SendDlgItemMessage(IDC_SP12, BM_GETCHECK) == BST_CHECKED)
	{
		if (SendDlgItemMessage(IDC_CIC3, BM_GETCHECK) == BST_CHECKED)
			itype = SP12_CIC3;
		else 
			itype = SP12_CIC4;
		EmulMode = (SendDlgItemMessage(IDC_EMUL, BM_GETCHECK) == BST_CHECKED);
	}
	else
	{
		MessageBox ("Invalid control setting");
		return 0;
	}
	return 1;
}

BOOL CSetProcessorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	char sprocessor[16], errstr[256];
	hPar = GetParent();

	Registry(1, HKEY_LOCAL_MACHINE, REGISTRY_KEY4HW, REGISTRY_SUBKEY_4HW, sprocessor, errstr);
	itype = ((CpsyntegDlg*)hPar)->string2itype(sprocessor, EmulMode);
	SetControls();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSetProcessorDlg::OnBnClickedOK()
{
	char sprocessor[16], errstr[256];
	if (GetControls())  
	{
		((CpsyntegDlg*)hPar)->itype2string(itype, EmulMode, sprocessor); 
		if (!Registry(0, HKEY_LOCAL_MACHINE, REGISTRY_KEY4HW, REGISTRY_SUBKEY_4HW, sprocessor, errstr))
			MessageBox ("Hardware setting update error");
		EndDialog(IDOK);
	}
}

int CSetProcessorDlg::CheckEMULOK()
{
	if (SendDlgItemMessage(IDC_EMUL, BM_GETCHECK) == BST_CHECKED &&
		SendDlgItemMessage(IDC_SPRINT, BM_GETCHECK) == BST_CHECKED)
	{
		MessageBox ("Emulation mode requires either L34 or SP12");
		return 0;
	}
	return 1;
}

void CSetProcessorDlg::OnBnClickedL34()
{
	EnableID(this, IDC_CIC4, 1);
	EnableID(this, IDC_COMP_EDIT, 0);
}

void CSetProcessorDlg::OnBnClickedSP12()
{
	EnableID(this, IDC_CIC4, 1);
	EnableID(this, IDC_COMP_EDIT, 0);
}

void CSetProcessorDlg::OnBnClickedSprint()
{
	EnableID(this, IDC_CIC4, 0);
	EnableID(this, IDC_COMP_EDIT, 1);
	SendDlgItemMessage(IDC_CIC3, BM_SETCHECK, BST_CHECKED);
	SendDlgItemMessage(IDC_CIC4, BM_SETCHECK, BST_UNCHECKED);
	CheckEMULOK();
}
