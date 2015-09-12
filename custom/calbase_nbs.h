#include "calbase.h"

class cal_cisim : public calbase
{
public:
	cal_cisim(void) {name = "cisim";}
	virtual ~cal_cisim(void)	{}
	CEqSig& calc(list<CEqSig *> arg);
};

