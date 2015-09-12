#define FER(x) {\
char linestrysy[256], estrys[128]; int ecody = (x); \
if (ecody<0) { \
	flyErrMsg(ecody, estrys); \
    wsprintf (linestrysy, "Error occurred at "#x", code=%d", ecody); \
    MessageBox (linestrysy, estrys, MB_OK);} }

#define FER0(x) {\
char linestrysy[256], estrys[128]; int ecody = (x); \
if (ecody<0) { \
	flyErrMsg(ecody, estrys); \
    wsprintf (linestrysy, "Error occurred at "#x", code=%d", ecody); \
    MessageBox (hDlg, linestrysy, estrys, MB_OK); return ecody;} }

#define BOOLCHECK(x) {\
char linestrysy[256], estrys[128]; i = (x); \
if (!i) { \
	GetLastErrorStr(estrys); \
    wsprintf (linestrysy, "Error occurred at "#x", code=%d, nOutputCharsBeforeNote=%d,dww=%d", i,nOutputCharsBeforeNote,dww); \
    MessageBox (hDlg, linestrysy, estrys, MB_OK);} }
	