#include "Toak.h"


CTokens::CTokens()
:nItems(1)
{
	item = new string[nItems];
}

CTokens::~CTokens(void)
{
	delete[] item;
}

int CTokens::FillItems()
{
	nItems = countDeliminators(content.c_str(), "|");
	item = new string[nItems];
	str2array(item, nItems, content.c_str(), "|");
	return nItems;
}

const string& CTokens::linkwith20()
{ // not yet tested 
	size_t id;
	id = content.find("|");
	while (id!=string::npos)
	{
		content = content.replace(id, 1, " ");
		id = content.find("|");
	}
	return content;
}

CToak::CToak(void)
:len(0)
{
	name[0]=0;
	line = NULL;
	file = NULL;
	sig = NULL;
	order = NULL;
}

CToak::~CToak(void)
{
	delete[] line;
	delete[] file;
	delete[] sig;
	delete[] order;
}

int CToak::ReadSession(const char* sessionFile)
{
	string readstr, readstr2;
	name[0]=0;
	if (ReadINI(errstr, sessionFile, "FILES", readstr)<=0) {
		strcpy(errstr, "Error reading FILES field");
		return 0;
	}
	int nItems = countDeliminators(readstr.c_str(), "\r\n");
	if (ReadINI(errstr, sessionFile, "TOKENS", readstr2)<=0) {
		strcpy(errstr, "Error reading TOKENS field");
		return 0;
	}
	int nItems2 = countDeliminators(readstr2.c_str(), "\r\n");
	if (nItems !=nItems2) {
		strcpy(errstr, "Fields FILES and TOKENS not matched");
		return 0;
	}
	string parsed[2], parsed2[2], vern, vern2;
	delete[] line; line = new CTokens[nItems];
	delete[] file; file = new string[nItems];
	delete[] sig; sig = new CSignals[nItems];
	delete[] order; order = new int[nItems];
	vern = readstr; vern2 = readstr2;
	int c = 0;
	for (int res=2; res>1;) {
		res = str2array(parsed, 2, vern, "\r\n");
		res = str2array(parsed2, 2, vern2, "\r\n");
		line[c].content = parsed2[0];
		line[c].FillItems();
		if (!sig[c].Wavread(parsed[0].c_str(), errstr))
			return 0;
		//if (sig[c].GetFs()!=fs && !sig[c].Resample(fs,errstr)) return 0;
		trim(parsed[0], ' ');
		file[c++] = parsed[0];
		vern = parsed[1]; vern2 = parsed2[1];
	}
	len = c;
	if (ReadINI(errstr, sessionFile, "IDENTIFIER", readstr)<=0) {
		strcpy(errstr, "Error reading IDENTIFIER field");
		return 0;
	}
	strcpy(name, readstr.c_str());
	return 1;
}