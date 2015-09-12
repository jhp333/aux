#include "stdafx.h"
#include "_graphy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTick::CTick()
{
	strcpy(type, "tick");
	gap4next.x = 5;
	gap4next.y = 2;
	nDiv=0;
	nLabelSkip=0;
	format[0]='\0';
	mult=1.;
	offset=0.;
	labelPos=7;
	size=4;
	a1=0.;
	a2 = a1-1.; //whatever a1 is, a2 is smaller than a1, indicating an uninitialized object.
	font.CreateFont(15, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
}

CTick::~CTick()
{
	font.DeleteObject(); 
}
