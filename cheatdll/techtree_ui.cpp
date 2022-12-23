#include "stdafx.h"
#include "techtree_ui.h"

int get_tech_tree_width(void* tech_tree)
{
    return *((int*)tech_tree + 0x375);
}

int get_tech_tree_height(void* tech_tree)
{
    return *((int*)tech_tree + 0x376);
}

int get_window_width(void* tech_tree)
{
    return *((int*)tech_tree + 0x36F);
}

int get_window_height(void* tech_tree)
{
    return *((int*)tech_tree + 0x370);
}

int* get_tech_tree_pos_x_ptr(void* tech_tree)
{
    return (int*)tech_tree + 0x3B9;
}

int* get_tech_tree_pos_y_ptr(void* tech_tree)
{
    return (int*)tech_tree + 0x3BA;
}

void tech_tree_set_left(void* tech_tree)
{
    int* x_pos_ptr = get_tech_tree_pos_x_ptr(tech_tree);
    *x_pos_ptr = 0;
}

void tech_tree_set_right(void* tech_tree)
{
    int* x_pos_ptr = get_tech_tree_pos_x_ptr(tech_tree);
    *x_pos_ptr = -(get_tech_tree_width(tech_tree) - get_window_width(tech_tree));
}

void tech_tree_verify_pos(void* tech_tree)
{
    int* x_pos_ptr = get_tech_tree_pos_x_ptr(tech_tree);
    if (*x_pos_ptr > 0)
        *x_pos_ptr = 0;
    int min_x_pos = -(get_tech_tree_width(tech_tree) - get_window_width(tech_tree));
    if (*x_pos_ptr < min_x_pos)
        *x_pos_ptr = min_x_pos;
}

void tech_tree_scroll_right(void* tech_tree)
{
    int* x_pos_ptr = get_tech_tree_pos_x_ptr(tech_tree);
    *x_pos_ptr -= get_window_width(tech_tree) - 48;
    tech_tree_verify_pos(tech_tree);
}

void tech_tree_scroll_left(void* tech_tree)
{
    int* x_pos_ptr = get_tech_tree_pos_x_ptr(tech_tree);
    *x_pos_ptr += get_window_width(tech_tree) - 48;
    tech_tree_verify_pos(tech_tree);
}

bool __stdcall handle_tech_tree_key(int key, __int16 count, int alt_key, int ctrl_key, int shift_key, TribeTechHelpScreen* tech_tree)
{
    bool result = false;

    if (count >= 1 && !alt_key && !ctrl_key && !shift_key)
    {
        switch (key)
        {
        case VK_HOME:
            tech_tree_set_left(tech_tree);
            TribeTechHelpScreen__reposition_screen_components(tech_tree);
            ((TPanel*)tech_tree)->vfptr->set_redraw((TPanel*)tech_tree, 1);
            result = true;
            break;
        case VK_END:
            tech_tree_set_right(tech_tree);
            TribeTechHelpScreen__reposition_screen_components(tech_tree);
            ((TPanel*)tech_tree)->vfptr->set_redraw((TPanel*)tech_tree, 1);
            result = true;
            break;
        case VK_PRIOR:
            tech_tree_scroll_left(tech_tree);
            TribeTechHelpScreen__reposition_screen_components(tech_tree);
            ((TPanel*)tech_tree)->vfptr->set_redraw((TPanel*)tech_tree, 1);
            result = true;
            break;
        case VK_NEXT:
            tech_tree_scroll_right(tech_tree);
            TribeTechHelpScreen__reposition_screen_components(tech_tree);
            ((TPanel*)tech_tree)->vfptr->set_redraw((TPanel*)tech_tree, 1);
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
