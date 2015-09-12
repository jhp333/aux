
#define NUCLEUS_SERVICES_DLL_BUILD
#define NUCLEUS_NIC_DLL_BUILD

#include "resource.h"
#include "services/c_interface/CNICStreamClient.h"
#include "nic/c_interface/CSequence.h"
#include "nic/c_interface/CStimulusCommand.h"
#include "nic/c_interface/CBiphasicStimulus.h"
#include "services/c_interface/CServicesErrors.h"

typedef void* (CALL_CONVENTION * PFUN_CLIENTNEW) (const char*);
typedef void* (CALL_CONVENTION * PFUN_SEQNEW) (int);
typedef SuccessCode (CALL_CONVENTION *PFUN_CL_HANDLE) (NICStreamClientHandle);
typedef SuccessCode (CALL_CONVENTION *PFUN_CL_SEQ_HANDLE) (NICStreamClientHandle,SequenceCommandHandle);
typedef SuccessCode (CALL_CONVENTION *PFUN_SEQCOMM_HANDLE) (SequenceCommandHandle);
typedef SuccessCode (CALL_CONVENTION *PFUN_SEQREAD) (SequenceCommandHandle, const char*);
typedef SuccessCode (CALL_CONVENTION *PFUN_GETSTAT) (NICStreamClientHandle, StreamStatus*);
typedef ErrorCode (CALL_CONVENTION *PFUN_GETERR) ();

typedef StimulusHandle (CALL_CONVENTION *PFUN_STIM) (int, int, int, Microsec, Microsec, Microsec);
typedef SequenceCommandHandle (CALL_CONVENTION *PFUN_SEQ_COMM) (StimulusHandle);
typedef SuccessCode (CALL_CONVENTION *PFUN_SET_INT) (StimulusHandle, int);
typedef SuccessCode (CALL_CONVENTION *PFUN_SET_PINT) (StimulusHandle, int*);
typedef SuccessCode (CALL_CONVENTION *PFUN_SET_SEQ_STIM) (SequenceCommandHandle, StimulusHandle);
typedef SuccessCode (CALL_CONVENTION *PFUN_APPEND) (SequenceCommandHandle, SequenceCommandHandle);
typedef SuccessCode (CALL_CONVENTION *PFUN_STIM_DEL) (StimulusHandle);
typedef SuccessCode (CALL_CONVENTION *PFUN_SEQ_INT) (SequenceCommandHandle, int);
typedef SuccessCode (CALL_CONVENTION *PFUN_SEQ_PINT) (SequenceCommandHandle, int*);
typedef SuccessCode (CALL_CONVENTION *PFUN_SEQ_SEQ_INT) (SequenceCommandHandle, SequenceCommandHandle, int);
typedef SuccessCode (CALL_CONVENTION *PFUN_SET_DOUBLE) (StimulusHandle, double);
typedef SuccessCode (CALL_CONVENTION *PFUN_SET_PDOUBLE) (StimulusHandle, double*);

PFUN_STIM fp_BiphasicStimulusNew(NULL);
PFUN_SEQ_COMM fp_StimulusCommandNew(NULL);
PFUN_SET_INT fp_bsSetActiveElectrode(NULL), fp_bsSetReferenceElectrode(NULL), fp_bsSetCurrentLevel(NULL);
PFUN_SET_PINT fp_bsGetActiveElectrode(NULL), fp_bsGetReferenceElectrode(NULL), fp_bsGetCurrentLevel(NULL);
PFUN_SET_DOUBLE fp_bsSetPhaseWidth(NULL), fp_bsSetPhaseGap(NULL), fp_bsSetPeriod(NULL);
PFUN_SET_PDOUBLE fp_bsGetPhaseWidth(NULL), fp_bsGetPhaseGap(NULL), fp_bsGetPeriod(NULL);
PFUN_SET_SEQ_STIM fp_scSetStimulus(NULL), fp_scGetStimulus(NULL);
PFUN_APPEND fp_sequenceAppend(NULL);
PFUN_STIM_DEL fp_stimulusDelete(NULL);
PFUN_SEQ_PINT fp_sequenceGetSize(NULL), fp_sequenceGetNumberRepeats(NULL);
PFUN_SEQ_INT fp_sequenceSetNumberRepeats(NULL);
PFUN_CLIENTNEW pf4(NULL);

static HMODULE hLib; // nic.dll
static HMODULE hLib2; // services.dll

PFUN_SEQNEW pf_seq(NULL);
PFUN_CL_HANDLE fp_ClientDelete(NULL);
PFUN_CL_HANDLE fp_stop(NULL);
PFUN_CL_HANDLE fp_start(NULL);
PFUN_GETERR fp_err(NULL);
PFUN_SEQREAD fp_seqRead(NULL);
PFUN_CL_SEQ_HANDLE fp_snd_data(NULL);
PFUN_GETSTAT fP_stat(NULL);
PFUN_SEQCOMM_HANDLE fp_sq_del(NULL);



void GetLastErrorStr(char *errstr);
char * FulfillFile (char *out, const char *path, const char * file);

