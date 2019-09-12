#include "stdafx.h"

#include "processunit.h"
#include "casts.h"

#pragma warning(push)
#pragma warning(disable:4100)

void __stdcall airRegen(float* hp_leftover, int timerRes)
{
	UNREFERENCED_PARAMETER(hp_leftover);
	UNREFERENCED_PARAMETER(timerRes);
	//*hp_leftover += *(float*)&timerRes * 10.0f;
}

__declspec(naked) void _type70() //0055A760
{
	__asm
	{
		mov     eax, [ecx + 14h]
		mov     ax, [eax + 1Eh]
		cmp     ax, 48
		jnz     _no_regen
		push    ecx
		mov     eax, [ecx + 18h]
		mov     eax, [eax + 8Ch]
		mov     eax, dword ptr [eax + 0A8h]
		push    eax
		lea     eax, [ecx + 1C0h]
		push    eax
		call    airRegen
		pop     ecx

_no_regen:
		push    ecx
		push    ebx
		push    esi
		mov     esi, ecx
		push    edi

		push    0055A766h
		ret
	}
}

__declspec(naked) void regenFix() //0055A974
{
	__asm
	{
		test    al, al
		jnz     _regenAllowed
		mov     eax, [esi + 14h]
		mov     ax, [eax + 1Eh]
		cmp     ax, 48
		jnz      _regenNotAllowed
		mov     eax, [esi + 18h]
		mov     eax, [eax + 0ACh]
		fld     dword ptr [eax + 350h]
		_emit   0xD8    //compare to 0
		_emit   0x1D
		_emit   0xA4
		_emit   0x45
		_emit   0x65
		_emit   0x00
		fnstsw  ax
		test    ah, 41h
		jz      _regenAllowed
_regenNotAllowed:
		push    0055AA6Fh
		ret
_regenAllowed:
		push    0055A97Ch
		ret
	}
}

__declspec(naked) void __stdcall player_createUnitAt(void* unit, int id)
{
	__asm
	{
		push    ebp
		mov     ebp, esp
		push    1
		push    0
		mov     eax, [ebp + 8]
		mov     ecx, [eax + 4Ch]
		push    ecx
		mov     ecx, [eax + 48h]
		push    ecx
		mov     ecx, [eax + 18h]
		mov     eax, [ebp + 0Ch]
		push    eax
		mov     eax, 004C2D10h
		call    eax
		leave
		ret     8
	}
}

void __stdcall make_unit_extra(void* unit)
{
	/*short id = *(short*)(*(int*)((char*)unit + 0x14) + 0x18);
	UNIT_EXTRA* ud;
	if (id == 3610)
	{
		ud = new UNIT_EXTRA;
		memset(ud, 0, sizeof(UNIT_EXTRA));
		ud->spawnMinTime = 30;
		ud->spawnMaxTime = -1;
		ud->spawnEnabled = 1;
		ud->spawnProb = RAND_MAX / 10;
		ud->spawnID = 0x19E;
		addUnitExtra(unit, ud);
	}
	if (id == 0x19E)
		player_createUnitAt(unit, 3610);*/
}

__declspec(naked) void _type10_constructor() //0048D310
{
	__asm
	{
		push    eax
		push    eax
		call    make_unit_extra
		pop     eax
		add     esp, 0Ch
		ret     10h
	}
}

__declspec(naked) void _type20_constructor() //00488920
{
	__asm
	{
		push    eax
		push    eax
		call    make_unit_extra
		pop     eax
		add     esp, 0Ch
		ret     10h
	}
}

__declspec(naked) void __stdcall process_effect(void* effect, int game_ptr)
{
	__asm
	{
		mov     ecx, [esp + 4]
		mov     eax, [esp + 8]
		push    eax
		mov     eax, 0x005F2AE0
		call    eax
		ret     8
	}
}

#pragma optimize( "s", on )
void setProcessUnitHooks ()
{
	//setHook ((void*)0x0055A760, &_type70);
	setHook ((void*)0x0055A974, regenFix);
}
#pragma optimize( "", on )

#pragma warning(pop)
