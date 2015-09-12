#include "stdafx.h"
#include <math.h>
#include <stdlib.h>
#include <math.h>
#include "audfret.h"
#include "audstr.h"

#include <string>
#include "sigproc.h"
#include "supportFunctions.h"
using namespace std;

#define CHAR_NUMERIC ":.+-0123456789"

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

int IsIndexInsideBracket(CString str, int id)
{
	CString before, after;
	int iOpen, iClose;

	before = str.Left(id);
	iClose = before.ReverseFind(']');
	iOpen = before.ReverseFind('[');
	if (iClose<iOpen)
		return 0;

	after = str.Mid(id);
	iOpen = after.ReverseFind('[');
	iClose = after.ReverseFind(']');
	if (iClose<iOpen)
		return 0;
	return 1;
}

int randin(int limit)
{// generated a random number with uniform distr (from 0 thru limit-1)
	if (limit==0) return 0;
	return (int)fmod((double)rand(),(double)limit);
}

char * perm(char *out, char m)
{ // permute the order of an array out with length m
	int i, j, k;
	char temp;
	if (out==NULL)
		return out;
	for (i=0; i<m*1000; i++)
	{
		j=randin(m);
		k=randin(m);
		temp = out[j];
		out[j] = out[k];
		out[k] = temp;
	}
	return out;
}


int GetIndexWithZero(double* array, int len)
{
	int i;
	for (i=0; i<len;i++)
	{
		if (array[i] == 0)
			return i;
	}
	return -1; // no element is zero
}

int AppendVal2Array(double* array, double val)
{// array should have at least one member with the value zero.
	UINT ind=0;
	while (1)
	{
		if (array[ind] == 0.)
		{
			array[ind] = val;
			return ind;
		}
		else 
			ind++;
		if (ind > 65536*2-1)
			return -1; //error!! 
	}
}

double appendMeanIt (double* x, double offsetORvalue, double *std)
{
	// if the first argument is null, it means initializing currentID
	// if the 3rd argument is null, it means append to array
	
	// offset: the number to skip in calculation of the average.

	static int currentID;
	int i; 
	double mean, sqSum=0., sum=0.;

	if (x==NULL)
	{
		currentID = 0;
	}
	else if (std==NULL)
	{
		x[currentID++]=offsetORvalue;
	}
	else
	{
		for (i=(int)offsetORvalue; i<currentID; i++)
		{
			sum += x[i];
			sqSum += (x[i] * x[i]) ; 
		}
		mean = (double)sum / ((double)i-offsetORvalue);
		*std = sqrt ( (double)sqSum / ((double)i-offsetORvalue) - mean*mean);
		return mean;
	}
	return 0.;
}

int getRandAnswer(int cinterval)
{
	int x;
	if (cinterval==0)
		return 0;
	else
	{
		x = rand()% cinterval;
		return x % cinterval;
	}
}

void EnableID(CWnd *hDlg, int id, int onoff)
{
	CWnd *h = hDlg->GetDlgItem(id);
	h->EnableWindow(onoff);
}

int countChar(CString const str, char const c)
{
	int id, count=0;
	id=str.Find(c);
	while (id!=-1)
	{
		count++;
		id=str.Find(c,id+1);
	}
	return count;
}


int FindAny(const CString str, const CString chars)
{
	int i;
	CString a=str;
	CString b=chars;
	a.MakeLower();
	b.MakeLower();
	for (i=0; i<b.GetLength(); i++)
		if (a.Find(b.GetAt(i))!=-1)
			return 1;
	return 0;
}

double getmean(int len, double *x, double *std)
{
	double mean, sqSum(0.), sum(0.);
	for (int i=0; i<len; i++)
	{
		sum += x[i];
		sqSum += (x[i] * x[i]) ; 

	}
	mean = sum / (double)len;
	*std = sqrt ( sqSum/(double)len - mean*mean );
	return mean;
}

int DoesItHaveVariable(const CString& strIn)
{ // For now, 'v' is the variable.
	int id;
	CString str(strIn);
	if ((id=str.Find('v'))==-1) return 0;
	if (id>=2) 
	{
		if (str[id-2]=='w' && str[id-1]=='a') 
		{
			str = str.Right(str.GetLength()-id-1);
			return DoesItHaveVariable(str);
		}
	}
	return 1;
}

int FindFirstNonNumeric(const CString& str)
{
	CString s(CHAR_NUMERIC);
	for (int i=0; i<str.GetLength(); i++)
	{
		if (s.Find(str[i])==-1)
			return i;
	}
	return -1;
}

int IsNumeric(const CString str)
{
	CString _str(str);
	_str.Trim();
	_str.Replace("[",".");
	_str.Replace("]",".");
	_str.Replace(" ",".");

	if (FindFirstNonNumeric(_str)==-1) return 1;
	else return 0;
}

int GetSurroundedBy(char c1, char c2, string &in, string &out)
{
	char *buffer = new char[in.length()+1];
	int res = GetSurroundedBy(c1, c2, in.c_str(), buffer, 0);
	out = buffer;
	delete[] buffer;
	return res;
}