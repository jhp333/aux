/*
 
This is the source file of BRIC library, which provides a high-level tool
for a NIC programmer to construct a complex stimulus easily. 


BRIC v2.1------Sep 2. 2005
Changes: mod.env.i must be freed and re-allocated by the application whenever mod.cycle is changing.
         (good news: application does not have any other burden of memory management than that).


For detailed instruction, refer to the following documents:

  BRIC Quick Start - Rev B.doc
  BRIC v2.0 User's Guide & Reference .doc

  This is developed based on NIC1.0. (NIC1.0 is required to generate the library)

  This should not be disseminated to 3rd party without the consent from the author or the assignee.

  Copyright: Cochlear Americas
  Author: Bomjun Kwon
  Date : November 24, 2003

*/

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include "nic.h"
#include "bric.h"

#define CHECKNIC1(X) if (X!=Call_Succeeded) { stimulusDelete(fr); sequenceCommandDelete(unitSeq); sequenceCommandDelete(stimComm); return -5000-lastServicesError(); }
#define CHECKNIC2(X) if (X!=Call_Succeeded) { stimulusDelete(fr); sequenceCommandDelete(stimComm); return -5000-lastServicesError(); }
#define CHECKNIC3(X) if (X!=Call_Succeeded) return -3000-lastServicesError();

#define NIC_ERROR_CHECK(e)  \
{ \
    if ( e != 1 )/*if failed*/ \
    {     \
	   switch (GetLastNICError()) \
	   {\
	   case 0: \
			return BR_ERR_NIC_0;   case 1: \
			return BR_ERR_NIC_1;\
	   case 2: \
			return BR_ERR_NIC_2;\
	   case 3: \
			return BR_ERR_NIC_3;\
	   case 4: \
			return BR_ERR_NIC_4;\
	   case 5: return BR_ERR_NIC_5;\
	   case 6: return BR_ERR_NIC_6;\
	   case 7: return BR_ERR_NIC_7;\
	   case 8: return BR_ERR_NIC_8;\
	   case 9: return BR_ERR_NIC_9;\
	   case 10: return BR_ERR_NIC_10;\
	   case 11: return BR_ERR_NIC_11;\
	   case 12: return BR_ERR_NIC_12;\
	   case 13: return BR_ERR_NIC_13;\
	   case 14: return BR_ERR_NIC_14;\
	   case 15: return BR_ERR_NIC_15;\
	   }\
    }\
}
static int powerE1, powerE2;

#define CHECK_ERR_WRITE(x) { if (x==0) return BR_ERR_CANNOT_WRITE;}
#define CHECK_ERR_READ(x) { if (x==0) return BR_ERR_CANNOT_READ;}

unsigned char ElectrodeTo_FiveBits (unsigned char c)
{
	if ( c >= 100 )
		c -= 77;
	return c;
}

unsigned char FiveBitsTo_Electrode (unsigned char c)
{
	if ( c > 0x16)
		c += 77;
	return c;
}

unsigned char GetTheDigit(int number)
{
	// ASSUMES only number is greater than 0
	int expo(1);
	for (int i=0; i<9; i++)
	{
		if ( number < expo ) return (unsigned char)i;
		expo *= 10;
	}
	return (unsigned char)0;
}
__int64 encodePeriod17bit(double period)
{
	// encode the period with 17-bit in the following format: 
	// _ . _ _ _ x exp(_)
	int iPeriod = (int)(period+.5);
	unsigned char expo = GetTheDigit(iPeriod);
	for (int j=0; j<expo-4; j++) iPeriod /= 10;
	__int64 y = iPeriod * 10 + expo;
	return y;
}

double exbase2double(unsigned int x)
{
	unsigned int y = x/10;
	int expo = x - y*10;
	double drest = y / 1000.;
	for (int i=0; i<expo-1; i++) drest *= 10.;
	return drest;
}

double getPeriod(__int64 x)
{
	unsigned int temp(0); 
	double res;
	if (x & 0x20000) // if this is long period
	{
		temp = (unsigned int) (x & 0x1ffff);
		res = exbase2double(temp);
	}
	else
	{
		temp = (unsigned int) (x & 0x1ffff);
		res = (double) temp / 5. ;
	}
	return res ;
}

BRIC::BRIC()
:nowPt(0)
{
}

BRIC::~BRIC()
{
}

const BRIC& BRIC::operator=(const BRIC& rhs)
{
	if (this != &rhs)
	{
		ae=rhs.ae;
		re=rhs.re;
		cl=rhs.cl;
		pam.period=rhs.pam.period;
		pam.pw=rhs.pam.pw;
		pam.pg=rhs.pam.pg;
		repeat=rhs.repeat;
		tmark=rhs.tmark;
	}
	return *this;
}


char* GetBRErrorString (char* errStr, int i)
{
	// Memory space at least 55 bytes for errStr must have been allocated.

	switch(i)
	{
	case 0:
		strcpy(errStr, "No error.");
		break;
	case -1:
		strcpy(errStr, "BRC file not found (or sharing violation).");
		break;
	case -2:
		strcpy(errStr, "BRC file already exists.");
		break;
	case -3:
		strcpy(errStr, "Unable to close BRC file");
		break;
	case -4:
		strcpy(errStr, "Unable to write into BRC file");
		break;
	case -5:
		strcpy(errStr, "Unable to read BRC file");
		break;
	case -6:
		strcpy(errStr, "Too many brics for BRC file");
		break;
	case -7:
		strcpy(errStr, "Too many, zero or negative number of brics");
		break;
	case -10:
		strcpy(errStr, "Output pointer is not prepared.");
		break;
	case -11:
		strcpy(errStr, "Invalid index to remove.");
		break;
	case -12:
		strcpy(errStr, "Not a valid BRC file format");
		break;
	case -13:
		strcpy(errStr, "Internal error. Contact the developer.");
		break;
	case -20:
		strcpy(errStr, "Sequence occupying memory beyond Sprint's capacity.");
		break;
	case -21:
		strcpy(errStr, "Time conflict between frames");
		break;
	case -30:
		strcpy(errStr, "Invalid electrode");
		break;
	case -31:
		strcpy(errStr, "Invalid phasewidth");
		break;
	case -32:
		strcpy(errStr, "Invalid phasegap");
		break;
	case -33:
		strcpy(errStr, "Invalid period");
		break;
	case -34:
		strcpy(errStr, "Invalid current level");
		break;
	case -35:
		strcpy(errStr, "Invalid tmark");
		break;
	case -37:
		strcpy(errStr, "Invalid repeat");
		break;
	case -38:
		strcpy(errStr, "Invalid delay");
		break;
	case -100:
		strcpy(errStr, "(NIC Error) No_Error");
		break;
	case -101:
		strcpy(errStr, "(NIC Error) Null_Pointer_Function_Parameter");
		break;
	case -102:
		strcpy(errStr, "(NIC Error) Precondition_Failure");
		break;
	case -103:
		strcpy(errStr, "(NIC Error) Internal_Error");
		break;
	case -104:
		strcpy(errStr, "(NIC Error) RF_Frequency_Out_Of_Range");
		break;
	case -105:
		strcpy(errStr, "(NIC Error) Transmit_Protocol_Out_Of_Range");
		break;
	case -106:
		strcpy(errStr, "(NIC Error) Implant_IC_Out_Of_Range");
		break;
	case -107:
		strcpy(errStr, "(NIC Error) Implant_Not_Selected");
		break;
	case -108:
		strcpy(errStr, "(NIC Error) Wrong_Implant_Type");
		break;
	case -109:
		strcpy(errStr, "(NIC Error) Current_Out_Of_Range");
		break;
	case -110:
		strcpy(errStr, "(NIC Error) Active_Electrode_Out_Of_Range");
		break;
	case -111:
		strcpy(errStr, "(NIC Error) Reference_Electrode_Out_Of_Range");
		break;
	case -112:
		strcpy(errStr, "(NIC Error) Stimulation_Mode_Out_Of_Range");
		break;
	case -113:
		strcpy(errStr, "(NIC Error) HardwareUnavailable");
		break;
	case -114:
		strcpy(errStr, "(NIC Error) SpeechProcessorNotPresent");
		break;
	case -115:
		strcpy(errStr, "(NIC Error) WrongVersionSpeechProcessorSoftware");
		break;
	case -9999:
		strcpy(errStr, "Unknown error");
		break;
	default:
		strcpy(errStr, "Function operated successfully. Analyze its return value.");
		break;
	}
	return errStr;
}

void FindNextTmark (BRIC *br, int Nbrics, double *tmark1, int *ind1, double *tmark2, int *ind2)
{
/* Objective: 
	 To find the time mark (tmark) and its index of the next immediately occuring frame 
	 in the given BRIC array, br. (They will be tmark1 and ind1, respectively)

	 In addition, the following frame (after the immediate frame) will be obtained.
	              (tmark2 and ind2)

   For example, if there are two brics in the br, and given the following,

   br[0].tmark = 0.;
   br[0].pam.period = 1000.;
   br[0].repeat = 5;
   .....(other params)
   br[1].tmark = 1500;
   .....(other params)

   tmark1, ind1, tmark2, and ind2 will be, 
   0., 0, 1000., 0, respectively.

*/

	int i, ind;
	double tmark = 1.e10;
	double temp;
	for (i=0; i<Nbrics; i++)
	{
		if (br[i].tmark<tmark) // if current tmark is less than the previous tmark,
		{ // update it
			tmark = br[i].tmark; 
			ind=i;
		}	
	}
	*tmark1=tmark;
	*ind1=ind;

	tmark = 1.e10;

	// Next, remove tmark1 to get second next tmark and repeat the above step
	for (i=0; i<Nbrics ; i++)
	{
		if (i!=*ind1) // if i is not ind1, do it normally
			temp = br[i].tmark;
		else // if i is ind1, i.e., the bric identified above step,
			// then it needs to determine whether this bric ends after tmark1 or not
			
			// if the bric did not end at tmark1,  
			// the tmark of next pulse will be chosen for comparison candidate.
			if (br[i].repeat>1) 
				temp = br[i].tmark + br[i].pam.period;
			else // if not, put the maximum number so that it can't be chosen.
				temp = 1.e10;

		if (temp<tmark) // same as above, 
			{ 
				tmark = temp; 
				ind=i;
			}
	}
	*tmark2=tmark;
	*ind2=ind;
}

int IsValidBRIC (BRIC br[], int num)
{
	int i;

	for (i=0; i<num; i++)
	{
		if (br[i].ae*(br[i].ae-22)*(br[i].ae-100)*(br[i].ae-103)>0)
			return BR_ERR_INVALID_ELECTRODE;
		if (br[i].re*(br[i].re-22)*(br[i].re-100)*(br[i].re-103)>0)
			return BR_ERR_INVALID_ELECTRODE;
		if ((br[i].pam.pw-MIN_PHASEWIDTH)*(br[i].pam.pw-MAX_PHASEWIDTH)>0)
			return BR_ERR_INVALID_PHASEWIDTH;
		if ((br[i].pam.pg-MIN_PHASEGAP)*(br[i].pam.pg-MAX_PHASEGAP)>0)
			return BR_ERR_INVALID_PHASEGAP;
		if ((br[i].pam.period-MIN_PERIOD)*(br[i].pam.period-MAX_PERIOD)>0)
			return BR_ERR_INVALID_PERIOD;
		if (br[i].cl*(br[i].cl-255)>0)
			return BR_ERR_INVALID_CL;
		if (br[i].tmark<0)
			return BR_ERR_INVALID_TMARK;
		if (br[i].repeat==0)
			return BR_ERR_INVALID_REPEAT;
	}
	return 1;
}

int RemoveItemInternal (BRIC br[], int *num, int k) 
{
	// remove the item at the k-th index and 
	int i;
	if (br==NULL)
		return BR_ERR_NULL_POINTER;
	if (*num <= 0)
		return BR_ERR_INVALID_NUMBRICS;
	if (k > *num || k < 0)
		return BR_ERR_INVALID_INDEX;
	for (i=k; i<*num-1; i++)
		br[i] = br[i+1];
	br[*num-1].ae = br[*num-1].cl = br[*num-1].re = '\0';
	br[*num-1].repeat = 0;
	br[*num-1].pam.period = br[*num-1].pam.pg = br[*num-1].pam.pw = br[*num-1].tmark = 0.;
	--*num;
	return BR_NO_ERR;
}

void pulseParam2int64 (__int64 *x, int ae, int re, unsigned short uPw, unsigned short uPg, int cl, double period)
{
	// pw and pg are RF counts, but period is double value.
	__int64 y;
	int iPeriod;
	*x = (__int64) ElectrodeTo_FiveBits((unsigned char)ae) ;
	*x <<= 59 ;
	y = (__int64) ElectrodeTo_FiveBits((unsigned char)re) ;
	y <<= 54;
	*x |= y;
	y = uPw;
	y <<= 40;
	*x |= y;
	y = uPg;
	y <<= 26;
	*x |= y;
	y = cl ;
	y <<= 18;
	*x |= y;

	if ( period <= 26214.4) // 26214.4---65536*2 (the maximum a 17-bit can handle) divided by 5
	{
		iPeriod = (int)(period * 5.);
		*x += iPeriod;
	}
	else
	{ 
		y = encodePeriod17bit(period);
		*x += y ; 
		*x += 131072; // 65536* 2, set the 18th bit (to mark a long period)
	}
}

int timeSlide (BRIC br[], int *m, double amount2Slide)
{
	// assumption in the context where this is called: 
	// amount2Slide is tmark of the bric with immediately following pulse

	// Returns 0 if one bric is removed (if *m decreases)
	// returns the number of brics (*m) if no bric is removed (i.e., if *m did not change)
	// returns -1 if tmark is negative or repeat is zero or negative

	int i, id2Reduce, id2Remove;

	// Just a safety net
	for (i=0; i<*m; i++)
		if (br[i].tmark <0 || br[i].repeat<=0)
			return -1;

	// Obtain the index of br to reduce its repeat counter
	id2Reduce= -1;
	for (i=0; i<*m && id2Reduce==-1; i++)
	{
		if (br[i].tmark < amount2Slide)
			id2Reduce = i;
	}

	// Slide in time
	for (i=0; i<*m; i++)
	{
		if (i==id2Reduce)
		{
			--br[i].repeat;
			br[i].tmark += br[i].pam.period - amount2Slide;
		}
		else
			br[i].tmark -= amount2Slide;
	}

	// Remove br if repeat counter is zero.
	id2Remove = -1;
	for (i=0; i<*m && id2Remove==-1; i++)
	{
		if (br[i].repeat ==0)
			id2Remove = i;
	}
	RemoveItemInternal (br, m, id2Remove);
	if (id2Remove != -1) return 0;
	else			 	 return *m;
}

int FindFirstTwoTmarks (BRIC br[], int num, int *id1, int *id2)
{
	if (num<2) return 0;
	double temp(1.e15);
	//Find the first tmark
	for (int i=0; i<num; i++)
	{
		if (br[i].tmark<temp)
		{
			temp = br[i].tmark; 
			*id1=i;
		}
	}
	//Find the second tmark
	temp = 1.e15;
	for (int i=0; i<num; i++)
	{
		if (i!= *id1)
			if (br[i].tmark<temp)
			{
				temp = br[i].tmark; 
				*id2=i;
			}
	}
	return 1;
}

int timeSlide2 (BRIC br[], int *num, double amount2Slide)
{
	int *ind2discard = new int [*num];
	int i, j(0), cum(0), diff;
	for (i=0; i<*num; i++)
	{
		diff = (int)ceil((amount2Slide-br[i].tmark)/br[i].pam.period);
		br[i].tmark -= amount2Slide;
		if (br[i].tmark<0)  
		{
			diff = min (br[i].repeat, diff); // repeat cannot be negative
			br[i].repeat -= diff;
			br[i].tmark += diff * br[i].pam.period;
			br[i].nowPt += diff;
		}
		if (br[i].repeat<=0)  ind2discard[cum++]=i;
	}
	for (i=0; i<cum; i++)
		RemoveItem(br, num, ind2discard[i]);
	delete[] ind2discard;
	return *num;
}

int BricTimingCheck (BRIC brIn[], int num, double *confTmarks, int *lenConfTmarks)
{
	int i, res, id1, id2;
	BRIC *br;
	double gap, tmarkNow(0);
	bool ch(false);

	*lenConfTmarks = 0;

	br = new BRIC[num];

	for (i=0; i<num; i++)
		br[i] = brIn[i];

	while (num > 1)
	{
		FindFirstTwoTmarks(br, num, &id1, &id2);
		double tmark = 0;//GetLastTmarkB4ID2(br, num, id1, id2);
		while (br[id2].tmark == tmark) // if synch'ed, make it slightly break the synch
		{
			br[id1].tmark -= .01; 
			FindFirstTwoTmarks(br, num, &id1, &id2);
			tmark = 0;//GetLastTmarkB4ID2(br, num, id1, id2);
			ch = true;
		}
		gap = br[id2].tmark - tmark;
		tmarkNow += br[id2].tmark;
		if (gap < 2*br[id1].pam.pw + br[id1].pam.pg + MINIMUM_SHORTING)
		{
			confTmarks[(*lenConfTmarks)++] = tmarkNow;

		}
		res=timeSlide (br, &num, br[id2].tmark);
		if (ch) { br[id1].tmark += .01; ch = false;}
	}
	delete[] br;
	return 1; // success
}



void SortBR (BRIC *br, int m)
{
  int i, j ;
  BRIC temp;

  for (i = (m - 1); i >= 0; i--)
  {
    for (j = 1; j <= i; j++)
    {
      if (br[j-1].tmark > br[j].tmark)
      {
        temp = br[j-1];
        br[j-1] = br[j];
        br[j] = temp;
      }
    }
  }
}

int RemoveItem (BRIC br[], int *num, int k) 
{
	// remove the item at the k-th index and 
	int i;
	if (br==NULL)
		return BR_ERR_NULL_POINTER;
	if (*num <= 0)
		return BR_ERR_INVALID_NUMBRICS;
	if (k > *num || k < 0)
		return BR_ERR_INVALID_INDEX;
	for (i=k; i<*num-1; i++)
		br[i] = br[i+1];
	br[*num-1].ae = br[*num-1].cl = br[*num-1].re = '\0';
	br[*num-1].repeat = 0;
	br[*num-1].pam.period = br[*num-1].pam.pg = br[*num-1].pam.pw = br[*num-1].tmark = 0.;
	--*num;
	return BR_NO_ERR;
}

float getBricVersion ()
{
	return BRICVERSION;
}

void FindNext (BRIC *br, int nBrics, int &fstBric, int &secBric) 
// Get the indices of the first two brics in time
// for the same tmarks, the indices are in the order of br input
{
	if (nBrics==1) 
	{ fstBric=0; secBric=-1; return;}
	
	double temp(1.e15);
	//Find the bric that occurs the first
	for (int i=0; i<nBrics; i++)
	{
		if (br[i].tmark<temp)
		{
			temp = br[i].tmark; 
			fstBric=i;
		}
	}
	//Find the bric that occurs the second
	temp = 1.e15;
	for (int i=0; i<nBrics; i++)
	{
		if (i!= fstBric)
			if (br[i].tmark<temp)
			{
				temp = br[i].tmark; 
				secBric=i;
			}
	}
}

int Bric2Int64Array (__int64 *outArray, BRIC *brIn, int Nbrics)
{
/* Objective: Convert a bric array into a 64-bit integer
  */

	int i, ae, re, Nframes = 0;
	__int64 x;
	unsigned y;
	unsigned short uPw, uPg ; // rf count
	double dPeriod, toShift;
	BRIC *br;

	int fstBric, secBric;

	br = new BRIC[Nbrics];

	for (i=0; i<Nbrics; i++)
		br[i] = brIn[i];

	//double out(0.), tmark(0);
	//while (out>=0)
	//{
	//	out = NextPulse(br, num, tmark, y);
	//	tmark = out+.0001;
	//}

	while (Nbrics > 0)
	{
		FindNext (br, Nbrics, fstBric, secBric);

		ae = ElectrodeTo_FiveBits( br[fstBric].ae) ;
		re = ElectrodeTo_FiveBits( br[fstBric].re) ;

		uPw = (unsigned short) (br[fstBric].pam.pw * 5.); // rf counts
		uPg = (unsigned short) (br[fstBric].pam.pg * 5.); // rf counts
		y = br[fstBric].cl ;

		
		if (Nbrics==1) // period is always the same.
		{
			dPeriod =  br[0].pam.period;
			toShift = br[0].pam.period;
		}
		else
		{
			double nextFstTmark = br[fstBric].tmark+br[fstBric].pam.period;
			//while (br[secBric].tmark == nextFstTmark) // if synch'ed, make it slightly break the synch
			//{
			//	br[secBric].tmark += .01; 
			//	FindNext (br, Nbrics, fstBric, secBric);
			//}
			
			if (br[secBric].tmark < nextFstTmark) // two brics are interleaved
			{
				dPeriod = br[secBric].tmark - br[fstBric].tmark;
				toShift = br[secBric].tmark;
			}
			else if (br[fstBric].repeat >1) // two brics are not interleaved and the first bric has more than one 
			{
				toShift = dPeriod = br[fstBric].pam.period;
			}
			else // two brics not interleaved and the first bric has one left
			{
				toShift = dPeriod = br[secBric].tmark;
			}
		}

		pulseParam2int64 (&x, ae, re, uPw, uPg, y, dPeriod);
		outArray[Nframes++] = x;

		if (dPeriod==0) // the first two pulses are sync'ed
		{
			if (br[fstBric].repeat == 1) RemoveItem(br, &Nbrics, fstBric);
			else { br[fstBric].repeat--; br[fstBric].tmark += br[fstBric].pam.period; }
		}
		timeSlide2 (br, &Nbrics, toShift);
	}
	delete[] br;
	return Nframes;
}

int IsPeriodic (__int64 *array, int Nframes, int period)
{
	// Inspect whether array is periodic with period. 

	// return value: how many cycles will be counted with the given period.
	// For example, if the array is 1000 pulses pulse train, and period is one,
	// then this will return 1000.
	// If the array is not periodic with period, one will be returned.

	int i, beginId4ThisLoop, completedPeriods;
	__int64 x, y;

	beginId4ThisLoop = 0;
	completedPeriods = 1;

	do
	{
		beginId4ThisLoop = (completedPeriods-1)*period;
		for (i=0; i<period; i++)
			if (array[beginId4ThisLoop+i] != array[beginId4ThisLoop+i+period])
				return completedPeriods;
		completedPeriods++;
	}
	while ( beginId4ThisLoop+i < Nframes-2*period);

	if (period>1)
	{
	// For the last loop to compare, the period of the last one needs not be the same
		beginId4ThisLoop = (completedPeriods-1)*period;
		for (i=0; i<period-1; i++)
			if (array[beginId4ThisLoop+i] != array[beginId4ThisLoop+i+period])
				return completedPeriods;
	
		x = array[beginId4ThisLoop+i] >>16;
		y = array[beginId4ThisLoop+i] >>16;
		if (x!=y)
			return completedPeriods;
		completedPeriods++;
	}
	return completedPeriods;
}


int GetTotalNFramesWithPowerFrames (BRIC * br, int num)
{
	double timeDelayBetweenBrics;
	int i, NframesWithPFrames=0;
	// sort br in the order of tmark
	SortBR (br, num);

	for (i=0; i < num; i++)
	{
		// add current total frames
		NframesWithPFrames += br[i].repeat;
		// Add power frames necessary for this bric (if the period is larger)
		NframesWithPFrames += (int) (br[i].pam.period / POWER_FRAME_PERIOD) * br[i].repeat;
		// add power frames if the following bric occurs after more than a pwoer frame period since the last frame of the current bric
		// This does not apply to the last bric
		if ( i < num-1 )
		{
			// timeDelayBetweenBrics: when the second bric begins since the end of the first bric.
			timeDelayBetweenBrics = br[i+1].tmark - (br[i].repeat-1)*br[i].pam.period;
			if (timeDelayBetweenBrics >= POWER_FRAME_PERIOD)
				NframesWithPFrames += (int) ceil(timeDelayBetweenBrics/POWER_FRAME_PERIOD);
		}
	}
	return NframesWithPFrames*3;
}


void int64_to_params (__int64 x, unsigned char* ae, unsigned char* re, unsigned char* cl,
					  unsigned short* uPw, unsigned short* uPg, double* fPeriod)
{
	*fPeriod = getPeriod(x);
	x >>= 18;
	*cl		= (unsigned char)  (x & 0xff);
	x >>= 8;
	*uPg		= (unsigned short) (x & 0x3fff);
	x >>= 14;
	*uPw		= (unsigned short) (x & 0x3fff);
	x >>= 14;
	*re 		= (unsigned char)  (x & 0x1f);
	x >>= 5;
	*ae 		= (unsigned char)  (x & 0x1f);

	*ae = FiveBitsTo_Electrode(*ae);
	*re = FiveBitsTo_Electrode(*re);
}

int int64_to_frame (IMP_FRAME* frImg, __int64 x)
{
	// return NIC error code (with minus) on NIC error,
	//	or 1 if succeeds.

	unsigned short uPw, uPg;
	double dPeriod;
	unsigned char ae, re, cl;

	int64_to_params (x, &ae, &re, &cl, &uPw, &uPg, &dPeriod);

	NIC_ERROR_CHECK(ImpFrameSetElectrodes (frImg, (Electrode)ae, (Electrode)re));
	NIC_ERROR_CHECK(ImpFrameSetPhaseWidth (frImg, (double)uPw*.2));
	NIC_ERROR_CHECK(ImpFrameSetPhaseGap (frImg, (double)uPg*.2));
	NIC_ERROR_CHECK(ImpFrameSetPeriod (frImg, dPeriod));
	NIC_ERROR_CHECK(ImpFrameSetCurrentLevel (frImg, cl));

	return 1;
}

int enic2(int elec)
{
	switch(elec)
	{
	case 101:
		return -1;
	case 102:
		return -2;
	case 103:
		return -3;
	default:
		return elec;
	}
}

int int64_to_frame2 (StimulusHandle frame, __int64 x)
{
	unsigned short uPw, uPg;
	double dPeriod;
	unsigned char ae, re, cl;

	int64_to_params (x, &ae, &re, &cl, &uPw, &uPg, &dPeriod);

	CHECKNIC3(bsSetActiveElectrode(frame, enic2(ae)))
	CHECKNIC3(bsSetReferenceElectrode(frame, enic2(re)))
	CHECKNIC3(bsSetCurrentLevel(frame, cl))
	CHECKNIC3(bsSetPhaseWidth(frame, (double)uPw*.2))
	CHECKNIC3(bsSetPhaseGap(frame, (double)uPg*.2))
	CHECKNIC3(bsSetPeriod(frame, dPeriod))
	return Call_Succeeded;
}

int InsertPowerFramesIfNeeded (__int64 *x, int Nframes, int e1, int e2, int bufferSize)
{
	// return value: (if positive) the number of tokens added to seq due to power frame insertion
	//               (if zero): no need to insert a power frame, nothing happened to sequence (now, x)
	//               (if negative): during the attempt to insert power frames, NIC error occurred.
    //                              (minus) NIC_error	                               
	// e1, e2: active, indifferent electrodes of the power-up frame

	unsigned short uPw, uPg;
	int i, j, numPframe /*number of power frames for the current calculation*/,
		cumNPfr/*cumulative Number of Power Frames*/;
	double remainder, dPeriod, dTemp;
	__int64 *y, a;
	unsigned char ae, re, cl;


	y = (__int64*) calloc (sizeof(__int64), bufferSize);
	cumNPfr=0;
	for (i=0; i<Nframes; i++)
	{
		dPeriod = getPeriod(x[i]);

		if ( dPeriod <= POWER_FRAME_PERIOD) // no power frames needed
			y[i+cumNPfr]=x[i]; 
		else 
		{
			// Continue here 
			// just saw an outrageous dPeriod value---maybe getPeriod was corrupt
			///
			// 
			// 6:58pm 10-23-07
			if ( dPeriod < POWER_FRAME_PERIOD + MIN_PERIOD)
			{
				int64_to_params (x[i], &ae, &re, &cl, &uPw, &uPg, &dPeriod);		
				pulseParam2int64 (&a, ae, re, uPw, uPg, cl, dPeriod-MIN_PERIOD);
				y[i+cumNPfr]=a; 
				dPeriod -= (POWER_FRAME_PERIOD-MIN_PERIOD);
			}
			else // uPeriod greater than POWER_FRAME_PERIOD + MIN_PERIOD
			{
				int64_to_params (x[i], &ae, &re, &cl, &uPw, &uPg, &dTemp);		
				pulseParam2int64 (&a, ae, re, uPw, uPg, cl, POWER_FRAME_PERIOD);
				y[i+cumNPfr]=a; 
				dPeriod -= POWER_FRAME_PERIOD;				
			}

			numPframe = (int)(dPeriod / POWER_FRAME_PERIOD);
			remainder = fmod (dPeriod, POWER_FRAME_PERIOD);

			if ( remainder > 0. && remainder < MIN_PERIOD) 
			// re-distribution of power-up frame periods
			// in this case, numPframe must be non-zero (otherwise, it should have been rejected at timing check routine)
			{
				pulseParam2int64 (&a, e1, e2, 125, 40, 0, dPeriod / (double)(numPframe+1.) );
				for (j=i+cumNPfr+1; j < i+cumNPfr+numPframe+2; j++)
					y[j] = a;
				cumNPfr +=numPframe+1;
			}
			else
			{
				pulseParam2int64 (&a, e1, e2, 125, 40, 0, POWER_FRAME_PERIOD);
				for (j=i+cumNPfr+1; j < i+cumNPfr+numPframe+1; j++)
					y[j] = a;
				cumNPfr +=numPframe;
				if (remainder>0.)
				{
					pulseParam2int64 (&a, e1, e2, 125, 40, 0, remainder);
					y[i+cumNPfr+1] = a;
					cumNPfr++;
				}
			}
		}
			
	}
	for (i=0; i<Nframes+cumNPfr+1; i++)
		x[i] = y[i];
	free(y);
	return cumNPfr;
}

int Bric2Sequence (IMP_SEQUENCE* seqOut, BRIC brIn[], int num, unsigned long flag)
{
	int i, err, bufferSize4TotalFrames, addedPFrames;
	int leftFrames, NTotalFrames(0), memoryUsed(0), period2Try, NPeriods, FilledFrameSoFar(0), items2Cover;
	__int64 *int64Buffer;
	IMP_FRAME* frImg;

	if (seqOut==NULL)		return BR_ERR_NULL_POINTER;
	if (num<=0)		return BR_ERR_INVALID_NUMBRICS;

	if ((err=IsValidBRIC(brIn, num))<0)		return err;

//	if (!BricTimingCheck (brIn, num, &x, &y))		return BR_ERR_TIMING;

	for (i=0; i<num; i++)
		NTotalFrames += brIn[i].repeat;

	// GetTotalNFramesWithPowerFrames () calculates the total number of frames after powerframe insertion
	bufferSize4TotalFrames = GetTotalNFramesWithPowerFrames (brIn, num);
	int64Buffer = (__int64 *)malloc (sizeof(__int64)*bufferSize4TotalFrames);
	
	// This error should not occur, because BricTimingCheck was already checked above.
//	if (Bric2Int64Array (int64Buffer, brIn, num)<0)		return BR_ERR_INTERNAL; 

	// default stimulation mode for a power up frame 
	if (powerE1==0) 		powerE1 = ECE1;
	if (powerE2==0) 		powerE2 = ECE2;

	// NTotalFrames is updated after the power-up frame insertion
	addedPFrames = InsertPowerFramesIfNeeded (int64Buffer, NTotalFrames, powerE1, powerE2, bufferSize4TotalFrames);
	if (addedPFrames<0)		return BR_ERR_INTERNAL; 
	else		NTotalFrames += addedPFrames;

	leftFrames = NTotalFrames;
	
	frImg = ImpFrameNew ();

	if (flag!=BR_NO_PERIOD_CHECK)
	{
	   /*ROUTINE FOR PERIOD INSPECTION AND SEQUENCE APPEND  */
		while (leftFrames > 0)
		{
			period2Try=1;
			NPeriods = 1; 
			while (NPeriods==1 && period2Try <= leftFrames/2)
			{
				NPeriods=IsPeriodic (&int64Buffer[FilledFrameSoFar], NTotalFrames, period2Try);
				if (NPeriods==1) // if it is not periodic with period2Try, then add one
					period2Try++;
			}

			if (NPeriods>1)	
			{
				if (NPeriods==2 && period2Try==1)
				{
					if ((err=int64_to_frame (frImg, int64Buffer[FilledFrameSoFar]))<0)
						return err;
					for (i=0; i<2; i++)
						NIC_ERROR_CHECK(ImpSequenceAppendFrame(seqOut, frImg));
					memoryUsed += 2;
					FilledFrameSoFar += 2;
					leftFrames -= 2;
				}
				else
				{
					NIC_ERROR_CHECK(ImpSequenceAppendRepeatToken (seqOut, NPeriods));
					memoryUsed++;
					items2Cover = NPeriods * period2Try;
					for (i=0; i < period2Try; i++)
					{
						if ((err=int64_to_frame (frImg, int64Buffer[FilledFrameSoFar+i]))<0)
							return err;
						NIC_ERROR_CHECK(ImpSequenceAppendFrame(seqOut, frImg));
						memoryUsed++;
					}
					NIC_ERROR_CHECK(ImpSequenceAppendNextToken (seqOut));
					memoryUsed++;
					FilledFrameSoFar += items2Cover;
					leftFrames -= items2Cover;
				}
			}
			else //NPeriods=1 means there is no periodicity, then process only one frame
			{
				if ((err=int64_to_frame (frImg, int64Buffer[FilledFrameSoFar++]))<0)
					return err;
				NIC_ERROR_CHECK(ImpSequenceAppendFrame(seqOut, frImg));
				memoryUsed++;
				leftFrames--;
			}

		}
	}
	/*END OF ROUTINE FOR PERIOD INSPECTION AND SEQUENCE APPEND  */
	else
	{ // no periodicity checking, no looping--just append them all!!
		for (i=0; i<NTotalFrames; i++)
		{
			if ((err=int64_to_frame (frImg, int64Buffer[FilledFrameSoFar++]))<0)
				return err;
			NIC_ERROR_CHECK(ImpSequenceAppendFrame(seqOut, frImg));
		}
		memoryUsed = NTotalFrames;
	}
	if (memoryUsed > MAX_ALLOWABLE_SEQUENCE)
		return BR_ERR_MEMORY_EXCESS;
	free(int64Buffer);
	NIC_ERROR_CHECK(ImpFrameDelete(frImg));
	return memoryUsed;
}




int InterleaveBrics (BRIC brDest[], int *numDest, BRIC brSecond[], int numSecond, double offset)
{
	//Warning!! It is the programmer's responsibility to pre-allocate sufficient memory space
	// for the destination bric array. If the size of destination bric array is not sufficient,
	// the behavior of this function is unpredictable (like other C functions, e.g., strcpy()...)

	int i, y;

	if (brDest==NULL)
		return BR_ERR_NULL_POINTER;

	for (i=0; i<numSecond; i++)
	{
		brDest[*numDest+i] = brSecond[i];
		brDest[*numDest+i].tmark += offset;
	}

	*numDest += numSecond;

	double *conf = new double[10000];
	BricTimingCheck (brDest, *numDest, conf, &y);
	delete[] conf;
	return BR_NO_ERR;
}


double NextPulse (BRIC *br, int num, double refPt, int& firstID)
{
	// Finds the first pulse later refPt (or at that precise moment), retrieves the index of the pulse in the BRIC
	// returns the tmark of that pulse.
	double tp, temp(1.e15);
	int *nPulses = new int[num];
	double *tmark = new double[num];

	for (int i=0; i<num; i++)
	{
		nPulses[i] = (int)ceil((refPt-br[i].tmark) / br[i].pam.period) + 1;
		if (nPulses[i]<=br[i].repeat)		tmark[i] = br[i].tmark + (nPulses[i]-1) * br[i].pam.period;
		else							tmark[i] = -1.;
			
	}
	for (int i=0; i<num; i++)
	{
		tp = br[i].tmark + (nPulses[i]-1) * br[i].pam.period; // next pulse at or after refPt
		if (tmark[i]>=0)
		{
			if (tmark[i]<temp)
			{
				temp = tp; 
				firstID=i;
			}
		}
	}
	delete[] nPulses;
	delete[] tmark;
	if (temp==1.e15) return -1.;
	else			return temp;
}



int SetupPowerUpFrameElectrodes (int e1, int e2)
{
	// set up active and reference electrodes for power up frame insertion
	
	// if input electrode number is out of range, return FALSE with no updating.
	if ( (e1-1)*(e1-22)*(e1-100)*(e1-103) >0 )
		return 0;
	if ( (e2-1)*(e2-22)*(e2-100)*(e2-103) >0 )
		return 0;
	// If the values are proper, update the electrodes used for power frame construction.
	powerE1 = e1;
	powerE2 = e2;
	return 1;
}

#define ERR1(x) \
{ \
	int err;\
	if ((err=x)<0) \
	{ \
		ImpSequenceDelete(seq1); \
		ImpFrameDelete(powerfrImg);\
		return err;}}

int RepeatedBR2Sequence (IMP_SEQUENCE* seqOut, BRIC br[], int Nbrics, double delay, int repeat)
{  
	// This creates a sequence from br[] and Nbrics with repetition 
	// delay: the delay between repetition in microseconds.
	// returns the memory consumption if suceeds, error code (negative) if fails

	IMP_SEQUENCE* seq1; 
	IMP_FRAME* powerfrImg;
	__int64 a;
	int numPframe, memoryUsed, memory4Br;
	double remainder;

	if (repeat > 65535 || repeat <= 0 )
		return BR_ERR_INVALID_REPEAT;
	if (delay < 0)
		return BR_ERR_INVALID_DELAY;

	if (repeat==1)
	{
		memory4Br = Bric2Sequence (seqOut, br, Nbrics, 0);
		return memory4Br;
	}

	powerfrImg = ImpFrameNew ();
	memoryUsed= 0;

	if (powerE1==0) 
		powerE1 = ECE1;
	if (powerE2==0) 
		powerE2 = ECE2;

	pulseParam2int64 (&a, powerE1, powerE2, 125, 40, 0, POWER_FRAME_PERIOD);
	int64_to_frame (powerfrImg, a);

	seq1 = ImpSequenceNew (powerfrImg);
	ERR1( (memory4Br = Bric2Sequence (seq1, br, Nbrics, 0)));


	numPframe = (int)(delay / POWER_FRAME_PERIOD);
	remainder = fmod (delay, POWER_FRAME_PERIOD);

	pulseParam2int64 (&a, powerE1, powerE2, 125, 40, 0, POWER_FRAME_PERIOD);
	ERR1(int64_to_frame (powerfrImg, a));
	NIC_ERROR_CHECK(ImpSequenceAppendFrames (seq1, powerfrImg, numPframe));

	if (remainder<MIN_PERIOD)
		if (remainder!=0)
			return BR_ERR_TIMING;
	else
		pulseParam2int64 (&a, powerE1, powerE2, 125, 40, 0, remainder);
		
	ERR1(int64_to_frame (powerfrImg, a));
	NIC_ERROR_CHECK(ImpSequenceAppendFrame (seq1, powerfrImg));
	NIC_ERROR_CHECK(ImpSequenceAppendSequence (seqOut, seq1, repeat));

	// For the SPrint memory consumption, refer to the document SPrintMemoryLimitTest.doc
	if (memory4Br==1)
	{
		if (repeat == 1)
			memoryUsed = 3;
		else
			memoryUsed = 4;
	}
	else
	{
		if (repeat == 1)
			memoryUsed = memory4Br+4;
		else
			memoryUsed = memory4Br+6;
	}
	ImpSequenceDelete(seq1); 
	ImpFrameDelete(powerfrImg); 
	return memoryUsed;
}

void PowerBric (BRIC *br)
{
	if (powerE1==0) 
		powerE1 = ECE1;
	if (powerE2==0) 
		powerE2 = ECE2;

	br->ae = powerE1;
	br->re = powerE2;
	br->cl = 0;
	br->pam.pw = 25.;
	br->pam.pg = 8.;
	br->pam.period = POWER_FRAME_PERIOD;
	br->tmark = 0.;
	br->repeat = 1;
}

void ChangePeriod(__int64 *xx, double oldp, double newp)
{
	int iPeriod;
	__int64 y;
	if ( oldp <= 26214.4) // 26214.4---65536*2 (the maximum a 17-bit can handle) divided by 5
	{
		iPeriod = (int)(oldp * 5.);
		*xx -= iPeriod;
	}
	else
	{ 
		y = encodePeriod17bit(oldp);
		*xx -= y ; 
		*xx -= 131072; // 65536* 2, set the 18th bit (to mark a long period)
	}
	if ( newp <= 26214.4) // 26214.4---65536*2 (the maximum a 17-bit can handle) divided by 5
	{
		iPeriod = (int)(newp * 5.);
		*xx += iPeriod;
	}
	else
	{ 
		y = encodePeriod17bit(newp);
		*xx += y ; 
		*xx += 131072; // 65536* 2, set the 18th bit (to mark a long period)
	}
}

int Bric2Seqq (SequenceCommandHandle seqOut, BRIC *br, int num, unsigned long flag)
{
	int i, j, x, err, bufferSize4TotalFrames, nPulses, addedPFrames;
	int leftFrames, NTotalFrames(0), memoryUsed(0), period2Try, NPeriods, FilledFrameSoFar(0), items2Cover;

	StimulusHandle fr;
	SequenceCommandHandle unitSeq, stimComm;

	if (seqOut==NULL)		return BR_ERR_NULL_POINTER;
	if (num<=0)				return BR_ERR_INVALID_NUMBRICS;
	if ((err=IsValidBRIC(br, num))<0)		return err;

	for (i=0; i<num; i++)		NTotalFrames += br[i].repeat;
	//NTotalFrames may not be accurate. Need to do it again below.

	__int64 xx, hold, *int64Buffer;
	double *required_gaps = new double [num*22]; // reserving just a big array
	double *actual_gaps = new double [num*22]; // reserving just a big array

	// GetTotalNFramesWithPowerFrames () calculates the total number of frames after powerframe insertion
	bufferSize4TotalFrames = GetTotalNFramesWithPowerFrames (br, num);
	int64Buffer = (__int64 *)malloc (sizeof(__int64)*bufferSize4TotalFrames);
	
	nPulses=Bric2Int64Array (int64Buffer, br, num);

	double tgap, total_req_gaps, total_act_gaps, tpoint(0), makeup(0);
	unsigned short upw, upg;
	unsigned char ae, re, cl;
	int nConf; // number of pulses that are colliding
	for (i=0; i<nPulses; i++) 
	{
		nConf=0;
		total_req_gaps = total_act_gaps = 0;
		int64_to_params (int64Buffer[i], &ae, &re, &cl, &upw, &upg, &tgap);
		if (makeup >0 && i<nPulses-1)
			ChangePeriod(&int64Buffer[i], tgap, tgap-makeup);
		tpoint += tgap;
		while (tgap < (2*upw +upg)/5. + MINIMUM_SHORTING)		
		{
			total_req_gaps += (required_gaps[nConf] = (2*upw +upg)/5. + MINIMUM_SHORTING); // minimum time gap required before the following pulse
			total_act_gaps += (actual_gaps[nConf] = tgap); // actual gap
			nConf++;
			int64_to_params (int64Buffer[i+nConf], &ae, &re, &cl, &upw, &upg, &tgap);
		}
		if (nConf>0)
		{
			xx = int64Buffer[i-1];
			int64_to_params (int64Buffer[i-1], &ae, &re, &cl, &upw, &upg, &tgap);
			makeup = (total_req_gaps - total_act_gaps)/2;
			pulseParam2int64 (&hold, ae, re, upw, upg, cl, tgap-makeup);
			int64Buffer[i-1] = hold;
			ChangePeriod(&xx, tgap, tgap-makeup); // check if you can replace above 4 lines with this one line
		}
		else
			makeup = 0;
		for (j=i; j<i+nConf; j++)
		{
			ChangePeriod(&int64Buffer[j], actual_gaps[j-i], required_gaps[j-i]);
			int64_to_params (int64Buffer[j], &ae, &re, &cl, &upw, &upg, &tgap);
		}
		if (makeup>0) i = j-1;
	}

	// default stimulation mode for a power up frame 
	if (powerE1==0) 		powerE1 = ECE1;
	if (powerE2==0) 		powerE2 = ECE2;

	for (i=0, NTotalFrames=0; i<num; i++)		NTotalFrames += br[i].repeat;

	// NTotalFrames is updated after the power-up frame insertion
	addedPFrames = InsertPowerFramesIfNeeded (int64Buffer, NTotalFrames, powerE1, powerE2, bufferSize4TotalFrames);
	if (addedPFrames<0)		return BR_ERR_INTERNAL; 
	else					NTotalFrames += addedPFrames;

	leftFrames = NTotalFrames;

	fr = BiphasicStimulusNew(0, 0, 0, 0, 0, 0);
	if ((stimComm = StimulusCommandNew(fr))==NULL) return -4999;
	if (flag!=BR_NO_PERIOD_CHECK)
	{
	   /*ROUTINE FOR PERIOD INSPECTION AND SEQUENCE APPEND  */
		while (leftFrames > 0)
		{
			period2Try=1;
			NPeriods = 1; 
			while (NPeriods==1 && period2Try <= leftFrames/2)
			{
				NPeriods=IsPeriodic (&int64Buffer[FilledFrameSoFar], NTotalFrames, period2Try);
				if (NPeriods==1) // if it is not periodic with period2Try, then add one
					period2Try++;
			}
			
			if (NPeriods>1)	
			{
				unitSeq = SequenceNew(NPeriods);
				items2Cover = NPeriods * period2Try;
				for (i=0; i < period2Try; i++)
				{
					int64_to_frame2(fr, int64Buffer[FilledFrameSoFar+i]);
					CHECKNIC1(scSetStimulus(stimComm, fr))
					CHECKNIC1(sequenceAppend(unitSeq, stimComm))
				}
				memoryUsed += period2Try*2;
				FilledFrameSoFar += items2Cover;
				leftFrames -= items2Cover;
				CHECKNIC1(sequenceAppend(seqOut, unitSeq))
				CHECKNIC2(sequenceCommandDelete(unitSeq))
			}
			else //NPeriods=1 means there is no periodicity, then process only one frame
			{
				int64_to_frame2(fr, int64Buffer[FilledFrameSoFar++]);
				CHECKNIC2(scSetStimulus(stimComm, fr))
				CHECKNIC2(sequenceAppend(seqOut, stimComm))
				memoryUsed++;
				leftFrames--;
			}
		}
	}
	/*END OF ROUTINE FOR PERIOD INSPECTION AND SEQUENCE APPEND  */
	else
	{ // no periodicity checking, no looping--just append them all!!
		
		for (i=0; i<NTotalFrames; i++)
		{
			int64_to_frame2(fr, int64Buffer[FilledFrameSoFar++]);
			CHECKNIC2(scSetStimulus(stimComm, fr))
			CHECKNIC2(sequenceAppend(seqOut, stimComm))
		}
		memoryUsed = NTotalFrames;
		sequenceGetSize(seqOut, &x);
	}
	CHECKNIC2(stimulusDelete(fr))
	CHECKNIC2(sequenceCommandDelete(stimComm))
	free(int64Buffer);
	return memoryUsed;
}

int GetDuration(BRIC *br, int num, double *duration_ms)
{
	int err;
	if (num<=0)				return BR_ERR_INVALID_NUMBRICS;
	if ((err=IsValidBRIC(br, num))<0)		return err;
	GetTotalNFramesWithPowerFrames (br, num);
	double out(0.), temp;
	for (int i=0; i< num; i++)
	{
		temp = br[i].tmark + br[i].pam.period * br[i].repeat;
		if ( temp > out) out = temp;
	}
	*duration_ms = temp/1000.;
	return 1;
}