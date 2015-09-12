// License.cpp : implementation file
//

#include "stdafx.h"
#include "psycon.h"
#include "License.h"
#include "Request.h"

void encode (char *buffer, int len)
{
	for (int i=0; i<len; i++)
		if (buffer[i]!='\0' && buffer[i]!='\r' && buffer[i]!='\n' && buffer[i]!='\x1f') buffer[i] += 10;
}

#define LICENSE_FILE "apreginf.dat"

int IsRecMsgOK(const char *buffer)
{
	if (strstr(buffer, "200 OK")!=NULL) 	return 1;
	else return 0;
}


void decode (char *buffer, int len)
{
	for (int i=0; i<len; i++)
		if (buffer[i]!='\0' && buffer[i]!='\r' && buffer[i]!='\n' && buffer[i]!='\x1f') buffer[i] -= 10;
}

int checkLicFile()
{
	HANDLE hFile;
	char buffer[32], dir[MAX_PATH], fname[MAX_PATH];
	DWORD dw;
	int res;

	GetSystemDirectory(dir, sizeof(dir));
	FulfillFile(fname, dir, LICENSE_FILE);
	if ( (hFile = ::CreateFile (fname, GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ))== INVALID_HANDLE_VALUE)
				res = -1;
	else
	{
		if (ReadFile(hFile, buffer, 32, &dw, NULL)) 
		{
			if (dw>20) 	res = 0;
			else
			{
				buffer[dw]=0;
				// The content of apreginf.dat must be z83(space)encoded_date
				// format of date is YEARMODY
				if (strncmp(buffer,"z83 ", 4)) res = 0;
				else
				{
					decode (buffer+4, dw-4);
					SYSTEMTIME stime;
					GetSystemTime (&stime);
					int today=stime.wYear*10000 + stime.wMonth * 100 + stime.wDay;
					int day2compare;
					sscanf(buffer+4, "%d", &day2compare);
					if (today> day2compare)
					{
						CloseHandle(hFile);
						AfxMessageBox("The license has expired. Update your license.");
						return -1;
					}
					res = 1;
				}
			}
		}
		else res = 0;
		CloseHandle(hFile);
	}
	return res;
}



// CLicense dialog

IMPLEMENT_DYNAMIC(CLicense, CDialog)

CLicense::CLicense(CWnd* pParent /*=NULL*/)
	: CDialog(CLicense::IDD, pParent)
{

}

CLicense::~CLicense()
{
}

void CLicense::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLicense, CDialog)
	ON_BN_CLICKED(IDOK, &CLicense::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK, &CLicense::OnBnClickedCheck)
END_MESSAGE_MAP()


void CLicense::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CLicense::OnBnClickedCheck()
{
	// the dlg box should close-return 10 if authentication was temporary, so that it can be set in apreginf.dat
	EnableID(this, IDC_CHECK, 0);
	Request main;
	int res(0);
	char url[256],code[64];
	GetDlgItemText(IDC_CODE, code, sizeof(code));

	CString sHeaderSend, sHeaderReceive, sMessage, items[2];
	sprintf(url,"http://auditorypro.com/dot/sres.php?key=%s", code);

	sbar.SetText("Connecting to authentication server...", 0, 0);
	if (main.SendRequest(false, url, sHeaderSend, sHeaderReceive, sMessage)!=1 || !IsRecMsgOK(sHeaderReceive))
	{
		EndDialog(0); // connection to server failed or PhP in the server failed
	}
	else
	{
		sscanf(sMessage, "%d", &res);
		EndDialog(res);
	}
	return ;
}

BOOL CLicense::OnInitDialog()
{
	CDialog::OnInitDialog();
	sbar.Create	(WS_CHILD|WS_VISIBLE, CRect(1,2,1,2), this, 1000);

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CheckLicense(char* errmsg)
{
	HANDLE hFile, hFile2;
	FILETIME ft;
	char buffer[16], dir[MAX_PATH], fname[MAX_PATH];
	DWORD dw;
	int res;
	CLicense dlgLicense;

	while (checkLicFile()!=1)
	{
		res = dlgLicense.DoModal();
		if (res==0 || res==IDCANCEL)
		{
			strcpy(errmsg, "Code=7");; // invalid license code
			return res;
		}
		// Generate LICENSE_FILE
		GetSystemDirectory(dir, sizeof(dir));
		FulfillFile(fname, dir, LICENSE_FILE);
		if ( (hFile = ::CreateFile (fname, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
					OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ))== INVALID_HANDLE_VALUE)
		{
			strcpy(errmsg, "Cannot create license file (You might not have proper previlege)."); return 0;
		}
		strcpy(buffer,"z83 ");
		if (!WriteFile(hFile, buffer, 4, &dw, NULL))
		{
			strcpy(errmsg, "Cannot write license file."); CloseHandle(hFile); return 0;
		}
		sprintf(buffer, "%d", res);
		encode(buffer, strlen(buffer));
		if (!WriteFile(hFile, buffer, strlen(buffer), &dw, NULL))
		{
			strcpy(errmsg, "Cannot write license file."); CloseHandle(hFile); return 0;
		}
		// Get file time of calc.exe
		FulfillFile(fname, dir, "calc.exe");
		if ( (hFile2 = ::CreateFile (fname, GENERIC_READ, FILE_SHARE_READ, NULL,
					OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ))!= INVALID_HANDLE_VALUE)
		{
			GetFileTime(hFile2, &ft, NULL, NULL);
			SetFileTime(hFile, &ft, &ft, &ft);
			CloseHandle(hFile2);
		}
		CloseHandle(hFile);

		if (checkLicFile()!=1)
			{
			strcpy(errmsg, "Code=-7"); // invalid license code
			return res;
			}
	}
	return 1;
}
