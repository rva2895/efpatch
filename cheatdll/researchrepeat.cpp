#include "stdafx.h"

#include "researchrepeat.h"

__declspec (naked) void removeCheckResearched() //005BFF0B
{
	setByte(0x5BFF0B, 0x0F);
	//setByte (0x5BFF0C, 0x8D);
	//setByte (0x5BFF0D, 0x70);
	//setByte (0x5BFF0E, 0x02);
	//setByte (0x5BFF0F, 0x00);
	setInt(0x5BFF0C, 0x0002708D);
	setByte(0x5BFF10, 0x00);
	__asm
	{
		mov     eax, 005BFF11h
		jmp     eax
	}
}

__declspec (naked) void removeCheckCiv() //005BFF25
{
	setByte(0x5BFF25, 0x0F);
	setByte(0x5BFF26, 0x8E);
	setByte(0x5BFF27, 0x56);
	setByte(0x5BFF28, 0x02);
	setByte(0x5BFF29, 0x00);
	setByte(0x5BFF2A, 0x00);
	__asm
	{
		mov     ecx, 005BFF2Bh
		jmp     ecx
	}
}

__declspec (naked) void readQuant() //put on 5F2F72
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
#ifdef _DEBUG
	log("Setting research repeat hooks...");
#endif
	setHook((void*)0x005F2F72, readQuant);
}

void __stdcall removeCheckResearchedOnce()
{
	setHook((void*)0x005BFF0B, removeCheckResearched);
}
