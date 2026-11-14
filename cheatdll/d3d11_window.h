#pragma once

#ifndef WINDOWS_IGNORE_PACKING_MISMATCH
#define WINDOWS_IGNORE_PACKING_MISMATCH
#endif

#include <Windows.h>

bool configureD3D11WindowedMode(HWND window, DWORD width, DWORD height);
bool setD3D11WindowMouseMapping(HWND window, DWORD width, DWORD height, bool enabled);
bool shutdownD3D11Window();
