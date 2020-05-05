#include "stdafx.h"

#include "cliff.h"

void* scen_ptr;

__declspec(naked) void scen_ptr_hook() //005C9D57
{
	__asm
	{
		mov     scen_ptr, esi
		mov     eax, esi
		pop     edi
		pop     esi
		mov     ecx, [esp + 8]
		mov     ebx, 005C9D5Fh
		jmp     ebx
	}
}

__declspec(naked) void sub_5CAF00()
{
	__asm
	{
		mov     eax, 005CAF00h
		jmp     eax
	}
}

#pragma warning(push)
#pragma warning(disable:4100)
__declspec(naked) void __stdcall setCliffType(int id, void* ptr) //base id
{
	__asm
	{
		push    ebp
		mov     ebp, esp
		push    edi
		push    esi
		push    ebx
		or      edi, 0FFFFFFFFh
		xor     ebx, ebx
		mov     esi, [ebp + 0Ch]	//ptr

		push    3FC00000h
		push    3FC00000h
		push    edi
		push    edi
		push    10h
		mov     eax, [ebp + 8]	//id
		push    eax
		push    ebx
		push    ebx
		push    ebx
		push    1
		mov     ecx, esi
		call    sub_5CAF00
		push    3FC00000h
		push    3FC00000h
		push    edi
		push    edi
		push    12h
		mov     eax, [ebp + 8]	//id
		push    eax
		push    ebx
		push    ebx
		push    1
		push    ebx
		mov     ecx, esi
		call    sub_5CAF00
		push    3FC00000h
		push    3FC00000h
		push    edi
		push    edi
		push    11h
		mov     eax, [ebp + 8]	//id
		push    eax
		push    ebx
		push    1
		push    ebx
		push    ebx
		mov     ecx, esi
		call    sub_5CAF00
		push    3FC00000h
		push    3FC00000h
		push    edi
		push    edi
		push    13h
		mov     eax, [ebp + 8]	//id
		push    eax
		push    1
		push    ebx
		push    ebx
		push    ebx
		mov     ecx, esi
		call    sub_5CAF00
		push    3FC00000h
		push    3FC00000h
		push    edi
		push    edi
		push    14h
		mov     eax, [ebp + 8]	//id
		add     eax, 2
		push    eax
		push    ebx
		push    ebx
		push    ebx
		push    edi
		mov     ecx, esi
		call    sub_5CAF00
		push    3FC00000h
		push    3FC00000h
		push    edi
		push    edi
		push    16h
		mov     eax, [ebp + 8]	//id
		inc     eax
		push    eax
		push    ebx
		push    ebx
		push    edi
		push    ebx
		mov     ecx, esi
		call    sub_5CAF00
		push    3FC00000h
		push    3FC00000h
		push    edi
		push    edi
		push    15h
		mov     eax, [ebp + 8]	//id
		add     eax, 2
		push    eax
		push    ebx
		push    edi
		push    ebx
		push    ebx
		mov     ecx, esi
		call    sub_5CAF00
		push    3FC00000h
		push    3FC00000h
		push    edi
		push    edi
		push    17h
		mov     eax, [ebp + 8]	//id
		inc     eax
		push    eax
		push    edi
		push    ebx
		push    ebx
		push    ebx
		mov     ecx, esi
		call    sub_5CAF00
		push    40000000h
		push    3FC00000h
		push    edi
		push    edi
		push    ebx
		mov     eax, [ebp + 8]	//id
		add     eax, 6
		push    eax
		push    ebx
		push    ebx
		push    1
		push    edi
		mov     ecx, esi
		call    sub_5CAF00
		push    40000000h
		push    3F800000h
		push    edi
		push    edi
		push    9
		mov     eax, [ebp + 8]	//id
		add     eax, 8
		push    eax
		push    ebx
		push    edi
		push    edi
		push    ebx
		mov     ecx, esi
		call    sub_5CAF00
		push    3FC00000h
		push    3F800000h
		push    edi
		push    edi
		push    6
		mov     eax, [ebp + 8]	//id
		add     eax, 4
		push    eax
		push    edi
		push    1
		push    ebx
		push    ebx
		mov     ecx, esi
		call    sub_5CAF00
		push    3FC00000h
		push    3FC00000h
		push    edi
		push    edi
		push    3
		mov     eax, [ebp + 8]	//id
		push    eax
		push    1
		push    ebx
		push    ebx
		push    1
		mov     ecx, esi
		call    sub_5CAF00
		push    3FC00000h
		push    40000000h
		push    edi
		push    edi
		push    0Ch
		mov     eax, [ebp + 8]	//id
		add     eax, 3
		push    eax
		push    ebx
		push    ebx
		push    edi
		push    1
		mov     ecx, esi
		call    sub_5CAF00
		push    3FC00000h
		push    3FC00000h
		push    edi
		push    edi
		push    0Fh
		mov     eax, [ebp + 8]	//id
		push    eax
		push    ebx
		push    1
		push    1
		push    ebx
		mov     ecx, esi
		call    sub_5CAF00
		push    3F800000h
		push    3FC00000h
		push    edi
		push    edi
		push    0Eh
		mov     eax, [ebp + 8]	//id
		add     eax, 5
		push    eax
		push    1
		push    edi
		push    ebx
		push    ebx
		mov     ecx, esi
		call    sub_5CAF00
		push    3F800000h
		push    40000000h
		push    edi
		push    edi
		push    0Dh
		mov     eax, [ebp + 8]	//id
		add     eax, 7
		push    eax
		push    edi
		push    ebx
		push    ebx
		push    edi
		mov     ecx, esi
		call    sub_5CAF00
		push    3FC00000h
		push    3FC00000h
		push    5
		mov     eax, [ebp + 8]	//id
		push    eax
		push    4
		push    eax
		push    ebx
		push    1
		push    ebx
		push    1
		mov     ecx, esi
		call    sub_5CAF00
		push    3FC00000h
		push    3FC00000h
		push    2
		mov     eax, [ebp + 8]	//id
		push    eax
		push    1
		push    eax
		push    1
		push    ebx
		push    1
		push    ebx
		mov     ecx, esi
		call    sub_5CAF00
		push    3FC00000h
		push    3FC00000h
		push    0Ah
		mov     eax, [ebp + 8]	//id
		add     eax, 2
		push    eax
		push    0Bh
		push    eax
		push    ebx
		push    edi
		push    ebx
		push    edi
		mov     ecx, esi
		call    sub_5CAF00
		push    3FC00000h
		push    3FC00000h
		push    7
		mov     eax, [ebp + 8]	//id
		inc     eax
		push    eax
		push    8
		push    eax
		push    edi
		push    ebx
		push    edi
		push    ebx
		mov     ecx, esi
		call    sub_5CAF00

		//
		mov     ebx, [ebp + 8]
		mov     esi, ebx
		add     esi, 9
		mov     edi, writeByte
		push    ebx
		push    005CAFF5h
		call    edi
		mov     al, bh
		push    eax
		push    005CAFF6h
		call    edi
		push    ebx
		push    005CB02Fh
		call    edi
		mov     al, bh
		push    eax
		push    005CB030h
		call    edi
		//
		push    ebx
		push    005CB083h
		call    edi
		mov     al, bh
		push    eax
		push    005CB084h
		call    edi
		push    ebx
		push    005CB0B6h
		call    edi
		mov     al, bh
		push    eax
		push    005CB0B7h
		call    edi
		//
		mov     ebx, esi
		push    ebx
		push    005CAFFCh
		call    edi
		mov     al, bh
		push    eax
		push    005CAFFDh
		call    edi
		push    ebx
		push    005CB036h
		call    edi
		mov     al, bh
		push    eax
		push    005CB037h
		call    edi
		//
		push    ebx
		push    005CB08Ah
		call    edi
		mov     al, bh
		push    eax
		push    005CB08Bh
		call    edi
		push    ebx
		push    005CB0BDh
		call    edi
		mov     al, bh
		push    eax
		push    005CB0BEh
		call    edi
		//
		add     esp, 8*2*4*2
		//

		pop     ebx
		pop     esi
		pop     edi
		mov     esp, ebp
		pop     ebp
		retn    8
	}
}
#pragma warning(pop)

int cliff_type = 0x108;

__declspec(naked) void onCliffPaint() //005CB1B0
{
	__asm
	{
		push    ecx
		push    ecx
		mov     eax, cliff_type
		push    eax
		call    setCliffType
		pop     ecx

		sub     esp, 14h
		mov     eax, [esp + 20h]
		mov     edx, 005CB1B7h
		jmp     edx
	}
}

#define RMS_CLIFF_TYPE_ID 256

__declspec(naked) void onRmsCliff() //004E3780
{
	__asm
	{
		mov     eax, [esp + 4]
		mov     eax, [eax + 68h]
		cmp     eax, RMS_CLIFF_TYPE_ID
		jz      _set_cliff
		mov     edx, 004E3787h
		jmp     edx
_set_cliff:
		mov     edx, scen_ptr
		mov     eax, [ecx + 46678h]
		push    edx
		push    eax
		call    setCliffType
		mov     edx, 004E37F7h
		jmp     edx
	}
}

__declspec(naked) void onResetCliffType() //004DFA63
{
	__asm
	{
		mov     edx, scen_ptr
		mov     eax, 108h
		push    edx
		push    eax
		call    setCliffType
		mov     ecx, [ebp + 140h]
		mov     edx, 004DFA69h
		jmp     edx
	}
}


#pragma optimize( "s", on )
void setCliffTypeHooks()
{
	//setHook((void*)0x005CB1B0, &onCliffPaint);
	setHook((void*)0x005C9D57, scen_ptr_hook);
	setHook((void*)0x004E3780, onRmsCliff);
	setHook((void*)0x004DFA63, onResetCliffType);
}
#pragma optimize( "", on )
