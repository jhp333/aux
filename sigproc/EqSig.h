#pragma once

#include "CSignals.h"
#include "psycon.yacc.h"
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
	char *str; // DO NOT EVER CHANGE THIS IN APPLICATION CODES. USE setstr() instead.
	EXP_CS CEqSig();
	EXP_CS CEqSig(const CEqSig& src);
	EXP_CS CEqSig(int fs, const char *st="");
	EXP_CS const CEqSig& operator=(const CEqSig& rhs);
	EXP_CS ~CEqSig();

	EXP_CS int Make(int nTags, CEqSig *tags, char *errstr);
	CSignals& Compute(AstNode *pnode, int nTags, CEqSig *tags);
	EXP_CS int Play(int devID, char *errstr, int len);
	EXP_CS int GetNonZeroPt();
	EXP_CS int DoesItHaveThis(int nTags, CEqSig *tags, const char *str1);
	EXP_CS void SetmaxCalLevel(double level);
	EXP_CS void setstr(const char *st);
	EXP_CS int len() {return sig.nSamples;};
	EXP_CS size_t strlength() {return strlen(str);};
	void FileWithPath(string strIn, string& strOut, const char* defaultExt);
	EXP_CS string sig2str();
private:
	double maxCalLevel;
	int CheckParenth();
	int ParseLRHS(string &tagname, string &expression, string &operation);
	int CountTerms(char delim);
	int ParseTerms(char delim, string& term1, string& term2);
	int StripParenthGetSign(char& wasParenth);
	void* ReadArray(int nTags, CEqSig *tags);
	CSignals& Compute(int nTags, CEqSig *tags);
	CSignals& Process(int nTags, CEqSig *tags, bool fRegardBlank = false);
	int RetrieveTag(int nTags, CEqSig *tags);
	void series2sig(int nTags, CEqSig *tags, const char *instr, CSignals &carrier);
	CSignals& handle_sigma(char *argstr, int nTags, CEqSig *tags);
	CSignals& handle_varvar(CEqSig *args, int nTags, CEqSig *tags);
};

class CLineParser
{
public:
	int nTags;
	CEqSig *tag; 
	string errstr;
	EXP_CS CLineParser(void);
	EXP_CS virtual ~CLineParser(void);
	EXP_CS int Lines2Tags(string *line, int nLines);
	EXP_CS int IsExist(const char *tagname);
	EXP_CS void UpdateTag(CEqSig *tag2add, int id2beInserted);
	EXP_CS int RemoveTag(int id);
	EXP_CS int RemoveTag(const char *tagname);

	EXP_CS int Ast2Tags(AstNode *pblock);

private:
	int EvalNonLoopLines(string *line, int nLines);
	int EvalLoop (string *line, int nLines);
	int DoTheseIfTrue(string &condstr, string *line, int nLines);
};
