#pragma once

#include "WndDlg.h"
#include <commctrl.h>  // includes status bar
#include "resource.h"
#include "msgCrack_seacrm.h"
#include "audfret.h"
#include "audstr.h"
#include "sigproc.h"
#include "graffy.h"

#define INIFILE "auxgv.ini"
#define FONTS_DEF "MS Reference Sans Serif"
#define DEFAULT_FS 22050
#define DEFAULT_BLOCK_SIZE 200.

BOOL CALLBACK  setfsproc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam);