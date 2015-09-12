// _graphy.h : main header file for the GRAPHY DLL
//

#pragma once

#include "resource.h"		// main symbols
#include "PlotDlg.h"	
#include "sigproc.h"	
#include "supportFcn.h"

/////////////////////////////////////////////////////////////////////////////
// CGraphyApp
// See graphy.cpp for the implementation of this class
//

class CGraphyApp : public CWinApp
{
public:
	CPlotDlg **dlg;
	int closeFigure(int figId);
	CFigure* openChildFigure(CRect *rt, const CSignals &data, CWnd* pParent=NULL);
	CFigure* openChildFigure(CRect *rt, const CSignals &data);
	CFigure* openFigure(CRect *rt, const CString& caption, const CSignals &data, int devID);
	CFigure* openFigure(CRect *rt, const char* caption, const CSignals &data, int devID);
	CFigure* openFigure(CRect *rt, const CSignals &data, int devID);
	int ShowFigure(CFigure *hFig, int showCode);
	int nFigures;
	CGraphyApp();
	~CGraphyApp();

	DECLARE_MESSAGE_MAP()
};
