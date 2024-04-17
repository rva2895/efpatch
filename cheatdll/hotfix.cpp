#include "stdafx.h"
#include "hotfix.h"

extern int current_loaded_version;

TRIBE_Tech* __stdcall do_tech_fix(TRIBE_Tech* tech)
{
    if (current_loaded_version == 6 && tech->tech_tree_num > 1218 && tech->tech_tree[1218].effect == 1229)
        tech->tech_tree[1218].effect = 1230;

    return tech;
}

void __stdcall do_master_objects_fix(RGE_Master_Player* master_player)
{
    if (current_loaded_version == 6 && master_player->master_object_num > 1253 && master_player->master_objects[1253]->available == 1)
        master_player->master_objects[1253]->available = 0;
}

__declspec(naked) void on_tech_load() //005BEFE4
{
    __asm
    {
        mov     eax, [ebp - 14h]
        push    eax
        call    do_tech_fix
        mov     esp, ebp
        pop     ebp
        ret     8
    }
}

__declspec(naked) void on_master_objects_load() //004ACB03
{
    __asm
    {
        push    esi
        call    do_master_objects_fix
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        ret     0Ch
    }
}

#pragma optimize( "s", on )
void setHotfixHooks()
{
    setHook((void*)0x005BEFE4, on_tech_load);
    setHook((void*)0x004ACB03, on_master_objects_load);
}
#pragma optimize( "", on )
