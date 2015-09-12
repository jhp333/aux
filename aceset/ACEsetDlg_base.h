#include "FileDlg.h"

/* Note on Oct 28, 2010. BJ Kwon

It is difficult to explicitly load member functions of CParamACE class (maybe almost impossible),
particularly its constructor, from aceproc__.dll (the names will depend on the matlab vesion), so
the member object of the CParamACE class is not used in either CACEDlg nor CACEplayerDlg class.
Instead only the pointer to CParamACE is the member of these class, and it is user's responsibility
to instantiate them during the use.

In other words, 

CParamACE param;   ---> (X)

CParamACE *param;   ---> (O)  .

Then param object must be instantiated by newCParamACE call prior to its use.... so
probably the best way to handle this: as soon as newCParamACE is available, this should be done.
Therefore, this is done in the Init member function. If you are going to make a full use of newCParamACE 
(not as a dummy), you must call Init prior to any other operation.

I referenced the document from http://www.codeguru.com/cpp/w-p/dll/importexportissues/article.php/c123

*/

class CACEDlg :	public CWndDlg
{
public:
	ACESET_API void SetAddMLines(const char *str);
	ACESET_API void EVK_OpenMAP();
	ACESET_API void EVK_CloseMAP();
	ACESET_API double GetLevel();
	ACESET_API HWND Create(HWND hPar, POINT position);
	ACESET_API CACEDlg* GetThis();
	ACESET_API int Screen2Param();
	ACESET_API int Param2Screen();
	ACESET_API int UpdateData(bool data2screen);
	ACESET_API void OpenMap(const char* mapfilefull, InterfaceType iProc);
	ACESET_API int Init(const char* dllname, char* errstr);

	int readACEPatientFile (const char *fname, CParamACE *param);
	int writeACEPatientFile (const char *fname, CParamACE *param, char *errStr);

	BOOL OnInitDialog(HWND hwndFocus, LPARAM lParam);
	void onOpenMap(const char* mapfilefull, InterfaceType iProc);
	void OnCommand(int idc, HWND hwndCtl, UINT event);

	int UpdateCutoffFreq();

	ACESET_API CACEDlg(void);
	ACESET_API ~CACEDlg(void);
	
	CFileDlg fdlg;
	CParamACE *param;
	CStdString errstr;
	char MAPfilename[MAX_PATH];

	int (*GateProcess)(ACESEQ *, const char *, const char *, const char *, CParamACE *, char **, char **);
	int (*GetCutoffFreqs)(int, double*, double*);
	int (*FixCRCR)(char *);
	int (*InitMatlabLib)(const char *, char **);
	void (*CloseMatlabLib)();
	CParamACE* (*newCParamACE)();
};


typedef int (*PF_INT_GATEPROC) (ACESEQ *, const char *, const char *, const char *, CParamACE *, char **, char **);
typedef int (*PF_INT_FREQ) (int, double*, double*);
typedef int (*PF_CRCR) (char *);
typedef int (*PF_INITMATLAB) (const char *, char **);
typedef void (*PF_VOID) ();
typedef CParamACE* (*PF_CPARAM) ();

