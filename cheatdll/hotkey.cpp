#include "stdafx.h"
#include "hotkey.h"

__declspec(naked) void keydown_1() //004C61D4
{
	__asm
	{
		xor     ebx, ebx
		mov     edx, [esi + 148h]
		cmp     [esp + 20h], ebx
		jz      short loc_7C41D2
		mov     edi, [esi + 144h]
		mov     eax, [esi + 140h]
		test    edi, edi
		jnz     short loc_7C41FA
		test    eax, eax
		jnz     short loc_7C41FA

loc_7C41D2:
		cmp     edx, 2
		jnz     short loc_7C41E5
		mov     edi, [esi + 11Ch]
		mov     eax, [esi + 0F8h]
		jmp     short loc_7C41FA

loc_7C41E5:
		movsx   eax, word ptr [esi + 202h]
		mov     edi, [esi + eax * 4 + 11Ch]
		mov     eax, [esi + eax * 4 + 0F8h]

loc_7C41FA:
		cmp     dword ptr [esi + 14h], 29h
		jnz     short loc_7C4201
		dec     ebx

loc_7C4201:
		mov     edx, [ecx]
		push    edi
		add     ebx, 2
		push    eax
		push    ebx
		push    esi
		call    dword ptr [edx + 0D0h]
		pop     edi
		pop     esi
		mov     eax, 1
		pop     ebx
		retn    14h
	}
}

__declspec(naked) void keydown_2() //004C65DF
{
	__asm
	{
		mov     ecx, [esi + 40h]
		mov     edx, [esp + 0Ch]
		test    ecx, ecx
		jz      short loc_7C4253
		cmp     edx, 004C5E8Fh
		mov     edx, [ecx]
		jz      short loc_7C4259
		cmp     dword ptr [esi + 14h], 29h
		jnz     short loc_7C4259
		cmp     edx, 0065FE48h//00659D4Ch
		jnz     short loc_7C4259

loc_7C4253:
		pop     edi
		pop     esi
		pop     ebx
		retn    4

loc_7C4259:
		push    ebx
		push    edi
		mov     eax, 004C65EAh
		jmp     eax
	}
}

void setHotkeyHooks()
{
	setHook((void*)0x004C61D4, keydown_1);
	setHook((void*)0x004C65DF, keydown_2);
}
