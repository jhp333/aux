// AceconsolWnd.h: interface for the CAceconsolWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "..\WndDlg.h" //check this file... local.... different from common include
#include <time.h>

class CAceconsolWnd : public CWndDlg
{
public:
	void GetLastErrorStr(CStdString& str);
	CStdString PipeName;

	void OnCommand(int idc, HWND hwndCtl, UINT event);
	BOOL OnInitDialog(HWND hwndFocus, LPARAM lParam);
	void OnFlyArrived(WORD command, WORD len, void* inBuffer);
	void OnFlyClosed();
	void OnFlyConnected(char *hostname, char* ipa);

	HANDLE hPipe;
	CAceconsolWnd();
	virtual ~CAceconsolWnd();

private:
};

