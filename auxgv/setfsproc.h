#include <windows.h>
#include "WndDlg.h" 

class CSetFS : public CWndDlg
{
public:
	BOOL OnInitDialog(HWND hwndFocus, LPARAM lParam);
	void OnCommand(int idc, HWND hwndCtl, UINT event);
	void OnClose();
	void InitDialog2(int fs);
	CSetFS();
	~CSetFS();
};
