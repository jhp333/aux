// Eview.cpp : implementation file
//

#include "stdafx.h"
#include "psycon.h"
#include "Eview.h"
#include "rfftw.h"
#include <math.h>


double getMax(int len, double *x)
{
	double maax=-1.e100;
	for (int i=0; i<len; i++)
		if (x[i]>maax)
			maax = x[i];
	return maax;
}

double getMean(int len, double *x)
{
	double sum(0.);
	for (int i=0; i<len; i++) sum += x[i];
	return sum/len;
}

// CEview dialog

IMPLEMENT_DYNAMIC(CEview, CDialog)

CEview::CEview()
	: CDialog(CEview::IDD)
{
}

CEview::~CEview()
{
	delete[] hFig;
}

void CEview::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEview, CDialog)
//	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_GENERATE, &CEview::OnBnClickedPlot)
	ON_EN_CHANGE(IDC_EXPRESSION, &CEview::OnEnChangeExpression)
END_MESSAGE_MAP()


// CEview message handlers

BOOL CEview::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	// Draws the waveform and spectrum view child windows
	CRect rt;
	rt.SetRect(0, 250, 787, 575);
	hFig = OpenChildFigure(&rt,this);
	hAx[0] = AddAxis(hFig, .05, .57, .9, .4);
	hAx[1] = AddAxis(hFig, .05, .07, .9, .4);

	SetDlgItemText(IDC_EDIT_PLOT, "Click here to plot the signal");
	hAccel = GetAccel(curFig = hFig);
	hExpression = GetDlgItem(IDC_EXPRESSION)->m_hWnd;
	hButt = GetDlgItem(IDC_GENERATE)->m_hWnd;
	hViewPlotDlg = GetHWND_PlotDlg(curFig);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEview::OnBnClickedPlot()
{
	CEqSig exo;
	CString str;
	string estr;
	static CString expressionStrKept("");

	GetDlgItemText(IDC_EXPRESSION,str);
	if (str==expressionStrKept) return;
	expressionStrKept = str;

	exo.setstr(str);
	if(!exo.Make(0, NULL, estr))  // no tags are used for this
	{
		MessageBox (estr.c_str());
		return; 
	}

	static CLine *h1, *h2;
	rfftw_plan plan;
	double *x, *fft, *mag, *freq, fs;
	if (h1!=NULL)  deleteObj(h1); 
	if (h2!=NULL)  deleteObj(h2);
	int len = exo.sig.GetLength();
	x = new double[len];
	fs = (double)exo.sig.GetFs();
	for (int i=0; i<len; i++)	x[i] = (double)i/fs;

	h1 = hAx[0]->plot(len, x, exo.sig.buf);
	SetRange(hAx[0], 'x', 0, x[len-1]);
	hAx[0]->setRange('y', -1, 1);
	hAx[0]->setTick('x', 0, x[len-1]/10, 1, 0, "%6.3f");
	hAx[0]->setTick('y', 0, 1, 4);
	delete[] x;

	if (exo.sig.Max()>.001)
	{
		freq = new double[len/2];
		fft = new double[len];
		mag = new double[len/2];
		for (int i=0; i<len/2; i++)	freq[i]=(double)i/(double)len*fs;
		plan = rfftw_create_plan(len, FFTW_FORWARD, FFTW_ESTIMATE|FFTW_OUT_OF_PLACE);
		rfftw_one(plan, exo.sig.buf, fft);
		rfftw_destroy_plan(plan);
		for (int i=0; i<len/2; i++) 	mag[i] = 20.*log10(fabs(fft[len-i]));
		double maxmag = getMax(len/2,mag);
		double maxx = 5.*(maxmag/5.+1);
		for (int i=0; i<len/2; i++)	mag[i] -= maxx;
		double ylim = 10.*(maxmag/10.+1)-maxx;
		h2 = hAx[1]->plot(len/2, freq, mag, RGB(230, 150, 100));
		SetRange(hAx[1], 'x', 0, fs/2);
		SetTick(hAx[1], 'x', 0, 1000, 0, 0, "%2.0lfk", 0.001);
		SetRange(hAx[1], 'y', getMean(len/2,mag)-40, ylim);
		SetTick(hAx[1], 'y', 0, 10);
		delete[] freq;
		delete[] fft;
		delete[] mag;
	}
	HWND  h = ::SetFocus(hViewPlotDlg);
	EnableDlgItem(m_hWnd, IDC_GENERATE, 0);
}

LRESULT CEview::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CEview::PreTranslateMessage(MSG* pMsg)
{
	// Do not process the Figure accelarator if the current message is for the edit box
	int j;
	if (pMsg->message==0x0100)
		j=0;
	if (pMsg->hwnd==hExpression || pMsg->hwnd==hButt || !::TranslateAccelerator(pMsg->hwnd, hAccel, pMsg))
		return CDialog::PreTranslateMessage(pMsg);
	else
	{
		return 1;
	}
	
}

void CEview::OnEnChangeExpression()
{
	EnableDlgItem(m_hWnd, IDC_GENERATE, 1);
}
