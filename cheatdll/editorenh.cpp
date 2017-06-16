#include "stdafx.h"
#include "editorenh.h"
#include "triggerdesc.h"

int placementSettings = 0;

__declspec(naked) void noTerrainRestrictionHook() //00618FEE
{
	__asm
	{
		mov		ecx, placementSettings
		test	ecx, 1					//no terrain restriction
		jnz		_place_unit
		and		eax, 0FFh
		jz		_place_unit
_no_place_unit:
		push	0061922Eh
		ret
_place_unit:
		push	00618FF9h
		ret
	}
}

__declspec(naked) void noGridHook() //00618F90
{
	__asm
	{
		mov		edx, placementSettings
		test	edx, 2					//no grid
		jnz		_no_grid
		mov     edx, [edi]
		push    eax
		mov     eax, [ebx + 108h]
		push	00618F99h
		ret
_no_grid:
		push	00618FA9h
		ret
	}
}

__declspec(naked) void removeUnitsFix() //0049539F
{
	__asm
	{
		mov     ecx, [ebx]
		test	ecx, ecx
		jz		_no_remove
		mov     eax, [ecx + 14h]
		test	eax, eax
		jz		_no_remove
		mov     ax, [eax + 18h]
		push	004953A8h
		ret
_no_remove:
		push	004954A0h
		ret
	}
}

__declspec(naked) void __stdcall window_setRect(void*, int, int, int, int)
{
	__asm
	{
		pop		eax		//ret addr
		pop		ecx		//this
		push	eax
		mov		edx, [ecx]
		jmp		dword ptr [edx + 28h]
	}
}

void* __stdcall getEffectParams_hook(void* _this, effect* e)
{
	switch (e->id)
	{
	case 0x2A:		//change unit data
	case 0x2B:		//change property object
	case 0x2D:		//change unit variable
		//message hlp
		window_setRect(*(void**)((int)_this + 0xEB4), 568, 2, 0x96, 0x14);
		//message
		window_setRect(*(void**)((int)_this + 0xEB8), 568, 0x16, 0x14A, 0x3C);
		//obj list type hlp
		window_setRect(*(void**)((int)_this + 0xE58), 0xE6, 0x3C, 0x96, 0x14);
		//obj list type
		window_setRect(*(void**)((int)_this + 0xE5C), 0xE6, 0x50, 0x96, 0x14);
		break;
	case 0x12:		//change ownership
		//obj list type hlp
		window_setRect(*(void**)((int)_this + 0xE58), 0x181, 0x67, 0x96, 0x14);
		//obj list type
		window_setRect(*(void**)((int)_this + 0xE5C), 0x181, 0x7B, 0x96, 0x14);
		break;
	default:
		//timer hlp
		window_setRect(*(void**)((int)_this + 0xE28), 0xE6, 2, 0x96, 0x14);
		//timer
		window_setRect(*(void**)((int)_this + 0xE2C), 0xE6, 0x16, 0x5A, 0x14);
		//obj list type hlp
		window_setRect(*(void**)((int)_this + 0xE58), 0xE6, 0x3C, 0x96, 0x14);
		//obj list type
		window_setRect(*(void**)((int)_this + 0xE5C), 0xE6, 0x50, 0x96, 0x14);
		//message hlp
		window_setRect(*(void**)((int)_this + 0xEB4), 0xE6, 0x41, 0x96, 0x14);
		//message
		window_setRect(*(void**)((int)_this + 0xEB8), 0xE6, 0x55, 0x14A, 0x3C);
		break;
	}
	return _this;
}

void(__thiscall* window_setText) (void*, int, int) =
	(void(__thiscall*) (void*, int, int)) 0x004C4F80;

void* __stdcall getConditionParams_hook(void* _this, condition* c)
{
	switch (c->id)
	{
	case 0x1A:		//alliance state
		//timer hlp
		window_setRect(*(void**)((int)_this + 0xE28), 0xE6, 0x32, 0x96, 0x14);
		//window_setText(*(void**)((int)_this + 0xE28), 0, 10718);
		//timer
		window_setRect(*(void**)((int)_this + 0xE2C), 0xE6, 0x46, 0x5A, 0x14);
		break;
	default:
		//timer hlp
		window_setRect(*(void**)((int)_this + 0xE28), 0xE6, 2, 0x96, 0x14);
		//window_setText(*(void**)((int)_this + 0xE28), 0, 10723);
		//timer
		window_setRect(*(void**)((int)_this + 0xE2C), 0xE6, 0x16, 0x5A, 0x14);
		//obj list type hlp
		window_setRect(*(void**)((int)_this + 0xE58), 0xE6, 0x3C, 0x96, 0x14);
		//obj list type
		window_setRect(*(void**)((int)_this + 0xE5C), 0xE6, 0x50, 0x96, 0x14);
		break;
	}
	return _this;
}

__declspec(naked) void getEffectParams_new()
{
	__asm
	{
		mov		eax, [esp + 4] //params
		push	eax
		push	ecx
		call	getEffectParams_hook
		mov		ecx, eax
		mov		eax, 0053C920h
		jmp		eax
	}
}

__declspec(naked) void getConditionParams_new()
{
	__asm
	{
		mov		eax, [esp + 4] //params
		push	eax
		push	ecx
		call	getConditionParams_hook
		mov		ecx, eax
		mov		eax, 0053C310h
		jmp		eax
	}
}

void setEditorEnhHooks()
{
	setHook((void*)0x00618FEE, &noTerrainRestrictionHook);
	setHook((void*)0x00618F90, &noGridHook);
	setHook((void*)0x0049539F, &removeUnitsFix);

	//set control locations
	//message input
	//setByte(0x0053833E, 2);		//yhlp
	//setByte(0x00538357, 0x16);	//ymain
	//setInt(0x00538342, 565);
	//setInt(0x0053835B, 565);
	setInt(0x0053C2E4, (int)&getEffectParams_new - 0x0053C2E8);
	setInt(0x0053DD6B, (int)&getConditionParams_new - 0x0053DD6F);
	setInt(0x0053C26F, (int)&getConditionParams_new - 0x0053C273);
}
