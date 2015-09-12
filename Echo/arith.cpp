#include "Echo.h"

#define PERMUTE_CONSTANT	20

int mod (int big, int divider)
{
// calculating int remainder
	return (int)fmod((double)big, (double)divider);
}

int randin(int limit)
{// generated a random number with uniform distr (from 0 thru limit-1)
	return mod(rand(),limit);
}

void permute(char *out, int length)
{
	int i, j, k;
	char temp;
	for (i=0; i<length*PERMUTE_CONSTANT; i++)
	{
		j=randin(length);
		k=randin(length);
		temp = out[j];
		out[j] = out[k];
		out[k] = temp;
	}
}

void permuteThem (char ** PreOrder, char * presentSettingID, char * addprocID, int length)
{
	int i, j, k, m;
	char temp;
	for (i=0; i<length*PERMUTE_CONSTANT; i++)
	{
		j=randin(length);
		k=randin(length);
		temp = presentSettingID[j];
		presentSettingID[j] = presentSettingID[k];
		presentSettingID[k] = temp;
		temp = addprocID[j];
		addprocID[j] = addprocID[k];
		addprocID[k] = temp;
		for (m=0; m<nConc; m++)
		{
			temp = PreOrder[m][j];
			PreOrder[m][j] = PreOrder[m][k];
			PreOrder[m][k] = temp;
		}
	}
}


int randperm (char *out, int length, int range)
{ // array from 0 thru m-1
	// m is the actual length of the array necessary (length of out)
	// range is the total number of possible combinations.
	// length must be equal or less than range.
	int i;
	char *tmpOut;
	if (out==NULL)
		return -1;
	if (length < range)
		tmpOut = new char[range];
	else //if (range >= length)
		tmpOut = out;
	for (i=0; i<max(length,range); i++)
		tmpOut[i]=mod(i,range);

	permute(tmpOut, length);
	if (length < range)
	{
		memcpy((void*)out, (void*)tmpOut, length*sizeof(char));
		delete tmpOut;
		return 0; // still success, but different return from when range = length
	}
	return 1;
}


int GenerateRandomList (int nConc, char **PreOrder, int nCombo, int mult, char *presentSettingID, char *addprocID)
{
	int i, j, k, org_nTrials, nPresets, nAddprocs;

	if (!mult)
	{
		for (i=0; i<nConc; i++)
		{
			// randperm() produces a permuted series of integer array between 1 and nCombo (but only the first nTrials elements).
			if (randperm (PreOrder[i], nTrials, nCombo)<0)
			{
				MessageBox (NULL, "un-initialized PreOrder", "randomizeList failed", MB_OK);
				return 0;
			}
		}
	}
	else
	{
		nPresets = presentSettingID[0];
		nAddprocs = addprocID[0];
		org_nTrials = nTrials / nPresets /nAddprocs;
		// First, make PreOrder (with org_nTrials length) with nPresets*nAddprocs (nTrials) times with randperm
		// and align it sequentially with presentSettingID and addprocID then permute the order again.
		// so that each presentation is balanced across presentSettingID and addprocID.

		// randperm
		for (k=0; k<nPresets; k++)
			for (j=0; j<nAddprocs; j++)
				for (i=0; i<nConc; i++)
					if (randperm (PreOrder[i]+org_nTrials*(j+nAddprocs*k), org_nTrials, nCombo)<0)
					{
						MessageBox (NULL, "un-initialized PreOrder", "randomizeList failed", MB_OK);
						return 0;
					}
		// aligned indices
		for (i=0; i<nPresets; i++)
		{
			memset(presentSettingID+org_nTrials*nAddprocs*i, i, org_nTrials*nAddprocs);
			for (j=0; j<nAddprocs; j++)
				memset(addprocID+org_nTrials*(nAddprocs*i+j), j, org_nTrials);
		}
		// permute PreOrder with presentSettingID and addprocID
		permuteThem (PreOrder, presentSettingID, addprocID, nTrials);
	}
	return 1;
}

int wavID2tokenID (int wavID, int nToken, int nSpeakers, int permuteType)
{
	if (permuteType) // token list first, speaker list later.. such as ah-aba ah-ada ah-aga ct-aba ct-ada ct-aga
		return mod(wavID, nToken);
	else// speaker list first, token list later... such as had1 had2 had3 head1 head2 head3
		return wavID/nSpeakers;
}

