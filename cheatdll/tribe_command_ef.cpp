#include "stdafx.h"
#include "tribe_command_ef.h"
#include "effects.h"
#include "advtriggereffect.h"

void make_cheat_by_id(RGE_Player* player, int ef_cheat_id)
{
    int order_size = 4;
    void* cmd = calloc_internal(1, order_size);
    if (cmd)
    {
        *(uint8_t*)cmd = 0x83;
        uint8_t issuer = player->id;
        *((uint8_t*)cmd + 1) = 1; //ef_cheat
        *((uint8_t*)cmd + 2) = issuer;
        *((uint8_t*)cmd + 3) = ef_cheat_id;

        RGE_Command__submit((RGE_Command*)(*base_game)->world->commands, cmd, order_size, issuer);
    }
}

void make_cheat_by_id_with_unit_list(RGE_Player* player, int ef_cheat_id, RGE_Static_Object** units, int n)
{
    int order_size = 4 + 4 + 4 * n;
    void* cmd = calloc_internal(1, order_size);
    if (cmd)
    {
        *(uint8_t*)cmd = 0x83;
        uint8_t issuer = player->id;
        *((uint8_t*)cmd + 1) = 1; //ef_cheat
        *((uint8_t*)cmd + 2) = issuer;
        *((uint8_t*)cmd + 3) = ef_cheat_id;
        *((uint32_t*)cmd + 1) = n;
        for (int i = 0; i < n; i++)
            *((int*)cmd + 2 + i) = units[i]->id;

        RGE_Command__submit((RGE_Command*)(*base_game)->world->commands, cmd, order_size, issuer);
    }
}

void make_cheat_by_id_with_unit_list_and_option(RGE_Player* player, int ef_cheat_id, RGE_Static_Object** units, int n, int option)
{
    int order_size = 4 + 4 + 4 * n;
    void* cmd = calloc_internal(1, order_size);
    if (cmd)
    {
        *(uint8_t*)cmd = 0x83;
        uint8_t issuer = player->id;
        *((uint8_t*)cmd + 1) = 1; //ef_cheat
        *((uint8_t*)cmd + 2) = issuer;
        *((uint8_t*)cmd + 3) = ef_cheat_id;
        *((uint16_t*)cmd + 2) = n;
        *((uint16_t*)cmd + 3) = option;
        for (int i = 0; i < n; i++)
            *((int*)cmd + 2 + i) = units[i]->id;

        RGE_Command__submit((RGE_Command*)(*base_game)->world->commands, cmd, order_size, issuer);
    }
}

void make_cheat_by_id_with_position(RGE_Player* player, int ef_cheat_id, float x, float y)
{
    int order_size = 4 * 3;
    void* cmd = calloc_internal(1, order_size);
    if (cmd)
    {
        *(uint8_t*)cmd = 0x83;
        uint8_t issuer = player->id;
        *((uint8_t*)cmd + 1) = 1; //ef_cheat
        *((uint8_t*)cmd + 2) = issuer;
        *((uint8_t*)cmd + 3) = ef_cheat_id;

        *((float*)cmd + 1) = x;
        *((float*)cmd + 2) = y;

        RGE_Command__submit((RGE_Command*)(*base_game)->world->commands, cmd, order_size, issuer);
    }
}

float cheat_x;
float cheat_y;

void __stdcall ef_do_command(TRIBE_Command* command, void* order)
{
    TRIBE_World* world = (TRIBE_World*)command->world;
    uint8_t order_type = *((uint8_t*)order + 1);
    switch (order_type)
    {
    case 0: //ef reserved
        break;
    case 1: //ef cheat
    {
        if (RGE_Base_Game__allowCheatCodes(*base_game))
        {
            uint8_t player_id = *((uint8_t*)order + 2);
            uint8_t ef_cheat_id = *((uint8_t*)order + 3);
            RGE_Player* player = (RGE_Player*)(*base_game)->world->players[player_id];
            uint8_t target_player_id;
            RGE_Player* target_player;
            switch (ef_cheat_id)
            {
            case EF_CHEAT_HELP_ME_OBIWAN: //tech cheats
            case EF_CHEAT_NOW_THIS_IS_PODRACING:
            case EF_CHEAT_UNLIMITED_POWER:
            case EF_CHEAT_DEPLOY_THE_GARRISON:
            case EF_CHEAT_MY_HOME_THIS_IS:
                TRIBE_World__cheat(world, player_id, ef_cheat_id + 0x64);
                break;
            case EF_CHEAT_ULTIMATE_POWER_IN_THE_UNIVERSE: //create unit cheats
            case EF_CHEAT_SIMONSAYS:
            case EF_CHEAT_SCARYNEIGHBOR:
            case EF_CHEAT_THATS_NO_MOON:
            case EF_CHEAT_IMPERIAL_ENTANGLEMENTS:
            case EF_CHEAT_TANTIVE_IV:
            case EF_CHEAT_GALACTIC_UPHEAVAL:
            case EF_CHEAT_KOELSCH:
            case EF_CHEAT_THERES_ALWAYS_A_BIGGER_FISH:
            case EF_CHEAT_CREATE_WORKER:
                cheat_x = *((float*)order + 1);
                cheat_y = *((float*)order + 2);
                TRIBE_World__cheat(world, player_id, ef_cheat_id + 0x64);
                break;
            case EF_CHEAT_YOU_HAVE_FAILED_ME_FOR_THE_LAST_TIME: //modify units cheats
            case EF_CHEAT_THE_DEFLECTOR_SHIELD_IS_TOO_STRONG:
            case EF_CHEAT_NO_SHIP_THAT_SMALL_HAS_A_CLOAKING_DEVICE:
            case EF_CHEAT_FORCEHEAL:
            case EF_CHEAT_FORCEPROTECT:
            case EF_CHEAT_JOIN_US_OR_DIE:
                for (int i = 0; i < *((int32_t*)order + 1); i++)
                {
                    TRIBE_Combat_Object* unit = (TRIBE_Combat_Object*)RGE_Game_World__object(command->world, *((uint32_t*)order + 2 + i));
                    RGE_Player* unit_owner = (RGE_Player*)unit->owner;
                    if (unit->master_obj->master_type >= 70)
                    {
                        switch (ef_cheat_id)
                        {
                        case EF_CHEAT_YOU_HAVE_FAILED_ME_FOR_THE_LAST_TIME:
                            if (unit_owner != player)
                                unit->vfptr->cancel_object(unit);
                            break;
                        case EF_CHEAT_THE_DEFLECTOR_SHIELD_IS_TOO_STRONG:
                            if (unit_owner == player)
                            {
                                TRIBE_Combat_Object__create_own_master(unit);
                                unit->master_obj->object_flags |= 0x40;
                            }
                            break;
                        case EF_CHEAT_NO_SHIP_THAT_SMALL_HAS_A_CLOAKING_DEVICE:
                            if (unit_owner == player)
                            {
                                TRIBE_Combat_Object__create_own_master(unit);
                                unit->master_obj->object_flags |= 0x4;
                            }
                            break;
                        case EF_CHEAT_FORCEHEAL:
                            if (unit_owner == player)
                                effectUnitVarActual((RGE_Static_Object*)unit, "SET HPPercent 100");
                            break;
                        case EF_CHEAT_FORCEPROTECT:
                            if (unit_owner == player)
                            {
                                TRIBE_Combat_Object__create_own_master(unit);
                                advTriggerEffect_do_single_line_effect((RGE_Master_Static_Object*)unit->master_obj, (RGE_Static_Object*)unit, NULL, "ADD Armor 3 1");
                                advTriggerEffect_do_single_line_effect((RGE_Master_Static_Object*)unit->master_obj, (RGE_Static_Object*)unit, NULL, "ADD Armor 4 1");
                            }
                            break;
                        case EF_CHEAT_JOIN_US_OR_DIE:
                            if (unit_owner != player)
                            {
                                unit->vfptr->change_ownership(unit, player);
                                RGE_Player* current_player = RGE_Base_Game__get_player(*base_game);
                                if ((player == current_player) &&
                                    (unit->master_obj->master_type != 80 || *((DWORD*)unit + 0x77) == 0))
                                {
                                    RGE_Base_Game__play_sound(*base_game, 0x26, 0, 0);
                                    RGE_View* main_view = (RGE_View*)(*base_game)->vfptr->get_view_panel(*base_game);
                                    if (main_view)
                                        RGE_View__display_object_selection(main_view, unit->id, 1500, 2, 4);
                                }
                            }
                            break;
                        default:
                            break;
                        }
                    }
                }
                break;
            case EF_CHEAT_GIFT:
                target_player_id = *((uint16_t*)order + 3);
                if (target_player_id < (*base_game)->world->player_num)
                {
                    target_player = (RGE_Player*)(*base_game)->world->players[target_player_id];
                    for (int i = 0; i < *((int16_t*)order + 2); i++)
                    {
                        TRIBE_Combat_Object* unit = (TRIBE_Combat_Object*)RGE_Game_World__object(command->world, *((uint32_t*)order + 2 + i));
                        RGE_Player* unit_owner = (RGE_Player*)unit->owner;
                        if (unit->master_obj->master_type >= 70 && unit_owner == player)
                        {
                            unit->vfptr->change_ownership(unit, target_player);
                            RGE_Player* current_player = RGE_Base_Game__get_player(*base_game);
                            if ((target_player == current_player) &&
                                (unit->master_obj->master_type != 80 || *((DWORD*)unit + 0x77) == 0))
                            {
                                RGE_Base_Game__play_sound(*base_game, 0x26, 0, 0);
                                RGE_View* main_view = (RGE_View*)(*base_game)->vfptr->get_view_panel(*base_game);
                                if (main_view)
                                    RGE_View__display_object_selection(main_view, unit->id, 1500, 2, 4);
                            }
                        }
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
    case EF_CHEAT_MY_HOME_THIS_IS:
        return -1233;
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
    case EF_CHEAT_THERES_ALWAYS_A_BIGGER_FISH:
        return 6324;
    case EF_CHEAT_CREATE_WORKER:
        return 83;
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
