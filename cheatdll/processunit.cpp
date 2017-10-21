#include "stdafx.h"

#include "processunit.h"
#include "casts.h"

void __stdcall airRegen (float* hp_leftover, int timerRes)
{
	//*hp_leftover += *(float*)&timerRes * 10.0f;
}

__declspec(naked) void _type70 () //0055A760
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
		mov     eax, dword ptr [eax+0A8h]
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

__declspec(naked) void regenFix () //0055A974
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

/*trigger_data* trigger;

void build_table ()
{
	trigger = new trigger_data;
	memset (trigger, -1, sizeof(trigger_data));
	trigger->table_ptr = new char* [0x2F];
	for (int i = 0; i < 0x2F; i++)
	{
		trigger->table_ptr[i] = new char [0x17];
		memset (trigger->table_ptr[i], 0, 0x17);
	}
	trigger->effect = 0;
	trigger->zero1 = 0;
	trigger->zero2 = 0;
	trigger->zero3 = 0;
}*/

__declspec(naked) void __stdcall player_createUnitAt (void* unit, int id)
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

void __stdcall make_unit_extra (void* unit)
{
	/*short id = *(short*)(*(int*)((char*)unit + 0x14) + 0x18);
	UNIT_EXTRA* ud;
	if (id == 3610)
	{
		ud = new UNIT_EXTRA;
		memset (ud, 0, sizeof(UNIT_EXTRA));
		ud->spawnMinTime = 30;
		ud->spawnMaxTime = -1;
		ud->spawnEnabled = 1;
		ud->spawnProb = RAND_MAX / 10;
		ud->spawnID = 0x19E;
		addUnitExtra (unit, ud);
	}
	if (id == 0x19E)
		player_createUnitAt (unit, 3610);*/
}

__declspec(naked) void _type10_constructor () //0048D310
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

__declspec(naked) void _type20_constructor () //00488920
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

__declspec(naked) void __stdcall process_effect (void* effect, int game_ptr)
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

/*void __stdcall make_unit (void* unit, int id)
{
	for (int i = 0; i < 0x2F; i++)
		memset (trigger->table_ptr[i], 0, 0x17);

	trigger->effect = 0xB;   //create object
	trigger->table_ptr[trigger->effect][6] = 1;
	trigger->table_ptr[trigger->effect][7] = 1;
	trigger->table_ptr[trigger->effect][0xE] = 1;
	trigger->table_ptr[trigger->effect][0xF] = 1;

	//trigger->Object = *(short*)(*(int*)((char*)unit + 0x14) + 0x18);
	trigger->Object = id;
	trigger->Source_Player = 0;
	trigger->Location_x = *(float*)((int)unit + 0x48);
	trigger->Location_y = *(float*)((int)unit + 0x4C);

	int game_ptr = *(int*)(*(int*)0x6A3684 + 0x17B4);
	if (game_ptr)
	{
		game_ptr = *(int*)(game_ptr + 0x126C);
		process_effect (trigger, game_ptr);
	}
}*/

void setProcessUnitHooks ()
{
	//setHook ((void*)0x0055A760, &_type70);
	setHook ((void*)0x0055A974, &regenFix);

	//setHook ((void*)0x0048D310, &_type10_constructor);
	//setHook ((void*)0x00488920, &_type20_constructor);

	//build_table ();
}
