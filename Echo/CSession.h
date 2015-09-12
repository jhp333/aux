#pragma once

class CSession
{
public:
	BOOL Update(int inout);
	char sessionName[32];
	char wavePath[256];
	char nameRule[256];
	int nCombo;
	int nToken;
	char **token;
	char **tokenExample;

	CSession();
	~CSession();
	const CSession& operator=(const CSession& rhs);

};
