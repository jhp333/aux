#pragma once
#include <strstream>
#include "sigproc.h"
#include "audstr.h"
#include "audfret.h"

int isFirstWordThis(string &in, string thisword)
{
	string firstwo[2];
	str2array(firstwo, 2, in.c_str(), " ");
	return (firstwo[0]==thisword);
}

int ExtractLines4Loop(string *lineIn, int nLines, int &ind1, int &ind2)
{ //This gives ind1 and ind2 for the first loop that occurs in input string
	string firstwo[2];
	bool b(true);
	for (int i(0), j; i<nLines; i++)
	{
		str2array(firstwo, 2, lineIn[i].c_str(), " ");
		if (firstwo[0]=="if" || firstwo[0]=="while" || firstwo[0]=="for")
		{
			for (j=nLines-1; j>1 && b; j--)
			{
				if (isFirstWordThis(lineIn[j], "end") ) b=false;
			}
			if (j==1) throw "end not found.";
			ind1 = i;
			ind2 = j-i;
			return 1;
		}
	}
	ind1 = 0;
	ind2 = nLines;
	return 2; // no looping, just continue line by line
}

CLineParser::CLineParser(void)
:nTags(0), tag(NULL), errstr("")
{

}

CLineParser::~CLineParser(void)
{
	if (nTags>0) delete[] tag;
}

int CLineParser::IsExist(const char *tagname)
{
	//returns the index if it exists, -1 otherwise.
	int i;
	for (i=0; i<nTags; i++) 
	{
		if (!strcmp(tag[i].str, tagname)) 
		{
			return i;
		}
	}
	return -1;
}

void CLineParser::UpdateTag(CEqSig *tag2add, int id2beInserted)
{
	int i;
	if (id2beInserted==-1)
	{
		for (i=0; i<nTags; i++) 
		{
			if (!strcmp(tag[i].str, tag2add->str)) 
			{
				tag[i] = *tag2add;
				return;
			}
		}
	}
	CEqSig *newTagList = new CEqSig [++nTags];
	if (id2beInserted<0 || id2beInserted>nTags-1) id2beInserted = nTags-1;
	for (int i=0; i<id2beInserted; i++) 
		newTagList[i] = tag[i];
	newTagList[id2beInserted] = *tag2add;
	for (int i=id2beInserted+1; i<nTags; i++)  
		newTagList[i] = tag[i-1];
	delete[] tag;
	tag = newTagList;
}

int CLineParser::RemoveTag(int id)
{
	if (id>=nTags || id<-1) return 0;
	if (id==-1)
	{
		if (tag!=NULL)	delete[] tag;
		nTags = 0;
		tag = NULL;
		return 1;
	}
	CEqSig *newTagList = new CEqSig [--nTags];
	for (int i=0; i<id; i++) newTagList[i] = tag[i];
	for (int i=id; i<nTags; i++) newTagList[i] = tag[i+1]; 
	delete[] tag;
	tag = newTagList;
	return 1;
}

int CLineParser::RemoveTag(const char *tagname)
{
	int id;
	if ((id=IsExist(tagname))>=0)
		return RemoveTag(id);
	else
		return 0;
}

int CLineParser::Lines2Tags(string *line, int nLines)
{
	int in1(0), in2(0), res;
	try 
	{
		res=ExtractLines4Loop(line, nLines, in1, in2);
		if (res==2)	EvalNonLoopLines(line, in2);
		else
		{
			// up to the loop
			EvalNonLoopLines(line, in1);
			EvalLoop(line+in1, in2);
			EvalNonLoopLines(line, in1-1);
		}
		return 1;
	}
	catch (char *buf)
	{
		errstr = buf;
		return 0;
	}
}

int CLineParser::EvalNonLoopLines(string *line, int nLines)
{
	CEqSig runner(22050);
	char buf[256];
	for (int i=0; i<nLines; i++)
	{
		runner.sig.Reset(22050);
		runner.setstr(line[i].c_str());
		if (!runner.Make(nTags, tag, buf)) {throw buf;}
		UpdateTag(&runner, -1);
	}
	return 1;
}


int CLineParser::DoTheseIfTrue(string &condstr, string *line, int nLines)
{
	CEqSig xx(-1, condstr.c_str());
	char buf[256];
	if (!xx.Make(nTags, tag, buf)) 	{throw buf;}
	if (xx.sig.value()) 
	{
		Lines2Tags(line, nLines);
		return 1;
	}
	else
		return 0;
}


int CLineParser::EvalLoop (string *line, int nLines)
{ // first line: if, for or while followed by condition string or step
	char buf[256];
	string firstwo[2];
	str2array(firstwo, 2, line[0].c_str(), " ");
	firstwo[1] = line[0].substr(firstwo[0].length(), line[0].length()-line[0].find(firstwo[0]));
	int res;
	if (firstwo[0]=="if")
	{
		return DoTheseIfTrue(firstwo[1], line+1, nLines);
	}
	else if (firstwo[0]=="while")
	{
		do 
		{
			res = DoTheseIfTrue(firstwo[1], line+1, nLines);
		} while (res);
	}
	else if (firstwo[0]=="for")
	{
		res = countDeliminators(firstwo[1].c_str(), ",");
		string reconst, *cond = new string[res];
		int insertedID;
		str2array(cond, res, firstwo[1].c_str(), ","); 
		array2str(reconst, cond+1, res-1, ",");
		//evaluate the looping variable
		CEqSig loop(-1, cond[0].c_str());
		bool b(true);
		delete[] cond;
		if (!loop.Make(nTags, tag, buf)) 	{throw buf;}
		for (int i=0; i<loop.len(); i++)
		{
			CEqSig loopval(-1, loop.str);
			loopval.sig.SetValue(loop.sig.buf[i]);
			insertedID = nTags-1;
			UpdateTag(&loopval, -1);
			for (int pos(0), j=1; j<res-1 && b; j++)
			{
				CEqSig xx(-1, reconst.substr(pos, reconst.find(',', pos)).c_str());
				if (!xx.Make(nTags, tag, buf)) { RemoveTag ( insertedID); throw buf;}
				if (!xx.sig.value()) b = false;
				else pos++;
			}
			if (b) 
			{
				Lines2Tags(line+1, nLines);
			}
		}
		RemoveTag (insertedID);
	}
	return 0;
}
