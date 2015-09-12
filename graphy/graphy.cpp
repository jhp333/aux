// graphy.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "_graphy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraphyApp

BEGIN_MESSAGE_MAP(CGraphyApp, CWinApp)
	//{{AFX_MSG_MAP(CGraphyApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphyApp construction

CGraphyApp::CGraphyApp()
{
	nFigures=0;
	dlg = new CPlotDlg*[nFigures];
}

CGraphyApp::~CGraphyApp()
{
	int i;
	for (i=0; i<nFigures; i++)
		delete dlg[i];
	delete[] dlg;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGraphyApp object

CGraphyApp theApp;

GRAPHY_EXPORT HACCEL GetAccel(CFigure* hFig)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	for (int i=0; i<theApp.nFigures; i++)
		if (hFig==theApp.dlg[i]->gcf)  return theApp.dlg[i]->GetAccel();
	return NULL;
}

GRAPHY_EXPORT HWND GetHWND_PlotDlg(CFigure* hFig)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	for (int i=0; i<theApp.nFigures; i++)
		if (hFig==theApp.dlg[i]->gcf)  return theApp.dlg[i]->m_hWnd;
	return NULL;
}

GRAPHY_EXPORT CFigure* OpenFigure(RECT *rt, const CSignals &data, int devID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CRect Rt(*rt);
	return theApp.openFigure(&Rt, "", data, devID);
}

GRAPHY_EXPORT CFigure* OpenFigure(CRect *rt, const CSignals &data, int devID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	return theApp.openFigure(rt, "", data, devID);
}

GRAPHY_EXPORT CFigure* OpenFigure(RECT *rt, const char* caption, const CSignals &data, int devID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CRect Rt(*rt);
	return theApp.openFigure(&Rt, caption, data, devID);
}

GRAPHY_EXPORT CFigure* OpenFigure(CRect *rt, const char *caption, const CSignals &data, int devID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	return theApp.openFigure(rt, caption, data, devID);
}

GRAPHY_EXPORT CFigure* OpenChildFigure(RECT *rt, const CSignals &data)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CRect Rt(*rt);
	return theApp.openChildFigure(&Rt, data, NULL);
}

GRAPHY_EXPORT CFigure* OpenChildFigure(CRect *rt, const CSignals &data, CWnd* pParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	return theApp.openChildFigure(rt, data, pParent);
}

GRAPHY_EXPORT int ShowFigure(CFigure *hFig, int showCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	return theApp.ShowFigure(hFig, showCode);
}

GRAPHY_EXPORT int CloseFigure(int id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	return theApp.closeFigure(id);
}

GRAPHY_EXPORT CAxis * AddAxis(CFigure* fig, double x0, double y0, double wid, double hei)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CPosition pos(x0, y0, wid, hei);
	CAxis * ax = fig->axes(pos);
	theApp.dlg[theApp.nFigures-1]->gca = ax;
	return ax;
}

GRAPHY_EXPORT void SetRange(CAxis *ax, const char xy, double x1, double x2)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	ax->setRange(xy, x1, x2);
}

GRAPHY_EXPORT void SetTick (CAxis *ax, const char xy, double x1, double x2, int nDiv, int nSkips, char const *format, double coeff, double offset)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	ax->setTick(xy, x1, x2, nDiv, nSkips, format, coeff, offset);
}

GRAPHY_EXPORT CLine * PlotDouble(CAxis *ax, int len, double *x, double *y, COLORREF col, char cymbol, int lineWidth)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if (y!=NULL)
		return ax->plot(len, x, y, col, cymbol, lineWidth);
	int i;
	CLine * out;
	double *z = new double[len];
	for (i=0; i<len; i++)
		z[i] = (double)i;
	out = ax->plot(len, z, y, col, cymbol, lineWidth);
	delete[] z;
	return out;
}

GRAPHY_EXPORT void deleteObj (CFigure *hFig)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	int i, j, ind2Del;
	bool loop(true);
	for (i=0; i<theApp.nFigures && loop; i++)
		if (hFig==theApp.dlg[i]->gcf) 
			{ind2Del=i; loop=false;}
	if (loop) return ; // If hFig is a ghost, do nothing.
	CPlotDlg **pFigs;
	pFigs = new CPlotDlg*[theApp.nFigures-1];
	//storing objects to keep
	for (i=0, j=0; i<theApp.nFigures; i++)
		if (i!=ind2Del) 
			pFigs[j++] = theApp.dlg[i];
		else
			delete theApp.dlg[i];
	delete hFig;
		
	delete[] theApp.dlg;
	theApp.dlg = pFigs;
	theApp.nFigures--;
}
							 
GRAPHY_EXPORT void deleteObj (CAxis *hAx)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	int i, j, ind2Del;
	CFigure *hParent = (CFigure *) hAx->hPar;
	for (i=0; i<hParent->nAxes; i++)
		if (hAx==hParent->ax[i]) {ind2Del=i; break;}
	if (i==hParent->nAxes) return ; // If hAx is a ghost, do nothing.
	CAxis **pnew;
	pnew = new CAxis*[hParent->nAxes-1];
	//storing objects to keep
	for (i=0, j=0; i<hParent->nAxes; i++)
		if (i!=ind2Del) pnew[j++] = hParent->ax[i];
	delete hAx;
	delete[] hParent->ax;
	hParent->ax = pnew;
	hParent->nAxes--;
}

GRAPHY_EXPORT void deleteObj (CLine *hLine)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
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

GRAPHY_EXPORT void deleteObj (CText *hText)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	int i, j, ind2Del;
	CFigure *hParent = (CFigure *) hText->hPar;
	for (i=0; i<hParent->nTexts; i++)
		if (hText==hParent->text[i]) {ind2Del=i; i=hParent->nTexts+1;}
	if (i==hParent->nTexts) return ; // If hText is a ghost, do nothing.
	CText **pnew;
	pnew = new CText*[hParent->nTexts-1];
	//storing objects to keep
	for (i=0, j=0; i<hParent->nTexts; i++)
		if (i!=ind2Del) pnew[j++] = hParent->text[i];
	delete hText;
	delete[] hParent->text;
	hParent->text = pnew;
	hParent->nTexts--;
}


CFigure* CGraphyApp::openFigure(CRect *rt, const CSignals &data, int devID)
{
	CString s="";
	return openFigure(rt, s, data, devID);
}

CFigure* CGraphyApp::openFigure(CRect *rt, const char* caption, const CSignals &data, int nDev)
{
	CString s(caption);
	return openFigure(rt, s, data, nDev);
}

CFigure* CGraphyApp::openFigure(CRect *rt, const CString& caption, const CSignals &data, int devID)
{
	int i;
	CPlotDlg **pdlg;
	CString s;
	pdlg = new CPlotDlg*[nFigures+1];
	for (i=0; i<nFigures; i++)
		pdlg[i] = dlg[i];
	if (dlg) delete[] dlg;
	pdlg[nFigures] = new CPlotDlg(data);
	pdlg[nFigures]->Create(IDD_PLOT);
	pdlg[nFigures]->devID = devID;
	dlg = pdlg;
	pdlg[nFigures]->MoveWindow(rt);
	if (caption=="")
	{
		s.Format("Figure %d", nFigures+1);
		pdlg[nFigures]->SetWindowText(s);
	}
	else
		pdlg[nFigures]->SetWindowText(caption);
	return pdlg[nFigures++]->gcf;
}

int CGraphyApp::ShowFigure(CFigure *hFig, int showCode)
{
	return hFig->Show(showCode);
}

CFigure* CGraphyApp::openChildFigure(CRect *rt, const CSignals &data)
{
	return openChildFigure(rt, data, NULL);
}

CFigure* CGraphyApp::openChildFigure(CRect *rt, const CSignals &data, CWnd* pParent)
{ 
	int i;
	CPlotDlg **pdlg;
	pdlg = new CPlotDlg*[nFigures+1];
	for (i=0; i<nFigures; i++)
		pdlg[i] = dlg[i];
	delete[] dlg;
	pdlg[nFigures] = new CPlotDlg(data);
	i=pdlg[nFigures]->Create(IDD_PLOT_CHILD, pParent);
	dlg = pdlg;
	pdlg[nFigures]->MoveWindow(rt);
	return pdlg[nFigures++]->gcf;
}

int CGraphyApp::closeFigure(int figId)
{
	// figId begins with 1.
	int i;
	if (figId > nFigures || figId < 0)
		return 0;

	if (figId==0)
	{
		for (i=0; i<nFigures; i++)
			dlg[i]->CWnd::DestroyWindow();
		delete[] dlg;
	}
	else
	{
		CPlotDlg **pdlg;
		pdlg = new CPlotDlg*[nFigures-1];
		for (i=0; i<figId-2; i++)
			pdlg[i] = dlg[i];
		for (i=figId; i<nFigures; i++)
			pdlg[i] = dlg[i];
		dlg[figId-1]->CWnd::DestroyWindow();
		delete[] dlg;
	}
	return 1;
}



