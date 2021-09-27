#include "stdafx.h"
#include "unit_master.h"

#pragma warning(push)
#pragma warning(disable:4100)
__declspec(naked) UNIT_MASTER_EXTRA* __fastcall get_unit_master_extra(prop_object* master)
{
    __asm
    {
        mov     eax, [ecx + 4]
        mov     al, [ecx + 13h]
        ret
    }
}

__declspec(naked) void __fastcall add_unit_master_extra(prop_object* master, UNIT_MASTER_EXTRA* ex)
{
    __asm
    {
        add     ecx, 4
        mov     [ecx + 0Fh], dl
        and     edx, 0FFFFFF00h
        or      dword ptr [ecx], edx
        ret
    }
}

#pragma warning(pop)

void __stdcall copy_unit_master_extra(prop_object* dst, prop_object* src)
{

}

__declspec(naked) void static_setup_1() //0048BEDA
{
    __asm
    {
        push    edi
        push    esi
        call    copy_unit_master_extra
        mov     byte ptr [esi + 4], 0Ah
        mov     al, [edi + 0BCh]
        mov     ecx, 0048BEE4h
    }
}

void setUnitMasterHooks()
{

}
