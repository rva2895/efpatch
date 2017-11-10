#include "stdafx.h"
#include "advcheat.h"

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
		push    eax
		push    edx
		mov     ecx, [esi + 68h]
		push    ecx
		call    setGameSpeed_new
		pop     edi
		pop     esi
		pop     ecx
		retn    4
	}
}

//recorded game
void __fastcall printSpeed(int speed)
{
	char* s = "";
	switch (speed)
	{
	case 0:
		s = "0.5";
		break;
	case 1:
		s = "0.7";
		break;
	case 2:
		s = "1.0";
		break;
	case 3:
		s = "1.3";
		break;
	case 4:
		s = "1.6";
		break;
	case 5:
		s = "2.0";
		break;
	case 6:
		s = "4.0";
		break;
	case 7:
		s = "unbounded";
		break;
	default:
		s = "";
		break;
	}
	chat("Playback Speed: %s", s);
}

__declspec(naked) void onSlowDown() //004FAF6B
{
	__asm
	{
		mov     ecx, [eax + 540h]
		test    ecx, ecx
		jle     _no_slow_down
		dec     ecx
_no_slow_down:
		mov     [eax + 540h], ecx
		call    printSpeed
		push    004FAF75h
		ret
	}
}

__declspec(naked) void onSpeedUp() //004FAF38
{
	__asm
	{
		mov     ecx, [eax + 540h]
		cmp     ecx, 7
		jge     _no_speed_up
		inc     ecx
_no_speed_up:
		mov     [eax + 540h], ecx
		call    printSpeed
		push    004FAF42h
		ret
	}
}

__declspec(naked) void onSpeedNormal() //004FAF9F
{
	__asm
	{
		mov     ecx, 2
		mov     [eax + 540h], ecx
		call    printSpeed
		push    004FAFA9h
		ret
	}
}

__declspec(naked) void onCheckSpeed() //0061EB93
{
	__asm
	{
		cmp     eax, 1
		jnz     short _next1
		mov     eax, [ebp - 10h]		//1.5
		lea     eax, [eax + eax * 2]
		shr     eax, 1
		jmp     _end
_next1:
		cmp     eax, 2
		jnz     _next2
		mov     eax, [ebp - 10h]		//1.0
		jmp     _end
_next2:
		cmp     eax, 3
		jnz     _next3
		mov     eax, [ebp - 10h]		//0.75
		lea     eax, [eax + eax * 2]
		shr     eax, 2
		jmp     _end
_next3:
		cmp     eax, 3
		jnz     _next4
		mov     eax, [ebp - 10h]		//0.625
		lea     eax, [eax + eax * 4]
		shr     eax, 3
		jmp     _end
_next4:
		cmp     eax, 4
		jnz     _next5
		mov     eax, [ebp - 10h]		//0.5
		shr     eax, 1
		jmp     _end
_next5:
		cmp     eax, 5
		jnz     _next6
		mov     eax, [ebp - 10h]		//0.25
		shr     eax, 2
_end:
		mov     [edi + 204h], eax
_next6:
		push    0061EBA1h
		ret
	}
}

void setGameSpeedHooks()
{
	setHook((void*)0x0050189E, onCheckGameSpeed);

	//recorded game
	setByte(0x0061EBC2, 7);
	setHook((void*)0x0061EB93, onCheckSpeed);
	setHook((void*)0x004FAF6B, onSlowDown);
	setHook((void*)0x004FAF38, onSpeedUp);
	setHook((void*)0x004FAF9F, onSpeedNormal);
}
