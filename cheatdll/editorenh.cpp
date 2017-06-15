#include "stdafx.h"
#include "editorenh.h"

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

void setEditorEnhHooks()
{
	setHook((void*)0x00618FEE, &noTerrainRestrictionHook);
	setHook((void*)0x00618F90, &noGridHook);
	setHook((void*)0x0049539F, &removeUnitsFix);
}
