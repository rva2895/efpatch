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
		mov		ecx, 0061922Eh
		jmp		ecx
_place_unit:
		mov		ecx, 00618FF9h
		jmp		ecx
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
		mov		ecx, 00618F99h
		jmp		ecx
_no_grid:
		mov		ecx, 00618FA9h
		jmp		ecx
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
		mov		edx, 004953A8h
		jmp		edx
_no_remove:
		mov		edx, 004954A0h
		jmp		edx
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

__declspec(naked) void __stdcall window_setText(void*, char*, int)
{
	__asm
	{
		mov		eax, [esp + 0Ch]
		mov     ecx, [esp + 4]
		push	eax
		mov		edx, [esp + 4 + 8]
		push	edx
		mov		eax, 004D0AF0h
		call	eax
		ret		0Ch
	}
}

char szAmount[] = "Amount";
char szTimer[] = "Timer";
char szAllianceState[] = "Alliance State";
char szTargetPlayer[] = "Target Player";
char szQuantity[] = "Quantity";
char szOptions[] = "Options";

__declspec(naked) void __fastcall flush_ai_trigger_dropdown(void*)
{
	__asm
	{
		push	esi
		mov		esi, ecx
		lea     edx, [esi + 0ED0h]
		mov     ecx, esi
		push    edx
		mov		eax, 00428520h
		call	eax

		mov     ecx, [esi + 918h]
		lea     eax, [esi + 0ED0h]
		push    eax
		push    ecx
		mov     ecx, esi
		mov		eax, 00529E50h
		call	eax

		pop		esi
		ret
	}
}

void(__thiscall* window_dropdown_addText) (void*, char*, int) =
	(void(__thiscall*) (void*, char*, int)) 0x004C82A0;

void* __stdcall getEffectParams_hook(void* _this, effect* e)
{
	//quantity
	window_setText(*(void**)((int)_this + 0xE68), szQuantity, 0);
	switch (e->id)
	{
	case 2:			//research technology
		flush_ai_trigger_dropdown(_this);
		window_dropdown_addText(*(void**)((int)_this + 0xED0), "Default", -1);
		window_dropdown_addText(*(void**)((int)_this + 0xED0), "Allow stacking", 1);
		window_dropdown_addText(*(void**)((int)_this + 0xED0), "Ignore civ restrictions", 2);
		window_dropdown_addText(*(void**)((int)_this + 0xED0), "Stacking/ignore civ restrictions", 3);
		//ai trigger hlp
		window_setRect(*(void**)((int)_this + 0xECC), 0x190, 2, 0xC8, 0x14);
		window_setText(*(void**)((int)_this + 0xECC), szOptions, 0);
		//ai trigger
		window_setRect(*(void**)((int)_this + 0xED0), 0x190, 0x16, 0xC8, 0x14);
		break;
	case 0xA:		//ai script goal
		setByte(0x0052A8F6, 0x8D);
		setByte(0x0052A8F7, 0x4C);
		setInt(0x0052A8F8, 0x0A6A1024);
		flush_ai_trigger_dropdown(_this);
		setHook((void*)0x0052A8F6, (void*)0x0052A943);	//remove ai trigger dropdown init
		//ai trigger hlp
		window_setRect(*(void**)((int)_this + 0xECC), 4, 0x3C, 0xC8, 0x14);
		window_setText(*(void**)((int)_this + 0xECC), "AI Trigger Number", 0);
		//ai trigger
		window_setRect(*(void**)((int)_this + 0xED0), 4, 0x50, 0xC8, 0x14);
		break;
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

__declspec(naked) void __fastcall flush_ai_signal_dropdown(void*)
{
	__asm
	{
		push	esi
		mov		esi, ecx
		lea     edx, [esi + 0ED8h]
		mov     ecx, esi
		push    edx
		mov		eax, 00428520h
		call	eax

		mov     ecx, [esi + 918h]
		lea     eax, [esi + 0ED8h]
		push    eax
		push    ecx
		mov     ecx, esi
		mov		eax, 00529E50h
		call	eax

		pop		esi
		ret
	}
}

char* var_names[] =
{
	"HP",
	"HPPercent",
	"SP",
	"SPPercent",
	"ReloadCooldown",
	"ReloadCooldownPercent",
	"Resources",
	"Garrison"
};

void* __stdcall getConditionParams_hook(void* _this, condition* c)
{
	switch (c->id)
	{
	case 5:			//objects in area
		flush_ai_signal_dropdown(_this);
		window_dropdown_addText(*(void**)((int)_this + 0xED8), "Default", -1);
		window_dropdown_addText(*(void**)((int)_this + 0xED8), "Ungarrisoned only", 1);
		window_dropdown_addText(*(void**)((int)_this + 0xED8), "Garrisoned only", 2);
		//ai signal hlp
		window_setRect(*(void**)((int)_this + 0xED4), 0x240, 2, 0xC8, 0x14);
		window_setText(*(void**)((int)_this + 0xED4), szOptions, 0);
		//ai signal
		window_setRect(*(void**)((int)_this + 0xED8), 0x240, 0x16, 0xC8, 0x14);
		break;
	case 0xC:		//ai signal
		setByte(0x0052A977, 0x8D);
		setInt(0x0052A978, 0x6A102444);
		flush_ai_signal_dropdown(_this);
		setHook((void*)0x0052A977, (void*)0x0052A9C4);
		//ai signal hlp
		window_setRect(*(void**)((int)_this + 0xED4), 4, 0x3C, 0xC8, 0x14);
		window_setText(*(void**)((int)_this + 0xED4), "AI Signal Value", 0);
		//ai signal
		window_setRect(*(void**)((int)_this + 0xED8), 4, 0x50, 0xC8, 0x14);
		break;
	case 0x1A:		//alliance state
		//timer hlp
		window_setRect(*(void**)((int)_this + 0xE28), 0xE6, 0x32, 0x96, 0x14);
		window_setText(*(void**)((int)_this + 0xE28), szTargetPlayer, 0);
		//timer
		window_setRect(*(void**)((int)_this + 0xE2C), 0xE6, 0x46, 0x5A, 0x14);

		flush_ai_signal_dropdown(_this);
		window_dropdown_addText(*(void**)((int)_this + 0xED8), "Ally", 0);
		window_dropdown_addText(*(void**)((int)_this + 0xED8), "Neutral", 1);
		window_dropdown_addText(*(void**)((int)_this + 0xED8), "Enemy", 3);
		//ai signal hlp
		window_setRect(*(void**)((int)_this + 0xED4), 0x190, 2, 0xC8, 0x14);
		window_setText(*(void**)((int)_this + 0xED4), "Alliance State", 0);
		//ai signal
		window_setRect(*(void**)((int)_this + 0xED8), 0x190, 0x16, 0xC8, 0x14);
		break;
	case 0x1B:		//var GE
	case 0x1C:		//var E
		flush_ai_signal_dropdown(_this);
		for (int i = 0; i < sizeof(var_names) / sizeof(var_names[0]); i++)
			window_dropdown_addText(*(void**)((int)_this + 0xED8), var_names[i], i);

		//timer hlp
		window_setRect(*(void**)((int)_this + 0xE28), 0x240, 0x3C, 0x96, 0x14);
		window_setText(*(void**)((int)_this + 0xE28), szAmount, 0);
		//timer
		window_setRect(*(void**)((int)_this + 0xE2C), 0x240, 0x50, 0x5A, 0x14);
		//ai signal hlp
		window_setRect(*(void**)((int)_this + 0xED4), 0x240, 2, 0xC8, 0x14);
		window_setText(*(void**)((int)_this + 0xED4), "Variable", 0);
		//ai signal
		window_setRect(*(void**)((int)_this + 0xED8), 0x240, 0x16, 0xC8, 0x14);
		//quantity
		window_setText(*(void**)((int)_this + 0xE68), szQuantity, 0);

		//
		//window_setText(*(void**)((int)_this + 0xED8), szQuantity, 0);

		break;
	default:
		//timer hlp
		window_setRect(*(void**)((int)_this + 0xE28), 0xE6, 2, 0x96, 0x14);
		window_setText(*(void**)((int)_this + 0xE28), szTimer, 0);
		//window_setText(*(void**)((int)_this + 0xE28), 0, 10723);
		//timer
		window_setRect(*(void**)((int)_this + 0xE2C), 0xE6, 0x16, 0x5A, 0x14);
		//obj list type hlp
		window_setRect(*(void**)((int)_this + 0xE58), 0xE6, 0x3C, 0x96, 0x14);
		//obj list type
		window_setRect(*(void**)((int)_this + 0xE5C), 0xE6, 0x50, 0x96, 0x14);
		//quantity
		window_setText(*(void**)((int)_this + 0xE68), szQuantity, 0);
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

	setHook((void*)0x0052A977, (void*)0x0052A9C4);	//remove ai signal dropdown init
	setHook((void*)0x0052A8F6, (void*)0x0052A943);	//remove ai trigger dropdown init

	setInt(0x0053C2E4, (int)&getEffectParams_new - 0x0053C2E8);
	setInt(0x0053DD6B, (int)&getConditionParams_new - 0x0053DD6F);
	setInt(0x0053C26F, (int)&getConditionParams_new - 0x0053C273);
}
