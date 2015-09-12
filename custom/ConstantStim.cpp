// ConstantStim.cpp : implementation file
//

#include "stdafx.h"
#include "psycon.h"
#include "ConstantStim.h"
#include "psyconDlg.h"

// CConstantStim dialog

IMPLEMENT_DYNAMIC(CConstantStim, CDialog)

CConstantStim::CConstantStim(CPsyconDlg* pParent /*=NULL*/)
	: CProcDialog(CConstantStim::IDD, pParent),
	hPar(pParent),
	m_instruction(""),
	varlistStr(""),
	nIntervals(0),
	nTrials(0),	nSteps(0), nVars(0)
{
	varNames = new string[2];
}

CConstantStim::~CConstantStim()
{
	delete[] varNames;
	delete m_crv;
}

void CConstantStim::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TRIALS, nTrials);
	DDX_Text(pDX, IDC_IND_VAR_STR, varlistStr);
	DDX_Text(pDX, IDC_INST, m_instruction);
}


BEGIN_MESSAGE_MAP(CConstantStim, CDialog)
	ON_EN_KILLFOCUS(IDC_IND_VAR_STR, &CConstantStim::OnEnKillfocusIndVarStr)
END_MESSAGE_MAP()


// CConstantStim message handlers

int CConstantStim::ProcessList()
{
	// after parsing is done, member variable  var (type: CEqSig) contains the values of variables in the order described below.
	// psyntegDlg.cpp can use var for the values and length (which is twice the number of conditions)
	//returns the number of "local tag" variables to be used in each trial, 0 if only the default variable ("v") is used. -1 for error.
	nVars=0, nSteps=0;
	((CPsyconDlg*)hPar)->PrepareTags();
	CAstSig ast(&hPar->AstSig);
	char *MsgTitle = "Independent Variables";
	UpdateData();
	try {
		ast.SetNewScript(varlistStr);
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
			var = ast.Eval(p);
			nSteps = var.nSamples;
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

		CSignals addnum;
		addnum.UpdateBuffer(2);
		if (nVars == 2) {
			// For now, only two variables are supported.
			// They will be aligned in the order looping the second var first.
			var.Reset(-1);
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
						var += &addnum;
						nSteps++;
					}
				}
			}
		} else if (pCondNode)
			throw "Condition requires two variable definitions.";
	} catch (const char *errmsg) {
		MessageBox(errmsg, MsgTitle);
		return -1;
	}
	for (int i=0; i<nVars; i++)	// initialize variables for out-of-procedure plot & play
		ast.SetTag(varNames[i].c_str(), var.buf[i]); 

	SetDlgItemInt (IDC_IND_VAR_COUNT_STATIC, nSteps);
	return nVars;
}


void CConstantStim::GenLogStr(CString &str, string tagStrSpaceDelim, short *cumCorr, short *cumTrials)
{
	char buf[16], *tag[2], tag1[16], tag2[16];
	tag[0]=tag1; tag[1]=tag2;
	CString st, tagStr;
	str="";
	if (nVars==0)	strcpy(tag[0],"v");
	else if (nVars==1) 
	{
		trim(tagStrSpaceDelim, " ");
		strcpy(tag[0], tagStrSpaceDelim.c_str());
	}
	else
		str2array(tag, 2,  tagStrSpaceDelim.c_str(), " ");
	for (int i=0; i<nSteps; i++)
	{
		if (nVars==0)
		{
			sprintfFloat(var.buf[i], 3, buf, sizeof(buf));
			st.Format("%s=%s ", tag[0], buf);
			str += st;
		}
		else
			for (int j=0; j<nVars; j++)
			{
				sprintfFloat(var.buf[nVars*i+j], 3, buf, sizeof(buf));
				st.Format("%s=%s ", tag[j], buf);
				str += st;
			}
		st.Format("%d/%d correct\r\n", cumCorr[i], cumTrials[i]);
		str += st;
	}
	str += "\r\n";
}


LRESULT CConstantStim::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message==WM_COMMAND)
		if (wParam==IDOK || wParam==IDCANCEL) 
		return NULL;
	
	return CDialog::WindowProc(message, wParam, lParam);
}
void CConstantStim::OnEnKillfocusIndVarStr()
{
	ProcessList();
}

BOOL CConstantStim::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	//OnEnKillfocusIndVarStr();
	m_crv = (CConstant_resview*)new CConstant_resview();
	m_crv->hPar = this;
	m_crv->Create(IDD_CONSTANT_LOG, this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
