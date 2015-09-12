#pragma once
#include "WndDlg.h"

using namespace std;

#define ACESET_API __declspec(dllexport)
#include <limits>

#define ACESET_QNAN numeric_limits<double>::quiet_NaN()

#define W_ACE	"wACE"
#define is_wACE(prm)	(prm->rate == -1)

ACESET_API void OpenMAP(HWND hACEDlg, const char *mapfile, InterfaceType procEnum);
ACESET_API HWND ACEChildDlg (HWND hParent, POINT position, CParamACE * param, char *errstr);
ACESET_API CParamACE * GetParam(HWND hACEDlg, char *errstr);
ACESET_API double GetLevel(HWND hACEDlg);
ACESET_API LRESULT ClickOpenMAP(HWND hACEDlg);
ACESET_API LRESULT SetMax(HWND hACEDlg, int max);
ACESET_API LRESULT SetQ(HWND hACEDlg, int Q);
ACESET_API LRESULT ToggleElectrode(HWND hACEDlg, int electrode);
ACESET_API LRESULT SelectElectrode(HWND hACEDlg, int electrode, int onoff);
ACESET_API LRESULT SelectFreqLimits(HWND hACEDlg, double freq1, double freq2);
ACESET_API LRESULT GlobalAdjT(HWND hACEDlg, bool increment);
ACESET_API LRESULT GlobalAdjC(HWND hACEDlg, bool increment);
ACESET_API LRESULT GlobalAdjGain(HWND hACEDlg, bool increment);


#define min_cic4_pw 12.
#define min_cic4_pg 6.
