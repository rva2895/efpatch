#include "stdafx.h"

#include "chatgaiatoall.h"

__declspec(naked) int srcPlayerCheck_chat() //005F3049
{
	__asm
	{
		mov     ecx, [edi + 28h]
		test    ecx, ecx
		jz      toSendChat_chat

		cmp     [esp + 14h], eax
		jnz     toDefault_chat

toSendChat_chat:
		mov     eax, 005F3053h
		jmp     eax

toDefault_chat:
		mov     ebx, 005F3DB1h
		jmp     ebx
	}
}

__declspec(naked) int srcPlayerCheck_sound() //005F310C
{
	__asm
	{
		mov     ecx, [edi + 28h]
		test    ecx, ecx
		jz      toSendChat_sound

		cmp     [esp + 14h], eax
		jnz     toDefault_sound

toSendChat_sound:
		mov     eax, 005F3116h
		jmp     eax

toDefault_sound:
		mov     ebx, 005F3DB1h
		jmp     ebx
	}
}


#pragma optimize( "s", on )
void setChatGaiaToAllHooks()
{
	setHook((void*)0x005F3049, srcPlayerCheck_chat);
	setHook((void*)0x005F310C, srcPlayerCheck_sound);
}
#pragma optimize( "", on )
