#include "stdafx.h"
#include "tribe_command_ef.h"
#include "effects.h"
#include "advtriggereffect.h"

void make_cheat_by_id(void* player, int ef_cheat_id)
{
    int order_size = 4;
    void* cmd = calloc_internal(1, order_size);
    if (cmd)
    {
        *(uint8_t*)cmd = 0x83;
        uint8_t issuer = *(uint32_t*)((uint8_t*)player + 0xA0);
        *((uint8_t*)cmd + 1) = 1; //ef_cheat
        *((uint8_t*)cmd + 2) = issuer;
        *((uint8_t*)cmd + 3) = ef_cheat_id;

        RGE_Command__submit(get_TRIBE_Command(), cmd, order_size, issuer);
    }
}

void make_cheat_by_id_with_unit_list(void* player, int ef_cheat_id, UNIT** units, int n)
{
    int order_size = 4 + 4 + 4 * n;
    void* cmd = calloc_internal(1, order_size);
    if (cmd)
    {
        *(uint8_t*)cmd = 0x83;
        uint8_t issuer = *(uint32_t*)((uint8_t*)player + 0xA0);
        *((uint8_t*)cmd + 1) = 1; //ef_cheat
        *((uint8_t*)cmd + 2) = issuer;
        *((uint8_t*)cmd + 3) = ef_cheat_id;
        *((uint32_t*)cmd + 1) = n;
        for (int i = 0; i < n; i++)
            *((int*)cmd + 2 + i) = units[i]->ordinal;

        RGE_Command__submit(get_TRIBE_Command(), cmd, order_size, issuer);
    }
}

void make_cheat_by_id_with_position(void* player, int ef_cheat_id, float x, float y)
{
    int order_size = 4 * 3;
    void* cmd = calloc_internal(1, order_size);
    if (cmd)
    {
        *(uint8_t*)cmd = 0x83;
        uint8_t issuer = *(uint32_t*)((uint8_t*)player + 0xA0);
        *((uint8_t*)cmd + 1) = 1; //ef_cheat
        *((uint8_t*)cmd + 2) = issuer;
        *((uint8_t*)cmd + 3) = ef_cheat_id;

        *((float*)cmd + 1) = x;
        *((float*)cmd + 2) = y;

        RGE_Command__submit(get_TRIBE_Command(), cmd, order_size, issuer);
    }
}

float cheat_x;
float cheat_y;

void (__thiscall* World__cheat)(void* this_, unsigned int player, __int16 cheat_code) =
    (void (__thiscall*)(void*, unsigned int, __int16))0x00603D90;

void __stdcall ef_do_command(void* this_, void* order)
{
    uint8_t order_type = *((uint8_t*)order + 1);
    switch (order_type)
    {
    case 0: //ef reserved
        break;
    case 1: //ef cheat
    {
        if (BaseGame__allowCheatCodes(*base_game))
        {
            uint8_t player_id = *((uint8_t*)order + 2);
            uint8_t ef_cheat_id = *((uint8_t*)order + 3);
            void* player = get_player(player_id);
            switch (ef_cheat_id)
            {
            case EF_CHEAT_HELP_ME_OBIWAN: //tech cheats
            case EF_CHEAT_NOW_THIS_IS_PODRACING:
            case EF_CHEAT_UNLIMITED_POWER:
            case EF_CHEAT_DEPLOY_THE_GARRISON:
                World__cheat(*((void**)this_ + 1), player_id, ef_cheat_id + 0x64);
                break;
            case EF_CHEAT_ULTIMATE_POWER_IN_THE_UNIVERSE: //create unit cheats
            case EF_CHEAT_SIMONSAYS:
            case EF_CHEAT_SCARYNEIGHBOR:
            case EF_CHEAT_THATS_NO_MOON:
            case EF_CHEAT_IMPERIAL_ENTANGLEMENTS:
            case EF_CHEAT_TANTIVE_IV:
            case EF_CHEAT_GALACTIC_UPHEAVAL:
            case EF_CHEAT_KOELSCH:
                cheat_x = *((float*)order + 1);
                cheat_y = *((float*)order + 2);
                World__cheat(*((void**)this_ + 1), player_id, ef_cheat_id + 0x64);
                break;
            case EF_CHEAT_YOU_HAVE_FAILED_ME_FOR_THE_LAST_TIME: //modify units cheats
            case EF_CHEAT_THE_DEFLECTOR_SHIELD_IS_TOO_STRONG:
            case EF_CHEAT_NO_SHIP_THAT_SMALL_HAS_A_CLOAKING_DEVICE:
            case EF_CHEAT_FORCEHEAL:
            case EF_CHEAT_FORCEPROTECT:
            case EF_CHEAT_JOIN_US_OR_DIE:
                for (int i = 0; i < *((int32_t*)order + 1); i++)
                {
                    UNIT* unit = (UNIT*)BaseWorld__object(*((void**)this_ + 1), *((uint32_t*)order + 2 + i));
                    switch (ef_cheat_id)
                    {
                    case EF_CHEAT_YOU_HAVE_FAILED_ME_FOR_THE_LAST_TIME:
                        if (unit->player != player && unit->prop_object->type > 30)
                            kill_unit(unit);
                        break;
                    case EF_CHEAT_THE_DEFLECTOR_SHIELD_IS_TOO_STRONG:
                        if (unit->player == player)
                        {
                            unit_detach(unit);
                            unit->prop_object->unit_attribute |= 0x40;
                        }
                        break;
                    case EF_CHEAT_NO_SHIP_THAT_SMALL_HAS_A_CLOAKING_DEVICE:
                        if (unit->player == player)
                        {
                            unit_detach(unit);
                            unit->prop_object->unit_attribute |= 0x4;
                        }
                        break;
                    case EF_CHEAT_FORCEHEAL:
                        if (unit->player == player)
                            effectUnitVarActual(unit, "SET HPPercent 100");
                        break;
                    case EF_CHEAT_FORCEPROTECT:
                        if (unit->player == player)
                        {
                            unit_detach(unit);
                            advTriggerEffectActual(unit->prop_object, "ADD Armor 3 1");
                            advTriggerEffectActual(unit->prop_object, "ADD Armor 4 1");
                        }
                        break;
                    case EF_CHEAT_JOIN_US_OR_DIE:
                        if (unit->player != player)
                        {
                            unit_change_ownership(unit, player);
                            void* current_player = getCurrentPlayer();
                            if ((player == current_player) &&
                                (unit->prop_object->type != 80 || *((DWORD*)unit + 0x77) == 0))
                            {
                                GameSoundEffectsManager__playSound(*base_game, 0x26, 0, 0);
                                void* main_view = get_main_view();
                                if (main_view)
                                    RGE_View__display_object_selection(main_view, unit->ordinal, 1500, 2, 4);
                            }
                        }
                        break;
                    default:
                        break;
                    }
                }
                break;
            default:
                break;
            }
        }
        break;
    }
    default:
        break;
    }
}

__declspec(naked) void TRIBE_Command__do_command_ef()
{
    __asm
    {
        push    esi
        push    edi
        call    ef_do_command
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        pop     ecx
        retn    4
    }
}

int __fastcall get_unit_or_tech_id(int cheat_id) //returns positive if unit, negative if tech
{
    switch (cheat_id)
    {
    case EF_CHEAT_HELP_ME_OBIWAN: //tech cheats
        return -798;
    case EF_CHEAT_NOW_THIS_IS_PODRACING:
        return -799;
    case EF_CHEAT_UNLIMITED_POWER:
        return -810;
    case EF_CHEAT_DEPLOY_THE_GARRISON:
        return -961;
    case EF_CHEAT_ULTIMATE_POWER_IN_THE_UNIVERSE: //unit cheats
        return 1802;
    case EF_CHEAT_SIMONSAYS:
        return 1204;
    case EF_CHEAT_SCARYNEIGHBOR:
        return 1314;
    case EF_CHEAT_THATS_NO_MOON:
        return 1587;
    case EF_CHEAT_IMPERIAL_ENTANGLEMENTS:
        return 1586;
    case EF_CHEAT_TANTIVE_IV:
        return 1580;
    case EF_CHEAT_GALACTIC_UPHEAVAL:
        return 545;
    case EF_CHEAT_KOELSCH:
        return 4894;
    default:
        return 0;
    }
}

__declspec(naked) void world_cheat_dispatcher()
{
    __asm
    {
        call    get_unit_or_tech_id
        test    eax, eax
        jz      bad_cheat
        js      cheat_tech
        mov     dword ptr [esp + 10h], eax
        mov     ecx, 00603E68h
        jmp     ecx
cheat_tech:
        neg     eax
        push    eax
        mov     ecx, 00603DF3h
        jmp     ecx
bad_cheat:
        mov     ecx, 006044CAh
        jmp     ecx
    }
}

__declspec(naked) void world_cheat_location_fix() //00603E88
{
    __asm
    {
        mov     eax, cheat_x
        mov     ecx, cheat_y
        mov     [esp + 14h], eax
        mov     [esp + 18h], ecx
        mov     eax, 00603E9Ch
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setTribeCommandEFHooks()
{
    writeDword(0x00603DBF, (DWORD)world_cheat_dispatcher - 0x00603DC3);
    setHook((void*)0x00603E88, world_cheat_location_fix);

    writeWord(0x0060410A, 0x05D8);
    writeDword(0x0060410C, (DWORD)&cheat_x);
    writeWord(0x00604119, 0x05D8);
    writeDword(0x0060411B, (DWORD)&cheat_y);
}
#pragma optimize( "", on )
