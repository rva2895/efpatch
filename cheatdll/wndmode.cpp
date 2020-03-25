#include "stdafx.h"

#include "wndmode.h"

int isTechTree = 0;

__declspec(naked) void onTechTreeCreate() //00462352
{
	__asm
	{
		xor     eax, eax
		inc     eax
		mov     isTechTree, eax
		lea     eax, [esi + 1C4Ch]
		mov     ecx, 00462358h
		jmp     ecx
	}
}

__declspec(naked) void onTechTreeDestroy() //00463F68
{
	__asm
	{
		xor     eax, eax
		mov     isTechTree, eax
		mov     eax, esi
		pop     esi
		ret     4
	}
}

char* cmdline = 0;
bool normalmouse = false;

void __stdcall fixCur(HWND hWnd, LPRECT rect)
{
	if (!cmdline)
	{
		cmdline = GetCommandLine();
		if (strstr(cmdline, "NORMALMOUSE"))
			normalmouse = true;
	}
	GetClientRect(hWnd, rect);
	if (!normalmouse)
		if (!isTechTree)
			SetCursor(0);
}

__declspec(naked) void wndtmp() //00616C0F
{
	__asm
	{
		push    00616C15h
		jmp     fixCur
	}
}

void setWndModeHooks()
{
	if (LoadLibrary("wndmode.dll"))
		log("Loaded wndmode.dll");
	else
		log("No wndmode.dll. Using ddraw window mode if possible");

	setHook((void*)0x00616C0F, wndtmp);

	writeByte(0x0047166E, 0x90);
	writeByte(0x0047166F, 0x90);
	writeDword(0x00471670, 0x90909090);
	writeDword(0x00471674, 0x90909090);
	writeDword(0x00471678, 0x90909090);
	writeDword(0x0047167C, 0x90909090);
	writeByte(0x00471680, 0x90);
	writeByte(0x00471681, 0x90);

	//tech tree mouse
	setHook((void*)0x00462352, onTechTreeCreate);
	setHook((void*)0x00463F68, onTechTreeDestroy);
}
