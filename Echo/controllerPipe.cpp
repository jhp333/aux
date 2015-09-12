#include "Echo.h"

int ConnectToPresenterPipe (HANDLE *hPipe, char* NodeName, char *errstr)
{
	char	PipeName[128];
	DWORD	PipeMode = PIPE_READMODE_MESSAGE | PIPE_WAIT;

// wait for Presenter to create the pipe ...
	wsprintf(PipeName, "\\\\%s\\pipe\\%s", NodeName, INTERFACE_PIPENAME);
	while (! WaitNamedPipe(PipeName, NMPWAIT_USE_DEFAULT_WAIT))		
	{
		if (GetLastError()!= ERROR_FILE_NOT_FOUND  )
		{
			GetLastErrorStr(errstr);
			return 0;
		}
		Sleep(100);				// retry every 100 ms
	}

// Open the Pipe ...

	*hPipe = CreateFile(	PipeName,					// pipe name
						GENERIC_READ | GENERIC_WRITE,	// desired access
						0,							// no sharing
						NULL,						// no security attributes
						OPEN_EXISTING,				// create disposition
						FILE_FLAG_WRITE_THROUGH,		// flags & attributes
						NULL);						// no template file
	if (*hPipe == NULL)
	{
		GetLastErrorStr(errstr);
		return 0;
	}

// Ensure message mode ...

	if (! SetNamedPipeHandleState(*hPipe,				// handle to named pipe
								&PipeMode,			// new pipe mode
								NULL,				// no maximum collection count
								NULL	))				// no time-out value
	{
		GetLastErrorStr(errstr);
		return 0;
	}

	return 1;
}
