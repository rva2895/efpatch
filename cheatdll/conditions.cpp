#include "stdafx.h"

#include "conditions.h"
#include "triggerdesc.h"
#include "casts.h"
#include "autosave.h"

#define NEW_COND 5

#define ALLIANCE_ALLY 0
#define ALLIANCE_NEUTRAL 1
#define ALLIANCE_ENEMY 3

int condJMPTable [] =
{
	0x5F1E53,
	0x5F1EA8,
	0x5F1F25,
	0x5F1F9B,
	0x5F2054,
	0x5F22C5,
	0x5F22E4,
	0x5F2312,
	0x5F2343,
	0x5F2374,
	0x5F2396,
	0x5F23DB,
	0x5F2411,
	0x5F2435,
	0x5F24DB,
	0x5F2540,
	0x5F25A5,
	0x5F25D2,
	0x5F25FE,
	0x5F1FE6,
	0x5F20F5,
	0x5F2227,
	0x5F1E25,
	0,
	0,
	0,
	0,
	0
};

void conditionAreaExplored();
void conditionAlliance();
void conditionVariable();

__declspec(naked) void condNotMet ()
{
	__asm
	{
		mov		ebx, 005F2626h
		//mov		ebx, 005F260Eh
		jmp		ebx
	}
}

__declspec(naked) void condMet ()
{
	__asm
	{
		mov		ebx, 005F1E95h
		jmp		ebx
	}
}

__declspec(naked) void cond1 ()
{
	__asm
	{
		//call    ds:[rand]
		mov		edx, 00632BDDh
		call	edx				//rand
		xor     edx, edx
		mov     edi, 3E8h
		idiv    edi
		mov     edi, [esi + 0Ch]
		cmp     edi, edx
		jg      cYes
		jmp     condNotMet
cYes:
		jmp     condMet
	}
}

void setConditionNumbers ()
{
	setByte (0x005F538A, 0x60+4*NEW_COND);
	setByte (0x005F5505, 0x60+4*NEW_COND);
	setByte (0x005F5548, 0x60+4*NEW_COND);
	setByte (0x0053BD77, 0x18+NEW_COND);

	setByte(0x005F5565, 0x17 + NEW_COND);
	setByte(0x005F554E, 0x17 + NEW_COND);

	setInt (0x005F1E21, (int)condJMPTable);
	setByte (0x005F1E17, 0x16+NEW_COND);

	condJMPTable[0x17] = (int)&cond1;
	condJMPTable[0x18] = (int)&conditionAreaExplored;
	condJMPTable[0x19] = (int)&conditionAlliance;
	condJMPTable[0x1A] = (int)&conditionVariable;
	condJMPTable[0x1B] = (int)&conditionVariable;
}

__declspec(naked) void condParams () //005F55AA
{
	__asm
	{
		mov     eax, [esi + 8]
		mov     edx, [eax + 60h]    //new condition 1
		mov		[edx + 0], cl

		mov     eax, [esi + 8]
		mov     edx, [eax + 64h]    //new condition 2
		mov		[edx + 0], cl
		mov		[edx + 5], cl
		mov		[edx + 9], cl
		mov		[edx + 0Ah], cl
		mov		[edx + 0Bh], cl
		mov		[edx + 0Ch], cl

		mov     eax, [esi + 8]
		mov     edx, [eax + 68h]    //new condition alliance state
		mov		[edx + 5], cl
		mov		[edx + 7], cl
		mov		[edx + 0Fh], cl

		mov     eax, [esi + 8]
		mov     edx, [eax + 6Ch]    //new condition var GE
		mov		cl, 2
		mov		[edx + 0], cl
		mov		[edx + 2], cl
		mov		[edx + 4], cl
		mov		[edx + 5], cl
		mov		[edx + 7], cl
		mov		[edx + 9], cl
		mov		[edx + 0Ah], cl
		mov		[edx + 0Bh], cl
		mov		[edx + 0Ch], cl
		mov		[edx + 0Dh], cl
		mov		[edx + 0Eh], cl
		mov		[edx + 0Fh], cl

		mov     eax, [esi + 8]
		mov     edx, [eax + 70h]    //new condition var E
		mov		cl, 2
		mov		[edx + 0], cl
		mov		[edx + 2], cl
		mov		[edx + 4], cl
		mov		[edx + 5], cl
		mov		[edx + 7], cl
		mov		[edx + 9], cl
		mov		[edx + 0Ah], cl
		mov		[edx + 0Bh], cl
		mov		[edx + 0Ch], cl
		mov		[edx + 0Dh], cl
		mov		[edx + 0Eh], cl
		mov		[edx + 0Fh], cl

		mov     eax, [esi + 8]
		mov     edx, [eax + 14h]    //old condition objects in area
		mov		[edx + 0Fh], cl

		mov     eax, [esi+8]
		mov     edx, [eax+8]
		mov		eax, 005F55B0h
		jmp		eax
	}
}

int invBtnPtr;

__declspec(naked) void funcCreateWrapper ()
{
	__asm
	{
		mov     eax, [esp+4]
		push    esi
		push    edi
		mov     edi, [esp+10h]
		push    0
		push    1
		push    0
		push    0
		push    0
		push    0
		push    edi
		mov     esi, ecx
		push    eax
		mov     eax, 4b9520h
		call    eax
		test    eax, eax
		jnz     short loc_4ED558
		pop     edi
		pop     esi
		retn    8
		//---------------------------------------------------------------------------
		
loc_4ED558:                      //       ; CODE XREF: sub_4ED530+21j
		mov     ecx, [edi]
		push    0
		push    1
		mov     eax, 4B7450h
		call    eax
		mov     ecx, [edi]
		push    0
		mov     edx, [ecx]
		call    dword ptr [edx+14h]
		mov     eax, [esi+320h]
		test    eax, eax
		jz      short loc_4ED5B5
		xor     eax, eax
		xor     ecx, ecx
		mov     al, [esi+32Dh]
		mov     cl, [esi+32Ch]
		xor     edx, edx
		push    eax
		mov     dl, [esi+32Bh]
		push    ecx
		xor     eax, eax
		push    edx
		mov     al, [esi+32Ah]
		xor     ecx, ecx
		mov     cl, [esi+329h]
		xor     edx, edx
		mov     dl, [esi+328h]
		push    eax
		push    ecx
		mov     ecx, [edi]
		push    edx
		push    3
		mov     eax, 4C5190h
		call    eax
		
loc_4ED5B5:             //                ; CODE XREF: sub_4ED530+44j
		pop     edi
		mov     eax, 1
		pop     esi
		retn    8
	}
}

//constructor?
__declspec(naked) void inv1 () //00529A06
{
	__asm
	{
		mov     ecx, esi
		mov     eax, 531E80h
		call    eax
		mov     eax, [esi+918h]
		lea     edi, invBtnPtr
		push    edi
		push    eax
		mov     ecx, esi
		call    funcCreateWrapper
		test    eax, eax
		jz      short loc_7E22C4
		mov     ecx, ds:invBtnPtr
		push    12h
		push    12h
		push    2
		mov     edx, [ecx]
		push    0BAh
		call    dword ptr [edx+28h]

loc_7E22C4:
		//
		call	editor_enter
		//
		mov		eax, 00529A0Dh
		jmp		eax
	}
}

//destructor?
__declspec(naked) void inv2 () //0052ABD7
{
	__asm
	{
		lea     eax, invBtnPtr
		mov     ecx, esi
		push    eax
		mov     eax, 428520h
		call    eax
		//
		call	editor_exit
		//
		lea     eax, [esi+964h]
		mov		ecx, 0052ABDDh
		jmp		ecx
	}
}

__declspec(naked) void inv3 () //0053BDA0
{
	__asm
	{
		mov     ecx, invBtnPtr
		push    0
		mov     edx, [ecx]
		call    dword ptr [edx + 14h]

		mov     ecx, [esi+0E1Ch]
		mov		edx, 0053BDA6h
		jmp		edx
	}
}

__declspec(naked) void inv4 () //0053C37C
{
	__asm
	{
		movsx   eax, byte ptr [edi+2Ch]
		neg     eax
		mov     ecx, ds:invBtnPtr
		push    eax
		mov     edx, [ecx]
		call    dword ptr [edx+104h]
		mov     ecx, ds:invBtnPtr
		push    1
		mov     edx, [ecx]
		call    dword ptr [edx+14h]
		mov     ecx, edi
		//call    sub_553EA0 -> 419150
		mov     eax, [ecx+4]

		mov     ecx, edi
		mov     eax, 419150h
		call    eax
		mov		ecx, 0053C383h
		jmp		ecx
	}
}

__declspec(naked) void inv5 () //0053E014
{
	__asm
	{
		mov     ecx, ds:invBtnPtr
		mov     eax, 4C5280h
		call    eax
		neg     eax
		mov     [ebx+2Ch], al

		mov     ecx, ebx
		mov     eax, 419150h
		call    eax
		mov		esi, 0053E01Bh
		jmp		esi
	}
}

__declspec(naked) void inv6 () //005F2756
{
	__asm
	{
		test    cl, cl
		jnz     cont
		cmp     ebp, 8
		jz      short loc_7E236B
		mov     dword ptr [ebx], 0FFFFFFFFh
		//
loc_7E236B:                     //        ; CODE XREF: sub_437120+3AB243j
		inc     ebp
		add     ebx, 4
		cmp     ebp, 10h
		jge     jmpFar
		xor     edx, edx
		mov		eax, 005F274Bh
		jmp		eax
jmpFar:
		mov		eax, 005F2817h
		jmp		eax
cont:
		mov		eax, 005F275Eh
		jmp		eax
	}
}

char invCond;

__declspec(naked) void invProcessCond () //005F4A0F
{
	__asm
	{
		mov     ecx, [ecx+edi*4]
		mov     al, [ecx+2Ch]
		mov     ds:invCond, al
		mov     eax, 005F1DE0h
		call    eax
		cmp     al, 2
		jz      loc_5F4A2D
		add     al, ds:invCond
		jnz     loc_5F4A63
		mov		ecx, 005F4A23h
		jmp		ecx
loc_5F4A63:
		mov		ecx, 005F4A63h
		jmp		ecx
loc_5F4A2D:
		mov		ecx, 005F4A2Dh
		jmp		ecx
	}
}

extern int* mapptr;

__declspec(naked) void conditionAreaExplored()
{
	__asm
	{
		mov     ebp, mapptr
		mov     edx, [esi + 34h]    //y1
		mov     edi, [esi + 3Ch]    //y2
		//sub     ebx, edx          //rect x size - 1

		shl     edx, 2
		shl     edi, 2
		add     edx, ebp          //y1 coloumn, x1 to x2
		add     edi, ebp          //y2 coloumn, x1 to x2

		xor		ecx, ecx
		//mov     esi, edx

x_cont :
		//edx: current coloumn
		mov     ebp, [edx]

		mov     eax, [esi + 30h]
		//mov     eax, [esi+ecx]
		push	edx
y_cont :
		shl		ecx, 10h
		mov     cl, [esi + 20h]
		mov     ebx, 10000h
		shl     ebx, cl
		shr		ecx, 10h
		test	[ebp + eax * 4], ebx
		setnz	bl
		movzx	ebx, bl
		add		ecx, ebx
		inc     eax
		cmp     eax, [esi + 38h]
		jle     y_cont
		pop		edx
		cmp     edx, edi
		jge     x_end
		add     edx, 4
		jmp     x_cont
x_end :
		//ecx = n tiles
		mov		eax, [esi + 38h]
		sub		eax, [esi + 30h]
		inc		eax			//eax = xl
		mov		edx, [esi + 3Ch]
		sub		edx, [esi + 34h]
		inc		edx			//edx = yl
		mul		edx
		mov		ebx, [esi + 0Ch]
		mul		ebx
		mov		ebx, 100
		div		ebx
		//eax = n tiles required
		cmp		ecx, eax
		jl		_not_enough_tiles
		jmp		condMet
_not_enough_tiles:
		jmp		condNotMet
	}
}

__declspec(naked) void conditionAlliance()
{
	__asm
	{
		mov		ecx, [esp + 38h]	//player ptr
		mov		eax, [esi + 28h]	//timer = target player
		push	eax
		mov		eax, 004C2C60h
		call	eax
		mov		cl, [esi + 48h]
		cmp		al, cl
		jz		_alliance_same
		jmp		condNotMet
_alliance_same:
		jmp		condMet
	}
}

int(__thiscall* unitContainter_countUnits) (void*, int, int, int, int, int, int, int, int, void*, char) =
	(int(__thiscall*) (void*, int objListType, int objGroup, int unk2, int x1, int y1, int x2, int y2, int objType, void* unitsVectorPtr, char unk02))0x004AF980;

bool __fastcall compare_null(int, int) { return false; }
bool __fastcall compare_ge(int v1, int v2) { return v1 >= v2; }
bool __fastcall compare_g(int v1, int v2) { return v1 > v2; }
bool __fastcall compare_e(int v1, int v2) { return v1 == v2; }

//////////////

float __fastcall getval_null(UNIT*) { return 0; }
float __fastcall getval_hp(UNIT* unit) { return unit->hp; }
float __fastcall getval_sp(UNIT* unit) { return unit->sp; }
float __fastcall getval_resources(UNIT* unit) { return unit->resources; }
float __fastcall getval_reload(UNIT* unit) { return *(float*)((int)unit + 0x174); }
//float __stdcall getval_constr(UNIT* unit) { return *(float*)((int)unit + 0x230); }

float __fastcall getval_hp_percent(UNIT* unit)
{
	float maxHP = unit->prop_object->hit_points;
	return getval_hp(unit) / maxHP * 100;
}

float __fastcall getval_sp_percent(UNIT* unit)
{
	float maxHP = unit->prop_object->hit_points;
	return getval_sp(unit) / maxHP * 100;
}

float __fastcall getval_garrison(UNIT* unit)
{
	int ptr = *(int*)((int)unit + 0x30);
	if (ptr)
		return *(int*)(ptr + 4);
	else
		return 0.0f;
}

float __fastcall getval_reload_percent(UNIT* unit)
{
	int propObj = (int)unit->prop_object;
	float reloadTime = *(short*)(propObj + 0x150);
	return getval_reload(unit) / reloadTime * 100;
}

/*float __stdcall getval_constr_percent(UNIT* unit)
{
	int propObj = *(int*)((int)unit + 0x14);
	if (*(char*)(propObj + 4) == 80)
	{
		float trainTime = *(short*)(propObj + 0x18E);
		return getval_constr(unit) / trainTime * 100;
	}
	else
		return 100;
}*/

float __fastcall getval_counter(UNIT* unit, int c)
{
	UNIT_EXTRA* ud = getUnitExtra(unit);
	if (ud && (ud->countersUsed))
		switch (c)
		{
		case 1:
			return ud->miscCounter1;
			break;
		case 2:
			return ud->miscCounter2;
			break;
		case 3:
			return ud->miscCounter3;
			break;
		case 4:
			return ud->miscCounter4;
			break;
		case 5:
			return ud->miscCounter5;
			break;
		default:
			return 0;
			break;
		}
	else
		return 0;
}

float __fastcall getval_counter1(UNIT* unit) { return getval_counter(unit, 1); }
float __fastcall getval_counter2(UNIT* unit) { return getval_counter(unit, 2); }
float __fastcall getval_counter3(UNIT* unit) { return getval_counter(unit, 3); }
float __fastcall getval_counter4(UNIT* unit) { return getval_counter(unit, 4); }
float __fastcall getval_counter5(UNIT* unit) { return getval_counter(unit, 5); }

bool __stdcall conditionVariable_actual(condition* c, void* player, UNIT* object)
{
	UNIT* units[0x1000];
	memset(units, 0, 0x1000 * 4);

	bool(__fastcall* compare) (int, int) = compare_null;

	switch (c->id)
	{
	case 0x1B:
		compare = compare_ge;
		break;
	case 0x1C:
		compare = compare_e;
		break;
	default:
		break;
	}

	int n;
	if (object)
	{
		n = 1;
		units[0] = object;
	}
	else
		n = unitContainter_countUnits(*(void**)((int)player + 0x78), c->obj_list_type, c->obj_group, 2,
			c->area_x1, c->area_y1, c->area_x2, c->area_y2, c->obj_type, units, 0);

	float(__fastcall* getval) (UNIT*) = getval_null;

	switch (c->ai_signal)
	{
	case 0:
		getval = getval_hp;
		break;
	case 1:
		getval = getval_hp_percent;
		break;
	case 2:
		getval = getval_sp;
		break;
	case 3:
		getval = getval_sp_percent;
		break;
	case 4:
		getval = getval_reload;
		break;
	case 5:
		getval = getval_reload_percent;
		break;
	case 6:
		getval = getval_resources;
		break;
	case 7:
		getval = getval_garrison;
		break;
	case 8:
		getval = getval_counter1;
		break;
	case 9:
		getval = getval_counter2;
		break;
	case 10:
		getval = getval_counter3;
		break;
	case 11:
		getval = getval_counter4;
		break;
	case 12:
		getval = getval_counter5;
		break;
	default:
		break;
	}

	int cnt = 0;

	for (int i = 0; i < n; i++)
		if (!(c->trigger & 1) ^ compare(getval(units[i]), c->timer))
			cnt++;

	return !(c->trigger & 1) ^ cnt >= c->quantity;
}

__declspec(naked) void conditionVariable()
{
	__asm
	{
		mov		eax, [esp + 14h]	//object
		mov		ecx, [esp + 38h]	//player
		push	eax
		push	ecx
		push	esi					//condition
		call	conditionVariable_actual
		test	al, al
		jnz		condMet
		jmp		condNotMet
	}
}

void* unitContainter_countUnits_ungarrisoned;
void* unitContainter_countUnits_garrisoned;

void reloc(void* src, void* dst)
{
	*(unsigned long*)src = (unsigned long)dst - ((unsigned long)src + 4);
}

__declspec(naked) void unitContainter_countUnits_wrapper()
{
	__asm
	{
		mov		eax, [esi + 48h]
		test	eax, eax
		jz		_count_default
		cmp		eax, 1
		jz		_count_ungarrisoned
		cmp		eax, 2
		jz		_count_garrisoned
_count_default:
		mov		eax, 004AF980h
		jmp		eax
_count_ungarrisoned:
		mov		eax, unitContainter_countUnits_ungarrisoned
		jmp		eax
_count_garrisoned:
		mov		eax, unitContainter_countUnits_garrisoned
		jmp		eax
	}
}

extern void* new_memory_pages;

void make_counter_functions()
{
	DWORD r;
	unitContainter_countUnits_ungarrisoned = new_memory_pages;
	ReadProcessMemory(GetCurrentProcess(), (void*)0x004AF980, unitContainter_countUnits_ungarrisoned, 0x160, &r);
	unitContainter_countUnits_garrisoned = (char*)unitContainter_countUnits_ungarrisoned + 0x160;
	memcpy(unitContainter_countUnits_garrisoned, unitContainter_countUnits_ungarrisoned, 0x160);

	reloc((BYTE*)unitContainter_countUnits_ungarrisoned + 0x0AB, (void*)0x00632BAC);
	reloc((BYTE*)unitContainter_countUnits_ungarrisoned + 0x0B9, (void*)0x00632BAC);
	reloc((BYTE*)unitContainter_countUnits_ungarrisoned + 0x0D9, (void*)0x00632BAC);
	reloc((BYTE*)unitContainter_countUnits_ungarrisoned + 0x0E7, (void*)0x00632BAC);

	reloc((BYTE*)unitContainter_countUnits_garrisoned + 0x0AB, (void*)0x00632BAC);
	reloc((BYTE*)unitContainter_countUnits_garrisoned + 0x0B9, (void*)0x00632BAC);
	reloc((BYTE*)unitContainter_countUnits_garrisoned + 0x0D9, (void*)0x00632BAC);
	reloc((BYTE*)unitContainter_countUnits_garrisoned + 0x0E7, (void*)0x00632BAC);

	setHook((char*)unitContainter_countUnits_ungarrisoned + 0x0A3, (char*)unitContainter_countUnits_ungarrisoned + 0x134);
	setHook((char*)unitContainter_countUnits_garrisoned + 0x0D1, (char*)unitContainter_countUnits_garrisoned + 0x134);

	VirtualProtect((void*)0x005F207E, 0x10, PAGE_READWRITE, &r);
	reloc((void*)0x005F207E, &unitContainter_countUnits_wrapper);
	VirtualProtect((void*)0x005F207E, 0x10, PAGE_EXECUTE_READ, &r);

	VirtualProtect(unitContainter_countUnits_ungarrisoned, 0x1000, PAGE_EXECUTE_READ, &r);
}

void setConditionHooks ()
{
	setConditionNumbers ();
	setHook ((void*)0x005F55AA, &condParams);
	setHook ((void*)0x00529A06, &inv1);
	setHook ((void*)0x0052ABD7, &inv2);
	setHook ((void*)0x0053BDA0, &inv3);
	setHook ((void*)0x0053C37C, &inv4);
	setHook ((void*)0x0053E014, &inv5);
	setHook ((void*)0x005F2756, &inv6);

	setHook ((void*)0x005F4A0F, &invProcessCond);

	make_counter_functions();
}
