#include "Echo.h"
#include "EchoMainWnd.h"
#include "multiProc.h"

#define MAX_LEN_ITEM    64

CmultiProc::CmultiProc()
{
	nPresets = 1;
	nAddprocs = 1;
	PresenterSettings = new char*[nPresets];
	AddProcs = new char*[nAddprocs];
	PresenterSettings[0] = new char[MAX_LEN_ITEM];
	AddProcs[0] = new char[MAX_LEN_ITEM];
	AddProcs[0][0]='\0';
	PresenterSettings[0][0]='\0';
}

CmultiProc::~CmultiProc()
{
	CString buf;
	char errStr[256], iniFile[MAX_PATH];
	sprintf (iniFile, "%s%s", MainAppWnd.AppPath, INI_NAME);

	GetDlgItemText (IDC_ADDPROC, buf);
	printfINI(errStr, iniFile, "MULTI WAV PROCESSING", "%s", buf);
	buf.Empty();
	GetDlgItemText (IDC_PRESENT_CONTROL, buf);
	printfINI(errStr, iniFile, "MULTI PRESENTER SETTINGS", "%s", buf);

	CleanAllocced();
}

void CmultiProc::Alloc()
{
	int i;
	PresenterSettings = new char*[nPresets];
	AddProcs = new char*[nAddprocs];
	for (i=0; i<nAddprocs; i++)
	{
		AddProcs[i] = new char[MAX_LEN_ITEM];
		AddProcs[i][0]='\0';
	}
	for (i=0; i<nPresets; i++)
	{
		PresenterSettings[i] = new char[MAX_LEN_ITEM];
		PresenterSettings[i][0]='\0';
	}

}

void CmultiProc::CleanAllocced()
{
	int i;
	for (i=0; i<nAddprocs; i++)
		delete[] AddProcs[i];
	for (i=0; i<nPresets; i++)
		delete[] PresenterSettings[i];
	delete[] AddProcs;
	delete[] PresenterSettings;
}


int CmultiProc::ReadyProcessingStrings()
{
	char buf[1024], buf2[1024];

	CleanAllocced();
	GetDlgItemText (IDC_PRESENT_CONTROL, buf, sizeof(buf));
	trimLeft(buf, " \r\n"); trimRight(buf, " \r\n");
	SetDlgItemText (IDC_PRESENT_CONTROL, buf);
	nPresets = max(1,countDeliminators(buf, "\r\n"));

	GetDlgItemText (IDC_ADDPROC, buf2, sizeof(buf2));
	trimLeft(buf2, " \r\n"); trimRight(buf2, " \r\n");
	SetDlgItemText (IDC_ADDPROC, buf2);
	nAddprocs = enumVarStr(buf2, TrialVarNames, TrialVarValues);
	if (nAddprocs < 0)
		return 0;
	nAddprocs = max(1, nAddprocs);
	Alloc();
	if (nPresets>1)
		str2strarray(PresenterSettings, nPresets, buf, "\r\n");
	else
	{
		strcpy(PresenterSettings[0], buf);
		::SetDlgItemText (MainAppWnd.hDlg, IDC_PRESENT_CONTROL, buf);
	} 
	for (int i=0; i<nAddprocs; ++i) {
		AddProcs[i][0] = '\0';
		for (unsigned j=0; j<TrialVarNames.size(); ++j) {
			if (j)
				strcat(AddProcs[i], ",");
			sprintf(AddProcs[i]+strlen(AddProcs[i]), "%s=%d", TrialVarNames[j].c_str(), round(TrialVarValues.buf[TrialVarNames.size()*i+j]));
		}
	}
	return 1;
}

int CmultiProc::setNth(const int index, const vector<string> &varNames, const CSignals &varlist)
{
	int nVar = varNames.size();
	for (int i=0; i<nVar; ++i)
		MainAppWnd.AstSig.SetTag(varNames[i].c_str(), varlist.buf[index*nVar+i]);
	return 1;
}


int CmultiProc::enumVarStr(const string varlistStr, vector<string> &varNames, CSignals &varlist)
// Return values
// positive: Number of steps
// 0: Nothing to process
// -1: Error, message box already shown
{
	int nVars=0, nSteps=0;
	//PrepareTags();	// initialize AstSig global tag
	CAstSig ast(&MainAppWnd.AstSig);	// get the AstSig global environment
	char *MsgTitle = "Independent Variables";

	varNames.clear();
	varNames.resize(2);
	try {
		ast.SetNewScript(varlistStr.c_str());
		AstNode *p = ast.GetAst(), *pCondNode = NULL;
		if (!p)	// empty list
			return 0;
		CSignals sig[2];
		ostringstream oss;
		switch (p->type) {
		case '=':
		case NODE_VECTOR:
			nVars = 1;
			varNames[0] = (p->type == '=') ? p->str : "v";
			varlist = ast.Eval(p);
			nSteps = varlist.nSamples;
			break;
		case NODE_BLOCK:
			for (p = p->child; p; p=p->next) {
				if (p->type == '=') {
					if (++nVars > 2) {
						oss << "Line " << p->line << ": " << "Third variable definition - variable list can have at most two variable definitions.";
						MessageBox(oss.str().c_str(), MsgTitle);
						return -1;
					}
					varNames[nVars-1] = p->str;
					sig[nVars-1] = ast.Eval(p);
				} else if (ast.IsCondition(p)) {
					if (pCondNode) {
						oss << "Line " << p->line << ": " << "Second condition line - variable list can have only one condition line.\nUse '&&' or '||' operators for multiple comparisons.";
						MessageBox(oss.str().c_str(), MsgTitle);
						return -1;
					}
					pCondNode = p;
				} else if (p->type == NODE_VECTOR) {
					oss << "Line " << p->line << ": " << "Untagged vector cannot be used in multi-line variable list.";
					MessageBox(oss.str().c_str(), MsgTitle);
					return -1;
				} else {
					oss << "Line " << p->line << ": " << "Cannot be used in variable list.";
					MessageBox(oss.str().c_str(), MsgTitle);
					return -1;
				}
			}
			if (nVars)
				break;
			// go through
		default:
			throw "Variable list must have at least one variable definition or vector.";
		}
		varNames.resize(nVars);

		CSignals addnum;
		addnum.UpdateBuffer(2);
		if (nVars == 2) {
			// For now, only two variables are supported.
			// They will be aligned in the order looping the second var first.
			varlist.Reset(-1);
			for (int i=0; i<sig[0].GetLength(); i++) {
				ast.SetTag(varNames[0].c_str(), sig[0].buf[i]);
				for (int j=0; j<sig[1].GetLength(); j++) {
					if (pCondNode) {
						ast.SetTag(varNames[1].c_str(), sig[1].buf[j]);
						ast.Eval(pCondNode);
					}
					if (!pCondNode || ast.Sig.value()) {
						addnum.buf[0] = sig[0].buf[i];
						addnum.buf[1] = sig[1].buf[j];
						varlist += &addnum;
						++nSteps;
					}
				}
			}
		} else if (pCondNode)
			throw "Condition requires two variable definitions.";
	} catch (const char *errmsg) {
		MessageBox(errmsg, MsgTitle);
		return -1;
	}

	return nSteps;
}

#define BUFSIZE 4096

void CmultiProc::OnTimer(HWND hwnd, UINT id)
{
	if (id==SETPARAM)
	{
		KillTimer(id);
		int res;
		char errStr[256], iniFile[MAX_PATH], *buf2read;
		sprintf (iniFile, "%s%s", MainAppWnd.AppPath, INI_NAME);

		buf2read = (char*)calloc(sizeof(char), BUFSIZE);
		if ((res=strscanfINI(errStr, iniFile, "MULTI WAV PROCESSING", buf2read, BUFSIZE))>=0)
			SetDlgItemText (IDC_ADDPROC, buf2read);
		if ((res=strscanfINI(errStr, iniFile, "MULTI PRESENTER SETTINGS", buf2read, BUFSIZE))>=0)
			SetDlgItemText (IDC_PRESENT_CONTROL, buf2read);
		free(buf2read);
	}
}

void CmultiProc::OnDestroy()
{
	hDlg = NULL;
}


BOOL CmultiProc::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
	return 1;
}

void CmultiProc::OnCommand(int idc, HWND hwndCtl, UINT event)
{
	int i, res;
	switch (idc)
	{
	case IDC_GO:
		if (!ReadyProcessingStrings())
			break;

		i=GetDlgItemInt (IDC_NUM_TRIALS, &res, 0);
		if (!res)
		{
			MessageBox ("Invalid number of trials");
			break;
		}
		nTrials = i;
		::SetDlgItemInt (MainAppWnd.hDlg, IDC_NUM_TRIALS, i, 0);
		SetDlgItemInt (IDC_TOTALTRIALS, i*nAddprocs*nPresets, 0);
		::SendMessage(MainAppWnd.hDlg, WM_COMMAND, MAKELONG(IDC_BEGIN, USR_MULTI), (LPARAM)hwndCtl);
		ShowWindow (hDlg, SW_HIDE);
		EnableDlgItem(hDlg, IDC_GO, 0);
		break;
	case WM_DESTROY:
		ShowWindow (hDlg, 0);
		break;
	}
}

BOOL CALLBACK  multiProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hpar;
	switch (umsg)
	{
	chHANDLE_DLGMSG (hDlg, WM_COMMAND, MainAppWnd.hMulti->OnCommand);
	chHANDLE_DLGMSG (hDlg, WM_INITDIALOG, MainAppWnd.hMulti->OnInitDialog);
	chHANDLE_DLGMSG (hDlg, WM_DESTROY, MainAppWnd.hMulti->OnDestroy);
	chHANDLE_DLGMSG (hDlg, WM_TIMER, MainAppWnd.hMulti->OnTimer);
	default:		return 0;
	}
	return 1;
}