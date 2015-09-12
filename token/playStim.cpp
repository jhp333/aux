#include "token.h"
#include "tokenDlg.h"

char PipeName[128];
/*
int CTokenDlg::playStim (char *errStr)
{
	char reply[512], command[512], *buffer[16], errstr[2048], NodeName[512];
	char fname0[MAX_LEN_WAVE_NAME], fname1[MAX_LEN_WAVE_NAME], fname2[MAX_LEN_WAVE_NAME], fname3[MAX_LEN_WAVE_NAME];
	char *fname[4], wav2Play[576];
	char hostname[128];
	char *ps;
	int i, res, nItems;
	unsigned long nBytes;
	double level, level2;

	gethostname(hostname, sizeof(hostname));
	for (i=0; i<16; i++) buffer[i]=(char*)calloc(256,1);

	fname[0]=fname0; fname[1]=fname1; fname[2]=fname2; fname[3]=fname3;
	GetDlgItemText (IDC_ADDPROC, command, 128);
	
	if (strlen(command)>0)
	{
		int ch = (SendDlgItemMessage (IDC_STEREO, BM_GETCHECK)==BST_CHECKED) ? 1:0;	
		if (!SignalPlusBackground(STIM_PATH_FILE, STIM_PATH_FILE, command, ch, errStr))
			return 0;
		wsprintf (wav2Play,"c:%s%s", STIM_PATH, STIM_FILE);
	}
	else
		wsprintf (wav2Play,"c:%s%s", STIM_PATH, STIM_FILE);

	level = GetDlgItemDouble (hDlg, IDC_LEVEL);

	if (level>0.)
		level -= LeveldBrms(wav2Play, &level2, errstr); // remember: results from LeveldBrms will always be negative...

	GetDlgItemText (IDC_NODELIST, NodeName, sizeof(NodeName));
	wsprintf(PipeName, "\\\\%s\\pipe\\%s", NodeName, INTERFACE_PIPENAME);
	if (strlen(NodeName)==1 && NodeName[0]=='.')
		sprintf (command, "PREPARE %5.1f %s", level, wav2Play);
	else
	{
		char *point = strchr(wav2Play, ':');
		sprintf (command, "PREPARE %5.1f \\\\%s%s", level, hostname, point+1);
	}
	DISP_PIPE_LOG("[out]",command);
	res = CallNamedPipe(PipeName, (LPVOID)(LPCTSTR)command, strlen(command)+1,
		preparedCondition, MAX_PROCESSING_CONDITION_STRING, &nBytes, NMPWAIT_WAIT_FOREVER);
	while (!res)
	{
		GetLastErrorStr(reply);
		sprintf (errstr, "CallNamedPipe failed during PREPARE,\nmsg:%s", reply);
		if (MessageBox (errstr, "Retry or cancel? (you can still retry after changing settings)", MB_RETRYCANCEL)==IDRETRY)
		{
		res = CallNamedPipe(PipeName, (LPVOID)(LPCTSTR)command, strlen(command)+1,
				preparedCondition, MAX_PROCESSING_CONDITION_STRING, &nBytes, NMPWAIT_WAIT_FOREVER);
		}
		else
		{
			strcpy(errStr, errstr);
			return -1;
		}
	}
	preparedCondition[nBytes]='\0';
	DISP_PIPE_LOG("[in]",preparedCondition);
	nItems=str2strarray(buffer, 16, preparedCondition, " ");
	if (strcmp(buffer[0],"SUCCESS")) 
	{		sprintf (errStr, "FAILURE during PREPARE, message: %s", &preparedCondition[strlen(buffer[0])]);
		return 0;
	}
	else
	{
		if (nItems==0)
			strcpy(preparedCondition, "");
		else
		{
			ps = strstr(preparedCondition, buffer[1]);
			for (i=0; i<=(int)strlen(preparedCondition)-8; i +=8)
				strncpy (preparedCondition+i, preparedCondition+i+8, 8);
		}
	}
	strcpy(command, "PRESENT");
	if (SendDlgItemMessage (IDC_ACOUSTIC, BM_GETCHECK)==BST_CHECKED)
		PlaySound (wav2Play, NULL, SND_FILENAME | SND_ASYNC);

	DISP_PIPE_LOG("[out]",command);
	res = CallNamedPipe(PipeName, (LPVOID)(LPCTSTR)command, strlen(command)+1,
		reply, sizeof(reply), &nBytes, NMPWAIT_WAIT_FOREVER); 
	while (!res)
	{
		GetLastErrorStr(errstr);
		if(MessageBox ("CallNamedPipe failed(PRESENT). Retry(Yes), or Abort(No)?", errstr, MB_YESNO)==IDYES)
		{
			res = CallNamedPipe(PipeName, (LPVOID)(LPCTSTR)command, strlen(command)+1,
				reply, MAX_PROCESSING_CONDITION_STRING, &nBytes, NMPWAIT_WAIT_FOREVER);
		}
		else
		{
			strcpy(errStr, errstr);
			return 0;
		}
	}
	reply[nBytes]='\0';
	DISP_PIPE_LOG("[in]",reply);
	str2strarray(buffer, 16, reply, " ");
	if (strcmp(buffer[0],"SUCCESS")) // changed for version 1.93 --present error recovery
	{
		if(MessageBox ("Fail to PRESENT. Retry(Yes), or Abort(No)?", reply, MB_YESNO)==IDYES)
		{
			return playStim(errStr);
		}
		else
		{
			strcpy(errStr, reply);
			return 0;
		}
	}
	for (i=0; i<16; i++)
		free(buffer[i]);
	sprintf (errStr, "No error");
	return 1;
}

void CTokenDlg::PrepareNoise(char *noiseFile)
{
	static int initialized=0;
	WIN32_FIND_DATA fd;
	char fullname[MAX_PATH], buf[MAX_PATH];
	if (!initialized)
	{
		_prepareNoise(noiseFile);
		initialized=1;
	}
	else
	{
		if (strcmp(ss->noisefile,noiseFile))
		{
			if (noiseFile[0]!='\0')
			{
				append_path_file (fullname, ss->wavePath, noiseFile);
				if (FindFirstFile(fullname, &fd)==INVALID_HANDLE_VALUE)
				{
					sprintf (buf, "File %s not found.\nYES to continue with white noise, NO to re-type", fullname);
					if (MessageBox ("Noise file not found.", "", MB_YESNO)==IDYES)
					{
						SetDlgItemText (IDC_NOISEFILE, "");
						_prepareNoise("");
					}
					else
						SetFocus(GetDlgItem(IDC_NOISEFILE));
				}
				else
					_prepareNoise(noiseFile);
			}
			else
				_prepareNoise(noiseFile);
		}
	}
}

void CTokenDlg::_prepareNoise(char *noiseFile)
{
	char buf[MAX_PATH], errstr[256];
	int fs;
	if (noiseFile[0]=='\0')
	{
		append_path_file (buf, ss->wavePath, waveNameList[0]);
		if (!ReadwavInfo(buf, &fs, NULL, NULL, errstr))
			MessageBox (errstr, "Stimuli file not read, cannot prepare noise (fs?)");
		else if(!PrepBack(NOISE_PATH_FILE, NULL, fs, 100000, errstr))
			MessageBox (errstr, "Error in PrepBack()");
	}
	else
	{
		append_path_file (buf, ss->wavePath, noiseFile);
		if(!ReadBack(buf, NULL, errstr))
			MessageBox (errstr, "Error in ReadBack()");
	}
	strcpy(ss->noisefile, noiseFile);
}
*/