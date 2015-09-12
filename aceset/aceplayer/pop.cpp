#include <windows.h>
#include <commdlg.h>
#include <stdlib.h>
#include <stdio.h>

static OPENFILENAME ofn ;

void FileDlgInit(const char *initDir)
{
     ofn.lStructSize       = sizeof (OPENFILENAME) ;
     ofn.hInstance         = NULL ;
     ofn.lpstrCustomFilter = NULL ;
     ofn.nMaxCustFilter    = 0 ;
     ofn.nFilterIndex      = 0 ;
     ofn.nMaxFile          = _MAX_PATH ;
     ofn.nMaxFileTitle     = _MAX_FNAME + _MAX_EXT ;
     ofn.lpstrInitialDir   = initDir ;
     ofn.Flags             = 0 ; 
     ofn.nFileOffset       = 0 ;
     ofn.nFileExtension    = 0 ;
     ofn.lCustData         = 0L ;
     ofn.lpfnHook          = NULL ;
     ofn.lpTemplateName    = NULL ;
}

BOOL FileOpenDlg (HWND hwnd, LPSTR pstrFileName, LPSTR pstrTitleName, LPSTR szFilter, LPSTR lpstrDefExt)
{
     ofn.hwndOwner         = hwnd ;
     ofn.lpstrFile         = pstrFileName ;
     ofn.lpstrFileTitle    = pstrTitleName ;
	 ofn.lpstrFilter = szFilter;
	 ofn.lpstrDefExt = lpstrDefExt;
     ofn.Flags             = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	 return GetOpenFileName (&ofn) ;
}

BOOL FileSaveDlg (HWND hwnd, LPSTR pstrFileName, LPSTR pstrTitleName, LPSTR szFilter, LPSTR lpstrDefExt)
{
     ofn.hwndOwner         = hwnd ;
     ofn.lpstrFile         = pstrFileName ;
     ofn.lpstrFileTitle    = pstrTitleName ;
	 ofn.lpstrFilter = szFilter;
	 ofn.lpstrDefExt = lpstrDefExt;

     return GetSaveFileName (&ofn) ;
}

