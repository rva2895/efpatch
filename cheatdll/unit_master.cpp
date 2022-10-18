#include "stdafx.h"
#include "unit_master.h"

extern int current_save_game_version;

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
        jmp     ecx
    }
}

__declspec(naked) void static_setup_2() //0048CE4F
{
    __asm
    {
        push    eax
        push    edi
        push    esi
        call    copy_unit_master_extra
        pop     eax
        mov     dl, [edi + 0BCh]
        mov     [esi + 0BCh], dl
        mov     edx, 0048CE5Bh
        jmp     edx
    }
}

void __stdcall load_master_extra_from_save(prop_object* master, void* infile)
{
    if (current_save_game_version >= 4)
    {
        bool master_extra_exists;
        deflate_read(infile, &master_extra_exists, sizeof(bool));
        if (master_extra_exists)
        {

        }
    }
}

__declspec(naked) void on_load_master_extra_from_save() //0048D1F4
{
    __asm
    {
        push    ebp
        push    esi
        call    load_master_extra_from_save
        pop     edi
        pop     esi
        pop     ebp
        mov     eax, 1
        pop     ebx
        ret     10h
    }
}

void setUnitMasterHooks()
{

}
