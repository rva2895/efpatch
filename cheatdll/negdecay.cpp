#include "stdafx.h"

#include "negdecay.h"

__declspec(naked) int negDecay()  //put on 0054F123
{
	__asm
	{
		jnz     jump
		fld     dword ptr [esi + 54h]
		fcomp   dword ptr [ecx + 80h]
		fnstsw  ax
		test    ah, 41h
		jnz     jumpgood
		mov     eax, [ecx + 80h]
		mov     [esi + 54h], eax
jumpgood:
		mov     ecx, 0054F1B6h
		jmp     ecx
jump:
		mov     ecx, 0054F129h
		jmp     ecx
	}
}

void setDecayHooks()
{
	setByte(0x54F0F6, 0x40);
	setHook((void*)0x0054F123, negDecay);
}
