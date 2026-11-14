#include "stdafx.h"
#include "bldg_heal_rate_save.h"

extern int current_save_game_version;

__declspec(naked) void bldg_master_small_save() //005C750D
{
    __asm
    {
        push    4
        lea     edx, [esi + 218h]
        mov     ecx, edi
        call    rge_write

        pop     edi
        pop     esi
        retn    4
    }
}

__declspec(naked) void bldg_master_small_read() //005C72AF
{
    __asm
    {
        mov     eax, current_save_game_version
        cmp     eax, 3
        jl      skip_bldg_small_save_read
        mov     ecx, [esp + 0Ch]
        push    4
        lea     edx, [esi + 218h]
        call    rge_read

skip_bldg_small_save_read:
        pop     edi
        mov     eax, 1
        pop     esi
        retn    10h
    }
}

#pragma optimize( "s", on )
void setBldgHealRateSaveHooks()
{
    // retired 2026-11-15: baked into TRIBE_Master_Building_Object__setup4_new
    // (version-gated heal_rate read) and TRIBE_Master_Building_Object__small_
    // save_new (heal_rate write), src/engine/object/TRIBE_Master_Building_
    // Object.cpp — see 0x005C72AF / 0x005C750D.
    // setHook((void*)0x005C750D, bldg_master_small_save);
    // setHook((void*)0x005C72AF, bldg_master_small_read);
}
#pragma optimize( "", on )
