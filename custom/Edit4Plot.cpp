// Edit4Plot.cpp : implementation file
//

#include "stdafx.h"
#include "psycon.h"
#include "psyconDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEdit4Plot

CEdit4Plot::CEdit4Plot()
{
}

CEdit4Plot::~CEdit4Plot()
{
}


BEGIN_MESSAGE_MAP(CEdit4Plot, CEdit)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEdit4Plot message handlers

void CEdit4Plot::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CPsyconDlg *hPar = (CPsyconDlg*)GetParent();
	if (nRepCnt==1)
        {
		switch (nChar)
		{
		case VK_F5:
			hPar->Plot(GetDlgCtrlID());
			break;
		}
        }

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}
