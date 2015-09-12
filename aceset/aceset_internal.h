#include <windows.h>
#include <StdString.h>
#include "aceproc.h"
#include "audfret.h"
#include "audstr.h"
#include "resource.h"

void sprintfFloat(double f, int max_fp, CStdString& strOut);
CStdString& FulfillFile (CStdString& out, const char *path, const char * file);
int strscanfINI (char *errstr, const char *fname, const char *heading, CStdString& outString);
int UpdateCutoffFreq (HWND hDlg);
void FileDlgInit(OPENFILENAME *ofn, const char *initDir);
BOOL FileOpenDlg (OPENFILENAME *ofn, HWND hwnd, LPSTR pstrFileName, LPSTR pstrTitleName, LPSTR szFilter, LPSTR lpstrDefExt);
BOOL FileSaveDlg (OPENFILENAME *ofn, HWND hwnd, LPSTR pstrFileName, LPSTR pstrTitleName, LPSTR szFilter, LPSTR lpstrDefExt);

typedef enum
{
	UNSPECIFIED = 0,
	SPRINT,
	L34_CIC3,
	L34_CIC4,
} InterfaceType;

#include "aceset.h"
