#if !defined(AFX_EDIT4PLOT_H__EDA2BC21_B7B1_41A3_9249_82C1A14AA4BF__INCLUDED_)
#define AFX_EDIT4PLOT_H__EDA2BC21_B7B1_41A3_9249_82C1A14AA4BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Edit4Plot.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CEdit4Plot window

class CEdit4Plot : public CEdit
{
// Construction
public:
	CEdit4Plot();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdit4Plot)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEdit4Plot();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEdit4Plot)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDIT4PLOT_H__EDA2BC21_B7B1_41A3_9249_82C1A14AA4BF__INCLUDED_)
