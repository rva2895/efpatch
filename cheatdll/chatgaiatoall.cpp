#include "stdafx.h"

#include "chatgaiatoall.h"

__declspec(naked) int srcPlayerCheck () //put on 005F3049
{
	__asm
	{
		mov   ecx, [edi + 28h]
		test  ecx, ecx
		jz    toSendChat

		_emit 0x39      //cmp     [esp+55F0h+player], eax
		_emit 0x44
		_emit 0x24
		_emit 0x14
		jnz   toDefault

toSendChat:
		push  005F3053h
		ret

toDefault:
		push  005F3DB1h
		ret
	}
}

void setChatGaiaToAllHooks ()
{
#ifdef _DEBUG
	log ("Setting gaia -> chat to all hooks...");
#endif
	setHook ((void*)0x005F3049, &srcPlayerCheck);
}
