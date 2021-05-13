#include "stdafx.h"
#include "medic.h"

int medic_max;
int medic_current;
int medic_candidates;

__declspec(naked) void onMedicCheck() //005B4278
{
    __asm
    {
        mov     [esp + 10h], eax
        mov     eax, medic_current
        mov     ecx, medic_max;
        cmp     eax, ecx
        jl      _medic_allowed1
        or      eax, -1
        mov     [esp + 10h], eax
_medic_allowed1:
        inc     medic_candidates
        mov     ecx, 005B4280h
        jmp     ecx
    }
}

__declspec(naked) void onMedicCount() //005B41F0
{
    __asm
    {
        mov     al, [edi + 1ADh]
        movsx   eax, al
        mov     ecx, medic_max
        mov     medic_current, eax
        cmp     eax, ecx
        jmp     _medic_allowed
        mov     ecx, 005B4280h
        jmp     ecx
_medic_allowed:
        mov     ecx, 005B41FEh
        jmp     ecx
    }
}

__declspec(naked) void onMedicSearch() //005B440A
{
    __asm
    {
        xor     edx, edx
        mov     medic_max, edx

_medic_continue:
        inc     medic_max
        xor     edx, edx
        mov     medic_candidates, edx
        mov     eax, [esi]
        lea     ecx, [esp + 10h]
        push    ecx
        push    1
        mov     ecx, esi
        mov     dword ptr [esp + 18h], 40A00000h
        call    dword ptr [eax + 0ECh]
        cmp     eax, -1
        jnz     _medic_use
        mov     eax, medic_candidates
        test    eax, eax
        jnz     _medic_continue
        mov     ecx, 005B4528h
        jmp     ecx
_medic_use:
        mov     ecx, 005B442Ch
        jmp     ecx
    }
}

#pragma optimize( "s", on )
void setMedicHooks()
{
    setHook((void*)0x005B41F0, onMedicCount);
    setHook((void*)0x005B440A, onMedicSearch);
    setHook((void*)0x005B4278, onMedicCheck);
}
#pragma optimize( "", on )
