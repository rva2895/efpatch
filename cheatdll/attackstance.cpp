#include "stdafx.h"
#include "attackstance.h"

void* freeze_edi;

__declspec(naked) void freezeSaveEDI() //005F39E1
{
    __asm
    {
        mov     eax, [esp + 10h]
        mov     freeze_edi, edi
        xor     edi, edi
        mov     ecx, 005F39E7h
        jmp     ecx
    }
}

__declspec(naked) void onFreezeUnit() //005F3A0E
{
    __asm
    {
        mov     ecx, freeze_edi
        mov     eax, [ecx + 0Ch]
        cmp     eax, -1
        jz      _old_freeze
        mov     ecx, [esi]
        mov     eax, 0040C840h
        call    eax
        mov     edx, [eax]
        mov     ecx, freeze_edi
        mov     ecx, [ecx + 0Ch]
        dec     ecx
        push    ecx
        mov     ecx, eax
        call    dword ptr [edx + 0C4h]
        mov     eax, 005F3A31h
        jmp     eax
_old_freeze:
        mov     ecx, [esi]
        mov     eax, 0040C840h
        call    eax
        mov     ecx, 005F3A15h
        jmp     ecx
    }
}

#pragma optimize( "s", on )
void setAttackStanceHooks()
{
    setHook((void*)0x005F3A0E, onFreezeUnit);
    setHook((void*)0x005F39E1, freezeSaveEDI);
}
#pragma optimize( "", on )
