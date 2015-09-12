#include "stdafx.h"
#include <math.h>
#include "audfret.h"
#include "bjtools_internal.h"

#define MAX_HEADING_LENGTH		128
#define MAX_CHAR 29998

void getVersionStringFromNumber (const char* verStringIn, char* verStringOut)
{
	int ver[4];
	char buf[5];
	memset(ver, 0, sizeof(ver));
	str2intarray (ver, 4, verStringIn, "., ");

	sprintf (verStringOut, "v%d.%d%d", ver[0], ver[1], ver[2]);
	if (ver[3]!=0)
	{
		sprintf (buf, "%d", ver[3]);
		strcat(verStringOut, buf);
	}
}

int countFloatingPoints(double f, int max_fp)
{
	int i, tens;
	double err, errCrit=1.;

	for (i=0; i<max_fp; i++)
		errCrit /= 10.;
	tens = 1;
	for (i=0; i<max_fp; i++)
	{
		err = (f*tens - (double)(__int64)(f*tens)) / tens;
		if (err<errCrit)
			return i;
		tens *= 10;
	}
	return i;
}

AUDFRET_EXP char* getVersionStringAndUpdateTitle  (HWND hDlg, const char* executableName, char* verStringOut)
{
	int i;
	DWORD  dwSize, dwReserved;
	UINT dwLength;
	LPVOID lpBuffer, lpStr;
	char zbuf[64];

	dwSize = GetFileVersionInfoSize( executableName, &dwReserved );
	if (dwSize==0)
	{
		verStringOut[0]='\0';
		return verStringOut;
	}
	lpBuffer = HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize );
	GetFileVersionInfo( executableName, 0, dwSize, lpBuffer );
	i=VerQueryValue( lpBuffer, "\\StringFileInfo\\040904b0\\FileVersion", &lpStr, &dwLength );
	if (i!=0)
		getVersionStringFromNumber ((char* )lpStr, verStringOut);
	else
	{
		MessageBox (NULL, "Version information not available", "version?", MB_OK);
		HeapFree( GetProcessHeap(), 0, lpBuffer );
		verStringOut[0]='\0'; return verStringOut;
	}
	if (hDlg!=NULL)
	{
		GetWindowText(hDlg, zbuf, 100);
		strcat (zbuf, " ");
		SetWindowText (hDlg, strcat (zbuf, verStringOut));
	}
	HeapFree( GetProcessHeap(), 0, lpBuffer );
	return verStringOut;
}

AUDFRET_EXP int VersionCheck (const char *procName, const char *requiredVersionStr)
{
	char ver[64];

	getVersionStringAndUpdateTitle(NULL, procName, ver);
	if (strlen(ver)==0)		return 0;
	// The first character of ver is v...
	if (strcmp(&ver[1],requiredVersionStr)<0)		return 0;
	else		return 1;
}

AUDFRET_EXP int countDeliminators (const char* buf, const char* deliminators)
{
	char *token, *newBuffer;
	int i=0;
	newBuffer = new char[strlen(buf)+1];
	strcpy(newBuffer, buf);

	token = strtok( newBuffer, deliminators );
	while( token != NULL )
	{
		i++;
		token = strtok( NULL, deliminators );
	}
	delete[] newBuffer;
	return i; 
}

/*
AUDFRET_EXP int str2intarray (int* x, int maxSize_x, const char* str, const char* deliminators)
{
	// maxSize_x is the size of the input array x (not the last index)
	// if the actual number of parsed elements exceed it, no more parsing is performed. 
	// (the rest of str is ignored to prevent a pointer error.).

	char *token, *newBuffer;
	int i=0;
	size_t len = strlen(str);
	size_t len2 = strlen(deliminators);
	char tail;
	unsigned int j, tailtick=0;

	if (x==NULL || str==NULL || deliminators==NULL)
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
			if (sscanf(token, "%d", &x[i++])==0) return 0;
	   token = strtok( NULL, deliminators );
	}
	free(newBuffer);
	return i; 
}

AUDFRET_EXP int str2doublearray (double* x, int maxSize_x, const char* str, const char* deliminators)
{
	char *token, *newBuffer;
	int i=0;
	size_t len = strlen(str);
	size_t len2 = strlen(deliminators);
	char tail;
	unsigned int j, tailtick=0;

	if (x==NULL || str==NULL || deliminators==NULL)
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
		   if (sscanf(token, "%lf", &x[i++])==0) return 0;
	   token = strtok( NULL, deliminators );
	}
	free(newBuffer);
	return i;
}

AUDFRET_EXP int str2strarray (char** x, int maxSize_x, const char* str, const char* deliminators)
{
	char *token, *newBuffer;
	int i=0;
	size_t len = strlen(str);
	size_t len2 = strlen(deliminators);
	char tail;
	unsigned int j, tailtick=0;

	if (x==NULL || str==NULL || deliminators==NULL)
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
		   strcpy(x[i++],token);
	   token = strtok( NULL, deliminators );
	}
	free(newBuffer);
	return i;
}
*/
AUDFRET_EXP int countchar(const char *str, char c)
{
	int count(-1);
	const char *ploc(str), *ploc2(str);
	while (ploc2!=NULL)
	{
		ploc2 = strchr(ploc, c);
		if (ploc2!=NULL)	ploc = ploc2+1;
		count++;
	}
	return count;
}

AUDFRET_EXP int countstr(const char *str, char* marker)
{
	int count(-1);
	const char *ploc(str), *ploc2(str);
	while (ploc2!=NULL)
	{
		ploc2 = strstr(ploc, marker);
		if (ploc2!=NULL)	ploc = ploc2+1;
		count++;
	}
	return count;
}

AUDFRET_EXP int countMarkers(const char *str, char* marker)
{
	//HAVEN'T VERIFIED 04/11/2009
	int i,b=1;
	size_t len, lenMarker;
	char *ptFound, *ptCurrent, *newBuffer;
	if (marker==NULL)
		return -1;
	i=0;
	lenMarker = strlen(marker);
	len = strlen(str);
	newBuffer = (char*)calloc(len+1, sizeof(char));
	strcpy(newBuffer, str);
	ptCurrent=newBuffer;
	while (b)
	{
		ptFound = strstr(ptCurrent, marker);
		if (ptFound==ptCurrent)
		{
			ptCurrent = ptFound + lenMarker;
		}
		else 
		{
			if (ptFound==NULL)
			{
				ptFound = ptCurrent + strlen(ptCurrent);
				b=0;
			}
			if (ptCurrent[0]!='\0')
				i++;
			ptCurrent = ptFound + lenMarker;
		}
	}
	free(newBuffer);
	return i;
}

void EditPrintf0 (HWND hwndEdit, char *str)
{
	char buf[MAX_CHAR+1];
	GetWindowText (hwndEdit, buf, sizeof(buf));
	SendMessage (hwndEdit, EM_SETSEL, (WPARAM)strlen(buf), (LPARAM)strlen(buf)) ;
	 SendMessage (hwndEdit, EM_REPLACESEL, FALSE, (LPARAM) str) ;
	 if (GetFocus()!=hwndEdit)	 SendMessage (hwndEdit, EM_SCROLLCARET, 0, 0) ;//don't understand how this works.
}

AUDFRET_EXP void ClearEditPrintf (HWND hwndEdit)
{
	LRESULT i;
     i=SendMessage (hwndEdit, EM_SETSEL, (WPARAM) 0, (LPARAM) -1) ;
     i=SendMessage (hwndEdit, EM_REPLACESEL, FALSE, (LPARAM)"") ;
     i=SendMessage (hwndEdit, EM_SCROLLCARET, 0, 0) ;
}

AUDFRET_EXP void EditPrintf (HWND hwndEdit, const char * szFormat, ...)
{
     static char szBuffer[MAX_CHAR+1], prevStr[MAX_CHAR+1];
     va_list pArgList ;
	 size_t size1, size2;
	 int nChar2Delete=0, trimBegin=0;

     va_start (pArgList, szFormat) ;
     vsprintf (szBuffer, szFormat, pArgList) ;
     va_end (pArgList) ;

	 GetWindowText (hwndEdit, prevStr, sizeof(prevStr));
	 size1 = strlen(prevStr);
	 size2 = strlen(szBuffer);
	 if (size2>MAX_CHAR) 
	 {	trimBegin = (int)size2-MAX_CHAR;
		size2 = strlen(szBuffer+trimBegin);
	 }
	 if (size1+size2 > MAX_CHAR) 
	 {	 nChar2Delete = (int)(size1+size2)-MAX_CHAR;
		 ClearEditPrintf (hwndEdit);
		 EditPrintf0 (hwndEdit, &prevStr[nChar2Delete]);
	 }
	 EditPrintf0 (hwndEdit, szBuffer+trimBegin);
}

AUDFRET_EXP BOOL IsBetween (int x, int begin, int ending)
{
	// inclusive the edge points
	if (x>=begin && x<=ending)
		return 1;
	else 
		return 0;
}

AUDFRET_EXP void SetDlgItemDouble (HWND hDlg, int id, char *formatstr, double x)
{
	char buf[256];
	sprintf (buf, formatstr, x);
	SetDlgItemText (hDlg, id, buf);
}

AUDFRET_EXP double GetDlgItemDouble (HWND hDlg, int id, int* lpTrans)
{	
	int res;
	double x;
	char buf[32];
	GetDlgItemText (hDlg, id, buf, sizeof(buf));
	res = sscanf(buf,"%lf", &x);
	if (lpTrans==NULL)
		return x;
	*lpTrans = (res>0);
	return x;
}

AUDFRET_EXP char * CutEndSpace(char * str, char c)
{
	int i;
	for (i=(int)strlen(str)-1; i>0; i--)
	{
		if (str[i]==c)
			str[i]='\0';
		else
			return str;
	}
	return str;
}

void sprintfFloat(char *strOut, double f, int max_fp)
{
	int fp;
	char *temp, format[64];

	if (f<0)
	{
		sprintfFloat(strOut, -f, max_fp);
		temp = (char*)calloc(strlen(strOut)+2,1);
		strcpy(temp,"-");
		strcat(temp+1, strOut);
		strcpy(strOut, temp);
		free(temp);
		return;
	}

	if ((fp=countFloatingPoints(f, max_fp))==0)
		sprintf (strOut, "%d", (int)f);
	else
	{
		sprintf (format, "%%%d.%df", fp+3, fp);
		sprintf (strOut, format, f);
	}
	trimLeft(strOut, " ");
}

AUDFRET_EXP int sprintfFloat(double f, int max_fp, char *strOut, size_t sizeStrOut)
{
	char buffer[128];
	if (f>1.e10 || f<-1.e10) {	memset((void*)strOut,'*', sizeStrOut-2); strOut[sizeStrOut-1]=0; return 0;}
	sprintfFloat (buffer, f, max_fp);
	if (strlen(buffer)>sizeStrOut-1)
	{
		if (sizeStrOut>1) 
		{	memset((void*)strOut,'*', sizeStrOut-2); strOut[sizeStrOut-1]=0;}
		else if (sizeStrOut==1) 
			strOut[sizeStrOut-1]=0;
		return 0;
	}
	strcpy(strOut, buffer);
	return 1;
}

AUDFRET_EXP void GetCurrentProcInfo (char *path, char *procName, char *verStr)
{
//Can AfxGetInstanceHandle() be used instead of GetModuleHandle()?
	char fullname[512];
 	char drive[16], dir[256], ext[8], buffer[256];
	GetModuleFileName (GetModuleHandle(NULL), fullname, sizeof(fullname));

 	_splitpath(fullname, drive, dir, buffer, ext);
	if (path!=NULL)
		sprintf(path, "%s%s", drive, dir);
	if (procName!=NULL)
		sprintf(procName, "%s%s", buffer, ext);
	if (verStr!=NULL)
		getVersionStringAndUpdateTitle(NULL, fullname, verStr);
}

AUDFRET_EXP int EnableDlgItem (HWND hwnd, int id, int onoff)
{
	return EnableWindow(GetDlgItem(hwnd, id), onoff);
}

AUDFRET_EXP int ShowDlgItem (HWND hwnd, int id, int nCmdShow)
{
	return ShowWindow (GetDlgItem(hwnd, id), nCmdShow);
}

AUDFRET_EXP char *replaceString (char *str, size_t lenStr, const char *find, const char *replace2)
{

/*  NOT WORKING PROPERLY
    len2copy is computed incorrectly.
   Check this before using this function further
 Nov 29 2007
*/


	/* lenStr is the allocated size of str.
	When replace2 is longer than find, if strlen(str) is big such that the whole string 
	cannot be contained within lenStr after replacement, it will truncate to lenStr*/
	char *pdest;
	int diff, cum;
	size_t foundInd, id1, id2, len2copy, len = min(lenStr, strlen(str));
	if (strlen(find)==0)		return str;
	if (strlen(str)==0)			return str;
	if (strlen(str) > len)		str[len]=0;

	char *buffer = new char[lenStr];

	id1=id2=0;
	cum=0;
	diff=(int)strlen(replace2)-(int)strlen(find);
	pdest = strstr(str+id1, find);
	while (pdest!=NULL)
	{
		foundInd = pdest - (str+id1);
		strncpy(buffer+id2, str+id1, foundInd); buffer[foundInd+id2]='\0';
		len2copy = min (strlen(replace2), lenStr-foundInd-id2-1);
		strncat(buffer, replace2, len2copy);
		id1 += foundInd + strlen(find);
		pdest = strstr(str+id1+1, find);
		id2 += foundInd+len2copy;
		cum++;
	}
	len2copy = min (len-id2+cum*diff, lenStr-id2-1);
	if (len2copy>0)
	{
		strncpy(buffer+id2, str+id2-cum*diff, len2copy); 
		buffer[id2+len2copy]='\0';
	}
	strcpy(str, buffer);
	delete[] buffer;
	return str;
}

AUDFRET_EXP char *trimLeft (char* string, const char *chars2Trimmed)
{
	char *p;
	for (p=string; *p && strchr(chars2Trimmed, *p); p++)
		;
	memmove(string, p, strlen(p)+1);
	return string;
}

AUDFRET_EXP char *trimRight (char* string, const char *chars2Trimmed)
{
	char *p;
	for (p=strchr(string, '\0')-1; p>=string && strchr(chars2Trimmed, *p); p--)
		;
	*(p+1) = '\0';
	return string;
}

AUDFRET_EXP int IsSemiMonotonic (int len, double *x)
{
	int i, updown;
	updown = x[1]>x[0];
	for (i=2; i<len; i++)
	{
		if ( updown != (x[i]>x[i-1]) )
			return 0;
	}
	return 1;
}

AUDFRET_EXP int belowabove (double val, int len, double *x)
{
	if (val<x[0]) 
		return -1;
	if (val>=x[len-1]) 
		return len-1;
	return 0;
}

AUDFRET_EXP int GetMaxInd4Cut (double criterion, int len, double *x)
{
	// Find the greatest index of the array x, which has the element smaller than (or the same as) val.
	// if it is the first one, returns -1. return value below -1 indicates an error.
	if (len<=0) return -2; // invalid length
	if (!IsSemiMonotonic(len, x)) return -3; // not (semi)monotonic
	if (x[0]>x[1]) return -4; // must not be decreasing.

	int lastID, curId, runlen(len);
	curId=0;
	lastID = belowabove (criterion,len,x);
	if (lastID!=0)
		return lastID;
	while ( runlen>1 && curId<len)
	{
		runlen /= 2;
		if (belowabove (criterion,runlen,x+curId)>0)
			curId += runlen + belowabove (criterion,runlen,x+curId+runlen);
	}
	if (curId>=len) curId = len-1;
	return curId;
}

AUDFRET_EXP int mod (int big, int divider)
{
// calculating int remainder
	return (int)fmod((double)big, (double)divider);
}

AUDFRET_EXP int getrand(int x)
{// generates a random number between 0 and x-1
	if (x==0) return 0;
	return mod(rand(),x);
}

AUDFRET_EXP short double2short(double x)
{
	// This maps a double variable raning -1 to 1, to a short variable ranging -32768 to 32767.
	return (short)(x*32768.-.5);
}

AUDFRET_EXP double short2double(short x)
{
	return ((double)x+.5)/32767.5;
}

AUDFRET_EXP double getexp(double dB)
{
	return exp( log(10.)/20.*(dB));
}

AUDFRET_EXP char * FulfillFile (char *out, const char *path, const char * file)
{
	size_t len = strlen(path);
	if (len==0)		
		strcpy(out, file);
	else if (path[len-1]!='\\')
		sprintf(out, "%s\\%s", path, file);
	else
		sprintf(out, "%s%s", path, file);
	return out;
}

AUDFRET_EXP void GetPathFileNameFromFull (char *path, char *fname, const char *fullname)
{
	char drive[MAX_PATH], dir[MAX_PATH], buf[MAX_PATH], ext[32];
 	_splitpath(fullname, drive, dir, buf, ext);
	sprintf(path,"%s%s", drive, dir);
	sprintf(fname,"%s%s", buf, ext);
}

AUDFRET_EXP int GetSurroundedBy(char c1, char c2, const char* in, char* out, int iStart)
{
	// This extracts the string bounded by c1 and c2 (the first boundary)
	// returns the index of the ending border
	// c1 and c2 must be different
	// out must have been allocated by the caller.
	bool inside(false), loop(true);
	int i, cumInt(0);
	if (in==NULL || out==NULL) return -1;
	size_t len = strlen(in);
	out[0]='\0';
	if (len>65535)  return -1;
	for (i=iStart; i<(int)len && loop; i++)
	{
		if (in[i]==c1) { cumInt++; inside=true;}
		else if (in[i]==c2) cumInt--; 
			if (cumInt<0) return -1; // unbalanced parenthesis 
		if (inside && cumInt==0) loop=false;
	}
	if (cumInt!=0 || !inside) return -1;
	const char *firstind_c1 = strchr(in+iStart, c1);
	int first = (int) (firstind_c1 - in);
	int count = i - first-2;
	strncpy(out, firstind_c1+1, count);
	out[count]='\0';
	return i-1;
}

AUDFRET_EXP void GetLastErrorStr(DWORD ecode, char *errstr)
{
	char  string[256];
	DWORD nchar;

	nchar = FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, ecode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							(LPTSTR) string, sizeof(string), NULL  );
	if (nchar == 0)
		MessageBox (NULL, "Failed to translate Windows error", "", MB_OK);
	else
	{
		trimRight(string, "\r");
		trimRight(string, "\n");
		strcpy(errstr, string);
	}
	return;
}

AUDFRET_EXP void GetLastErrorStr(char *errstr)
{
	GetLastErrorStr(GetLastError(), errstr);
}
