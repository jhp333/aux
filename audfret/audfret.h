#ifndef BJTOOLS
#define BJTOOLS

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

//If this library is linked statically define LINK_STATIC_AUDFRET in the application project.
#ifndef LINK_STATIC_AUDFRET
#define AUDFRET_EXP __declspec (dllexport)
#else 
#define AUDFRET_EXP 
#endif

#define AUD_ERR_FILE_NOT_FOUND			-2
#define AUD_ERR_CANNOT_READ_FILE		-3
#define AUD_ERR_HEADING_NOT_FOUND		-4
#define AUD_ERR_HEADING_FOUND_MULTIPLE	-5


AUDFRET_EXP char* getVersionStringAndUpdateTitle  (HWND hDlg, const char* executableName, char* verStringOut);
AUDFRET_EXP void EditPrintf (HWND hwndEdit, const char * szFormat, ...);
AUDFRET_EXP void ClearEditPrintf (HWND hwndEdit);
AUDFRET_EXP int countDeliminators (const char* buf, const char* deliminators);
AUDFRET_EXP BOOL IsBetween (int x, int begin, int ending);
AUDFRET_EXP int GetMaxInd4Cut (double criterion, int len, double *x);
AUDFRET_EXP void SetDlgItemDouble (HWND hDlg, int id, char *formatstr, double x);
AUDFRET_EXP void GetCurrentProcInfo (char *path, char *procName, char *verStr);
AUDFRET_EXP char* FulfillFile (char* out, const char *path, const char * file);
AUDFRET_EXP void GetPathFileNameFromFull (char *path, char *fname, const char *fullname);

AUDFRET_EXP int sprintfFloat(double f, int max_fp, char *strOut, size_t sizeStrOut);
AUDFRET_EXP int countMarkers(const char *str, char* marker);
AUDFRET_EXP int countstr(const char *str, char* marker);
AUDFRET_EXP int countchar(const char *str, char c);
AUDFRET_EXP int EnableDlgItem (HWND hwnd, int id, int onoff);
AUDFRET_EXP int ShowDlgItem (HWND hwnd, int id, int nCmdShow);
AUDFRET_EXP int VersionCheck (const char *procName, const char *requiredVersionStr);
AUDFRET_EXP int printfINI (char *errstr, const char *fname, const char *heading, const char * szFormat, ...);
AUDFRET_EXP int ReadINI (char *errstr, const char *fname, const char *heading, char *strout, size_t strLen);
AUDFRET_EXP int ReadINI(char *errstr, const char *fname, const char *heading, std::string &strOut);
AUDFRET_EXP int sscanfINI (char *errstr, const char *fname, const char *heading, const char * szFormat, ...);
AUDFRET_EXP int GetSurroundedBy(char c1, char c2, const char* in, char* out, int iStart);

/*Do not use %s format for sscanfINI ---it won't work*/
/* sprintfFloat generates a string array from a double value, its floating point varies depending on the value and specified max_fp  */

AUDFRET_EXP char *trimLeft (char* string, const char *chars2Trimmed);
AUDFRET_EXP char *trimRight (char* string, const char *chars2Trimmed);
AUDFRET_EXP int howmanychr(const char* buffer, const char c);
AUDFRET_EXP int howmanystr(const char* buffer, const char* str);
AUDFRET_EXP char *replaceString (char *str, size_t lenStr, const char *find, const char *replace2);
AUDFRET_EXP short double2short(double x);
AUDFRET_EXP double short2double(short x);
AUDFRET_EXP int mod (int big, int divider);
AUDFRET_EXP int getrand(int x);
AUDFRET_EXP double getexp(double dB);

AUDFRET_EXP int Registry(int isitget, HKEY reg_key_handle, const char *keyName, const char *ItemName, char *buffer, char *errstr);
AUDFRET_EXP void GetLastErrorStr(DWORD ecode, char *errstr);
AUDFRET_EXP void GetLastErrorStr(char *errstr);
AUDFRET_EXP double compFileTime(const char *fname1, const char *fname2);
AUDFRET_EXP int isSameFile(const char *fname1, const char *fname2);
AUDFRET_EXP int compFileLength(const char *fname1, const char *fname2);

#ifdef __cplusplus
AUDFRET_EXP double GetDlgItemDouble (HWND hDlg, int id, int* lpTrans=NULL);

//The functions below are not really for users--only for str2array
// In order to use str2array, make audstr.h and audstr.cpp part of the project with audfretDLL.lib
AUDFRET_EXP char * copyItem(char * out, char * in);
AUDFRET_EXP int copyItem(int out, char * in);
AUDFRET_EXP double copyItem(double out, char * in);
AUDFRET_EXP char* recoverNULL(int *dummy, char *dest, const char *src);
AUDFRET_EXP char* recoverNULL(float *dummy, char *dest, const char *src);
AUDFRET_EXP char* recoverNULL(double *dummy, char *dest, const char *src);
//end of str2array stuff


#else
double GetDlgItemDouble (HWND hDlg, int id, int* lpTrans);
#endif

AUDFRET_EXP INT_PTR InputBox(
    LPCTSTR szTitle, 
    LPCTSTR szPrompt, 
    LPTSTR szResult, 
    DWORD nResultSize,
    bool bMultiLine = false,
    HWND hwndParent = 0);

//Obsolete....for compatibility only
AUDFRET_EXP int str2intarray(int* out, int maxSize_x, const char * str, const char *deliminators);
AUDFRET_EXP int str2strarray(char** out, int maxSize_x, const char * str, const char *deliminators);
AUDFRET_EXP int str2doublearray(double* out, int maxSize_x, const char * str, const char *deliminators);
AUDFRET_EXP int readINI2Buffer (char *errstr, const char *fname, const char *heading, char * dynamicBuffer, int length);
AUDFRET_EXP int strscanfINI (char *errstr, const char *fname, const char *heading, char *strBuffer, size_t strLen);
AUDFRET_EXP int ReadItemFromINI (char *errstr, const char *fname, const char *heading, char *strout, size_t strLen);

#endif // BJTOOLS