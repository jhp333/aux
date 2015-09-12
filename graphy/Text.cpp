#include "stdafx.h"
#include "_graphy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CText::CText(CGobj* pParent, const char *strInit, CPosition posInit)
{
	strcpy(type, "text");
	fontsize=15;
	strcpy(fontname,"Arial");
	strcpy(string, strInit);
	pos = posInit;
	hPar = pParent;
	color = RGB(0, 0, 200);
	hDlg = pParent->hDlg;
	font.CreateFont(fontsize, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, fontname);
}

CText::~CText() 
{
	font.DeleteObject();
}

int CText::SetFont(LPCTSTR fontName, int fontSize)
{
	font.DeleteObject();
	BOOL res = font.CreateFont(fontSize, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
	OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, fontName);
	return res;
}
