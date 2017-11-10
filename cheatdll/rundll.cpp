#include "stdafx.h"
#include "rundll.h"
#include "autosave.h"
#include "editorstatus.h"
#include "cliff.h"
#include "terrain.h"
#include "crashreporter.h"

#include <CrashRpt.h>

extern crash_rpt::CrashRpt* g_crashRpt;

extern int placementSettings;
extern int cliff_type;

extern bool editorstatus_isValid;

extern void* scen_ptr;

int CALLBACK WndProc_dll(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

int (WINAPI* WinMain_exe) (HINSTANCE, HINSTANCE, LPSTR, int) =
	(int (WINAPI*) (HINSTANCE, HINSTANCE, LPSTR, INT)) 0x0048EFC0;

extern "C" __declspec(dllexport) int WINAPI WinMain_dll (
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

	log("WinMain_dll called");

	//InitCommo

	initialSetup();

	setInt(0x00426509, (int)&WndProc_dll);

	int retval;

#ifndef _DEBUG
	initCrashReporter();
	
	__try
	{
#endif
		retval = WinMain_exe(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
#ifndef _DEBUG
	}
	__except (g_crashRpt->SendReport(GetExceptionInformation()))
	{
		ExitProcess(0);
	}
#endif

	log("WinMain_exe returned %d, exiting", retval);
	return retval;
}

int (CALLBACK* WndProc_exe) (HWND, UINT, WPARAM, LPARAM) =
	(int (CALLBACK*) (HWND, UINT, WPARAM, LPARAM)) 0x00426530;

HWND hWnd_main = 0;

int cliff_types[] = { 0x108, 3971, 3981, 3991, 0 };
int* cliff_types_ptr = cliff_types;

extern bool isEditor;

int terrain_paint_mode = 0;	//default

int CALLBACK WndProc_dll(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	bool repaint = false;

	if (msg == WM_KEYDOWN)
	{
		if (isEditor)
		{
			if (LOWORD(wParam) == 'S')						//grid - collision
			{
				if (short x = GetKeyState(VK_CONTROL))
				{
					placementSettings++;
					if (placementSettings > 3)
						placementSettings = 0;
					editorstatus_isValid = false;
				}
			}
			/*if (LOWORD(wParam) == 'L')						//terrain transition
			{
				if (short x = GetKeyState(VK_CONTROL))
				{
					terrain_paint_mode = !terrain_paint_mode;
					terrain_transition_change(terrain_paint_mode);
					editorstatus_isValid = false;
				}
			}*/
#ifndef _CHEATDLL_CC
			if (LOWORD(wParam) == 'Q')						//cliff type
			{
				if (short x = GetKeyState(VK_CONTROL))
				{
					cliff_types_ptr++;
					if (!*cliff_types_ptr)
						cliff_types_ptr = cliff_types;
					cliff_type = *cliff_types_ptr;

					setCliffType(cliff_type, scen_ptr);

					editorstatus_isValid = false;
				}
			}
#endif
		}
	}
	if (msg == WM_TIMER)
	{
		if (wParam == AUTOSAVE_TIMER)
		{
			editor_autosave();
			editorstatus_isValid = false;
		}
	}
	if (msg == WM_PAINT)
		repaint = paint_test();

	if (!hWnd_main)
		hWnd_main = hWnd;

	return WndProc_exe(hWnd, msg, wParam, lParam);
}
