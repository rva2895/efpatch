#include "stdafx.h"


__declspec(naked) int __stdcall isPlacePowered(char player, int x, int y, int unk)
{
	__asm
	{
		mov     ecx, 006A3684h
		mov     ecx, [ecx]
		mov     ecx, [ecx + 17B4h]
		mov     ecx, [ecx + 126Ch]
		mov     edx, [ecx + 34h]
		mov     ecx, [edx + 0BF18h]		//large map beware
		mov     eax, y
		mov     edx, unk
		mov     ecx, [ecx + eax * 4]
		mov     al, player
		push    edx
		mov     edx, x
		add     ecx, edx
		push    eax
		mov     eax, 0048F970h
		call    eax
		ret     10h
	}
}

void setAIPowerHooks()
{

}

//00585870 -> ai check power