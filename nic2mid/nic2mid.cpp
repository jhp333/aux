// nic2mid.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "nic2mid_internal.h"

#define NUCLEUS_SERVICES_API __declspec( dllexport )
#define NUCLEUS_NIC_API __declspec(dllexport)

#ifndef NDEBUG 
#define LOADPFNIC(OUT, DEFTYPE, FUNCSIGNATURE) if ((OUT = (DEFTYPE)GetProcAddress((HMODULE)hLib, FUNCSIGNATURE))==NULL) { sprintf(errstr, "Loading %s not successful", FUNCSIGNATURE); return 0;} 
#define LOADPFSERVICE(OUT, DEFTYPE, FUNCSIGNATURE) if ((OUT = (DEFTYPE)GetProcAddress((HMODULE)hLib2, FUNCSIGNATURE))==NULL) { sprintf(errstr, "Loading %s not successful", FUNCSIGNATURE); return 0;} 
#else 
#define LOADPFNIC(OUT, DEFTYPE, FUNCSIGNATURE) OUT = (DEFTYPE)GetProcAddress((HMODULE)hLib, FUNCSIGNATURE);
#define LOADPFSERVICE(OUT, DEFTYPE, FUNCSIGNATURE) OUT = (DEFTYPE)GetProcAddress((HMODULE)hLib2, FUNCSIGNATURE);
#endif

#ifdef _MANAGED
#pragma managed(push, off)
#endif

#ifndef LINK_STATIC
HMODULE hDllModule;

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
	hDllModule = (HMODULE)hModule;
    return TRUE;
}
#endif

#define NIC_BINARY_FNAME1 "2\\nic.dll"
#define NIC_BINARY_FNAME2 "services.dll"

NUCLEUS_SERVICES_API int CleanupNIC2Lib()
{
	return FreeLibrary(hLib2) * FreeLibrary(hLib); 
}

NUCLEUS_SERVICES_API int LoadNIC2Lib(const char * nic2path, char * errstr)
{ // nic2path must contain services.dll and subdirectory named "2" and nic.dll (for NIC2) in there

	char estr[MAX_PATH], estr2[MAX_PATH];
	char fname[MAX_PATH], libName[MAX_PATH];

	estr[0]=0; estr2[0]=0;
//	LoadString (hDllModule, IDS_NIC2_DLL1, fname, sizeof(fname));
	strcpy(fname, NIC_BINARY_FNAME1);

	FulfillFile(libName, nic2path, fname);
	hLib = LoadLibrary(libName);
	if (hLib==NULL ) {
		GetLastErrorStr(errstr);
		sprintf(estr, "Loading error(%s): %s\r\n", libName, errstr); 
	}
//	LoadString (hDllModule, IDS_NIC2_DLL2, fname, sizeof(fname));
	strcpy(fname, NIC_BINARY_FNAME2);
	FulfillFile(libName, nic2path, fname);
	hLib2 = LoadLibrary(libName);
	if (hLib2==NULL ) {
		GetLastErrorStr(errstr);
		if (strlen(estr)>0) strcat(estr, "\r\n");
		sprintf(estr2, "Loading error(%s): %s", libName, errstr); 
		strcat(estr, estr2);
	}
	if (strlen(estr)>0) { strcpy(errstr, estr); return 0;		}
	LOADPFNIC(pf_seq, PFUN_SEQNEW, "_SequenceNew@4")


	LOADPFSERVICE(pf4, PFUN_CLIENTNEW, "_NICStreamClientNew@4")
	LOADPFSERVICE(fp_err, PFUN_GETERR, "_lastServicesError@0")
	LOADPFSERVICE(fp_snd_data, PFUN_CL_SEQ_HANDLE, "_nscSendData@8")
	LOADPFSERVICE(fp_start, PFUN_CL_HANDLE, "_nscStartStream@4")
	LOADPFSERVICE(fp_stop, PFUN_CL_HANDLE, "_nscStopStream@4")
	LOADPFSERVICE(fP_stat, PFUN_GETSTAT, "_nscGetStreamStatus@8");
	LOADPFSERVICE(fp_ClientDelete, PFUN_CL_HANDLE,  "_nicStreamClientDelete@4");

	LOADPFNIC(fp_seqRead, PFUN_SEQREAD, "_sequenceRead@8")
	LOADPFNIC(fp_sq_del, PFUN_SEQCOMM_HANDLE, "_sequenceCommandDelete@4")

	LOADPFNIC(fp_BiphasicStimulusNew, PFUN_STIM, "_BiphasicStimulusNew@36")
	LOADPFNIC(fp_StimulusCommandNew, PFUN_SEQ_COMM, "_StimulusCommandNew@4")
	LOADPFNIC(fp_bsSetActiveElectrode, PFUN_SET_INT,"_bsSetActiveElectrode@8")
	LOADPFNIC(fp_bsSetReferenceElectrode, PFUN_SET_INT, "_bsSetReferenceElectrode@8")
	LOADPFNIC(fp_bsSetCurrentLevel, PFUN_SET_INT, "_bsSetCurrentLevel@8")
	LOADPFNIC(fp_bsGetActiveElectrode, PFUN_SET_PINT, "_bsGetActiveElectrode@8")
	LOADPFNIC(fp_bsGetReferenceElectrode, PFUN_SET_PINT, "_bsGetReferenceElectrode@8")
	LOADPFNIC(fp_bsGetCurrentLevel, PFUN_SET_PINT, "_bsGetCurrentLevel@8")
	LOADPFNIC(fp_bsSetPhaseWidth, PFUN_SET_DOUBLE, "_bsSetPhaseWidth@12")
	LOADPFNIC(fp_bsSetPhaseGap, PFUN_SET_DOUBLE, "_bsSetPhaseGap@12")
	LOADPFNIC(fp_bsSetPeriod, PFUN_SET_DOUBLE, "_bsSetPeriod@12")
	LOADPFNIC(fp_bsGetPhaseWidth, PFUN_SET_PDOUBLE, "_bsGetPhaseWidth@8")
	LOADPFNIC(fp_bsGetPhaseGap, PFUN_SET_PDOUBLE, "_bsGetPhaseGap@8")
	LOADPFNIC(fp_bsGetPeriod, PFUN_SET_PDOUBLE, "_bsGetPeriod@8")

	LOADPFNIC(fp_scSetStimulus, PFUN_SET_SEQ_STIM, "_scSetStimulus@8")
	LOADPFNIC(fp_scGetStimulus, PFUN_SET_SEQ_STIM, "_scGetStimulus@8")
	LOADPFNIC(fp_sequenceAppend, PFUN_APPEND, "_sequenceAppend@8")
	LOADPFNIC(fp_stimulusDelete, PFUN_STIM_DEL, "_stimulusDelete@4")
	LOADPFNIC(fp_sequenceGetSize, PFUN_SEQ_PINT, "_sequenceGetSize@8")
	LOADPFNIC(fp_sequenceGetNumberRepeats, PFUN_SEQ_PINT, "_sequenceGetNumberRepeats@8")
	LOADPFNIC(fp_sequenceSetNumberRepeats, PFUN_SEQ_INT, "_sequenceSetNumberRepeats@8")

	errstr[0]='\0';
	return 1;
}

NUCLEUS_SERVICES_API NICStreamClientHandle CALL_CONVENTION NICStreamClientNew(const char* server_address)
{
	return pf4(server_address);
}

NUCLEUS_NIC_API SequenceCommandHandle CALL_CONVENTION SequenceNew(
        int number_repeats )
{
	return pf_seq(number_repeats);
}

NUCLEUS_SERVICES_API ErrorCode CALL_CONVENTION lastServicesError()
{
	return fp_err();
}

NUCLEUS_NIC_API SuccessCode CALL_CONVENTION sequenceRead(
    SequenceCommandHandle sequence_object,
    const char* file_name )
{
	return fp_seqRead(sequence_object, file_name);
}

NUCLEUS_SERVICES_API SuccessCode CALL_CONVENTION nscSendData(
NICStreamClientHandle client_object,
SequenceCommandHandle sequence_object )
{
	return fp_snd_data (client_object, sequence_object );
}
NUCLEUS_SERVICES_API SuccessCode CALL_CONVENTION nscStartStream(
NICStreamClientHandle client_object )
{
	return fp_start (client_object);
}

NUCLEUS_SERVICES_API SuccessCode CALL_CONVENTION nscStopStream(
NICStreamClientHandle client_object )
{
	return fp_stop (client_object);
}

NUCLEUS_SERVICES_API SuccessCode CALL_CONVENTION  nicStreamClientDelete(
NICStreamClientHandle client_object )
{
	return fp_ClientDelete(client_object);
}

NUCLEUS_SERVICES_API SuccessCode CALL_CONVENTION nscGetStreamStatus(
NICStreamClientHandle client_object,
StreamStatus* status)
{
	return fP_stat (client_object, status);
}

NUCLEUS_NIC_API SuccessCode CALL_CONVENTION sequenceCommandDelete(
SequenceCommandHandle command_object )
{
	return fp_sq_del (command_object);
}

NUCLEUS_NIC_API StimulusHandle CALL_CONVENTION BiphasicStimulusNew(
int active_electrode, int reference_electrode, int current_level, Microsec phase_width, Microsec phase_gap, Microsec period )
{
	return fp_BiphasicStimulusNew  (active_electrode, reference_electrode, current_level, phase_width, phase_gap, period);
}

NUCLEUS_NIC_API SequenceCommandHandle CALL_CONVENTION StimulusCommandNew(
StimulusHandle stimulus_object )
{
	return fp_StimulusCommandNew (stimulus_object);
}

NUCLEUS_NIC_API SuccessCode CALL_CONVENTION bsSetActiveElectrode(
StimulusHandle stimulus_object, int active_electrode )
{
	return fp_bsSetActiveElectrode (stimulus_object, active_electrode);
}

NUCLEUS_NIC_API SuccessCode CALL_CONVENTION bsSetReferenceElectrode(
StimulusHandle stimulus_object, int reference_electrode)
{
	return fp_bsSetReferenceElectrode (stimulus_object, reference_electrode);
}

NUCLEUS_NIC_API SuccessCode CALL_CONVENTION bsSetCurrentLevel(
StimulusHandle stimulus_object, int current_level)
{
	return fp_bsSetCurrentLevel ( stimulus_object, current_level );
}
NUCLEUS_NIC_API SuccessCode CALL_CONVENTION bsGetActiveElectrode(
StimulusHandle stimulus_object, int* active_electrode )
{
	return fp_bsGetActiveElectrode (stimulus_object, active_electrode);
}

NUCLEUS_NIC_API SuccessCode CALL_CONVENTION bsGetReferenceElectrode(
StimulusHandle stimulus_object, int* reference_electrode )
{
	return fp_bsGetReferenceElectrode (stimulus_object, reference_electrode);
}
NUCLEUS_NIC_API SuccessCode CALL_CONVENTION bsGetCurrentLevel(
StimulusHandle stimulus_object, int* current_level)
{
	return fp_bsGetCurrentLevel (stimulus_object, current_level);
}

NUCLEUS_NIC_API SuccessCode CALL_CONVENTION bsSetPhaseWidth(
StimulusHandle stimulus_object, Microsec phase_width )
{
	return fp_bsSetPhaseWidth (stimulus_object, phase_width );
}

NUCLEUS_NIC_API SuccessCode CALL_CONVENTION bsSetPhaseGap(
StimulusHandle stimulus_object, Microsec phase_gap )
	
{
	return fp_bsSetPhaseGap (stimulus_object, phase_gap);
}

NUCLEUS_NIC_API SuccessCode CALL_CONVENTION bsSetPeriod(
StimulusHandle stimulus_object, Microsec period )
{
	return fp_bsSetPeriod (stimulus_object, period);
}

NUCLEUS_NIC_API SuccessCode CALL_CONVENTION bsGetPhaseWidth(
StimulusHandle stimulus_object, Microsec* phase_width)
{
	return fp_bsGetPhaseWidth (stimulus_object, phase_width);
}

NUCLEUS_NIC_API SuccessCode CALL_CONVENTION bsGetPhaseGap(
StimulusHandle stimulus_object, Microsec* phase_gap ) 
{
	return fp_bsGetPhaseGap (stimulus_object, phase_gap);
}

NUCLEUS_NIC_API SuccessCode CALL_CONVENTION bsGetPeriod(
StimulusHandle stimulus_object, Microsec* period )
{
	return fp_bsGetPeriod (stimulus_object, period);
}

NUCLEUS_NIC_API SuccessCode CALL_CONVENTION scSetStimulus(
SequenceCommandHandle command_object, StimulusHandle stimulus_object )
{
	return fp_scSetStimulus (command_object, stimulus_object);
}

NUCLEUS_NIC_API SuccessCode CALL_CONVENTION scGetStimulus(
SequenceCommandHandle command_object, StimulusHandle stimulus_object )
{
	return fp_scGetStimulus (command_object, stimulus_object);
}

NUCLEUS_NIC_API SuccessCode CALL_CONVENTION sequenceAppend(
SequenceCommandHandle sequence_object, SequenceCommandHandle command_object )
{
	return fp_sequenceAppend (sequence_object, command_object);
}

NUCLEUS_NIC_API SuccessCode CALL_CONVENTION stimulusDelete(
StimulusHandle stimulus_object )
{
	return fp_stimulusDelete (stimulus_object);
}

NUCLEUS_NIC_API SuccessCode CALL_CONVENTION sequenceGetSize(
SequenceCommandHandle sequence_object, int* sequence_size )
{
	return fp_sequenceGetSize (sequence_object, sequence_size );
}

NUCLEUS_NIC_API SuccessCode CALL_CONVENTION sequenceGetNumberRepeats( SequenceCommandHandle sequence_object, int* number_repeats )
{
	return fp_sequenceGetNumberRepeats (sequence_object, number_repeats );
}

NUCLEUS_NIC_API SuccessCode CALL_CONVENTION sequenceSetNumberRepeats( SequenceCommandHandle sequence_object, int number_repeats )
{
	return fp_sequenceSetNumberRepeats (sequence_object, number_repeats );
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

