#pragma once

#include "sigproc.h"

//#include <Gdiplusinit.h>




#ifdef GRAFFY_INTERNAL //====================== GRAFFY_INTERNAL
#include "wincore.h"
#include "winutils.h"
#define GRAPHY_EXPORT __declspec (dllexport)
#else
#define GRAPHY_EXPORT __declspec (dllimport)
#endif                //====================== GRAFFY_INTERNAL

#include "_graffy2.h"

#define GRAPHY_REPLACE_SAME_KEY		(DWORD)0x0000
#define GRAPHY_REPLACE_NO_KEY		(DWORD)0x0001
#define GRAPHY_RETURN_ERR_SAME_KEY	(DWORD)0x0002

GRAPHY_EXPORT HANDLE  OpenFigure(RECT *rt, const CSignals &data, int devID=0, double block=200.);
GRAPHY_EXPORT HANDLE  OpenFigure(RECT *rt, const char* caption, const CSignals &data, int devID=0, double block=200.);
GRAPHY_EXPORT HANDLE  OpenChildFigure(RECT *rt, const CSignals &data, HWND hParent=NULL);

#ifdef _WIN32XX_WINCORE_H_
#define NO_USING_NAMESPACE
GRAPHY_EXPORT HANDLE  OpenFigure(CRect *rt, const ::CSignals &data, int devID=0);
GRAPHY_EXPORT HANDLE  OpenFigure(CRect *rt, const char* caption, const CSignals &data, int devID=0);
GRAPHY_EXPORT HANDLE  OpenChildFigure(CRect *rt, const ::CSignals &data, HWND hParent=NULL);
#endif 

GRAPHY_EXPORT HANDLE	AddAxis(HANDLE fig, double x0, double y0, double wid, double hei);
GRAPHY_EXPORT HANDLE	AddText (HANDLE fig, const char* text, double x, double y, double wid, double hei);
GRAPHY_EXPORT HANDLE	PlotDouble(HANDLE ax, int len, double *x, double *y=NULL, COLORREF col=0xff0000, char cymbol=0, int lineWidth=1);
GRAPHY_EXPORT void		SetRange(HANDLE ax, const char xy, double x1, double x2);
GRAPHY_EXPORT void		SetTick(HANDLE ax, const char xy, double x1, double x2, int nDiv=0, int nSkips=0, char const *format=NULL, double coeff=1., double offset=0.);

GRAPHY_EXPORT HACCEL GetAccel(HANDLE hFig);
GRAPHY_EXPORT HWND GetHWND_PlotDlg(HANDLE hFig);

GRAPHY_EXPORT void deleteObj (HANDLE h);



/*
GRAPHY_EXPORT CFigure* OpenFigure(RECT *rt, const CSignals &data, int devID=0);
GRAPHY_EXPORT CFigure* OpenFigure(RECT *rt, const char* caption, const CSignals &data, int devID=0);
GRAPHY_EXPORT int ShowFigure(CFigure *hFig, int showCode);
GRAPHY_EXPORT int closeFigure(int figId);


template <class T>
GRAPHY_EXPORT void deleteObj (T *hObj)
{
	int i, j, ind2Del;
	CAxis *hParent = (CAxis *) hLine->hPar;
	for (i=0; i<hParent->nLines; i++)
		if (hLine==hParent->m_ln[i]) {ind2Del=i; break;}
	if (i==hParent->nLines) return ; // If hLine is a ghost, do nothing.
	CLine **pnew;
	pnew = new CLine*[hParent->nLines-1];
	//storing objects to keep
	for (i=0, j=0; i<hParent->nLines; i++)
		if (i!=ind2Del) pnew[j++] = hParent->m_ln[i];
	delete hLine;
	delete[] hParent->m_ln;
	hParent->m_ln = pnew;
	hParent->nLines--;
}

*/