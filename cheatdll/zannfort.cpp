#include "stdafx.h"

#include "zannfort.h"
#include "researchrepeat.h"

__declspec(naked) void onNewFort () //00554B8A
{
	__asm
	{
		push    ebx
		push    3F800000h
		push    0ADh
		mov     eax, [ecx]
		call    dword ptr [eax+80h]
		call    removeCheckResearchedOnce
		mov     ecx, [esi+18h]
		push    2D6h             //research id
		mov     ecx, [ecx+1D94h]
		mov     eax, 005BFEC9h   //researchTech
		push    00554B9Dh
		jmp     eax
	}
}

void setZannFortHooks ()
{
	//setHook ((void*)0x00554B8A, &onNewFort);
}
