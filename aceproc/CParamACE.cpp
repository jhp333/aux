#include "stdafx.h"

#include "aceproc.h"
#include <audfret.h>
#include <stdio.h>

#define N_ELEC		24 // NUMBER OF PHYSICAL ELECTRODES. Use this instead of PHYSICAL_ELEC_COUNT in SONIC


__declspec (dllexport) CParamACE::CParamACE()
:ptParam2(NULL), pg(0.), rate(0.), numBands(0), numMax(0), level(0.), Q(20), lenAddmlines(0)
{
	subj[0]='\0';
	mode[0]='\0';
	memset((void*)pw, 0, sizeof(pw));
	memset((void*)c, 0, sizeof(c));
	memset((void*)t, 0, sizeof(t));
	memset((void*)gains, 0, sizeof(gains));
	addmlines = NULL;
}

CParamACE::~CParamACE()
{
}

__declspec (dllexport) CParamACE* newCParamACE()
{
	return new CParamACE();
}

__declspec (dllexport) int CParamACE::mode2enum(char * remainderStr)
{
	char *str[2], str1[16], str2[16];
	int j, x[5];
	str[0]=str1;	str[1]=str2;

	if (str2strarray(str,2,mode," \t")>1)
		strcpy(remainderStr, str[1]);

	if (!_strnicmp (mode, "CG", 2))
	{
		return 0;
	}
	else if (!_strnicmp (mode, "MP", 2))
	{
		j=str2intarray(x,2, &mode[2], " +_=\\|:;""'<>,./?");
		if (j==0)
			return 103; // if MP, then treat it as MP1+2
		else if (j==1)
			if (x[0]==1 || x[0]==2)
				return 100+x[0];
			else if (x[0]==12)
				return 103;
			else 
				return -1;
		else if (j==2)
			if (x[0]==1 || x[1]==2)
				return 103;
			else 
				return -1;
		else
			return -1;
	}
	else if (!_strnicmp (mode, "BP", 2))
	{
		j=str2intarray(x,1, &mode[2], " +_=\\|:;""'<>,./?");
		if (j==0)
			return 1;
		else if (IsBetween(x[0],0,N_ELEC-4))
			return x[0]+1;
		else
			return -1;
	}
	else if (!_strnicmp (mode, "TP", 2))
	{
		j=str2intarray(x,1, &mode[2], " +_=\\|:;""'<>,./?");
		if (j==-1)
			return 201;
		else if (IsBetween(x[0],0,9))
			return x[0]+201;
		else
			return -1;
	}
	else
		return -1;
}

__declspec (dllexport) int CParamACE::GetRE(int ae)
{
	char buf[32];
	int res;
	if ((res=mode2enum(buf))>100) return res;
	else if (res<0) return -1;
	return ae + res;
}