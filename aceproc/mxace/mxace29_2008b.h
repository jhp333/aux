/*
 * MATLAB Compiler: 4.9 (R2008b)
 * Date: Thu Jan 05 12:37:25 2012
 * Arguments: "-B" "macro_default" "-W" "lib:mxace29_2008b" "-T" "link:lib"
 * "wav2seq" "wavletace" "freqwidth" "elimcrcr" "addmlines" 
 */

#ifndef __mxace29_2008b_h
#define __mxace29_2008b_h 1

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

#ifdef EXPORTING_mxace29_2008b
#define PUBLIC_mxace29_2008b_C_API __global
#else
#define PUBLIC_mxace29_2008b_C_API /* No import statement needed. */
#endif

#define LIB_mxace29_2008b_C_API PUBLIC_mxace29_2008b_C_API

#elif defined(_HPUX_SOURCE)

#ifdef EXPORTING_mxace29_2008b
#define PUBLIC_mxace29_2008b_C_API __declspec(dllexport)
#else
#define PUBLIC_mxace29_2008b_C_API __declspec(dllimport)
#endif

#define LIB_mxace29_2008b_C_API PUBLIC_mxace29_2008b_C_API


#else

#define LIB_mxace29_2008b_C_API

#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_mxace29_2008b_C_API 
#define LIB_mxace29_2008b_C_API /* No special import/export declaration */
#endif

extern LIB_mxace29_2008b_C_API 
bool MW_CALL_CONV mxace29_2008bInitializeWithHandlers(mclOutputHandlerFcn error_handler,
                                                      mclOutputHandlerFcn print_handler);

extern LIB_mxace29_2008b_C_API 
bool MW_CALL_CONV mxace29_2008bInitialize(void);

extern LIB_mxace29_2008b_C_API 
void MW_CALL_CONV mxace29_2008bTerminate(void);



extern LIB_mxace29_2008b_C_API 
void MW_CALL_CONV mxace29_2008bPrintStackTrace(void);


extern LIB_mxace29_2008b_C_API 
bool MW_CALL_CONV mlxWav2seq(int nlhs, mxArray *plhs[],
                             int nrhs, mxArray *prhs[]);

extern LIB_mxace29_2008b_C_API 
bool MW_CALL_CONV mlxWavletace(int nlhs, mxArray *plhs[],
                               int nrhs, mxArray *prhs[]);

extern LIB_mxace29_2008b_C_API 
bool MW_CALL_CONV mlxFreqwidth(int nlhs, mxArray *plhs[],
                               int nrhs, mxArray *prhs[]);

extern LIB_mxace29_2008b_C_API 
bool MW_CALL_CONV mlxElimcrcr(int nlhs, mxArray *plhs[],
                              int nrhs, mxArray *prhs[]);

extern LIB_mxace29_2008b_C_API 
bool MW_CALL_CONV mlxAddmlines(int nlhs, mxArray *plhs[],
                               int nrhs, mxArray *prhs[]);

extern LIB_mxace29_2008b_C_API 
long MW_CALL_CONV mxace29_2008bGetMcrID() ;



extern LIB_mxace29_2008b_C_API bool MW_CALL_CONV mlfWav2seq(int nargout
                                                            , mxArray** q
                                                            , mxArray** p
                                                            , mxArray* wavname
                                                            , mxArray* p_in1
                                                            , mxArray* seqfilename);

extern LIB_mxace29_2008b_C_API bool MW_CALL_CONV mlfWavletace(int nargout
                                                              , mxArray** out
                                                              , mxArray** p
                                                              , mxArray* in
                                                              , mxArray* p_in1
                                                              , mxArray* outputfilename);

extern LIB_mxace29_2008b_C_API bool MW_CALL_CONV mlfFreqwidth(int nargout
                                                              , mxArray** bins
                                                              , mxArray** cumBins
                                                              , mxArray** freqBound
                                                              , mxArray* nBands
                                                              , mxArray* freqs);

extern LIB_mxace29_2008b_C_API bool MW_CALL_CONV mlfElimcrcr(int nargout
                                                             , mxArray** s
                                                             , mxArray* strIn);

extern LIB_mxace29_2008b_C_API bool MW_CALL_CONV mlfAddmlines(int nargout
                                                              , mxArray** p
                                                              , mxArray* p_in1
                                                              , mxArray* str);

#ifdef __cplusplus
}
#endif

#endif
