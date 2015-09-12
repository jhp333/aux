/* Version mxace1.dll */
#include <windows.h>
#include "stdstring.h"

#ifdef __cplusplus
#define ACEPROC_EXP extern "C" __declspec (dllexport)
#else
#define ACEPROC_EXP __declspec (dllexport)
#endif

#ifdef __cplusplus /* Declare the class only for CPP */
#define EXPO __declspec (dllexport)

/* Class declaration --CParamACE */

class CParamACE
{
public:
	char subj[16];
	char mode[8];
	double pw[22];
	double pg;
	double rate;
	double c[22];
	double t[22];
	bool selected[22]; // 1 if the electrode in the index is selected
	double freqsL[22], freqsU[22]; // frequency boundaries (lower, upper)
	int numBands;
	int numMax;
	double level;
	double gains[22]; // now in dB
	int Q;
	CParamACE * ptParam2; // This is the pointer of param2 (necessary to port out)
	DWORD dwReserved; // This is extra pointer used for pt to conditioner structure
	bool rms_scaled; // if set, the level indicates the rms, if not it indicates the full-scale calibrated level.
	double gaterate;
	char *addmlines;
	int lenAddmlines;

	EXPO CParamACE();
	virtual ~CParamACE();
	EXPO virtual int mode2enum (char * remainderStr);
	EXPO virtual int GetRE(int ae);
};

#endif /* end---Declare the class only for CPP */

/* Structure declaration -- ACESEQ */

typedef struct
{
	int length;
	double *ae;
	double *mode;
	int lenMode;
	double *cl;
	double *pw;
	int lenPW;
	double *pg;
	int lenPG;
	double *period;
	int lenPeriod;
} ACESEQ;

/* Functions declarations */

ACEPROC_EXP int GateProcess(ACESEQ * seqOut, const char *procStr, const char *wavname, const char *seqFilename, CParamACE *pm, char **errstr, char **warnstr);
ACEPROC_EXP int GetCutoffFreqs(int nBands, double *freq_limits, double *freqs);
ACEPROC_EXP int FixCRCR(char *str);
ACEPROC_EXP int InitMatlabLib(const char* libname, char **errstr);
ACEPROC_EXP void CloseMatlabLib();
ACEPROC_EXP CParamACE* newCParamACE();

