#include <Windows.h>

#ifndef LINK_STATIC_LIBCURL
BOOL APIENTRY DllMain( HANDLE hModule, DWORD u, LPVOID rese)
{
	return TRUE;     
}
#endif
