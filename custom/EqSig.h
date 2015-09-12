#pragma once

#include "CSignals.h"
#include <string>
#include <iostream>
#include <list>

using namespace std;

extern int CheckTagItem (string &strIn, string &estr);
extern void ParseTagItem (string &strIn, string &tagname, string &definedStr);
extern int GetSurroundedBy(char c1, char c2, string &in, string &out);

class CEqSig
{
public:
	CSignals sig;
	string str; // DO NOT EVER CHANGE THIS IN APPLICATION CODES. USE setstr() instead.
	EXP_CS CEqSig();
	EXP_CS CEqSig(const CEqSig& src);
	EXP_CS CEqSig(int fs, const char *st="");
	CEqSig(int fs, string st);
	EXP_CS const CEqSig& operator=(const CEqSig& rhs);
	EXP_CS ~CEqSig();

	EXP_CS int Make(int nTags, CEqSig *tags, char **errstr);
	EXP_CS int Play(int devID, char *errstr, int len);
	EXP_CS int GetNonZeroPt();
	EXP_CS void SetDefaultFilePath(const char *path);
	EXP_CS int DoesItHaveThis(int nTags, CEqSig *tags, const char *str1);
	EXP_CS void SetmaxCalLevel(double level);
	EXP_CS void setstr(const char *st) {str = st;};
	EXP_CS int len() {return sig.nSamples;};
	void FileWithPath(string strIn, string& strOut, const char* defaultExt);
private:
	string appPath;
	double maxCalLevel;
	int CheckParenth();
	int ParseLRHS(string &tagname, string &expression, string &operation);
	int CountTerms(char delim);
	int ParseTerms(char delim, string& term1, string& term2);
	int StripParenthGetSign(bool& wasParenth);
	void* ReadArray(int nTags, CEqSig *tags);
	CSignals& Compute(int nTags, CEqSig *tags);
	CSignals& Process(int nTags, CEqSig *tags);
	int RetrieveTag(int nTags, CEqSig *tags);
	void series2sig(int nTags, CEqSig *tags, const char *instr, CSignals &carrier);
	CSignals& handle_sigma(string argstr, int nTags, CEqSig *tags);
};

