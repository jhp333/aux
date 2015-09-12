
#define NIC_DLL_BUILD

#include "resource.h"
#include "nic.h"

typedef Boolean (CALL_CONVENTION *PFUN_VOID) ();
PFUN_VOID fp_ImpCommunicatorConnect(NULL);
PFUN_VOID fp_ImpCommunicatorDelete(NULL);
PFUN_VOID fp_ImpCommunicatorDisconnect(NULL);
PFUN_VOID fp_ImpCommunicatorForceConnect(NULL);
PFUN_VOID fp_ImpCommunicatorInit(NULL);
PFUN_VOID fp_ImpCommunicatorReset(NULL);

typedef Boolean (CALL_CONVENTION *PFUN_INT) (int);
PFUN_INT fp_ImpCommunicatorSetCPSConfiguration(NULL);
PFUN_INT fp_ImpCommunicatorStopSequence(NULL);

typedef Boolean (CALL_CONVENTION *PFUN_MICROSEC) (Microsec);
PFUN_MICROSEC fp_ImpCommunicatorSetDampGap(NULL);

typedef Boolean (CALL_CONVENTION *PFUN_INT_INT) (int,int);
PFUN_INT_INT fp_ImpCommunicatorSetPPSConfiguration(NULL);

typedef Boolean (CALL_CONVENTION *PFUN_start_seq) (SpeechProcessorProgram);
PFUN_start_seq fp_ImpCommunicatorStartSequence(NULL);

typedef Boolean (CALL_CONVENTION *PFUN_WRITE_SEQ) (SpeechProcessorProgram, IMP_SEQUENCE*);
PFUN_WRITE_SEQ fp_ImpCommunicatorWriteSequence(NULL);

typedef IMP_FRAME* (CALL_CONVENTION *PFUN_FRAME_NEW)(void);
PFUN_FRAME_NEW fp_ImpFrameNew(NULL);
typedef Boolean (CALL_CONVENTION *PFUN_FRAME)(IMP_FRAME*);
PFUN_FRAME fp_ImpFrameDelete(NULL);
typedef Boolean (CALL_CONVENTION *PFUN_FRAME_GET_ELEC)(IMP_FRAME*, Electrode*);
PFUN_FRAME_GET_ELEC fp_ImpFrameGetActiveElectrode(NULL);
PFUN_FRAME_GET_ELEC fp_ImpFrameGetReferenceElectrode(NULL);
typedef Boolean (CALL_CONVENTION *PFUN_FRAME_SET_ELECS)(IMP_FRAME*, Electrode, Electrode);
PFUN_FRAME_SET_ELECS fp_ImpFrameSetElectrodes(NULL);
typedef Boolean (CALL_CONVENTION *PFUN_FRAME_GET_CL)(IMP_FRAME*, int*);
PFUN_FRAME_GET_CL fp_ImpFrameGetCurrentLevel(NULL);
typedef Boolean (CALL_CONVENTION *PFUN_FRAME_SET_CL)(IMP_FRAME*, int);
PFUN_FRAME_SET_CL fp_ImpFrameSetCurrentLevel(NULL);
typedef Boolean (CALL_CONVENTION *PFUN_FRAME_GET_MICROSEC)(IMP_FRAME*, Microsec*);
PFUN_FRAME_GET_MICROSEC fp_ImpFrameGetPeriod(NULL);
PFUN_FRAME_GET_MICROSEC fp_ImpFrameGetPhaseGap(NULL);
PFUN_FRAME_GET_MICROSEC fp_ImpFrameGetPhaseWidth(NULL);
typedef Boolean (CALL_CONVENTION *PFUN_FRAME_SET_MICROSEC)(IMP_FRAME*, Microsec);
PFUN_FRAME_SET_MICROSEC fp_ImpFrameSetPeriod(NULL);
PFUN_FRAME_SET_MICROSEC fp_ImpFrameSetPhaseGap(NULL);
PFUN_FRAME_SET_MICROSEC fp_ImpFrameSetPhaseWidth(NULL);

typedef Boolean (CALL_CONVENTION *PFUN_IMPL_SEL_TYPE) (ImplantIC, Protocol, Megahertz);
PFUN_IMPL_SEL_TYPE fp_ImplantSelectType(NULL);

typedef IMP_SEQUENCE* (CALL_CONVENTION *PFUN_SEQ_NEW)(IMP_FRAME*);
PFUN_SEQ_NEW fp_ImpSequenceNew(NULL);
typedef Boolean (CALL_CONVENTION *PFUN_SEQ)(IMP_SEQUENCE*);
PFUN_SEQ fp_ImpSequenceAppendEndToken(NULL);
PFUN_SEQ fp_ImpSequenceAppendNextToken(NULL);
PFUN_SEQ fp_ImpSequenceAppendPowerFrame(NULL);
PFUN_SEQ fp_ImpSequenceDelete(NULL);
PFUN_SEQ fp_ImpSequenceReset(NULL);
typedef Boolean (CALL_CONVENTION *PFUN_SEQ_FRAME)(IMP_SEQUENCE*, IMP_FRAME*);
PFUN_SEQ_FRAME fp_ImpSequenceAppendFrame(NULL);
PFUN_SEQ_FRAME fp_ImpSequenceSetPowerFrame(NULL);
typedef Boolean (CALL_CONVENTION *PFUN_SEQ_INT)(IMP_SEQUENCE*, int);
PFUN_SEQ_INT fp_ImpSequenceAppendPowerFrames(NULL);
PFUN_SEQ_INT fp_ImpSequenceAppendRepeatToken(NULL);
typedef Boolean (CALL_CONVENTION *PFUN_SEQ_SEQ_INT)(IMP_SEQUENCE*, IMP_SEQUENCE*, int);
PFUN_SEQ_SEQ_INT fp_ImpSequenceAppendSequence(NULL);
typedef Boolean (CALL_CONVENTION *PFUN_SEQ_FRAME_INT)(IMP_SEQUENCE*, IMP_FRAME*, int);
PFUN_SEQ_FRAME_INT fp_ImpSequenceAppendFrames(NULL);

typedef Boolean (CALL_CONVENTION *PFUN_REG)(OnConditionFunctionPtr);
PFUN_REG fp_RegisterOnSequenceEndFunction(NULL);

typedef NICErrorCode (CALL_CONVENTION *PFUN_LASTERROR)(void);
PFUN_LASTERROR fp_GetLastNICError(NULL);

static HMODULE hLib; // nic.dll

void GetLastErrorStr(char *errstr);
char * FulfillFile (char *out, const char *path, const char * file);

