#include "flyPrivate.h"

#define WM__GET_HOSTNAME		WM_APP+500
#define WM__GOT_HOSTNAME		WM_APP+501

// The normal HANDLE_MSG macro in WindowsX.h does not work properly for dialog
// boxes because DlgProc return a BOOL instead of an LRESULT (like
// WndProcs). This chHANDLE_DLGMSG macro corrects the problem:

#define     SetDlgMsgResultBK(hwnd, msg, result) (( \
        (msg) == WM_CTLCOLORMSGBOX      || \
        (msg) == WM_CTLCOLOREDIT        || \
        (msg) == WM_CTLCOLORLISTBOX     || \
        (msg) == WM_CTLCOLORBTN         || \
        (msg) == WM_CTLCOLORDLG         || \
        (msg) == WM_CTLCOLORSCROLLBAR   || \
        (msg) == WM_CTLCOLORSTATIC      || \
        (msg) == WM_COMPAREITEM         || \
        (msg) == WM_VKEYTOITEM          || \
        (msg) == WM_CHARTOITEM          || \
        (msg) == WM_QUERYDRAGICON       || \
        (msg) == WM_INITDIALOG             \
    ) ? (BOOL)(result) : (SetWindowLongPtr((hwnd), DWLP_MSGRESULT, (LONG)(LPARAM)(LRESULT)(result)), TRUE))


#define chHANDLE_DLGMSG(hwnd, message, fn)                 \
   case (message): return (SetDlgMsgResultBK(hwnd, uMsg,     \
      HANDLE_##message((hwnd), (wParam), (lParam), (fn))))

#define HANDLE_WM__GET_HOSTNAME(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (char*)wParam), 0)
#define HANDLE_WM__GOT_HOSTNAME(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), lParam), 0)


int nHosts;
static string *host;
char *resolvedHostAddr = NULL;
string reqestedHost("");
char buffer[512];
extern WSADATA wsdat;

static HANDLE hAsync;
static char hostHolder[MAXGETHOSTSTRUCT];


int compare( const void *arg1, const void *arg2 )
{
   return _stricmp( * ( char** ) arg1, * ( char** ) arg2 );
}

int DidLastAppearMiddle(string *str, int len)
{
	// if yes, it returns the index where it appeared
	// if no, it returns -1

	if (len<=1) return -1;

	int i;

	for (i=0; i<len; i++)	if (str[len-1]==str[i]) return i;
	return -1;
}

BOOL OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	int i, maxlen(0);
	char **temp;

	WSAStartup(MAKEWORD(2,0), &wsdat);

	if (reqestedHost=="") // the file didn't contain the selected host name in the last index
	{
		MessageBox (hDlg, "No remote hostname is requested. (Select or add/remove)", "", MB_OK);
	}
	else
	{
		nHosts--;
		i = DidLastAppearMiddle(host, nHosts);
		SendDlgItemMessage(hDlg, IDC_HOST_LIST, LB_SETCURSEL, i, 0);
	}
	temp = new char*[nHosts];
	for (i=0; i<nHosts; i++) maxlen = max (maxlen, (int)host[i].length());
	for (i=0; i<nHosts; i++) 
	{
		temp[i] = new char[maxlen+2];
		strcpy(temp[i], host[i].c_str());
	}
	qsort((void*)temp, nHosts, sizeof(char*), compare);
	for (i=0; i<nHosts; i++) 
	{
		host[i] = temp[i];
		delete[] temp[i];
	}
	delete temp;
	for (i=0; i<nHosts; i++)
		SendDlgItemMessage(hDlg, IDC_HOST_LIST, LB_ADDSTRING, 0, (LPARAM)host[i].c_str());
	if (reqestedHost!="") 
		PostMessage (hDlg, WM__GET_HOSTNAME, (WPARAM)reqestedHost.c_str(), 0);
	return 1;
}


void OnCommand(HWND hDlg, int idc, HWND hwndCtl, UINT icode)
{
	LRESULT il;
	char buf[256];

	switch (idc)
	{
	case WM_DESTROY:
		EndDialog (hDlg, 0);
		break;
	case IDC_HOST_LIST:
		if (icode==LBN_DBLCLK)
		{
			EnableDlgItem (hDlg, IDC_CANCEL, 1);
			il = SendDlgItemMessage(hDlg, IDC_HOST_LIST, LB_GETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_HOST_LIST, LB_GETTEXT, il, (LPARAM)buffer);
			reqestedHost = buffer;
			PostMessage (hDlg, WM__GET_HOSTNAME, (WPARAM)reqestedHost.c_str(), 0);
		}
		break;
	case IDC_NEWHOST:
		if (icode==EN_UPDATE)
		{
			GetDlgItemText (hDlg, idc, buf, sizeof(buf));
			if (strlen(buf)>0)
				EnableDlgItem (hDlg, IDC_ADD_HOST, 1);
			else
				EnableDlgItem (hDlg, IDC_ADD_HOST, 0);
		}
		break;
	case IDC_ADD_HOST:
		GetDlgItemText (hDlg, IDC_NEWHOST, buf, sizeof(buf));
		SendDlgItemMessage(hDlg, IDC_HOST_LIST, LB_ADDSTRING, 0, (LPARAM)buf);
		SetDlgItemText (hDlg, IDC_NEWHOST, "");
		delete[] host;
		il=SendDlgItemMessage(hDlg, IDC_HOST_LIST, LB_GETCOUNT, 0, 0);
		nHosts = (int)il;
		host = new string [nHosts];
		break;
	case IDC_REMOVE_HOST:
		il=SendDlgItemMessage(hDlg, IDC_HOST_LIST, LB_GETCURSEL, 0, 0);
		SendDlgItemMessage(hDlg, IDC_HOST_LIST, LB_DELETESTRING, il, 0);
		break;
	}
}

void OnDestroy(HWND hDlg)
{
	int i;
	LRESULT il;
	il=SendDlgItemMessage(hDlg, IDC_HOST_LIST, LB_GETCOUNT, 0, 0);

	for (i=0; i<il; i++)
		SendDlgItemMessage(hDlg, IDC_HOST_LIST, LB_GETTEXT, i, (LPARAM)host[i].c_str());
}

BOOL OnGetHost(HWND hDlg, char* hostname)
{
	struct in_addr hostAddress;
	int i;
	char buf[256];
	EnableDlgItem (hDlg, IDC_REMOVE_HOST, 0);
	EnableDlgItem (hDlg, IDC_NEWHOST, 0);
	EnableDlgItem (hDlg, IDC_ADD_HOST, 0);
	HOSTENT *hont = gethostbyname(hostname);
	if (hont==NULL) 
	{
		i=WSAGetLastError();
		sprintf(buf, "%s cannot be found in network--Choose different PC", hostname);
		MessageBox (hDlg, buf, "PickRemotePC", MB_OK);
	}
	EnableDlgItem (hDlg, IDC_REMOVE_HOST, 1);
	EnableDlgItem (hDlg, IDC_NEWHOST, 1);
	EnableDlgItem (hDlg, IDC_ADD_HOST, 1);
	if (hont!=NULL)
	{
		wsprintf (buf, "%s found. Do you want to use this PC for remote host?", hont->h_name);
		if (MessageBox (hDlg, buf, "", MB_YESNO | MB_ICONQUESTION)==IDYES)
		{
			hostAddress.S_un.S_addr = *(u_long*)(hont->h_addr_list[0]);
			resolvedHostAddr = inet_ntoa(hostAddress);
			EndDialog (hDlg, TRUE);
		}
	}
	return 1;
}


BOOL CALLBACK RemoteHostsDlgProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch( uMsg )
    {
	chHANDLE_DLGMSG (hDlg, WM_INITDIALOG, OnInitDialog);
	chHANDLE_DLGMSG (hDlg, WM_DESTROY, OnDestroy);
	chHANDLE_DLGMSG (hDlg, WM_COMMAND, OnCommand);
	chHANDLE_DLGMSG (hDlg, WM__GET_HOSTNAME, OnGetHost);
	default:
		return 0;
	}
	return 1;
}

FLY_EXP int PickRemotePC (char* ipaddr, char *hostname, const char* filename)
{                      //   out             out             in
	// This returns 1 for success of hostname resolution, 
	// returns 0 if requested host name is not resolved (still hostname is the requested name),
	// or no name has been requested.

	HANDLE hFile;
	int i, id, res;
	DWORD dw;
	char buffer[4096];

	if ( (hFile = CreateFile (filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, 
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))==INVALID_HANDLE_VALUE )
	{
		nHosts = 0;
	}
	else
	{
		SetFilePointer (hFile, 0, NULL, FILE_BEGIN);
		ReadFile (hFile, buffer, sizeof(buffer), &dw, NULL);
		buffer[dw]=0;
		CloseHandle (hFile);
		nHosts = countDeliminators (buffer, " \t\n\r");
	}
	host = new string[nHosts];
	str2array (host, nHosts, buffer, " \t\n\r");
	id = DidLastAppearMiddle(host, nHosts);
	if (id>=0)
		reqestedHost = host[id];

	if (!DialogBox (hDllModule, MAKEINTRESOURCE(IDD_REMOTE_HOSTS), NULL, RemoteHostsDlgProc)) return 0;
	
	hFile = CreateFile (filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, 
								OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile==INVALID_HANDLE_VALUE)
	{
		sprintf (buffer, "Error writing %s", filename);
		MessageBox (NULL, buffer, "", MB_OK);
		return (resolvedHostAddr!=NULL);
	}

	for (i=0; i<nHosts; i++)
	{
		sprintf (buffer, "%s\r\n", host[i].c_str());
		WriteFile (hFile, buffer, (DWORD)strlen(buffer), &dw, NULL);
	}
	if (resolvedHostAddr!=NULL)
		res=1;
	else
		res=0;
	if (reqestedHost!="")
		strcpy(hostname, reqestedHost.c_str());
	sprintf (buffer, "%s\r\n", hostname);
	WriteFile (hFile, buffer, (DWORD)strlen(buffer), &dw, NULL);
	SetEndOfFile (hFile);
	CloseHandle (hFile);
	if (ipaddr!=NULL) 
		strcpy(ipaddr, resolvedHostAddr);
	delete[] host;
	return res;
}

int isIP(const char *str)
{
	int i, ipadd[4];
	int nItems = str2intarray(ipadd, 4, str, ".");
	if (nItems==4)
		for (i=0; i<4; i++) { if (ipadd[i]<0 || ipadd[i]>255) return 0; }
	else
		return 0;
	return 1;
}

FLY_EXP char* flyErrMsg (int errcode, char* errstr)
{
	switch (errcode)
	{
	case FER_OK:
		strcpy(errstr, "No error.");
		break;
	case FER_TCPIP: // same as SOCKET_ERROR
		strcpy(errstr, "send() error");
		break;
	case FER_NULL_POINTER:
		strcpy(errstr, "pointer is not ready.");
		break;
	case FER_CLOSESOCKET:
		strcpy(errstr, "closesocket() error");
		break;
	case FER_CANNOT_CREATE_FLY_HANDLE:
		strcpy(errstr, "A valid window handle for FLY processing cannot be created.");
		break;
	case FER_NOT_CONNECTED:
		strcpy(errstr, "No valid Fly connection available.");
		break;
	case FER_WSACLEAN:
		strcpy(errstr, "WSACleanup() error");
		break;
	case FER_FILE_OPEN:
		strcpy(errstr, "File creation error");
		break;
	case FER_FILE_CLOSE:
		strcpy(errstr, "File closing error");
		break;
	case FER_INVALID_PARAMETER:
		strcpy(errstr, "Not a valid parameter");
		break;
	case FER_FLY_HANDLE_UNAVAILABLE:
		strcpy(errstr, "Fly window handle not available");
		break;
	case FER_HOST_NAME_CANNOT_RESOLVE:
		strcpy(errstr, "Remote Host name cannto be resolved.");
		break;
	case FER_TOO_LARGE_MESSAGE_SIZE:
		strcpy(errstr, "Too large message block to send");
		break;
	case FER_OUTBUFFER_OVERFLOW:
		strcpy(errstr, "Out-buffer overflow. Slow down the transmission.");
		break;
	case FER_INVALID_IP_ADDRESS:
		strcpy(errstr, "IP address not valid.");
		break;
	case FER_TIME_OUT:
		strcpy(errstr, "Time out.");
		break;
	case FER_FLY_HANDLE_ALREADY_EXISTS:
		strcpy(errstr, "Fly handle already exists.");
		break;
	case FER_BLOCKINGMODE_REQUIRED:
		strcpy(errstr, "Blocking mode is required for the specified function.");
		break;

	default:
		strcpy(errstr, "Unknown error");
		break;
	}
	return errstr;
}

