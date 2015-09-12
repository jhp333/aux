#include "WndDlg.h"

//If this library is linked statically define LINK_STATIC_GRAFFY in the application project.
#ifndef LINK_STATIC_GRAFFY
#define AUDFRET_EXP __declspec (dllexport)
#else 
#define AUDFRET_EXP 
#endif


// CFigure objects are instantiated ("new'ed") during instantiation of CPlotDlg
// CAxis objects are instantiated by CFigure::axes() 

// CFigure, CAxis objects have the proper m_dlg member variable during their instantiation.

class CPosition
{
public:
	//all the values in this class is scaled by 1 ("normalized" as in Matlab)
	double x0;
	double y0;
	double width;
	double height;
	CPosition(double x, double y, double w, double h);
	CPosition();
};

class CGobj
{
public:
	COLORREF color;
	CPosition pos;
	CGobj *hPar;
	CWndDlg *m_dlg;
	char type[16];
	int nChildren;
	CGobj();
	virtual ~CGobj();
};

class CTick : public CGobj
{
public:
	POINT gap4next;
	int size; // in pixel
	int labelPos; // in pixel
	double a1;
	double a2;
	int nDiv;
	int nLabelSkip;
	char format[32];
	double mult;
	double offset;

#ifdef _WIN32XX_WINCORE_H_
	CFont font;
#endif 
	CTick(CWndDlg * base=NULL);
	virtual ~CTick();
};

class CLine : public CGobj
{
public:
	double *xdata, *ydata;
	int len;
	char symbol;
	int lineWidth;
	CLine(CWndDlg * base, CGobj* pParent = NULL, int len=0);   // standard constructor
	virtual ~CLine();   
};

class CText : public CGobj
{
public:

	int fontsize; // in pixel
	char string[512];
	char fontname[64];
#ifdef _WIN32XX_WINCORE_H_
	CRect textRect; // in Client coordiate
	CFont font;
#endif 
	HFONT SetFont(LPCTSTR fontName, int fontSize=15);
	CText(CWndDlg * base, CGobj* pParent = NULL, const char* strInit=NULL, CPosition posInit=CPosition(0,0,0,0));   // standard constructor
	virtual ~CText();   
};

class CAxis : public CGobj
{
public:
	COLORREF colorBack;
	COLORREF colorAx;
	CPosition pos;
	double xlim[2], ylim[2];
	double xlimFull[2], ylimFull[2];
	CTick xtick; // How do I pass the argument for instantiation of these objects?
	CTick ytick; // CTick xtick(this) didn't work..... Maybe during instantiation of CAxis set xtick.m_dlg = this...
	CLine** m_ln;
	int nLines;
	void GetCoordinate(POINT* pt, double& x, double& y);
	AUDFRET_EXP CLine * plot(int length, double *y, COLORREF col=0xff0000, char cymbol=0, int lineWidth=1);
	AUDFRET_EXP CLine * plot(int length, double *x, double *y, COLORREF col=0xff0000, char cymbol=0, int lineWidth=1);
	void GetRange(double *x, double *y);
	AUDFRET_EXP void DeleteLine(int index);
	void setRangeFromLines(char xy);
	POINT GetPixelAbs(double x, double y);
	int GetPixelAbs(double a, char xy);
	int GetPixelRel(double a, char xy);
	AUDFRET_EXP void setRange(const char xy, double x1, double x2);
	AUDFRET_EXP void setTick(const char xy, double const x1, double const x2, int const nDiv=1, int const nSkips=0, char const *format=NULL, double const coeff=1., double const offset=0.);
	void setPos(CPosition &posIn);
	void setPos(double x0, double y0, double width, double height);
	CAxis(CWndDlg * base, CGobj* pParent = NULL);   // standard constructor
	virtual ~CAxis();
#ifdef _WIN32XX_WINCORE_H_
	CRect axRect;
	CRect rcAx;
#endif 
};

class CFigure : public CGobj
{
public:
	void DeleteAxis(int index);
	CText **text;
	int nTexts;
	CAxis **ax;
	int nAxes;
	CText *AddText(const char* string, CPosition pos);
	CAxis *axes(double x0, double y0, double width, double height);
	CAxis *axes(CPosition pos);
	CFigure(CWndDlg * base, CGobj* pParent = NULL);   // standard constructor
	virtual ~CFigure();
};

