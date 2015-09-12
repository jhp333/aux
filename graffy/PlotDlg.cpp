// PlotDlg.cpp : implementation file
//

#include "PlotDlg.h"
#include "FileDlg.h"

void sprintfFloat(double f, int max_fp, CStdString& strOut);

#define IDM_SPECTRUM_INTERNAL	2222
#define IDC_LEVEL			7000
#define IDC_STATIC_LEVEL	7001
#define N_SOUND_PLAY_CALLBACK_MSGS 10

CPlotDlg::CPlotDlg(const CSignals &data, HINSTANCE hInstance, CGobj *hPar)
:levelView(false), specView(false), playing(false), paused(false), devID(0), soundID(-1)
{
	sig = data;
	gcf = new CFigure(this, hPar);
	menu.LoadMenu(IDR_POPMENU);
	subMenu = menu.GetSubMenu(0);
	gcmp=CPoint(-1,-1);
	hInst = hInstance;
	hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATOR));
}

CPlotDlg::~CPlotDlg()
{
}

void CPlotDlg::OnClose()
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CPlotDlg message handlers

HACCEL CPlotDlg::GetAccel()
{
	return hAccel;
}

void CPlotDlg::OnCommand(int idc, HWND hwndCtl, UINT event)
{
	switch (idc)
	{
	case IDM_FULLVIEW:
	case IDM_ZOOM_IN:
	case IDM_ZOOM_OUT:
	case IDM_SCROLL_LEFT:
	case IDM_SCROLL_RIGHT:
	case IDM_LEFT_STEP:
	case IDM_RIGHT_STEP:
	case IDM_PLAY:
	case IDM_SPECTRUM:
	case IDM_WAVWRITE:
		OnMenu(idc);
	}
}


void CPlotDlg::OnPaint() 
{
	PAINTSTRUCT  ps;
	if (hDlg==NULL) return;
	HDC hdc = BeginPaint(hDlg, &ps);
	if (hdc==NULL) return;
	CDC dc(hdc, hDlg);
	int radius = 5;
	CPen **pen, **txtPen;
	CRect circleRt;
	CPoint pt;
	CAxis *ax;
	Win32xx::CRect rt;
	double x,y;
	CStdString s;
	CSignals signal;

	GetClientRect(hDlg, &rt);
	if (rt.Height()<15) return;
	dc.SolidFill(gcf->color, rt);

	for (int j=0; j<gcf->nAxes; j++)
	{
		ax = gcf->ax[j];
		if (ax->xlim[1]<ax->xlim[0])
			ax->setRangeFromLines('x');
		if (ax->ylim[1]<ax->ylim[0])
			ax->setRangeFromLines('y');
		ax->rcAx=DrawAxis(&dc,ax);
		pen = new CPen*[ax->nLines];
		for (int i=0; i<ax->nLines; i++) {
			pen[i] = new CPen;
			pen[i]->CreatePen(PS_SOLID, 1, ax->m_ln[i]->color);
			dc.SelectObject(pen[i]);
			if (ax->m_ln[i]->symbol!=0)
			{
				for (int k=0; k<ax->m_ln[i]->len; k++) {
					pt = ax->GetPixelAbs(ax->m_ln[i]->xdata[k],ax->m_ln[i]->ydata[k]);
					if(IsInsideRect(&ax->rcAx, &pt))
					{
						circleRt.SetRect(pt+CPoint(-radius,-radius), pt+CPoint(radius,radius));
						dc.Ellipse(circleRt);
					}
				}
			}
			if (ax->m_ln[i]->lineWidth>0)
			{
				bool fLineStarted = false;
				for (int k=0; k<ax->m_ln[i]->len; k++) {
					for (int l=0; l<2; ++l) {	// to retry at the edge
						if (l==0)
							pt = ax->GetPixelAbs(ax->m_ln[i]->xdata[k],ax->m_ln[i]->ydata[k]);
						else if (ax->ylim[1] - ax->m_ln[i]->ydata[k] < ax->m_ln[i]->ydata[k] - ax->ylim[0]) 
							pt = ax->GetPixelAbs(ax->m_ln[i]->xdata[k],ax->ylim[1]);	// top edge
						else
							pt = ax->GetPixelAbs(ax->m_ln[i]->xdata[k],ax->ylim[0]);	// bottom edge

						if (IsInsideRect(&ax->rcAx, &pt))
						{
							if (fLineStarted)
								dc.LineTo(pt);
							else {
								dc.MoveTo(pt);
								fLineStarted = true;
							}
							break;
						}
					}
				}
			}
			pen[i]->DeleteObject();
			delete pen[i];
		}
		// add ticks and ticklabels
		DrawTicks(&dc, ax);
		delete[] pen;
	}

	//Drawing texts
	dc.SetTextAlign(TA_LEFT);
	txtPen = new CPen*[gcf->nTexts];
	for (int j=0; j<gcf->nTexts; j++) {
		txtPen[j] = new CPen;
		txtPen[j]->CreatePen(PS_SOLID, 1, gcf->text[j]->color);
		dc.SelectObject(txtPen[j]);
		dc.SelectObject(&gcf->text[j]->font);
		pt.x = (int)((double)rt.right * gcf->text[j]->pos.x0+.5);
		pt.y = (int)((double)rt.bottom * gcf->text[j]->pos.y0+.5);
		dc.TextOut(pt.x, pt.y, gcf->text[j]->string);
		txtPen[j]->DeleteObject();
		delete txtPen[j];
	}
	delete[] txtPen;

	// Drawing coordiate
	if (gcmp.x!=-1 &&gcmp.y!=-1 && gca->nLines>0)
	{
		if(!GetSignalInRange(signal,0))		{	return;	}
		gca->GetCoordinate(&gcmp, x, y);
		s.Format("%f, %f %7.2fdBrms", x,y, signal.LevelLT());
		dc.TextOut(gca->axRect.right-200, gca->axRect.top-10, s);
	}

	//Sound play buffer done (each block)
	if (soundID>-1)
	{
 		s.Format("%d", (int)((double)soundID*block));
		dc.TextOut(gca->axRect.right-200, gca->axRect.top+30, s);
	}


	EndPaint(hDlg, &ps);
}


void CPlotDlg::DrawTicksFrom(CDC *pDC, CAxis *pax, char xy, int beginPt, int direction)
{
	HDC hdc = HDC(*pDC);
	double value, ibt/*intervalBetweenTicks*/;
	int pt2Draw(beginPt), lastpt2Draw, res(1), cum(0);
	LOGFONT fnt;
	CSize sz;
	CStdString label;
	switch(xy)
	{
	case 'x':
		ibt = (pax->xtick.a2 - pax->xtick.a1) / (pax->xtick.nDiv+1);
		if (direction)	lastpt2Draw=-32767;
		else			lastpt2Draw=32768;
		while (res)
		{
			if (LRrange(&pax->rcAx, pt2Draw, 'x')==0)
			{
				pDC->MoveTo(pt2Draw, pax->rcAx.bottom);
				pDC->LineTo(pt2Draw, pax->rcAx.bottom - pax->xtick.size);
				value = pax->xtick.mult*(pax->xtick.a1+cum*ibt)+pax->xtick.offset;
				if (pax->xtick.format[0]!=0)
					label.Format(pax->xtick.format, value);
				else
					sprintfFloat(value, 3, label);
				GetTextExtentPoint32(hdc, label.c_str(), label.length(), &sz);
				if (iabs(pt2Draw-lastpt2Draw)> sz.cx + pax->xtick.gap4next.x) // only if there's enough gap, Textout
				{
					pDC->TextOut(pt2Draw, pax->rcAx.bottom + pax->xtick.labelPos, label);
					lastpt2Draw = pt2Draw;
				}
				if (direction)	cum++;
				else			cum--;
		
				pt2Draw = pax->GetPixelAbs(pax->xtick.a1 + cum*ibt, 'x');
				res = (LRrange(&pax->rcAx, pt2Draw, 'x')==0);
			}
			else // This must anticipate the within-range... (e.g., when going left, it should be left of the range...)
			{
				if (direction)	cum++;
				else			cum--;
				pt2Draw = pax->GetPixelAbs(pax->xtick.a1 + cum*ibt, 'x');
			} 
		}
		break; 
	case 'y':
		fnt = pax->ytick.font.GetLogFont(); // chech before and after here
		ibt = (pax->ytick.a2 - pax->ytick.a1) / (pax->ytick.nDiv+1);
		if (direction)	lastpt2Draw=-32767;
		else			lastpt2Draw=32768;
		while (res)
		{
			if (LRrange(&pax->rcAx, pt2Draw, 'y')==0)
			{
				pDC->MoveTo(pax->rcAx.left, pt2Draw);
				pDC->LineTo(pax->rcAx.left + pax->ytick.size, pt2Draw);
				value = pax->ytick.mult*(pax->ytick.a1+cum*ibt)+pax->ytick.offset;
				if (pax->ytick.format[0]!=0)
					label.Format(pax->ytick.format, value);
				else
					sprintfFloat(value, 3, label);
				GetTextExtentPoint32(hdc, label.c_str(), label.length(), &sz);
				if (iabs(pt2Draw-lastpt2Draw)> sz.cy + pax->xtick.gap4next.y) // only if there's enough gap, Textout
				{
					pDC->TextOut(pax->rcAx.left - pax->ytick.labelPos, pt2Draw-fnt.lfHeight/2, label);
					lastpt2Draw = pt2Draw;
				}
				if (direction)	cum++;
				else			cum--;
				pt2Draw = pax->GetPixelAbs(pax->ytick.a1+cum*ibt, 'y');
				res = (LRrange(&pax->rcAx, pt2Draw, 'y')==0);
			}
			else // This must anticipate the within-range... (e.g., when going left, it should be left of the range...)
			{
				if (direction)	cum++;
				else			cum--;
				pt2Draw = pax->GetPixelAbs(pax->ytick.a1+cum*ibt, 'y');
			}
		}
		break;
	}
}

void CPlotDlg::DrawTicks(CDC *pDC, CAxis *pax)
{
	CPen pen;
	int loc;
	char xy;

	pen.CreatePen(PS_SOLID, 1, RGB(0,0,0)/*pax->colorAx*/);
	pDC->SelectObject(&pax->xtick.font);
	pDC->SelectObject(&pen);
	pDC->SetTextAlign (TA_CENTER);
	//x
	if (pax->xtick.a2 > pax->xtick.a1 && pax->xtick.nDiv>=0) // if initialized
	{
		xy='x';
		loc = pax->GetPixelAbs(pax->xtick.a1, xy);
		if (LRrange(&pax->rcAx, loc, xy)==-1) 
			DrawTicksFrom(pDC, pax, xy, loc, GO_RIGHT);
		else if (LRrange(&pax->rcAx, loc, xy)==1) 
			DrawTicksFrom(pDC, pax, xy, loc, GO_LEFT);
		else
		{
			DrawTicksFrom(pDC, pax, xy, loc, GO_RIGHT);
			DrawTicksFrom(pDC, pax, xy, loc, GO_LEFT);
		}
	}
	//y
	pDC->SetTextAlign (TA_RIGHT);
	if (pax->ytick.a2 > pax->ytick.a1 && pax->ytick.nDiv>=0) // if initialized
	{
		xy='y';
		loc = pax->GetPixelAbs(pax->ytick.a1, xy);
		if (LRrange(&pax->rcAx, loc, xy)==-1) 
			DrawTicksFrom(pDC, pax, xy, loc, GO_RIGHT);
		else if (LRrange(&pax->rcAx, loc, xy)==1) 
			DrawTicksFrom(pDC, pax, xy, loc, GO_LEFT);
		else
		{
			DrawTicksFrom(pDC, pax, xy, loc, GO_RIGHT);
			DrawTicksFrom(pDC, pax, xy, loc, GO_LEFT);
		}
	}
}

CRect CPlotDlg::DrawAxis(CDC *pDC, CAxis *pax)
{
	int width, height, axHeight, axWidth;
	CPen pen;
	COLORREF orgCol;
	POINT org;
	CRect rt;
	GetClientRect(hDlg, &rt);
	pen.CreatePen(PS_SOLID, 1, pax->colorAx);
	pDC->SelectObject(&pen);
	width = (rt.right-rt.left);
	height= rt.bottom-rt.top;
	org.x = (int)((double)rt.left + (double)width*pax->pos.x0+.5);
	org.y = (int)((double)rt.bottom - (double)height*pax->pos.y0+.5);
	axWidth = (int)((double)width*pax->pos.width+.5);
	axHeight = (int)((double)height*pax->pos.height+.5);
	CRect rt2(org.x, org.y-axHeight, org.x+axWidth, org.y);
	orgCol = pDC->GetBkColor();
	CRect rctAx(org, CPoint(org.x+axWidth, org.y-axHeight));
	pDC->SolidFill(gca->colorBack, rctAx);
	pDC->MoveTo(org);
	pDC->LineTo(org.x, org.y-axHeight);
	pDC->LineTo(org.x+axWidth, org.y-axHeight);
	pDC->LineTo(org.x+axWidth, org.y);
	pDC->LineTo(org);
	pDC->SetBkColor(orgCol);
	pax->axRect = rt2;
	return rt2;
}

void CPlotDlg::OnSize(UINT nType, int cx, int cy) 
{
	// TODO: Add your message handler code here
	InvalidateRect	(NULL);
}
//Convention: if a figure handle has two axes, the first axis is the waveform viewer, the second one is for spectrum viewing.

void CPlotDlg::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CAxis *hax = CurrentPoint2CurrentAxis(&point);
	if (hax!=NULL)
	{
		int iSel(-1);
		for (int i=0 ;i<gcf->nAxes; i++) if (hax==gcf->ax[i]) iSel=i;
		//Following the convention
		subMenu->EnableMenuItem(IDM_PLAY, iSel==0? MF_ENABLED : MF_GRAYED);
		subMenu->EnableMenuItem(IDM_WAVWRITE, iSel==0? MF_ENABLED : MF_GRAYED);
		subMenu->EnableMenuItem(IDM_SPECTRUM, iSel==0? MF_ENABLED : MF_GRAYED);
		ClientToScreen(hDlg, &point);    // To convert point to re: the whole screen 
		TrackPopupMenu(subMenu->GetHandle(), TPM_RIGHTBUTTON, point.x, point.y, 0, hDlg, 0);
	}
}

void CPlotDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnMenu(IDM_PLAY);
}

void CPlotDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (CurrentPoint2CurrentAxis(&point)!=NULL)
	{
		gcmp=point;
		CRect rt(CPoint(gca->axRect.right-200, gca->axRect.top-10), CSize(200,15));
		InvalidateRect(&rt);
	}
}

void CPlotDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	gcmp=CPoint(-1,-1);

	RECT *rt= new RECT[gcf->nAxes];
	for (int i=0; i<gcf->nAxes; i++)
	{
		rt[i] = CRect (CPoint(gcf->ax[i]->axRect.right-200, gcf->ax[i]->axRect.top-10), CSize(200,15));
		InvalidateRect(&rt[i]);
	}
	delete[] rt;
}

void CPlotDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	
//	CDialog::OnMouseMove(nFlags, point);
}

//

void CPlotDlg::OnSoundEvent(int index, int code)
{
	soundID = index;
	CRect rt(CPoint(gca->axRect.right-200, gca->axRect.top+30), CSize(200,15));
	InvalidateRect(&rt);
	if (code>1) playing = false; // when done, code is the total number of samples played.

}

void CPlotDlg::OnMenu(UINT nID)
{
	static char fullfname[MAX_PATH];
	static CAxis *axSpec(NULL);
	CAxis *localax(gcf->ax[0]); // Following the convention
	CRect rt;
	CSize sz;
	CFont editFont;
	CPosition pos;
	double range, miin(1.e15), maax(-1.e15);
	double width, newmin, newmax;
	int i, len, id1, id2, iSel(-1);
	char errstr[256];
	CSignals signal;
	bool multi;
	static void  *playPoint;
	switch (nID)
	{
	case IDM_FULLVIEW:
		localax->setRange('x',localax->m_ln[0]->xdata[0], localax->m_ln[0]->xdata[localax->m_ln[0]->len-1]);
		localax->setTick('x', 0, localax->m_ln[0]->xdata[localax->m_ln[0]->len-1]/10, 1, 0, "%6.3f");
		// 2/3/2011, commented out by jhpark, because y axis is not affected by zooming.
		//localax->setRange('y', -1, 1);
		OnMenu(IDM_SPECTRUM_INTERNAL);
		return;

	case IDM_ZOOM_IN:
		width = localax->xlim[1] - localax->xlim[0];
		if (width<0.005) return;
		localax->setRange('x', localax->xlim[0] + width/4., localax->xlim[1] - width/4.);
		width = localax->xtick.a2 - localax->xtick.a1;
		localax->xtick.a2 -= width/4.;
		localax->xtick.a1 += width/4.;
		OnMenu(IDM_SPECTRUM_INTERNAL);
		return;

	case IDM_ZOOM_OUT:
		if (localax->xlim[1]==localax->xlimFull[1] && localax->xlim[0]==localax->xlimFull[0]) return;
		for (i=0; i<localax->nLines; i++)
			miin = min(miin, getMin(localax->m_ln[i]->len, localax->m_ln[i]->xdata));
		for (i=0; i<localax->nLines; i++)
			maax = max(maax, getMax(localax->m_ln[i]->len, localax->m_ln[i]->xdata));
		width = localax->xlim[1] - localax->xlim[0];
		localax->setRange('x', max(miin,localax->xlim[0] - width/2.), min(maax,localax->xlim[1] + width/2.));
		width = localax->xtick.a2 - localax->xtick.a1;
		//Oh, I just realized that a1 and a2 do not need to be within the range==> no need to check, it will still draw the ticks only within the range.
		localax->xtick.a2 += width/2.;
		localax->xtick.a1 -= width/2.;
		if (localax->xtick.a1<0) localax->xtick.a1=0;
		OnMenu(IDM_SPECTRUM_INTERNAL);
		return;
	case IDM_SCROLL_LEFT:
		for (i=0; i<localax->nLines; i++)
			miin = min(miin, getMin(localax->m_ln[i]->len, localax->m_ln[i]->xdata));
		range = localax->xlim[1] - localax->xlim[0];
		newmin = max(miin, localax->xlim[0]-range);
		localax->setRange('x', newmin, newmin+range);
		rt = localax->axRect;
		rt.InflateRect(5,0,5,30);
		InvalidateRect(&rt);
		OnMenu(IDM_SPECTRUM_INTERNAL);
		return;
	case IDM_SCROLL_RIGHT:
		for (i=0; i<localax->nLines; i++)
			maax = max(maax, getMax(localax->m_ln[i]->len, localax->m_ln[i]->xdata));
		range = localax->xlim[1] - localax->xlim[0];
		newmax = min(maax, localax->xlim[1]+range);
		localax->setRange('x', newmax-range, newmax);
		rt = localax->axRect;
		rt.InflateRect(5,0,5,30);
		InvalidateRect(&rt);
		OnMenu(IDM_SPECTRUM_INTERNAL);
		return;
	case IDM_LEFT_STEP:
		for (i=0; i<localax->nLines; i++)
			miin = min(miin, getMin(localax->m_ln[i]->len, localax->m_ln[i]->xdata));
		range = localax->xlim[1] - localax->xlim[0];
		newmin = max(miin, localax->xlim[0]-range/4.);
		localax->setRange('x', newmin, newmin+range);
		rt = localax->axRect;
		rt.InflateRect(5,0,5,30);
		InvalidateRect(&rt);
		OnMenu(IDM_SPECTRUM_INTERNAL);
		return;
	case IDM_RIGHT_STEP:
		for (i=0; i<localax->nLines; i++)
			maax = max(maax, getMax(localax->m_ln[i]->len, localax->m_ln[i]->xdata));
		range = localax->xlim[1] - localax->xlim[0];
		newmax = min(maax, localax->xlim[1]+range/4.);
		localax->setRange('x', newmax-range, newmax);
		rt = localax->axRect;
		rt.InflateRect(5,0,5,30);
		InvalidateRect(&rt);
		OnMenu(IDM_SPECTRUM_INTERNAL);
		return;
	case IDM_PLAY:
		errstr[0]=0;
		if (!playing)
			if(!GetSignalInRange(signal,1))		MessageBox (errStr);
			else 
			{
				playPoint = signal.PlayArray(devID, WM__SOUND_EVENT, hDlg, &block, errstr);
				playing = true;
			}
		else
		{
			PauseResumePlay(playPoint, paused);
			playing = paused;
			if (paused) paused = false;
		}
		return;
	case IDM_PAUSE:
		errstr[0]=0;
		if (playing)
		{
			PauseResumePlay(playPoint, false);
			paused = true;
		}
		return;
	case IDM_SPECTRUM:
		// To draw a spectrum, first re-adjust the aspect ratio of the client window 
		// so that the width is at least 1.5 times the height.
		if (!specView)
		{
			GetWindowRect(&rt);
			sz = rt.Size();
			if (sz.cx<3*sz.cy/2)
			{
				rt.InflateRect(0, 0, 3*sz.cy/2, 0);
				MoveWindow(&rt, 0);
			}
			//Again, //Assuming that the first axis is the waveform viewer, the second one is for spectrum viewing
			lastPos = gcf->ax[0]->pos;
			gcf->ax[0]->setPos(.08, .1, .62, .8);
			axSpec = gcf->axes(.75, .3, .22, .4);
			axSpec->colorBack = RGB(230, 230, 190);
			OnMenu(IDM_SPECTRUM_INTERNAL);
		}
		else
		{
			deleteObj(axSpec);
			axSpec=NULL;
			gcf->ax[0]->setPos(lastPos);
		}
		specView = !specView;
		break;
	case IDM_SPECTRUM_INTERNAL:
		rfftw_plan plan;
		double *freq, *fft, *mag, *fft2, *mag2, fs, maxx, maxmag, ylim;
		multi = localax->nLines>1 ? true : false;
		if (gcf->nAxes==1) break;
		fs = (double)sig.GetFs();
		for (; gcf->ax[1]->nLines>0;)	
			deleteObj(gcf->ax[1]->m_ln[0]);
		GetIndicesInRange(localax, id1, id2);
		len = id2-id1+1;
		freq = new double[len];
		fft = new double[len];
		mag = new double[len/2];
		for (i=0; i<len; i++)
			freq[i]=(double)i/(double)len*fs;
		plan = rfftw_create_plan(len, FFTW_FORWARD, FFTW_ESTIMATE|FFTW_OUT_OF_PLACE);
		rfftw_one(plan, &localax->m_ln[0]->ydata[id1], fft);
		if (multi) 
		{
			fft2 = new double[len];
			mag2 = new double[len/2];
			rfftw_one(plan, &localax->m_ln[1]->ydata[id1], fft2);
		}
		rfftw_destroy_plan(plan);
		for (i=0; i<len/2; i++)		mag[i] = 20.*log10(fabs(fft[len-i]));
		if (multi) for (i=0; i<len/2; i++)	mag2[i] = 20.*log10(fabs(fft2[len-i]));
		maxmag = getMax(len/2,mag);
		maxx = 5.*(maxmag/5.+1);
		for (int j=0; j<len/2; j++)	mag[j] -= maxx;
		if (multi) 
		{
			maxmag = getMax(len/2,mag2);
			maxx = 5.*(maxmag/5.+1);
			for (int j=0; j<len/2; j++)	mag2[j] -= maxx;
		}
		ylim = 10.*(maxmag/10.+1)-maxx;
		PlotDouble(gcf->ax[1], len/2, freq, mag);
		SetRange(gcf->ax[1], 'x', 0, fs/2);
		SetTick(gcf->ax[1], 'x', 0, 1000, 0, 0, "%2.0lfk", 0.001);
		SetRange(gcf->ax[1], 'y', getMean(len/2,mag)-40, ylim);
		SetTick(gcf->ax[1], 'y', 0, 10);
		gcf->ax[1]->m_ln[0]->color = gcf->ax[0]->m_ln[0]->color;
		if (multi) 
		{
			PlotDouble(gcf->ax[1], len/2, freq, mag2);
			gcf->ax[1]->m_ln[1]->color = gcf->ax[0]->m_ln[1]->color;
			delete[] fft2;
			delete[] mag2;
		}
		delete[] freq;
		delete[] fft;
		delete[] mag;
		break;
	case IDM_WAVWRITE:
		char fname[MAX_PATH];
		CFileDlg fileDlg;
		fileDlg.InitFileDlg(hDlg, hInst, "");
		errstr[0]=0;
		if(!GetSignalInRange(signal,1))
		{	MessageBox (errStr);	return;	}
		if (fileDlg.FileSaveDlg(fullfname, fname, "Wav file (*.WAV)\0*.wav\0", "wav"))
			if (!signal.Wavwrite(fullfname, errstr))	
			{MessageBox (errstr);}
		else
		{
			CStdString str;
			if (GetLastError()!=0) { GetLastErrorStr(str); MessageBox (str.c_str(), "Filesave error");}
		}
		return;
	}
	InvalidateRect(NULL);
}

CAxis * CPlotDlg::CurrentPoint2CurrentAxis(CPoint *point)
{
	int i;
	for (i=0; i<gcf->nAxes; i++)
	{
		if (IsInsideRect(&gcf->ax[i]->axRect, point))
		{
			gca = gcf->ax[i];
			return gcf->ax[i];
		}
	}
	return NULL;
}



int CPlotDlg::GetSignalInRange(CSignals &signal, int sigOnly)
{
	int ind1, ind2;
	CAxis *lax;
	if (!sigOnly)
		lax = gca;
	else
		lax = gcf->ax[0]; // Following the convention
	ind1 = GetMaxInd4Cut(lax->xlim[0], lax->m_ln[0]->len, lax->m_ln[0]->xdata);
	ind2 = GetMaxInd4Cut(lax->xlim[1], lax->m_ln[0]->len, lax->m_ln[0]->xdata);
	if (ind1<-2 || ind2<-2)
	{
		errStr="x variable is not monotonic or increasing.";
		return 0;
	}
	signal = sig;
	signal.Truncate(ind1, ind2);
	errStr="";
	return 1;
}

int CPlotDlg::GetIndicesInRange(CAxis* axx, int &ind1, int &ind2)
{
	ind1 = GetMaxInd4Cut(axx->xlim[0], axx->m_ln[0]->len, axx->m_ln[0]->xdata);
	ind2 = GetMaxInd4Cut(axx->xlim[1], axx->m_ln[0]->len, axx->m_ln[0]->xdata);
	if (ind1<-2 || ind2<-2)
	{
		errStr="x variable is not monotonic or increasing.";
		return 0;
	}
	errStr="";
	return 1;
}

void CPlotDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	static double prev_ylim[2] = {0, -1};
	static CTick prev_ytick;

	int id, *itemp;
	CLine **temp;
//	if (nChar==17) {CDialog::OnKeyDown(nChar, nRepCnt, nFlags); return;}
	switch(nChar)
	{
	case VK_F2:
		id = nChar - VK_F2;
		itemp = new int [gca->nLines];
		temp = new CLine*[gca->nLines];
		for (int j(0), i(0); i<gca->nLines; i++)
		{
			itemp[i] = (i==id) ? gca->nLines-1 : j++;
		}
		for (int i=0; i<gca->nLines; i++) temp[i] = gca->m_ln[itemp[i]] ;
		delete[] gca->m_ln;
		gca->m_ln = temp;
		delete[] itemp;
		InvalidateRect(NULL);
		break;
	case VK_F1:	// toggle between fixed and automatic y range/tick
		if (prev_ylim[0] > prev_ylim[1]) {
			prev_ylim[0] = gca->ylim[0];
			prev_ylim[1] = gca->ylim[1];
			gca->ylim[0] = 0;
			gca->ylim[1] = -1;
		} else {
			gca->ylim[0] = prev_ylim[0];
			gca->ylim[1] = prev_ylim[1];
			prev_ylim[0] = 0;
			prev_ylim[1] = -1;
		}
		if (prev_ytick.a1 > prev_ytick.a2) {
			prev_ytick.a1 = gca->ytick.a1;
			prev_ytick.a2 = gca->ytick.a2;
			gca->ytick.a1 = 0;
			gca->ytick.a2 = -1;
		} else {
			gca->ytick.a1 = prev_ytick.a1;
			gca->ytick.a2 = prev_ytick.a2;
			prev_ytick.a1 = 0;
			prev_ytick.a2 = -1;
		}
		InvalidateRect(NULL);
		break;
	case VK_F4:
		OnMenu(IDM_SPECTRUM);
		break;
	case VK_DIVIDE:
		OnMenu(IDM_WAVWRITE);
		break;
	case VK_TAB:
		OnMenu(IDM_PAUSE);
		break;
	case VK_SPACE:
		OnMenu(IDM_PLAY);
		break;
	case VK_MULTIPLY:
		OnMenu(IDM_FULLVIEW);
		break;
	case VK_F8:
		OnMenu(IDM_SCROLL_LEFT);
		break;
	case VK_F9:
		OnMenu(IDM_SCROLL_RIGHT);
		break;
	case VK_LEFT:
		OnMenu(IDM_LEFT_STEP);
		break;
	case VK_RIGHT:
		OnMenu(IDM_RIGHT_STEP);
		break;
	case VK_ADD:
	case '=':
		OnMenu(IDM_ZOOM_IN);
		break;
	case VK_SUBTRACT:
		OnMenu(IDM_ZOOM_OUT);
		break;
	}
}



/*
BOOL CPlotDlg::PreCreateWindow(CREATESTRUCT& cs)
{
	// Let's give the figure window tab stop
	cs.style &= WS_TABSTOP ; //This doesn't work somehow......

	return CDialog::PreCreateWindow(cs);
}



BOOL CPlotDlg::PreTranslateMessage(MSG* pMsg)
{ //WHY IS THIS NOT WORKING? (1-21-2008)
	if (!::TranslateAccelerator(pMsg->hwnd, hAccel, pMsg))
		return CDialog::PreTranslateMessage(pMsg);
	else
	{
		return 1;
	}
	
}*/