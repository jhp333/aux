#include <math.h>
#include <atlrx.h>
#include <sstream>
#include <vector>
#include <algorithm>
#include "audfret.h"
#include "calbase.h"
#include "sigproc.h"
#include "psycon.tab.h"


CAstException::CAstException(const AstNode *p0, const AstNode *p, const char** FuncSigs, const string msg)
: pnode(p), int1(0)
{
	const string fname = p0->str;
	string fnsig;
	for (int i=0; FuncSigs[i]; ++i)
		fnsig += "\n\t" + fname + FuncSigs[i];
	str1 = fname + "( ) " + msg + "\n\n Usage:" + fnsig;
	makeOutStr();
}


void checkNumArgs(const AstNode *pnode, const AstNode *p, const char** FuncSigs, const int minArgs, const int maxArgs)
{
	ostringstream msg;
	int nArgs=0;
	for (const AstNode *cp=p; cp; cp=cp->next)
		++nArgs;
	msg << "must have ";
	if (minArgs == 0 && maxArgs == 0 && nArgs > 0)
		msg << "no argument.";
	else if (nArgs < minArgs && maxArgs == 0)
		msg << "at least " << minArgs << (minArgs>1 ? " arguments." : " argument.");
	else if (nArgs < minArgs || maxArgs > 0 && nArgs > maxArgs) {
		msg << minArgs;
		for (int i=minArgs+1; i<maxArgs; ++i)
			msg << ", " << i;
		if (minArgs != maxArgs)
			msg << " or " << maxArgs;
		msg << (maxArgs>1 ? " arguments." : " argument.");
	} else
		return;
	throw CAstException(pnode, pnode, FuncSigs, msg.str());
}


double aux_sgn(const double x)
{
	return (x==0.)?0.:((x>0.)?1.:-1.);
}


void aux_getfs(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const char *fnsigs[] = {
		"()", 0};
	checkNumArgs(pnode, p, fnsigs, 0, 0);
	ast.Sig.SetValue(ast.GetFs());
}


void aux_rms(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const char *fnsigs[] = {
		"(signal)",
		"(signal, TargetRmsValue)",
		"(signal, Ref, Signal2RefRatio_dB)", 0};
	checkNumArgs(pnode, p, fnsigs, 1, 3);
	if (!p->next) {		// one argument
		ast.Compute(p);
		if (ast.Sig.nSamples == 0 || (ast.Sig.Min()==0. && ast.Sig.Max()==0.))
			ast.Sig.SetValue(-9999.99);
		else
			ast.Sig = ast.Sig.LevelLTmc();
		return;
	}
	CSignals rms2, lastArg;
	if (p->next->next) {// three arguments
		CSignals second = ast.Compute(p->next);
		if (second.IsScalar())
			throw CAstException(pnode, p->next, fnsigs, "2nd argument of 3-argument rms( ) must be a signal.");
		rms2 = second.LevelLTmc();
		lastArg = ast.Compute(p->next->next);
	} else {			// two arguments
		rms2.SetValue(-0.000262);	// the calculated rms value of a full scale sinusoid (this is necessary to avoid the clipping of rms adjusted full scale sinusoid)
		lastArg = ast.Compute(p->next);
	}
	ast.Compute(p);	// Sig gets the first operand
	if (ast.Sig.GetLength() <= 1)
		throw CAstException(pnode, p, fnsigs, "1st argument must be a signal.");
	CSignals rms = -ast.Sig.LevelLTmc();
	if (rms.nSamples == 1) {
		if (rms2.nSamples > 1)
			throw CAstException(pnode, p->next, fnsigs, "2nd signal must be mono when 1st one is mono.");
		if (lastArg.nSamples > 1)
			throw CAstException(pnode, p->next->next?p->next->next:p->next, fnsigs, "Last argument must be scalar when 1st one is mono.");
	}
	//ast.Sig *= exp((lastArg - rms + rms2) * LOG10DIV20);
	rms += rms2;
	rms += lastArg;
	rms *= LOG10DIV20;
	ast.Sig *= rms.each(exp).transpose1();
}


void aux_caret(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const char *fnsigs[] = {
		"(signal, scalar)", 0};
	checkNumArgs(pnode, p, fnsigs, 2, 2);

	ast.Compute(p->next);
	if (!ast.Sig.IsScalar())
		throw CAstException(pnode, p->next, fnsigs, "2nd argument must be a scalar.");
	double param = ast.Sig.value();

	ast.Compute(p);
	if (ast.Sig.GetLength() <= 1)
		throw CAstException(pnode, p, fnsigs, "1st argument must be a signal.");
	int orgfs = ast.Sig.GetFs();
	char errstr[256] = "";
	ast.Sig.Resample(round(orgfs/param), errstr);
	ast.Sig.SetFs(orgfs);
	if (*errstr)
		throw errstr;
}


void processEscapes(string &str)
{
	size_t pos;
	for (size_t start=0; (pos=str.find('\\', start))!=string::npos; start=pos+1)
		switch (str[pos+1]) {
		case 'n':
			str.replace(pos, 2, "\n");
			break;
		case 't':
			str.replace(pos, 2, "\t");
			break;
		default:
			str.erase(pos, 1);
		}
}


void aux_sprintf(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const char *fnsigs[] = {
		"(format_string, ...)", 0};
	checkNumArgs(pnode, p, fnsigs, 1, 0);

	CAtlRegExp<> regexp;
	REParseError status = regexp.Parse("%([-+ #0]+)?({\\z|\\*})?(\\.{\\z|\\*})?[hlL]?{[cuoxXideEgGfs]}" );
	if (REPARSE_ERROR_OK != status)
		throw "Internal error! RegExp.Parse( ) failed.";

	ast.Sig.Reset(2);	// to get the output string
	CAstSig tast(&ast);	// to preserve this->Sig
	CAtlREMatchContext<> mcFormat;
	string fmtstring = tast.ComputeString(p);
	processEscapes(fmtstring);
	const char *fmtstr = fmtstring.c_str();
	for (const char *next; fmtstr && regexp.Match(fmtstr, &mcFormat, &next); fmtstr=next) {
		const CAtlREMatchContext<>::RECHAR* szStart = 0;
		const CAtlREMatchContext<>::RECHAR* szEnd = 0;
		string vstring;
		double v;
		string fmt1str(fmtstr, mcFormat.m_Match.szEnd-fmtstr);
		vector<char> outStr;
		outStr.resize(100);
		for (UINT nGroupIndex = 0; nGroupIndex < mcFormat.m_uNumGroups; ++nGroupIndex) {
			mcFormat.GetMatch(nGroupIndex, &szStart, &szEnd);
			if (nGroupIndex == 2 || (nGroupIndex < 2 && szStart && *szStart == '*')) {	// condition for consuming an argument
				if ((p = p->next) == NULL)
					throw CAstException(pnode, pnode, fnsigs, "Not enough arguments.");
				if (nGroupIndex == 2 && *szStart == 's')
					vstring = tast.ComputeString(p);
				else if (tast.Compute(p).IsScalar())
					v = tast.Sig.value();
				else
					throw CAstException(pnode, p, fnsigs, "Scalar value expected for this argument.");
				if (nGroupIndex != 2) {
					char width[20];
					sprintf(width, "%d", round(v));
					fmt1str.replace(szStart-fmtstr, 1, width);
				}
			}
		}
		switch (*szStart) {
		case 'e': case 'E':
		case 'g': case 'G':
		case 'f':
			sprintf(&outStr[0], fmt1str.c_str(), v);
			break;
		case 'c': case 'o':
		case 'x': case 'X':
		case 'i': case 'u':
		case 'd':
			sprintf(&outStr[0], fmt1str.c_str(), round(v));
			break;
		case 's':
			outStr.resize(vstring.size()+100);
			sprintf(&outStr[0], fmt1str.c_str(), vstring.c_str());
			break;
		}
		int n = ast.Sig.nSamples;
		ast.Sig.UpdateBuffer(n + (int)strlen(&outStr[0]));
		for (int j=0; outStr[j]; j++)
			ast.Sig.buf[n+j] = outStr[j];
	}
	int n = ast.Sig.nSamples;
	ast.Sig.UpdateBuffer(n + (int)strlen(fmtstr));
	for (int j=0; fmtstr[j]; j++)
		ast.Sig.buf[n+j] = fmtstr[j];
}


void aux_fprintf(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const char *fnsigs[] = {
		"(filename, format_string, ...)", 0};
	checkNumArgs(pnode, p, fnsigs, 2, 0);

	CAtlRegExp<> regexp;
	REParseError status = regexp.Parse("%([-+ #0]+)?({\\z|\\*})?(\\.{\\z|\\*})?[hlL]?{[cuoxXideEgGfs]}" );
	if (REPARSE_ERROR_OK != status)
		throw "Internal error! RegExp.Parse( ) failed.";

	string filename = ast.MakeFilename(ast.ComputeString(p), "txt");
	FILE *file;
	if (!(file = fopen(filename.c_str(), "a")))
		throw CAstException(pnode, "Cannot open file", filename);
	try {	// need this to close the file on exception
		CAtlREMatchContext<> mcFormat;
		p = p->next;
		string fmtstring = ast.ComputeString(p);
		processEscapes(fmtstring);
		const char *fmtstr = fmtstring.c_str();
		for (const char *next; fmtstr && regexp.Match(fmtstr, &mcFormat, &next); fmtstr=next) {
			const CAtlREMatchContext<>::RECHAR* szStart = 0;
			const CAtlREMatchContext<>::RECHAR* szEnd = 0;
			string vstring;
			double v;
			string fmt1string(fmtstr, mcFormat.m_Match.szEnd-fmtstr);
			for (UINT nGroupIndex = 0; nGroupIndex < mcFormat.m_uNumGroups; ++nGroupIndex) {
				mcFormat.GetMatch(nGroupIndex, &szStart, &szEnd);
				if (nGroupIndex == 2 || (nGroupIndex < 2 && szStart && *szStart == '*')) {	// condition for consuming an argument
					if ((p = p->next) == NULL)
						throw CAstException(pnode, pnode, fnsigs, "Not enough arguments.");
					if (nGroupIndex == 2 && *szStart == 's')
						vstring = ast.ComputeString(p);
					else if (ast.Compute(p).IsScalar())
						v = ast.Sig.value();
					else
						throw CAstException(pnode, p, fnsigs, "Scalar value expected for this argument.");
					if (nGroupIndex != 2) {
						char width[20];
						sprintf(width, "%d", round(v));
						fmt1string.replace(szStart-fmtstr, 1, width);
					}
				}
			}
			switch (*szStart) {
			case 'e': case 'E':
			case 'g': case 'G':
			case 'f':
				fprintf(file, fmt1string.c_str(), v);
				break;
			case 'c': case 'o':
			case 'x': case 'X':
			case 'i': case 'u':
			case 'd':
				fprintf(file, fmt1string.c_str(), round(v));
				break;
			case 's':
				fprintf(file, fmt1string.c_str(), vstring.c_str());
				break;
			}
		}
		fprintf(file, fmtstr);
		fclose(file);
	} catch (const char *) {
		fclose(file);
		throw;
	} catch (const CAstException &) {
		fclose(file);
		throw;
	}
}


void aux_sscanf(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const char *fnsigs[] = {
		"(string, format_string)", 0};
	checkNumArgs(pnode, p, fnsigs, 2, 0);

	string srcstring = ast.ComputeString(p);
	p = p->next;
	string fmtstring = ast.ComputeString(p);
	processEscapes(fmtstring);
	const char *fmtstr = fmtstring.c_str();
	const char *srcstr = srcstring.c_str();

	CAtlRegExp<> regexp;
	REParseError status = regexp.Parse("{%}([-+ #0]+)?({\\*})?({\\z})?[hlL]?{[cuoxXideEgGfs]}" );
	if (REPARSE_ERROR_OK != status)
		throw "Internal error! RegExp.Parse( ) failed.";
	CAtlREMatchContext<> mcFormat;
	char fmttype;
	const char *fmtspec;	// pointer to '%', for the error message.
	int CharWidth, fmtcnt = 0;
	for (const char *next; fmtstr && regexp.Match(fmtstr, &mcFormat, &next); fmtstr=next) {
		const CAtlREMatchContext<>::RECHAR* szStart = 0;
		const CAtlREMatchContext<>::RECHAR* szEnd = 0;
		bool fIgnore = false;
		for (UINT nGroupIndex = 0; nGroupIndex < mcFormat.m_uNumGroups; ++nGroupIndex) {
			mcFormat.GetMatch(nGroupIndex, &szStart, &szEnd);
			switch (nGroupIndex) {
			case 3:	// type specifier
				if (++fmtcnt > 1)
					throw "Only one value can be read and returned.";
				fmttype = *szStart;
				break;
			case 2:	// width specifier
				CharWidth = -1;
				if (szStart && sscanf(szStart, "%d", &CharWidth) != 1)
					throw "Invalid width.";
				break;
			case 1:	// starting asterisk : read and ignore (don't store)
				if (szStart)
					fIgnore = true;
				break;
			case 0:	// '%'
				fmtspec = szStart;
				break;
			}
			if (fIgnore)
				break;
		}
		if (fIgnore)
			continue;	
	}
	int res, iv;
	unsigned uv;
	float fv;
	vector<char> vbuf;
	switch (fmttype) {
	case 'e': case 'E':
	case 'g': case 'G':
	case 'f':
		if ((res = sscanf(srcstr, fmtstring.c_str(), &fv)) == 1)
			ast.Sig.SetValue(fv);
		break;
	case 'd': case 'o':
	case 'x': case 'X':
	case 'i':
		if ((res = sscanf(srcstr, fmtstring.c_str(), &iv)) == 1)
			ast.Sig.SetValue(iv);
		break;
	case 'u':
		if ((res = sscanf(srcstr, fmtstring.c_str(), &uv)) == 1)
			ast.Sig.SetValue(uv);
		break;
	case 's': case 'c':
		if (CharWidth == -1)
			CharWidth = fmttype=='s'?(int)srcstring.size():1;
		vbuf.resize(CharWidth+1);
		if ((res = sscanf(srcstr, fmtstring.c_str(), &vbuf[0])) == 1) {
			ast.Sig.UpdateBuffer((int)strlen(&vbuf[0]));
			for (int i=0; vbuf[i]; ++i)
				ast.Sig.buf[i] = vbuf[i];
		}
	}
	if (res != 1)
		throw CAstException(pnode, "sscanf failed at", fmtspec);
	return;
}


void aux_fdelete(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const char *fnsigs[] = {
		"(filename)", 0};
	checkNumArgs(pnode, p, fnsigs, 1, 1);
	string filename = ast.MakeFilename(ast.ComputeString(p), "txt");
	remove(filename.c_str());
}


void aux_error(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const char *fnsigs[] = {
		"(message)", 0};
	checkNumArgs(pnode, p, fnsigs, 1, 1);
	string errmsg = ast.ComputeString(p);
	throw CAstException(pnode, "User raised error - " + errmsg);
}


void aux_wave(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const char *fnsigs[] = {
		"(filename)", 0};
	checkNumArgs(pnode, p, fnsigs, 1, 1);
	string filename = ast.MakeFilename(ast.ComputeString(p), "wav");
	char errStr[256];
	if (!ast.Sig.Wavread(filename.c_str(), errStr))
		throw errStr;
	if (ast.Sig.GetFs()!=ast.GetFs() && !ast.Sig.Resample(ast.GetFs(),errStr))
		throw CAstException(pnode, string(errStr) + " " + filename);
}


void aux_wavwrite(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const char *fnsigs[] = {
		"(signal, filename)", 0};
	checkNumArgs(pnode, p, fnsigs, 2, 2);
	string filename = ast.MakeFilename(ast.ComputeString(p->next), "wav");
	ast.Compute(p);
	char errStr[256];
	if (!ast.Sig.Wavwrite(filename.c_str(), errStr))
		throw errStr;
}


void aux_file(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const char *fnsigs[] = {
		"(filename)", 0};
	checkNumArgs(pnode, p, fnsigs, 1, 1);

	HANDLE hFile = INVALID_HANDLE_VALUE;
	char *buf = NULL;
	try {
		string filename = ast.MakeFilename(ast.ComputeString(p), "txt");
		if ((hFile = CreateFile(filename.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
					OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ))== INVALID_HANDLE_VALUE)
			throw CAstException(pnode, "Cannot read file", filename);
		DWORD dw, fsize = GetFileSize(hFile, &dw);
		if (fsize==INVALID_FILE_SIZE)
			throw CAstException(pnode, "Cannot get the size of file", filename);
		buf = new char[fsize+1];
		if(!ReadFile (hFile, buf, fsize, &dw, NULL))
			throw CAstException(pnode, "Error reading file", filename);
		buf[dw]='\0';
		char errStr[256];
		if (ast.Sig.Str2Sig(buf, errStr)==NULL)
			throw CAstException(pnode, "Str2Sig Error.", errStr);
	} catch (const CAstException &) {
		if (buf) delete[] buf;
		if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
		throw;
	}
	if (buf) delete[] buf;
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
}


void aux_include(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const char *fnsigs[] = {
		"(filename)", 0};
	checkNumArgs(pnode, p, fnsigs, 1, 1);
	string filename = ast.ComputeString(p);
	if (FILE *auxfile = ast.OpenFileInPath(filename, "aux")) {
		try {
			CAstSig tast(&ast);
			tast.SetNewFile(auxfile);
			fclose(auxfile);
			ast.Sig = tast.Compute();
		} catch (const char *errmsg) {
			fclose(auxfile);
			throw CAstException(pnode, "Including "+filename+"\n\nIn the file: \n"+errmsg);
		}
	} else
		throw CAstException(pnode, "Cannot read file", filename);
}


void aux_eval(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const char *fnsigs[] = {
		"(string)", 0};
	checkNumArgs(pnode, p, fnsigs, 1, 1);
	string str = ast.ComputeString(p);
	try {
		CAstSig tast(str.c_str(), &ast);
		ast.Sig = tast.Compute();
	} catch (const char *errmsg) {
		throw CAstException(pnode, "Evaluating\n"+str+"\n\nIn the string: \n"+errmsg);
	}
}


void aux_interp(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const char *fnsigs[] = {
		"(vector, vector)", 0};
	checkNumArgs(pnode, p, fnsigs, 2, 2);
	CSignals first = ast.Compute(p);
	if (first.GetLength() <= 1)
		throw CAstException(pnode, p, fnsigs, "1st argument must be a vector.");
	CSignals second = ast.Compute(p->next);
	if (second.GetLength() <= 1)
		throw CAstException(pnode, p->next, fnsigs, "2nd argument must be a vector.");
	ast.Sig.Reset(ast.GetFs()).Interp(first, second);
}


void aux_zeros(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const char *fnsigs[] = {
		"(scalar)", 0};
	checkNumArgs(pnode, p, fnsigs, 1, 1);
	ast.Compute(p);
	if (!ast.Sig.IsScalar())
		throw CAstException(pnode, p, fnsigs, "argument must be a scalar.");
	int n = round(ast.Sig.value());
	if (n <= 0)
		throw CAstException(pnode, p, fnsigs, "argument must be a positive number.");
	ast.Sig.Reset(ast.GetFs()).UpdateBuffer(n);
	for (int i=0; i<n; ++i)
		ast.Sig.buf[i] = 0;
}


void aux_ones(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const char *fnsigs[] = {
		"(scalar)", 0};
	checkNumArgs(pnode, p, fnsigs, 1, 1);
	ast.Compute(p);
	if (!ast.Sig.IsScalar())
		throw CAstException(pnode, p, fnsigs, "argument must be a scalar.");
	int n = round(ast.Sig.value());
	if (n <= 0)
		throw CAstException(pnode, p, fnsigs, "argument must be a positive number.");
	ast.Sig.Reset(ast.GetFs()).UpdateBuffer(n);
	for (int i=0; i<n; ++i)
		ast.Sig.buf[i] = 1;
}


void aux_stereo(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const char *fnsigs[] = {
		"(left_signal, right_signal)", 0};
	checkNumArgs(pnode, p, fnsigs, 2, 2);
	CSignals right = ast.Compute(p->next);
	if (right.GetLength() <= 1)
		throw CAstException(pnode, p->next, fnsigs, "2nd argument must be a signal.");
	ast.Compute(p);	// ast.Sig gets the left signal
	if (ast.Sig.GetLength() <= 1)
		throw CAstException(pnode, p, fnsigs, "1st argument must be a signal.");
	if (ast.Sig.GetFs() != right.GetFs())
		throw CAstException(pnode, "The sampling rate of both arguments must be the same.");
	if (int diff=ast.Sig.GetLength() - right.GetLength()) {
		CSignals filler(ast.Sig.GetFs());
		filler.Silence(abs(diff));
		((diff < 0) ? ast.Sig :	right) += &filler;
	}
	ast.Sig.SetNextChan(&right);
};


void aux_left_right(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const string fname = pnode->str;
	const char *fnsigs[] = {
		"(stereo_signal)", 0};
	checkNumArgs(pnode, p, fnsigs, 1, 1);
	ast.Compute(p);
	if (ast.Sig.GetLength() <= 1)
		throw CAstException(pnode, p, fnsigs, "argument must be a signal.");
	if (fname == "right") {
		CSignals *right = ast.Sig.DetachNextChan();
		if (right == NULL) {
			ast.Sig.Reset();
			return;
		}
		ast.Sig.SwapContents1node(*right);
		delete right;	// deleting left channel since 'right' now points to the left channel
	}
	ast.Sig.SetNextChan(NULL);
};


void aux_hann_hamming(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const string fname = pnode->str;
	const char *fnsigs[] = {
		"(length)", 0};
	checkNumArgs(pnode, p, fnsigs, 1, 1);
	ast.Compute(p);
	if (!ast.Sig.IsScalar())
		throw CAstException(pnode, p, fnsigs, "argument must be a scalar.");
	int n = round(ast.Sig.value());
	if (n <= 0)
		throw CAstException(pnode, p, fnsigs, "argument must be a positive number.");
	if (fname == "hann")
		ast.Sig.Blackman(n, 0);
	else if (fname == "hamming")
		ast.Sig.Hamming(n);
};


void aux_blackman(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const char *fnsigs[] = {
		"(length, [alpha=0.16])", 0};
	checkNumArgs(pnode, p, fnsigs, 1, 2);
	ast.Compute(p);
	if (!ast.Sig.IsScalar())
		throw CAstException(pnode, p, fnsigs, "argument must be a scalar.");
	int n = round(ast.Sig.value());
	if (n <= 0)
		throw CAstException(pnode, p, fnsigs, "argument must be a positive number.");
	if (!p->next) {
		ast.Sig.Blackman(n);
		return;
	}
	ast.Compute(p->next);
	if (!ast.Sig.IsScalar())
		throw CAstException(pnode, p->next, fnsigs, "2nd argument must be a scalar.");
	ast.Sig.Blackman(n, ast.Sig.value());
};


void aux_fft(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const string fname = pnode->str;
	const char *fnsigs[] = {
		"(signal, [nSamples=512])", 0};
	checkNumArgs(pnode, p, fnsigs, 1, 2);
	int n = 512;
	if (p->next) {
		ast.Compute(p->next);
		if (!ast.Sig.IsScalar())
			throw CAstException(pnode, p->next, fnsigs, "2nd argument must be a scalar.");
		n = round(ast.Sig.value());
		if (n <= 0)
			throw CAstException(pnode, p->next, fnsigs, "2nd argument must be a positive number.");
	}
	ast.Compute(p);
	if (ast.Sig.GetLength() <= 1)
		throw CAstException(pnode, p, fnsigs, "1st argument must be a signal.");
	if (fname == "fft")
		ast.Sig.cFFT(n);
	else if (fname == "rfft")
		ast.Sig.FFT(n);
};


void aux_ifft(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const string fname = pnode->str;
	const char *fnsigs[] = {
		"(vector)", 0};
	checkNumArgs(pnode, p, fnsigs, 1, 1);
	ast.Compute(p);
	if (ast.Sig.GetLength() <= 1)
		throw CAstException(pnode, p, fnsigs, "argument must be a vector.");
	if (fname == "fft")
		ast.Sig.icFFT();
	else if (fname == "rfft")
		ast.Sig.iFFT();
};


void aux_hilbert(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const char *fnsigs[] = {
		"(signal)", 0};
	checkNumArgs(pnode, p, fnsigs, 1, 1);
	ast.Compute(p);
	int n = ast.Sig.GetLength();
	if (n <= 1)
		throw CAstException(pnode, p, fnsigs, "1st argument must be a signal.");
	ast.Sig.Hilbert(n);
};


void aux_filt(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const string fname = pnode->str;
	const char *fnsigs[] = {
		"(signal, Numerator_array [, Denominator_array=1 for FIR])", 0};
	checkNumArgs(pnode, p, fnsigs, 2, 3);

	CSignals third, second = ast.Compute(p->next);
	if (p->next->next) {	// 3 args
		third = ast.Compute(p->next->next);
		if (!second.IsScalar() && !third.IsScalar() && second.GetLength() != third.GetLength())
			throw CAstException(pnode, pnode, fnsigs, "2nd & 3rd argument must be of the same length, unless they're scalars.");
	} else {				// 2 args
		if (second.GetLength() <= 1)
			throw CAstException(pnode, p->next, fnsigs, "2nd argument must be a vector(the numerator array for filtering).");
		third.SetValue(1);
	}
	ast.Compute(p);
	if (ast.Sig.GetLength() <= 1)
		throw CAstException(pnode, p, fnsigs, "The first argument must be a signal.");

	int len = max(second.GetLength(), third.GetLength());
	if (second.IsScalar())
		second.UpdateBuffer(len);
	else if (third.IsScalar())
		third.UpdateBuffer(len);
	if (fname == "filt")
		ast.Sig.Filter(len, second.buf, third.buf);
	else if (fname == "filtfilt")
		ast.Sig.filtfilt(len, second.buf, third.buf);
}


void aux_show(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	const char *fnsigs[] = {
		"(format_string, ...)", 0};
	checkNumArgs(pnode, p, fnsigs, 1, 0);

	ostringstream caption;
	caption << "Line " << pnode->line;
	aux_sprintf(ast, pnode, p);
	MessageBox(NULL, ast.Sig.string().c_str(), caption.str().c_str(), MB_ICONINFORMATION);
};


void aux_HOOK(CAstSig &ast, const AstNode *pnode, const AstNode *p)
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

	CSignals first;
	char buf[512];

	if (ast.CallbackHook && ast.CallbackHook(ast, pnode, p)==0)
		;
	else if (HookName == "PLAY") {
		const char *fnsigs[] = {
			"(signal [, async_flag])",
			"(\"stop\", handle_scalar)", 0};
		checkNumArgs(pnode, args, fnsigs, 1, 2);
		first = ast.Compute(args);
		unsigned long second = 0;
		if (args->next) {
			ast.Compute(args->next);
			if (!ast.Sig.IsScalar())
				throw CAstException(pnode, args->next, fnsigs, "argument must be a scalar.");
			second = (unsigned long)(ast.Sig.value()+(ast.Sig.value()<0?-.5:.5));
		}
		int flag = 0;	
		if (first.IsString()) {
			string cmd = first.string();
			std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
			void *pvoid = (void *)second;
			if (cmd == "stop")
				TerminatePlay(pvoid);
			else if (cmd == "pause")
				PauseResumePlay(pvoid, 0);
			else if (cmd == "resume")
				PauseResumePlay(pvoid, 1);
			else
				throw CAstException(args, "Unknown command in the string -", cmd);
			return;
		}
		char *errstr = buf;
		// second == 0:Synchronous, 1:Asynchronous, 2:Loop
		void *res = first.PlayArray(0, (second==0)?0:WM_USER+293, NULL, (second==2)?-1:0, errstr);
		if (!res)
			throw CAstException(pnode, "PlayArray() failed:", errstr);
		ast.Sig.SetValue((double)(unsigned long)res);
	} else if (HookName == "INPUT") {
		const char *fnsigs[] = {
			"(message_string [, title_string])", 0};
		checkNumArgs(pnode, args, fnsigs, 1, 2);
		string msg, title;
		msg = ast.ComputeString(args);
		if (args->next)
			title = ast.ComputeString(args->next);
		else {
			ostringstream caption;
			caption << "Line " << pnode->line;
			title = caption.str();
		}
		buf[0] = '\0';
		InputBox(title.c_str(), msg.c_str(), buf, sizeof(buf));
		ast.Sig.UpdateBuffer((int)strlen(buf));
		for (int i=0; buf[i]; ++i)
			ast.Sig.buf[i] = buf[i];
	} else if (HookName == "PIPE") {
		const char *fnsigs[] = {
			"(message_string [, node_name_string [, pipe_name_string]])", 0};
		checkNumArgs(pnode, args, fnsigs, 1, 3);
#define SIGNAL_INTERFACE_PIPENAME  "CochlearAudioSignalInterfacePipe"
		string pipemsg = ast.ComputeString(args);
		string nodename = ".";
		string pipename = SIGNAL_INTERFACE_PIPENAME;
		if (args->next) {
			nodename = ast.ComputeString(args->next);
			if (args->next->next)
				pipename = ast.ComputeString(args->next->next);
		}
		pipename = "\\\\" + nodename + "\\pipe\\" + pipename;
		char reply[50000] = "";
		unsigned long nRead;
		if (!CallNamedPipe(pipename.c_str(), (LPVOID)pipemsg.c_str(), (DWORD)pipemsg.size()+1, reply, (DWORD)sizeof(reply), &nRead, NMPWAIT_WAIT_FOREVER)) {
			char *errstr = buf;
			GetLastErrorStr(errstr);
			throw CAstException(pnode, "CallNamedPipe(" + pipename + ") failed:", errstr);
		}
		reply[nRead]='\0';
		ast.Sig.UpdateBuffer((int)nRead);
		for (int i=0; reply[i]; ++i)
			ast.Sig.buf[i] = reply[i];
	} else if (HookName == "ELAPSE") {
		const char *fnsigs[] = {
			"( )", 0};
		checkNumArgs(pnode, args, fnsigs, 0, 0);
		ast.Sig.SetValue(GetTickCount() - ast.Tick0);
	} else if (HookName == "SLEEP") {
		const char *fnsigs[] = {
			"(millisecond)", 0};
		checkNumArgs(pnode, args, fnsigs, 1, 1);
		ast.Compute(args);
		if (!ast.Sig.IsScalar())
			throw CAstException(pnode, args, fnsigs, "argument must be a scalar.");
		int n = round(ast.Sig.value());
		Sleep(n);
	} else
		throw CAstException(pnode, "Undefined HOOK name:", HookName);
};


void HandleExp1(const AstNode *pnode, CSignals &Sig)
{
	string fname = pnode->str;
	AstNode *p=pnode->child;
	if (p && p->type == NODE_ARGS)
		p = p->child;

	const char *fnsigs[] = {
		"(signal)", 0};
	checkNumArgs(pnode, p, fnsigs, 1, 1);

	CSignals *next = Sig.DetachNextChan();
	if (next)
		HandleExp1(pnode, *next);

	double dbl1;
	if (fname == "length")		(dbl1 = Sig.GetLength());
	else if (fname == "dur")	(dbl1 = Sig.CalculateTotalDuration() - Sig.tmark);
	else if (fname == "mean")	(dbl1 = Sig.Mean());
	else if (fname == "max")	(dbl1 = Sig.Max());
	else if (fname == "min")	(dbl1 = Sig.Min());
	else if (fname == "tbegin")	(dbl1 = Sig.tmark);
	else if (fname == "tend")	(dbl1 = Sig.CalculateTotalDuration());
	else
		throw ("Unhandled AUX function: " + fname).c_str();

	Sig.SetValue(dbl1);
	if (next)
		Sig += next;
	delete next;
}


void CAstSig::HandleAuxFunctions(const AstNode *pnode)
{
	string fname = pnode->str;
	AstNode *p=pnode->child;
	if (p && p->type == NODE_ARGS)
		p = p->child;

	double (*fn1)(double) = NULL;

		 if (fname == "abs")	fn1 = fabs;
	else if (fname == "sgn")	fn1 = aux_sgn;
	else if (fname == "sin")	fn1 = sin;
	else if (fname == "cos")	fn1 = cos;
	else if (fname == "tan")	fn1 = tan;
	else if (fname == "log")	fn1 = log;
	else if (fname == "log10")	fn1 = log10;
	else if (fname == "exp")	fn1 = exp;
	else if (fname == "ceil")	fn1 = ceil;
	else if (fname == "floor")	fn1 = floor;

	else if (fname == "length" ||
			 fname == "dur" ||
			 fname == "mean" ||
			 fname == "max" ||
			 fname == "min" ||
			 fname == "tbegin" ||
			 fname == "tend") HandleExp1(pnode, p?Compute(p):Sig);

	else if (fname == "getfs")		aux_getfs(*this, pnode, p);
	else if (fname == "rms")		aux_rms(*this, pnode, p);
	else if (fname == "caret")		aux_caret(*this, pnode, p);
	else if (fname == "sscanf")		aux_sscanf(*this, pnode, p);
	else if (fname == "sprintf")	aux_sprintf(*this, pnode, p);
	else if (fname == "fprintf")	aux_fprintf(*this, pnode, p);
	else if (fname == "fdelete")	aux_fdelete(*this, pnode, p);
	else if (fname == "error")		aux_error(*this, pnode, p);
	else if (fname == "wave")		aux_wave(*this, pnode, p);
	else if (fname == "wavwrite")	aux_wavwrite(*this, pnode, p);
	else if (fname == "file")		aux_file(*this, pnode, p);
	else if (fname == "include")	aux_include(*this, pnode, p);
	else if (fname == "eval")		aux_eval(*this, pnode, p);
	else if (fname == "interp")		aux_interp(*this, pnode, p);
	else if (fname == "zeros")		aux_zeros(*this, pnode, p);
	else if (fname == "ones")		aux_ones(*this, pnode, p);
	else if (fname == "stereo")		aux_stereo(*this, pnode, p);
	else if (fname == "left" ||
			 fname == "right")		aux_left_right(*this, pnode, p);
	else if (fname == "hann" ||
			 fname == "hamming")	aux_hann_hamming(*this, pnode, p);
	else if (fname == "blackman")	aux_blackman(*this, pnode, p);
	else if (fname == "fft" ||
			 fname == "rfft")		aux_fft(*this, pnode, p);
	else if (fname == "ifft" ||
			 fname == "rifft")		aux_ifft(*this, pnode, p);
	else if (fname == "hilbert")	aux_hilbert(*this, pnode, p);
	else if (fname == "filt" ||
			 fname == "filtfilt")	aux_filt(*this, pnode, p);
	else if (fname == "show")		aux_show(*this, pnode, p);
	else if (fname == "HOOK" ||
			 fname[0] == '#')		aux_HOOK(*this, pnode, p);
	else {
		int count=0;
		CalcManager x;
		list<CEqSig> Args;
		for (CEqSig eqsig; p; p=p->next) {
			eqsig.sig = Compute(p);
			Args.push_back(eqsig);
			x.AddArg(&Args.back());
		}
		/*
		if (strcmp(pnode->str, "rms")==0 && count==2)
		{
			//Make sure if SetmaxCalLevel has been called in the main application whenever "rms" is called
			Args.back().sig.val -= maxCalLevel ;
		}
		else if (strcmp(pnode->str, "$")==0)	// $() requires tags. By jhpark 10/20/2009
			return handle_varvar(local, nTags, tags);
		*/
		Sig = x.Calc(pnode->str, pEnv->Fs).sig;
	}

	if (fn1) {
		const char *fnsigs[] = {
			"(scalar or vector)", 0};
		checkNumArgs(pnode, p, fnsigs, 1, 1);
		Compute(p);
		Sig.each(fn1);
	}
}
