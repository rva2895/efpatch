#include "stdafx.h"

#define DARK_GREY_COLOR 11

__declspec(naked) void onMinimapGrey() //005F8E72
{
	__asm
	{
		mov     eax, [ebx + 164h]
		mov     al, [eax + 20h]
		cmp     al, 84h
		jnz     _not_grey
		mov     al, DARK_GREY_COLOR
_not_grey:
		mov     ecx, 005F8E7Bh
		jmp     ecx
	}
}

__declspec(naked) void onMinimapGrey_1() //005F9EAC
{
	__asm
	{
		mov     ecx, [eax + 164h]
		mov     ebx, [ecx + 20h]
		cmp     bl, 84h
		jnz     _not_grey1
		mov     bl, DARK_GREY_COLOR
_not_grey1:
		mov     ecx, 005F9EB5h
		jmp     ecx
	}
}

__declspec(naked) void onMinimapGrey_2() //005F9F05
{
	__asm
	{
		mov     eax, [ecx + 164h]
		mov     ebx, [eax + 20h]
		cmp     bl, 84h
		jnz     _not_grey2
		mov     bl, DARK_GREY_COLOR
_not_grey2:
		mov     ecx, 005F9F0Eh
		jmp     ecx
	}
}

void setMinimapHooks()
{
	//setHook((void*)0x005F8E72, &onMinimapGrey);
	//setHook((void*)0x005F9EAC, &onMinimapGrey_1);
	//setHook((void*)0x005F9F05, &onMinimapGrey_2);
}
