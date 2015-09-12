#include <string.h> 
 
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