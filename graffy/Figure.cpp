#include "graffy.h"	
#include "supportFcn.h"

#include "WndDlg.h"

CGobj::CGobj()
:m_dlg(NULL)
{
	strcpy(type, "root");
}

CGobj::~CGobj()
{

}

CPosition::CPosition()
{

}

CPosition::CPosition(double x, double y, double w, double h)
:x0(x), y0(y), width(w), height(h)
{
}

CFigure::CFigure(CWndDlg * base, CGobj* pParent)
{
	m_dlg = base;
	hPar = pParent;
	color = RGB(230, 230, 210);
	nAxes=0;
	nTexts=0;
	ax = new CAxis*[nAxes];
	text = new CText*[nTexts];
	strcpy(type, "figure");
}

CFigure::~CFigure()
{
	for (int i=0; i<nAxes; i++)
		delete ax[i];
	delete[] ax;
	for (int i=0; i<nTexts; i++)
		delete text[i];
	delete[] text;
}

CAxis *CFigure::axes(CPosition pos)
{
	CAxis **pax;
	pax = new CAxis*[nAxes+1];
	for (int i=0; i<nAxes; i++)
		pax[i] = ax[i];
	delete[] ax;
	pax[nAxes] = new CAxis(m_dlg, this);
	ax = pax;
	pax[nAxes]->setPos(pos);
	return pax[nAxes++];
}

CAxis *CFigure::axes(double x0, double y0, double width, double height)
{
	CAxis **pax;
	pax = new CAxis*[nAxes+1];
	for (int i=0; i<nAxes; i++)
		pax[i] = ax[i];
	delete[] ax;
	pax[nAxes] = new CAxis(m_dlg, this);
	ax = pax;
	pax[nAxes]->setPos(x0, y0, width, height);
	return pax[nAxes++];
}

void CFigure::DeleteAxis(int index)
{
	CAxis **pax;
	nAxes--;
	pax = new CAxis*[nAxes];
	for (int i=0; i<index; i++)
		pax[i] = ax[i];
	for (int i=index+1; i<nAxes; i++)
		pax[i-1] = ax[i];
	delete[] ax;
	ax = pax;
}

CText *CFigure::AddText(const char* string, CPosition pos)
{
	CText **ptxt;
	ptxt = new CText*[nTexts+1];
	for (int i=0; i<nTexts; i++)
		ptxt[i] = text[i];
	delete[] text;
	ptxt[nTexts] = new CText(m_dlg, this, string, pos);
	text = ptxt;
	return ptxt[nTexts++];
}
