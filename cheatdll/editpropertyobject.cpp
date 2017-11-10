#include "stdafx.h"

#include "effects.h"
#include "editpropertyobject.h"
#include "advtriggereffect.h"

#pragma warning(push)
#pragma warning(disable:4100)
__declspec(naked) void* __stdcall getPropertyObject(int player, int unitID)
{
	__asm
	{
		mov     eax, 6A3684h
		mov     eax, [eax]
		mov     eax, [eax + 17B4h]
		mov     eax, [eax + 126Ch]
		mov     eax, [eax + 68h]
		mov     eax, [eax + 4]
		mov     eax, [eax + 4Ch]
		mov     ecx, [esp + 4] //player
		mov     eax, [eax + ecx * 4]
		mov     eax, [eax + 74h]
		mov     ecx, [esp + 8] //unitID
		mov     eax, [eax + ecx * 4]
		ret     8
	}
}
#pragma warning(pop)

__declspec(naked) void changePropertyObjectHook()
{
	__asm
	{
		mov     eax, [edi + 6Ch]
		push    eax //message
		mov     eax, [edi + 24h]
		cmp     eax, -1 //if no unit is selected
		jnz     cont
		add     esp, 4
		jmp     end
cont:
		push    eax //unit
		mov     eax, [edi + 28h]
		push    eax //player
		call    getPropertyObject
		push    eax //property object
		call    advTriggerEffectActual
end:
		mov     ebx, 005F3DB1h
		jmp     ebx
	}
}
