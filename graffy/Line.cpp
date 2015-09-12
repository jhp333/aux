#include "graffy.h"

CLine::CLine(CWndDlg * base, CGobj * pParent, int length)
: symbol('\0')
, lineWidth(1)
{
	m_dlg = base;
	hPar=pParent;
	color = RGB(0, 0, 200);
	len = length;
	xdata = new double[len];
	ydata = new double[len];
	strcpy(type, "line");
}

CLine::~CLine()
{
	delete[] xdata;
	delete[] ydata;
}
