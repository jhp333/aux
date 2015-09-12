#if !defined(BATCH_DEFINED)
#define BATCH_DEFINED

#include "stdafx.h"
#include "WndDlg.h" 

class BATCH  : public CWndDlg
{
public:
	HWND hPreviousBatch;
	int onoff;
	char BEGIN_fname2save[256];
	void execute(char *commandLine, int batchornot);
	void OnRunbatch();
	BOOL OnInitDialog(HWND hwndFocus, LPARAM lParam);
	void OnCommand(int idc, HWND hwndCtl, UINT event);
	void OnClose();
	void OnDestroy();
	BATCH();
	virtual ~BATCH();
private:
};

#endif