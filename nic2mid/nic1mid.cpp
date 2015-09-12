// nic1mid.cpp
//

#include "stdafx.h"
#include "nic1mid_internal.h"

#define NUCLEUS_SERVICES_API __declspec( dllexport )
#define NUCLEUS_NIC_API __declspec(dllexport)
#define NIC_API __declspec(dllexport)

#ifndef NDEBUG 
#define LOADFP(OUT, DEFTYPE, FUNCSIGNATURE) if ((OUT = (DEFTYPE)GetProcAddress((HMODULE)hLib, FUNCSIGNATURE))==NULL) { sprintf(errstr, "Loading %s not successful", FUNCSIGNATURE); return 0;} 
#else 
#define LOADFP(OUT, DEFTYPE, FUNCSIGNATURE) OUT = (DEFTYPE)GetProcAddress((HMODULE)hLib, FUNCSIGNATURE);
#endif

#ifdef _MANAGED
#pragma managed(push, off)
#endif


NUCLEUS_SERVICES_API int CleanupNIC1Lib()	
{
	return FreeLibrary(hLib); 
}

#define NIC_BINARY_FNAME "NIC.DLL"

NUCLEUS_SERVICES_API int LoadNIC1Lib(const char * nic1path, char * errstr)
{ // nic1path must contain nic.dll for NIC1 library

	char estr[MAX_PATH], estr2[MAX_PATH];
	char fname[MAX_PATH], libName[MAX_PATH];

//	LoadString (hDllModule, IDS_NIC1_DLL, fname, sizeof(fname));
	strcpy(fname, NIC_BINARY_FNAME);
	FulfillFile(libName, nic1path, fname);
	hLib = LoadLibrary(libName);
	if (hLib==NULL) {
		GetLastErrorStr(estr2);
		sprintf(estr, "Loading error(%s): %s\r\n", libName, estr2); 
		strcpy(errstr, estr); return 0;	
	}

	LOADFP(fp_ImpCommunicatorConnect, PFUN_VOID, "ImpCommunicatorConnect")
	LOADFP(fp_ImpCommunicatorDelete, PFUN_VOID, "ImpCommunicatorDelete")
	LOADFP(fp_ImpCommunicatorDisconnect, PFUN_VOID, "ImpCommunicatorDisconnect")
	LOADFP(fp_ImpCommunicatorForceConnect, PFUN_VOID, "ImpCommunicatorForceConnect")
	LOADFP(fp_ImpCommunicatorInit, PFUN_VOID, "ImpCommunicatorInit")
	LOADFP(fp_ImpCommunicatorReset, PFUN_VOID, "ImpCommunicatorReset")
	LOADFP(fp_ImpCommunicatorSetCPSConfiguration, PFUN_INT, "ImpCommunicatorSetCPSConfiguration")
	LOADFP(fp_ImpCommunicatorStopSequence, PFUN_INT, "ImpCommunicatorStopSequence")
	LOADFP(fp_ImpCommunicatorSetDampGap, PFUN_MICROSEC, "ImpCommunicatorSetDampGap")
	LOADFP(fp_ImpCommunicatorSetPPSConfiguration, PFUN_INT_INT, "ImpCommunicatorSetPPSConfiguration")
	LOADFP(fp_ImpCommunicatorStartSequence, PFUN_start_seq, "ImpCommunicatorStartSequence")
	LOADFP(fp_ImpCommunicatorWriteSequence, PFUN_WRITE_SEQ, "ImpCommunicatorWriteSequence")

	LOADFP(fp_ImpFrameNew, PFUN_FRAME_NEW, "ImpFrameNew")
	LOADFP(fp_ImpFrameDelete, PFUN_FRAME, "ImpFrameDelete")
	LOADFP(fp_ImpFrameGetActiveElectrode, PFUN_FRAME_GET_ELEC, "ImpFrameGetActiveElectrode")
	LOADFP(fp_ImpFrameGetReferenceElectrode, PFUN_FRAME_GET_ELEC, "ImpFrameGetReferenceElectrode")
	LOADFP(fp_ImpFrameSetElectrodes, PFUN_FRAME_SET_ELECS, "ImpFrameSetElectrodes")
	LOADFP(fp_ImpFrameGetCurrentLevel, PFUN_FRAME_GET_CL, "ImpFrameGetCurrentLevel")
	LOADFP(fp_ImpFrameSetCurrentLevel, PFUN_FRAME_SET_CL, "ImpFrameSetCurrentLevel")
	LOADFP(fp_ImpFrameGetPeriod, PFUN_FRAME_GET_MICROSEC, "ImpFrameGetPeriod")
	LOADFP(fp_ImpFrameGetPhaseGap, PFUN_FRAME_GET_MICROSEC, "ImpFrameGetPhaseGap")
	LOADFP(fp_ImpFrameGetPhaseWidth, PFUN_FRAME_GET_MICROSEC, "ImpFrameGetPhaseWidth")
	LOADFP(fp_ImpFrameSetPeriod, PFUN_FRAME_SET_MICROSEC, "ImpFrameSetPeriod")
	LOADFP(fp_ImpFrameSetPhaseGap, PFUN_FRAME_SET_MICROSEC, "ImpFrameSetPhaseGap")
	LOADFP(fp_ImpFrameSetPhaseWidth, PFUN_FRAME_SET_MICROSEC, "ImpFrameSetPhaseWidth")
	LOADFP(fp_ImplantSelectType, PFUN_IMPL_SEL_TYPE, "ImplantSelectType")

	LOADFP(fp_ImpSequenceNew, PFUN_SEQ_NEW, "ImpSequenceNew")
	LOADFP(fp_ImpSequenceAppendEndToken, PFUN_SEQ, "ImpSequenceAppendEndToken")
	LOADFP(fp_ImpSequenceAppendNextToken, PFUN_SEQ, "ImpSequenceAppendNextToken")
	LOADFP(fp_ImpSequenceAppendPowerFrame, PFUN_SEQ, "ImpSequenceAppendPowerFrame")
	LOADFP(fp_ImpSequenceDelete, PFUN_SEQ, "ImpSequenceDelete")
	LOADFP(fp_ImpSequenceReset, PFUN_SEQ, "ImpSequenceReset")
	LOADFP(fp_ImpSequenceAppendFrame, PFUN_SEQ_FRAME, "ImpSequenceAppendFrame")
	LOADFP(fp_ImpSequenceSetPowerFrame, PFUN_SEQ_FRAME, "ImpSequenceSetPowerFrame")
	LOADFP(fp_ImpSequenceAppendPowerFrames, PFUN_SEQ_INT, "ImpSequenceAppendPowerFrames")
	LOADFP(fp_ImpSequenceAppendRepeatToken, PFUN_SEQ_INT, "ImpSequenceAppendRepeatToken")
	LOADFP(fp_ImpSequenceAppendSequence, PFUN_SEQ_SEQ_INT, "ImpSequenceAppendSequence")
	LOADFP(fp_ImpSequenceAppendFrames, PFUN_SEQ_FRAME_INT, "ImpSequenceAppendFrames")

	LOADFP(fp_RegisterOnSequenceEndFunction, PFUN_REG, "RegisterOnSequenceEndFunction")
	LOADFP(fp_GetLastNICError, PFUN_LASTERROR, "GetLastNICError")

	errstr[0]=0;
	return 1;
}

NIC_API Boolean CALL_CONVENTION ImpCommunicatorConnect(void)
{
	return fp_ImpCommunicatorConnect();
}


NIC_API Boolean CALL_CONVENTION ImpCommunicatorDelete (void)
{
	return fp_ImpCommunicatorDelete();
}
NIC_API Boolean CALL_CONVENTION ImpCommunicatorDisconnect (void)
{
	return fp_ImpCommunicatorDisconnect();
}
NIC_API Boolean CALL_CONVENTION ImpCommunicatorForceConnect  (void)
{
	return fp_ImpCommunicatorForceConnect();
}
NIC_API Boolean CALL_CONVENTION ImpCommunicatorInit(void)
{
	return fp_ImpCommunicatorInit();
}
NIC_API Boolean CALL_CONVENTION ImpCommunicatorReset  (void)
{
	return fp_ImpCommunicatorReset();
}
NIC_API Boolean CALL_CONVENTION ImpCommunicatorSetCPSConfiguration  (int address)
{
	return fp_ImpCommunicatorSetCPSConfiguration(address);
}

NIC_API Boolean CALL_CONVENTION ImpCommunicatorSetDampGap (Microsec damp_gap)
{
	return fp_ImpCommunicatorSetDampGap(damp_gap);
}
NIC_API Boolean CALL_CONVENTION ImpCommunicatorSetPPSConfiguration  (int port, int baud)
{
	return fp_ImpCommunicatorSetPPSConfiguration(port, baud);
}
NIC_API Boolean CALL_CONVENTION ImpCommunicatorStartSequence (SpeechProcessorProgram slot_number)
{
	return fp_ImpCommunicatorStartSequence(slot_number);
}
NIC_API Boolean CALL_CONVENTION ImpCommunicatorStopSequence  (int keep_implant_powered)
{
	return fp_ImpCommunicatorStopSequence(keep_implant_powered);
}
DLL_EXPORT Boolean CALL_CONVENTION ImpCommunicatorWriteSequence (SpeechProcessorProgram slot_number, IMP_SEQUENCE* s)
{
	return fp_ImpCommunicatorWriteSequence(slot_number, s);
}

NIC_API Boolean CALL_CONVENTION ImpFrameDelete  (IMP_FRAME* frame)
{
	return fp_ImpFrameDelete(frame);
}
NIC_API Boolean CALL_CONVENTION ImpFrameGetActiveElectrode (IMP_FRAME* frame, Electrode* active_electrode)
{
	return fp_ImpFrameGetActiveElectrode(frame, active_electrode);
}
NIC_API Boolean CALL_CONVENTION ImpFrameGetCurrentLevel(IMP_FRAME* frame, int* current_level)
{
	return fp_ImpFrameGetCurrentLevel(frame, current_level);
}
NIC_API Boolean CALL_CONVENTION ImpFrameGetPeriod (IMP_FRAME* frame, Microsec* period)
{
	return fp_ImpFrameGetPeriod(frame, period);
}
NIC_API Boolean CALL_CONVENTION ImpFrameGetPhaseGap (IMP_FRAME* frame, Microsec* phase_gap)
{
	return fp_ImpFrameGetPhaseGap(frame, phase_gap);
}
NIC_API Boolean CALL_CONVENTION ImpFrameGetPhaseWidth  (IMP_FRAME* frame, Microsec* phase_gap)
{
	return fp_ImpFrameGetPhaseWidth(frame, phase_gap);
}
NIC_API Boolean CALL_CONVENTION ImpFrameGetReferenceElectrode (IMP_FRAME* frame, Electrode* reference_electrode)
{
	return fp_ImpFrameGetReferenceElectrode(frame, reference_electrode);
}
NIC_API IMP_FRAME* CALL_CONVENTION ImpFrameNew(void)
{
	return fp_ImpFrameNew();
}
NIC_API Boolean CALL_CONVENTION ImpFrameSetCurrentLevel(IMP_FRAME* frame, int current_level)
{
	return fp_ImpFrameSetCurrentLevel(frame, current_level);
}
NIC_API Boolean CALL_CONVENTION ImpFrameSetElectrodes  (IMP_FRAME* frame, Electrode active_electrode, Electrode reference_electrode)
{
	return fp_ImpFrameSetElectrodes(frame, active_electrode, reference_electrode);
}
NIC_API Boolean CALL_CONVENTION ImpFrameSetPeriod (IMP_FRAME* frame, Microsec period)
{
	return fp_ImpFrameSetPeriod(frame, period);
}
NIC_API Boolean CALL_CONVENTION ImpFrameSetPhaseGap (IMP_FRAME* frame, Microsec phase_gap)
{
	return fp_ImpFrameSetPhaseGap(frame, phase_gap);
}
NIC_API Boolean CALL_CONVENTION ImpFrameSetPhaseWidth  (IMP_FRAME* frame, Microsec phase_width)
{
	return fp_ImpFrameSetPhaseWidth(frame, phase_width);
}

NIC_API Boolean  CALL_CONVENTION ImplantSelectType(ImplantIC IC_type, Protocol transmit_protocol, Megahertz transmit_frequency)
{
	return fp_ImplantSelectType(IC_type, transmit_protocol, transmit_frequency);
}

NIC_API Boolean CALL_CONVENTION ImpSequenceAppendEndToken(IMP_SEQUENCE* sequence)
{
	return fp_ImpSequenceAppendEndToken(sequence);
}
NIC_API Boolean CALL_CONVENTION ImpSequenceAppendFrame(IMP_SEQUENCE* sequence, IMP_FRAME* frame)
{
	return fp_ImpSequenceAppendFrame(sequence, frame);
}

NIC_API Boolean CALL_CONVENTION ImpSequenceAppendFrames  (IMP_SEQUENCE* sequence, IMP_FRAME* frame, int repeat_count)
{
	return fp_ImpSequenceAppendFrames(sequence, frame, repeat_count);
}
NIC_API Boolean CALL_CONVENTION ImpSequenceAppendNextToken (IMP_SEQUENCE* sequence)
{
	return fp_ImpSequenceAppendNextToken(sequence);
}
NIC_API Boolean CALL_CONVENTION ImpSequenceAppendPowerFrame(IMP_SEQUENCE* sequence)
{
	return fp_ImpSequenceAppendPowerFrame(sequence);
}
NIC_API Boolean CALL_CONVENTION ImpSequenceAppendPowerFrames (IMP_SEQUENCE* sequence, int repeat_count)
{
	return fp_ImpSequenceAppendPowerFrames(sequence, repeat_count);
}
NIC_API Boolean CALL_CONVENTION ImpSequenceAppendRepeatToken (IMP_SEQUENCE* sequence, int repeat_count)
{
	return fp_ImpSequenceAppendRepeatToken(sequence, repeat_count);
}
NIC_API Boolean CALL_CONVENTION ImpSequenceAppendSequence(IMP_SEQUENCE* sequence, IMP_SEQUENCE* sub_sequence, int repeat_count)
{
	return fp_ImpSequenceAppendSequence(sequence, sub_sequence, repeat_count);
}
NIC_API Boolean CALL_CONVENTION ImpSequenceDelete (IMP_SEQUENCE* sequence)
{
	return fp_ImpSequenceDelete(sequence);
}
NIC_API IMP_SEQUENCE* CALL_CONVENTION ImpSequenceNew (IMP_FRAME* power_frame)
{
	return fp_ImpSequenceNew(power_frame);
}
NIC_API Boolean CALL_CONVENTION ImpSequenceReset  (IMP_SEQUENCE* sequence)
{
	return fp_ImpSequenceReset(sequence);
}
NIC_API Boolean CALL_CONVENTION ImpSequenceSetPowerFrame (IMP_SEQUENCE* sequence, IMP_FRAME* power_frame)
{
	return fp_ImpSequenceSetPowerFrame(sequence, power_frame);
}

NIC_API Boolean CALL_CONVENTION RegisterOnSequenceEndFunction(OnConditionFunctionPtr user_function)
{
	return fp_RegisterOnSequenceEndFunction(user_function);
}

NIC_API NICErrorCode CALL_CONVENTION GetLastNICError(void)
{
	return fp_GetLastNICError();
}












