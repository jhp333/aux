#include "Echo.h"

CSession::CSession()
{
	sessionName[0] = wavePath[0] = nameRule[0] = nToken = nCombo = 0;
	int i;
	token = (char**)calloc(MAX_NUM_TOKEN, sizeof(char*));
	tokenExample = (char**)calloc(MAX_NUM_TOKEN, sizeof(char*));
	for (i=0; i<MAX_NUM_TOKEN; i++)
	{
		token[i] = new char[MAX_LEN_TOKEN];
		tokenExample[i] = new char[MAX_LEN_TOKEN_EXAMPLE]; 
	}
}

CSession::~CSession()
{
	int i;
	for (i=0;i<MAX_NUM_TOKEN; i++)
	{
		delete tokenExample[i];
		delete token[i];
	}
	free(token);
	free(tokenExample);
}

const CSession& CSession::operator=(const CSession& rhs)
{   // not exactly error-proof.... but ok to use just once.
/*	int i;
	if (this != &rhs) 
	{
		token = (char**)calloc(MAX_NUM_TOKEN, sizeof(char*));
		tokenExample = (char**)calloc(MAX_NUM_TOKEN, sizeof(char*));
		for (i=0; i<MAX_NUM_TOKEN; i++)
		{
			token[i] = new char[MAX_LEN_TOKEN];
			tokenExample[i] = new char[MAX_LEN_TOKEN_EXAMPLE]; 
		}
	}
*/	return *this; // return self-reference so cascaded assignment works
}

BOOL CSession::Update(int inout)
{
	return 1;
}
