/*
 * MATLAB Compiler: 4.13 (R2010a)
 * Date: Wed May 11 15:11:08 2011
 * Arguments: "-B" "macro_default" "-W" "lib:mxace29_2010a" "-T" "link:lib"
 * "wav2seq" "wavletace" "freqwidth" "elimcrcr" "addmlines" 
 */

#ifndef __mxace29_2010a_h
#define __mxace29_2010a_h 1

#if defined(__cplusplus) && !defined(mclmcrrt_h) && defined(__linux__)
#  pragma implementation "mclmcrrt.h"
#endif
#include "mclmcrrt.h"
#ifdef __cplusplus
extern "C" {
#endif

#if defined(__SUNPRO_CC)
/* Solaris shared libraries use __global, rather than mapfiles
 * to define the API exported from a shared library. __global is
 * only necessary when building the library -- files including
 * this header file to use the library do not need the __global
 * declaration; hence the EXPORTING_<library> logic.
 */

#ifdef EXPORTING_mxace29_2010a
#define PUBLIC_mxace29_2010a_C_API __global
#else
#define PUBLIC_mxace29_2010a_C_API /* No import statement needed. */
#endif

#define LIB_mxace29_2010a_C_API PUBLIC_mxace29_2010a_C_API

#elif defined(_HPUX_SOURCE)

#ifdef EXPORTING_mxace29_2010a
#define PUBLIC_mxace29_2010a_C_API __declspec(dllexport)
#else
#define PUBLIC_mxace29_2010a_C_API __declspec(dllimport)
#endif

#define LIB_mxace29_2010a_C_API PUBLIC_mxace29_2010a_C_API


#else

#define LIB_mxace29_2010a_C_API

#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_mxace29_2010a_C_API 
#define LIB_mxace29_2010a_C_API /* No special import/export declaration */
#endif

extern LIB_mxace29_2010a_C_API 
bool MW_CALL_CONV mxace29_2010aInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_mxace29_2010a_C_API 
bool MW_CALL_CONV mxace29_2010aInitialize(void);

extern LIB_mxace29_2010a_C_API 
void MW_CALL_CONV mxace29_2010aTerminate(void);



extern LIB_mxace29_2010a_C_API 
void MW_CALL_CONV mxace29_2010aPrintStackTrace(void);

extern LIB_mxace29_2010a_C_API 
bool MW_CALL_CONV mlxWav2seq(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_mxace29_2010a_C_API 
bool MW_CALL_CONV mlxWavletace(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_mxace29_2010a_C_API 
bool MW_CALL_CONV mlxFreqwidth(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_mxace29_2010a_C_API 
bool MW_CALL_CONV mlxElimcrcr(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_mxace29_2010a_C_API 
bool MW_CALL_CONV mlxAddmlines(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_mxace29_2010a_C_API 
long MW_CALL_CONV mxace29_2010aGetMcrID();



extern LIB_mxace29_2010a_C_API bool MW_CALL_CONV mlfWav2seq(int nargout, mxArray** q, mxArray** p, mxArray* wavname, mxArray* p_in1, mxArray* seqfilename);

extern LIB_mxace29_2010a_C_API bool MW_CALL_CONV mlfWavletace(int nargout, mxArray** out, mxArray** p, mxArray* in, mxArray* p_in1, mxArray* outputfilename);

extern LIB_mxace29_2010a_C_API bool MW_CALL_CONV mlfFreqwidth(int nargout, mxArray** bins, mxArray** cumBins, mxArray** freqBound, mxArray* nBands, mxArray* freqs);

extern LIB_mxace29_2010a_C_API bool MW_CALL_CONV mlfElimcrcr(int nargout, mxArray** s, mxArray* strIn);

extern LIB_mxace29_2010a_C_API bool MW_CALL_CONV mlfAddmlines(int nargout, mxArray** p, mxArray* p_in1, mxArray* str);

#ifdef __cplusplus
}
#endif
#endif
