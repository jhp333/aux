#pragma once
#include "audfret.h"
#include "audstr.h"
#include "sigproc.h"

class CTokens
{
public:
	string content;
	string *item;
	int nItems;
	CTokens();
	virtual ~CTokens();
	int FillItems();
	const string& linkwith20();
};

class CToak
{
public:
	CToak(void);
	virtual ~CToak(void);
	int len;
	int *order;
	char name[32];
	char errstr[256];
	string *file;
	CTokens *line;
	CSignals *sig;
	int ReadSession(const char* sessionFile);
};
