#include <windows.h>
#include <commctrl.h>  
#include <stdio.h>
#include "flyplus.h"
#include "resource.h"
#include "audfret.h"
#include "audstr.h"
#include "..\protocol.h"


#define BUTT1		923
#define BUTT2		924
#define BUTT3		925
#define BUTT4		926
#define BUTT5		927
#define ID_HMSG		928
#define ID_FEEDBACK	929
#define ID_HBEGIN		1928
#define ID_HMSG2		948
#define ID_COUNTER		9248
#define ID_CAP			1200
#define IDC_PLAY			2001
#define IDC_DONE			2002

#define WM__STOPPED		WM_APP+229


#define FDBCKTIMER	1000
#define BEGINDELAY  1001
#define MAX_INTERVALS  5
#define STOPPED_BY_USER  1


