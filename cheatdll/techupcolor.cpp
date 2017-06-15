#include "stdafx.h"

#include "techupcolor.h"

void setTechUpColorHooks ()
{
#ifdef _DEBUG
	log ("Setting tech up player colour hooks...");
#endif

	setHook ((void*)0x005E9956, &beforeNotify);
	setHook ((void*)0x005E9995, &afterNotify);
}

__declspec(naked) int beforeNotify () //put on 005E9956
{
	__asm
	{
		push    ebx
		push    edx

		mov     ebx, setByte

		push    57h         //push edi
		push    004F9AFBh
		call    ebx
		push    90h         //nop
		push    004F9AFCh
		call    ebx
		add     esp, 10h

		pop     edx
		pop     ebx
		mov     eax, [edx + 4Ch]
		mov     ecx, [eax + edi * 4]

		push    005E995Ch
		ret
	}
}

__declspec(naked) int afterNotify () //put on 005E9995
{
	__asm
	{
		push    ebx

		mov     ebx, setByte

		push    6Ah         //push
		push    004F9AFBh
		call    ebx
		push    0FFh        //-1
		push    004F9AFCh
		call    ebx
		add     esp, 10h

		pop     ebx
		mov     ecx, [esi + 420h]

		push    005E999Bh
		ret
	}
}
