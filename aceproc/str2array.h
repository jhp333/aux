#pragma once

#include <string>
using namespace std;

template <class T1, class T2>
T1 copyItem(T1 out, T2 in)
{
	out = (T1)in;
	return out;
}

template <>
char * copyItem(char * out, char * in)
{
	return strcpy(out,in);
}

template <>
int copyItem(int out, char * in)
{
	int res = sscanf(in, "%d", &out);
	if (res==EOF || res==0) return 0;
	return out;
}

template <>
double copyItem(double out, char * in)
{
	int res = sscanf(in, "%lf", &out);
	if (res==EOF || res==0) return 0;
	return out;
}

template <class T> 
AUDFRET_EXP int str2array(T* out, int maxSize_x, const char *str, const char *deliminators)
{
	char *token, *newBuffer;
	int i=0;
	size_t len = strlen(str);
	size_t len2 = strlen(deliminators);
	char tail;
	unsigned int j, tailtick=0;

	if (out==NULL || str==NULL || deliminators==NULL)
		return -1;
	if (maxSize_x<0)
		return -2;

	newBuffer = (char*)calloc(len+len2+1, sizeof(char));

	strcpy(newBuffer, str);
	tail = str[len];
	for (j=0; j<len2; j++)
		if (tail==deliminators[j])
			tailtick=1;
	if (tailtick)
	{
		strncpy(newBuffer+len, deliminators, 1);
		newBuffer[len+1] = '\0';
	}


	token = strtok( newBuffer, deliminators );
    while( token != NULL && maxSize_x>i)
    {
	   if (strlen(token)>0)
		   out[i] = copyItem(out[i], token);
	   i++;
	   token = strtok( NULL, deliminators );
	}
	free(newBuffer);
	return i;
}

template <class T>
AUDFRET_EXP int str2array(T* out, int maxSize_x, string str, const char *deliminators)
{
	char *newBuffer = new char[str.length()+2];
	strcpy (newBuffer, str.c_str());
	int res = str2array(out, maxSize_x, newBuffer, deliminators);
	delete[] newBuffer;
	return res;
}
