#include "sigplus_internal.h"

char LastChar(char *str)
{
	return str[strlen(str)-1];
}

void ClipUpDown(double *xd, int len, double threshold, double depth)
{
	int i;

	for (i=0; i<len; i++)
		if (xd[i]>=threshold)
			xd[i] = 1.;
		else
			xd[i] = depth;
}

bool IsMagnitudeClipped(double *xx, int len, double threshold)
{
	// threshold must be positive!!! such as 127.
	for (int i=0; i<len; i++)
		if (xx[i] > threshold)
		{
			xx[i] = threshold;
			return true;
		}
		else if (xx[i] < (-threshold-1))
		{
			xx[i] = -threshold;
			return true;
		}
	return false;
}

unsigned int _gcd(unsigned int u, unsigned int v) 
{
	unsigned int k = 0;
	if (u == 0)
	 return v;
	if (v == 0)
	 return u;
	while ((u & 1) == 0  &&  (v & 1) == 0) { /* while both u and v are even */
	 u >>= 1;   /* shift u right, dividing it by 2 */
	 v >>= 1;   /* shift v right, dividing it by 2 */
	 k++;       /* add a power of 2 to the final result */
	}
	/* At this point either u or v (or both) is odd */
	do {
	 if ((u & 1) == 0)      /* if u is even */
		 u >>= 1;           /* divide u by 2 */
	 else if ((v & 1) == 0) /* else if v is even */
		 v >>= 1;           /* divide v by 2 */
	 else if (u >= v)       /* u and v are both odd */
		 u = (u-v) >> 1;
	 else                   /* u and v both odd, v > u */
		 v = (v-u) >> 1;
	} while (u > 0);
	return v << k;  /* returns v * 2^k */
}

double getmax(double *x, int len)
{
	int id;
	double localMax(0);
	for (int i=0; i<len; i++)
	{	if (x[i] > localMax)
		{localMax = x[i];			id=i; }
	}
	return x[id];
}