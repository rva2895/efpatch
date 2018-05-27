#include "stdafx.h"
#include "log.h"

#define OOS_STR "Out of sync replaying game: worldtime=%d, continuing anyway"

void __stdcall oos_log(int time)
{
	log(OOS_STR, time);
	chat(OOS_STR, time);
}

__declspec(naked) void rec_oos_hook() //0061EBE1
{
	__asm
	{
		cmp     esi, 3
		jz      _rec_oos
		mov     eax, 0061EC43h
		jmp     eax
		_rec_oos:
		push    ecx
		push    edx
		mov     eax, [ebx]
		push    eax
		call    oos_log
		pop     edx
		pop     ecx
		mov     eax, 0061ED09h
		jmp     eax
	}
}

void setOOSHooks()
{
	setHook((void*)0x0061EBE1, rec_oos_hook);
}
