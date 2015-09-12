#include <math.h>
//#include "sigproc.h"  // included in graffy.h
//
//
#include "graffy.h"	
#include "supportFcn.h"



using namespace Win32xx;

CAxis::CAxis(CWndDlg * base, CGobj* pParent /*=NULL*/)
{
	m_dlg = base;
	strcpy(type, "axis");
	colorBack = RGB(200, 210, 200);
	colorAx = 0;
	nLines=0;
	m_ln = new CLine*[nLines];
	pos.x0=0.;
	pos.y0=0.;
	pos.width=0.;
	pos.height=0.;
	hPar=pParent;
	xlim[0]=0.;
	ylim[0]=0.;
	xlim[1] = xlim[0]-1.; //reverse the large-small, so indicate uninitialized for lim
	ylim[1] = ylim[0]-1.; //reverse the large-small, so indicate uninitialized for lim
	xtick.m_dlg = base;
	ytick.m_dlg = base;
}

CAxis::~CAxis()
{
	for (int i=0; i<nLines; i++)
		delete m_ln[i];
	delete[] m_ln;
}
void CAxis::GetCoordinate(POINT* pt, double& x, double& y)
{
	int ix = pt->x - rcAx.left;
	int iy = rcAx.bottom - pt->y;
	double width = xlim[1]-xlim[0];
	double height = ylim[1]-ylim[0];
	x = ix*width/rcAx.Width() + xlim[0];
	y = iy*height/rcAx.Height() + ylim[0];
}

POINT CAxis::GetPixelAbs(double x, double y)
{
	POINT pt;
	pt.x = rcAx.left + GetPixelRel(x-xlim[0],'x');
	pt.y = rcAx.bottom - GetPixelRel(y-ylim[0],'y');
	return pt;
}

int CAxis::GetPixelAbs(double a, char xy)
{
	// rcAx field, xlim, ylim must have been prepared prior to this call.
	switch(xy)
	{
	case 'x':
		return rcAx.left + GetPixelRel(a-xlim[0],xy);
	case 'y':
		return rcAx.bottom - GetPixelRel(a-ylim[0],xy);
	default:
		return -9999;
	}
}

int CAxis::GetPixelRel(double a, char xy)
{
	// rcAx field, xlim, ylim must have been prepared prior to this call.
	double rangeWidth, relativeVal;
	int axWidth;
	switch(xy)
	{
	case 'x':
		rangeWidth = xlim[1]-xlim[0];
		relativeVal = (a) / rangeWidth;
		axWidth = rcAx.right-rcAx.left;
		break;
	case 'y':
		rangeWidth = ylim[1]-ylim[0];
		relativeVal = (a) / rangeWidth;
		axWidth = rcAx.bottom-rcAx.top;
		break;
	default:
		return -9999;
	}
	return (int)((double)axWidth*relativeVal+.5);
}

AUDFRET_EXP void CAxis::DeleteLine(int index)
{
	int j=0, i=0;
	int res=1;
	if (index==-1)
	{
		for (i=0; i<nLines; i++)
			delete  m_ln[i];
		delete  m_ln;
		nLines=0;
		m_ln = new CLine*[nLines];
		return;
	}
	if (nLines==0)		return;
	if (index>nLines-1) return;
	delete  m_ln[index];
	CLine** tempLine;
	tempLine = new CLine*[nLines-1];
	for (i=0; i<nLines; i++)
	{
		if (i!=index)		tempLine[j++] = m_ln[i];
	}
	delete  m_ln;
	m_ln  = tempLine;
	nLines--;
	if (nLines==0)
	{
		xlim[1] = xlim[0]-1.;
		ylim[1] = ylim[0]-1.;
	}
}

void CAxis::setRangeFromLines(char xy)
{
	double miin, maax;
	int i;
	if (nLines < 1)
		return;

	if (xy=='x')
	{
		for (i=0; i<nLines; i++)
		{
			miin = getMin (m_ln[i]->len, m_ln[i]->xdata);
			maax = getMax (m_ln[i]->len, m_ln[i]->xdata);
			if (miin==maax)
			{
				xlim[0] = miin-.005;
				xlim[1] = miin+.005;
			}
			else
			{
				xlim[0] = miin;
				xlim[1] = maax;
			}
		}
	}
	else if (xy=='y')
	{
		miin = getMin(m_ln[0]->len, m_ln[0]->ydata);
		maax = getMax(m_ln[0]->len, m_ln[0]->ydata);
		for (i=1; i<nLines; i++) {
			miin = min(miin, getMin(m_ln[i]->len, m_ln[i]->ydata));
			maax = max(maax, getMax(m_ln[i]->len, m_ln[i]->ydata));
		}
		if (abs(maax+miin) < (maax-miin) * 0.5) {
			// center on zero.
			maax = max(abs(maax), abs(miin));
			miin = -maax;
		}
		if (miin==maax) {
			ylim[0] = miin-.005;
			ylim[1] = miin+.005;
		} else {
			ylim[0] = miin - .05*(maax-miin);
			ylim[1] = maax + .05*(maax-miin);
		}
		if (ytick.a2 < ytick.a1) {	// if not initialized, go automatic.
			if (miin==maax) {
				// display the value on y axis.
				ytick.a1 = miin;
				ytick.a2 = miin + 1;
			} else {
				double atom = pow(10.0,floor(log10(maax-miin))-2);
				miin = round(miin/atom/4)*atom*4;
				maax = round(maax/atom/4)*atom*4;
				ytick.a1 = miin;
				ytick.a2 = miin + (maax-miin)/4;
			}
		}
	}
	
}

AUDFRET_EXP void CAxis::setRange(const char xy, double x1, double x2)
{
	if (x2<x1) return;
	if (xy=='x' || xy=='X')
	{
		xlim[0] = x1;
		xlim[1] = x2;
	}
	else if (xy=='y' || xy=='Y')
	{
		ylim[0] = x1;
		ylim[1] = x2;
	}
	
}

AUDFRET_EXP void CAxis::setTick(const char xy, double const x1, double const x2, int const nDiv, int const nSkips, char const *format, double const coeff, double const offset)
{
	if (xy=='x' || xy=='X')
	{
		xtick.a1=x1;
		xtick.a2=x2;
		xtick.nDiv=nDiv;
		xtick.nLabelSkip=nSkips;
		if (format!=NULL)
			strcpy(xtick.format,format);
		xtick.mult=coeff;
		xtick.offset=offset;
	}
	else if (xy=='y' || xy=='Y')
	{
		ytick.a1=x1;
		ytick.a2=x2;
		ytick.nDiv=nDiv;
		ytick.nLabelSkip=nSkips;
		if (format!=NULL)
			strcpy(xtick.format,format);
		ytick.mult=coeff;
		ytick.offset=offset;
	}
	
}


void CAxis::setPos(CPosition &posIn)
{
	pos = posIn;
	
}

void CAxis::setPos(double x0, double y0, double width, double height)
{
	pos.x0 = x0;
	pos.y0 = y0;
	pos.width = width;
	pos.height  = height;
	
}

AUDFRET_EXP CLine * CAxis::plot(int length, double *y, COLORREF col, char cymbol, int lineWidth)
{
	int i;
	CLine * out;
	double *x = new double[length];
	for (i=0; i<length; i++)
		x[i] = (double)i;
	out = plot(length, x, y, col, cymbol, lineWidth);
	delete[] x;
	return out;
}

AUDFRET_EXP CLine * CAxis::plot(int length, double *x, double *y, COLORREF col, char cymbol, int lineWidth)
{
	int i;
	double maax(-1.e100), miin(1.e100);
	CLine** tempLine;
	tempLine = new CLine*[nLines+1];
	for (i=0; i<nLines; i++)
		tempLine[i] = m_ln[i];
	tempLine[nLines]=new CLine(m_dlg, this, length);
	delete[]  m_ln;
	m_ln  = tempLine;
	memcpy((void*)m_ln[nLines]->xdata,(void*)x, sizeof(double)*length);
	memcpy((void*)m_ln[nLines]->ydata,(void*)y, sizeof(double)*length);
	m_ln[nLines]->len = length;
	if (cymbol!=0)
		m_ln[nLines]->symbol = cymbol;
	if (col!=-1)
		m_ln[nLines]->color = col;
	if (lineWidth!=-1)
		m_ln[nLines]->lineWidth = lineWidth;
	// parent is Figure object, whose parent is root object whose member is m_dlg
	m_dlg->InvalidateRect(NULL);
	nLines++;
	for (i=0; i<nLines; i++)
		miin = min(miin, getMin(m_ln[i]->len, m_ln[i]->xdata));
	for (i=0; i<nLines; i++)
		maax = max(maax, getMax(m_ln[i]->len, m_ln[i]->xdata));
	xlimFull[0] = miin; xlimFull[1] = maax; 
	for (i=0; i<nLines; i++)
		miin = min(miin, getMin(m_ln[i]->len, m_ln[i]->ydata));
	for (i=0; i<nLines; i++)
		maax = max(maax, getMax(m_ln[i]->len, m_ln[i]->ydata));
	ylimFull[0] = miin; ylimFull[1] = maax; 
	return m_ln[nLines-1];//This is the line object that was just created.
}

void CAxis::GetRange(double *x, double *y)
{
	memcpy((void*)x,(void*)xlim, 2*sizeof(double));
	memcpy((void*)y,(void*)ylim, 2*sizeof(double));
}