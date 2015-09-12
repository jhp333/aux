#include "stdafx.h"
#include "aceproc.h"

int FFT_band_bins(int *width, int nBands)
{
	// width MUST be allocated with length at least 22
	// if nBands is greater than 22, the number of bands is still 22 but freq table lower than 6 is used

	int i;
	memset((void*)width, 0, sizeof(int)*22);
	switch(nBands)
	{
	case 1:
		width[0]=62;
		return 1;
	case 2:
		width[0]= 7; width[1]=55;
		return 2;
	case 3:
		width[0]= 7; width[1]= 15;  width[2]=40;
		return 3;
	case 4:
		width[0]= 7; width[1]= 8;  width[2]=16; width[3]=31; 
		return 4;
	case 5:
		width[0]= 3; width[1]= 4; width[2]= 8;  width[3]=16; width[4]=31; 
		return 5;
	case 6:
		width[0]= 3; width[1]= 4; width[2]= 6;  width[3]=9; width[4]=15; width[5]=25; 
		return 6;
	case 7:
		width[0]= 3; width[1]= 4; width[2]= 4; width[3]= 6;  width[4]=9; width[5]=14; width[6]=22; 
		return 7;
	case 8:
		width[0]= 2; width[1]= 2; width[2]= 3; width[3]= 4; width[4]= 6;  width[5]=9; width[6]=14; width[7]=22; 
		return 8;
	case 9:
		width[0]= 2; width[1]= 2; width[2]= 3; width[3]= 3; width[4]= 5; width[5]= 7; width[6]=9; width[7]=13; width[8]=18; 
		return 9;
	case 10:
		width[0]= 2; width[1]= 2; width[2]= 3; width[3]= 3; width[4]= 4; width[5]= 5; width[6]= 7; width[7]=9; width[8]=12; width[9]=15; 
		return 10;
	case 11:
		width[0]=1;
		for (i=1; i<=3; i++) width[i]=2;
		width[i++]=3;
		width[i++]=4;
		width[i++]=5;
		width[i++]=7;
		width[i++]=9;
		width[i++]=12;
		width[i++]=15;
		break;
	case 12:
		width[0]=1;
		for (i=1; i<=4; i++) width[i]=2;
		width[i++]=3;
		width[i++]=4;
		width[i++]=5;
		width[i++]=7;
		width[i++]=9;
		width[i++]=11;
		width[i++]=14;
		break;
	case 13:
		width[0]=1;
		for (i=1; i<=4; i++) width[i]=2;
		for (; i<=6; i++) width[i]=3;
		width[i++]=4;
		width[i++]=5;
		width[i++]=7;
		width[i++]=8;
		width[i++]=10;
		width[i++]=13;
		break;
	case 14:
		width[0]=1;
		for (i=1; i<=5; i++) width[i]=2;
		for (; i<=7; i++) width[i]=3;
		width[i++]=4;
		width[i++]=5;
		width[i++]=6;
		width[i++]=8;
		width[i++]=9;
		width[i++]=13;
		break;
	case 15:
		for (i=0; i<=2; i++) width[i]=1;
		for (; i<=6; i++) width[i]=2;
		for (; i<=8; i++) width[i]=3;
		width[i++]=4;
		width[i++]=5;
		width[i++]=6;
		width[i++]=8;
		width[i++]=9;
		width[i++]=13;
		break;

	case 16:
		for (i=0; i<=2; i++) width[i]=1;
		for (; i<=7; i++) width[i]=2;
		for (; i<=8; i++) width[i]=3;
		for (; i<=10; i++) width[i]=4;
		width[i++]=5;
		width[i++]=6;
		width[i++]=7;
		width[i++]=9;
		width[i++]=11;
		break;

	case 17:
		for (i=0; i<=2; i++) width[i]=1;
		for (; i<=7; i++) width[i]=2;
		for (; i<= 9; i++) width[i]=3;
		for (; i<=11; i++) width[i]=4;
		width[i++]=5;
		width[i++]=6;
		width[i++]=7;
		width[i++]=8;
		width[i++]=9;
		break;

	case 18:
		for (i=0; i<=4; i++) width[i]=1;
		for (; i<=8; i++) width[i]=2;
		for (; i<=10; i++) width[i]=3;
		for (; i<=12; i++) width[i]=4;
		width[i++]=5;
		width[i++]=6;
		width[i++]=7;
		width[i++]=8;
		width[i++]=9;
		break;

	case 19:
		for (i=0; i<=6; i++) width[i]=1;
		for (; i<=9; i++) width[i]=2;
		for (; i<=11; i++) width[i]=3;
		for (; i<=13; i++) width[i]=4;
		width[i++]=5;
		width[i++]=6;
		width[i++]=7;
		width[i++]=8;
		width[i++]=9;
		break;

	case 20:
		for (i=0; i<=7; i++) width[i]=1;
		for (; i<=10; i++) width[i]=2;
		for (; i<=12; i++) width[i]=3;
		for (; i<=14; i++) width[i]=4;
		width[i++]=5;
		width[i++]=6;
		width[i++]=7;
		width[i++]=8;
		width[i++]=8;
		break;

	case 21:
		for (i=0; i<=7; i++) width[i]=1;
		for (; i<=11; i++) width[i]=2;
		for (; i<=13; i++) width[i]=3;
		for (; i<=15; i++) width[i]=4;
		width[i++]=5;
		width[i++]=6;
		width[i++]=6;
		width[i++]=7;
		width[i++]=8;
		break;

	case 22:
		for (i=0; i<=8; i++) width[i]=1;
		for (; i<=12; i++) width[i]=2;
		for (; i<=14; i++) width[i]=3;
		for (; i<=16; i++) width[i]=4;
		for (; i<=18; i++) width[i]=5;
		width[i++]=6;
		width[i++]=7;
		width[i++]=8;
		break;
	default:
		return -1;
		}
	return i-1;
}

