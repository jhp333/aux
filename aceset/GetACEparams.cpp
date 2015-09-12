#include "aceplayer.h"
#include "aceplayerdlg.h"

int CACEplayerDlg::GetACEparams(char *errstr)
{
	int res, i, chek1, chek2, atten1, atten2;

	//check how a negative value is handled.
	atten1 = GetDlgItemInt (IDC_ATTEN_A, &chek1, 0);
	atten2 = GetDlgItemInt (IDC_ATTEN_B, &chek2, 0);
	if (!chek1 && !chek2) { strcpy(errstr, "Both cannot be full attenuation"); return 0;}
	if (chek1*chek2) // both have attenuation
	{
		if(!hACEobj[0].Screen2Param()) {strcpy(errstr, hACEobj[0].errstr); return 0;}
		if(!hACEobj[1].Screen2Param()) {strcpy(errstr, hACEobj[1].errstr); return 0;}
		param = hACEobj[0].param;
		param2 = hACEobj[1].param;
		param->ptParam2 = param2;
		param->ptParam2->ptParam2 = NULL;

		for (i=0; i<22; i++)
		{
			param->t[i] -= atten1;
			param->c[i] -= atten1;
			param->ptParam2->t[i] -= atten2; 
			param->ptParam2->c[i] -= atten2; 
		}
		res=2;
	}
	else //only one is used
	{
		if (!chek2) // param A is used.
		{
			if(!hACEobj[0].Screen2Param()) {strcpy(errstr,hACEobj[0].errstr); return 0;}
			param = hACEobj[0].param;
			param->ptParam2 = NULL;
			for (i=0; i<22; i++)
			{
				param->t[i] -= atten1;
				param->c[i] -= atten1;
			}
		}
		else
		{
			if(!hACEobj[1].Screen2Param()) {strcpy(errstr,hACEobj[1].errstr); return 0;}
			param = hACEobj[1].param;
			param->ptParam2 = NULL;
			for (i=0; i<22; i++)
			{
				param->t[i] -= atten2;
				param->c[i] -= atten2;
			}
		}
		res=1;
	}
	errstr[0]=0;
	return res;
}