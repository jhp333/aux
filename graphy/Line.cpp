#include "stdafx.h"
#include "_graphy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLine::CLine(CGobj * pParent, int length)
: symbol('\0')
, lineWidth(1)
{
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
