#pragma once
#include "aceset_internal.h"
#include "WndDlg.h"

using namespace std;

#define ACESET_API __declspec(dllexport)
#include <limits>

#define ACESET_QNAN numeric_limits<double>::quiet_NaN()
#include "ACEsetDlg_base.h"

#define WM__OPEN_MAP  WM_APP+100
