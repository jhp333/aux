#include "aceset_internal.h"
#include <stdio.h>


#define SCANITEM(NAME,FORMAT,VAR) if (sscanfINI(errStr, fname, NAME,FORMAT,&VAR)<=0) return 0;
#define SCANITEM2(NAME,FORMAT,VAR,VAR2) if (sscanfINI(errStr, fname, NAME,FORMAT,&VAR,&VAR2)<=0) return 0;


