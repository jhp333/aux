#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <audfret.h>
#include "audstr.h"

#include "sigproc.h"

#define MAX_24BIT		(double)0x007fffff
#define iMAX_24BIT		0x007fffff

#define DEFAULT_SRATE 22050

char LastChar(char *str);
int countChar(const char *str, char const c, int *maxLength, int *minLength);
double getmax(double *x, int len);
void ClipUpDown(double *xd, int len, double threshod, double depth);
unsigned int _gcd(unsigned int u, unsigned int v);
void filter(int nTabs, double *num, double *den, int length, int *in, int *out);
void filter(int nTabs, double *num, int length, int *in, int *out);
void filter(int nTabs, double *num, double *den, int length, double *in, double *out);
void filter(int nTabs, double *num, int length, double *in, double *out);
void filter(int nTabs, double *num, int length, double *data);
void filter(int nTabs, double *num, double *den, int length, double *data);
int remez(double h[], int numtaps, int numband, double bands[], double des[], double weight[], int type);

bool IsMagnitudeClipped(double *xx, int len, double threshold);

EXP_CS double LeveldBrms (const char *wave, double *level2, char *errstr);
EXP_CS double LeveldBpeak (const char *wave, double *level2, char *errstr);
EXP_CS int Rescale(const char *waveOut, const char *waveIn, double scaleFactor_dB, char *errstr); // 1 for success, -1 for clipping, 0 for failure, 
EXP_CS int ReadwavInfo(const char *wave, int *fs, int *nChan, int *lengthPerChan, char *errstr);
EXP_CS int MakeMono(const char *waveIn, const char *waveOut,char *errstr);
EXP_CS int WavCat(const char *waveOrg, const char *wave2Append, double delay, char *errstr);

//from ellf.c
extern "C" int design_iir(double *num, double *den, int fs, int kind, int type, int n, double *freqs, double dbr /*rippledB*/, double dbd /*stopEdgeFreqORattenDB*/);
