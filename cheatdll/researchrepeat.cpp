#include "stdafx.h"

#include "researchrepeat.h"

__declspec(naked) void removeCheckResearched() //005BFF0B
{
	writeByte(0x005BFF0B, 0x0F);
	//writeByte (0x005BFF0C, 0x8D);
	//writeByte (0x005BFF0D, 0x70);
	//writeByte (0x005BFF0E, 0x02);
	//writeByte (0x005BFF0F, 0x00);
	writeDword(0x005BFF0C, 0x0002708D);
	writeByte(0x005BFF10, 0x00);
	__asm
	{
		mov     eax, 005BFF11h
		jmp     eax
	}
}

__declspec(naked) void removeCheckCiv() //005BFF25
{
	writeByte(0x005BFF25, 0x0F);
	writeByte(0x005BFF26, 0x8E);
	writeByte(0x005BFF27, 0x56);
	writeByte(0x005BFF28, 0x02);
	writeByte(0x005BFF29, 0x00);
	writeByte(0x005BFF2A, 0x00);
	__asm
	{
		mov     ecx, 005BFF2Bh
		jmp     ecx
	}
}

__declspec(naked) void readQuant() //005F2F72
{
	__asm
	{
		push    edx
		push    esi
		mov     esi, [edi + 0Ch]
		test    esi, 0FFFFFFFCh
		jnz     noRemoveCiv
		test    esi, 1
		jz      noRemoveResearched
		push    removeCheckResearched
		push    005BFF0Bh
		call    setHook
		add     esp, 8
noRemoveResearched:
		test    esi, 2
		jz      noRemoveCiv
		push    removeCheckCiv
		push    005BFF25h
		call    setHook
		add     esp, 8
noRemoveCiv:
		pop     esi
		pop     edx
		mov     ecx, [edx + 1D94h]
		mov     eax, 005F2F78h
		jmp     eax
	}
}

void setResearchRepeatHooks()
{
	setHook((void*)0x005F2F72, readQuant);
}

void __stdcall removeCheckResearchedOnce()
{
	setHook((void*)0x005BFF0B, removeCheckResearched);
}
