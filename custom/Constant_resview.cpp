// Constant_resview.cpp : implementation file
//

#include "stdafx.h"
#include "psycon.h"
#include "Constant_resview.h"


// CConstant_resview dialog

IMPLEMENT_DYNAMIC(CConstant_resview, CDialog)

CConstant_resview::CConstant_resview(CWnd* pParent /*=NULL*/)
	: CDialog(CConstant_resview::IDD, pParent)
	, m_res_log(_T(""))
{

}

CConstant_resview::~CConstant_resview()
{
}

void CConstant_resview::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RESULT_LOG, m_const_log);
	DDX_Text(pDX, IDC_RESULT_LOG, m_res_log);
}


BEGIN_MESSAGE_MAP(CConstant_resview, CDialog)
END_MESSAGE_MAP()


// CConstant_resview message handlers
