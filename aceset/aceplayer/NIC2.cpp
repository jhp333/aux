#include "aceplayer.h"
#include "services/c_interface/CNICStreamClient.h"
#include "nic/c_interface/CSequence.h"
#include "nic/c_interface/CStimulusCommand.h"
#include "nic/c_interface/CBiphasicStimulus.h"
#include "services/c_interface/CServicesErrors.h"

int LoadNIC2Lib(const char * nic2path, char * errstr);
int CleanupNIC2Lib();

void evnThread (PVOID dummy);
void GetLastErrorStr(char *errstr);

SequenceCommandHandle sequence;
NICStreamClientHandle streamClient;

DWORD mainThreadID;
HWND hMain;

#define CHECKNIC2_0(X) \
	if ((X)!=Call_Succeeded) { sprintf(errstring,#X" failed. Code=%d", lastServicesError()); return 0;}

int Mode2RE(int mode)
{
	switch(mode)
	{
	case 101:
		return -1;
	case 102:
		return -2;
	case 103:
		return -3;
	default:
		return mode;
	}
}

int InitNIC2(HWND hDlg, const char *nicpath, const char *initializeStr, char *errstr)
{
	errstr[0]=0;
	if (!LoadNIC2Lib(nicpath, errstr)) return 0;

	if ((sequence=SequenceNew(1))==NULL) {
		sprintf(errstr, "Error in SequenceNew, code=%d", lastServicesError());
		CleanupNIC2Lib();
		return 0;		}
	if ((streamClient=NICStreamClientNew(initializeStr))==NULL) {
		sprintf(errstr, "Error in NICStreamClientNew, param=%s, code=%d", initializeStr, lastServicesError());
		CleanupNIC2Lib();
		return 0;		}
	hMain = hDlg;
	mainThreadID = GetCurrentThreadId();
	return 1;
}

int NIC2CleanUp()
{
	int res(1);

	res = nicStreamClientDelete(streamClient);
	res *= sequenceCommandDelete(sequence);
	CleanupNIC2Lib();
	return res;
}

int streamit(char *errstr)
{
	if (nscStopStream(streamClient)!=Call_Succeeded) {
		if (lastServicesError()!=NULL) 
			sprintf(errstr,"nscStopStream failed, code=%d", lastServicesError());
		else
			strcpy(errstr,"nscStopStream failed, lastServicesError failed, too.");
		return 0;
	}
	if (nscSendData(streamClient, sequence) != Call_Succeeded) {
		sprintf(errstr, "nscSendData failed. code=%d", lastServicesError());
		return 0;
	}
	if (nscStartStream(streamClient) != Call_Succeeded) {
		sprintf(errstr, "nscStartStream failed. code=%d", lastServicesError());
		return 0;
	}

	StreamStatus status;
	while (nscGetStreamStatus(streamClient, &status) == Call_Succeeded
			&& (status == Streaming || status == Stopped) )		// "Stopped" means the end of streaming, NOT the end of stimulation.
		Sleep(50);
	if (status != Idle && status != Waiting) {
		sprintf(errstr, "Unexpected StreamStatus: %d", status);
		return 0;
	}
	return 1;
}

int NIC2StreamIt (ACESEQ *seq, double dur_powerframes, char *errstring, bool readandstream)
{
	StimulusHandle unitframe;
	SequenceCommandHandle stimulusCommand;

	if (!sequenceCommandDelete(sequence)) {strcpy(errstring,"sequenceCommandDelete"); return 0;}
	sequence = SequenceNew(1);
	unitframe = BiphasicStimulusNew(0, 0, 0, 0, 0, 0);
	stimulusCommand = StimulusCommandNew(unitframe);

	//insert powerup frames
	double power_frames_period = 100.; //us
	for (int i=0; i<(int)(dur_powerframes*1000./power_frames_period+.5); i++)
	{
		CHECKNIC2_0(bsSetActiveElectrode(unitframe, 5))
		CHECKNIC2_0(bsSetCurrentLevel(unitframe, 0))
		CHECKNIC2_0(bsSetReferenceElectrode(unitframe, 6))
		CHECKNIC2_0(bsSetPhaseWidth(unitframe, 30))
		CHECKNIC2_0(bsSetPhaseGap(unitframe, 8))
		CHECKNIC2_0(bsSetPeriod(unitframe, 100))
		CHECKNIC2_0(scSetStimulus(stimulusCommand, unitframe))
		CHECKNIC2_0(sequenceAppend(sequence, stimulusCommand))
	}
	// March 20, 2011, note by BK
	// the length of seq->pw was mysteriously 22 (even thought in the result of the m code 
	// it was 1, as expected... therefore, it crashed inside of the loop....
	// solution: changed from seq->lenPW==1 to seq->lenPW!=seq->length,
	//                   from seq->lenPW!=1 to seq->lenPW==seq->length

	if (seq->lenMode!=seq->length)  
		CHECKNIC2_0(bsSetReferenceElectrode(unitframe, Mode2RE((int)*seq->mode)))
	if (seq->lenPG!=seq->length) 
		CHECKNIC2_0(bsSetPhaseGap(unitframe, *seq->pg))
	if (seq->lenPW!=seq->length)
		CHECKNIC2_0(bsSetPhaseWidth(unitframe, *seq->pw))
	if (seq->lenPeriod!=seq->length) 
		CHECKNIC2_0(bsSetPeriod(unitframe, *seq->period))
	for (int i=0; i<seq->length; i++)
	{
		CHECKNIC2_0(bsSetActiveElectrode(unitframe, (int)seq->ae[i]))
		CHECKNIC2_0(bsSetCurrentLevel(unitframe, (int)seq->cl[i]))
		if (seq->lenMode==seq->length) 
			CHECKNIC2_0(bsSetReferenceElectrode(unitframe, Mode2RE((int)seq->mode[i])))
		if (seq->lenPW==seq->length) 
			CHECKNIC2_0(bsSetPhaseWidth(unitframe, seq->pw[i]))
		if (seq->lenPG==seq->length) 
			CHECKNIC2_0(bsSetPhaseGap(unitframe, seq->pg[i]))
		if (seq->lenPeriod==seq->length) 
			CHECKNIC2_0(bsSetPeriod(unitframe, seq->period[i]))

		CHECKNIC2_0(scSetStimulus(stimulusCommand, unitframe))
		CHECKNIC2_0(sequenceAppend(sequence, stimulusCommand))
	}
	CHECKNIC2_0(stimulusDelete (unitframe))
	CHECKNIC2_0(sequenceCommandDelete(stimulusCommand))
	return streamit(errstring);
}


int NIC2StreamIt (const char *streamFile, char *errstr, bool readandstream)
{
	int res;

	if (readandstream)
	{
		res = sequenceRead(sequence,streamFile);
		if (res!=Call_Succeeded)
		{	sprintf(errstr, "sequenceRead failed. code=%d", lastServicesError()); return 0; }
	}
	return streamit(errstr);
}

int StopStreamIt (char *errstr)
{
	if (nscStopStream(streamClient)!=Call_Succeeded)
	{	sprintf(errstr, "nscStopStream failed. code=%d", lastServicesError()); return 0; }
	return 1;
}