// Settings.cpp : implementation file
//

#include "stdafx.h"
#include "psycon.h"
#include "Settings.h"
#include "psyconDlg.h"
#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GET 1
#define SET 0

static void *hPlay = NULL;

CStdioFile fle;


/////////////////////////////////////////////////////////////////////////////
// CSettings dialog


CSettings::CSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CSettings::IDD, pParent)
	, m_cal_freq(0), cal_done1(false), cal_done2(false), m_fs(22050)
{
	//{{AFX_DATA_INIT(CSettings)
	m_intervalDelay = 0;
	m_trialDelay = 0;
	m_instructions = _T("");
	m_singleplay = _T("");
	m_procPlay = _T("");
	m_savedetail = FALSE;
	hPar = (CPsyconDlg *)pParent;
	//}}AFX_DATA_INIT
}


void CSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettings)
	DDX_Control(pDX, IDC_PROCEDURE_PLAY, m_procPlayCombo);
	DDX_Control(pDX, IDC_SINGLEPLAY, m_singlePlayCombo);
	DDX_Text(pDX, IDC_DELAY_INTERVALS, m_intervalDelay);
	DDX_Text(pDX, IDC_DELAY_TRIALS, m_trialDelay);
	DDX_Text(pDX, IDC_INST, m_instructions);
	DDX_Text(pDX, IDC_UDFPATH, m_udfpath);
	DDX_Text(pDX, IDC_CONTROLLER_NODE, m_controller_node);
	DDX_Text(pDX, IDC_CONTROLLER_LEVEL, m_controller_level);
	DDX_CBString(pDX, IDC_SINGLEPLAY, m_singleplay);
	DDX_CBString(pDX, IDC_PROCEDURE_PLAY, m_procPlay);
	DDX_Check(pDX, IDC_SAVEDETAIL, m_savedetail);
	DDX_Check(pDX, IDC_USE_AS_CONTROLLER, m_ascontroller);
	DDX_Text(pDX, IDC_FS, m_fs);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SLIDER1, m_volSlider1);
	DDX_Control(pDX, IDC_SLIDER2, m_volSlider2);
	DDX_Control(pDX, IDC_USE_AS_CONTROLLER, m_cont);
	// Log reporting
	DDX_Check(pDX, IDC_LOG_EMAIL, m_log_email);
	DDX_Text(pDX, IDC_EMAIL_ADDRESS, m_email_address);
	DDX_Check(pDX, IDC_LOG_FTP, m_log_ftp);
	DDX_Text(pDX, IDC_FTP_ADDRESS, m_ftp_address);
	DDX_Text(pDX, IDC_FTP_ID, m_ftp_id);
	DDX_Text(pDX, IDC_FTP_PASSWORD, m_ftp_password);
}


BEGIN_MESSAGE_MAP(CSettings, CDialog)
	//{{AFX_MSG_MAP(CSettings)
	ON_BN_CLICKED(IDC_PLAY_STOP, OnPlayStop)
	ON_EN_KILLFOCUS(IDC_VOL1, OnKillFocusVol1)
	ON_EN_KILLFOCUS(IDC_VOL2, OnKillFocusVol2)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM__CAL_PLAYBACK, onCalPlaybackCB)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_CAL_LEVEL1, IDC_CAL_LEVEL2, OnKillfocusCalLevel)
	ON_BN_CLICKED(IDCANCEL, &CSettings::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_COPY_VOL1, &CSettings::OnBnClickedCopyVol1)
	ON_BN_CLICKED(IDC_COPY_VOL2, &CSettings::OnBnClickedCopyVol2)
	ON_BN_CLICKED(IDC_UPDATE_CAL, &CSettings::OnBnClickedUpdateCal)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_PROCEDURE_PLAY, &CSettings::OnCbnSelchangeProcedurePlay)
	ON_CBN_SELCHANGE(IDC_SINGLEPLAY, &CSettings::OnCbnSelchangeSingleplay)
	ON_BN_CLICKED(IDC_USE_AS_CONTROLLER, &CSettings::OnBnClickedUseAsController)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettings message handlers

void CSettings::OnOK() 
{
	if (hPlay) OnPlayStop();
	CDialog::OnOK();	// includes UpdateData();
	if (m_ascontroller && m_controller_node.GetLength()==0) 
	{
		m_controller_node = ".";
		SetDlgItemText(IDC_CONTROLLER_NODE, m_controller_node);
	}
	hTaskDlg->intervalDelay = m_intervalDelay ;
	hTaskDlg->trialDelay = m_trialDelay ;
	hTaskDlg->singlePlay = m_singlePlayCombo.GetCurSel();
	hTaskDlg->procPlay = m_procPlayCombo.GetCurSel();
	hTaskDlg->saveDetails = m_savedetail;
	hTaskDlg->fs = m_fs;
	hTaskDlg->m_udfpath = m_udfpath;
	if (m_udfpath.GetLength()>0)
		hTaskDlg->AstSig.SetPath("aux", m_udfpath);
	else
		hTaskDlg->AstSig.SetPath("aux", theApp.AppPath);
	hTaskDlg->ascontroller = m_ascontroller ? true : false;
	hTaskDlg->m_controller_node = m_controller_node;
	hTaskDlg->m_controller_level = m_controller_level;
	str2array(hTaskDlg->instruction, 2, m_instructions, "\r\n");
	// Log reporting
	if ((hPar->report.fUseEmail = m_log_email))
		hPar->report.email_address = m_email_address;
	if ((hPar->report.fUseFtp = m_log_ftp)) {
		hPar->report.ftp_address = m_ftp_address;
		hPar->report.ftp_id = m_ftp_id;
		hPar->report.ftp_password = m_ftp_password;
	}
}

BOOL CSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	hTaskDlg = (CPsyconDlg *)hPar;
	int i;
	m_fs = hTaskDlg->fs;
	m_intervalDelay = hTaskDlg->intervalDelay;
	m_trialDelay = hTaskDlg->trialDelay;
	m_instructions.Format("%s\r\n%s", hTaskDlg->instruction[0], hTaskDlg->instruction[1]);
	m_udfpath = hTaskDlg->m_udfpath;
	m_ascontroller = hTaskDlg->ascontroller;
	m_controller_node = hTaskDlg->m_controller_node;
	m_controller_level = hTaskDlg->m_controller_level;
	m_savedetail = hTaskDlg->saveDetails;
	for (i=0; i< hTaskDlg->nDev; i++)
	{
		m_singlePlayCombo.AddString(hTaskDlg->devCaptions[i]);
		m_procPlayCombo.AddString(hTaskDlg->devCaptions[i]);
		MasterVolume(i, curvol[i], maxvol[i], GET);
	}
	// Log reporting
	if ((m_log_email = hPar->report.fUseEmail))
		m_email_address = hPar->report.email_address;
	if ((m_log_ftp = hPar->report.fUseFtp)) {
		m_ftp_address = hPar->report.ftp_address;
		m_ftp_id = hPar->report.ftp_id;
		m_ftp_password = hPar->report.ftp_password;
	}
	UpdateData(0);
	m_singlePlayCombo.SetCurSel(hTaskDlg->singlePlay);
	m_procPlayCombo.SetCurSel(hTaskDlg->procPlay);
	OnChangeDev(IDC_SINGLEPLAY);
	OnChangeDev(IDC_PROCEDURE_PLAY);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

#define MCHEC(X) {rc=X; if (rc!=MMSYSERR_NOERROR) {s.Format("Error, at "#X "code=%d\n", rc); MessageBox(s);}}

void CSettings::OnCbnSelchangeProcedurePlay()
{
	OnChangeDev(IDC_PROCEDURE_PLAY);
}

void CSettings::OnCbnSelchangeSingleplay()
{
	OnChangeDev(IDC_SINGLEPLAY);
}

void CSettings::OnChangeDev(UINT nID, bool more)
{
	CComboBox *pcombo;
	CSliderCtrl *pslider;
	int idc_vol, idc_vol_caption, sel;

	switch(nID)
	{
	case IDC_SINGLEPLAY:
		pcombo = &m_singlePlayCombo;
		pslider = &m_volSlider1;
		idc_vol = IDC_VOL1;
		idc_vol_caption = IDC_MAXVOL_CAPTION1;
		break;
	case IDC_PROCEDURE_PLAY:
		pcombo = &m_procPlayCombo;
		pslider = &m_volSlider2;
		idc_vol = IDC_VOL2;
		idc_vol_caption = IDC_MAXVOL_CAPTION2;
		break;
	}
  	sel = pcombo->GetCurSel();
	SetDlgItemInt (idc_vol_caption, maxvol[sel]);
	pslider->SetRange(0, maxvol[sel]);
	SetDlgItemInt (idc_vol, curvol[sel], 0);
	pslider->SetPos(curvol[sel]);
	if (m_singlePlayCombo.GetCurSel()==m_procPlayCombo.GetCurSel() && more)
	{
		if (nID==IDC_SINGLEPLAY) OnChangeDev (IDC_PROCEDURE_PLAY, false);
		else OnChangeDev (IDC_SINGLEPLAY, false);
	}
}

int CSettings::MasterVolume(DWORD devID, DWORD& value, DWORD& maxVal, int get)
{
	CString s, s1;
	MIXERCAPS cap;
	HMIXER hMixer;
	MMRESULT rc;
	MIXERLINE line;
	MIXERLINECONTROLS lineControls;
	MIXERCONTROLDETAILS details;
	PMIXERCONTROL paMixerControl;
	MIXERCONTROLDETAILS_UNSIGNED* pmxcdu;
	unsigned int u, j, w;

	if (mixerGetDevCaps (devID, &cap, sizeof(cap))!=0)
	{
		s.Format("Error, mixerGetDevCaps(%d,  &cap, sizeof(cap));", devID);
		MessageBox (s);
		return 0;
	}
	MCHEC(mixerOpen(&hMixer, devID, 0, 0, 0))
	for (j=0; j<cap.cDestinations; j++)
	{
		line.cbStruct = sizeof(MIXERLINE);
		line.dwDestination = j;
		MCHEC(mixerGetLineInfo ((HMIXEROBJ)hMixer, &line,  0))
		if (line.dwComponentType==MIXERLINE_COMPONENTTYPE_DST_SPEAKERS)
		{
			paMixerControl = (MIXERCONTROL*) calloc(line.cControls, sizeof(MIXERCONTROL));
			lineControls.cbStruct = sizeof(MIXERLINECONTROLS);
			lineControls.dwLineID = line.dwLineID;
			lineControls.dwControlID = 0; // dwControlType will also be 0
			lineControls.cControls = line.cControls;
			lineControls.cbmxctrl = sizeof(MIXERCONTROL);
			lineControls.pamxctrl = paMixerControl;
			MCHEC(mixerGetLineControls((HMIXEROBJ)hMixer, &lineControls, MIXER_GETLINECONTROLSF_ALL))
			for (u=0; u<line.cControls; u++)
			{
				if (paMixerControl[u].dwControlType==MIXERCONTROL_CONTROLTYPE_VOLUME)
				{
					pmxcdu = (MIXERCONTROLDETAILS_UNSIGNED*)calloc(line.cChannels, sizeof(MIXERCONTROLDETAILS_UNSIGNED));
					details.cbStruct       = sizeof(MIXERCONTROLDETAILS);
					details.dwControlID    = paMixerControl[u].dwControlID;
					details.cChannels      = line.cChannels;
					details.cMultipleItems = paMixerControl[u].cMultipleItems;
					details.cbDetails      = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
					details.paDetails      = pmxcdu;
					if (get==GET)
					{
						MCHEC(mixerGetControlDetails((HMIXEROBJ)hMixer, &details, 0L))
						value = pmxcdu[u].dwValue;
						maxVal = paMixerControl[u].Bounds.dwMaximum;
					}
					else // set
					{
						for (w=0; w<details.cChannels; w++)		pmxcdu[w].dwValue = value;
						MCHEC(mixerSetControlDetails((HMIXEROBJ)hMixer, &details, 0L))
					}
					
					free(pmxcdu);
					u=line.cControls+1;
					j=cap.cDestinations+1;
				}
			}
			free(paMixerControl);
		}
	}
	return 1;
}


/*
void _24bit_to_short(int* pint, short* pshort, int len)
{ // Assumes that memory blocks have been prepared.
	for (int i=0; i<len; i++) pshort[i] = (short)(pint[i]>>9);
}
*/
#define MAX_24BIT		(double)0x007fffff
#define _double_to_24bit(x) ((int)((x)*MAX_24BIT-.5))

void CSettings::OnPlayStop() 
{
	int check, nPoints;
	CSignals cal(m_fs);
	char errstr[256];
	double freq = GetDlgItemDouble(m_hWnd, IDC_CALTONE_FREQ, &check);
	
	if (!hPlay)
	{
		nPoints = m_fs * 15120 / (int)freq; // 15120 = 16 * 27 * 5 * 7
		cal.Tone(freq, nPoints);
		hPlay = cal.PlayArray(m_procPlayCombo.GetCurSel(), WM__CAL_PLAYBACK, m_hWnd, -1, errstr);
		SetDlgItemText(IDC_PLAY_STOP, "Stop");
		EnableID(this, IDC_COPY_VOL1,1);
		EnableID(this, IDC_COPY_VOL2,1);
	}
	else
	{
		TerminatePlay(hPlay);
		hPlay = NULL;
		SetDlgItemText(IDC_PLAY_STOP, "Play");
	}
}

void CSettings::OnKillFocusVol(int idc)
{
	CComboBox *pcombo;
	CSliderCtrl *pslider;
	int succ, selectedDev;
	DWORD val, dummy;
	CString s;
	int sel;
	switch(idc)
	{
	case IDC_VOL1:
		pcombo = &m_singlePlayCombo;
		pslider = &m_volSlider1;
		selectedDev = IDC_SINGLEPLAY;
		break;
	case IDC_VOL2:
		pcombo = &m_procPlayCombo;
		pslider = &m_volSlider2;
		selectedDev = IDC_PROCEDURE_PLAY;
		break;
	}
	val = GetDlgItemInt (idc, &succ);
	sel = pcombo->GetCurSel();
	if (succ && val >= 0 && val <= maxvol[sel])
		MasterVolume(sel, val, dummy, SET); //3rd argument ignored
	else
	{
		s.Format("Not valid value for master volume (min:0, max:%d)", maxvol[sel]);
		MessageBox (s);
	}
	if (m_singlePlayCombo.GetCurSel()==m_procPlayCombo.GetCurSel())
	{
		if (idc==IDC_VOL1) SetDlgItemInt (IDC_VOL2, GetDlgItemInt(IDC_VOL1));
		else SetDlgItemInt (IDC_VOL1, GetDlgItemInt(IDC_VOL2));
	}
	curvol[sel]=val;
	OnChangeDev(selectedDev);
	//pslider->SetRange(0, maxvol[sel]);
	//SetDlgItemInt (idc, curvol[sel], 0);
	//pslider->SetPos(curvol[sel]);
}


void CSettings::OnKillFocusVol1() 
{
	OnKillFocusVol(IDC_VOL1);
}

void CSettings::OnKillFocusVol2() 
{
	OnKillFocusVol(IDC_VOL2);
}

void CSettings::OnKillfocusCalLevel(UINT nID)
{
	int chek1, chek2, trans;
	double level1, level2;
	int vol1, vol2;
	level1 = GetDlgItemDouble(m_hWnd, IDC_CAL_LEVEL1, &chek1);
	level2 = GetDlgItemDouble(m_hWnd, IDC_CAL_LEVEL2, &chek2);
	if (chek1&chek2)	
	{
		vol1 = GetDlgItemInt(IDC_VOL_CAL1, &trans);
		if (!trans)	MessageBox ("Invalid Volume 1"); 
		else
		{
			vol2 = GetDlgItemInt(IDC_VOL_CAL2, &trans);
			if (!trans)	MessageBox ("Invalid Volume 2"); 
			else
			{
				if (vol1==vol2) MessageBox ("2 points calibration, please!"); 
				{
					slope = (level2-level1)/(double)(vol2-vol1);
					affin = level1;
					if (slope<0) MessageBox ("Wrong calibration data? (level should inrease with volume)");
					else { EnableID(this, IDC_UPDATE_CAL,1);	return;}
				}
			}
		}
	}
	EnableID(this, IDC_UPDATE_CAL,0);
}

LRESULT CSettings::onCalPlaybackCB(WPARAM wp, LPARAM lp)
{
	return 1;
}
void CSettings::OnBnClickedCancel()
{
	if (hPlay) OnPlayStop();
	OnCancel();
}

void CSettings::OnBnClickedCopyVol1()
{
	CString s;
	GetDlgItemText(IDC_VOL2, s);
	SetDlgItemText(IDC_VOL_CAL1, s);
}

void CSettings::OnBnClickedCopyVol2()
{
	CString s;
	GetDlgItemText(IDC_VOL2, s);
	SetDlgItemText(IDC_VOL_CAL2, s);
}

void CSettings::OnBnClickedUpdateCal()
{
	char buf[32], errStr[256];
	CString fnameWpath;
	int maxVol = maxvol[m_procPlayCombo.GetCurSel()];
	double vol1 = GetDlgItemInt(IDC_VOL_CAL1);
	double maxLevel = slope*(double)(maxVol-vol1) + affin;
	if (MessageBox ("Update the calibration level and save to file?","Settings", MB_YESNO)==IDYES)
	{
		fnameWpath.Format("%s%s", theApp.AppPath, INI_FILE_NAME);
		sprintfFloat (maxLevel, 2, buf, sizeof(buf));
		hTaskDlg->SetDlgItemText(IDC_MAX_LEVEL, buf);
		theApp.m_maxlevel = maxLevel;
		if (!printfINI(errStr, fnameWpath, "MAX LEVEL(dB)", "%6.2lf", maxLevel)) 
		{MessageBox (errStr); return;}
		if(!printfINI(errStr, fnameWpath, "dB per Volume_tick", "%12.10lf", slope))
		{MessageBox (errStr); return;}
	}
}

void CSettings::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl *pSlider = (CSliderCtrl *)pScrollBar;
	WORD pos = LOWORD(nPos);
	bool dual = (m_singlePlayCombo.GetCurSel()==m_procPlayCombo.GetCurSel());
	if (nSBCode != SB_ENDSCROLL)
	{
		if (pSlider == &m_volSlider1)
		{
			SetDlgItemInt (IDC_VOL1, pos);
			OnKillFocusVol(IDC_VOL1);
		}
		else
		{
			SetDlgItemInt (IDC_VOL2, pos);
			OnKillFocusVol(IDC_VOL2);
		}
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}




void CSettings::OnBnClickedUseAsController()
{
	static CString lastnode("");
	if (!m_cont.GetCheck())
	{
		 GetDlgItemText(IDC_CONTROLLER_NODE, lastnode);
		 SetDlgItemText(IDC_CONTROLLER_NODE, "");
		 EnableID(this, IDC_CONTROLLER_NODE, 0);
	}
	else
	{
		SetDlgItemText(IDC_CONTROLLER_NODE, lastnode);
		 EnableID(this, IDC_CONTROLLER_NODE, 1);
	}
}
