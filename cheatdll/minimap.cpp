#include "stdafx.h"

#define DARK_GREY_COLOR 11

/*__declspec(naked) void onMinimapGrey() //005F8E72
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
}*/

__declspec(naked) void grey_load_dat_unit() //0048C45F
{
	__asm
	{
		mov     dx, [edi + 18h]
		cmp     dx, 217
		jz      carbon_rock
		cmp     dx, 1117
		jz      carbon_rock
		cmp     dx, 1160
		jz      carbon_rock
		cmp     dx, 1341
		jnz     not_carbon_rock
carbon_rock:
		mov     byte ptr [edi + 9Eh], 0
not_carbon_rock:
		push    1
		lea     edx, [edi + 9Fh]
		mov     eax, 0048C467h
		jmp     eax
	}
}

__declspec(naked) void grey_load_dat_color() //0042E072
{
	__asm
	{
		mov     edx, [esi + 4]
		cmp     edx, 6
		jnz     not_grey
		mov     dword ptr [esi + 20h], DARK_GREY_COLOR
not_grey:
		push    4
		lea     edx, [esi + 24h]
		mov     eax, 0042E077h
		jmp     eax
	}
}

#pragma optimize( "s", on )
void setMinimapHooks()
{
	//setHook((void*)0x005F8E72, onMinimapGrey);
	//setHook((void*)0x005F9EAC, onMinimapGrey_1);
	//setHook((void*)0x005F9F05, onMinimapGrey_2);
	setHook((void*)0x0048C45F, grey_load_dat_unit);
	setHook((void*)0x0042E072, grey_load_dat_color);
}
#pragma optimize( "", on )
