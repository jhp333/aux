#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <process.h>
#include "sigplus_internal.h"

typedef struct {
  char			 chunkID[4];
  long           chunkSize;
  short          wFormatTag;
  unsigned short wChannels;
  unsigned long  dwSamplesPerSec;
  unsigned long  dwAvgBytesPerSec;
  unsigned short wBlockAlign;
  unsigned short wBitsPerSample;
} FormatChunk;

//FYI: These two message are sent to different threads.
#define WM__RETURN			WM_APP+10

#define OK			0
#define ERR			-1
#define NO_LOOP			0
#define LOOP_PLAY		1

HWND hMmBase = NULL;
double _24bit_to_double(int x);

#define MMCHECK(x) { rc = (x); if (rc!=MMSYSERR_NOERROR) {sprintf (errstr,"Error at "#x); return rc;}}

int CheckChunk(const char * chunkStr, char *errstr)
{
	if (strncmp(chunkStr, "data", 4)==0 || strncmp(chunkStr, "fmt ", 4)==0) 	return 1;	
	if (strncmp(chunkStr, "fact", 4)==0) {	strcpy(errstr, "Cannot read compressed wav file."); return 0; }
	else return -1; // other format chunk
}

int ReadWavInfo (const char *fname, WAVEFORMATEX * pwf, int *headerInd, char *errstr)
{
	HANDLE h;
	FormatChunk fmt;
	char buffer[128];
	long chunkSize, dataChunkSize;
	DWORD dw;
	int count(0), i, frameBytes, numFrames;

	if ((h = CreateFile (fname, GENERIC_READ, FILE_SHARE_READ, NULL,
					OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL) )==INVALID_HANDLE_VALUE)
	{	strcpy(errstr, "File not found"); return 0; }

	*headerInd=0;

	i=ReadFile (h, buffer, 12, &dw, NULL);// already read and interpret at ReadWavInfo
	*headerInd += dw;
	if (strncmp(buffer, "RIFF", 4)!=0)
	{	strcpy(errstr, "Wave file must start in RIFF format"); CloseHandle(h); return 0; }
	if (strncmp(buffer+8, "WAVE", 4)!=0)
	{	strcpy(errstr, "Wave file must start be RIFF WAVE"); CloseHandle(h); return 0; }
	i=ReadFile (h, buffer, 4*sizeof(WORD), &dw, NULL);
	*headerInd += dw;

	while (dw>0 && count<2)
	{
		i = CheckChunk(buffer, errstr);
		memcpy((void*)&chunkSize,(void*)(buffer+4), sizeof(chunkSize));
		if (!i) return 0; // fact chunk  (compressed)
		if (i>0) //chunk either "fmt " or "data"
		{
			count++;
			if (strncmp(buffer, "fmt ", 4)==0) 
			{
				ReadFile (h, buffer+dw, chunkSize, &dw, NULL);
				*headerInd += dw;
				memcpy((void*)&fmt, (void*)buffer, sizeof(fmt));
			}
			else // data
				dataChunkSize = (long)*(long*)(buffer+4);
		}
		else 
		{
			ReadFile (h, buffer+dw, chunkSize, &dw, NULL);
			*headerInd += dw;
		}
		if (count<2) 
		{
			ReadFile (h, buffer, 4*sizeof(WORD), &dw, NULL);
			*headerInd += dw;
		}
	}
	frameBytes = fmt.wChannels * fmt.wBitsPerSample / 8 ;
	numFrames = dataChunkSize / frameBytes;
	CloseHandle(h);
	pwf->nSamplesPerSec = fmt.dwSamplesPerSec;
	pwf->nChannels = fmt.wChannels;
	pwf->wBitsPerSample = fmt.wBitsPerSample;
	pwf->wFormatTag = fmt.wFormatTag;
	pwf->nAvgBytesPerSec = fmt.dwAvgBytesPerSec;
	pwf->nBlockAlign = fmt.wBlockAlign;
	pwf->cbSize = (unsigned short)fmt.chunkSize;
	return numFrames;
}

void MakeWavInfoBlock (char *block, int fs, short nChan, int nSamples)
{
	// For a 16bit PCM wave file, the header block (before the actual data block) including the
	// length-specifier is 44 bytes long. This function makes that block.
	// cf. Microsoft WAVE soundfile format.htm

	int x;
	short y;
	strcpy(block, "RIFF");
	int SubChunk2Size = nSamples * 2 * nChan;
	x = 36 + SubChunk2Size; // PCM only
	memcpy((void*)(block+4), (void*)&x, sizeof(x)); // 4 bytes
	strcpy(block+8, "WAVEfmt ");
	x=16; // 16 for PCM
	memcpy((void*)(block+16), (void*)&x, sizeof(x)); // 4 bytes
	y=1; // AudioFormat, 1 for PCM
	memcpy((void*)(block+20), (void*)&y, sizeof(y)); // 2 bytes
	memcpy((void*)(block+22), (void*)&nChan, sizeof(nChan)); // 2 bytes
	memcpy((void*)(block+24), (void*)&fs, sizeof(fs)); // 4 bytes
	x = 2*fs*nChan; // byte rate , 16 bit PCM
	memcpy((void*)(block+28), (void*)&x, sizeof(x)); // 4 bytes
	y = 2*nChan; //block align, 16 bit PCM
	memcpy((void*)(block+32), (void*)&y, sizeof(y)); // 2 bytes
	y = 16;//bits per sample
	memcpy((void*)(block+34), (void*)&y, sizeof(y)); // 2 bytes
	strcpy(block+36, "data");
	x = nSamples * 2 * nChan; // number of bytes in the data (16 bit PCM)
	memcpy((void*)(block+40), (void*)&x, sizeof(x)); // 4 bytes
}

int ReadWavData (const char *fname, void * outBuffer)
{
	// fname : input 
	// pwf : output
	// outBuffer : output. This must have been prepared with the format parameter obtained by ReadWavInfo
	//             (the size of this buffer should be (wBitsPerSample/8)*nChannels*numFrames

	char buffer[128], errstr[128];
	void *out;
	HANDLE h;
	DWORD dw;
	long dataChunkSize;
	int ind, frameBytes, numFrames;
	WAVEFORMATEX wf;

	ReadWavInfo (fname, &wf, &ind, errstr);
	if ((h = CreateFile (fname, GENERIC_READ, FILE_SHARE_READ, NULL,
					OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL) )==INVALID_HANDLE_VALUE)
					return -1; // file not exist

	SetFilePointer(h, ind-4, NULL, FILE_BEGIN); //This will move the file pointer right after "data" in wave header
	ReadFile (h, buffer, sizeof(long), &dw, NULL);
	memcpy((void*)&dataChunkSize, (void*)buffer, sizeof(long));
	frameBytes = wf.nChannels * wf.wBitsPerSample / 8 ;
	numFrames = dataChunkSize / frameBytes;
	out = calloc (numFrames*frameBytes, 1);
	ReadFile (h, out, numFrames*frameBytes, &dw, NULL);
	if ((int)dw!=numFrames*frameBytes)
		return -6; // actual data and wave header not consistent
	memcpy(outBuffer, out, numFrames*frameBytes);
	CloseHandle (h);
	free(out);
	return numFrames;
}

int sortWavDataBufferByChannel(double *sorted, void *wavData, int ch, int nSamples, WAVEFORMATEX *wFormat)
{ 
	char c;
	int k;
	if (wFormat->wBitsPerSample==8)
	{
		for (int i=0; i<nSamples; i++)
		{
			c = ((unsigned char*)wavData)[i*wFormat->nChannels+ch] - (unsigned char)0x80;
			k = c << 24; // CHECK!!!
			sorted [i] = _24bit_to_double(k);
		}
	}
	else if (wFormat->wBitsPerSample==16)
	{
		for (int i=0; i<nSamples; i++)
		{
			k = ((short*)wavData)[i*wFormat->nChannels+ch] <<8;
			sorted [i] = _24bit_to_double(k);
		}
	}
	else if (wFormat->wBitsPerSample==24)
	{
		for (int i=0; i<nSamples; i++)
		{
			k = ((int*)wavData)[i*wFormat->nChannels+ch];
			sorted [i] = _24bit_to_double(k);
		}
	}
	else
		return 0;
	return 1;
}
	
class CWavePlay
{
public:
	HWND			hWnd_calling;
	uintptr_t	hThread;
	unsigned long	procID;
	UINT			msgID;
	DWORD			totalSamples;
	DWORD			curProcPt;
	int				ReportIndex;
	bool			blockMode;
	bool			stopped;
	bool			ready2Clean;
	DWORD			playBufferLen;
	VOID*			playBuffer;
	HWAVEOUT		hwo;
	WAVEOUTCAPS		woc;
	//WAVEHDR *		wh[2];
	WAVEHDR			wh[2];
	WAVEFORMATEX	wfx;
	CWavePlay();
	~CWavePlay();
	void load2whlpData(WAVEHDR* lpwh, void* buffer, DWORD offset, int nBlock);
	void EndwavBuffer2snd();
	int OnBlockDone (WAVEHDR* lpwh);
private:
	int	cleanUp();
};

#define FILEOUT {HANDLE hf; DWORD dw; hf = CreateFile("wavex.txt", GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);\
	SetFilePointer(hf, NULL,NULL, FILE_END); WriteFile(hf, fout, strlen(fout), &dw, NULL); CloseHandle(hf);}

CWavePlay::CWavePlay()
: playBuffer(NULL), hThread(NULL)
{
	// wh[i] pointers should be global variables, or it is unstable, crashes or at least waveOutUnPrepareHeader returns error consistantly when cleaning up.

	// really? Let me try. Changing "WAVEHDR *wh[2]" to "WAVEHDR wh[2]"...
	// Works well. It might have been the DLL boundary problem. 3/9/2012 jhpark
	//wh[0] = &wavheaderforOut1;
	//wh[1] = &wavheaderforOut2;
}

CWavePlay::~CWavePlay()
{
	// Freeing only if it was not cleaned normally (the MM_DONE message will normally clean it, but if the application ends before MM_DONE is posted, this is necessary.
	if (playBuffer) free(playBuffer);
}


int CWavePlay::cleanUp()
{
	if (ready2Clean) {
		int rc = 0;
		for (int i=0; i<2 && (rc=waveOutUnprepareHeader(hwo, wh+i, sizeof(WAVEHDR)))==MMSYSERR_NOERROR; ++i)
			;
		if (rc==MMSYSERR_NOERROR)
			rc = waveOutClose(hwo);
		if (rc==MMSYSERR_NOERROR) {
			if (blockMode)
				PostThreadMessage (procID, WM__RETURN, OK, 0);
		} else {
			SendMessage (hWnd_calling, msgID, 0xFFFF, (LPARAM)rc);
			PostThreadMessage (procID, WM__RETURN, ERR, (LPARAM)rc);
		}
		return rc;
	} else {
		 ready2Clean=true;
		 return 1;
	}
}

void CWavePlay::load2whlpData (WAVEHDR* lpwh, void* buffer, DWORD offset, int nBlock)
{	
	if (wfx.wBitsPerSample ==8)
		lpwh->lpData=(char*)buffer+offset;
	else if (wfx.wBitsPerSample ==16)
		lpwh->lpData=(char*)((short*)buffer+offset);
	lpwh->dwBufferLength = nBlock * wfx.wBitsPerSample / 8 ;
	lpwh->dwFlags = 0;
}

map<WAVEHDR *, CWavePlay *> mapWavePlays;

int CWavePlay::OnBlockDone(WAVEHDR* lpwh)
{
	int blockLen;
	MMRESULT	rc;
	int i = GetCurrentThreadId ();

	if (stopped)  
	{	SendMessage (hWnd_calling, msgID, 0, ReportIndex);	return cleanUp(); }
	if (++ReportIndex>0 && !blockMode) 	
		SendMessage (hWnd_calling, msgID, (curProcPt == totalSamples)*(totalSamples/wfx.nChannels), ReportIndex);
	if (curProcPt<totalSamples)
	{  // blockLen could be all the same if it is not integer. In reality, because it is integer, there will be 
		// some leftover if blockLen always the same as playBufferLen. So the last block should be treated differently.
		if (totalSamples-curProcPt>=2*playBufferLen) 
			blockLen = playBufferLen;
		else
			blockLen = totalSamples-curProcPt;
		load2whlpData (lpwh, playBuffer, curProcPt, blockLen);
		rc = waveOutPrepareHeader (hwo, lpwh, sizeof(WAVEHDR));
		if (rc==MMSYSERR_NOERROR)
			rc = waveOutWrite (hwo, lpwh, sizeof(WAVEHDR));
		if (rc!=MMSYSERR_NOERROR)
		{
			SendMessage (hWnd_calling, msgID, 0xFFFF, (LPARAM)rc);
			PostThreadMessage (procID, WM__RETURN, ERR, (LPARAM)rc);
			return -1;
		}
		curProcPt += blockLen;
		return 0;
	}
	else // curProcPt must be equal to totalSamples
		return cleanUp();
}

void EndwavBuffer2snd(CWavePlay *pWavePlay)
{
	int rc;
	pWavePlay->stopped = true;
	if ((rc=waveOutReset(pWavePlay->hwo))!=MMSYSERR_NOERROR)
		SendMessage (pWavePlay->hWnd_calling, pWavePlay->msgID, 0xFFFF, (LPARAM)rc);
}

LRESULT CALLBACK mmBaseProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch( uMsg )
    {
	case WOM_DONE:
		{
			WAVEHDR *pWH = (WAVEHDR *)lParam;
			if (CWavePlay *pWavePlay = mapWavePlays[pWH]) {
				switch (int t = pWavePlay->OnBlockDone(pWH)) {
				case -1:// Error. waveOutXXXX failed... What should be done?
					// let's clean everything up for now.
					// go through
				case 2:	// playing is finished. Clean up WavePlay.
					mapWavePlays.erase(pWH);
					delete pWavePlay;
					break;
				case 1:	// a buffer is finished. Clean up the link.
					mapWavePlays.erase(pWH);
					break;
				case 0:	// playing is in progress... no clean up work.
					break;
				}
			}
			return 0 ;
		}
	}
	return DefWindowProc (hwnd, uMsg, wParam, lParam) ;
}
unsigned int WINAPI Thread4MM (PVOID hParent)
{
	WNDCLASSEX wndclass ;
	MSG        msg ;
	HINSTANCE hInst = GetModuleHandle(NULL);

	if (GetClassInfoEx (hInst, "WC_WAVE_PLAY_PROC", &wndclass)==FALSE)
	{
	    wndclass.cbSize        = sizeof (wndclass) ;
		wndclass.hInstance     = hInst ;
	    wndclass.lpszMenuName = 0 ;
	    wndclass.lpszClassName = "WC_WAVE_PLAY_PROC" ;
	    wndclass.lpfnWndProc   = mmBaseProc ;
		wndclass.style         = wndclass.cbClsExtra = wndclass.cbWndExtra = 0;
	    wndclass.hIcon         = wndclass.hIconSm = wndclass.hCursor = 0;
		wndclass.hbrBackground = 0;
		RegisterClassEx (&wndclass) ;
	}

	hMmBase = CreateWindow ("WC_WAVE_PLAY_PROC", "", WS_OVERLAPPEDWINDOW, 0, 0, 0, 0,
                          NULL, NULL, hInst, NULL) ;

	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return 0;
}

void initCWavePlay(CWavePlay **pWP, int DevID, SHORT *dataBuffer, int length, int nChan, int fs)
{


}

int wavBuffer2snd(CWavePlay *pWP, int DevID, SHORT *dataBuffer, int length, int nChan, int fs, UINT userDefinedMsgID, HWND hApplWnd, int nProgReport, char *errstr)
// userDefinedMsgID == 0 : blocking (synchronous)
// nProgReport == -1 : loop
{	
	int	i, nDev;
	int nSamplesInBlock; // this is per each channel
	MMRESULT	rc;
	MSG msg;

	pWP->ready2Clean=false;
	pWP->ReportIndex=0;
	pWP->blockMode = (userDefinedMsgID==0);	// userDefinedMsgID == 0 : blocking (synchronous)
	if (pWP->playBuffer)
		free(pWP->playBuffer);
	pWP->playBuffer = dataBuffer;
	pWP->hWnd_calling = hApplWnd;
	pWP->msgID = userDefinedMsgID;
	pWP->procID = GetCurrentThreadId ();

	if (pWP->hThread==0)
	{
		pWP->hThread = _beginthreadex(NULL, 0, Thread4MM, 0, NULL, 0);
	}

	nDev=waveOutGetNumDevs();
	if (nDev<=DevID)
	{	strcpy(errstr, "Specified audio device not available.");	return -8;	}
	pWP->wfx.nChannels		= nChan;
	pWP->wfx.wFormatTag		= WAVE_FORMAT_PCM;
	pWP->wfx.nSamplesPerSec	= fs;
	pWP->wfx.nBlockAlign		= 2 * nChan;
	pWP->wfx.nAvgBytesPerSec	= fs * pWP->wfx.nBlockAlign;
	pWP->wfx.wBitsPerSample	= 16;
	pWP->wfx.cbSize			= 16;

	for (int i=0; !hMmBase && i<100; ++i)	// Waits for the window to finish initializingk, maximum 3 secs.
		Sleep(30);							// Without this, it intermittently freezes after playing a signal especially in Windows 7.
											// jhpark 12/9/2011
	MMCHECK(waveOutOpen (&pWP->hwo, DevID, &pWP->wfx, (DWORD_PTR)hMmBase, 0, CALLBACK_WINDOW));

	pWP->totalSamples = length * nChan;
	nSamplesInBlock = length / max(nProgReport,1);
	pWP->playBufferLen = nSamplesInBlock * nChan;
	pWP->stopped = false;

	// Initiate playing after double-buffering, any leftover is processed in hMmBase
	pWP->curProcPt = 0;
	for (i=0; i<2; i++) // if playBufferLen is odd, the first and second will be even-odd order.
	{ // if its multi-channel(i.e., stereo), playBufferLen must be multiple of nChan, otherwise left-right channels might be swapped around in the middle
		pWP->load2whlpData (pWP->wh+i, pWP->playBuffer, pWP->curProcPt, pWP->playBufferLen);
		pWP->curProcPt += pWP->playBufferLen;
		pWP->playBufferLen = (pWP->totalSamples-pWP->playBufferLen) / max((nProgReport-1),1); //only meaningful within this loop (only for when i=1)
		if (pWP->wh[i].dwUser & LOOP_PLAY)
		{
			pWP->wh[i].dwLoops=2;
			if (i==0) pWP->wh[i].dwFlags |= WHDR_BEGINLOOP;
			else pWP->wh[i].dwFlags |= WHDR_ENDLOOP;
		}
		MMCHECK(waveOutPrepareHeader (pWP->hwo, pWP->wh+i, sizeof(WAVEHDR)));
		MMCHECK(waveOutWrite (pWP->hwo, pWP->wh+i, sizeof(WAVEHDR)));
		if (i==0) rc=waveOutPause(pWP->hwo);
		else  rc=waveOutRestart(pWP->hwo);
		if (nProgReport<=1) break; // if nProgReport is 1 or 0, no double buffering
	}

	if (pWP->blockMode)
	{
		while (GetMessage(&msg, NULL, 0, 0))
		{
			if (msg.message==WM__RETURN)
			{
				if (msg.wParam==OK)
					return MMSYSERR_NOERROR;
				else
					return (int)msg.lParam;
			}
		}
		return -1;
	}
	else
	{
		return MMSYSERR_NOERROR;
	}
}

EXP_CS void *PlayBufAsynch16(int DevID, short *dataBuffer, int length, int nChan, int fs, UINT userDefinedMsgID, HWND hApplWnd, double block_dur, char *errstr)
// block_dur (in milliseconds) is the block duration for event notification.
{
	return NULL;
}

EXP_CS void *PlayBufAsynch16(int DevID, short *dataBuffer, int length, int nChan, int fs, UINT userDefinedMsgID, HWND hApplWnd, int nProgReport, char *errstr)
// loop play when nProgReport == -1
{
	static CWavePlay *pWavePlay = new CWavePlay();
	for (int i=0; i<sizeof(pWavePlay->wh)/sizeof(pWavePlay->wh[0]); ++i) {
		mapWavePlays[pWavePlay->wh+i] = pWavePlay;
		pWavePlay->wh[i].dwUser = (nProgReport==-1) ? LOOP_PLAY : NO_LOOP;
	}
	if (nProgReport==-1 && userDefinedMsgID==0)
		userDefinedMsgID = WM_USER+293;	// default message for event notification
	if (MMSYSERR_NOERROR != wavBuffer2snd(pWavePlay, DevID, dataBuffer, length, nChan, fs, userDefinedMsgID, hApplWnd, nProgReport, errstr)) {
		for (int i=0; i<sizeof(pWavePlay->wh)/sizeof(pWavePlay->wh[0]); ++i) {
			mapWavePlays.erase(pWavePlay->wh+i);
		}
		delete pWavePlay;
		return NULL;
	} else
		return pWavePlay;
}

EXP_CS void TerminatePlay(void *pWavePlay)
{
	bool fFound = false;
	CWavePlay *pWP = (CWavePlay *)pWavePlay;
	for (map<WAVEHDR *, CWavePlay *>::iterator it=mapWavePlays.begin(); it!=mapWavePlays.end(); ++it)
		if (it->second == pWP)
			fFound = true;
	if (fFound)
		EndwavBuffer2snd(pWP);
}

EXP_CS void PauseResumePlay(void *pWavePlay, bool fOnOff)
{
	bool fFound = false;
	CWavePlay *pWP = (CWavePlay *)pWavePlay;
	for (map<WAVEHDR *, CWavePlay *>::iterator it=mapWavePlays.begin(); it!=mapWavePlays.end(); ++it)
		if (it->second == pWP)
			fFound = true;
	if (!fFound)
		return;
	int rc;
	if (fOnOff)
		rc = waveOutRestart(pWP->hwo);
	else
		rc = waveOutPause(pWP->hwo);
	if (rc!=MMSYSERR_NOERROR)
		SendMessage (pWP->hWnd_calling, pWP->msgID, 0xFFFF, (LPARAM)rc);
}