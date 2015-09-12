#include <windows.h>
#include <stdio.h>
#include "flyplus.h"
#include "flyPrivate.h"
#include <process.h>

/* flyplus version 1.0 (continuing from flyd version 3.1)

structure of sending message 

(2 bytes unsigned) (2 bytes unsigned) (n bytes)   <===    0 <= n <= 0xffff
(length)			(comm code)       (flying body)
*/

#pragma data_seg ("shared")

WSADATA wsdat;
int flyRecording, unsentSize, blockingMode;
int connected, DuringOpenwaitTime;
SOCKET activeSocket;
HWND hMom, hBase;
unsigned int applProcID, flyProcID, connectThreadID;
uintptr_t hThreadFly;
char unsentBuffer[FL_BUFFERSIZE+256];
char versionStr[32], RemoteAddress[64], RemoteHostname[128];
HMODULE hDllModule;
char* outBuffer;
int remRecvItems;
UINT_PTR recv_TimerID, send_TimerID, open_TimerID;
FILE *fp;

u_short targetPort(RESERVED_PORT); // when a target is specificly requested, it is taken care of in initflySpecialPort

#pragma data_seg ()

#ifndef LINK_STATIC_FLYPLUS
BOOL APIENTRY DllMain( HANDLE hModule, DWORD u, LPVOID rese)
{
	hDllModule = (HMODULE)hModule;
	strcpy(wsdat.szDescription,"");
	return TRUE;     
}
#endif

FLY_EXP char* getFlyVersion ()
{
	char modulename[MAX_PATH];
	GetModuleFileName(hDllModule, modulename, sizeof(modulename));
	getVersionStringAndUpdateTitle(NULL, modulename, versionStr);
	return versionStr;
}

FLY_EXP int changeHWnd (HWND newhwnd)
{
	if (newhwnd==NULL)		return FER_NULL_POINTER;
	hMom = newhwnd;
	return FER_OK;
}

unsigned int WINAPI Thread4Fly (PVOID isClient)
{
	WNDCLASSEX wndclass ;
	MSG        msg ;

	HINSTANCE hInstFly = GetModuleHandle(NULL);

	if (GetClassInfoEx (hInstFly, "flybase", &wndclass)==FALSE)
	{
	    wndclass.cbSize        = sizeof (wndclass) ;
		wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	    wndclass.lpfnWndProc   = Base ;
		wndclass.cbClsExtra    = 0;
	    wndclass.cbWndExtra    = 0;
		wndclass.hInstance     = hInstFly ;
	    wndclass.hIcon         = LoadIcon (NULL, "icon") ;
		wndclass.hCursor       = NULL ;
		wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
		wndclass.lpszMenuName  = NULL ;
	    wndclass.lpszClassName = "flybase" ;
		wndclass.hIconSm       = LoadIcon (NULL, "icon") ;
		RegisterClassEx (&wndclass) ;
	}
	

	hBase = CreateWindow ("flybase", "", WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstFly, &isClient) ;

     while (GetMessage (&msg, NULL, 0, 0))
	 {
		 TranslateMessage (&msg) ;
		 DispatchMessage (&msg) ;
	 }
	 if( WSACleanup() ==SOCKET_ERROR)
		 MessageBox (NULL, "WSACleanup() failed", "", MB_OK);
	 return 0;
}

VOID CALLBACK TimerProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// The message should always be WM_TIMER.
	int j= uMsg == WM_TIMER; 
	KillTimer (hwnd, wParam);
	if (wParam==recv_TimerID)
		PostThreadMessage (applProcID, WM__RECV_WAIT_TIME_OVER, 0, 0);
	else if (wParam==open_TimerID)
		DuringOpenwaitTime = 0;
	else if (wParam==send_TimerID)
		PostThreadMessage (applProcID, WM__SEND_WAIT_TIME_OVER, 0, 0);
}

int _initfly (HWND hParent, const char* remotehostnameOrIP, int waitTime, bool isClient)
{  	// Pass a negative number for waitTime for non-blocking mode.
	int res;
	HOSTENT *hont;
	struct in_addr hostAddress;

	if (hThreadFly!=0L)		return FER_FLY_HANDLE_ALREADY_EXISTS;

	if (isIP(remotehostnameOrIP))
	{
		strcpy(RemoteAddress, remotehostnameOrIP);
		strcpy(RemoteHostname, "");
	}
	else
	{
		hont = gethostbyname(remotehostnameOrIP);
		if (hont==NULL) return FER_HOST_NAME_CANNOT_RESOLVE;
		hostAddress.S_un.S_addr = *(u_long*)(hont->h_addr_list[0]);
		strcpy(RemoteAddress, inet_ntoa(hostAddress));
		strcpy(RemoteHostname, remotehostnameOrIP);
	}

	if (stricmp(wsdat.szSystemStatus, "Running")) //If WSAStartup was called in PickRemotePC
		res = WSAStartup(MAKEWORD(2,0), &wsdat);
	applProcID = GetCurrentThreadId ();

	if ( ( hThreadFly = _beginthreadex(NULL, 0, Thread4Fly, (void*)isClient, 0, &flyProcID) )== 0 )
		return FER_CANNOT_CREATE_FLY_HANDLE;

	if (waitTime<0 && hParent !=NULL)
	{
		blockingMode = 0;
		hMom = hParent;
		return FER_OK;
	}
	else if (hParent==NULL && waitTime>=0)
	{
		blockingMode = 1;
		DuringOpenwaitTime=1;
		open_TimerID = SetTimer (NULL, 0, waitTime, (TIMERPROC)TimerProc);
		while (DuringOpenwaitTime)
		{
			if (connected)
				return FER_OK;
			Sleep(50);
		}
		return FER_TIME_OUT;
	}
	else 
		return FER_INVALID_PARAMETER;
}

FLY_EXP int initfly (HWND hParent, const char* remotehostnameOrIP, int waitTime, bool isClient)
{
	return _initfly(hParent, remotehostnameOrIP, waitTime, isClient);
}

FLY_EXP int initflySpecialPort (HWND hParent, const char* remotehostnameOrIP, unsigned short port, int waitTime, bool isClient)
{
	targetPort = port;
	return _initfly(hParent, remotehostnameOrIP, waitTime, isClient);
}

FLY_EXP int flySendText (WORD command, const char* charbuffer)
{
	return sendfly (command, charbuffer, 0);
}

FLY_EXP int flySendBin (WORD command, void* pbody, size_t count)
{
	return sendflyn (command, pbody, count, 0);
}

FLY_EXP int sendfly (WORD command, const char* charbuffer, WORD identifier)
{
	// Terminating zero is included in the length of fly message.
	// For example, for a single character, 2-byte fly body is sent.
	return sendflyn (command, (void*)charbuffer, strlen(charbuffer)+1, identifier);
}

int sendblock(WORD command, size_t count, WORD identifier)
{ // Attempt to send the outgoing buffer; if WSAEWOULDBLOCK, 
  // increase unsentSize by the block size not sent and
  // move the outgoing block to unsentBuffer, so that it could be
  // re-tried when FD_WRITE is on.
  // Whenever accumUnsentSize is positive, the send function is
  // not called in this function.

	MSG msg;
	int sentBytes;
	char header[4];
	if (unsentSize==0)
	{
		memcpy((void*)header, (void*)&count, sizeof(WORD));
		memcpy((void*)&header[sizeof(WORD)], (void*)&command, sizeof(WORD));
		//Sending 4-byte header (count followed by command) first
		sentBytes = send(activeSocket, header, 4, 0);
		if (sentBytes!=4)
		{
			// header was not sent... keep the header and outBuffer in the unsentBuffer and try when FD_WRITE is issued.
			if (sentBytes==-1)						sentBytes++;
			memcpy(unsentBuffer+unsentSize, &header[sentBytes], 4-sentBytes);
			unsentSize += 4;
			//outBuffer....
			memcpy(unsentBuffer+unsentSize, outBuffer, count);
			unsentSize += (int)count-sentBytes;
			send_TimerID = SetTimer (NULL, 0, SEND_WAIT_TIME, (TIMERPROC)TimerProc);
		}
		else // header sent correctly
		{
			sentBytes = send (activeSocket, outBuffer, (int)count, 0);
			if (sentBytes == (int)count) // success of transmission
			{
				SendMessage (hMom, WM_FL_SENT_OK, MAKELONG(command,identifier), (LPARAM)outBuffer);
				return (int)count+2; 
			}
			else 
			{
				if (sentBytes==-1)						sentBytes++;
				memcpy(unsentBuffer+unsentSize, &outBuffer[sentBytes], count-sentBytes);
				unsentSize += (int)count-sentBytes;
				send_TimerID = SetTimer (NULL, 0, SEND_WAIT_TIME, (TIMERPROC)TimerProc);
			}
		}
		while (GetMessage(&msg, NULL, 0, 0))
		{
			if (msg.message==WM_FL_ASYNC_SENT)
			{
				sentBytes = LOWORD(msg.wParam);
				return (int)count; 
			}
			else if (msg.message==WM_TIMER)
				DispatchMessage (&msg);
			else if (msg.message==WM__SEND_WAIT_TIME_OVER)
				break;
			else if (msg.message==WM_FL_NETWORK_ERR)
			{
				// if WSAEWOULDBLOCK, do not return.
				if (WSAGetLastError()!=WSAEWOULDBLOCK)
					return FER_TCPIP;
			}
		}
		return FER_TIME_OUT;
	}
	else
		return FER_OUTBUFFER_OVERFLOW;
}

FLY_EXP int sendflyn (WORD command, void* pbody, size_t count, WORD identifier)
{
	// count: only message length (not including the 4 bytes for command and n of bytes)

	static int res=1;

	if (activeSocket==0)
		return FER_NOT_CONNECTED;

	if (count>FL_BUFFERSIZE)
		return FER_TOO_LARGE_MESSAGE_SIZE;

	if (hBase==NULL)
		return FER_FLY_HANDLE_UNAVAILABLE; // if valid handle to window does not exist.
	else
	{
		outBuffer = (char*)pbody;
		res =  sendblock (command, count, identifier);
		return res;
	}
}

FLY_EXP int flyRecv (WORD *command, void *pbody, unsigned short *count, int waitTime)
{
	MSG  msg ;
	if (!blockingMode)
		return FER_BLOCKINGMODE_REQUIRED;

	recv_TimerID = SetTimer (NULL, 0, waitTime, (TIMERPROC)TimerProc);

	// BK is proud of this part of the code.
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message==WM_FL_ARRIVED)
		{
			*command = LOWORD(msg.wParam);
			*count = HIWORD(msg.wParam);
			memcpy(pbody, (void*)msg.lParam, *count+4);
			remRecvItems--;
			return FER_OK;
		}
		else if (msg.message==WM_TIMER)
			DispatchMessage (&msg);
		else if (msg.message==WM__RECV_WAIT_TIME_OVER)
			break;
	}
	return FER_TIME_OUT;
}

void NotifyApplication(char *buffer)
{
	WORD msgSize, command;

	msgSize = *(WORD*)&buffer[0];
	command = *(WORD*)&buffer[2];

	if (blockingMode)
		PostThreadMessage (applProcID, WM_FL_ARRIVED, MAKELONG(command,msgSize), (LPARAM)(buffer+4));
	else
		SendMessage (hMom, WM_FL_ARRIVED, MAKELONG(command,msgSize), (LPARAM)(buffer+4));
	remRecvItems++;
}


unsigned int WINAPI ConnectingThread (PVOID soc)
{
	MSG  msg ;
	struct sockaddr_in *sa;
	char buf[32];

	msg.message=WM__TRY_CONNECT;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message==WM__TRY_CONNECT)
		{
			sa = (sockaddr_in *)msg.wParam;
			if (connect ((SOCKET)soc, (SOCKADDR *)sa, sizeof(*sa))==-1)
			{
				int i=WSAGetLastError();
				if (i!=WSAECONNREFUSED && i!=WSAEWOULDBLOCK)
				{
					sprintf(buf, "code=%d", i);
					MessageBox (NULL, buf, "Error in connect", MB_OK);
				}
			}
		}
	}
	return 0;
}

#define notifier szbuffer

int CreateInitSockClient(HWND hwnd, SOCKET& sock, sockaddr_in sa, char *buf)
{
	int i;
	char lineErr[256], buf2[128];
	CHECKERR(sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP), FL_ERR_SOCKET);
	CHECKERR(i = WSAAsyncSelect (sock, hwnd, WM_FLY_TASKS, FD_CONNECT | FD_WRITE | FD_READ | FD_CLOSE ), FL_ERR_WSAAYNC);
	i=connect (sock, (SOCKADDR *)&sa, sizeof(sa));
	if (i==-1)
	{
		if (WSAGetLastError()!=WSAEWOULDBLOCK)
		{
			sprintf(buf, "code=%d", i);
			MessageBox (NULL, buf, "Error in connect", MB_OK);
		}
	}
	_beginthreadex (NULL, 0, ConnectingThread, (void*)sock, 0, &connectThreadID);
	return i;
}

LRESULT CALLBACK Base (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wEvent, wError;
	char buf2[512];

	static char szbuffer[FL_BUFFERSIZE], receiver[FL_BUFFERSIZE];
	static char lineErr[256], errMsg[256];
	static int j, i=0;
	// sock: socket variable generated when socket() was called
	// acceptedSock: derived socket variable by the accept() call
	// activeSocket: the connected socket currently used for communication
	static SOCKET sock, acceptedSock;
	static struct sockaddr_in sa;
	static int connectStatus; // -1: not connected, 0: waiting as a server, 1: connected as a server, 2: connected as a client, 
	static int msgSz, carriedLength ;

	bool *pisClient;
	CREATESTRUCT * crstruc;

    switch( uMsg )
    {
	case WM_CREATE:
		crstruc = (CREATESTRUCT *)lParam;
		pisClient = (bool*)(crstruc->lpCreateParams);

		connectStatus = -1;
		sa.sin_family = AF_INET;
		sa.sin_port = htons(targetPort);

		if (!*pisClient)
		{
			CHECKERR(sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP), FL_ERR_SOCKET);
			sa.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
			while ((j=bind (sock, (SOCKADDR *)&sa, sizeof(sa)))==-1)
			{
				if ((wError=WSAGetLastError())==WSAEADDRINUSE)
				{
					CHECKERR(closesocket(sock/*flush & try again*/), FL_ERR_CLOSESOCKET);
					CHECKERR(sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP/*flush & try again*/), FL_ERR_SOCKET);
				}
				else
				{
					sprintf (buf2, "code=%d", wError);
					SendMessage (hMom, WM_FL_NETWORK_ERR, (WPARAM)buf2, (LPARAM)"binding error"); 
				}
				Sleep(500);
			}
			CHECKERR(WSAAsyncSelect (sock, hwnd, WM_FLY_TASKS, FD_ACCEPT | FD_WRITE | FD_READ | FD_CLOSE ), FL_ERR_WSAAYNC);
			CHECKERR(listen (sock, MAXPENDING), FL_ERR_LISTEN);
		}
		else
		{
			// Attempting to establish a client application
			sa.sin_addr.S_un.S_addr = inet_addr(RemoteAddress);
			CreateInitSockClient(hwnd, sock, sa, buf2);
		}
		connectStatus = 0;
		return 0;

	 case WM__READ_LOOP:
		return 0;

	 case WM_FLY_TASKS:
		 wEvent = WSAGETSELECTEVENT (lParam) ; 
		 wError = WSAGETSELECTERROR (lParam) ; 
		 if (wError!=0 && wError!=WSAEWOULDBLOCK)
		 {
			 if (!( (wEvent==FD_CLOSE && wError==WSAECONNABORTED) || (wEvent==FD_CONNECT && wError==WSAECONNREFUSED) ) )
			 {
				 sprintf (buf2, "Error occurred with Async message sent. event=%d, error code=%d", wEvent, wError);
				 SendMessage (hMom, WM_FL_NETWORK_ERR, (WPARAM)buf2, (LPARAM)"");
			 }
		 }
		 switch (wEvent)
		 {
		 case FD_CONNECT:
			 if (wError==0)
			 {
				 connectStatus = 2;			 
				 connected = 1 ;
				 activeSocket = sock;
				 SendMessage (hMom, WM_FL_CONNECTED, (WPARAM)RemoteHostname, (LPARAM)RemoteAddress); 
				 PostThreadMessage(connectThreadID, WM_QUIT, 0, 0);
			 }
			 else
				 PostThreadMessage(connectThreadID, WM__TRY_CONNECT, (WPARAM)&sa, 0);
			 break;

		 case FD_ACCEPT:
			 i = sizeof(sa);
			 CHECKERR (acceptedSock = accept(sock, (SOCKADDR *)&sa, &i), FL_ERR_ACCEPT);
			 if (strcmp(inet_ntoa (sa.sin_addr), RemoteAddress)!=0)
			 {
				 sprintf(buf2, "Requested IP: %s, accepted IP: %s", RemoteAddress, inet_ntoa(sa.sin_addr));
				 SendMessage (hMom, WM_FL_NETWORK_ERR, (WPARAM)buf2, (LPARAM)"WARNING!");
			 }
			 connectStatus = 1;
			 activeSocket = acceptedSock;
			 PostMessage (hMom, WM_FL_CONNECTED, (WPARAM)RemoteHostname, (LPARAM)RemoteAddress); 
			 connected = 1 ;
			 break;

		 case FD_WRITE:
			 if (unsentSize>0)
			 {
				 int sentBytes = send (activeSocket, unsentBuffer, unsentSize, 0);
				 if (sentBytes == unsentSize)
				 {
					 unsentSize -= sentBytes;
					 SendMessage (hMom, WM_FL_ASYNC_SENT, (WPARAM)sentBytes, (LPARAM)unsentBuffer);
					 i=PostThreadMessage (applProcID, WM_FL_ASYNC_SENT, (WPARAM)sentBytes, (LPARAM)0);
				 }
				 else if ( sentBytes*(sentBytes-unsentSize) <= 0)
				 { // in fact, sentBytes cannot be zero, because FD_WRITE was posted.
					 memcpy(unsentBuffer, &outBuffer[sentBytes], unsentSize);
					 unsentSize = unsentSize-sentBytes;
				 }
				 else // sentBytes must be SOCKET_ERROR
				 {
					 i=PostThreadMessage (applProcID, WM_FL_NETWORK_ERR, (WPARAM)WSAGetLastError(), (LPARAM)0);
					 break;
				 }
			 }
			 break;

 		 case FD_READ:
			int bytesRcvd;
			bytesRcvd = recv(wParam, receiver, FL_BUFFERSIZE, 0);
			if (bytesRcvd>0)
			{
				int ind(0);
				int leftBytes = bytesRcvd;
				if (carriedLength==0) // if there's no carry-over
					msgSz = *(WORD*)receiver + 4;
				// if there was a carry-over, msgSz should be carried over, too.

				while (msgSz <= leftBytes + carriedLength)
				{
					if (carriedLength>0)
					{
						int copiedChunk = msgSz - carriedLength ;
						memcpy ((void*)&notifier[carriedLength], &receiver[ind], copiedChunk);
						NotifyApplication(notifier);
						ind += copiedChunk; leftBytes -= copiedChunk;
						carriedLength=0;
					}
					else
					{
						NotifyApplication(&receiver[ind]);
						ind += msgSz; leftBytes -= msgSz;
					}
					
					msgSz = *(WORD*)&receiver[ind] + 4;
				}

				if (leftBytes>0) // need to carry-over
				{
					memcpy ((void*)&notifier[carriedLength], &receiver[ind], leftBytes);
					carriedLength += leftBytes;
				}

				// Need to delay FD_READ if the incoming data is not yet received, otherwise, the window message is flooded, so that 
				// a malfunction occurs with GetMessage in flyRecv (the content of msg.lParam changed)
				while (blockingMode && (remRecvItems>0)  )
					Sleep(50);
			}
			else
			{
				sprintf(buf2, "Error in recv (WM__READ_LOOP), code=%d", WSAGetLastError());
				SendMessage (hMom, WM_FL_NETWORK_ERR, (WPARAM)buf2, (LPARAM)"11232"); 
			}
			break;

		case FD_CLOSE:
			if (connected==0) // connected was already set to zero, i.e., from closefly() call... 
			{
				if ((i=closesocket (sock))==SOCKET_ERROR) return FER_CLOSESOCKET;
			}
			else 
			{
				if (connectStatus==2) // if this is a client, no need to close socket but re-try connect
					CreateInitSockClient(hwnd, sock, sa, buf2);
//				else if (connectStatus==1) // if this is a server
					// do nothing
				else if (connectStatus==0)
					MessageBox (NULL, "Unexpected path in FD_CLOSE", "", NULL);
			}
			activeSocket = 0 ;
			connected = 0;
			SendMessage (hMom, WM_FL_CLOSE, (WPARAM)RemoteHostname, (LPARAM)(LPCSTR)RemoteAddress);
			break;
		 }
		 return 0;
	}
	return DefWindowProc (hwnd, uMsg, wParam, lParam) ;
}

FLY_EXP int closefly ()
{
	int j, res;
	struct sockaddr_in	saddr;

	j = sizeof(SOCKADDR);
	getsockname (activeSocket, (SOCKADDR *)&saddr, &j); // no need to leave this (just in case in the future)

	connected = 0;
	if (SendMessage (hBase, WM_FLY_TASKS, 0, FD_CLOSE)==FER_CLOSESOCKET)
		res = FER_CLOSESOCKET;
	else 
		res = FER_OK;
	//This terminates hBase window proc (this must be called for the cleaning call WSACleanup).
	j=PostThreadMessage(flyProcID, WM_QUIT, 0, 0);
	hBase = NULL;
	return res;
}

