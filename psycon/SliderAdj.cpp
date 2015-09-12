// SliderAdj.cpp : implementation file
//

#include "stdafx.h"
#include "psycon.h"
#include "SliderAdj.h"
#include "PsyconDlg.h"

// CSliderAdj dialog

//IMPLEMENT_DYNAMIC(CSliderAdj, CDialog)

CSliderAdj::CSliderAdj(CPsyconDlg* pParent /*=NULL*/)
	: CProcDialog(CSliderAdj::IDD, pParent),\
	hPar(pParent),
	m_instruction("")
{
}

CSliderAdj::~CSliderAdj()
{
}

void CSliderAdj::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INIT_VAL, initValStr);
	DDX_Text(pDX, IDC_STEP_BIG, m_step_big);
	DDX_Text(pDX, IDC_STEP_SMALL, m_step_small);
	DDX_Text(pDX, IDC_INST, m_instruction);
}


BEGIN_MESSAGE_MAP(CSliderAdj, CDialog)
END_MESSAGE_MAP()



LRESULT CSliderAdj::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CSliderAdj::UpdateDataPlus(BOOL bSaveAndValidate)
{
	CString str;
	CPsyconDlg* hpar = (CPsyconDlg*)hPar;
	UpdateData(bSaveAndValidate);

	if (m_step_big<m_step_small)
	{
		MessageBox ("Big step value should be greater than small step value");
		return FALSE;
	}


	if (!bSaveAndValidate)
	{
		//str.Format("%f", val);
		//SetDlgItemText(IDC_INIT_VAL, str);
	}
	else
	{
		if (initValStr.GetLength()>0)
		{
			try {
				CAstSig arg(initValStr, &hpar->AstSig);
				hpar->variable = arg.Compute().value();
			} catch (const char *errmsg) {
				MessageBox (errmsg);
			}
		}
	}
	return TRUE;
}