#include <stdlib.h>
#include <string.h>
#include "Echo.h"

int findstr(int *foundIndices, char *source, char *token);
 
int findMarkers4EachItems (map<int,int> &indices, char *rule)
{
	int cnt = 0;
	for (int next=0,pos=0; (pos=strcspn(rule+next, "\\*#")) != strlen(rule+next); next+=pos+1)
		indices[cnt++] = next+pos;
	indices[cnt] = strlen(rule);
	return cnt;
}

#define MAX_NUM_ELEMENTS	100
#define MAX_LEN_ELEMENT		64
int DecodeNameRuleAndGenerateList (char *ruleStr, map<int,string> &FileNameList, map<int,int> &wavID2tokenID, CSession *sess)
{//                                       in                       out                        out                   in/out
	char elementss[MAX_NUM_ELEMENTS][MAX_LEN_ELEMENT];
	char *elements[MAX_NUM_ELEMENTS];
	int nNames, nRuleItems;
	map<int,int> indices;

	if ((nRuleItems=findMarkers4EachItems (indices, ruleStr)) == 0)
		return ERR_DECODING_INVALID_FORMAT;

	for (int i=0; i<MAX_NUM_ELEMENTS; i++)
		elements[i] = elementss[i];

	bool fTokenKeyUsed = false;	// flag for '*'
	for (int i=0; i<nRuleItems; i++)
	{
		char block[MAX_LEN_ELEMENT];
		int nElements4Each, ind[20];

		memcpy(block, &ruleStr[indices[i]], indices[i+1]-indices[i]);
		block[indices[i+1]-indices[i]] = 0;

		switch(block[0])
		{
		case '\\':
			if (findstr(ind, block+1, " ")+1 > MAX_NUM_ELEMENTS)
				return ERR_DECODING_NUMBERS_TOO_MANY;
			nElements4Each = str2strarray (elements, MAX_NUM_ELEMENTS, block+1, " ");
			break;
		case '*':
			if (sess->nToken > MAX_NUM_ELEMENTS)
				return ERR_DECODING_NUMBERS_TOO_MANY;
			for (int j=0; j<sess->nToken; j++)
				strcpy(elements[j], sess->token[j]);
			nElements4Each = sess->nToken;
			if (fTokenKeyUsed)
				return ERR_DECODING_INVALID_FORMAT;	// '*' can appear only once.
			else
				fTokenKeyUsed = true;
			break;
		case '#':
			if (str2intarray (ind, 16, block, "# -") != 2)
				return ERR_DECODING_MUST_BE_TWO_NUMBERS;
			if (ind[1] < ind[0])
				return ERR_DECODING_NUMBERS_MUST_BE_ASCENDING;
			if (ind[1]-ind[0]+1 > MAX_NUM_ELEMENTS)
				return ERR_DECODING_NUMBERS_TOO_MANY;
			for (int j=0,k=0; j<ind[1]-ind[0]+1; j++, k++)
				itoa(ind[0]+k, elements[j], 10);
			nElements4Each = ind[1]-ind[0]+1;
			break;
		default:
			return ERR_DECODING_INVALID_FORMAT;
		}
		if (i==0) {	// make the initial FileNameList with the first rule
			for (int j=0; j<nElements4Each; j++) {
				FileNameList[j] = elements[j];
				if (block[0] == '*')
					wavID2tokenID[j] = j;
			}
			nNames = nElements4Each;
		} else {	// expand FileNameList for the current rule. The list is growing by nElements4Each times.
			for (int j=0; j<nElements4Each; j++) {
				for (int k=0; k<nNames; k++) {
					if (j < nElements4Each-1) {	// except for the last element
						// copy the current name to the next block (another block of the previous names will be ready at the end of k loop, to be used by next j)
						FileNameList[nNames*(j+1)+k] = FileNameList[nNames*j+k];
						if (fTokenKeyUsed)
							wavID2tokenID[nNames*(j+1)+k] = wavID2tokenID[nNames*j+k];
					}
					FileNameList[nNames*j+k] += elements[j];
					if (block[0] == '*')
						wavID2tokenID[nNames*j+k] = j;
				}
			}
			nNames *= nElements4Each;
		}
	}
	strcpy(sess->nameRule, ruleStr);	// update nameRule
	return nNames;
}