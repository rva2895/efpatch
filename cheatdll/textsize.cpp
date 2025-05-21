#include "stdafx.h"
#include "textsize.h"

int* const obj_panel_obj_name_offset_x = (int* const)0x0078F444;
int* const obj_panel_obj_name_offset_y = (int* const)0x0078F450;

int* const obj_panel_icon_offset_x = (int* const)0x0078F45C;
int* const obj_panel_icon_offset_y = (int* const)0x0078F468;

int* const obj_panel_hp_bar_offset_x = (int* const)0x0078F474;
int* const obj_panel_hp_bar_offset_y = (int* const)0x0078F480;

int* const obj_panel_hp_text_offset_x = (int* const)0x0078F48C;
int* const obj_panel_hp_text_offset_y = (int* const)0x0078F498;

int* const obj_panel_sp_bar_offset_x = (int* const)0x0078F4A4;
int* const obj_panel_sp_bar_offset_y = (int* const)0x0078F4B0;

int* const obj_panel_sp_text_offset_x = (int* const)0x0078F4BC;
int* const obj_panel_sp_text_offset_y = (int* const)0x0078F4C8;

int* const obj_panel_player_name_offset_x = (int* const)0x0078F4D4;
int* const obj_panel_player_name_offset_y = (int* const)0x0078F4E0;

int* const obj_panel_civ_offset_x = (int* const)0x0078F4EC;
int* const obj_panel_civ_offset_y = (int* const)0x0078F4F8;

int* const obj_panel_alliance_state_offset_x = (int* const)0x0078F504;
int* const obj_panel_alliance_state_offset_y = (int* const)0x0078F510;

int* const obj_panel_stat_offset_x = (int* const)0x0078F51C;
int* const obj_panel_stat_offset_y = (int* const)0x0078F528;

int* const obj_panel_research_icon_offset_x = (int* const)0x0078F534;
int* const obj_panel_research_icon_offset_y = (int* const)0x0078F540;

int* const obj_panel_research_progress_offset_x = (int* const)0x0078F564;
int* const obj_panel_research_progress_offset_y = (int* const)0x0078F570;

const int obj_panel_research_text_offset_y_new[] =
{
    0x29, 0x27, 0x26
};

extern int second_col_offset;

int housing_text_rect_width = 0x78;

int achievements_player_width = 0x78;
int achievements_player_offset = 0x14;

__declspec(naked) void on_creating_housing_text() //005DC954
{
    __asm
    {
        add     eax, housing_text_rect_width
        lea     ecx, [ecx + edx + 1]

        mov     edx, 005DC95Bh
        jmp     edx
    }
}

__declspec(naked) void on_create_achievements_players() // 004EBB39
{
    __asm
    {
        lea     edx, [edi - 0Dh]
        push    achievements_player_width
        push    edx
        push    achievements_player_offset

        mov     ecx, 004EBB41h
        jmp     ecx
    }
}

int __fastcall adjust_font_size(int fontSize)
{
    if (fontSize >= 19 && fontSize < 20)
        fontSize *= 1.0;
    else if (fontSize >= 13 && fontSize < 19)
        fontSize *= 1.125;
    else if (fontSize >= 12 && fontSize < 13)
        fontSize *= 1.25;
    else if (fontSize < 12)
        fontSize *= 1.25;
    return fontSize;
}

__declspec(naked) void on_create_font() //00427EE6
{
    __asm
    {
        mov     ecx, [esp + 154h]
        call    adjust_font_size
        mov     ecx, eax

        mov     eax, 00427EEDh
        jmp     eax
    }
}

const int text_size_shift_offset = 15;
const int text_size_shift_offset_small = 10;

#pragma optimize( "s", on )
void setTextSizeHooks(bool text_rendering)
{
    writeByte(0x005D85C5, 3);   //resource y offset

    obj_panel_obj_name_offset_y[2]--;

    obj_panel_research_icon_offset_x[2] += text_size_shift_offset;
    obj_panel_research_progress_offset_x[2] += text_size_shift_offset;

    obj_panel_sp_bar_offset_y[2] += 1;
    obj_panel_sp_text_offset_y[2] += 1;

    obj_panel_stat_offset_y[2] += 2;

    obj_panel_player_name_offset_x[2] += text_size_shift_offset;
    obj_panel_civ_offset_x[2] += text_size_shift_offset;
    obj_panel_alliance_state_offset_x[2] += text_size_shift_offset;

    second_col_offset += text_size_shift_offset;

    //

    obj_panel_research_icon_offset_x[1] += text_size_shift_offset_small;
    obj_panel_research_progress_offset_x[1] += text_size_shift_offset_small;

    obj_panel_hp_text_offset_x[1] -= 1;
    obj_panel_sp_text_offset_x[1] -= 1;

    //obj_panel_sp_bar_offset_y[2] += 1;
    //obj_panel_sp_text_offset_y[2] += 1;

    //obj_panel_stat_offset_y[2] += 2;

    obj_panel_player_name_offset_x[1] += text_size_shift_offset_small;
    obj_panel_civ_offset_x[1] += text_size_shift_offset_small;
    obj_panel_alliance_state_offset_x[1] += text_size_shift_offset_small;

    //writeDword(0x006680C8, 0x7);   //obj_panel_research_text_offset_y
    //writeDword(0x005DC02E, (DWORD)obj_panel_research_text_offset_y_new);
    writeDword(0x005DC22A, (DWORD)obj_panel_research_text_offset_y_new);
    writeDword(0x005DC2B9, (DWORD)obj_panel_research_text_offset_y_new);
    writeDword(0x005DC941, (DWORD)obj_panel_research_text_offset_y_new);
    writeDword(0x005DCAA5, (DWORD)obj_panel_research_text_offset_y_new);
    writeDword(0x005DCB34, (DWORD)obj_panel_research_text_offset_y_new);

    housing_text_rect_width += 10;
    achievements_player_offset -= 4;
    achievements_player_width += 12;

    setHook((void*)0x005DC954, on_creating_housing_text);
    setHook((void*)0x004EBB39, on_create_achievements_players);

    //replay controls
    writeByte(0x00502894, 32);
    writeByte(0x005028A4, 90);
    writeByte(0x005028B8, 90);
    writeByte(0x005028CC, 90);

    if (text_rendering)
    {
        //achievements
        writeDword(0x004EB7BE, 544);
    }
    else
    {
        setHook((void*)0x00427EE6, on_create_font);
    }

    //achievements
    writeByte(0x004EBB40, 16);
    writeByte(0x004EB7BA, 42);
    writeByte(0x004EB7BC, 97);
    writeDword(0x004EB7A3, 85);
}
#pragma optimize( "", on )
