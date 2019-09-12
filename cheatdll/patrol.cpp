#include "stdafx.h"
#include "patrol.h"

int (__thiscall* unitGroup_patrol)(void* this_, UNIT* units[], __int16 count, int one, float* x, float* y, int zero) =
	(int (__thiscall*)(void*, UNIT*[], __int16, int, float*, float*, int))0x0044EB40;

int compare_ordinals(const void* a, const void* b)
{
	return ((**(UNIT**)a).ordinal - (**(UNIT**)b).ordinal);
}

void __stdcall patrolNew(void* this_, UNIT* units[], float* x, float* y, int count)
{
	qsort(units, count, sizeof(void*), compare_ordinals);
	do
	{
		unitGroup_patrol(this_, units, count > 40 ? 40 : count, 1, x, y, 0);
		units += 40;
		count -= 40;
	} while (count > 0);
}

__declspec(naked) void patrolHook() //005F3926
{
	__asm
	{
		fild    dword ptr [edi + 44h]
		mov     dx, [edi + 1Ch]
		cmp     dx, 0FFFFh
		jnz     _old_patrol
		fstp    dword ptr [esp + 58h]
		fild    dword ptr [edi + 48h]
		fstp    dword ptr [esp + 54h]
		mov     eax, [esp + 10h] //count
		lea     ecx, [esp + 54h] //y
		lea     edx, [esp + 58h] //x
		push    eax
		push    ecx
		push    edx
		lea     eax, [esp + 140h] //location object
		push    eax
		mov     ecx, 6A3684h
		mov     ecx, [ecx]
		mov     edx, [ecx + 420h]
		mov     ecx, [edx + 68h]
		push    ecx
		call    patrolNew
		mov     esi, 005F3DB1h
		jmp     esi
_old_patrol:
		mov     ecx, 005F392Dh
		jmp     ecx
	}
}

#pragma optimize( "s", on )
void setPatrolHooks()
{
	setHook((void*)0x005F3926, patrolHook);
}
#pragma optimize( "", on )
