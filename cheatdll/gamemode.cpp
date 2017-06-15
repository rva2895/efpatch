#include "stdafx.h"

#include "gamemode.h"

int selectGameModeListJump [] =
{
	0x51F955,
	0x51F934,
	0x51F938,
	0x51F930,
	0x51F948,
	0x51F93C,
	0x51F940,
	0x51F944,
	0xFFFFFFFF     //new, edit
};

__declspec(naked) int newModeItem ()   //put on 51F908
{
	__asm
	{
		mov  ecx, [esi+0AF0h]
		push 7
		push 62000
		mov  eax, 4C82D0h
		call eax
		mov  ecx, [esi+0AF0h]
		push 51F90Eh
		ret
	}
}

__declspec(naked) int pushMode ()
{
	__asm
	{
		push 7
		push 51F957h
		ret
	}
}

__declspec(naked) int hookGameModeSet () //put on 521B5D
{
	__asm
	{
		cmp  eax, 7
		jnz  end
		push 8
		push 521B0Dh
		ret
end:
		mov  ecx, [esi+0B10h]
		push 521B63h
		ret
	}
}

void setGameModeHooks ()
{
	/*setByte (0x51F926, 8);
	setInt (0x51F92C, (int)selectGameModeListJump);
	selectGameModeListJump[8] = (int)&pushMode;
	setHook ((void*)0x51F908, &newModeItem);
	setHook ((void*)0x521B5D, &hookGameModeSet);*/
}
