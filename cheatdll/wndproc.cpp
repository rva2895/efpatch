#include "stdafx.h"

#include "iuserpatch.h"
#include "wndproc.h"
#include "rec.h"

#ifdef TARGET_VOOBLY_
extern IVoobly *g_pVoobly;

int terrain_paint_mode = 0;

HWND hWnd_main;

int (CALLBACK* WndProc_exe) (HWND, UINT, WPARAM, LPARAM) =
	(int (CALLBACK*) (HWND, UINT, WPARAM, LPARAM)) 0x00426530;

int CALLBACK WndProc_dll(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	hWnd_main = hWnd;

	if (msg == WM_SYSKEYDOWN)
	{

		if ((LOWORD(wParam) >= '1') && (LOWORD(wParam) <= '9')) //rec switch player
		{
			//if (short x = GetKeyState(VK_SHIFT))
			//{
			recSwitch(LOWORD(wParam) - 0x30);
			//}
		}
	}

	return WndProc_exe(hWnd, msg, wParam, lParam);
}

void setWndProcHooks()
{
	g_pVoobly->Write(0x00426509, (DWORD)&WndProc_dll);
}
#endif