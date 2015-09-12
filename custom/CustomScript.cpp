// CustomScript.cpp : implementation file
//
#include "stdafx.h"
#include "psycon.h"
#include "psyconDlg.h"
#include "CustomScript.h"
#include <algorithm>
// CCustomScript dialog

IMPLEMENT_DYNAMIC(CCustomScript, CDialog)

CCustomScript *GpCustomScript;

CCustomScript::CCustomScript(CPsyconDlg* pParent /*=NULL*/)
	: CProcDialog(CCustomScript::IDD, pParent),
	hPar(pParent), m_Script(_T("")), hEvent(NULL),
	AstSig(&pParent->AstSig)
{
	GpCustomScript = this;
}

CCustomScript::~CCustomScript()
{
}

void CCustomScript::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CUSTOM_SCRIPT, m_Script);
}


BEGIN_MESSAGE_MAP(CCustomScript, CDialog)
	//AFX_PMSG
	ON_BN_CLICKED(IDC_EXT_EDITOR, OnBnClickedExtEditor)
END_MESSAGE_MAP()


// CCustomScript message handlers

LRESULT CCustomScript::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message==WM_COMMAND)
		if (wParam==IDOK || wParam==IDCANCEL) 
			return NULL;
	
	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CCustomScript::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCustomScript::OnBnClickedExtEditor()
{
	UpdateData();
	MessageBox(m_Script, "Ext. editor clicked.");
	return;
}

int handleHookAST(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	string HookName;
	const AstNode *args;

	if (pnode->str[0] == '#') {
		HookName = pnode->str+1;
		args = p;
	} else {
		const char *fnsigs[] = {
			"(name_string, ...)", 0};
		HookName = ast.ComputeString(p);
		checkNumArgs(pnode, p, fnsigs, 1, 0);
		args = p->next;
	}

	if (HookName == "INIT_RESPONSE") {
		const char *fnsigs[] = {
			"(num_interval [, instruction_string])", 0};
		checkNumArgs(pnode, args, fnsigs, 1, 2);
		ast.Compute(args);
		if (!ast.Sig.IsScalar())
			throw CAstException(pnode, args, fnsigs, "argument must be a scalar.");
		int n = round(ast.Sig.value());

#define FLY_ERR(x) {\
	char linestr[256], estr[128]; int ecode = (x); \
	if (ecode<0) { flyErrMsg(ecode, estr); wsprintf (linestr, "Error occurred at "#x", code=%d", ecode); throw CAstException(pnode, "flySend...() failed:", linestr);} \
}
		LONG sentobj = MAKELONG((WORD)n, (WORD)n);
		FLY_ERR(flySendBin(FL_INIT, &sentobj, sizeof(sentobj)))
		string instruction = (args->next) ? ast.ComputeString(args->next) : "";
		FLY_ERR(flySendText(FL_INSTRUCTION, instruction.c_str()))
		if (WaitForSingleObject(GpCustomScript->hEvent, INFINITE) != WAIT_OBJECT_0)
			throw CAstException(pnode, "WaitForSingleObject() failed -", GetLastError());
	} else if (HookName == "BUTTON") {
		const char *fnsigs[] = {
			"(index, \"command\" [,# of buttons])", 0};
		checkNumArgs(pnode, args, fnsigs, 1, 3);
		ast.Compute(args);
		if (!ast.Sig.IsScalar())
			throw CAstException(pnode, args, fnsigs, "argument must be a scalar.");
		BYTE iBtn = (BYTE)round(ast.Sig.value());
		if (iBtn > 999)
			throw CAstException(pnode, "Button index is too big (max. 999)", iBtn);
		BYTE nBtn = 0, fShow = 0, fEnable = 0;
		if (args->next) {
			string cmd = ast.ComputeString(args->next);
			std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
			if (cmd.find("show") != string::npos)
				fShow = 2;
			else if (cmd.find("hide") != string::npos)
				fShow = 1;
			if (cmd.find("enable") != string::npos)
				fEnable = 2;
			else if (cmd.find("disable") != string::npos)
				fEnable = 1;
			if (cmd.find("text") != string::npos) {
				if (args->next->next) {
					char buf[10];
					sprintf(buf, "%d:", iBtn);
					string txt = buf + ast.ComputeString(args->next->next);
					FLY_ERR(flySendText(FL_BUTTON_TEXT, txt.c_str()))
				} else
					throw CAstException(pnode, args->next, fnsigs, "3rd argument is required when \"text\" command is used.");
			} else if (args->next->next) {
				ast.Compute(args->next->next);
				if (!ast.Sig.IsScalar())
					throw CAstException(pnode, args->next->next, fnsigs, "argument must be a scalar.");
				nBtn = round(ast.Sig.value());
			}
		} else
			fShow = 2;
		if (nBtn || fShow || fEnable) {
			LONG lv = MAKELONG(MAKEWORD(iBtn, nBtn), MAKEWORD(fShow, fEnable));
			FLY_ERR(flySendBin(FL_BUTTON, &lv, sizeof(lv)))
		}
	} else if (HookName == "RESPONSE" || HookName == "RECEIVE" ) {
		const char *fnsigs[] = {
			"(message_string [, \"special_command\"])", 0};
		checkNumArgs(pnode, args, fnsigs, 1, 2);
		string instruction;
		DWORD timeout = INFINITE;
		if (args)
			instruction = ast.ComputeString(args);
		if (args->next) {
			string cmd = ast.ComputeString(args->next);
			std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
			if (cmd.find("sequence") != string::npos) {
				FLY_ERR(sendfly(FL_RECEIVE_SEQ, instruction.c_str(), 0))
			}
			size_t p;
			if ((p = cmd.find("timeout=")) != string::npos) {
				if (sscanf(cmd.c_str()+p, "timeout=%d", &timeout) != 1)
					throw CAstException(args->next, "'timeout=' option requires an integer -", cmd.c_str()+p);
			}
		} else
			FLY_ERR(sendfly(FL_RECEIVE, instruction.c_str(), 0))
		while (GpCustomScript->LastResponse < 0 && !ast.isInterrupted()) {
			switch (WaitForSingleObject(GpCustomScript->hEvent, timeout)) {
			case WAIT_OBJECT_0:
				break;
			case WAIT_TIMEOUT:
				flySendText(FL_RECEIVE_SEQ_END, "");
				break;
			default:
				throw CAstException(pnode, "WaitForSingleObject() failed", GetLastError());
			}
		}
		if (GpCustomScript->LastResponse > 0)
			ast.Sig.SetValue(GpCustomScript->LastResponse);
		else if (GpCustomScript->LastResponse == 0)
			ast.Sig = GpCustomScript->LastRespSig;
		GpCustomScript->LastResponse = -1;
	} else if (HookName == "FEEDBACK"
			|| HookName == "INSTRUCT") {
		const char *fnsigs[] = {
			"(message_string)", 0};
		checkNumArgs(pnode, args, fnsigs, 1, 1);
		string msg = ast.ComputeString(args);
		if (HookName == "FEEDBACK")
			FLY_ERR(flySendText(FL_FEEDBACK, msg.c_str()))
		else if (HookName == "INSTRUCT")
			FLY_ERR(flySendText(FL_INSTRUCT, msg.c_str()))
	} else
		return -1;
	return 0;
}


void ThreadCustomScript(void *pCCustomScript)
{
	CCustomScript *pDlg = (CCustomScript *)pCCustomScript;
	srand((unsigned int)time(NULL) ^ (unsigned int)GetCurrentThreadId());
	try {
		pDlg->AstSig.Compute();
	} catch (const char *errstr) {
		pDlg->MessageBox(errstr, "Custom Script Runtime Error", MB_ICONQUESTION);
 	}
	pDlg->hPar->DoneSession();
	_endthread();
}

int CCustomScript::OnBnClickedGo()
{
	UpdateData();
	if (CString(m_Script).Trim().IsEmpty()) {	// make a copy of m_Script so that we can check for emptiness without affecting the line number
		MessageBox("Script is empty.", "Error", MB_ICONQUESTION);
		return 0;
	}
	try {
		AstSig.Reset();
		AstSig.CallbackHook = handleHookAST;
		AstSig.SetNewScript(m_Script);
	} catch (const char *errstr) {
		MessageBox (errstr, "Custom Script Syntax Error", MB_ICONQUESTION);
		return 0;
 	}
	if (hEvent)
		CloseHandle(hEvent);
	hEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("PsyconCustomScriptEvent")); 
	if (hEvent == NULL) {
		CString errstr;
		errstr.Format("CreateEvent failed (%d)", GetLastError());
		MessageBox(errstr, "Error", MB_ICONQUESTION);
		return 0;
	}
	LastResponse = -1;
	_beginthread(ThreadCustomScript, 0, this);
	return 1;
}

LRESULT CCustomScript::OnFlyArrived(WPARAM code, LPARAM lParam)
{
	switch (LOWORD(code)) {
	case FL_RESPONSE:
		{
			int ResLen = HIWORD(code)/sizeof(int);
			int *pInt = (int *)lParam;
			if (ResLen > 1) {
				LastRespSig.Reset(1).UpdateBuffer(ResLen);
				for (int i=0; i<ResLen; ++i)
					LastRespSig.buf[i] = pInt[i];
				LastResponse = 0;
			} else
				LastResponse = *pInt + 1;
		}
		// go through
	case FL_BEGIN:
		if (!SetEvent(hEvent)) {
			CString errstr;
			errstr.Format("SetEvent failed (%d)\n", GetLastError());
			MessageBox(errstr, "Error", MB_ICONQUESTION);
		}
		return 0;
	}
	return 1;
}

int CCustomScript::OnBnClickedStop()
{
	AstSig.interrupt();
	if (!SetEvent(hEvent)) {
		CString errstr;
		errstr.Format("SetEvent failed (%d)\n", GetLastError());
		MessageBox(errstr, "Error", MB_ICONQUESTION);
	}
	return 1;
}

int CCustomScript::DoneSession()
{
	return 1;
}