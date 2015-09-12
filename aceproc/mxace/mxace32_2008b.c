/*
 * MATLAB Compiler: 4.9 (R2008b)
 * Date: Sat Sep 08 13:00:47 2012
 * Arguments: "-B" "macro_default" "-W" "lib:mxace32_2008b" "-T" "link:lib"
 * "wav2seq" "wavletace" "freqwidth" "turn2cic4" "show_p" 
 */

#include <stdio.h>
#define EXPORTING_mxace32_2008b 1
#include "mxace32_2008b.h"
#ifdef __cplusplus
extern "C" {
#endif

extern mclComponentData __MCC_mxace32_2008b_component_data;

#ifdef __cplusplus
}
#endif


static HMCRINSTANCE _mcr_inst = NULL;


#if defined( _MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__) || defined(__LCC__)
#ifdef __LCC__
#undef EXTERN_C
#endif
#include <windows.h>

static char path_to_dll[_MAX_PATH];

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, void *pv)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        if (GetModuleFileName(hInstance, path_to_dll, _MAX_PATH) == 0)
            return FALSE;
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
    }
    return TRUE;
}
#endif
#ifdef __cplusplus
extern "C" {
#endif

static int mclDefaultPrintHandler(const char *s)
{
  return mclWrite(1 /* stdout */, s, sizeof(char)*strlen(s));
}

#ifdef __cplusplus
} /* End extern "C" block */
#endif

#ifdef __cplusplus
extern "C" {
#endif

static int mclDefaultErrorHandler(const char *s)
{
  int written = 0;
  size_t len = 0;
  len = strlen(s);
  written = mclWrite(2 /* stderr */, s, sizeof(char)*len);
  if (len > 0 && s[ len-1 ] != '\n')
    written += mclWrite(2 /* stderr */, "\n", sizeof(char));
  return written;
}

#ifdef __cplusplus
} /* End extern "C" block */
#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_mxace32_2008b_C_API 
#define LIB_mxace32_2008b_C_API /* No special import/export declaration */
#endif

LIB_mxace32_2008b_C_API 
bool MW_CALL_CONV mxace32_2008bInitializeWithHandlers(
    mclOutputHandlerFcn error_handler,
    mclOutputHandlerFcn print_handler
)
{
  if (_mcr_inst != NULL)
    return true;
  if (!mclmcrInitialize())
    return false;
  if (!GetModuleFileName(GetModuleHandle("mxace32_2008b"), path_to_dll, _MAX_PATH))
    return false;
  if (!mclInitializeComponentInstanceWithEmbeddedCTF(&_mcr_inst,
                                                     &__MCC_mxace32_2008b_component_data,
                                                     true, NoObjectType,
                                                     LibTarget, error_handler,
                                                     print_handler, 261349, path_to_dll))
    return false;
  return true;
}

LIB_mxace32_2008b_C_API 
bool MW_CALL_CONV mxace32_2008bInitialize(void)
{
  return mxace32_2008bInitializeWithHandlers(mclDefaultErrorHandler,
                                             mclDefaultPrintHandler);
}

LIB_mxace32_2008b_C_API 
void MW_CALL_CONV mxace32_2008bTerminate(void)
{
  if (_mcr_inst != NULL)
    mclTerminateInstance(&_mcr_inst);
}

LIB_mxace32_2008b_C_API 
long MW_CALL_CONV mxace32_2008bGetMcrID() 
{
  return mclGetID(_mcr_inst);
}

LIB_mxace32_2008b_C_API 
void MW_CALL_CONV mxace32_2008bPrintStackTrace(void) 
{
  char** stackTrace;
  int stackDepth = mclGetStackTrace(_mcr_inst, &stackTrace);
  int i;
  for(i=0; i<stackDepth; i++)
  {
    mclWrite(2 /* stderr */, stackTrace[i], sizeof(char)*strlen(stackTrace[i]));
    mclWrite(2 /* stderr */, "\n", sizeof(char)*strlen("\n"));
  }
  mclFreeStackTrace(&stackTrace, stackDepth);
}


LIB_mxace32_2008b_C_API 
bool MW_CALL_CONV mlxWav2seq(int nlhs, mxArray *plhs[],
                             int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "wav2seq", nlhs, plhs, nrhs, prhs);
}

LIB_mxace32_2008b_C_API 
bool MW_CALL_CONV mlxWavletace(int nlhs, mxArray *plhs[],
                               int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "wavletace", nlhs, plhs, nrhs, prhs);
}

LIB_mxace32_2008b_C_API 
bool MW_CALL_CONV mlxFreqwidth(int nlhs, mxArray *plhs[],
                               int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "freqwidth", nlhs, plhs, nrhs, prhs);
}

LIB_mxace32_2008b_C_API 
bool MW_CALL_CONV mlxTurn2cic4(int nlhs, mxArray *plhs[],
                               int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "turn2cic4", nlhs, plhs, nrhs, prhs);
}

LIB_mxace32_2008b_C_API 
bool MW_CALL_CONV mlxShow_p(int nlhs, mxArray *plhs[],
                            int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "show_p", nlhs, plhs, nrhs, prhs);
}

LIB_mxace32_2008b_C_API 
bool MW_CALL_CONV mlfWav2seq(int nargout, mxArray** q, mxArray** p
                             , mxArray* wavname, mxArray* p_in1
                             , mxArray* seqfilename)
{
  return mclMlfFeval(_mcr_inst, "wav2seq", nargout, 2, 3,
                     q, p, wavname, p_in1, seqfilename);
}

LIB_mxace32_2008b_C_API 
bool MW_CALL_CONV mlfWavletace(int nargout, mxArray** out, mxArray** p
                               , mxArray* in, mxArray* p_in1
                               , mxArray* outputfilename)
{
  return mclMlfFeval(_mcr_inst, "wavletace", nargout, 2, 3,
                     out, p, in, p_in1, outputfilename);
}

LIB_mxace32_2008b_C_API 
bool MW_CALL_CONV mlfFreqwidth(int nargout, mxArray** bins
                               , mxArray** cumBins, mxArray** freqBound
                               , mxArray* nBands, mxArray* freqs)
{
  return mclMlfFeval(_mcr_inst, "freqwidth", nargout, 3, 2,
                     bins, cumBins, freqBound, nBands, freqs);
}

LIB_mxace32_2008b_C_API 
bool MW_CALL_CONV mlfTurn2cic4(int nargout, mxArray** p, mxArray* p_in1)
{
  return mclMlfFeval(_mcr_inst, "turn2cic4", nargout, 1, 1, p, p_in1);
}

LIB_mxace32_2008b_C_API 
bool MW_CALL_CONV mlfShow_p(int nargout, mxArray** out
                            , mxArray* p, mxArray* fname)
{
  return mclMlfFeval(_mcr_inst, "show_p", nargout, 1, 2, out, p, fname);
}
