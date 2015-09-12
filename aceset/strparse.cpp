#include <StdString.h>
#include <string.h>
#include "audfret.h"

CStdString& FulfillFile (CStdString& out, const char *path, const char * file)
{
	CStdString _out("");
	size_t len = strlen(path);
	if (len==0)		
		_out = file;
	else if (path[len-1]!='\\')
		_out.Format("%s\\%s", path, file);
	else
		_out.Format("%s%s", path, file);
	out = _out;
	return out;
}

void sprintfFloat(double f, int max_fp, CStdString& strOut)
{
	size_t len=MAX_PATH;
	char *buf= new char[len];

	while (!sprintfFloat(f, max_fp, buf, len))
	{
		delete[] buf;
		len *=2;
		buf= new char[len];
	}
	strOut = buf;
	delete[] buf;
}