#pragma once

#include "resource.h"
BOOL CALLBACK ConfigDlgProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam);

void __stdcall launchConfigurator(HWND hWnd);
