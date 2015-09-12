/* flyplus.h version 1.0

  Developer : Bomjun Kwon
  May 5, 2007
*/

#ifndef FLYPLUS
#define FLYPLUS

//If this library is linked statically define LINK_STATIC_FLYPLUS in the application project.
#ifndef LINK_STATIC_FLYPLUS
#define FLY_EXP __declspec (dllexport)
#else 
#define FLY_EXP
#endif


#define FL_FILE			(WORD)0XFAFF
#define FL_FILEINFO		(WORD)0XFAFE
#define FL_RUNDLL		(WORD)0XFAFD

// Function declarations 
FLY_EXP int		changeHWnd (HWND newhwnd);						
FLY_EXP int		closefly ();								
FLY_EXP char*	flyErrMsg (int errcode, char*errstr);				
FLY_EXP int		flySendBin (WORD command, void* pbody, size_t count);
FLY_EXP int		flySendText (WORD command, const char* charbuffer);
FLY_EXP char*	getFlyVersion ();
FLY_EXP int		initflySpecialPort (HWND hParent, const char* remotehostnameOrIP, unsigned short port, int waitTime, bool isClient);
FLY_EXP int		initfly	(HWND hParent, const char* remotehostnameOrIP, int waitTime, bool isClient);		
FLY_EXP int		PickRemotePC (char* ipaddr, char *hostname, const char* filename);
FLY_EXP int		sendfly (WORD command, const char* charbuffer, WORD identifier);		
FLY_EXP int		sendflyn (WORD command, void* pbody, size_t count, WORD identifier);
FLY_EXP int		flyRecv (WORD *command, void* pbody, unsigned short *count, int waitPeriod);
FLY_EXP int		setSendTimeout (int waitPeriod);

// FLY error codes
#define FER_OK							0
#define FER_TCPIP						-1
#define FER_NULL_POINTER				-2
#define FER_CLOSESOCKET					-3
#define FER_CANNOT_CREATE_FLY_HANDLE	-5
#define FER_NOT_CONNECTED				-6
#define FER_WSACLEAN					-7
#define FER_FILE_OPEN					-8
#define FER_FILE_CLOSE					-9
#define FER_INVALID_PARAMETER			-10
#define FER_FLY_HANDLE_UNAVAILABLE		-11
#define FER_HOST_NAME_CANNOT_RESOLVE	-12
#define FER_TOO_LARGE_MESSAGE_SIZE		-13
#define FER_OUTBUFFER_OVERFLOW			-14
#define FER_CONNECTION_ABORTED			-15
#define FER_TIME_OUT					-17
#define FER_FLY_HANDLE_ALREADY_EXISTS	-18
#define FER_INVALID_IP_ADDRESS			-19
#define FER_BLOCKINGMODE_REQUIRED		-20

// FLY error codes notified with WM_FL_NETWORK_ERR
#define 	FL_ERR_SOCKET			(WORD)1
#define 	FL_ERR_WSAAYNC			(WORD)2
#define 	FL_ERR_BIND				(WORD)3
#define 	FL_ERR_LISTEN			(WORD)4
#define 	FL_ERR_ACCEPT			(WORD)5
#define 	FL_ERR_CONNECT			(WORD)6
#define 	FL_ERR_WSACLEAN			(WORD)7

/* Reserved fly command. */
#define 	FL_RESERVED				(WORD)0XFFFF

/* Messages to send to the application window 
!!! DO NOT use a user-defined message in this range in your application !!!!!
*/
#define	WM_FL_CONNECTED		(WM_APP+4601)
#define	WM_FL_CLOSE			(WM_APP+4602)
#define	WM_FL_ARRIVED		(WM_APP+4603)
#define	WM_FL_SENT_OK		(WM_APP+4604)
#define	WM_FL_ASYNC_SENT	(WM_APP+4605)
#define	WM_FL_NETWORK_ERR	(WM_APP+4606)
#define	WM_FL_ERR_UNWANTED	(WM_APP+4607)

#endif // FLYPLUS