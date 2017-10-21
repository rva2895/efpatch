#include "stdafx.h"

void (__thiscall* setGameSpeed) (void* this_, float spd) =
	(void(__thiscall*) (void*, float))0x005BD6B0;

float game_speeds[] =
{
	0.5, 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 10.0
};

void __stdcall setGameSpeed_new(void* this_, float spd, int isInc)
{
	int i;
	for (i = 0; (i < sizeof(game_speeds) / sizeof(game_speeds[0])) && (spd > game_speeds[i]); i++)
		;
	if (isInc) i++; else i--;
	if ((i >= 0) && (i < sizeof(game_speeds) / sizeof(game_speeds[0])))
		setGameSpeed(this_, game_speeds[i]);
}

__declspec(naked) void onCheckGameSpeed() //0050189E
{
	__asm
	{
		push	eax
		push	edx
		mov		ecx, [esi + 68h]
		push	ecx
		call	setGameSpeed_new
		pop		edi
		pop		esi
		pop		ecx
		retn	4
	}
}

void setGameSpeedHooks()
{
	setHook((void*)0x0050189E, onCheckGameSpeed);
}
