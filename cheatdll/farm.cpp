#include "stdafx.h"
#include "farm.h"
#include "harbor.h"

extern int current_loaded_version;

#define AQUA_HARVESTER_ID 199

#define GAME_COMMAND_AQUA_HARVESTER_QUEUE_ADD 15
#define GAME_COMMAND_AQUA_HARVESTER_QUEUE_REMOVE 16
#define GAME_COMMAND_SET_FARM_AUTO_QUEUE 17
#define GAME_COMMAND_SET_AQUA_HARVESTER_AUTO_QUEUE 18

#define BUTTON_PIC_FARM_AUTO_QUEUE_ENABLE 22
#define BUTTON_PIC_FARM_AUTO_QUEUE_DISABLE 23
#define BUTTON_PIC_AQUA_HARVESTER_AUTO_QUEUE_ENABLE 24
#define BUTTON_PIC_AQUA_HARVESTER_AUTO_QUEUE_DISABLE 25

#define AUTO_FARM_QUEUE_ENABLE_BUTTON 1001
#define AUTO_FARM_QUEUE_DISABLE_BUTTON 1002
#define AUTO_AQUA_HARVESTER_QUEUE_ENABLE_BUTTON 1003
#define AUTO_AQUA_HARVESTER_QUEUE_DISABLE_BUTTON 1004
#define ADD_AQUA_HARVESTER_QUEUE_BUTTON 1005
#define ADD_AQUA_HARVESTER_QUEUE_SHIFT_BUTTON 1006
#define REMOVE_AQUA_HARVESTER_QUEUE_BUTTON 1007

int __stdcall process_fishing_ship_notify(TribeFishingShipUnitAIModule* unit_ai, NotifyEvent* nEvent)
{
    if (current_loaded_version >= 8)
    {
        int target_type = unit_ai->currentTargetTypeValue;
        if (target_type != -1 || (target_type = unit_ai->lastTargetTypeValue, target_type != -1) || (target_type = nEvent->p2, target_type != -1))
        {
            int resource_type = unit_ai->vfptr->convertToLOSResourceType(unit_ai, target_type);
            int obj;
            if (resource_type == -1)
            {
                if (nEvent->p2 == 7)
                    obj = unit_ai->vfptr->closestObject(unit_ai, target_type, -1, -1, AQUA_HARVESTER_ID, 1, 0);
                else
                    obj = unit_ai->vfptr->closestObject(unit_ai, target_type, -1, -1, -1, -1, 0);
            }
            else
            {
                obj = unit_ai->vfptr->closestResourceObject(unit_ai, resource_type, nEvent->p3, -1, 0);
            }
            if (obj != -1)
            {
                if (unit_ai->vfptr->gatherObject(unit_ai, obj, 1) == 1)
                    return 3;
            }
        }
        if (unit_ai->objectValue->owner->vfptr->computerPlayer(unit_ai->objectValue->owner) == 1)
        {
            UnitAIModule__removeCurrentTarget((UnitAIModule*)unit_ai);
            unit_ai->vfptr->stopObject(unit_ai, 1);
            unit_ai->vfptr->notifyCommander_1(unit_ai, nEvent);
        }
        return 2;
    }
    else
        return 0;
}

__declspec(naked) void fishing_ship_process_notify() //005B633D
{
    __asm
    {
        cmp     dword ptr [esi + 30h], 25Ah
        jz      fishing_ship_process_notify_continue
        cmp     dword ptr [esi + 30h], 609  //gather
        jnz     fishing_ship_process_notify_exit
        push    edi
        push    esi
        call    process_fishing_ship_notify
        test    eax, eax
        jz      fishing_ship_process_notify_exit
        mov     edx, 005B64D9h
        jmp     edx

fishing_ship_process_notify_continue:
        mov     edx, 005B634Ah
        jmp     edx

fishing_ship_process_notify_exit:
        mov     edx, 005B64CCh
        jmp     edx
    }
}

void __stdcall action_gather_set_farm_xy(RGE_Action_Gather* action)
{
    if (current_loaded_version >= 8)
    {
        action->farm_x = action->target_x;
        action->farm_y = action->target_y;
    }
}

__declspec(naked) void action_gather_farm_xy_1() //00405FDE
{
    __asm
    {
        cmp     word ptr [eax + 18h], 32h
        jz      action_gather_farm_xy_1_farm
        cmp     word ptr [eax + 18h], AQUA_HARVESTER_ID
        jnz     action_gather_farm_xy_1_not_farm
        push    esi
        call    action_gather_set_farm_xy

action_gather_farm_xy_1_not_farm:
        mov     eax, 00406058h
        jmp     eax

action_gather_farm_xy_1_farm:
        mov     eax, 00405FE5h
        jmp     eax
    }
}

__declspec(naked) void action_gather_farm_xy_2() //004041DD
{
    __asm
    {
        cmp     word ptr [ecx + 18h], 32h
        jz      action_gather_farm_xy_2_farm
        cmp     word ptr [ecx + 18h], AQUA_HARVESTER_ID
        jnz     action_gather_farm_xy_2_not_farm

        mov     eax, current_loaded_version
        cmp     eax, 8
        jge     action_gather_farm_xy_2_farm

action_gather_farm_xy_2_not_farm:
        mov     eax, 004041F0h
        jmp     eax

action_gather_farm_xy_2_farm:
        mov     eax, 004041E4h
        jmp     eax
    }
}

int TRIBE_Player__add_to_aquaharvester_queue(TRIBE_Player* player, int count)
{
    while (count > 0)
    {
        if (!TRIBE_Player__check_obj_cost(player, AQUA_HARVESTER_ID, NULL, count, 1, 0, 0))
            count--;
        else
            break;
    }
    if (count <= 0)
        return 0;

    TRIBE_Master_Combat_Object* aqua_harvester = (TRIBE_Master_Combat_Object*)player->master_objects[AQUA_HARVESTER_ID];
    for (int i = 0; i < 3; i++)
    {
        if (aqua_harvester->build_inventory[i].type >= 0 && aqua_harvester->build_inventory[i].flag)
        {
            float* attribute = &player->attributes[aqua_harvester->build_inventory[i].type];
            *attribute -= (count * aqua_harvester->build_inventory[i].amount);
        }
    }

    player->aqua_harvester_queue_count_new += count;
    return 1;
}

int TRIBE_Player__remove_from_aquaharvester_queue(TRIBE_Player* player, int count)
{
    if (count > player->aqua_harvester_queue_count_new)
        count = player->aqua_harvester_queue_count_new;
    if (count < 1)
        return 0;
    player->aqua_harvester_queue_count_new -= count;
    TRIBE_Master_Combat_Object* aqua_harvester = (TRIBE_Master_Combat_Object*)player->master_objects[AQUA_HARVESTER_ID];
    if (aqua_harvester->master_type >= 70)
    {
        for (int i = 0; i < 3; i++)
        {
            if (aqua_harvester->build_inventory[i].type >= 0 && aqua_harvester->build_inventory[i].flag)
            {
                float* attribute = &player->attributes[aqua_harvester->build_inventory[i].type];
                *attribute += (count * aqua_harvester->build_inventory[i].amount);
            }
        }
    }
    return 1;
}

void TRIBE_Player__set_farm_auto_queue(TRIBE_Player* player, __int8 val)
{
    if (current_loaded_version >= 8)
        player->farm_auto_reseed = val;
}

void TRIBE_Player__set_aqua_harvester_auto_queue(TRIBE_Player* player, __int8 val)
{
    if (current_loaded_version >= 8)
        player->aqua_harvester_auto_reseed = val;
}

void __stdcall new_command_game_dispatch(TRIBE_Command* command, TRIBE_Command_Game* order)
{
    switch (order->game_command)
    {
    case GAME_COMMAND_AQUA_HARVESTER_QUEUE_ADD:
        TRIBE_Player__add_to_aquaharvester_queue((TRIBE_Player*)command->world->players[order->var1], order->var2);
        break;
    case GAME_COMMAND_AQUA_HARVESTER_QUEUE_REMOVE:
        TRIBE_Player__remove_from_aquaharvester_queue((TRIBE_Player*)command->world->players[order->var1], order->var2);
        break;
    case GAME_COMMAND_SET_FARM_AUTO_QUEUE:
        TRIBE_Player__set_farm_auto_queue((TRIBE_Player*)command->world->players[order->var1], order->var2);
        if (command->world->curr_player == order->var1)
            TRIBE_Screen_Game__object_changed(((TRIBE_Game*)(*base_game))->game_screen);
        break;
    case GAME_COMMAND_SET_AQUA_HARVESTER_AUTO_QUEUE:
        TRIBE_Player__set_aqua_harvester_auto_queue((TRIBE_Player*)command->world->players[order->var1], order->var2);
        if (command->world->curr_player == order->var1)
            TRIBE_Screen_Game__object_changed(((TRIBE_Game*)(*base_game))->game_screen);
        break;
    default:
        break;
    }
}

__declspec(naked) void on_do_command_game() //005BB74A
{
    __asm
    {
        mov     ecx, [esp + 514h]
        push    esi
        push    edi
        call    new_command_game_dispatch

        mov     edi, 005BB751h
        jmp     edi
    }
}

int __fastcall TRIBE_Player__get_farm_queue_count_new(TRIBE_Player* player) //005D7420
{
    if (current_loaded_version >= 8)
        return player->farm_queue_count_new;
    else
        return player->farm_queue_count;
}

int __fastcall TRIBE_Player__get_aqua_harvester_queue_count_new(TRIBE_Player* player) //005D7420
{
    if (current_loaded_version >= 8)
        return player->aqua_harvester_queue_count_new;
    else
        return 0;
}

__declspec(naked) void on_add_to_farm_queue() //005D7509
{
    __asm
    {
        mov     eax, current_loaded_version
        cmp     eax, 8
        jge     add_to_farm_queue_new
        mov     eax, [esi + 29A0h]
        add     eax, edi
        mov     [esi + 29A0h], eax
        jmp     add_to_farm_queue_end

add_to_farm_queue_new:
        movsx   eax, byte ptr [esi + 29A0h]
        add     eax, edi
        mov     [esi + 29A0h], al

add_to_farm_queue_end:
        pop     ebx
        pop     edi
        mov     eax, 005D7519h
        jmp     eax
    }
}

__declspec(naked) void on_remove_from_farm_queue_1() //005D7530
{
    __asm
    {
        mov     eax, current_loaded_version
        cmp     eax, 8
        jge     remove_from_farm_queue_1_new
        mov     eax, [ecx + 29A0h]
        jmp     remove_from_farm_queue_1_end

remove_from_farm_queue_1_new:
        movsx   eax, byte ptr [ecx + 29A0h]

remove_from_farm_queue_1_end:
        mov     edx, 005D7536h
        jmp     edx
    }
}

__declspec(naked) void on_remove_from_farm_queue_2() //005D754E
{
    __asm
    {
        mov     edx, current_loaded_version
        cmp     edx, 8
        jge     remove_from_farm_queue_2_new
        mov     [ecx + 29A0h], eax
        jmp     remove_from_farm_queue_2_end

remove_from_farm_queue_2_new:
        mov     [ecx + 29A0h], al

remove_from_farm_queue_2_end:
        mov     eax, 005D7554h
        jmp     eax
    }
}

void TRIBE_Player__use_farm_queue_new(TRIBE_Player* player, __int16 farm_obj_id)
{
    if (farm_obj_id == AQUA_HARVESTER_ID)
    {
        if (current_loaded_version >= 8)
        {
            player->aqua_harvester_queue_count_new--;
            if (player->aqua_harvester_queue_count_new < 0)
                player->aqua_harvester_queue_count_new = 0;
        }
    }
    else
    {
        if (current_loaded_version >= 8)
        {
            player->farm_queue_count_new--;
            if (player->farm_queue_count_new < 0)
                player->farm_queue_count_new = 0;
        }
        else
        {
            player->farm_queue_count--;
            if (player->farm_queue_count < 0)
                player->farm_queue_count = 0;
        }
    }
}

int try_auto_queue_farm(TRIBE_Player* player, __int16 farm_obj_id)
{
    __int8 auto_queue_enabled;
    if (current_loaded_version >= 8)
    {
        if (farm_obj_id == AQUA_HARVESTER_ID)
            auto_queue_enabled = player->aqua_harvester_auto_reseed;
        else
            auto_queue_enabled = player->farm_auto_reseed;

        if (auto_queue_enabled && TRIBE_Player__check_obj_cost(player, farm_obj_id, NULL, 1, 1, 0, 0))
            return 1;
        else
            return 0;
    }
    else
        return 0;
}

void pay_obj_cost(TRIBE_Player* player, __int16 obj_id)
{
    TRIBE_Master_Combat_Object* obj = (TRIBE_Master_Combat_Object*)player->master_objects[obj_id];
    for (int i = 0; i < 3; i++)
    {
        if (obj->build_inventory[i].type >= 0 && obj->build_inventory[i].flag)
        {
            float* attribute = &player->attributes[obj->build_inventory[i].type];
            *attribute -= obj->build_inventory[i].amount;
        }
    }
}

int __fastcall RGE_Action_Gather__auto_rebuild_farm_new(RGE_Action_Gather* action)
{
    if (action->farm_x == -1.0f || action->farm_y == -1.0f)
        return 0;

    __int16 farm_obj_id;
    int count;
    if (action->obj->master_obj->object_group == 14)
    {
        if (current_loaded_version < 8)
            return 0;
        farm_obj_id = AQUA_HARVESTER_ID;
        count = action->obj->owner->aqua_harvester_queue_count_new;
    }
    else
    {
        farm_obj_id = 50;
        if (current_loaded_version >= 8)
            count = action->obj->owner->farm_queue_count_new;
        else
            count = action->obj->owner->farm_queue_count;
    }

    if (count <= 0)
    {
        if (!try_auto_queue_farm(action->obj->owner, farm_obj_id))
            return 0;
    }

    TRIBE_Master_Building_Object* farm_bldg = (TRIBE_Master_Building_Object*)action->obj->owner->master_objects[farm_obj_id];
    if (farm_bldg->vfptr->check_placement(farm_bldg, (RGE_Player*)action->obj->owner, action->farm_x, action->farm_y, NULL, 1, 1, 1, 1, 1, 1, NULL, 0))
        return 0;

    RGE_Static_Object* new_farm = farm_bldg->vfptr->make_new_obj_2(farm_bldg, (RGE_Player*)action->obj->owner, action->farm_x, action->farm_y, 0.0f, -1);
    if (!new_farm)
        return 0;

    if (count <= 0)
        pay_obj_cost(action->obj->owner, farm_obj_id);
    else
        TRIBE_Player__use_farm_queue_new(action->obj->owner, farm_obj_id);

    RGE_Static_Object__removeAllFromPathingGroup((RGE_Static_Object*)action->obj);
    action->obj->vfptr->do_command(action->obj, new_farm, action->farm_x, action->farm_y, 0.0f);

    return 1;
}

extern void* iconsBldgPtr;

void __stdcall food_proc_display_auto_queue_button(TRIBE_Screen_Game* game_screen, TRIBE_Player* player)
{
    char msg_str[0x200];
    int civ = game_screen->game_obj->vfptr->gbg_get_civ_override(game_screen->game_obj);
    TShape** bldg_pics = (TShape**)iconsBldgPtr;

    (*base_game)->vfptr->get_string3(
        (*base_game),
        player->farm_auto_reseed ? 4148 : 4147,
        msg_str,
        _countof(msg_str));
    TRIBE_Screen_Game__set_button(
        game_screen,
        game_screen->button_cmd_pic,
        7,
        player->farm_auto_reseed ? BUTTON_PIC_FARM_AUTO_QUEUE_DISABLE : BUTTON_PIC_FARM_AUTO_QUEUE_ENABLE,
        player->farm_auto_reseed ? AUTO_FARM_QUEUE_DISABLE_BUTTON : AUTO_FARM_QUEUE_ENABLE_BUTTON,
        -1,
        player->farm_auto_reseed ? 4948 : 4947,
        0,
        -1,
        NULL,
        NULL,
        msg_str,
        0);
}

__declspec(naked) void on_food_proc_auto_queue_icon() //00504FCE
{
    __asm
    {
        push    ebp
        push    esi
        call    food_proc_display_auto_queue_button
        mov     eax, [ebp + 74h]
        mov     ecx, [eax + 0C8h]
        mov     edx, 00504FD7h
        jmp     edx
    }
}

void __stdcall harbor_display_aqua_harvester_buttons(TRIBE_Screen_Game* game_screen, TRIBE_Player* player)
{
    if (game_screen->game_obj->master_obj->id == HARBOR_ID_COMPLETE && game_screen->game_obj->object_state == 2)
    {
        char msg_str[0x200];
        RGE_Master_Static_Object* aqua_harvester = (RGE_Master_Static_Object*)player->master_objects[AQUA_HARVESTER_ID];
        if (!aqua_harvester->disabled)
        {
            Item_Avail item;
            __int16 inv_1;
            __int16 inv_amt_1;
            __int16 inv_2;
            __int16 inv_amt_2;
            __int16 inv_3;
            __int16 inv_amt_3;
            item.id = AQUA_HARVESTER_ID;
            item.pict = aqua_harvester->button_pict;
            item.name = aqua_harvester->name;
            TRIBE_Player__obj_cost(player, AQUA_HARVESTER_ID, 0, &inv_1, &inv_amt_1, &inv_2, &inv_amt_2, &inv_3, &inv_amt_3, 1.0);
            item.inventory_item_1 = inv_1;
            item.inventory_cost_1 = inv_amt_1;
            item.inventory_item_2 = inv_2;
            item.inventory_cost_2 = inv_amt_2;
            item.inventory_item_3 = inv_3;
            item.inventory_cost_3 = inv_amt_3;
            item.help_string_id = RGE_Master_Static_Object__get_help_message(aqua_harvester);
            item.help_page_id = RGE_Master_Static_Object__get_help_page(aqua_harvester);
            TRIBE_Screen_Game__calc_text_msg(game_screen, msg_str, &item, -1, 4139);
            TShape** bldg_pics = (TShape**)iconsBldgPtr;
            int civ = game_screen->game_obj->vfptr->gbg_get_civ_override(game_screen->game_obj);
            TRIBE_Screen_Game__set_button(game_screen, bldg_pics[civ], 4, item.pict, ADD_AQUA_HARVESTER_QUEUE_BUTTON, AQUA_HARVESTER_ID, 4939, 0, -1, 0, 0, msg_str, 0);
            game_screen->buttons[4]->shift_id1 = ADD_AQUA_HARVESTER_QUEUE_SHIFT_BUTTON;
            game_screen->buttons[4]->shift_id2 = 0;
        }
        (*base_game)->vfptr->get_string3(
            (*base_game),
            player->aqua_harvester_auto_reseed ? 4143 : 4142,
            msg_str,
            _countof(msg_str));
        TRIBE_Screen_Game__set_button(
            game_screen,
            game_screen->button_cmd_pic,
            7,
            player->aqua_harvester_auto_reseed ? BUTTON_PIC_AQUA_HARVESTER_AUTO_QUEUE_DISABLE : BUTTON_PIC_AQUA_HARVESTER_AUTO_QUEUE_ENABLE,
            player->aqua_harvester_auto_reseed ? AUTO_AQUA_HARVESTER_QUEUE_DISABLE_BUTTON : AUTO_AQUA_HARVESTER_QUEUE_ENABLE_BUTTON,
            -1,
            player->aqua_harvester_auto_reseed ? 4943 : 4942,
            0,
            -1,
            NULL,
            NULL,
            msg_str,
            0);
    }
}

__declspec(naked) void on_harbor_aqua_harvester_icons() //00505103
{
    __asm
    {
        mov     edi, [esi + 1280h]
        push    ebp
        push    esi
        call    harbor_display_aqua_harvester_buttons
        mov     eax, 00505109h
        jmp     eax
    }
}

void TRIBE_Command__command_add_aqua_harvester(TRIBE_Command* command, int player_id, int count)
{
    TRIBE_Command_Game* order = (TRIBE_Command_Game*)calloc_internal(1, sizeof(TRIBE_Command_Game));
    if (order)
    {
        order->command = 103;
        order->game_command = GAME_COMMAND_AQUA_HARVESTER_QUEUE_ADD;
        order->var1 = player_id;
        order->var2 = count;
        RGE_Command__submit((RGE_Command*)command, order, sizeof(TRIBE_Command_Game), 0);
    }
}

void TRIBE_Command__command_remove_aqua_harvester(TRIBE_Command* command, int player_id, int count)
{
    TRIBE_Command_Game* order = (TRIBE_Command_Game*)calloc_internal(1, sizeof(TRIBE_Command_Game));
    if (order)
    {
        order->command = 103;
        order->game_command = GAME_COMMAND_AQUA_HARVESTER_QUEUE_REMOVE;
        order->var1 = player_id;
        order->var2 = count;
        RGE_Command__submit((RGE_Command*)command, order, sizeof(TRIBE_Command_Game), 0);
    }
}

void TRIBE_Command__command_set_farm_auto_queue(TRIBE_Command* command, int player_id, int val)
{
    TRIBE_Command_Game* order = (TRIBE_Command_Game*)calloc_internal(1, sizeof(TRIBE_Command_Game));
    if (order)
    {
        order->command = 103;
        order->game_command = GAME_COMMAND_SET_FARM_AUTO_QUEUE;
        order->var1 = player_id;
        order->var2 = val;
        RGE_Command__submit((RGE_Command*)command, order, sizeof(TRIBE_Command_Game), 0);
    }
}

void TRIBE_Command__command_set_aqua_harvester_auto_queue(TRIBE_Command* command, int player_id, int val)
{
    TRIBE_Command_Game* order = (TRIBE_Command_Game*)calloc_internal(1, sizeof(TRIBE_Command_Game));
    if (order)
    {
        order->command = 103;
        order->game_command = GAME_COMMAND_SET_AQUA_HARVESTER_AUTO_QUEUE;
        order->var1 = player_id;
        order->var2 = val;
        RGE_Command__submit((RGE_Command*)command, order, sizeof(TRIBE_Command_Game), 0);
    }
}

void TRIBE_Screen_Game__command_add_aqua_harvester(TRIBE_Screen_Game* game_screen, int count)
{
    char str[0x200];

    if (!RGE_Base_Game__get_paused(*base_game, 0) && game_screen->game_obj && game_screen->game_obj->master_obj->id == HARBOR_ID_COMPLETE)
    {
        TRIBE_Player* player = (TRIBE_Player*)RGE_Base_Game__get_player(*base_game);
        int queue_count = TRIBE_Player__get_aqua_harvester_queue_count_new(player);
        if (queue_count < 40)
        {
            if (count + queue_count > 40)
            {
                count = 40 - queue_count;
            }
            if (count <= 0)
                return;

            bool not_enough_resources_flag = false;
            __int16 attribute_needed;
            while (count > 0)
            {
                if (!TRIBE_Player__check_obj_cost(player, AQUA_HARVESTER_ID, &attribute_needed, count, 1, 0, 0))
                {
                    count--;
                    not_enough_resources_flag = true;
                }
                else
                    break;
            }

            if (not_enough_resources_flag)
            {
                RGE_Base_Game__play_sound(*base_game, 3, 0, 0);
                (*base_game)->vfptr->get_string4(*base_game, 103, 1, attribute_needed, str, _countof(str));
                TMessagePanel__show_message(game_screen->message_panel, 1, str, 0x24, 0, 0, 0, 0, -1, 0, -1);
            }

            if (count > 0)
            {
                TRIBE_Command__command_add_aqua_harvester(game_screen->world->commands, player->id, count);
            }
        }
        else
        {
            RGE_Base_Game__play_sound(*base_game, 3, 0, 0);
            (*base_game)->vfptr->get_string3(*base_game, 3032, str, _countof(str));
            TMessagePanel__show_message(game_screen->message_panel, 1, str, 0x24, 0, 0, 0, 0, -1, 0, -1);
        }
    }
}

void TRIBE_Screen_Game__command_set_farm_auto_queue(TRIBE_Screen_Game* game_screen, int val)
{
    if (!RGE_Base_Game__get_paused(*base_game, 0) && game_screen->game_obj && game_screen->game_obj->master_obj->id == 68)
    {
        TRIBE_Player* player = (TRIBE_Player*)RGE_Base_Game__get_player(*base_game);
        TRIBE_Command__command_set_farm_auto_queue(game_screen->world->commands, player->id, val);
    }
}

void TRIBE_Screen_Game__command_set_aqua_harvester_auto_queue(TRIBE_Screen_Game* game_screen, int val)
{
    if (!RGE_Base_Game__get_paused(*base_game, 0) && game_screen->game_obj && game_screen->game_obj->master_obj->id == HARBOR_ID_COMPLETE)
    {
        TRIBE_Player* player = (TRIBE_Player*)RGE_Base_Game__get_player(*base_game);
        TRIBE_Command__command_set_aqua_harvester_auto_queue(game_screen->world->commands, player->id, val);
    }
}

void __stdcall game_screen_button_action_dispatch(TRIBE_Screen_Game* game_screen, int action)
{
    switch (action)
    {
    case AUTO_FARM_QUEUE_ENABLE_BUTTON:
        TRIBE_Screen_Game__command_set_farm_auto_queue(game_screen, 1);
        break;
    case AUTO_FARM_QUEUE_DISABLE_BUTTON:
        TRIBE_Screen_Game__command_set_farm_auto_queue(game_screen, 0);
        break;
    case AUTO_AQUA_HARVESTER_QUEUE_ENABLE_BUTTON:
        TRIBE_Screen_Game__command_set_aqua_harvester_auto_queue(game_screen, 1);
        break;
    case AUTO_AQUA_HARVESTER_QUEUE_DISABLE_BUTTON:
        TRIBE_Screen_Game__command_set_aqua_harvester_auto_queue(game_screen, 0);
        break;
    case ADD_AQUA_HARVESTER_QUEUE_BUTTON:
        TRIBE_Screen_Game__command_add_aqua_harvester(game_screen, 1);
        break;
    case ADD_AQUA_HARVESTER_QUEUE_SHIFT_BUTTON:
        TRIBE_Screen_Game__command_add_aqua_harvester(game_screen, 5);
        break;
    default:
        break;
    }
}

__declspec(naked) void on_game_screen_button_action() //004FD1A9
{
    __asm
    {
        inc     edi
        push    edi
        push    esi
        call    game_screen_button_action_dispatch
        mov     ecx, [ebp - 0Ch]
        pop     edi
        pop     esi
        pop     ebx
        mov     eax, 004FD1AFh
        jmp     eax
    }
}

bool __stdcall check_aqua_harvester_queue(TRIBE_Panel_Object* obj_panel, RGE_Static_Object* obj)
{
    return (current_loaded_version >= 8
        && obj->object_state == 2
        && obj->owner == obj_panel->player
        && obj->owner->aqua_harvester_queue_count_new > 0);
}

__declspec(naked) void panel_object_draw_aqua_harvester_queue() //005D9AF3
{
    __asm
    {
        cmp     word ptr [ecx + 18h], 44h
        jz      draw_food_proc_queue
        cmp     word ptr [ecx + 18h], HARBOR_ID_COMPLETE
        jnz     draw_food_proc_queue_end
        push    eax
        push    ebp
        call    check_aqua_harvester_queue
        test    al, al
        jz      draw_food_proc_queue_end
        mov     edx, 005D9B16h
        jmp     edx

draw_food_proc_queue_end:
        mov     edx, 005D9B1Fh
        jmp     edx

draw_food_proc_queue:
        mov     edx, 005D9AFAh
        jmp     edx
    }
}

int __stdcall get_aqua_havester_queue_count(TRIBE_Panel_Object* obj_panel, int garrison_count)
{
    if (current_loaded_version >= 8 && garrison_count == 0)
        return obj_panel->player->aqua_harvester_queue_count_new;
    else
        return 0;
}

__declspec(naked) void panel_object_draw_garrison() //005DD516
{
    __asm
    {
        cmp     word ptr [ecx + 18h], 44h
        jz      draw_harbor_garrison
        cmp     word ptr [ecx + 18h], HARBOR_ID_COMPLETE
        jnz     draw_harbor_garrison_end
        push    ebp
        push    esi
        call    get_aqua_havester_queue_count
        test    eax, eax
        jz      draw_harbor_garrison_end
        mov     edi, 005DD52Fh
        jmp     edi

draw_harbor_garrison_end:
        mov     edi, 005DD539h
        jmp     edi

draw_harbor_garrison:
        mov     edx, 005DD51Dh
        jmp     edx
    }
}

int __stdcall panel_object_garrison_get_button_pict(TRIBE_Panel_Object* obj_panel)
{
    int obj_id;
    if (obj_panel->game_obj->master_obj->id == HARBOR_ID_COMPLETE)
        obj_id = AQUA_HARVESTER_ID;
    else
        obj_id = 50;
    
    return obj_panel->player->master_objects[obj_id]->button_pict;
}

int __stdcall panel_object_garrison_get_button_action(TRIBE_Panel_Object* obj_panel)
{
    return obj_panel->game_obj->master_obj->id == HARBOR_ID_COMPLETE ? -5 : -4;
}

__declspec(naked) void panel_object_draw_garrison_item() //005DD874
{
    __asm
    {
        push    eax
        push    esi
        call    panel_object_garrison_get_button_action
        pop     ecx
        push    eax
        push    ecx
        push    esi
        call    panel_object_garrison_get_button_pict
        mov     ecx, eax
        pop     eax
        push    0
        mov     edx, 005DD882h
        jmp     edx
    }
}

void __stdcall panel_object_do_ungarrison_aqua_harvester(TRIBE_Panel_Object* obj_panel)
{
    TRIBE_Command__command_remove_aqua_harvester((*base_game)->world->commands, obj_panel->player->id, 1);
}

__declspec(naked) void panel_object_ungarrison_aqua_harvester() //005DE997
{
    __asm
    {
        cmp     edx, 0FFFFFFFBh
        jz      ungarrison_aqua_harvester
        mov     eax, [eax + 148h]
        mov     ecx, 005DE99Dh
        jmp     ecx

ungarrison_aqua_harvester:
        push    esi
        call    panel_object_do_ungarrison_aqua_harvester
        mov     eax, 005DEB6Ch
        jmp     eax
    }
}

__declspec(naked) void action_gather_user_released() //00404F54
{
    __asm
    {
        cmp     word ptr [ecx + 18h], 32h
        jz      action_gather_user_released_jmp
        cmp     word ptr [ecx + 18h], AQUA_HARVESTER_ID
        jz      action_gather_user_released_jmp
        mov     ecx, 00404F68h
        jmp     ecx

action_gather_user_released_jmp:
        mov     ecx, 00404F5Bh
        jmp     ecx
    }
}

int __stdcall TribeFishingShipUnitAIModule__convertToLOSResourceType_new(int oType) //005B62A0
{
    switch (oType)
    {
    case 23:
    case 25:
        return 2;
    case 24:
        return 5;
    default:
        return -1;
    }
}

unsigned __int8 __stdcall closest_object_workers_dispatch(__int16 target_id)
{
    if (current_loaded_version >= 8)
        return target_id == 50 || target_id == AQUA_HARVESTER_ID;
    else
        return target_id == 50;
}

__declspec(naked) void closest_object_workers() //0041BDB7
{
    __asm
    {
        mov     dx, word ptr [ecx + 18h]
        push    eax
        push    edx
        call    closest_object_workers_dispatch
        pop     edx
        xchg    eax, edx
        test    dl, dl
        jnz     closest_object_workers_jmp
        mov     edx, 0041BDC9h
        jmp     edx

closest_object_workers_jmp:
        mov     edx, 0041BDBEh
        jmp     edx
    }
}

__declspec(naked) void action_gather_work() //00405F38
{
    __asm
    {
        cmp     word ptr[ecx + 18h], 32h
        jz      action_gather_work_jmp
        cmp     word ptr[ecx + 18h], AQUA_HARVESTER_ID
        jz      action_gather_work_jmp
        mov     eax, 00405F3Fh
        jmp     eax

action_gather_work_jmp:
        mov     edx, 00405F48h
        jmp     edx
    }
}

int __stdcall fishing_ship_get_number_important_objects()
{
    if (current_loaded_version >= 8)
        return 43;
    else
        return 42;
}

__declspec(naked) void fishing_ship_important_objects_size() //005B5FCE
{
    __asm
    {
        call    fishing_ship_get_number_important_objects
        mov     edx, eax
        shl     edx, 2
        push    edx
        mov     dword ptr [esp + 14h], 0
        mov     dword ptr [esi], 006664A8h //offset TribeFishingShipUnitAIModule__vftable
        mov     dword ptr [esi + 164h], eax
        mov     eax, 005B5FEBh
        jmp     eax
    }
}

__declspec(naked) void fishing_ship_important_objects() //005B6230
{
    __asm
    {
        mov     dword ptr [eax + 0A4h], 3Dh
        mov     eax, current_loaded_version
        cmp     eax, 8
        jl      fishing_ship_important_objects_skip
        mov     ecx, [esi + 160h]
        mov     dword ptr [ecx + 0A8h], 7

fishing_ship_important_objects_skip:
        mov     ecx, 005B623Ah
        jmp     ecx
    }
}

#pragma optimize( "s", on )
void setFarmHooks()
{
    setHook((void*)0x005B633D, fishing_ship_process_notify);
    setHook((void*)0x00405FDE, action_gather_farm_xy_1);
    setHook((void*)0x004041DD, action_gather_farm_xy_2);
    setHook((void*)0x00404F54, action_gather_user_released);
    setHook((void*)0x00405F38, action_gather_work);

    setHook((void*)0x0041BDB7, closest_object_workers);

    setHook((void*)0x005BB74A, on_do_command_game);
    setHook((void*)0x005D7420, TRIBE_Player__get_farm_queue_count_new);
    setHook((void*)0x005D7509, on_add_to_farm_queue);
    setHook((void*)0x005D7530, on_remove_from_farm_queue_1);
    setHook((void*)0x005D754E, on_remove_from_farm_queue_2);

    setHook((void*)0x004060B0, RGE_Action_Gather__auto_rebuild_farm_new);

    setHook((void*)0x00504FCE, on_food_proc_auto_queue_icon);
    setHook((void*)0x00505103, on_harbor_aqua_harvester_icons);
    setHook((void*)0x004FD1A9, on_game_screen_button_action);
    setHook((void*)0x005D9AF3, panel_object_draw_aqua_harvester_queue);
    setHook((void*)0x005DD516, panel_object_draw_garrison);
    setHook((void*)0x005DD874, panel_object_draw_garrison_item);
    setHook((void*)0x005DE997, panel_object_ungarrison_aqua_harvester);

    setHook((void*)0x005B62A0, TribeFishingShipUnitAIModule__convertToLOSResourceType_new);

    setHook((void*)0x005B5FCE, fishing_ship_important_objects_size);
    setHook((void*)0x005B6230, fishing_ship_important_objects);
}
#pragma optimize( "", on )
