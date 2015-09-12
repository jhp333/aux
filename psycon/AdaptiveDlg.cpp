// AdaptiveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "psycon.h"
#include "psyconDlg.h"
#include "AdaptiveDlg.h"

// CAdaptiveDlg dialog

IMPLEMENT_DYNAMIC(CAdaptiveDlg, CDialog)

CAdaptiveDlg::CAdaptiveDlg(CPsyconDlg* pParent /*=NULL*/)
	: CProcDialog(CAdaptiveDlg::IDD, pParent),
	hPar(pParent),
	m_crossing_limit(0),
	m_instruction("")
{
}

CAdaptiveDlg::~CAdaptiveDlg()
{
}

void CAdaptiveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INST, m_instruction);
}


BEGIN_MESSAGE_MAP(CAdaptiveDlg, CDialog)
	ON_EN_UPDATE(IDC_INIT_VAL, &CAdaptiveDlg::OnUpdateInitVal)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CAdaptiveDlg message handlers


#define CECI(Y,X,STR) {itemp=GetDlgItemInt(X,&check); if (!check) {	MessageBox (STR,se); return FALSE;} else Y = itemp;}
#define CECD(Y,X,STR) {dtemp=GetDlgItemDouble(m_hWnd,X,&check); if (!check) {	MessageBox (STR,se); return FALSE;} else Y = dtemp;}

BOOL CAdaptiveDlg::UpdateDataPlus(BOOL bSaveAndValidate)
{
	char buffer[128];
	CString s, se("Existing value will be used.");
	double out[2], xf[2], dtemp;
	int i, m[3], x[2], itemp, check;
	CButton *hRadio;

	if (!UpdateData(bSaveAndValidate)) return FALSE;

	if (!bSaveAndValidate)
	{
		sprintfFloat(adap.stepsize[0], 4, buffer, 128);
		s = buffer;
		SetDlgItemText (IDC_STEP1, s);
		sprintfFloat(adap.stepsize[1], 4, buffer, 128);
		s = buffer;
		SetDlgItemText (IDC_STEP2, s);
		SetDlgItemInt(IDC_REVERSALS1, adap.reversal[0]);
		SetDlgItemInt(IDC_REVERSALS2, adap.reversal[1]);
		SetDlgItemInt(IDC_DOWN, adap.down);
		SetDlgItemInt(IDC_UP, adap.up);
		SetDlgItemInt(IDC_TRIAL_LIMIT, adap.m_limit);
		sprintfFloat(adap.initialPt, 3, buffer, 128);
		s = buffer;
		SetDlgItemText (IDC_INIT_VAL, s);
		sprintfFloat(m_crossing_limit, 3, buffer, 128);
		s = buffer;
		SetDlgItemText (IDC_CROSSING_LIMIT, s);
		SetDlgItemInt(IDC_FINAL_REV_4AVG, adap.finalRev4Avg);
		if (adap.descending) hRadio = (CButton *)GetDlgItem(IDC_DESCENDING);
		else hRadio = (CButton *)GetDlgItem(IDC_ASCENDING);
		hRadio->SetCheck(1);
		hRadio = (CButton *)GetDlgItem(IDC_FACTOR);
		if (adap.factor)		hRadio->SetCheck(1);
		else			hRadio->SetCheck(0);
		OnUpdateInitVal();
	}
	else
	{
		hRadio = (CButton*)GetDlgItem(IDC_DESCENDING);
		adap.descending = (hRadio->GetCheck()) ? 1:0;

		//Validation
		s = "Stepsize Validation failed.";
		CECD(xf[0],IDC_STEP1, s)
		CECD(xf[1],IDC_STEP2, s)
		s = "reversal Validation failed.";
		CECI(x[0],IDC_REVERSALS1, s)
		CECI(x[1],IDC_REVERSALS2, s)
		s = "Number of interval invalid.";
		s = "Down invalid."; CECI(m[0],IDC_DOWN,s)
		s = "Up invalid."; CECI(m[1],IDC_UP,s)
		s = "Number of final reversals invalid.";
		CECI(m[2],IDC_FINAL_REV_4AVG,s)
		GetDlgItemText (IDC_INIT_VAL, buffer, sizeof(buffer));
		i=str2array(out, 2, buffer, " ,");
		if (i==0) 	{	MessageBox ("Invalid initial value","Existing value will be used."); return FALSE;}
		if (i==2) 
		{
			adap.dualSeries=1;
			adap2.initialPt = out[1];
		}
		adap.initialPt = out[0];
		hRadio = (CButton *)GetDlgItem(IDC_FACTOR);
		adap.factor = (hRadio->GetCheck()==BST_CHECKED); 

		//Update
		adap.down = m[0];
		adap.up = m[1];
		adap.finalRev4Avg = m[2];
		CECI(adap.m_limit,IDC_TRIAL_LIMIT,"Invalid session limit")
		dtemp = GetDlgItemDouble(m_hWnd, IDC_CROSSING_LIMIT, &check);
		if (check) m_crossing_limit = dtemp;
		memcpy((void*)adap.stepsize, (void*)xf, sizeof(xf));
		memcpy((void*)adap.reversal, (void*)x, sizeof(x));
		SyncAdap();
	}
	return TRUE;
}



BOOL CAdaptiveDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	UpdateDataPlus(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CAdaptiveDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message==WM_COMMAND)
	{
		if (wParam==IDCANCEL || wParam==IDOK)
		return NULL;
	}
	
	return CDialog::WindowProc(message, wParam, lParam);
}

void CAdaptiveDlg::CheckValuesUpdateADscending()
{
	CButton *hRadio;
//		if (sscanf(st, "%lf", &refVal)==1)
//			adap.descending = (adap.initialPt > refVal)? 1:0;
	if (adap.descending)
	{
		hRadio = (CButton *)GetDlgItem(IDC_ASCENDING);
		hRadio->SetCheck(0);
		hRadio = (CButton *)GetDlgItem(IDC_DESCENDING);
		hRadio->SetCheck(1);
	}
	else
	{
		hRadio = (CButton *)GetDlgItem(IDC_ASCENDING);
		hRadio->SetCheck(1);
		hRadio = (CButton *)GetDlgItem(IDC_DESCENDING);
		hRadio->SetCheck(0);
	}

}

void CAdaptiveDlg::OnUpdateInitVal() 
{
	int check;
	double val = GetDlgItemDouble(m_hWnd, IDC_INIT_VAL, &check);
	if (check)
	{
		adap.initialPt = val;
		CheckValuesUpdateADscending();
	}
}

void CAdaptiveDlg::SyncAdap()
{ /* The fields of adap and adap2 are the same except for initialPt and descending (and every correct-incorrect) */
	memcpy((void*)adap2.stepsize,(void*)adap.stepsize, sizeof(adap.stepsize));
	memcpy((void*)adap2.reversal,(void*)adap.reversal, sizeof(adap.reversal));
	adap2.down=adap.down;
	adap2.up=adap.up;
	adap2.finalRev4Avg=adap.finalRev4Avg;
	adap2.descending = !adap.descending;
}

HBRUSH CAdaptiveDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	static DWORD colorg;
	if (nCtlColor==CTLCOLOR_STATIC && pWnd->GetDlgCtrlID()==IDC_WARNING_STATIC)
		colorg = ::GetSysColor(COLOR_BTNFACE);

	if (adap.warning || adap2.warning)
	{
		if (nCtlColor==CTLCOLOR_EDIT && pWnd->GetDlgCtrlID()==IDC_CROSSING_LIMIT)
			pDC->SetBkColor(RGB(90, 200, 80));
		if (nCtlColor==CTLCOLOR_STATIC && pWnd->GetDlgCtrlID()==IDC_WARNING_STATIC)
			pDC->SetBkColor(RGB(90, 200, 80));
	}
	else
	{
		if (nCtlColor==CTLCOLOR_EDIT && pWnd==GetDlgItem(IDC_CROSSING_LIMIT))
			pDC->SetBkColor(colorg);
		if (nCtlColor==CTLCOLOR_STATIC && pWnd==GetDlgItem(IDC_WARNING_STATIC))
			pDC->SetBkColor(colorg);
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}
