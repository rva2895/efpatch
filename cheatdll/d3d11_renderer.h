#pragma once

#ifndef WINDOWS_IGNORE_PACKING_MISMATCH
#define WINDOWS_IGNORE_PACKING_MISMATCH
#endif

#include <Windows.h>
#include <ddraw.h>

using D3D11LogCallback = void (*)(const char* message);

void setD3D11LogCallback(D3D11LogCallback callback);
void resetD3D11Renderer();
HRESULT presentD3D11Frame(HWND window, DWORD width, DWORD height,
                          const void* pixels, UINT pitch,
                          const PALETTEENTRY* palette, UINT syncInterval);
