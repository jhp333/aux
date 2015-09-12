typedef void (*OnConditionFunctionPtr)();

DLL_EXPORT Boolean    CALL_CONVENTION ImpFrameGetCurrentLevel       (IMP_FRAME* frame, int* current_level);
DLL_EXPORT Boolean    CALL_CONVENTION ImpFrameGetPeriod             (IMP_FRAME* frame, Microsec* period);
DLL_EXPORT Boolean    CALL_CONVENTION ImpFrameGetPhaseGap           (IMP_FRAME* frame, Microsec* phase_gap);
DLL_EXPORT Boolean    CALL_CONVENTION ImpFrameGetPhaseWidth         (IMP_FRAME* frame, Microsec* phase_gap);
DLL_EXPORT Boolean    CALL_CONVENTION ImpFrameSetCurrentLevel       (IMP_FRAME* frame, int current_level);
DLL_EXPORT Boolean    CALL_CONVENTION ImpFrameSetPeriod             (IMP_FRAME* frame, Microsec period);
DLL_EXPORT Boolean    CALL_CONVENTION ImpFrameSetPhaseGap           (IMP_FRAME* frame, Microsec phase_gap);
DLL_EXPORT Boolean    CALL_CONVENTION ImpFrameSetPhaseWidth         (IMP_FRAME* frame, Microsec phase_width);

DLL_EXPORT Boolean  CALL_CONVENTION ImplantSelectType     (ImplantIC IC_type, Protocol transmit_protocol, Megahertz transmit_frequency);

DLL_EXPORT Boolean       CALL_CONVENTION ImpSequenceAppendEndToken              (IMP_SEQUENCE* sequence);
DLL_EXPORT Boolean       CALL_CONVENTION ImpSequenceAppendFrame                 (IMP_SEQUENCE* sequence, IMP_FRAME* frame);
DLL_EXPORT Boolean       CALL_CONVENTION ImpSequenceAppendFrames                (IMP_SEQUENCE* sequence, IMP_FRAME* frame, int repeat_count);
DLL_EXPORT Boolean       CALL_CONVENTION ImpSequenceAppendNextToken             (IMP_SEQUENCE* sequence);
DLL_EXPORT Boolean       CALL_CONVENTION ImpSequenceAppendPowerFrame            (IMP_SEQUENCE* sequence);
DLL_EXPORT Boolean       CALL_CONVENTION ImpSequenceAppendPowerFrames           (IMP_SEQUENCE* sequence, int repeat_count);
DLL_EXPORT Boolean       CALL_CONVENTION ImpSequenceAppendRepeatToken           (IMP_SEQUENCE* sequence, int repeat_count);
DLL_EXPORT Boolean       CALL_CONVENTION ImpSequenceAppendSequence              (IMP_SEQUENCE* sequence, IMP_SEQUENCE* sub_sequence, int repeat_count);
DLL_EXPORT Boolean       CALL_CONVENTION ImpSequenceDelete                      (IMP_SEQUENCE* sequence);
DLL_EXPORT IMP_SEQUENCE* CALL_CONVENTION ImpSequenceNew                         (IMP_FRAME* power_frame);
DLL_EXPORT Boolean       CALL_CONVENTION ImpSequenceReset                       (IMP_SEQUENCE* sequence);
DLL_EXPORT Boolean       CALL_CONVENTION ImpSequenceSetPowerFrame               (IMP_SEQUENCE* sequence, IMP_FRAME* power_frame);

DLL_EXPORT Boolean CALL_CONVENTION RegisterOnSequenceEndFunction                 (OnConditionFunctionPtr user_function);

DLL_EXPORT NICErrorCode CALL_CONVENTION GetLastNICError       (void);
