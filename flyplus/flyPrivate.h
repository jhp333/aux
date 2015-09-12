#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include "resource.h"
#include "flyplus.h"
#include "audfret.h"
#include "audstr.h"

#define WM_FLY_TASKS				WM_APP+134
#define WM__READ_LOOP				WM_APP+135
#define WM__RECV_WAIT_TIME_OVER		WM_APP+136
#define WM__SEND_WAIT_TIME_OVER		WM_APP+137
#define WM__TRY_CONNECT				WM_APP+138

#define 	RESERVED_PORT			1024
// FOR "SPECIAL" CONNECTION, ANY PORT NUMBER IN 1025 AND 65535 IS OK 

#define 	FL_BUFFERSIZE			65532
#define 	SEND_WAIT_TIME			5000
// an argument for listen(); originating from Berkely Socket.
#define MAXPENDING		5	
#define MFORM FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS
#define LANGID MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)

// Check error Macro, send window message, WM_FL_NETWORK_ERR, to the main appl, if error occurrs.
#define CHECKERR(x, notycode) \
{	SOCKET return_value=(x); int errcody=WSAGetLastError();\
	if ( ( errcody > 0 ) && ( errcody != WSAEWOULDBLOCK) ) { \
		sprintf (buf2, "code=%d", errcody);	sprintf(lineErr, "Error on: "#x);\
		SendMessage (hMom, WM_FL_NETWORK_ERR, (WPARAM)buf2, (LPARAM)lineErr); \
		return FALSE;}}

#define RECORDFLY(x)  {  if (flyRecording) fprintf (fp, x); }
#define RECORDFLY2(x,y)  {  if (flyRecording) fprintf (fp, x, y); }
#define RECORDFLY3(x,y,z)  {  if (flyRecording) fprintf (fp, x, y, z); }
#define RECORDFLY4(x,y,z,a)  {  if (flyRecording) fprintf (fp, x, y, z, a); }
#define RECORDFLY5(x,y,z,a,b)  {  if (flyRecording) fprintf (fp, x, y, z, a, b); }



extern HWND hBase;
extern FILE *fp;
extern int flyRecording;
extern HMODULE hDllModule;
extern char *selectedHost;


LRESULT CALLBACK Base (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int isIP(const char *str);