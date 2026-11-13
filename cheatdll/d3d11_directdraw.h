#pragma once

#ifndef WINDOWS_IGNORE_PACKING_MISMATCH
#define WINDOWS_IGNORE_PACKING_MISMATCH
#endif

#include <Windows.h>

bool initializeD3D11Renderer(HMODULE module);
