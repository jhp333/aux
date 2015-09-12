// ViewVector.cpp : implementation file
//

#include "stdafx.h"
#include "psycon.h"
#include "ViewVector.h"


// CViewVector dialog

IMPLEMENT_DYNAMIC(CViewVector, CDialog)

CViewVector::CViewVector(CWnd* pParent /*=NULL*/)
	: CDialog(CViewVector::IDD, pParent)
{

}

CViewVector::~CViewVector()
{
}

void CViewVector::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CViewVector, CDialog)
END_MESSAGE_MAP()


// CViewVector message handlers
