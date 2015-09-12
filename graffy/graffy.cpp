// graffy.cpp : Defines the initialization routines for the DLL.
//


#include "PlotDlg.h"
#include "_graffy.h"
#include "msgCrack.h"
#include <string.h>

HINSTANCE hInst;

CGraffyDLL theApp; 
CPlotDlg* childfig;

GRAPHY_EXPORT HWND hPlotDlgCurrent;

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
	hInst = hModule;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

    return TRUE;
}

CPoint uglyfcn(int x, int y)
{
	CPoint out(x,y);
	return out;
}

#define THE_CPLOTDLG  static_cast <CPlotDlg*>(theApp.fig[id])

int getID4hDlg(HWND hDlg)
{
	int i;
	if (theApp.fig[theApp.nFigures-1]->hDlg==NULL) return theApp.nFigures-1;
	for (i=0; i<theApp.nFigures; i++)
	{
		if (hDlg==theApp.fig[i]->hDlg) 	return i; 
	}
	MessageBox (NULL, "<<<ASDJ", "", MB_OK);
	return 0;
}

BOOL CALLBACK DlgProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	int id = getID4hDlg(hDlg);
	switch (umsg)
	{
	case WM_INITDIALOG:
		break;
	chHANDLE_DLGMSG (hDlg, WM_PAINT,THE_CPLOTDLG->OnPaint);
	chHANDLE_DLGMSG (hDlg, WM_SIZE, THE_CPLOTDLG->OnSize);
	chHANDLE_DLGMSG (hDlg, WM_CLOSE, THE_CPLOTDLG->OnClose);
	chHANDLE_DLGMSG (hDlg, WM_COMMAND, THE_CPLOTDLG->OnCommand);
	chHANDLE_DLGMSG (hDlg, WM_RBUTTONUP, THE_CPLOTDLG->OnRButtonUp);
	chHANDLE_DLGMSG (hDlg, WM_LBUTTONUP, THE_CPLOTDLG->OnLButtonUp);
	chHANDLE_DLGMSG (hDlg, WM_LBUTTONDOWN, THE_CPLOTDLG->OnLButtonDown);
	chHANDLE_DLGMSG (hDlg, WM_MOUSEMOVE, THE_CPLOTDLG->OnMouseMove);
	chHANDLE_DLGMSG (hDlg, WM_KEYDOWN, THE_CPLOTDLG->OnKeyDown);
	chHANDLE_DLGMSG (hDlg, WM__SOUND_EVENT, THE_CPLOTDLG->OnSoundEvent);
	case WM_ACTIVATE:
		if (0 != wParam)             // becoming active
              hPlotDlgCurrent = hDlg;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CALLBACK ChildDlgProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	switch (umsg)
	{
	case WM_INITDIALOG:
		break;
	chHANDLE_DLGMSG (hDlg, WM_PAINT, childfig->OnPaint);
	chHANDLE_DLGMSG (hDlg, WM_COMMAND, childfig->OnCommand);
	default:
		return FALSE;
	}
	return TRUE;
}


GRAPHY_EXPORT HACCEL GetAccel(HANDLE hFig)
{
	for (int i=0; i<theApp.nFigures; i++)
		if (hFig==theApp.fig[i]->gcf)  return theApp.fig[i]->GetAccel();
	return NULL;
}

GRAPHY_EXPORT HWND GetHWND_PlotDlg(HANDLE hFig)
{
	for (int i=0; i<theApp.nFigures; i++)
		if (hFig==theApp.fig[i]->gcf)  return theApp.fig[i]->hDlg;
	return NULL;
}

GRAPHY_EXPORT HANDLE  OpenFigure(RECT *rt, const CSignals &data, int devID, double block)
{
	CRect Rt(*rt);
	return theApp.openFigure(&Rt, "", data, devID, block);
}

GRAPHY_EXPORT HANDLE OpenFigure(RECT *rt, const char* caption, const CSignals &data, int devID, double block)
{
	CRect Rt(*rt);
	return theApp.openFigure(&Rt, caption, data, devID, block);
}

GRAPHY_EXPORT HANDLE OpenFigure(CRect *rt, const CSignals &data, int devID, double block)
{
	return theApp.openFigure(rt, "", data, devID, block);
}

GRAPHY_EXPORT HANDLE  OpenFigure(CRect *rt, const char *caption, const CSignals &data, int devID, double block)
{
	return theApp.openFigure(rt, caption, data, devID, block);
}

GRAPHY_EXPORT HANDLE   OpenChildFigure(RECT *rt, const CSignals &data, HWND hParent)
{
	CRect Rt(*rt);
	return theApp.openChildFigure(&Rt, data, hParent);
}

GRAPHY_EXPORT HANDLE   OpenChildFigure(CRect *rt, const CSignals &data, HWND hParent)
{
	return theApp.openChildFigure(rt, data, hParent);
}

GRAPHY_EXPORT int CloseFigure(int id)
{
	return theApp.closeFigure(id);
}

GRAPHY_EXPORT HANDLE  AddAxis(HANDLE _fig, double x0, double y0, double wid, double hei)
{
	CFigure *fig = static_cast<CFigure *>(_fig);
	CPosition pos(x0, y0, wid, hei);
	CAxis * ax = fig->axes(pos);
	theApp.fig[theApp.nFigures-1]->gca = ax; 
	return ax;
}

GRAPHY_EXPORT HANDLE  AddText (HANDLE _fig, const char* text, double x0, double y0, double wid, double hei)
{
	CFigure *fig = static_cast<CFigure *>(_fig);
	CPosition pos(x0, y0, wid, hei);
	return fig->AddText(text, pos);
}

GRAPHY_EXPORT void SetRange(HANDLE _ax, const char xy, double x1, double x2)
{
	CAxis *ax = static_cast<CAxis *>(_ax);
	ax->setRange(xy, x1, x2);
}

GRAPHY_EXPORT void SetTick (HANDLE _ax, const char xy, double x1, double x2, int nDiv, int nSkips, char const *format, double coeff, double offset)
{
	CAxis *ax = static_cast<CAxis *>(_ax);
	ax->setTick(xy, x1, x2, nDiv, nSkips, format, coeff, offset);
}

GRAPHY_EXPORT HANDLE  PlotDouble(HANDLE _ax, int len, double *x, double *y, COLORREF col, char cymbol, int lineWidth)
{	
	CAxis *ax = static_cast<CAxis *>(_ax);
	if (y!=NULL)
		return ax->plot(len, x, y, col, cymbol, lineWidth);
	int i;
	CLine * out;
	double *z = new double[len];
	for (i=0; i<len; i++)
		z[i] = (double)i;
	out = ax->plot(len, z, x, col, cymbol, lineWidth);
	delete[] z;
	return out;
}

void _deleteObj (CFigure *hFig)
{
	int i, j, ind2Del;
	bool loop(true);
	for (i=0; i<theApp.nFigures && loop; i++)
		if (hFig==theApp.fig[i]->gcf) 
			{ind2Del=i; loop=false;}
	if (loop) return ; // If hFig is a ghost, do nothing.
	CPlotDlg **pFigs;
	pFigs = new CPlotDlg*[theApp.nFigures-1];
	//storing objects to keep
	for (i=0, j=0; i<theApp.nFigures; i++)
		if (i!=ind2Del) 
			pFigs[j++] = theApp.fig[i];
		else
			delete theApp.fig[i];
	delete hFig;
		
	delete[] theApp.fig;
	theApp.fig = pFigs;
	theApp.nFigures--;
}
							 
void _deleteObj (CAxis *hAx)
{
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

void _deleteObj (CLine *hLine)
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

void _deleteObj (CText *hText)
{
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

GRAPHY_EXPORT void deleteObj (HANDLE h)
{
	if (!strcmp(static_cast<CGobj*>(h)->type,"figure"))	{_deleteObj((CFigure *)h); return;}
	if (!strcmp(static_cast<CGobj*>(h)->type,"axis")) {_deleteObj((CAxis*)h); return;}
	if (!strcmp(static_cast<CGobj*>(h)->type,"line")) {_deleteObj((CLine *)h); return;}
	if (!strcmp(static_cast<CGobj*>(h)->type,"text")) {_deleteObj((CText *)h); return;}
}

