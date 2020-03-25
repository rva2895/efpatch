#include "stdafx.h"
#include "rundll.h"
#include "autosave.h"
#include "editorstatus.h"
#include "cliff.h"
#include "terrain.h"
#include "crashreporter.h"
#include "registry.h"
#include "palette.h"
#include "rec.h"
#include "overlay.h"

#ifndef TARGET_VOOBLY

#include <CrashRpt.h>

extern crash_rpt::CrashRpt* g_crashRpt;

extern int placementSettings;
extern int cliff_type;

extern bool editorstatus_isValid;

extern void* scen_ptr;

extern CONFIG_DATA cd;

int CALLBACK WndProc_dll(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

int (WINAPI* WinMain_exe) (HINSTANCE, HINSTANCE, LPSTR, int) =
	(int (WINAPI*) (HINSTANCE, HINSTANCE, LPSTR, INT)) 0x0048EFC0;

extern "C" __declspec(dllexport) int WINAPI WinMain_dll(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

	log("WinMain_dll called");

#ifndef _CHEATDLL_CC
	installPalette();
#endif

	initialSetup();

	writeDword(0x00426509, (DWORD)&WndProc_dll);

	int retval;

#ifdef _CHEATDLL_CC
	cd.crashReporting = 0;
#endif

	if (cd.crashReporting)
	{
		log("Crash reporting is ON");
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
	}
	else
	{
		log("Crash reporting is OFF");
		retval = WinMain_exe(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	}

	log("WinMain_exe returned %d, exiting", retval);
	return retval;
}

int (CALLBACK* WndProc_exe) (HWND, UINT, WPARAM, LPARAM) =
	(int (CALLBACK*) (HWND, UINT, WPARAM, LPARAM)) 0x00426530;

HWND hWnd_main = 0;

int cliff_types[] = { 0x108, 3971, 3981, 3991, 4196, 4206, 4216, 4226, 4236, 0};
int* cliff_types_ptr = cliff_types;

extern bool isEditor;

extern DWORD window_editorbk;

__declspec(naked) void __stdcall update_editor_bk()
{
	__asm
	{
		mov     ecx, window_editorbk
		mov     eax, [ecx + 18h]
		mov     edx, [ecx + 14h]
		push    eax
		push    edx
		mov     eax, [ecx]
		call    dword ptr [eax + 5Ch]

		mov     ecx, window_editorbk
		push    1
		mov     eax, [ecx]
		call    dword ptr [eax + 2Ch]
		ret
	}
}

extern bool rec_cache_invalid;

__declspec(naked) void __stdcall update_window(void* wnd)
{
	__asm
	{
		mov     ecx, [esp + 4]
		mov     eax, [ecx + 18h]
		mov     edx, [ecx + 14h]
		//push    eax
		//push    edx
		//mov     eax, [ecx]
		//call    dword ptr[eax + 5Ch]

		mov     ecx, [esp + 4]
		push    1
		mov     eax, [ecx]
		call    dword ptr[eax + 2Ch]
		ret     4
	}
}

HWND hWnd_global;

int terrain_paint_mode = 0;	//default

int CALLBACK WndProc_dll(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	hWnd_global = hWnd;

	//
	//if ((msg == WM_ACTIVATE) && (LOWORD(wParam) == WA_INACTIVE))
	//{
	//	return 0;
	//}
	//
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
			if (!editorstatus_isValid)
			{
				if (window_editorbk)
					update_editor_bk();

				editorstatus_isValid = true;
			}
		}
		else
		{
			if ((LOWORD(wParam) >= '1') && (LOWORD(wParam) <= '9')) //rec switch player
			{
				//if (short x = GetKeyState(VK_MENU))
				//{
					recSwitch(LOWORD(wParam)-0x30);
				//}
			}
			if (LOWORD(wParam) == VK_F8)						//rec overlay
			{
				overlay_switch();
			}
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
	if (msg == WM_APP + 1000) //updatebk
	{
		if (!rec_cache_invalid)
			update_window((void*)wParam);
	}

	if (!hWnd_main)
		hWnd_main = hWnd;

	return WndProc_exe(hWnd, msg, wParam, lParam);
}
#endif