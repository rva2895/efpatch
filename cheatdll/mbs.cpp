#include "stdafx.h"

//sub_445A10 -> sub_5EF170

__declspec(naked) void sub_5EF170_new()
{
	__asm
	{
		mov     eax, [ecx + 1440h]
		and     eax, 7
		retn
	}
}

__declspec(naked) void mbs_1() //00617D6B, 13945
{
	__asm
	{
		mov     edx, [edx + 18h]	//0Ch -> 18h
		cmp     [eax + 18h], edx	//should be player vtable
		jnz     short loc_413967
		mov     eax, 004C3530h
		call    eax
		mov     edx, 00617D70h
		jmp     edx
loc_413967:
		mov     ecx, 00617D7Fh
		jmp     ecx
	}
}

__declspec(naked) void mbs_2() //00601DEA, 29EFD
{
	__asm
	{
		cmp     eax, 0FFFFFFFDh
		jz      short loc_7AEFA1
		dec     eax
		jnz     loc_42A02E; jumptable 00429FA8 default case
		jmp     loc_429F04

loc_7AEFA1:
		cmp     [ebp + 48h], bx
		mov     esi, ebx
		jle     loc_42A02E; jumptable 00429FA8 default case
		mov     ecx, [ebp + 4Ch]
		mov     eax, [ecx]
		mov     ecx, [ebp + 6Ch]
		test    eax, eax
		jz      short loc_7AEFCD
		movsx   edi, word ptr [eax + 0CF6h]
		push    1
		mov     [ecx + 1B84h], edi		//offset is the same
		mov     eax, 005F6D20h	//call    sub_435C00
		call    eax
loc_7AEFCD:
		mov     eax, [ebp + 4Ch]
		push    1
		push    edi
		push    7
		push    0Ah
		mov     ecx, [eax + esi * 4]
		mov     ecx, [ecx + 84h]
		//call    sub_424C70
		mov     eax, 00608520h
		call    eax
		movsx   edx, word ptr [ebp + 48h]
		inc     esi
		cmp     esi, edx
		jl      short loc_7AEFCD
loc_42A02E:
		mov     ecx, 00601F1Bh
		jmp     ecx
loc_429F04:
		mov     esi, 00601DF1h
		jmp     esi
	}
}

#pragma optimize( "s", on )
void setMbsHooks()
{
	writeDword(0x00600F6E, (DWORD)&sub_5EF170_new - 0x00600F72); //28FB1
	//writeDword(0x00602629, (DWORD)&sub_5EF170_new - 0x0060262D); //2A72A, maybe for tech cap?
	//writeByte(0x0060262F, 0xFE);
	//writeByte(0x00602632, 4);
	writeDword(0x00541A0D, (DWORD)&sub_5EF170_new - 0x00541A11);
	writeDword(0x0051E22B, (DWORD)&sub_5EF170_new - 0x0051E22F);
	writeDword(0x004E546A, (DWORD)&sub_5EF170_new - 0x004E546E);
}
#pragma optimize( "", on )
