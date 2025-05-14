#include "stdafx.h"

#include "advcheat.h"
#include "advtriggereffect.h"
#include "effects.h"
#include "sngoal.h"
#include "tribe_command_ef.h"

//extern float glitched_res; //remove

//
//extern int control_source;
//extern int control_target;
//extern bool control_initiated;

__int16 player_id_for_gaia_control = 0;

bool __stdcall checkCheats(char* s2)
{
    char dummy[0x80];
    char s[0x200];
    strncpy(s, s2, 0x1FF);
    s[0x1FF] = '\0';
    _strupr(s);

    RGE_Player* player = RGE_Base_Game__get_player(*base_game);

    if (RGE_Base_Game__allowCheatCodes(*base_game))
    {
        //fixed old cheats, SP only
        if (RGE_Base_Game__singlePlayerGame(*base_game))
        {
            if (strstr(s, "SIMONSAYS"))
            {
                make_cheat_by_id_with_position(player, EF_CHEAT_SIMONSAYS,
                    floor(player->view_x) + 0.5f, floor(player->view_y) + 0.5f);
                return true;
            }
            if (strstr(s, "SCARYNEIGHBOR"))
            {
                make_cheat_by_id_with_position(player, EF_CHEAT_SCARYNEIGHBOR,
                    floor(player->view_x) + 0.5f, floor(player->view_y) + 0.5f);
                return true;
            }
            if (strstr(s, "THAT'S NO MOON"))
            {
                make_cheat_by_id_with_position(player, EF_CHEAT_THATS_NO_MOON,
                    floor(player->view_x) + 0.5f, floor(player->view_y) + 0.5f);
                return true;
            }
            if (strstr(s, "IMPERIAL ENTANGLEMENTS"))
            {
                make_cheat_by_id_with_position(player, EF_CHEAT_IMPERIAL_ENTANGLEMENTS,
                    floor(player->view_x) + 0.5f, floor(player->view_y) + 0.5f);
                return true;
            }
            if (strstr(s, "TANTIVE IV"))
            {
                make_cheat_by_id_with_position(player, EF_CHEAT_TANTIVE_IV,
                    floor(player->view_x) + 0.5f, floor(player->view_y) + 0.5f);
                return true;
            }
            if (strstr(s, "GALACTIC UPHEAVAL"))
            {
                make_cheat_by_id_with_position(player, EF_CHEAT_GALACTIC_UPHEAVAL,
                    floor(player->view_x) + 0.5f, floor(player->view_y) + 0.5f);
                return true;
            }
            //new ef cheats, SP only
            if (strstr(s, "LUMINOUS BEINGS ARE WE"))
            {
                int player_id = RGE_Base_Game__get_player(*base_game)->id;
                RGE_Player__unselect_object(player);
                if (player_id != 0)
                {
                    player_id_for_gaia_control = (__int16)player_id;
                    TRIBE_Game__set_player(*(TRIBE_Game**)base_game, 0);
                }
                else
                    TRIBE_Game__set_player(*(TRIBE_Game**)base_game, player_id_for_gaia_control);
                return true;
            }
            if (strstr(s, "YOU HAVE FAILED ME FOR THE LAST TIME"))
            {
                RGE_Static_Object** sel_units = player->sel_list;
                int n = player->sel_count;
                RGE_Static_Object* order_units[40];
                RGE_Static_Object** unit_ptr = order_units;
                for (int i = 0; i < n; i++)
                    if ((RGE_Player*)sel_units[i]->owner != player && sel_units[i]->master_obj->master_type >= 70)
                        *unit_ptr++ = sel_units[i];
                make_cheat_by_id_with_unit_list(player, EF_CHEAT_YOU_HAVE_FAILED_ME_FOR_THE_LAST_TIME, order_units, unit_ptr - order_units);
                return true;
            }
            if (strstr(s, "JOIN US OR DIE"))
            {
                RGE_Static_Object** sel_units = player->sel_list;
                int n = player->sel_count;
                RGE_Static_Object* order_units[40];
                RGE_Static_Object** unit_ptr = order_units;
                for (int i = 0; i < n; i++)
                    if ((RGE_Player*)sel_units[i]->owner != player)
                        *unit_ptr++ = sel_units[i];
                make_cheat_by_id_with_unit_list(player, EF_CHEAT_JOIN_US_OR_DIE, order_units, unit_ptr - order_units);
                return true;
            }
            if (strstr(s, "ULTIMATE POWER IN THE UNIVERSE"))
            {
                make_cheat_by_id_with_position(player, EF_CHEAT_ULTIMATE_POWER_IN_THE_UNIVERSE,
                    floor(player->view_x) + 0.5f, floor(player->view_y) + 0.5f);
                return true;
            }
            if (strstr(s, "KOELSCH"))
            {
                make_cheat_by_id_with_position(player, EF_CHEAT_KOELSCH,
                    floor(player->view_x) + 0.5f, floor(player->view_y) + 0.5f);
                return true;
            }
            if (strstr(s, "THERE'S ALWAYS A BIGGER FISH"))
            {
                make_cheat_by_id_with_position(player, EF_CHEAT_THERES_ALWAYS_A_BIGGER_FISH,
                    floor(player->view_x) + 0.5f, floor(player->view_y) + 0.5f);
                return true;
            }
            if (strstr(s, "LET'S GET TO WORK"))
            {
                make_cheat_by_id_with_position(player, EF_CHEAT_CREATE_WORKER,
                    floor(player->view_x) + 0.5f, floor(player->view_y) + 0.5f);
                return true;
            }
        }
        //new EF cheats, SP and MP
        if (strstr(s, "HELP ME OBI-WAN"))
        {
            make_cheat_by_id(player, EF_CHEAT_HELP_ME_OBIWAN);
            return true;
        }
        if (strstr(s, "NOW THIS IS PODRACING"))
        {
            make_cheat_by_id(player, EF_CHEAT_NOW_THIS_IS_PODRACING);
            return true;
        }
        if (strstr(s, "THE DEFLECTOR SHIELD IS TOO STRONG"))
        {
            RGE_Static_Object** sel_units = player->sel_list;
            int n = player->sel_count;
            RGE_Static_Object* order_units[40];
            RGE_Static_Object** unit_ptr = order_units;
            for (int i = 0; i < n; i++)
                if ((RGE_Player*)sel_units[i]->owner == player)
                    *unit_ptr++ = sel_units[i];
            make_cheat_by_id_with_unit_list(player, EF_CHEAT_THE_DEFLECTOR_SHIELD_IS_TOO_STRONG, order_units, unit_ptr - order_units);
            return true;
        }
        if (strstr(s, "NO SHIP THAT SMALL HAS A CLOAKING DEVICE"))
        {
            RGE_Static_Object** sel_units = player->sel_list;
            int n = player->sel_count;
            RGE_Static_Object* order_units[40];
            RGE_Static_Object** unit_ptr = order_units;
            for (int i = 0; i < n; i++)
                if ((RGE_Player*)sel_units[i]->owner == player)
                    *unit_ptr++ = sel_units[i];
            make_cheat_by_id_with_unit_list(player, EF_CHEAT_NO_SHIP_THAT_SMALL_HAS_A_CLOAKING_DEVICE, order_units, unit_ptr - order_units);
            return true;
        }
        if (strstr(s, "FORCEHEAL"))
        {
            RGE_Static_Object** sel_units = player->sel_list;
            int n = player->sel_count;
            RGE_Static_Object* order_units[40];
            RGE_Static_Object** unit_ptr = order_units;
            for (int i = 0; i < n; i++)
                if ((RGE_Player*)sel_units[i]->owner == player)
                    *unit_ptr++ = sel_units[i];
            make_cheat_by_id_with_unit_list(player, EF_CHEAT_FORCEHEAL, order_units, unit_ptr - order_units);
            return true;
        }
        if (strstr(s, "FORCEPROTECT"))
        {
            RGE_Static_Object** sel_units = player->sel_list;
            int n = player->sel_count;
            RGE_Static_Object* order_units[40];
            RGE_Static_Object** unit_ptr = order_units;
            for (int i = 0; i < n; i++)
                if ((RGE_Player*)sel_units[i]->owner == player)
                    *unit_ptr++ = sel_units[i];
            make_cheat_by_id_with_unit_list(player, EF_CHEAT_FORCEPROTECT, order_units, unit_ptr - order_units);
            return true;
        }
        if (strstr(s, "UNLIMITED POWER"))
        {
            make_cheat_by_id(player, EF_CHEAT_UNLIMITED_POWER);
            return true;
        }
        if (strstr(s, "DEPLOY THE GARRISON"))
        {
            make_cheat_by_id(player, EF_CHEAT_DEPLOY_THE_GARRISON);
            return true;
        }
        if (strstr(s, "MY HOME THIS IS"))
        {
            make_cheat_by_id(player, EF_CHEAT_MY_HOME_THIS_IS);
            return true;
        }
        if (char* gift_s = strstr(s, "GIFT"))
        {
            int id = -1;
            if (sscanf(gift_s + 4, "%d", &id) > 0 && id >= 0 && id <= 8)
            {
                if (id < (*base_game)->world->player_num && id != player->id)
                {
                    RGE_Static_Object** sel_units = player->sel_list;
                    int n = player->sel_count;
                    RGE_Static_Object* order_units[40];
                    RGE_Static_Object** unit_ptr = order_units;
                    for (int i = 0; i < n; i++)
                        if ((RGE_Player*)sel_units[i]->owner == player)
                            *unit_ptr++ = sel_units[i];
                    make_cheat_by_id_with_unit_list_and_option(player, EF_CHEAT_GIFT, order_units, unit_ptr - order_units, id);
                }
                return true;
            }
        }
    }
    //debug commands
    /*if (strstr(s, "/TEST"))
    {
        void* player = getCurrentPlayer();
        UNIT** sel_units = player_get_selection(player);
        int n = player_get_n_selection(player);
        for (int i = 0; i < n; i++)
        {
            sel_units[i]->prop_object->garrison_heal_rate *= 100;
            sel_units[i]->prop_object->displayed_pierce_armor *= 10;
        }
        return true;
    }
    if (strstr(s, "/CREATE-UNIT"))
    {
        sscanf(s, "%s %d", dummy, &id);
        prepareToEngageCheatCreateUnit(id);
        return false;
    }
    if (strstr(s, "/RESEARCH-TECH"))
    {
        sscanf(s, "%s %d", dummy, &id);
        prepareToEngageCheatResearchTech(id);
        return false;
    }*/
    /*if (strstr(s, "/UNDO-TECH"))
    {
        restoreCheatFlag = 1;
        sscanf(s, "%s %d", dummy, &id);
        player_undo_tech(getCurrentPlayer(), id);
        return true;
    }*/
    /*if (strstr(s, "/TAKE-CONTROL"))
    {
        sscanf(s, "%s %d", dummy, &id);
        if ((id >= 0) && (id <= 8))
        {
            //if (!control_initiated)
            //    control_source = getCurrentPlayerId();
            RGE_Player__unselect_object(getCurrentPlayer());
            takeControl(id);
            chat("Taking control of player %d", id);
            //control_initiated = true;
            //control_target = id;
        }
        return true;
    }*/
    //
    /*if (strstr(s, "/GET-MASTER"))
    {
        sel_iterator i(getCurrentPlayer());
        UNIT* unit;
        for (; unit = *i, unit != 0; ++i)
        {
            p = unit->prop_object;
            chat("0x%X", p);
        }
        return true;
    }
    if (strstr(s, "/START"))
    {
        time_collect = true;
        return true;
    }
    if (strstr(s, "/STOP"))
    {
        time_collect = false;
        time_stage_find = true;
        return true;
    }
    if (strstr(s, "/YES"))
    {
        answer = true;
        answer_flag = true;
        return true;
    }
    if (strstr(s, "/NO"))
    {
        answer = false;
        answer_flag = true;
        return true;
    }*/
    /*if (strstr(s, "/GLITCH"))
    {
        chat("Total glitched ore: %d", (int)glitched_res);
        return true;
    }
    if (strstr(s, "/GET"))
    {
        char resource_str[0x100];
        float amount;
        int resource;
        int player;
        sscanf(s, "%s %d %d %f", dummy, &player, &resource, &amount);
        chat("Adding resource %d amount %f to player %d", resource, amount, player);
        tribute(player, resource, amount);
        return true;
    }*/
    /*if (strstr(s, "/GET"))
    {
        char resource_str[0x100];
        int amount;
        int resource;
        sscanf(s, "%s %s %d", dummy, resource, &amount);
        if (strstr(resource_str, "FOOD"))
            resource = 0;
        else if (strstr(resource_str, "CARBON"))
            resource = 1;
        else if (strstr(resource_str, "ORE"))
            resource = 2;
        else if (strstr(resource_str, "NOVA"))
            resource = 3;
        sscanf(s, "%s %d %d", dummy, &resource, &amount);
        chat("Tributing resource %d amount %d", resource, amount);
        tribute(resource, amount);
        return true;
    }*/
    /*if (strstr(s, "/SEND"))
    {
        char resource_str[0x100];
        int source;
        int target;
        float amount;
        int resource;
        sscanf(s, "%s %s %d", dummy, resource, &amount);
        if (strstr(resource_str, "FOOD"))
        resource = 0;
        else if (strstr(resource_str, "CARBON"))
        resource = 1;
        else if (strstr(resource_str, "ORE"))
        resource = 2;
        else if (strstr(resource_str, "NOVA"))
        resource = 3;
        sscanf(s, "%s %d %d %d %f", dummy, &source, &target, &resource, &amount);
        chat("Tributing resource %d amount %f from player %d to player %d", resource, amount, source, target);
        tribute(source, target, resource, amount);
        return true;
    }
    if (strstr(s, "/RESIGN"))
    {
        int player;
        sscanf(s, "%s %d", dummy, &player);
        chat("Resigning player %d", player);
        player_resign(get_player(player), player, 1);
        return true;
    }*/
    if (strstr(s, "/SN"))
    {
        char name[0x80];
        name[0] = '\0';
        sscanf_s(s, "%s %s", dummy, (unsigned)_countof(dummy), name, (unsigned)_countof(name));
        get_sn_with_alias((TRIBE_Player*)player, name);
        return true;
    }
    if (strstr(s, "/GOAL"))
    {
        char name[0x80];
        name[0] = '\0';
        sscanf_s(s, "%s %s", dummy, (unsigned)_countof(dummy), name, (unsigned)_countof(name));
        get_goal_with_alias((TRIBE_Player*)player, name);
        return true;
    }
    /*if (strstr(s, "/OBJ") || strstr(s, "/OBJECT"))
    {
        sscanf(s, "%s %d", dummy, &id);
        void* base_world = *(void**)((char*)*base_game + 0x420);
        if (base_world)
        {
            UNIT* unit = (UNIT*)BaseWorld__object(base_world, id);
            if (unit)
            {
                void* player = getCurrentPlayer();
                RGE_Player__unselect_object(player);
                RGE_Player__select_object(player, unit, 1);
            }
            else
                chat("Invalid object id");
        }

        return true;
    }
    if (strstr(s, "/GOTO"))
    {
        float x, y;
        sscanf(s, "%s %f %f", dummy, &x, &y);
        void* player = getCurrentPlayer();
        RGE_Player__set_view_loc(player, x, y, 0);
        return true;
    }*/
    return false;
}

__declspec(naked) void scanChat() //005ED970
{
    __asm
    {
        mov     eax, [esp + 8] //chat string
        push    ecx

        push    eax
        call    checkCheats
        pop     ecx
        sub     esp, 300h
        push    ebx
        push    edi
        test    al, al
        jnz     no_chat
        mov     edi, 005ED978h
        jmp     edi
no_chat:
        mov     edi, 005EDD65h
        jmp     edi
    }
}

extern const float const_f_zero;

__declspec(naked) void checkPowerResource() //0054BE05
{
    __asm
    {
        mov     esi, [esi + 18h]
        mov     ecx, [esi + 0A8h]
        cmp     ecx, 211
        jle     not_always_powered
        mov     ecx, [esi + 0ACh]
        fld     dword ptr [ecx + 211 * 4]
        mov     ecx, eax
        fcomp   const_f_zero
        fnstsw  ax
        test    ah, 41h
        mov     eax, ecx
        jz      always_powered
not_always_powered:
        shl     edi, 5
        mov     edx, 0054BE0Bh
        jmp     edx
always_powered:
        mov     edx, 0054BE36h
        jmp     edx
    }
}

#pragma optimize( "s", on )
void setAdvCheatHooks()
{
    setHook((void*)0x005ED970, scanChat);
    setHook((void*)0x0054BE05, checkPowerResource);

    //tech
    //writeByte(0x005BFE08, 0xEB);
}
#pragma optimize( "", on )
