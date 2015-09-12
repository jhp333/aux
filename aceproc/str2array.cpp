#pragma once

#include <string>
using namespace std;
#include "audfret.h"
#include "audstr.h"

AUDFRET_EXP char* recoverNULL(int *dummy, char *dest, const char *src)
{ return NULL; }
AUDFRET_EXP char* recoverNULL(float *dummy, char *dest, const char *src)
{ return NULL; }
AUDFRET_EXP char* recoverNULL(double *dummy, char *dest, const char *src)
{ return NULL; }

AUDFRET_EXP char * copyItem(char * out, char * in)
{
	return strcpy(out,in);
}

AUDFRET_EXP int copyItem(int out, char * in)
{
	int res = sscanf(in, "%d", &out);
	if (res==EOF || res==0) return 0;
	return out;
}

AUDFRET_EXP double copyItem(double out, char * in)
{
	int res = sscanf(in, "%lf", &out);
	if (res==EOF || res==0) return 0;
	return out;
}

AUDFRET_EXP int str2intarray(int* out, int maxSize_x, const char * str, const char *deliminators)
{
	return str2array(out, maxSize_x, str, deliminators);
}

AUDFRET_EXP int str2strarray(char** out, int maxSize_x, const char * str, const char *deliminators)
{
	return str2array(out, maxSize_x, str, deliminators);
}

AUDFRET_EXP int str2doublearray(double* out, int maxSize_x, const char * str, const char *deliminators)
{
	return str2array(out, maxSize_x, str, deliminators);
}

