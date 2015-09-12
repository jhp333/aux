#ifndef LINK_STATIC_PRECONT
#define EXP_PRECONT __declspec (dllexport)
#else 
#define EXP_PRECONT
#endif

#define IDC_MSG_LOG4CONT                1040
#define IDD_LOG                         59901

EXP_PRECONT int PreparePresent(const char *_NodeName, const char *wavname, int acoustPlay, double level, char *prepInfo, size_t LenprepInfo, char *errstr);