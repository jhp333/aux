#include <sstream>
#include <list>
#include <exception>
#include <math.h>
#include <time.h>
#include "calbase.h"
#include "sigproc.h"
#include "psycon.tab.h"

#ifndef LINK_STATIC_SIGPROC
HMODULE hDllModule;
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	hDllModule = hModule;
	if (ul_reason_for_call == 1 || ul_reason_for_call == 2)
		srand((unsigned int)time(NULL) ^ (unsigned int)GetCurrentThreadId());
    return TRUE;
}
#endif

void CAstSig::initGlobals(const CAstSig *env)
{
	if (env) {
		pEnv = env->pEnv;
		pEnv->RefCount++;
	} else
		pEnv = new CAstSigEnv(DefaultFs);
}


CAstSig::CAstSig(const CAstSig &org)
{
	throw "Internal error: Copy constructor is prohibited. You probably meant to call CAstSig(&env)." ;
}


CAstSig::CAstSig(const CAstSig *env)
: pAst(NULL), Script(""), fAllocatedAst(false), CallbackCIPulse(NULL), CallbackHook(env->CallbackHook)
{
	initGlobals(env);
}

CAstSig::CAstSig(const char *str, const CAstSig *env)
: pAst(NULL), fAllocatedAst(false), CallbackCIPulse(NULL), CallbackHook(env->CallbackHook)
{
	initGlobals(env);
	SetNewScript(str);
}

CAstSig::CAstSig(AstNode *pnode, const CAstSig *env)
: pAst(pnode), Script(""), fAllocatedAst(false), CallbackCIPulse(NULL), CallbackHook(env->CallbackHook)
{
	initGlobals(env);
}


CAstSig::CAstSig(const int fs)
: pAst(NULL), Script(""), fAllocatedAst(false), pEnv(new CAstSigEnv(fs)), CallbackCIPulse(NULL), CallbackHook(NULL)
{
}

CAstSig::CAstSig(const char *str, const int fs)
: pAst(NULL), fAllocatedAst(false), pEnv(new CAstSigEnv(fs)), CallbackCIPulse(NULL), CallbackHook(NULL)
{
	SetNewScript(str);
}

CAstSig::CAstSig(AstNode *pnode, const int fs)
: pAst(pnode), Script(""), fAllocatedAst(false), pEnv(new CAstSigEnv(fs)), CallbackCIPulse(NULL), CallbackHook(NULL)
{
}


CAstSig::~CAstSig()
{
	if (pEnv->RefCount > 0)
		pEnv->RefCount--;
	else
		delete pEnv;
	if (fAllocatedAst)
		yydeleteAstNode(pAst, 0);
}


CAstSig &CAstSig::SetNewScript(const char *str)
{
	int res;
	char *errmsg;

	if (pAst && Script == str)
		return *this;
	if (fAllocatedAst) {
		yydeleteAstNode(pAst, 0);
		fAllocatedAst = false;
	}

	pAst = NULL;
	if ((res = yysetNewStringToScan(str)))
		throw "yysetNewStringToScan() failed!";
	res = yyparse(&pAst, &errmsg);
	fAllocatedAst = pAst ? true : false;

	if (!errmsg && res == 2)
		errmsg = "Out of memory!";
	if (errmsg) {
		if (fAllocatedAst) {
			yydeleteAstNode(pAst, 0);
			fAllocatedAst = false;
			pAst = NULL;
		}
		throw errmsg;
	}
	Script = str;
	return *this;
}


CAstSig &CAstSig::SetNewFile(FILE *source)
{
	int res;
	char *errmsg;

	if (fAllocatedAst) {
		yydeleteAstNode(pAst, 0);
		fAllocatedAst = false;
	}

	pAst = NULL;
	if ((res = yysetNewFileToScan(source)))
		throw "yysetNewFileToScan() failed!";
	res = yyparse(&pAst, &errmsg);
	fAllocatedAst = pAst ? true : false;

	if (!errmsg && res == 2)
		errmsg = "Out of memory!";
	if (errmsg) {
		if (fAllocatedAst) {
			yydeleteAstNode(pAst, 0);
			fAllocatedAst = false;
			pAst = NULL;
		}
		throw errmsg;
	}
	Script = "";
	return *this;
}


CAstException::CAstException(const AstNode *p, const string s1, const string s2)
: pnode(p), str1(s1), str2(s2), int1(0)
{
	makeOutStr();
}

CAstException::CAstException(const AstNode *p, const string s1, const int x, const string s2)
: pnode(p), str1(s1), str2(s2), int1(x)
{
	makeOutStr();
}

void CAstException::makeOutStr()
{
	ostringstream oss;
	oss << "Line " << pnode->line << ", Col " << pnode->column << ": " << str1;
	if (!str2.empty())
		oss << " \"" << str2 << '"';
	if (int1)
		oss << int1;
	outstr = oss.str();
}


bool GfInterrupted;	// needs to be global to interrupt even inside UDF
void CAstSig::interrupt(void)
{
	GfInterrupted = true;
}
bool CAstSig::isInterrupted(void)
{
	return GfInterrupted;
}


CSignals &CAstSig::Compute(void)
{
	try {
		if (!pAst)
			return Sig;
		GfInterrupted = false;
		Tick0 = GetTickCount();
		if (pAst->type == NODE_BLOCK) {	// main workspace, scan for local functions
			AstNode *pp = NULL;
			for (AstNode *p=pAst->child; p;) {
				if (p->type == T_FUNCTION) {
					if (AstNode *pOld=RetrieveUDF(p->str))
						yydeleteAstNode(pOld, 0);	// delete AST tree of existing function with the same name
					pEnv->UDFs[p->str] = p;
					// detach it from pAst tree so that it will remain available even after pAst is replaced by SetNewScript()
					if (pp) {	// take p out of pAst tree
						pp->next = p->next;
						p->next = NULL;
						p = pp;
					} else {	// replacing head node (first line is function definition)
						pAst->child = p->next;
						p->next = NULL;
						p = pAst->child;
						continue;
					}
				}
				pp = p;
				p = p->next;
			}
		}
		Compute(pAst);
		Tick1 = GetTickCount();
		return Sig;
	} catch (const CAstException &e) {
		char errmsg[500];
		strncpy(errmsg, e.getErrMsg().c_str(), sizeof(errmsg)/sizeof(*errmsg));
		errmsg[sizeof(errmsg)/sizeof(*errmsg)-1] = '\0';
		throw errmsg;
	}
}


CSignals &CAstSig::Eval(AstNode *pnode)
{
	try {
		if (!pnode)
			return Sig;
		return Compute(pnode);
	} catch (const CAstException &e) {
		char errmsg[500];
		strncpy(errmsg, e.getErrMsg().c_str(), sizeof(errmsg)/sizeof(*errmsg));
		errmsg[sizeof(errmsg)/sizeof(*errmsg)-1] = '\0';
		throw errmsg;
	}
}


CSignals &CAstSig::Compute(const AstNode *pnode)
{
	CSignals tsig, isig, *psig;
	if (!pnode)
		throw "Null AST node!";
	if (GfInterrupted)
		throw CAstException(pnode, "Script execution has been interrupted.");
	AstNode *p = pnode->child;

try {
	switch (pnode->type) {
	case T_NUMBER:
		Sig.SetValue(pnode->dval);
		break;
	case T_STRING:
		size_t len;
		len = strlen(pnode->str);
		Sig.Reset(2);
		Sig.UpdateBuffer((int)len);
		for (size_t i=0, j=0; i<len; i++, j++)
			Sig.buf[j] = pnode->str[i];
		break;
	case T_ID:
		if (p) {	/* child is for cell array index */
			tsig = Compute(p);
			if (!tsig.IsScalar())
				throw CAstException(p, "Array index must be a scalar.");
			map<int,CSignals> *pArray;
			if (!(pArray = RetrieveArray(pnode->str)))
				throw CAstException(pnode, "Unknown array - ", pnode->str);
			if (p->next) {	/* another child is for extraction by indices */
				psig = &(*pArray)[round(tsig.value())];
				p = p->next;
				goto extraction_by_indices;
			} else
				Sig = (*pArray)[round(tsig.value())];
		} else {
			if (!(psig = RetrieveTag(pnode->str)))
				throw CAstException(pnode, "Unknown tag - ", pnode->str);
			Sig = *psig;
		}
		break;
	case '=':
		if (!p)
			throw CAstException(pnode, "Internal error: Empty assignment!");
		if (p->next) {	/* another child means cell array index */
			isig = Compute(p->next);
			if (!isig.IsScalar())
				throw CAstException(p->next, "Array index must be a scalar.");
			SetCell(pnode->str, round(isig.value()), Compute(p));
		} else
			SetTag(pnode->str, Compute(p));
		break;
	case NODE_INITCELL:
		for (int i=1; p;i++, p=p->next)
			SetCell(pnode->str, i, Compute(p));
		break;
	// Be careful. Below 4 basic operators cut corners to minimize deep copying. Each of them makes only 1 deep copy.
	/* alternative method - makes 2 deep copies (assignments)
	case '/';
		tsig = Compute(p);
		tsig /= Compute(p->next); 
		Sig = tsig;
		break;
	*/
	/* This doesn't work!! Because Compute() uses/alters Sig.
	case '/';
		Compute(p);	// Sig gets the result.
		Sig /= Compute(p->next);	// Compute() alters Sig.
		break;
	*/
	case '+':
		tsig = Compute(p);
		Compute(p->next);	// Sig gets the result.
		Sig += tsig;
		break;
	case '-':
		tsig = Compute(p); 
		-Compute(p->next);
		Sig += tsig;
		break;
	case '*':
		tsig = Compute(p);
		Compute(p->next);
		Sig *= tsig;
		break;
	case '/':
		tsig = Compute(p);
		Compute(p->next).Reciprocal();
		Sig *= tsig;
		break;
	case T_NEGATIVE:
		-Compute(p);
		break;
	case '@':
		tsig = Compute(p->next);
		//if (!tsig.IsScalar())
		//	throw CAstException(p->next, "Last operand of '@' must be a scalar.");
		//else
		{
			bool fTrinary = false;
			CSignals rms2;
			if (p->type == '@') {	// trinary?
				CSignals second = Compute(p->child->next);
				if (!second.IsScalar()) {	// trinary!
					fTrinary = true;
					rms2 = second.LevelLTmc();
					Compute(p->child);	// Sig gets the first operand
				}
			}
			if (!fTrinary) {	// binary! - the first operand was not computed yet.
				rms2.SetValue(-0.000262);	// the calculated rms value of a full scale sinusoid (this is necessary to avoid the clipping of rms adjusted full scale sinusoid)
				Compute(p);
			}
			// Now Sig always has the first operand
			if (Sig.IsScalar())
				throw CAstException(pnode, "First operand of '@' must be a signal.");
			CSignals rms = -Sig.LevelLTmc();
			if (rms.nSamples == 1) {
				if (rms2.nSamples > 1)
					throw CAstException(p->child->next, "2nd signal must be mono when 1st one is mono.");
				if (rms.nSamples == 1 && tsig.nSamples > 1)
					throw CAstException(p->next, "Last operand must be scalar when 1st one is mono.");
			}
			//Sig *= exp((tsig - rms + rms2) * LOG10DIV20);
			rms += rms2;
			rms += tsig;
			rms *= LOG10DIV20;
			Sig *= rms.each(exp).transpose1();
		}
		break;
	case NODE_INTERPOL:
		tsig = Compute(p->next);
		tsig.MakeChainless();
		for (int i=0; i<tsig.nSamples; ++i)
			tsig.buf[i] = exp(tsig.buf[i] * LOG10DIV20);
		isig = Compute(p->next->next);
		isig.MakeChainless();
		tsig = Sig.Reset(GetFs()).Interp(tsig, isig);
		Compute(p);
		Sig *= tsig;
		break;
	case T_OP_SHIFT:
		tsig = Compute(p->next);
		if (!tsig.IsScalar())
			throw CAstException(p->next, "Second operand of '>>' must be a scalar.");
		Compute(p);
		Sig >>= tsig.value();
		break;
	case T_OP_SHIFT2:
		tsig = Compute(p->next);
		if (!tsig.IsScalar())
			throw CAstException(p->next, "Second operand of '<<' must be a scalar.");
		Compute(p);
		Sig <<= tsig.value();
		break;
	case T_OP_CONCAT:
		tsig = Compute(p->next);
		Compute(p);
		Sig += &tsig;
		break;
	case '<':
		tsig = Compute(p); 
		Sig =  tsig < Compute(p->next);
		break;
	case '>':
		tsig = Compute(p); 
		Sig =  tsig > Compute(p->next);
		break;
	case T_COMP_EQ:
		tsig = Compute(p);
		Sig.SetValue(tsig == Compute(p->next));
		break;
	case T_COMP_NE:
		tsig = Compute(p);
		Sig.SetValue(tsig != Compute(p->next));
		break;
	case T_COMP_LE:
		tsig = Compute(p);
		Sig = tsig <= Compute(p->next);
		break;
	case T_COMP_GE:
		tsig = Compute(p);
		Sig = tsig >= Compute(p->next);
		break;
	case T_LOGIC_NOT:
		Sig = !Compute(p);
		break;
	case T_LOGIC_AND:
		tsig = Compute(p);
		Sig = tsig && Compute(p->next);
		break;
	case T_LOGIC_OR:
		tsig = Compute(p);
		Sig = tsig || Compute(p->next);
		break;
	case T_IF:
		for (; p && p->next; p=p->next->next)	// p is a condition of 'if' or 'elseif', p->next is the code block.
			if (Compute(p).value())
				return Compute(p->next);	// no further processing of this 'if' statement.
		// now p is at the end of 'if' statement, without executing any conditional code.
		if (p)	// if not null, it's the 'else' code block
			Compute(p);
		break;
	case T_SWITCH:
		tsig = Compute(p);
		for (p=p->next; p && p->next; p=p->next->next)	// p is a case exp, pcase->next is the code block.
			if (p->type == NODE_ARGS) {
				for (AstNode *pa=p->child; pa; pa=pa->next)
					if (tsig == Compute(pa))
						return Compute(p->next);	// no further processing of this 'switch' statement.
			} else if (tsig == Compute(p))
				return Compute(p->next);	// no further processing of this 'switch' statement.
		// now p is at the end of 'case' list, without executing any conditional code.
		if (p)	// if not null, it's the 'otherwise' code block
			Compute(p);
		break;
	case T_WHILE:
		fExit=fBreak=false;
		while (Compute(p).value() && !fExit && !fBreak)
			Compute(p->next);
		fBreak = false;
		break;
	case T_FOR:
		fExit=fBreak=false;
		tsig = Compute(p->child);
		for (int i=0; i<tsig.nSamples && !fExit && !fBreak; i++) {
			SetTag(p->str, CSignals(tsig.buf[i]));
			Compute(p->next);
		}
		fBreak = false;
		break;
	case T_BREAK:
		fBreak = true;
		break;
	case T_CONTINUE:
		fContinue = true;
		break;
	case T_SIGMA:
		try {
			int n;
			tsig = Compute(p->child);
			n = 0;
			for (CAstSig SubAstSig(p->next, this); n<tsig.nSamples; n++) {
				SubAstSig.SetTag(p->str, tsig.buf[n]);
				if (n)
					Sig += SubAstSig.Compute();
				else
					Sig = SubAstSig.Compute();
			}
		} catch (const char *errmsg) {
			throw CAstException(pnode, "Calling sigma( )\n\nIn sigma expression:\n"+string(errmsg));
		}
		break;
	case T_RETURN:
		fExit = true;
		break;
	case NODE_BLOCK:
		for (fExit=fBreak=fContinue=false; p && !fExit && !fBreak && !fContinue; p=p->next)
			Compute(p);
		break;
	case NODE_VECTOR:
		if (!p) {
			Sig.Reset(1);
			break;
		}
		tsig = Compute(p);
		for (p=p->next; p; p=p->next) {
			Compute(p);
			tsig += &Sig;	// concatenate
		}
		Sig = tsig;
		break;
	case NODE_MATRIX:
		if (!p) {
			Sig.Reset(1);
			break;
		}
		tsig = Compute(p);
		//if (tsig.IsScalar())
		//	throw CAstException(p, "A scalar is not allowed here.");
		for (p=p->next,psig=&tsig; p; p=p->next,psig=psig->GetNextChan()) {
			Compute(p);
		//	if (Sig.IsScalar())
		//		throw CAstException(p, "A scalar is not allowed here.");
			psig->SetNextChan(&Sig);
		}
		Sig = tsig;
		break;
	case NODE_EXTRACT:
		double t[2];
		p = p->next;
		for (int i=0; i<2; i++,p=p->next) {
			if (!Compute(p).IsScalar())
				throw CAstException(p, "The arguments of extraction must be scalars.");
			t[i] = Sig.value();
		}
		if (t[0] > t[1]) {
			Compute(pnode->child).Trim(t[1], t[0]);
			Sig.ReverseTime();
		} else
			Compute(pnode->child).Trim(t[0], t[1]);
		break;
	case NODE_IXASSIGN:
		if (!p)
			throw CAstException(pnode, "Internal error: Empty assignment!");
		if (p->next && p->next->next)
			;// cell array indexed assignment
		else if (!(p->next && p->next->child && !p->next->child->next))
			throw CAstException(pnode, "Indexed assignment must have one argument.");
		isig = Compute(p->next->child);
		tsig = Compute(p);
		if (isig.nSamples != tsig.nSamples)
			throw CAstException(pnode, "Number of indices and number of elements on right side must be the same.");
		if (round(isig.Min()) < 1)
			throw CAstException(p->next->child, "Index cannot be smaller than 1.");
		while (!(psig = RetrieveTag(pnode->str)))
			SetTag(pnode->str, CSignals(pEnv->Fs));
		if (int maxIndex=round(isig.Max()))
			if (maxIndex > psig->GetLength())
				psig->UpdateBuffer(maxIndex);
		for (int i=0; i<isig.nSamples; i++)
			psig->buf[round(isig.buf[i])-1] = tsig.buf[i];
		break;
	case NODE_CALL:
		if (p && !p->next /* only one argument */ && (psig = RetrieveTag(pnode->str))) {
			// Extraction by indices
extraction_by_indices:
			isig = Compute(p);
			if (round(isig.Min()) < 1)
				throw CAstException(p, "Index cannot be smaller than 1.");
			if (round(isig.Max()) > psig->GetLength())
				throw CAstException(p, "Index exceeds size of vector.");
			Sig.Reset(isig.IsScalar() ? 1 : pEnv->Fs);
			Sig.UpdateBuffer(isig.nSamples);
			for (int i=0; i<isig.nSamples; i++)
				Sig.buf[i] = psig->buf[round(isig.buf[i])-1];	// -1 for one-based indexing
			break;
		} else if (AstNode *pUDF=RetrieveUDF(pnode->str)) {
			CallSub(pUDF, pnode);
			break;
		} else if (pEnv->NoAuxFiles.find(pnode->str) == pEnv->NoAuxFiles.end()) {	// if the Aux file hasn't been tried,
			string auxfilename = pnode->str;
			if (FILE *auxfile = OpenFileInPath(auxfilename, "aux")) {
				// process Aux files
				try {
					CAstSig aux;
					aux.SetNewFile(auxfile);
					fclose(auxfile);
					if (aux.pAst->type == NODE_BLOCK) {
						// multiple function definitions expected
						for (p=aux.pAst->child; p; p=p->next)
							if (p->type != T_FUNCTION)
								throw CAstException(p, "All codes in AUX file must be inside function definitions.");
						p = aux.pAst->child;
					} else
						p = aux.pAst;
					if (p->type != T_FUNCTION)
						throw CAstException(p, "AUX file must start with a function definition.");
					if (strcmp(p->str, pnode->str))
						throw CAstException(p, "The name of the first function in AUX file must be the same as the filename.");
					pEnv->UDFs[pnode->str] = p;	// p->next might be valid, which is a local function. It will be taken care of in CallSub()
					// The following should be after all the throws. Otherwise, the UDF AST will be dangling.
					// To prevent de-allocation of the AST of the UDF when the aux is destroyed.
					if (aux.pAst->type == NODE_BLOCK)
						aux.pAst->child = NULL;
					else
						aux.pAst = NULL;
				} catch (const char *errmsg) {
					fclose(auxfile);
					throw CAstException(pnode, "Calling "+string(pnode->str)+"( )\n\nIn file '"+auxfilename+"':\n"+errmsg);
				} catch (const CAstException &e) {
					fclose(auxfile);
					throw CAstException(pnode, "Calling "+string(pnode->str)+"( )\n\nIn file '"+auxfilename+"':\n"+e.getErrMsg());
				}
				CallSub(p, pnode);
				break;
			} else
				pEnv->NoAuxFiles.insert(pnode->str);	// mark it was tried.
				// go through remaining of this 'case' block.
		}
		HandleAuxFunctions(pnode);
		break;
	case NODE_CIPULSE3:
	case NODE_CIPULSE4:
	case NODE_CIPULSE5:
	case NODE_CIPULSE6:
		if (CallbackCIPulse)
			CallbackCIPulse(pnode, this);
		else
			throw CAstException(pnode, "Internal error! CI Pulse without handler!");
		break;
	case T_FUNCTION:
		// should have already been removed by Compute(void)
		throw CAstException(pnode, "Internal error! Unexpected node - T_FUNCTION");
		break;
	default:
		throw CAstException(pnode, "Internal error! Unknown node type - ", pnode->type);
	}
} catch (const char *errmsg) {
	throw CAstException(pnode, errmsg);
} catch (const exception &e) {
	throw CAstException(pnode, string("Internal error! ") + e.what());
}
	return Sig;
}


CSignals &CAstSig::CallSub(const AstNode *pUDF, const AstNode *pCall)
{
	ostringstream oss;
	// pUDF->child : parameter list
	// pUDF->child->next : UDF body

	if (!(pUDF && pUDF->type == T_FUNCTION))
		throw CAstException(pUDF, "Not a function definition!");
	if (!(pUDF->child && pUDF->child->type == NODE_IDLIST))
		throw CAstException(pUDF, "No parameter list!");

	CAstSig SubAstSig(pUDF->child->next, this->pEnv->Fs);	// does not pass 'this' so that SubAstSig has its own variable scope. Global variables are not available inside the function, for now. Might need 'global' keyword?
	SubAstSig.CallbackCIPulse = this->CallbackCIPulse;
	SubAstSig.CallbackHook = this->CallbackHook;
	AstNode *pf, *pa;	 // formal & actual parameter
	int cnt, cnt2;
	// output parameter binding
	AstNode *pOutParam = pUDF->child->next->next;
	if (pOutParam) {
		CSignals *psig;
		switch (pOutParam->type) {
		case T_ID:
			Sig.Reset();
			SubAstSig.pEnv->Refs[pOutParam->str] = &Sig;
			SubAstSig.SetTag("nargout", 1.0);
			break;
		case NODE_INITCELL:	// cell array return
			if (!(pCall->child && pCall->child->type == NODE_ARGS && pCall->child->next && pCall->child->next->type == NODE_INITCELL)) {
				oss << "Function '" << pCall->str << "' returns a cell array.";
				throw CAstException(pCall, oss.str());
			}
			pf=pOutParam, pa=pCall->child->next;
			map<int,CSignals> *pArray;
			while (!(pArray = RetrieveArray(pa->str)))	// new array, create
				pEnv->Arrays[pa->str];
			SubAstSig.pEnv->ArrRefs[pf->str] = pArray;
			SubAstSig.SetTag("nargout", 1.0);
			break;
		case NODE_VECTOR:	// multi-value return
			pf = pOutParam->child;
			if (!(pCall->child && pCall->child->type == NODE_ARGS && pCall->child->next && pCall->child->next->type == NODE_VECTOR)) {
				if (pf->type == T_ID) {
					SubAstSig.pEnv->Refs[pOutParam->child->str] = &Sig;
					SubAstSig.SetTag("nargout", 1.0);
					break;
				} else if (pf->type == NODE_INITCELL) {
					oss << "Function '" << pCall->str << "' returns a cell array at first element.";
					throw CAstException(pCall, oss.str());
				} else
					throw CAstException(pCall, "Calling "+string(pCall->str)+"( )\n\nIn function definition:\n"
								"All elements of vector on the left-hand side of assignment must be variables.");
			}
			for (pa=pCall->child->next->child, cnt=0; pf && pa; pf=pf->next, pa=pa->next, cnt++) {
				if (pf->type != T_ID && pf->type != NODE_INITCELL)
					throw CAstException(pCall, "Calling "+string(pCall->str)+"( )\n\nIn function definition:\n"
								"All elements of vector on the left-hand side of assignment must be variables.");
				switch (pa->type) {
				case T_ID:
					if (pf->type == NODE_INITCELL) {
						oss << "Function '" << pCall->str << "' returns a cell array at " << cnt+1 << "th element.";
						throw CAstException(pCall, oss.str());
					}
					while (!(psig = RetrieveTag(pa->str)))	// new variable, create
						SetTag(pa->str, CSignals());
					psig->nSamples = 0;
					SubAstSig.pEnv->Refs[pf->str] = psig;
					break;
				case NODE_INITCELL:
					if (pf->type == T_ID) {
						oss << "Function '" << pCall->str << "' returns a regular variable at " << cnt+1 << "th element.";
						throw CAstException(pCall, oss.str());
					}
					while (!(pArray = RetrieveArray(pa->str)))	// new array, create
						pEnv->Arrays[pa->str];
					SubAstSig.pEnv->ArrRefs[pf->str] = pArray;
					break;
				default:
					throw CAstException(pa, "All elements of vector on the left-hand side of assignment must be variables.");
				}
			}
			if (pa) {
				for (cnt2=0; pa; pa=pa->next, cnt2++)
					;
				oss << "Too many return arguments for function '" << pCall->str << "' - expecting " << cnt << ", supplied " << cnt+cnt2 << ".";
				throw CAstException(pCall, oss.str());
			}
			SubAstSig.SetTag("nargout", (double)cnt);
			break;
		default:
			throw CAstException(pOutParam, "Internal error! Unexpected node type - ", pOutParam->type);
		}
	}
	// input parameter binding
	pa = pCall->child;
	if (pa && pa->type == NODE_ARGS)
		pa = pa->child;
	for (pf=pUDF->child->child, cnt=0; pf && pa; pf=pf->next, pa=pa->next, cnt++) {
		if (pa->type == NODE_INITCELL) {
			if (pf->type != NODE_INITCELL) {
				oss << "Function '" << pCall->str << "' expects a non-array value at " << cnt+1 << "th parameter.";
				throw CAstException(pCall, oss.str());
			}
			if (pa->str)
				SubAstSig.pEnv->Arrays[pf->str] = pEnv->Arrays[pa->str];
			else {	// unnamed cell array, which means it's an actual initialization list.
				AstNode *p = pa->child;
				for (int i=1; p;i++, p=p->next)
					SubAstSig.SetCell(pf->str, i, Compute(p));
			}
		} else {
			if (pf->type == NODE_INITCELL) {
				oss << "Function '" << pCall->str << "' expects a cell array at " << cnt+1 << "th parameter.";
				throw CAstException(pCall, oss.str());
			}
			SubAstSig.SetTag(pf->str, Compute(pa));
		}
	}
	if (pa) {
		for (cnt2=0; pa; pa=pa->next, cnt2++)
			;
		oss << "Too many arguments for function '" << pCall->str << "' - expecting " << cnt << ", supplied " << cnt+cnt2 << ".";
		throw CAstException(pCall, oss.str());
	}
	SubAstSig.SetTag("nargin", (double)cnt);
	SubAstSig.pEnv->UDFs = pEnv->UDFs;	// copy other function definitions - especially for recursive calls.
	SubAstSig.pEnv->NoAuxFiles = pEnv->NoAuxFiles;
	SubAstSig.pEnv->WavePath = pEnv->WavePath;
	SubAstSig.pEnv->AuxPath = pEnv->AuxPath;
	SubAstSig.pEnv->DataPath = pEnv->DataPath;
	for (AstNode *p=pUDF->next; p; p=p->next)	// AUX file local functions
		if (p->type == T_FUNCTION)
			SubAstSig.pEnv->UDFs[p->str] = p;
	try {
		SubAstSig.Compute();
		SubAstSig.pEnv->UDFs.clear();	// clear it so that function definitions will not be deleted by the destructor of SubAstSig
	} catch (const char *errmsg) {
		SubAstSig.pEnv->UDFs.clear();
		throw CAstException(pCall, "Calling "+string(pCall->str)+"( )\n\nIn function definition:\n"+errmsg);
	}
	if (!pOutParam)	// no output parameter specified.
		Sig = SubAstSig.Sig;
	return Sig;
}


CAstSig &CAstSig::Reset(const int fs)
{
	map<string,AstNode *>::iterator it;
	for (it=pEnv->UDFs.begin(); it!=pEnv->UDFs.end(); it++)
		yydeleteAstNode(it->second, 0);
	pEnv->UDFs.clear();
	pEnv->Refs.clear();
	pEnv->Tags.clear();
	pEnv->ArrRefs.clear();
	pEnv->Arrays.clear();
	pEnv->NoAuxFiles.clear();
	if (fs > 1)
		pEnv->Fs = fs;
	return *this;
}


CSignals *CAstSig::RetrieveTag(const char *tagname)
{
	map<string,CSignals *>::iterator itRef;
	map<string,CSignals>::iterator itTag;

	itRef = pEnv->Refs.find(tagname);
	if (itRef != pEnv->Refs.end())
		return itRef->second;
	itTag = pEnv->Tags.find(tagname);
	if (itTag != pEnv->Tags.end())
		return &itTag->second;
	return NULL;
}


CAstSig &CAstSig::SetTag(const char *name, const CSignals &sig)
{
	CSignals *psig = RetrieveTag(name);	// RetrieveTag() takes care of reference variables.
	if (psig)
		*psig = sig;
	else	// new variable, create it.
		pEnv->Tags[name] = sig;
	return *this;
}


map<int,CSignals> *CAstSig::RetrieveArray(const char *arrayname)
{
	map<string,map<int,CSignals> *>::iterator itArrRef;
	map<string,map<int,CSignals>>::iterator itArr;

	itArrRef = pEnv->ArrRefs.find(arrayname);
	if (itArrRef != pEnv->ArrRefs.end())
		return itArrRef->second;
	itArr = pEnv->Arrays.find(arrayname);
	if (itArr != pEnv->Arrays.end())
		return &itArr->second;
	return NULL;
}


CAstSig &CAstSig::SetCell(const char *name, const int i, const CSignals &sig)
{
	map<int,CSignals> *pArray = RetrieveArray(name);	// RetrieveTag() takes care of reference variables.
	if (pArray)
		(*pArray)[i] = sig;
	else	// new array, create it.
		pEnv->Arrays[name][i] = sig;
	return *this;
}


AstNode *CAstSig::RetrieveUDF(const char *fname)
{
	map<string,AstNode *>::iterator itUDF;

	itUDF = pEnv->UDFs.find(fname);
	if (itUDF == pEnv->UDFs.end())
		return NULL;

	return itUDF->second;
}


CSignals &CAstSig::GetTag(const char *name)
{
	return pEnv->Tags[name];
}


void CAstSig::SetRef(const char *ref, const char *var)
{
	CAstSig astsig(this);
	CSignals *psig;
	char errmsg[250];

	astsig.SetNewScript(var);
	if (astsig.pAst->type != T_ID)
		throw "SetRef( ) : Expects only a single variable in the second parameter.";
	AstNode *p = astsig.pAst->child;
	if (p) {	/* child means cell array index */
		CSignals tsig = astsig.Compute(p);
		if (!tsig.IsScalar())
			throw "SetRef( ) : Array index must be a scalar.";
		map<int,CSignals> *pArray;
		if (!(pArray = RetrieveArray(astsig.pAst->str))) {
			sprintf(errmsg, "SetRef( ) : Unknown array - %s.", astsig.pAst->str);
			throw errmsg;
		}
		psig = &(*pArray)[round(tsig.value())];
	} else {
		if (!(psig = RetrieveTag(astsig.pAst->str))) {
			sprintf(errmsg, "SetRef( ) : Unknown tag - %s.", astsig.pAst->str);
			throw errmsg;
		}
	}
	pEnv->Refs[ref] = psig;
}


CAstSig &CAstSig::SetPath(const char *ext, const char *path)
{
	string strPath = path;
	size_t len = strlen(path);
	if (len && path[len-1] != '\\')
		strPath += '\\';
	if (*ext == '\0')
		pEnv->WavePath = pEnv->AuxPath = pEnv->DataPath = strPath;
	else if (!strcmp(ext, "wav"))
		pEnv->WavePath = strPath;
	else if (!strcmp(ext, "aux"))
		pEnv->AuxPath = strPath;
	else if (!strcmp(ext, "txt"))
		pEnv->DataPath = strPath;
	return *this;
}


const char *CAstSig::GetPath(const char *ext)
{
	if (!strcmp(ext, "wav"))
		return pEnv->WavePath.c_str();
	else if (!strcmp(ext, "aux"))
		return pEnv->AuxPath.c_str();
	else if (!strcmp(ext, "txt"))
		return pEnv->DataPath.c_str();
	return NULL;
}


FILE *CAstSig::OpenFileInPath(string &fname, const string ext)
{
	size_t pdot = fname.rfind('.');
	if (pdot == fname.npos || pdot < fname.length()-4)
		fname += "." + ext;
	if (fname[0] == '\\' || fname[1] == ':')
		return fopen(fname.c_str(), "r");
	else {
		string path, ofname(fname);
		if (ext == "wav")
			path = pEnv->WavePath;
		else if (ext == "aux")
			path = pEnv->AuxPath;
		else if (ext == "txt")
			path = pEnv->DataPath;
		for (size_t ps=0, p=path.find(';', ps); ; ps=p+1) {
			int pe = (p==string::npos) ? (int)path.length() - 1 : (int)p - 1;
			for (; ps < path.length() && isspace(path[ps]); ps++)
				;
			for (; pe >= 0 && isspace(path[pe]); pe--)
				;
			if ((int)ps <= pe)
				fname = path.substr(ps,pe-ps+1) + ofname;
			if (FILE *file = fopen(fname.c_str(), "r"))
				return file;
			if (p == string::npos)
				break;
		}
	}
	return NULL;
}


string CAstSig::MakeFilename(string fname, const string ext)
{
	size_t pdot = fname.rfind('.');
	if (pdot == fname.npos || pdot < fname.length()-4)
		fname += "." + ext;
	if (fname[0] != '\\' && fname[1] != ':')
		if (ext == "wav")
			return pEnv->WavePath + fname;
		else if (ext == "aux")
			return pEnv->AuxPath + fname;
		else if (ext == "txt")
			return pEnv->DataPath + fname;
	return fname;
}


bool CAstSig::IsCondition(const AstNode *pnode)
{
	switch (pnode->type) {
	case '<':
	case '>':
	case T_COMP_EQ:
	case T_COMP_NE:
	case T_COMP_LE:
	case T_COMP_GE:
	case T_LOGIC_NOT:
	case T_LOGIC_AND:
	case T_LOGIC_OR:
		return true;
	default:
		return false;
	}
}


string CAstSig::ComputeString(const AstNode *p)
{
	return (p->type==T_STRING) ? p->str : Compute(p).string();
}


CAstSigEnv::CAstSigEnv(const int fs)
: Fs(fs), RefCount(0)
{
	if (fs <= 1)
		throw "Internal error: Fs must be greater than 1. You probably failed to provide a valid Fs.";
}


CAstSigEnv::~CAstSigEnv()
{
	map<string,AstNode *>::iterator it;
	for (it=UDFs.begin(); it!=UDFs.end(); it++)
		yydeleteAstNode(it->second, 0);
}
