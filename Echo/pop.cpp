#include "stdafx.h"
#include <commdlg.h>
#include <stdlib.h>
#include <stdio.h>

static OPENFILENAME ofn ;
static char szResultFilter[] = "Text Files (*.TXT)\0*.txt\0";
static char szSSSFilter[] = "SESSION Files (*.SSS)\0*.sss\0";

void PopFileInitialize (HWND hwnd)
{

     ofn.lStructSize       = sizeof (OPENFILENAME) ;
//     ofn.hwndOwner         = hwnd ;
     ofn.hInstance         = NULL ;
//     ofn.lpstrFilter       = szFilter ;
     ofn.lpstrCustomFilter = NULL ;
     ofn.nMaxCustFilter    = 0 ;
     ofn.nFilterIndex      = 0 ;
//     ofn.lpstrFile         = NULL ;          // Set in Open and Close functions
     ofn.nMaxFile          = _MAX_PATH ;
//     ofn.lpstrFileTitle    = NULL ;          // Set in Open and Close functions
     ofn.nMaxFileTitle     = _MAX_FNAME + _MAX_EXT ;
     ofn.lpstrInitialDir   = NULL ;
//     ofn.lpstrTitle        = NULL ;
     ofn.Flags             = 0 ;             // Set in Open and Close functions
     ofn.nFileOffset       = 0 ;
     ofn.nFileExtension    = 0 ;
//     ofn.lpstrDefExt       = "sss" ;
     ofn.lCustData         = 0L ;
     ofn.lpfnHook          = NULL ;
     ofn.lpTemplateName    = NULL ;
}

BOOL SessionFileOpenDlg (HWND hwnd, PSTR pstrFileName, PSTR pstrTitleName)
{
     ofn.hwndOwner         = hwnd ;
     ofn.lpstrFile         = pstrFileName ;
     ofn.lpstrFileTitle    = pstrTitleName ;
	 ofn.lpstrFilter = szSSSFilter;
	 ofn.lpstrDefExt = "sss";
     ofn.Flags             = OFN_HIDEREADONLY | OFN_CREATEPROMPT ;
     return GetOpenFileName (&ofn) ;
}

BOOL PopFileSaveDlg (HWND hwnd, PSTR pstrFileName, PSTR pstrTitleName, PSTR szFilter, PSTR lpstrDefExt)
{
     ofn.hwndOwner         = hwnd ;
     ofn.lpstrFile         = pstrFileName ;
     ofn.lpstrFileTitle    = pstrTitleName ;
	 ofn.lpstrFilter = szFilter;
	 ofn.lpstrDefExt = lpstrDefExt;

     return GetSaveFileName (&ofn) ;
}

BOOL SessionFileSaveDlg (HWND hwnd, PSTR pstrFileName, PSTR pstrTitleName)
{
	ofn.Flags             = OFN_OVERWRITEPROMPT ;
	return PopFileSaveDlg (hwnd, pstrFileName, pstrTitleName, szSSSFilter, "sss"); 
}

BOOL ResultFileSaveDlg (HWND hwnd, PSTR pstrFileName)
{
     ofn.Flags             = !OFN_OVERWRITEPROMPT ;
	return PopFileSaveDlg (hwnd, pstrFileName, NULL, szResultFilter, "txt");
}
