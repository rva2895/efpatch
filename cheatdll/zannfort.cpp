#include "stdafx.h"
#include "zannfort.h"
#include "advtriggereffect.h"

const char zann_str_normal[] = "MUL Cost2Amount 0.75";
const char zann_str_1_5_0_fix[] = "MUL Cost2Amount 0";
extern int current_loaded_version;

void zann_resetCosts1(RGE_Player* player)
{
    //set to 1/4
    advTriggerEffect_do_single_line_effect(player->master_objects[2544], NULL, player, "SET Cost2Amount 40"); //base 160
    advTriggerEffect_do_single_line_effect(player->master_objects[2546], NULL, player, "SET Cost2Amount 40"); //base 160
    advTriggerEffect_do_single_line_effect(player->master_objects[1775], NULL, player, "SET Cost2Amount 50"); //base 200
    advTriggerEffect_do_single_line_effect(player->master_objects[1777], NULL, player, "SET Cost2Amount 50"); //base 200
    advTriggerEffect_do_single_line_effect(player->master_objects[1779], NULL, player, "SET Cost2Amount 150"); //base 600
}

void zann_resetCosts2(RGE_Player* player)
{
    //add 3/4
    advTriggerEffect_do_single_line_effect(player->master_objects[2544], NULL, player, "ADD Cost2Amount 120"); //base 160
    advTriggerEffect_do_single_line_effect(player->master_objects[2546], NULL, player, "ADD Cost2Amount 120"); //base 160
    advTriggerEffect_do_single_line_effect(player->master_objects[1775], NULL, player, "ADD Cost2Amount 150"); //base 200
    advTriggerEffect_do_single_line_effect(player->master_objects[1777], NULL, player, "ADD Cost2Amount 150"); //base 200
    advTriggerEffect_do_single_line_effect(player->master_objects[1779], NULL, player, "ADD Cost2Amount 450"); //base 600
}

void __stdcall zannfort_onChange(RGE_Player* player)
{
    const char* zann_str_to_use;
    if (current_loaded_version != 4)    //broken in 1.5.0
        zann_str_to_use = zann_str_normal;
    else
        zann_str_to_use = zann_str_1_5_0_fix;

    int nForts = (int)player->attributes[134];  //standing forts     

    zann_resetCosts1(player);

    for (int i = 0; i < nForts; i++)
    {
        advTriggerEffect_do_single_line_effect(player->master_objects[2544], NULL, player, zann_str_to_use);
        advTriggerEffect_do_single_line_effect(player->master_objects[2546], NULL, player, zann_str_to_use);
        advTriggerEffect_do_single_line_effect(player->master_objects[1775], NULL, player, zann_str_to_use);
        advTriggerEffect_do_single_line_effect(player->master_objects[1777], NULL, player, zann_str_to_use);
        advTriggerEffect_do_single_line_effect(player->master_objects[1779], NULL, player, zann_str_to_use);
    }

    zann_resetCosts2(player);
}

__declspec(naked) void onFortBuilt() //00554B81
{
    __asm
    {
        call    dword ptr [edx + 80h]
        mov     ecx, [esi + 18h]
        //mov     ecx, [ecx + 1D94h]
        push    ecx
        call    zannfort_onChange
        mov     eax, 00554B87h
        jmp     eax
    }
}

__declspec(naked) void onFortDestroyed() //00554D9C
{
    __asm
    {
        call    dword ptr [edx + 80h]
        mov     ecx, [esi + 18h]
        //mov     ecx, [ecx + 1D94h]
        push    ecx
        call    zannfort_onChange
        mov     eax, 00554DA2h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setZannFortHooks()
{
    setHook((void*)0x00554B81, onFortBuilt);
    setHook((void*)0x00554D9C, onFortDestroyed);
}
#pragma optimize( "", on )
