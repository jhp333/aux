#include "aceplayer.h"
#include "CStream.h"
#include "CStreamCallback.h"

int inProgress(0);

void OnStreamEnd ()
{
	if (inProgress)
	{
		inProgress = 0;
	}
}

void FormatErrStr(int errcode, char *errmsg)
{
	switch(errcode)
	{
	case NoError:
		strcpy(errmsg, "NoError");
		break;
	case InternalError:
		strcpy(errmsg, "InternalError");
		break;
	case NullPointerError:
		strcpy(errmsg, "NullPointerError");
		break;
	case FileAccessError:
		strcpy(errmsg, "FileAccessError");
		break;
	case OutOfRangeError:
		strcpy(errmsg, "OutOfRangeError");
		break;
	case NotInitialisedError:
		strcpy(errmsg, "NotInitialisedError");
		break;
	case BadFormatError:
		strcpy(errmsg, "BadFormatError");
		break;
	case ConfigurationFileError:
		strcpy(errmsg, "ConfigurationFileError");
		break;
	case FPGAFileError:
		strcpy(errmsg, "FPGAFileError");
		break;
	case SPrintFileError:
		strcpy(errmsg, "SPrintFileError");
		break;
	case TaskInProgressError:
		strcpy(errmsg, "TaskInProgressError");
		break;
	case InvalidArgumentError:
		strcpy(errmsg, "InvalidArgumentError");
		break;
	case NotImplementedError:
		strcpy(errmsg, "NotImplementedError");
		break;
	case BadConfigurationError:
		strcpy(errmsg, "BadConfigurationError");
		break;
	default:
		strcpy(errmsg, "Unknown error");
		break;
	}
}

int InitNICStream (HWND hDlg, const char *nicpath, char *errstr)
{
	int i, res;
	HMODULE hLibNicstream;
	char libName[256], errMsg[256], fname[256];
	LoadString (hInst, IDS_NICSTREAM_DLL, fname, sizeof(fname));
	FulfillFile(libName, nicpath, fname);
	if ((hLibNicstream = LoadLibrary(libName))=NULL) {
			sprintf(errstr, "Required DLL not found in %s. Program will abort.", nicpath);
			return 0;		}
	FreeLibrary(hLibNicstream);
	if ( (res=RegisterStreamStop(OnStreamEnd)) != NoError)
	{
		FormatErrStr(res, errMsg);
		sprintf(errstr, "RegisterStreamStop failed, code=%d (%s)", res, errMsg);
		MessageBox(hDlg, errstr, "", MB_OK);
		return 0 ;
	}
	if ( (i=Initialise(nicpath))!= NoError)
	{
		FormatErrStr(i, errMsg);
		sprintf(errstr, "Initialise failed, code=%s", errMsg);
		return 0;
	}
	return 1;
}


int NICStreamIt (const char *qFile, char *errstr)
{
	char errMsg[64];
	inProgress = 1;
	int res = StreamFile (qFile);
	if ( res != NoError) 
	{
		FormatErrStr(res, errMsg);
		sprintf(errstr, "StreamFile failed, code=%s, msg=%s", res, errMsg);
		return 0;
	}
	while (inProgress)
		Sleep(300);
	return 1;
}

void NICstreamCleanUp()
{
	CleanUp();
}