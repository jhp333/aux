#include <string.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <math.h> 
#include "echo.h" 
 
#define PERMUTE_CONSTANT	20

char * append_path_file (char *out, const char *path, const char * file)
{
	if (strlen(path)==0)
		strcpy(out, file);
	else if (path[strlen(path)-1]!='\\')
		sprintf(out, "%s\\%s", path, file);
	else
		sprintf(out, "%s%s", path, file);
	return out;
}

int _mod (int big, int divider)
{
// calculating int remainder
	return (int)fmod((double)big, (double)divider);
}

int randin(int limit)
{// generated a random number with uniform distr (from 0 thru limit-1)
	return _mod(rand(),limit);
}

void permute(int *out, int length)
{
	int i, j, k;
	int temp;
	for (i=0; i<length*PERMUTE_CONSTANT; i++)
	{
		j=randin(length);
		k=randin(length);
		temp = out[j];
		out[j] = out[k];
		out[k] = temp;
	}
}

void permute(int *prOrder, unsigned char *out1, unsigned char *out2, int nConc, int length)
{
	int i, j, k;
	int temp;
	for (i=0; i<length*PERMUTE_CONSTANT; i++)
	{
		j=randin(length);
		k=randin(length);
		temp = out1[j];
		out1[j] = out1[k];
		out1[k] = temp;
		temp = out2[j];
		out2[j] = out2[k];
		out2[k] = temp;

		temp = prOrder[j];
		prOrder[j] = prOrder[k];
		prOrder[k] = temp;
	}
}

int randperm (int *out, int length, int range)
{ // array from 0 thru m-1
	// m is the actual length of the array necessary (length of out)
	// range is the total number of possible combinations.
	if (out==NULL)
		return -1;
	int block_len = (int)ceil(length * 1.0 / range)*range;
	int *tmpOut = new int[block_len];

	for (int i=0; i<block_len; i++)
		tmpOut[i]=_mod(i,range);
	permute(tmpOut, block_len);
	memcpy((void*)out, (void*)tmpOut, length*sizeof(*out));
	
	delete[] tmpOut;
	return 0;
}


int findstr(int *foundIndices, char *source, char *token)
{
	char *pdest;
	int result, sourceLen, i=0;

	sourceLen = strlen(source);
	pdest = strstr (source, token);
	result = pdest - source;
  	while (pdest!=NULL)
	{
		foundIndices[i] = result;
		pdest = strstr (&source[result+1], token);
		result = pdest - source;
		i++;
	}
	return i; // Number of items in foundIndices
}

void bubbleSort(int numbers[], int array_size)
{
  int i, j, temp;

  for (i = (array_size - 1); i >= 0; i--)
  {
    for (j = 1; j <= i; j++)
    {
      if (numbers[j-1] > numbers[j])
      {
        temp = numbers[j-1];
        numbers[j-1] = numbers[j];
        numbers[j] = temp;
      }
    }
  }
}

int CountSameElements(int *x, int *y, int len)
{
	// This returns the number of elements that appear in both arrays x and y
	// Note: 1) x must be greater or equal to zero.
	//       2) the content of y will be altered.
	// This has not been optimized for large len----use this only when len is small (e.g., less than 5)
	// May24-2005. BJ Kwon
	int i, j, cum=0;
	if (len==1)
		return *y==*x;

	for (i=0; i<len; i++)
	{
		for (j=0; j<len; j++)
		{
			if (y[j]==x[i])
			{
				//clear y[j] (so that it won't be used again), increment the counter and break
				y[j]=-1; // x must be positive or zero
				cum++;
				j=len+1;
			}
		}
	}
	return cum;
}

int find(int* array, int len, int findee)
{
	int i, res=0;

	for (i=0; i<len; i++)
		if (array[i] == findee)
			res++;
	return res;
}

int findWhere (int* array, int len, int findee)
{
	// returns the index where it was found, if not found, returns -1
	int i;

	for (i=0; i<len; i++)
		if (array[i] == findee)
			return i;
	return -1;
}

int readINI2string (char* errstr, const char *fname, const char *heading, string &outString)
{
	int res, len;
	char *strPt;
	char errStr[MAX_PATH];

	len = sscanfINI(errStr, fname, heading, "GETSTRING", NULL);
	if (len>0)
	{
		strPt = new char[len+1];
		res = sscanfINI(errStr, fname, heading, "GETSTRING", strPt);
		if (res)	
		{
			outString = strPt;
			trim(outString, ' ');
			trimr(outString, "\r\n");
			res = outString.length();
		}
		delete[] strPt;
		
		if (res==0 && strlen(errStr)==0)
			sprintf(errstr, "%s (empty)", heading);
		else 
			errstr = errStr;
	}
	else
		errstr = errStr;
	return res;
}