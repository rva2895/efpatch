#include "stdafx.h"

const float flt_half = 0.5f;

__declspec(naked) void sub_7D0200()
{
	__asm
	{
		mov     eax, [esp + 4]
		push    0
		push    ecx
		push    edx
		fld     dword ptr[eax + 50h]
		fmul    ds:flt_half
		fld     dword ptr[eax + 48h]
		fadd    st, st(1)
		fstp    [esp]
		fld     dword ptr[eax + 4Ch]
		fsubrp  st(1), st
		fstp    [esp + 4]
		call    player_scrollView
		retn    4
	}
}

__declspec(naked) void sub_7D0230()
{
	__asm
	{
		mov     eax, [ecx + 1C4h]
		test    eax, eax
		jz      short loc_7D0257
		fld     dword ptr [eax + 50h]
		fmul    ds:flt_half
		fld     [esp + 4]
		fadd    st, st(1)
		fstp    [esp + 4]
		fld     [esp + 8]
		fsubrp  st(1), st
		fstp    [esp + 8]
loc_7D0257:
		jmp     player_scrollView
	}
}

__declspec(naked) void onElevation1() //005D14C6
{
	__asm
	{
		push    edi
		mov     ecx, esi
		call    sub_7D0200
		mov     ecx, 005D14D7h
		jmp     ecx
	}
}

extern void* (__thiscall* global_getCurrentPlayer) (void* globalPtr);

__declspec(naked) void onElevation2() //00502017
{
	__asm
	{
		push    esi
		mov     ecx, 006A3684h
		mov     ecx, [ecx]
		call    global_getCurrentPlayer
		mov     ecx, eax
		call    sub_7D0200
		mov     eax, 00502033h
		jmp     eax
	}
}

__declspec(naked) void onElevation3() //0050610E
{
	__asm
	{
		push    esi
		mov     ecx, 006A3684h
		mov     ecx, [ecx]
		call    global_getCurrentPlayer
		mov     ecx, eax
		call    sub_7D0200
		mov     eax, 0050612Ah
		jmp     eax
	}
}

__declspec(naked) void onElevation4() //005061EE
{
	__asm
	{
		push    esi
		mov     ecx, 006A3684h
		mov     ecx, [ecx]
		call    global_getCurrentPlayer
		mov     ecx, eax
		call    sub_7D0200
		mov     eax, 0050620Ah
		jmp     eax
	}
}

__declspec(naked) void onElevation5() //005062D4
{
	__asm
	{
		push    esi
		mov     ecx, 006A3684h
		mov     ecx, [ecx]
		call    global_getCurrentPlayer
		mov     ecx, eax
		call    sub_7D0200
		mov     eax, 005062F0h
		jmp     eax
	}
}

#pragma optimize( "s", on )
void setElevationHooks()
{
	writeDword(0x00502509, (DWORD)&sub_7D0230 - 0x0050250D);
	writeDword(0x004FA309, (DWORD)&sub_7D0230 - 0x004FA30D);
	writeDword(0x0050688C, (DWORD)&sub_7D0230 - 0x00506890); //gather point

	setHook((void*)0x005D14C6, onElevation1);
	setHook((void*)0x00502017, onElevation2);
	setHook((void*)0x0050610E, onElevation3);
	setHook((void*)0x005061EE, onElevation4);
	setHook((void*)0x005062D4, onElevation5);
}
#pragma optimize( "", on )
