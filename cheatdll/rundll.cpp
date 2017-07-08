#include "stdafx.h"
#include "rundll.h"
#include "autosave.h"
#include "editorstatus.h"
#include "crashreporter.h"

#include <CrashRpt.h>

extern crash_rpt::CrashRpt* g_crashRpt;

extern int placementSettings;

extern bool editorstatus_isValid;

int CALLBACK WndProc_dll(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

__declspec(dllexport) void CALLBACK RunGame (HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	MessageBox (hwnd, lpszCmdLine, "Rundll", 0);
	STARTUPINFO si;
	memset (&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	SetCurrentDirectory (lpszCmdLine);
	CreateProcess ("battlegrounds_x2.exe",
		0,
		0,
		0,
		FALSE,
		0,
		0,
		0,
		&si,
		0);
	//HMODULE gbg = LoadLibrary ("battlegrounds_x2.exe");
}

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

int CALLBACK WndProc_dll(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	bool repaint = false;

	if (msg == WM_KEYDOWN)
	{
		if (LOWORD(wParam) == 'S')
		{
			if (short x = GetKeyState(VK_CONTROL))
			{
				placementSettings++;
				if (placementSettings > 3)
					placementSettings = 0;
				editorstatus_isValid = false;
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
	if (msg == WM_PAINT)
		repaint = paint_test();

	if (!hWnd_main)
		hWnd_main = hWnd;

	return WndProc_exe(hWnd, msg, wParam, lParam);

	//if (repaint)
	//	InvalidateRect(hWnd, 0, true);
}
