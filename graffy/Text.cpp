#include "graffy.h"

CText::CText(CWndDlg * base, CGobj* pParent, const char *strInit, CPosition posInit)
{
	m_dlg = base;
	strcpy(type, "text");
	fontsize=15;
	strcpy(fontname,"Arial");
	strcpy(string, strInit);
	pos = posInit;
	hPar = pParent;
	color = RGB(0, 0, 200);
//	hDlg = pParent->hDlg;
	font.CreateFont(fontsize, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, fontname);
}

CText::~CText() 
{
	font.DeleteObject();
}

HFONT CText::SetFont(LPCTSTR fontName, int fontSize)
{
	font.DeleteObject();
	HFONT res = font.CreateFont(fontSize, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
	OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, fontName);
	return res;
}
