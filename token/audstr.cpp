#pragma once
#include <string>
#include <sstream>
#include <windows.h>

using namespace std;

#ifdef __AFXWIN_H__
char* recoverNULL(CString *dummy, char *dest, const char *src)
{
	return strcpy(dest, src);
}
#endif

char* recoverNULL(string *dummy, char *dest, const char *src)
{
	return strcpy(dest, src);
}

void sformat(string& out, size_t nLengthMax, const char* format, ...) 
{
	char *buffer = new char[nLengthMax];
	va_list vl;
	va_start(vl, format);
	vsnprintf(buffer, nLengthMax, format, vl);
	buffer[nLengthMax-1] =0;
	out = string(buffer);
	delete[] buffer;
}

void triml(string& str, string delim)
{
  string::size_type pos = str.find_first_not_of(delim);
  if(pos != string::npos) str.erase(0, pos);
  else str.erase(str.begin(), str.end());
}

void trimr(string& str, string delim)
{
  string::size_type pos = str.find_last_not_of(delim);
  if(pos != string::npos) str.erase(pos + 1);
  else str.erase(str.begin(), str.end());
}

void trim(string& str, string delim)
{
	triml(str, delim);
	trimr(str, delim);
}

void triml(string& str, char delim)
{
	string _delim(1,delim);
	triml(str, _delim);
}

void trimr(string& str, char delim)
{
	string _delim(1,delim);
	trimr(str, _delim);
}

void trim(string& str, char delim)
{
	string _delim(1,delim);
	trim(str, _delim);
}

void triml(string& str, char *delim)
{
	string _delim(delim);
	triml(str, _delim);
}

void trimr(string& str, char *delim)
{
	string _delim(delim);
	trimr(str, _delim);
}

void trim(string& str, char *delim)
{
	string _delim(delim);
	trim(str, _delim);
}