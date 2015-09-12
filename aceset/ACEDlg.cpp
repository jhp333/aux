#include "ACEsetDlg.h"
#include "aceset.h"
#include "msgCrack_aceset.h"
#include "audfret.h"
#include "audstr.cpp"

BOOL CALLBACK  ACEDlgProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam);

HMODULE hDllModule;
CWndDlg wnd1, wnd2;
CACEDlg *ACEDlg1, *ACEDlg2;

#define CHEC(x) if(!x) {return -1;}
#define CHEC2(x) if(x<0) {return -1;}
#define SCANSTR(HEAD,VAR) {\
	if ((res=ReadINI(errStr, fname, HEAD, strBuffer))>0) strncpy(VAR,strBuffer.c_str(),sizeof(VAR));\
	else { sprintf(buff, "Invalid [%s]\n%s", HEAD, errStr); MessageBox(buff, fname); return res; } }

#define CHECKNULL(X)\
	if ((X)==NULL) {sprintf(errstring, #X); return 0;}

#define SCANITEM(HEAD,FORMAT,VAR) \
if ((res=sscanfINI(errStr,fname,HEAD,FORMAT,&VAR))<=0 || VAR==0) { sprintf(buff, "Invalid [%s]\n%s", HEAD, errStr); MessageBox(buff,fname); return -1; } 

#define LINE_LENGTH 128
#define MAX_CHANNELS 24

BOOL CALLBACK  ACEDlgProc1 (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	switch (umsg)
	{
	chHANDLE_DLGMSG (hDlg, WM_INITDIALOG, wnd1.OnInitDialog);
	chHANDLE_DLGMSG (hDlg, WM__OPEN_MAP, ACEDlg1->onOpenMap);
	//chHANDLE_DLGMSG (hDlg, WM_CLOSE, ACEDlg.OnClose);
	chHANDLE_DLGMSG (hDlg, WM_COMMAND, ACEDlg1->OnCommand);
	//chHANDLE_DLGMSG (hDlg, WM_FL_ARRIVED, ACEDlg.OnFlyArrived);
	//chHANDLE_DLGMSG (hDlg, WM_FL_CLOSE, ACEDlg.OnFlyClosed);
	//chHANDLE_DLGMSG (hDlg, WM_FL_CONNECTED, ACEDlg.OnFlyConnected);
	//chHANDLE_DLGMSG (hDlg, WM_TIMER, ACEDlg.OnTimer);
	default:
		return FALSE;
	}
	return TRUE;
}
BOOL CALLBACK  ACEDlgProc2 (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	switch (umsg)
	{
	chHANDLE_DLGMSG (hDlg, WM_INITDIALOG, wnd2.OnInitDialog);
	chHANDLE_DLGMSG (hDlg, WM__OPEN_MAP, ACEDlg2->onOpenMap);
	//chHANDLE_DLGMSG (hDlg, WM_SIZE, ACEDlg.OnSize);
	//chHANDLE_DLGMSG (hDlg, WM_CLOSE, ACEDlg.OnClose);
	chHANDLE_DLGMSG (hDlg, WM_COMMAND, ACEDlg2->OnCommand);
	//chHANDLE_DLGMSG (hDlg, WM_FL_ARRIVED, ACEDlg.OnFlyArrived);
	//chHANDLE_DLGMSG (hDlg, WM_FL_CLOSE, ACEDlg.OnFlyClosed);
	//chHANDLE_DLGMSG (hDlg, WM_FL_CONNECTED, ACEDlg.OnFlyConnected);
	//chHANDLE_DLGMSG (hDlg, WM_TIMER, ACEDlg.OnTimer);
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	hDllModule = (HMODULE)hModule;
    return TRUE;
}


CACEDlg::CACEDlg(void)
: errstr(""), param(NULL)
{
	MAPfilename[0]=0;
}

CACEDlg::~CACEDlg(void)
{
	// Clean up param->addmlines must be done here, because it was allocated and modifed in this DLL.
	if (param->addmlines!=NULL && param->lenAddmlines>0) 
		delete[] param->addmlines;
	if (param!=NULL)	{delete param; param=NULL;}
}

HWND CACEDlg::Create(HWND hPar, POINT position)
{
	// As of July-4-07, this class is not complete: when two ACEdlg objects are used, let's go this way for the time being
	static int ind(0);
	RECT rt;

	if (hPar==NULL) {errstr = "Null parent pointer."; return NULL;}
	if (ind==0)
	{
		wnd1.hDlg = CreateDialog(hDllModule, MAKEINTRESOURCE(IDD_ACESET), hPar, ACEDlgProc1); 
		ACEDlg1 = this;
		hDlg = wnd1.hDlg ;
	}
	else
	{
		wnd2.hDlg = CreateDialog(hDllModule, MAKEINTRESOURCE(IDD_ACESET), hPar, ACEDlgProc2); 
		ACEDlg2 = this;
		hDlg = wnd2.hDlg ;
	}
	OnInitDialog(NULL, NULL);
	GetWindowRect(hDlg, &rt);
	MoveWindow(hDlg, position.x, position.y, rt.right-rt.left, rt.bottom-rt.top, TRUE);

	ind++;
	return hDlg;
}

BOOL CACEDlg::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
	CStdString str;
	fdlg.InitFileDlg(hDlg, NULL);
	Param2Screen();
	return 1;
}

void CACEDlg::onOpenMap(const char* mapfilefull, InterfaceType iProc)
{
	char buf[MAX_PATH];
	bool hiace(false), update2hiace(false);
	if (readACEPatientFile(mapfilefull, param)>0)
	{
		for (int i=0; i<22; i++) 
			if (param->pw[i]<24.8) 
				{hiace=true; break;}
		if (hiace || param->pg < 7.8) 
		{
			if (iProc!=L34_CIC4)
			{
				strcpy(buf, "The implant must be CIC4 to specify PW<25, PG<8. You must restart the program or open a MAP for CIC3."); 
				return;
			}
			for (int i=0; i<22; i++) 
				if (param->pw[i] != min_cic4_pw) update2hiace = true;
			if (param->pg != min_cic4_pg) update2hiace = true;
			if (update2hiace)
			{
				for (int i=0; i<22; i++) param->pw[i] = min_cic4_pw;
				param->pg = min_cic4_pg;
				if (writeACEPatientFile (mapfilefull, param, buf)==0)
				{ 
					MessageBox(buf);
					return;
				}
			}
		}
		if(!Param2Screen()) return;
	}
	else
		return;
	SetDlgItemText (IDC_MAP_FILE, mapfilefull);
	strcpy(MAPfilename, mapfilefull);
	_splitpath(mapfilefull, fdlg.LastPath, buf, NULL, NULL);
	strcat(fdlg.LastPath, buf);
}

void CACEDlg::OnCommand(int idc, HWND hwndCtl, UINT event)
{
	char fullfname[MAX_PATH];
	static int gainAdj;
	DWORD gle;
	char fname[MAX_PATH], errStr[128];
	CStdString str;
	switch(idc)
	{
	case IDC_OPEN_MAP:
		gainAdj = 0;
		fname[0]='\0';fullfname[0]='\0';
		if (!fdlg.FileOpenDlg(fullfname, fname, "ACE MAP Files (*.ACE)\0*.ace\0", "ace"))
		{
			gle = GetLastError();
			if (gle!=0)
			{
				sprintf(errStr, "GetOpenFileName failed. code=%d", gle);
				MessageBox (errStr, "aceset", MB_OK);
			}
		}
		else
		{
			onOpenMap(fullfname, UNSPECIFIED);
		}
		break;
	case IDC_ADDMLINES:
		if (event==EN_UPDATE)
		{
			GetDlgItemText(idc, str);
			if (str.length()>=(size_t)param->lenAddmlines)
			{
				if (param->lenAddmlines>0)		delete[] param->addmlines;
				param->lenAddmlines = (int)str.length() + 64;
				param->addmlines = new char[param->lenAddmlines];
			}
			strcpy(param->addmlines, str.c_str());
		}
		break;

	default:
		if (IsBetween(idc, IDC_ELEC_CHECK0, IDC_ELEC_CHECK21) && event==BN_CLICKED)
				UpdateCutoffFreq ();
	}
}

double CACEDlg::GetLevel()
{
	int ch;
	double level = GetDlgItemDouble(hDlg, IDC_LEVEL, &ch);
	if (!ch) 
	{
		MessageBox ("Invalid level", "Aceset"); 
		return ACESET_QNAN;
	}
	return level;
}

int CACEDlg::Param2Screen()
{
	int i, re, cum(0);
	char str[2048], str1[1024], str2[1024];

	SetDlgItemText(IDC_SUBJ_ID, param->subj);
	if (strlen(param->mode)>0) SetDlgItemText(IDC_MODE, param->mode); else return 0;
	sprintfFloat(param->pw[0], 1, str1, sizeof(str1)); sprintfFloat(param->pg, 1, str2, sizeof(str2));
	sprintf(str, "%s/%s", str1, str2);
	SetDlgItemText(IDC_PW_PG, str); 
	sprintfFloat(param->rate, 1, str, sizeof(str)); SetDlgItemText(IDC_CH_RATE, is_wACE(param)?W_ACE:str);
	if (is_wACE(param))
		SetDlgItemText(IDC_NUMMAX, W_ACE);
	else
		SetDlgItemInt(IDC_NUMMAX, param->numMax, 0);
	SetDlgItemInt (IDC_Q, param->Q, 0);
	for (i=0; i<22; i++) {
		if (is_wACE(param)) {
			if (param->t[i] == -1 || param->c[i] == -1)
				param->selected[i] = 0;
			else
				param->selected[i] = 1;
		}
		SetDlgItemInt(IDC_T0+i, (int)param->t[i], 1);
		SetDlgItemInt(IDC_C0+i, (int)param->c[i], 1);
		if (i>0 && param->pw[i]!= param->pw[0])
		{	sprintfFloat(param->pw[i], 1, str, sizeof(str)); SetDlgItemText(IDC_PW0+i, str);	}
		else
			SetDlgItemText(IDC_PW0+i, "");
		sprintfFloat(param->gains[i], 1, str, sizeof(str)); SetDlgItemText(IDC_GAIN0+i, str);
		if (param->selected[i]) SendDlgItemMessage(IDC_ELEC_CHECK0+i, BM_SETCHECK, BST_CHECKED, 0);
		else SendDlgItemMessage(IDC_ELEC_CHECK0 +i, BM_SETCHECK, BST_UNCHECKED, 0);
		re = param->GetRE(22-i);
		if (re>100)
		{
			SetDlgItemText(IDC_RE0+i, param->mode);
			EnableDlgItem(hDlg, IDC_ELEC_CHECK0+i, (param->rate!=-1));
		}
		else if (re<=22)
		{
			SetDlgItemInt(IDC_RE0+i, re, 0);
			EnableDlgItem(hDlg, IDC_ELEC_CHECK0+i, (param->rate!=-1));
		}
		else // disable all the entry for non-existing setting 
		{
			EnableDlgItem(hDlg, IDC_ELEC_CHECK0+i, 0);
			SetDlgItemText(IDC_RE0+i, "");
			SetDlgItemText(IDC_T0+i, "");
			SetDlgItemText(IDC_C0+i, "");
			SetDlgItemText(IDC_GAIN0+i, "");
		}
	}
	SetDlgItemInt(IDC_NUMBANDS, param->numBands, 0);
	if (param->lenAddmlines)		SetDlgItemText(IDC_ADDMLINES, param->addmlines);
	SetDlgItemText(IDC_MAP_FILE, MAPfilename);
	if (!UpdateCutoffFreq())		return 0;
	return 1;
}

int CACEDlg::Screen2Param()
{
	int i, j, res, trans, cum(0);
	double dtemp;
	CStdString str, str1, str2;

	GetDlgItemText(IDC_NUMMAX, str);
	if (str == W_ACE)
		param->numMax = -1;
	else {
		res = GetDlgItemInt(IDC_NUMMAX, &trans, 0);
		if (trans) param->numMax = res;
		else { errstr="Invalid number of Maxima"; return 0;}
	}
	res = GetDlgItemInt(IDC_Q, &trans, 0);
	if (trans) param->Q = res;
	else { errstr="Invalid Q"; return 0;}
	for (i=0, param->numBands=0; i<22; i++) {
		j = GetDlgItemInt(IDC_T0+i, &trans, 0); if (trans) param->t[i] = j;
		j = GetDlgItemInt(IDC_C0+i, &trans, 0); if (trans) param->c[i] = j;
		dtemp = GetDlgItemDouble (hDlg, IDC_GAIN0+i, &trans); if (trans) param->gains[i] = dtemp;
		if (SendDlgItemMessage(IDC_ELEC_CHECK0+i, BM_GETCHECK, 0, 0) == BST_CHECKED) 
		{	param->selected[i]=1; param->numBands++; }
		else
			param->selected[i]=0;
	}
	GetDlgItemText(IDC_ADDMLINES, str);
	str.Trim();
	if (param->lenAddmlines>0)		{ delete[] param->addmlines; param->addmlines = new char[ param->lenAddmlines = (int)str.length()+64 ];}
	strcpy(param->addmlines, str.c_str());
	return 1;
}

int CACEDlg::writeACEPatientFile (const char *fname, CParamACE *param, char *errStr)
{
	char buf[4096], buf2[128], onoffStr[8];
	int i, ae, re, count(0);
	CHEC(printfINI(errStr, fname, "SUBJECT ID", param->subj))
	CHEC(printfINI(errStr, fname, "STIMULATION MODE", param->mode))
	sprintfFloat(param->pw[0], 1, buf, sizeof(buf));
	CHEC(printfINI(errStr, fname, "PHASE WIDTH", buf))
	sprintfFloat(param->pg, 1, buf, sizeof(buf));
	CHEC(printfINI(errStr, fname, "PHASE GAP", buf))
	if (is_wACE(param)) {
		CHEC(printfINI(errStr, fname, "STIMULATION RATE", W_ACE))
	} else {
		sprintfFloat(param->rate, 1, buf, sizeof(buf));
		CHEC(printfINI(errStr, fname, "STIMULATION RATE", buf))
	}
	if (is_wACE(param)) {
		CHEC(printfINI(errStr, fname, "NUMBER OF MAXIMA", W_ACE))
	} else {
		CHEC(printfINI(errStr, fname, "NUMBER OF MAXIMA", "%d", param->numMax))
	}
	strcpy(buf, "\r\n");
	for (i=0; i<22; i++)
	{	
		ae = 22-i;
		re = param->mode2enum(buf);
		if (re<100) re = ae + re; // If not MP 
		if (param->selected[i]) { count++; strcpy(onoffStr, "O");}
		else strcpy(onoffStr, "X");
		if (re>100 || re<23)
		{
			sprintf (buf2, "%d\t%d\t%d\t%d\t%5.1f\t%s\r\n", ae, re, (int)param->t[i], (int)param->c[i], param->gains[i], onoffStr);
			strcat(buf, buf2);
		}
	}
	CHEC(printfINI(errStr, fname, "AE\tRE\tT\tC\tGAIN\tON/OFF", "%s", buf))
	CHEC(printfINI(errStr, fname, "Q", "%d", param->Q))
	return 0;
}

int CACEDlg::readACEPatientFile (const char *fname, CParamACE *param)
{
	int i, res, ind, ae, re, nItems, numBandsCounted;
	CStdString lineBuf[MAX_CHANNELS], temp[7];
	char errStr[256], errMsg[128], buff[256], buff2[256];
	string strBuffer;
	double temp_pw;
	bool varPw(false);

	if ((res=ReadINI(errStr, fname, "SUBJECT ID", strBuffer))>0)	strcpy(param->subj, strBuffer.c_str());
	else 
	{
		if (res==AUD_ERR_FILE_NOT_FOUND || res==AUD_ERR_CANNOT_READ_FILE)
		{
			sprintf(errStr, "File %s not found", fname);
			MessageBox (errStr);
			return 0;
		}
		else
			MessageBox ("Warning: Subject ID is missing or invalid");
	}
	SCANSTR("STIMULATION MODE", param->mode)
	if (param->GetRE(1)<0) {
		sprintf(errStr, "Invalid mode string: %s", param->mode);
		MessageBox (errStr);
		return 0;
	}
	SCANITEM("PHASE WIDTH", "%lf", temp_pw)
	for (i=0; i<22; i++) param->pw[i] = temp_pw;
	SCANITEM("PHASE GAP", "%lf", param->pg)
	
	SCANSTR("STIMULATION RATE", buff)
	SCANSTR("NUMBER OF MAXIMA", buff2)
	temp[0]=buff; temp[0].MakeLower(); temp[1]=buff2; temp[0].MakeLower(); temp[2]=W_ACE; temp[2].MakeLower();
	if (temp[0] == temp[2] || temp[1] == temp[2]) {
		param->rate = -1;
		param->numMax = -1;
	} else {
		SCANITEM("STIMULATION RATE", "%lf", param->rate)
		SCANITEM("NUMBER OF MAXIMA", "%d", param->numMax)
	}

	SCANITEM("Q", "%d", param->Q)
	
	//Check if this file is for newer version
	if (ReadINI(errStr, fname, "AE\tRE\tT\tC\tGAIN\tON/OFF", strBuffer)>0) // new version 
	{
		param->numBands = 0;
		nItems = str2array (lineBuf, 22, strBuffer.c_str(), "\r\n");
		memset((void*)param->selected, 0, sizeof(param->selected));
		memset((void*)param->gains, 0, sizeof(param->gains));
		for (int id=0; id<nItems; id++)
		{
			if ((res=str2array (temp, 7, lineBuf[id].c_str(), " \t"))<6)
			{	sprintf(errStr,"[AE RE T C GAIN ON/OFF]:only %d items parsed on line %d", res, id); 
				MessageBox(errStr, fname); return 0;
			}
			if (sscanf(temp[0].c_str(), "%d", &ae)<=0) {	sprintf(errStr, "Invalid Active electrode on line %d", id); MessageBox(errStr, fname); return 0;	}
			if (sscanf(temp[1].c_str(), "%d", &re)<=0) {	sprintf(errStr, "Invalid Ref electrode on line %d", id); MessageBox(errStr, fname); return 0;	}
			ind = 22 - ae;
			if (re != param->GetRE(ae)) {	sprintf(errStr, "Ref elec not matched from mode on line %d", id); MessageBox(errStr, fname); return 0;	}
			if (sscanf(temp[2].c_str(), "%lf", &param->t[ind])<=0) {	sprintf(errStr, "Invalid T on line %d", id); MessageBox(errStr, fname); return 0;	}
			if (sscanf(temp[3].c_str(), "%lf", &param->c[ind])<=0) {	sprintf(errStr, "Invalid C on line %d", id); MessageBox(errStr, fname); return 0;	}
			if (sscanf(temp[4].c_str(), "%lf", &param->gains[ind])<=0) {	sprintf(errStr, "Invalid GAIN on line %d", id); MessageBox(errStr, fname); return 0;	}
			temp[5].Trim();
			if (_stricmp( temp[5].c_str(), "o")==0) { param->selected[ind] = true; param->numBands++; }
			else  {   param->selected[ind] = false; }
			if (res==7)
			{
				if (sscanf(temp[6].c_str(), "%lf", &temp_pw)<=0)
				{	sprintf(errStr, "Invalid pw on line %d", id); MessageBox(errStr, fname); return 0;	}
				param->pw[ind] = temp_pw;
			}
		}
		// "NUMBER OF BANDS" is not read any more in the new style...but still recorded for convenience and fixed by a correct value
		if (sscanfINI(errStr,fname,"NUMBER OF BANDS", "%d", &re)<=0 || re !=param->numBands)
		CHEC(printfINI(errStr, fname, "NUMBER OF BANDS", "%d", param->numBands))
		if (param->numMax != -1 && param->numMax>param->numBands)
		{
			param->numMax = param->numBands;
			CHEC(printfINI(errStr, fname, "NUMBER OF MAXIMA", "%d", param->numMax))
			MessageBox ("NUMBER OF MAXIMA has been adjusted following the channel selection data",fname);
		}
	}
	else
	{
		if (sscanfINI(errStr, fname, "NUMBER OF BANDS", "%d", &param->numBands)<0)
		{	sprintf(errStr, "Invalid Number_of_Bands"); MessageBox(errStr, fname); return 0;	}
		if (ReadINI(errStr, fname, "EXCLUDING ELECTRODES", strBuffer)<0)
		{	sprintf(errStr, "Invalid EXCLUDING_ELECTRODES"); MessageBox(errStr, fname); return 0;	}
		for (i=0; i<22; i++)	param->selected[i] = 1;
		numBandsCounted = str2array (lineBuf, 22, strBuffer.c_str(), " ");
		for (i=0; i<numBandsCounted; i++)
			param->selected[22-atoi(lineBuf[i].c_str())] = 0;
		if (param->numBands!=22-numBandsCounted)
		{ // if numBands is incorrect, correct and save it
			param->numBands=22-numBandsCounted;
			writeACEPatientFile(fname, param, errMsg);
		}
		if (ReadINI(errStr, fname, "T C GAIN", strBuffer)<0)
		{	sprintf(errStr, "Error T C GAIN"); MessageBox(errStr, fname); return 0;	}
		nItems = str2array (lineBuf, MAX_CHANNELS, strBuffer.c_str(), "\r\n");
		for (i=0; i<nItems; i++)
		{
			if (str2array (temp, 3, lineBuf[i], " \t")<3)
			{		sprintf(errStr, "T C GAIN not valid");			return 0;		}
			if (sscanf(temp[0].c_str(), "%lf", &param->t[i])==EOF) {	sprintf(errStr, "Invalid T on line %d", i); return 0;	}
			if (sscanf(temp[1].c_str(), "%lf", &param->c[i])==EOF) {	sprintf(errStr, "Invalid C on line %d", i); return 0;	}
			if (sscanf(temp[2].c_str(), "%lf", &param->gains[i])==EOF) {	sprintf(errStr, "Invalid GAIN on line %d", i); return 0;	}
		}
		writeACEPatientFile(fname, param, errMsg); // so that this can be updated.
	}
	return 1;
}

ACESET_API void CACEDlg::EVK_OpenMAP()
{
	OnCommand(IDC_OPEN_MAP, GetDlgItem(IDC_OPEN_MAP), BN_CLICKED);
}

ACESET_API void CACEDlg::EVK_CloseMAP()
{
	if (param!=NULL)
		delete param;
	param = newCParamACE();
	SetDlgItemText(IDC_SUBJ_ID, "");
	SetDlgItemText(IDC_MODE, "");
	SetDlgItemText(IDC_PW_PG, ""); 
	SetDlgItemText(IDC_CH_RATE, "");  
	SetDlgItemText(IDC_NUMMAX, "");
	SetDlgItemText(IDC_Q, "");
	SetDlgItemText(IDC_MAP_FILE, "");
	MAPfilename[0]=0;
	for (int i=0; i<22; i++) {
		SetDlgItemText(IDC_T0+i, "");
		SetDlgItemText(IDC_C0+i, "");
		SetDlgItemText(IDC_PW0+i, "");
		SetDlgItemText(IDC_GAIN0+i, "");
		EnableDlgItem(hDlg, IDC_ELEC_CHECK0+i, 1);
		SendDlgItemMessage(IDC_ELEC_CHECK0 +i, BM_SETCHECK, BST_UNCHECKED, 0);
		SetDlgItemText(IDC_RE0+i, "");
		SetDlgItemText(IDC_LOFREQ0+i, "-----");
		SetDlgItemText(IDC_HIFREQ0+i, "-----");
	}
	SetDlgItemText(IDC_NUMBANDS, "");
	SetDlgItemText(IDC_NUMMAX, "");
}

ACESET_API void CACEDlg::SetAddMLines(const char *str)
{
	if (strlen(str)>=(size_t)param->lenAddmlines)
	{
		if (param->lenAddmlines>0)		delete[] param->addmlines;
		param->lenAddmlines = (int)strlen(str) + 64;
		param->addmlines = new char[param->lenAddmlines];
	}
	strcpy(param->addmlines, str);
}

ACESET_API void CACEDlg::OpenMap(const char* mapfilefull, InterfaceType iProc)
{
	onOpenMap(mapfilefull, iProc);
}

ACESET_API int CACEDlg::Init(const char* dllname, char* errstring)
{
	static HMODULE hLib(NULL);

	if (hLib==NULL) //this needs to be done only once, therefore hLib is static
	{
		hLib = LoadLibrary(dllname);
		if (hLib==NULL)
		{	sprintf(errstring,"%s cannot be loaded", dllname); return 0;	}
	}

	CHECKNULL(InitMatlabLib = (PF_INITMATLAB)GetProcAddress(hLib, "InitMatlabLib"))
	CHECKNULL(CloseMatlabLib = (PF_VOID)GetProcAddress(hLib, "CloseMatlabLib"))
	CHECKNULL(GetCutoffFreqs = (PF_INT_FREQ)GetProcAddress(hLib, "GetCutoffFreqs"))
	CHECKNULL(GateProcess = (PF_INT_GATEPROC)GetProcAddress(hLib, "GateProcess"))
	CHECKNULL(newCParamACE = (PF_CPARAM)GetProcAddress(hLib, "newCParamACE"))

	// Very important line... must be done prior to any real operation.
	// something you would do during construction (which is not possible 
	// because you are loading acproc.dll explicitly.
	// Oct-28-2010, BJ Kwon
	param = newCParamACE();
	errstr="";
	return 1;
}

int CACEDlg::UpdateCutoffFreq ()
{
	int howmany(0);
	double *freqs, freqLimits[] = { 188, 7938 };

	static double wAceFreqs[][23] = {
		{188,250,312.5,375,500,625,750,1000,1250,1500,1750,2000,2250,2500,2750,3000,3500,4000,4500,5000,6000,7000,8000},
		{188,250,312.5,375,500,625,750,1000,1250,1500,1750,2000,2250,2500,     3000,3500,4000,4500,5000,6000,7000,8000},
		{188,250,312.5,375,500,625,750,1000,1250,1500,1750,2000,2250,2500,     3000,3500,     4500,5000,6000,7000,8000},
		{188,250,312.5,375,500,625,750,1000,1250,1500,1750,2000,     2500,     3000,3500,     4500,5000,6000,7000,8000},
		{188,250,312.5,375,500,    750,1000,1250,1500,1750,2000,     2500,     3000,3500,     4500,5000,6000,7000,8000},
		{188,250,      375,500,    750,1000,1250,1500,1750,2000,     2500,     3000,3500,     4500,5000,6000,7000,8000},
	};

	//Count how many bands are selected
	for (int i=0; i<22; i++)
		if	(SendDlgItemMessage (IDC_ELEC_CHECK0+i,  BM_GETCHECK, 0, 0)==BST_CHECKED)
			howmany++;

	SetDlgItemInt (IDC_NUMBANDS, howmany, 0);

	if (is_wACE(param)) {
		if (howmany < 17) {
			MessageBox ("Too few electrodes are selected. The minimum is 17.", "Error");
			return 0;
		}
		freqs = wAceFreqs[22-howmany];
	} else {
		freqs = new double [howmany+1];
		if (GetCutoffFreqs(howmany, freqLimits, freqs)==-1)
		{ MessageBox ("GetCutoffFreqs not found in mxace__.dll", "Error"); delete[] freqs; return 0;}
	}

	double lower, upper;
	CStdString strLower, strUpper;
	for (int cumInd(0), i=0; i<22; i++)
	{
		if	(SendDlgItemMessage (IDC_ELEC_CHECK0+i,  BM_GETCHECK, 0, 0)==BST_CHECKED)
		{
			param->freqsL[i] = lower = freqs[cumInd];
			param->freqsU[i] = upper = freqs[cumInd+1];
			SetDlgItemInt (IDC_LOFREQ0+i, (int)(lower+.5), 0);
			SetDlgItemInt (IDC_HIFREQ0+i, (int)(upper+.5), 0);
			++cumInd;
		}
		else
		{
			SetDlgItemText (IDC_LOFREQ0+i, "---");
			SetDlgItemText (IDC_HIFREQ0+i, "---");
		}
	}
	if (!is_wACE(param)) delete[] freqs; 
	return 1;
}