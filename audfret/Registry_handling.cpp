#include <windows.h>
#include <stdio.h>
#include "bjtools_internal.h"
#include "audfret.h"

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

AUDFRET_EXP int Registry(int isitget, HKEY reg_key_handle, const char *keyName, const char *ItemName, char *buffer, char *errstr)
{
	bool done(false);
    char achClass[MAX_PATH], achValue[MAX_VALUE_NAME];
    DWORD  cchClassName(MAX_PATH), cSubKeys(0), cbMaxSubKey, cchMaxClass, cValues, cchMaxValue, cbMaxValueData, cbSecurityDescriptor, cchValue(MAX_VALUE_NAME);
	DWORD  retCode, i;
    FILETIME ftLastWriteTime;      // last write time 

	HKEY key;
	DWORD dw(0), dwres, dwType, cbData(MAX_PATH);
	BYTE btdata[MAX_PATH];
	errstr[0] = 0;
	achClass[0]='\0'; 

	LONG ln = RegCreateKeyEx( reg_key_handle, keyName, 0, NULL, 0, KEY_READ | STANDARD_RIGHTS_WRITE | KEY_WOW64_32KEY, NULL, &key, &dwres);
	if (ln!=ERROR_SUCCESS)
	{
		GetLastErrorStr(ln, errstr);
		return 0;
	}
	if (!isitget) // Set values in the registry 
	{
		if (RegSetValueEx(key, ItemName, 0, REG_SZ, (const BYTE*)&buffer[0], (DWORD)strlen(buffer))!=ERROR_SUCCESS)
		{	strcpy(errstr,"error on RegSetValueEx()"); return 0;  }
	}
	else // Get values from the registry 
	{
		buffer[0] = 0;
		if (dwres==REG_CREATED_NEW_KEY)
		{	sprintf(errstr,"The registry key %s does not exist (just created now)", keyName); return 0; }
		else if (dwres==REG_OPENED_EXISTING_KEY)
		{
			retCode = RegQueryInfoKey(key, achClass, &cchClassName, NULL,
				&cSubKeys, &cbMaxSubKey, &cchMaxClass, &cValues, &cchMaxValue, 
				&cbMaxValueData, &cbSecurityDescriptor, &ftLastWriteTime);
			if (cValues) 
			{
				for (i=0, retCode=ERROR_SUCCESS; i<cValues; i++) 
				{ 
					achValue[0] = '\0'; 
					cchValue = cbData = MAX_PATH;
					retCode = RegEnumValue(key, i, achValue, &cchValue, NULL, &dwType, btdata, &cbData);
					if (retCode == ERROR_SUCCESS) 
					{ 
						if (!strcmp(achValue,ItemName))
						{
							done = true;
							strcpy(buffer, (LPCSTR)btdata);
						}
					}
					else
					{   strcpy(errstr,"error on RegEnumValue()"); return 0;    }
				}
				if (!done)  { sprintf(errstr,"The registry key item %s does not exist.",ItemName ); return 0; }
			}
			else
			{	sprintf(errstr,"No items in the tree %s.", keyName); return 0; }
		}
	}
	RegCloseKey(key);
	return 1;
}
