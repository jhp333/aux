#pragma once

#include "AdaptiveDlg.h"
#include "SliderAdj.h"	
#include "ConstantStim.h"	
#include "CustomScript.h"	
#include "Edit4Plot.h"
#include "FIFO.h"
#include "tabber.h"
#include "afxwin.h"
#include "LogReport.h"

// CPsyconDlg dialog

class CPsyconDlg : public CDialog
{
	DECLARE_DYNAMIC(CPsyconDlg)

public:
	CToolTipCtrl  ToolTipCtrl;
	CMenu menu0;
	CStatusBarCtrl sbar;
	CFont SigEditFont;
	CAstSig AstSig;
	CAstSig sgen0, sgen1;
	CProcDialog *dlgs[4];
	CTabber m_tab;
	int ReadCalibrationFileUpdate(void);
	int fs;
	CSignals *intervalsig;
	void GetDefaultParameter(char *ItemName);
	int Prepare(int id=-1);
	CShortFIFO corr;
	CDoubleFIFO var;
	bool toggle;
	void PlotHistory(double *ydata);
	int AdapProceed(int corr);
	double varHistory[2][MAX_TRIALS];
	int saveDetails;
	CFigure* m_figProc;
	CAxis* m_axSpec[MAX_PLOTWND];
	CAxis* m_axWav[MAX_PLOTWND];
	CAxis* m_axProc;
	CFigure* m_figView[MAX_PLOTWND];
	CFigure* curFig;
	HWND hViewPlotDlg;
	HACCEL hAccel;
	BOOL m_bClose;
	CString* devCaptions;
	int destroyedWind;
	int nDev;
	int procPlay;
	int singlePlay;
	int m_freeze_rov;
	int stopped;
	void Plot(int idc);
	void Present(int playID);
	double sliderReference;
	CString instruction[2];
	int		trialDelay;
	int		intervalDelay;
	LogReport report;
	int playedItem;
	int SaveLogFile(CString& fname, CString& string2save);
	int nTrials;
	int nConstSteps;
	string tagIDs[2];
	double* scores;
	char* PresentingIDOrder;
	short* cumTrials;
	short* cumCorr;
	void DoneSession(CString buf2Print="", int doneyet=0);
	BOOL SendDll(CString dllname);
	double variable;
	double thresholdCand[2][MAX_TRIALS];
	int respHist[MAX_TRIALS];
	int corrHist[2][MAX_TRIALS];
	int interval;
	int trial;
	int answer;
	int taskID;
	int WriteINI(const char *fname);
	int _ReadINI(const char *fname);
	double dBperVoltick;
	CAdaptiveDlg* m_child1;
	CConstantStim* m_child2;
	CSliderAdj* m_child3;
	CCustomScript* m_child4;
	CString errstr;	
	CString trial_line_log;
	int		nIntervals;
	int		nIntLessAnc; //  Number of intervals not anchored
	bool	banchor[8]; // each item specifies whether this is for the anchor.. assuming no more than 8 intervals
	CString m_anchorStr;
	BOOL	feedback;
	bool	recalc;
	BOOL	ascontroller;
	CString m_controller_node;
	double	m_controller_level;
	CString m_udfpath;

	int PrepareTags();
	void SigFile(int read, LPCSTR fname);
	void SigFileOld(int read, LPCSTR fname);
	int MakeNextAnswer(int nInt);
	void UpdateText4SingleInt();
	CString m_strTagDef;
	CString m_strTagDef2;
	CPsyconDlg(CWnd* pParent = NULL);
	virtual ~CPsyconDlg();

// Dialog Data
	enum { IDD = IDD_PSYCON_DIALOG };
	CEdit4Plot	m_edit4plot2;
	CEdit4Plot	m_edit4plot1;
	CString	m_strRef;
	CString	m_strCont;
	CEdit	m_logEdit;

	virtual BOOL DestroyWindow();

protected:
	HICON m_hIcon;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	// Generated message map functions
	//{{AFX_MSG(CPsyconDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnUpdateTalk();
	afx_msg void OnChangeSignalCon();
	afx_msg void OnChangeSignalRef();
	afx_msg void OnPaint();
	afx_msg void OnFileOpenStim();
	afx_msg void OnFileSaveStim();
	afx_msg void OnFileSettings();
	afx_msg void OnFileExit();
	afx_msg void OnHelpWhatsNew();
	afx_msg void OnHelpAbout();
//	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg LRESULT OnFlyConnected(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnFlyClosed(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnFlyArrived(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnFlyErr(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnNext_Interval(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnSelChange(WPARAM wp, LPARAM lp=0);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedGo();
	afx_msg void OnBnClickedStop();
	afx_msg void OnEnChangeSignalTag();
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	CEdit m_edit_tags, m_edit_tags2;
	afx_msg void OnEnChangeNumIntervals();
};
