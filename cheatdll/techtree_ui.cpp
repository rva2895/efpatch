#include "stdafx.h"
#include "techtree_ui.h"

void tech_tree_set_left(TribeTechHelpScreen* tech_tree)
{
    tech_tree->scroll_x_offset = 0;
}

void tech_tree_set_right(TribeTechHelpScreen* tech_tree)
{
    tech_tree->scroll_x_offset = -(tech_tree->max_X_size - tech_tree->max_draw_x_size);
}

void tech_tree_verify_pos(TribeTechHelpScreen* tech_tree)
{
    if (tech_tree->scroll_x_offset > 0)
        tech_tree->scroll_x_offset = 0;
    int min_x_pos = -(tech_tree->max_X_size - tech_tree->max_draw_x_size);
    if (tech_tree->scroll_x_offset < min_x_pos)
        tech_tree->scroll_x_offset = min_x_pos;
}

void tech_tree_scroll_right(TribeTechHelpScreen* tech_tree)
{
    tech_tree->scroll_x_offset -= tech_tree->max_draw_x_size - 48;
    tech_tree_verify_pos(tech_tree);
}

void tech_tree_scroll_left(TribeTechHelpScreen* tech_tree)
{
    tech_tree->scroll_x_offset += tech_tree->max_draw_x_size - 48;
    tech_tree_verify_pos(tech_tree);
}

int get_building_id(int key, int alt_key, int ctrl_key, int shift_key)
{
    RGE_Hotkey_Handler* hotkeys = (RGE_Hotkey_Handler*)TRIBE_Game__get_hotkey_handler((TRIBE_Game*)*base_game);
    switch (RGE_Hotkey_Handler__return_function(hotkeys, 1, key, ctrl_key, alt_key, shift_key))
    {
    case 70:    //airbase
        return 317;
    case 35:    //troop center
        return 87;
    case 36:    //mech factory
        return 101;
    case 37:    //hvy weapons factory
        return 49;
    case 38:    //shipyard
        return 45;
    case 39:    //temple
        return 104;
    case 40:    //war center
        return 103;
    case 41:    //food proc cntr
        return 68;
    case 69:    //nova proc cntr
        return 323;
    case 42:    //research center
        return 209;
    case 49:    //fortress
        return 82;
    case 50:    //ore proc cntr
        return 584;
    case 51:    //carbon proc cntr
        return 562;
    case 72:    //animal nursery
        return 319;
    case 73:    //power core
        return 12;
    case 28:    //command center
        return 109;
    case 30:    //spaceport
        return 84;
    default:
        return -1;
    }
}

int get_building_offset(TribeTechHelpScreen* tech_tree, int key, int alt_key, int ctrl_key, int shift_key)
{
    int id = get_building_id(key, alt_key, ctrl_key, shift_key);
    if (id == -1)
        return -1;
    
    int start_pos = -1;
    int header_pos = -1;
    int tt_pos = -1;
    for (int i = 0; i < tech_tree->number_bld_zones; i++)
    {
        if (tech_tree->bld_zones[i].bld_node.bld_id == id)
        {
            if (start_pos == -1)
                start_pos = tech_tree->bld_zones[i].bld_node.fixed_x_loc - 3;
            if (tech_tree->bld_zones[i].bld_node.use_type != 2)
            {
                header_pos = tech_tree->bld_zones[i].bld_node.fixed_x_loc;
                //break;
            }
        }
    }
    if (header_pos != -1)
    {
        tt_pos = header_pos - tech_tree->max_draw_x_size / 2 + tech_tree->bld_zone_width / 2;
    }
    if (start_pos != -1 && start_pos < tt_pos)
    {
        tt_pos = start_pos;
    }
    return tt_pos;
}

void update_tech_tree_view(TribeTechHelpScreen* tech_tree, int old_x, int old_y)
{
    if (old_x != tech_tree->scroll_x_offset || old_y != tech_tree->scroll_y_offset)
    {
        tech_tree->popup_timer_flag = 0;
        tech_tree->old_draw_node = NULL;
        tech_tree->highlight_flag = 0;
        tech_tree->highlight_node = NULL;
        tech_tree->old_mouse_x = 0;
        tech_tree->old_mouse_y = 0;
        if (tech_tree->popup_flag)
        {
            tech_tree->popUpHelp1->vfptr->set_active(tech_tree->popUpHelp1, 0);
            tech_tree->popup_flag = 0;
        }
        TribeTechHelpScreen__reposition_screen_components(tech_tree);
        tech_tree->vfptr->set_redraw(tech_tree, 1);
    }
}

bool __stdcall handle_tech_tree_key(int key, __int16 count, int alt_key, int ctrl_key, int shift_key, TribeTechHelpScreen* tech_tree)
{
    bool result = false;

    int old_x = tech_tree->scroll_x_offset;
    int old_y = tech_tree->scroll_y_offset;

    if (count >= 1)
    {
        int offset = get_building_offset(tech_tree, key, alt_key, ctrl_key, shift_key);
        if (offset != -1)
        {
            tech_tree->scroll_x_offset = -offset;
            tech_tree_verify_pos(tech_tree);
            update_tech_tree_view(tech_tree, old_x, old_y);
            result = true;
        }
    }

    if (count >= 1 && !alt_key && !ctrl_key && !shift_key)
    {
        switch (key)
        {
        case VK_HOME:
            tech_tree_set_left(tech_tree);
            update_tech_tree_view(tech_tree, old_x, old_y);
            result = true;
            break;
        case VK_END:
            tech_tree_set_right(tech_tree);
            update_tech_tree_view(tech_tree, old_x, old_y);
            result = true;
            break;
        case VK_PRIOR:
            tech_tree_scroll_left(tech_tree);
            update_tech_tree_view(tech_tree, old_x, old_y);
            result = true;
            break;
        case VK_NEXT:
            tech_tree_scroll_right(tech_tree);
            update_tech_tree_view(tech_tree, old_x, old_y);
            result = true;
            break;
        default:
            break;
        }
    }
    return result;
}

__declspec(naked) void tech_tree_key_down() //004647E0
{
    __asm
    {
        push    ebx
        push    ecx
        push    ecx
        mov     eax, [esp + 20h] //shift
        mov     ecx, [esp + 1Ch] //ctrl
        mov     edx, [esp + 18h] //alt
        push    eax
        push    ecx
        push    edx
        mov     eax, [esp + 20h] //count
        mov     ecx, [esp + 1Ch] //key
        push    eax
        push    ecx
        call    handle_tech_tree_key
        pop     ecx
        test    al, al
        jnz     tech_tree_key_handled

        mov     eax, [esp + 18h]
        mov     edx, [esp + 14h]
        mov     ebx, 004647E9h
        jmp     ebx

tech_tree_key_handled:
        mov     ebx, 004648C0h
        jmp     ebx
    }
}

int __stdcall get_techtree_y_offset(int window_height)
{
    int techtree_height;
    if (window_height >= 1024)
        techtree_height = 1024;
    else if (window_height >= 768)
        techtree_height = 768;
    else
        techtree_height = 600;
    return (window_height - techtree_height) / 2;
}

__declspec(naked) void set_vmiddle_start() //0046C7E8
{
    __asm
    {
        mov     eax, [esi + 0DC0h]
        push    eax
        call    get_techtree_y_offset
        mov     [esi + 0EE8h], eax
        mov     ecx, esi
        mov     edx, 0046C7EEh
        jmp     edx
    }
}

#pragma optimize( "s", on )
void setTechTreeUIHooks()
{
    setHook((void*)0x004647E0, tech_tree_key_down);
    setHook((void*)0x0046C7E8, set_vmiddle_start);
}
#pragma optimize( "", on )
