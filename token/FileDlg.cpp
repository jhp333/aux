#pragma once
#include "FileDlg.h"

CFileDlg::CFileDlg()
{
     ofn.lStructSize       = sizeof (OPENFILENAME) ;
     ofn.hInstance        = NULL ;
	 ofn.lpfnHook = NULL ;
	ofn.lpTemplateName =  ofn.lpstrCustomFilter = NULL ;
     ofn.nMaxCustFilter    = ofn.nFilterIndex
		 = ofn.Flags 
		 = ofn.nFileOffset
		 = ofn.nFileExtension = 0;
	 ofn.lCustData =  0L ;
     ofn.nMaxFile          = _MAX_PATH ;
     ofn.nMaxFileTitle     = _MAX_FNAME + _MAX_EXT ;
	 ofn.lpstrInitialDir   = InitDir ;
	 LastPath[0]='\0';
}

void CFileDlg::InitFileDlg (HWND hwnd, const char *initDir)
{
     ofn.hwndOwner         = hwnd ;
	 if (initDir!=NULL)     strcpy(InitDir, initDir);
}

int CFileDlg::FileOpenDlg (LPSTR pstrFileName, LPSTR pstrTitleName, LPSTR szFilter, LPSTR lpstrDefExt)
{
	char drive[64], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
	ofn.lpstrFile         = pstrFileName ;
	ofn.lpstrFileTitle    = pstrTitleName ;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrDefExt = lpstrDefExt;
	ofn.Flags             = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	BOOL res = GetOpenFileName (&ofn) ;
	if (res)
	{
		_splitpath(pstrFileName, drive, dir, fname, ext);
		_makepath(LastPath, drive, dir, "", "");
	}
	return res;
}

int CFileDlg::FileSaveDlg (LPSTR pstrFileName, LPSTR pstrTitleName, LPSTR szFilter, LPSTR lpstrDefExt)
{
	char drive[64], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
	ofn.lpstrFile         = pstrFileName ;
	ofn.lpstrFileTitle    = pstrTitleName ;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrDefExt = lpstrDefExt;

	BOOL res = GetSaveFileName (&ofn) ;
	if (res)
	{
		_splitpath(pstrFileName, drive, dir, fname, ext);
		_makepath(LastPath, drive, dir, "", "");
	}
	return res;
}