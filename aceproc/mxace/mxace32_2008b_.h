/*
 * MATLAB Compiler: 4.9 (R2008b)
 * Date: Fri Jun 15 23:55:31 2012
 * Arguments: "-B" "macro_default" "-W" "lib:mxace32_2008b_" "-T" "link:lib"
 * "wav2seq" "wavletace" "freqwidth" "turn2cic4" "show_p" 
 */

#ifndef __mxace32_2008b__h
#define __mxace32_2008b__h 1

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

#ifdef EXPORTING_mxace32_2008b_
#define PUBLIC_mxace32_2008b__C_API __global
#else
#define PUBLIC_mxace32_2008b__C_API /* No import statement needed. */
#endif

#define LIB_mxace32_2008b__C_API PUBLIC_mxace32_2008b__C_API

#elif defined(_HPUX_SOURCE)

#ifdef EXPORTING_mxace32_2008b_
#define PUBLIC_mxace32_2008b__C_API __declspec(dllexport)
#else
#define PUBLIC_mxace32_2008b__C_API __declspec(dllimport)
#endif

#define LIB_mxace32_2008b__C_API PUBLIC_mxace32_2008b__C_API


#else

#define LIB_mxace32_2008b__C_API

#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_mxace32_2008b__C_API 
#define LIB_mxace32_2008b__C_API /* No special import/export declaration */
#endif

extern LIB_mxace32_2008b__C_API 
bool MW_CALL_CONV mxace32_2008b_InitializeWithHandlers(mclOutputHandlerFcn error_handler,
                                                       mclOutputHandlerFcn print_handler);

extern LIB_mxace32_2008b__C_API 
bool MW_CALL_CONV mxace32_2008b_Initialize(void);

extern LIB_mxace32_2008b__C_API 
void MW_CALL_CONV mxace32_2008b_Terminate(void);



extern LIB_mxace32_2008b__C_API 
void MW_CALL_CONV mxace32_2008b_PrintStackTrace(void);


extern LIB_mxace32_2008b__C_API 
bool MW_CALL_CONV mlxWav2seq(int nlhs, mxArray *plhs[],
                             int nrhs, mxArray *prhs[]);

extern LIB_mxace32_2008b__C_API 
bool MW_CALL_CONV mlxWavletace(int nlhs, mxArray *plhs[],
                               int nrhs, mxArray *prhs[]);

extern LIB_mxace32_2008b__C_API 
bool MW_CALL_CONV mlxFreqwidth(int nlhs, mxArray *plhs[],
                               int nrhs, mxArray *prhs[]);

extern LIB_mxace32_2008b__C_API 
bool MW_CALL_CONV mlxTurn2cic4(int nlhs, mxArray *plhs[],
                               int nrhs, mxArray *prhs[]);

extern LIB_mxace32_2008b__C_API 
bool MW_CALL_CONV mlxShow_p(int nlhs, mxArray *plhs[],
                            int nrhs, mxArray *prhs[]);

extern LIB_mxace32_2008b__C_API 
long MW_CALL_CONV mxace32_2008b_GetMcrID() ;



extern LIB_mxace32_2008b__C_API bool MW_CALL_CONV mlfWav2seq(int nargout
                                                             , mxArray** q
                                                             , mxArray** p
                                                             , mxArray* wavname
                                                             , mxArray* p_in1
                                                             , mxArray* seqfilename);

extern LIB_mxace32_2008b__C_API bool MW_CALL_CONV mlfWavletace(int nargout
                                                               , mxArray** out
                                                               , mxArray** p
                                                               , mxArray* in
                                                               , mxArray* p_in1
                                                               , mxArray* outputfilename);

extern LIB_mxace32_2008b__C_API bool MW_CALL_CONV mlfFreqwidth(int nargout
                                                               , mxArray** bins
                                                               , mxArray** cumBins
                                                               , mxArray** freqBound
                                                               , mxArray* nBands
                                                               , mxArray* freqs);

extern LIB_mxace32_2008b__C_API bool MW_CALL_CONV mlfTurn2cic4(int nargout
                                                               , mxArray** p
                                                               , mxArray* p_in1);

extern LIB_mxace32_2008b__C_API bool MW_CALL_CONV mlfShow_p(int nargout
                                                            , mxArray** out
                                                            , mxArray* p
                                                            , mxArray* fname);

#ifdef __cplusplus
}
#endif

#endif
