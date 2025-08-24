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
    tech_tree->scroll_x_offset -= tech_tree->max_draw_x_size - 68;
    tech_tree_verify_pos(tech_tree);
}

void tech_tree_scroll_left(TribeTechHelpScreen* tech_tree)
{
    tech_tree->scroll_x_offset += tech_tree->max_draw_x_size - 68;
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

TScreenPanel* blank_background_panel = NULL;

const char bk_panel_name[] = "Blank Background Screen";

bool tech_tree_called_from_game = false;

TPanel* __stdcall tech_tree_resolution_open_action(TPanel* parent, bool from_game)
{
    tech_tree_called_from_game = from_game;
    if (!from_game)
        RGE_Base_Game__set_resolution_to_in_game_resolution(*base_game);

    TPanelSystem__destroyPanel(panel_system, bk_panel_name);

    blank_background_panel = (TScreenPanel*)calloc_internal(1, sizeof(TScreenPanel));
    TScreenPanel__TScreenPanel(blank_background_panel, bk_panel_name);
    TScreenPanel__setup(blank_background_panel, (*base_game)->draw_area, NULL, -1, 0);
    blank_background_panel->parent_panel = parent;

    TPanelSystem__setCurrentPanel(panel_system, bk_panel_name, 0);
    
    TPanel* p = TPanelSystem__getTop(panel_system);
    RGE_Base_Game__enable_input(*base_game);
    return p;
}

void __stdcall tech_tree_resolution_close_action()
{
    TPanel* p = TPanelSystem__getTop(panel_system);
    if (p && !strcmp(p->panelNameValue, bk_panel_name))
    {
        if (!tech_tree_called_from_game)
            RGE_Base_Game__set_resolution_to_out_of_game_resolution(*base_game);
        TPanelSystem__setCurrentPanel(panel_system, p->parent_panel->panelNameValue, 0);
    }
    TPanelSystem__destroyPanel(panel_system, bk_panel_name);
}

__declspec(naked) void tech_tree_resolution_open() //0051838C
{
    __asm
    {
        push    eax
        push    0
        push    esi
        call    tech_tree_resolution_open_action
        pop     ecx
        push    eax
        call    TribeTechHelpScreen__TribeTechHelpScreen
        mov     ecx, 00518398h
        jmp     ecx
    }
}

__declspec(naked) void tech_tree_resolution_open_2() //00506924
{
    __asm
    {
        push    1
        push    edi
        call    tech_tree_resolution_open_action
        mov     ecx, esi
        push    eax
        call    TribeTechHelpScreen__TribeTechHelpScreen
        mov     ecx, 0050692Ch
        jmp     ecx
    }
}

__declspec(naked) void tech_tree_resolution_close_1() //0046440D
{
    __asm
    {
        call    TPanelSystem__destroyPanel
        call    tech_tree_resolution_close_action
        mov     eax, 00464412h
        jmp     eax
    }
}

__declspec(naked) void tech_tree_resolution_close_2() //0046485F
{
    __asm
    {
        call    TPanelSystem__destroyPanel
        call    tech_tree_resolution_close_action
        mov     eax, 00464864h
        jmp     eax
    }
}

__declspec(naked) void tech_tree_resolution_close_3() //004648B9
{
    __asm
    {
        call    TPanelSystem__destroyPanel
        call    tech_tree_resolution_close_action
        mov     eax, 004648BEh
        jmp     eax
    }
}

__declspec(naked) void tech_tree_resolution_close_4() //0046DF43
{
    __asm
    {
        call    TPanelSystem__destroyPanel
        call    tech_tree_resolution_close_action
        mov     eax, 0046DF48h
        jmp     eax
    }
}

__declspec(naked) void tech_tree_resolution_close_5() //00507419
{
    __asm
    {
        call    TPanelSystem__destroyPanel
        call    tech_tree_resolution_close_action
        mov     eax, 0050741Eh
        jmp     eax
    }
}

__declspec(naked) void tech_tree_resolution_close_6() //00516F02
{
    __asm
    {
        call    TPanelSystem__destroyPanel
        call    tech_tree_resolution_close_action
        mov     eax, 00516F07h
        jmp     eax
    }
}

__declspec(naked) void tech_tree_resolution_close_7() //005E4A27
{
    __asm
    {
        call    TPanelSystem__destroyPanel
        call    tech_tree_resolution_close_action
        mov     eax, 005E4A2Ch
        jmp     eax
    }
}

__declspec(naked) void tech_tree_resolution_close_8() //005ED758
{
    __asm
    {
        call    TPanelSystem__destroyPanel
        call    tech_tree_resolution_close_action
        mov     eax, 005ED75Dh
        jmp     eax
    }
}

bool __stdcall tech_tree_mouse_scroll_set_amount(TribeTechHelpScreen* tech_tree, int delta, short direction)
{
    if (tech_tree->scroll_horizontal_direction != 0)
        return false;
    else if (direction == -1)
    {
        tech_tree->actual_horizontal_scroll_amount = TribeTechHelpScreen__calculate_X_scroll(tech_tree, delta);
        tech_tree->scroll_horizontal_direction = tech_tree->actual_horizontal_scroll_amount != 0 ? 1 : 0;
        return true;
    }
    else if (direction == 1)
    {
        tech_tree->actual_horizontal_scroll_amount = TribeTechHelpScreen__calculate_X_scroll(tech_tree, -delta);
        tech_tree->scroll_horizontal_direction = tech_tree->actual_horizontal_scroll_amount != 0 ? 2 : 0;
        return true;
    }
    else
    {
        tech_tree->actual_horizontal_scroll_amount = 0;
        return false;
    }
}

__declspec(naked) void tech_tree_mouse_scroll() //0046BE3E
{
    __asm
    {
        push    ebp
        push    edx
        push    esi
        call    tech_tree_mouse_scroll_set_amount

        test    al, al
        jz      tech_tree_mouse_no_scroll

        mov     eax, 0046BF08h
        jmp     eax

tech_tree_mouse_no_scroll:
        mov     eax, 0046BF0Ch
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setTechTreeUIHooks()
{
    setHook((void*)0x004647E0, tech_tree_key_down);
    setHook((void*)0x0046C7E8, set_vmiddle_start);
    setHook((void*)0x0051838C, tech_tree_resolution_open);
    setHook((void*)0x00506924, tech_tree_resolution_open_2);
    setHook((void*)0x0046440D, tech_tree_resolution_close_1);
    setHook((void*)0x0046485F, tech_tree_resolution_close_2);
    setHook((void*)0x004648B9, tech_tree_resolution_close_3);
    setHook((void*)0x0046DF43, tech_tree_resolution_close_4);
    setHook((void*)0x00507419, tech_tree_resolution_close_5);
    setHook((void*)0x00516F02, tech_tree_resolution_close_6);
    setHook((void*)0x005E4A27, tech_tree_resolution_close_7);
    setHook((void*)0x005ED758, tech_tree_resolution_close_8);
    //setHook((void*)0x0046BE3E, tech_tree_mouse_scroll);
}
#pragma optimize( "", on )
