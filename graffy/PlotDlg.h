#include "graffy.h"	
#include "resource.h"	
#include "audfret.h"
#include "WndDlg.h"
#include "rfftw.h"
#include <math.h>
#include "supportFcn.h"


#define  WM__SOUND_EVENT  WM_APP+10

/////////////////////////////////////////////////////////////////////////////
// CPlotDlg dialog

class CPlotDlg : public CWndDlg
{
// Construction
public:
	POINT gcmp; //Current mouse point
	int GetIndicesInRange(CAxis* axx, int& id1, int& id2);
	CAxis * CurrentPoint2CurrentAxis (Win32xx::CPoint *point);
	CStdString errStr;
	CAxis * gca;
	CMenu * subMenu;
	CMenu menu;
	CSignals sig;
//	CWnd * hParent;
	int devID;
	double block;
	void DrawTicksFrom(CDC *pDC, CAxis *pax, char xy, int beginPt, int direction);
	void DrawTicks(CDC *pDC, CAxis *pax);
	CRect DrawAxis(CDC *pDC, CAxis *pax);
	CFigure *gcf;
	HACCEL GetAccel();
	CPlotDlg(const CSignals &data, HINSTANCE hInstance, CGobj *hPar = NULL);   // standard constructor
	//CPlotDlg();   // default constructor
	~CPlotDlg();
	int soundID;

private:
//	CStatic *m_statlevel;
	bool specView;
	bool levelView;
	bool playing;
	bool paused;
	CPosition lastPos;
	HACCEL hAccel;
	int GetSignalInRange(CSignals &signal, int sigOnly);
public:
	void OnPaint();
	void OnClose();
	void OnSize(UINT nType, int cx, int cy);
	void OnRButtonUp(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnMenu(UINT nId);
	void OnCommand(int idc, HWND hwndCtl, UINT event);
	void OnSoundEvent(int index, int code);
};
