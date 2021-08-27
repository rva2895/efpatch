#include "stdafx.h"

#include "advcheat.h"
#include "advtriggereffect.h"
#include "effects.h"
#include "sngoal.h"
#include "tribe_command_ef.h"

#pragma warning(push)
#pragma warning(disable:4100)
__declspec(naked) void __stdcall takeControl(int p)
{
    __asm
    {
        mov     eax, [esp + 4]
        push    esi
        push    edi
        push    ebp
        push    ebx
        mov     ecx, 6A3684h
        mov     ecx, [ecx]
        push    eax
        mov     edx, 005E68B0h //takeControl
        call    edx
        pop     ebx
        pop     ebp
        pop     edi
        pop     esi
        ret     4
    }
}
#pragma warning(pop)

//extern float glitched_res; //remove

int (__thiscall* player_add_attribute) (void *player, int resource, float amount) =
    (int(__thiscall*) (void*, int, float))0x004C3AE0; //0x005D07A0

int (__thiscall* player_command_resign) (void *player, int unk1, int unk2) =
    (int(__thiscall*) (void*, int, int))0x004C3B80;

char (__thiscall* TRIBE_Player_Tech__undo_tech) (void *player_tech, __int16 tech_id) =
    (char(__thiscall*) (void*, __int16))0x005BFDD2;

void __stdcall player_undo_tech(void* player, short tech)
{
    TRIBE_Player_Tech__undo_tech(*(void**)((DWORD)player + 0x1D94), tech);
}

void tribute(int player, int resource, float amount)
{
    //void* gaia = get_player(0);
    //void* target_p;
    /*if (target == -1)
    {
        target_p = getCurrentPlayer();
        int i;
        for (i = 0; i <= 8; i++)
            if (get_player(i) == target_p)
            {
                target = i;
                break;
            }
    }*/
    void* source_p = get_player(player);
    player_add_attribute(source_p, resource, amount);
}

//
//extern int control_source;
//extern int control_target;
//extern bool control_initiated;

int player_id_for_gaia_control = 0;

int __stdcall getCurrentPlayerId()
{
    void* c = getCurrentPlayer();
    for (int i = 0; i <= 8; i++)
        if (c == get_player(i))
            return i;
    return -1;
}

bool __stdcall checkCheats(char* s2)
{
    char dummy[100];
    char s[0x200];
    strncpy(s, s2, 0x1FF);
    s[0x1FF] = '\0';
    _strupr(s);

    if (BaseGame__allowCheatCodes(*BaseGame_bg))
    {
        //fixed old cheats, SP only
        if (BaseGame__singlePlayerGame(*BaseGame_bg))
        {
            if (strstr(s, "SIMONSAYS"))
            {
                void* player = getCurrentPlayer();
                make_cheat_by_id_with_position(player, EF_CHEAT_SIMONSAYS,
                    floor(player_get_camera_x(player)) + 0.5f, floor(player_get_camera_y(player)) + 0.5f);
                return true;
            }
            if (strstr(s, "SCARYNEIGHBOR"))
            {
                void* player = getCurrentPlayer();
                make_cheat_by_id_with_position(player, EF_CHEAT_SCARYNEIGHBOR,
                    floor(player_get_camera_x(player)) + 0.5f, floor(player_get_camera_y(player)) + 0.5f);
                return true;
            }
            if (strstr(s, "THAT'S NO MOON"))
            {
                void* player = getCurrentPlayer();
                make_cheat_by_id_with_position(player, EF_CHEAT_THATS_NO_MOON,
                    floor(player_get_camera_x(player)) + 0.5f, floor(player_get_camera_y(player)) + 0.5f);
                return true;
            }
            if (strstr(s, "IMPERIAL ENTANGLEMENTS"))
            {
                void* player = getCurrentPlayer();
                make_cheat_by_id_with_position(player, EF_CHEAT_IMPERIAL_ENTANGLEMENTS,
                    floor(player_get_camera_x(player)) + 0.5f, floor(player_get_camera_y(player)) + 0.5f);
                return true;
            }
            if (strstr(s, "TANTIVE IV"))
            {
                void* player = getCurrentPlayer();
                make_cheat_by_id_with_position(player, EF_CHEAT_TANTIVE_IV,
                    floor(player_get_camera_x(player)) + 0.5f, floor(player_get_camera_y(player)) + 0.5f);
                return true;
            }
            if (strstr(s, "GALACTIC UPHEAVAL"))
            {
                void* player = getCurrentPlayer();
                make_cheat_by_id_with_position(player, EF_CHEAT_GALACTIC_UPHEAVAL,
                    floor(player_get_camera_x(player)) + 0.5f, floor(player_get_camera_y(player)) + 0.5f);
                return true;
            }
            //new ef cheats, SP only
            if (strstr(s, "LUMINOUS BEINGS ARE WE"))
            {
                void* player = getCurrentPlayer();
                int player_id = getCurrentPlayerId();
                WorldPlayerBase__unselect_object(player);
                if (player_id != 0)
                {
                    player_id_for_gaia_control = player_id;
                    takeControl(0);
                }
                else
                    takeControl(player_id_for_gaia_control);
                return true;
            }
            if (strstr(s, "YOU HAVE FAILED ME FOR THE LAST TIME"))
            {
                void* player = getCurrentPlayer();
                UNIT** sel_units = player_get_selection(player);
                int n = player_get_n_selection(player);
                UNIT* order_units[40];
                UNIT** unit_ptr = order_units;
                for (int i = 0; i < n; i++)
                    if (sel_units[i]->player != player && sel_units[i]->prop_object->type > 30)
                        *unit_ptr++ = sel_units[i];
                make_cheat_by_id_with_unit_list(player, EF_CHEAT_YOU_HAVE_FAILED_ME_FOR_THE_LAST_TIME, order_units, unit_ptr - order_units);
                return true;
            }
            if (strstr(s, "JOIN US OR DIE"))
            {
                void* player = getCurrentPlayer();
                UNIT** sel_units = player_get_selection(player);
                int n = player_get_n_selection(player);
                UNIT* order_units[40];
                UNIT** unit_ptr = order_units;
                for (int i = 0; i < n; i++)
                    if (sel_units[i]->player != player)
                        *unit_ptr++ = sel_units[i];
                make_cheat_by_id_with_unit_list(player, EF_CHEAT_JOIN_US_OR_DIE, order_units, unit_ptr - order_units);
                return true;
            }
            if (strstr(s, "ULTIMATE POWER IN THE UNIVERSE"))
            {
                void* player = getCurrentPlayer();
                make_cheat_by_id_with_position(player, EF_CHEAT_ULTIMATE_POWER_IN_THE_UNIVERSE,
                    floor(player_get_camera_x(player)) + 0.5f, floor(player_get_camera_y(player)) + 0.5f);
                return true;
            }
            if (strstr(s, "KOELSCH"))
            {
                void* player = getCurrentPlayer();
                make_cheat_by_id_with_position(player, EF_CHEAT_KOELSCH,
                    floor(player_get_camera_x(player)) + 0.5f, floor(player_get_camera_y(player)) + 0.5f);
                return true;
            }
        }
        //new EF cheats, SP and MP
        if (strstr(s, "HELP ME OBI-WAN"))
        {
            make_cheat_by_id(getCurrentPlayer(), EF_CHEAT_HELP_ME_OBIWAN);
            return true;
        }
        if (strstr(s, "NOW THIS IS PODRACING"))
        {
            make_cheat_by_id(getCurrentPlayer(), EF_CHEAT_NOW_THIS_IS_PODRACING);
            return true;
        }
        if (strstr(s, "THE DEFLECTOR SHIELD IS TOO STRONG"))
        {
            void* player = getCurrentPlayer();
            UNIT** sel_units = player_get_selection(player);
            int n = player_get_n_selection(player);
            UNIT* order_units[40];
            UNIT** unit_ptr = order_units;
            for (int i = 0; i < n; i++)
                if (sel_units[i]->player == player)
                    *unit_ptr++ = sel_units[i];
            make_cheat_by_id_with_unit_list(player, EF_CHEAT_THE_DEFLECTOR_SHIELD_IS_TOO_STRONG, order_units, unit_ptr - order_units);
            return true;
        }
        if (strstr(s, "NO SHIP THAT SMALL HAS A CLOAKING DEVICE"))
        {
            void* player = getCurrentPlayer();
            UNIT** sel_units = player_get_selection(player);
            int n = player_get_n_selection(player);
            UNIT* order_units[40];
            UNIT** unit_ptr = order_units;
            for (int i = 0; i < n; i++)
                if (sel_units[i]->player == player)
                    *unit_ptr++ = sel_units[i];
            make_cheat_by_id_with_unit_list(player, EF_CHEAT_NO_SHIP_THAT_SMALL_HAS_A_CLOAKING_DEVICE, order_units, unit_ptr - order_units);
            return true;
        }
        if (strstr(s, "FORCEHEAL"))
        {
            void* player = getCurrentPlayer();
            UNIT** sel_units = player_get_selection(player);
            int n = player_get_n_selection(player);
            UNIT* order_units[40];
            UNIT** unit_ptr = order_units;
            for (int i = 0; i < n; i++)
                if (sel_units[i]->player == player)
                    *unit_ptr++ = sel_units[i];
            make_cheat_by_id_with_unit_list(player, EF_CHEAT_FORCEHEAL, order_units, unit_ptr - order_units);
            return true;
        }
        if (strstr(s, "FORCEPROTECT"))
        {
            void* player = getCurrentPlayer();
            UNIT** sel_units = player_get_selection(player);
            int n = player_get_n_selection(player);
            UNIT* order_units[40];
            UNIT** unit_ptr = order_units;
            for (int i = 0; i < n; i++)
                if (sel_units[i]->player == player)
                    *unit_ptr++ = sel_units[i];
            make_cheat_by_id_with_unit_list(player, EF_CHEAT_FORCEPROTECT, order_units, unit_ptr - order_units);
            return true;
        }
        if (strstr(s, "UNLIMITED POWER"))
        {
            make_cheat_by_id(getCurrentPlayer(), EF_CHEAT_UNLIMITED_POWER);
            return true;
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
            WorldPlayerBase__unselect_object(getCurrentPlayer());
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
        char name[0x100];
        sscanf(s, "%s %s", dummy, name);
        get_sn_with_alias(getCurrentPlayer(), name);
        return true;
    }
    if (strstr(s, "/GOAL"))
    {
        char name[0x100];
        sscanf(s, "%s %s", dummy, name);
        get_goal_with_alias(getCurrentPlayer(), name);
        return true;
    }
    /*if (strstr(s, "/OBJ") || strstr(s, "/OBJECT"))
    {
        sscanf(s, "%s %d", dummy, &id);
        void* base_world = *(void**)((char*)*BaseGame_bg + 0x420);
        if (base_world)
        {
            UNIT* unit = (UNIT*)BaseWorld__object(base_world, id);
            if (unit)
            {
                void* player = getCurrentPlayer();
                WorldPlayerBase__unselect_object(player);
                WorldPlayerBase__select_object(player, unit, 1);
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
        WorldPlayerBase__set_view_loc(player, x, y, 0);
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
        mov     ecx, [esi + 0ACh]
        fld     dword ptr [ecx + 211 * 4]
        mov     ecx, eax
        fcomp   const_f_zero
        fnstsw  ax
        test    ah, 41h
        mov     eax, ecx
        jz      always_powered
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
