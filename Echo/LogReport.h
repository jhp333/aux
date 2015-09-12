#pragma once
#include "stdafx.h"

class LogReport {
public:
	BOOL   fUseEmail;
	BOOL   fUseFtp;
	CString email_address;
	CString ftp_address;
	CString ftp_id;
	CString ftp_password;

	CString filename;
	CString NamePrefix;
	LogReport(char *namePrefix);
	void Upload(char *fname);
	void ReadINI(char *iniFilename);
	void WriteINI(char *iniFilename);
};
