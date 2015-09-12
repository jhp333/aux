#if !defined(AFX_PLOTDLG_H__BA379D3D_5ACA_4CC5_89F9_F5168F9955E9__INCLUDED_)
#define AFX_PLOTDLG_H__BA379D3D_5ACA_4CC5_89F9_F5168F9955E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlotDlg.h : header file
//

#include "graphy.h"	
#include "resource.h"	
#include "audfret.h"
#include "sigproc.h"

/////////////////////////////////////////////////////////////////////////////
// CPlotDlg dialog

class CPlotDlg : public CDialog
{
// Construction
public:
	POINT gcmp; //Current mouse point
	int GetIndicesInRange(CAxis* axx, int& id1, int& id2);
	CAxis * CurrentPoint2CurrentAxis (CPoint *point);
	CString errStr;
	CAxis * gca;
	CMenu * subMenu;
	CMenu menu;
	CSignals sig;
#ifdef __AFXWIN_H__
	CWnd * hParent;
#endif
	int devID;
	void DrawTicksFrom(CDC *pDC, CAxis *pax, char xy, int beginPt, int direction);
	void DrawTicks(CDC *pDC, CAxis *pax);
	CRect DrawAxis(CDC *pDC, CAxis *pax);
	CFigure *gcf;
	HACCEL GetAccel();
	CPlotDlg(const CSignals &data, CWnd* pParent = NULL);   // standard constructor
	~CPlotDlg();

// Dialog Data
	//{{AFX_DATA(CPlotDlg)
	enum { IDD = IDD_PLOT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlotDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPlotDlg)
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg void OnMenu(UINT nId);
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
private:
	CStatic *m_statlevel;
	bool specView;
	bool levelView;
	CPosition lastPos;
	HACCEL hAccel;
	int GetSignalInRange(CSignals &signal, int sigOnly);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLOTDLG_H__BA379D3D_5ACA_4CC5_89F9_F5168F9955E9__INCLUDED_)
