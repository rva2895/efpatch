#include "stdafx.h"
#include "wallbuild.h"
#include "registry.h"

extern CONFIG_DATA cd;

int sign(int a)
{
    if (a)
        return a > 0 ? 1 : -1;
    else
        return 0;
}

int draw_wall_get_facet(int delta_col, int delta_row)
{
    if (delta_col == 0 && delta_row == 0)
        return 2;
    if (delta_col == 0)
        return 1;
    if (delta_row == 0)
        return 0;
    if (delta_col * delta_row > 0)
        return 3;
    else
        return 4;
}

void draw_wall_outline_from_to(
    TRIBE_Main_View* main_view,
    TRIBE_Master_Building_Object* wall_master_object,
    int col1,
    int row1,
    int col2,
    int row2)
{
    int delta_col = sign(col2 - col1);
    int delta_row = sign(row2 - row1);

    int cur_col = col1;
    int cur_row = row1;

    TRIBE_Main_View__draw_wall_outline(main_view, cur_col, cur_row, wall_master_object, 2);

    bool reset_facet_available = true;
    bool set_reset_facet = false;

    do
    {
        cur_col += delta_col;
        if (cur_col == col2)
        {
            if (reset_facet_available && delta_col)
                set_reset_facet = true;
            delta_col = 0;
        }
        cur_row += delta_row;
        if (cur_row == row2)
        {
            if (reset_facet_available && delta_row)
                set_reset_facet = true;
            delta_row = 0;
        }
        int facet;
        if (set_reset_facet)
        {
            facet = 2;
            set_reset_facet = false;
            reset_facet_available = false;
        }
        else
            facet = draw_wall_get_facet(delta_col, delta_row);

        TRIBE_Main_View__draw_wall_outline(main_view, cur_col, cur_row, wall_master_object, facet);
    } while (delta_col || delta_row);
}

void wall_build_draw_wall_outlines(
    TRIBE_Main_View* main_view,
    TRIBE_Master_Building_Object* wall_master_object,
    int col1,
    int row1,
    int col2,
    int row2)
{
    int col = abs(col2 - col1);
    int row = abs(row2 - row1);

    int large_size = max(col, row);
    int small_size = min(col, row);

    if (large_size > small_size * 2) //line + diag pattern
        draw_wall_outline_from_to(main_view, wall_master_object, col2, row2, col1, row1);
    else //diag + line pattern
        draw_wall_outline_from_to(main_view, wall_master_object, col1, row1, col2, row2);
}

void __fastcall TRIBE_Main_View__draw_multi_object_outline_new(TRIBE_Main_View* main_view)
{
    if ((*base_game)->master_obj_id == -1)
        return;
    TRIBE_Master_Building_Object* wall_master_object = (TRIBE_Master_Building_Object*)main_view->player->master_objects[(*base_game)->master_obj_id];
    if (!wall_master_object)
        return;
    TDrawArea__SetClipRect(main_view->render_area, &main_view->clip_rect);

    int sel_col1_in;
    int sel_row1_in;
    int sel_col2_in;
    int sel_row2_in;

    if (!RGE_View__get_selection_area((RGE_View*)main_view, &sel_col1_in, &sel_row1_in, &sel_col2_in, &sel_row2_in, 0))
    {
        POINT pt;
        RGE_Pick_Info pick_info;
        RGE_Base_Game__get_mouse_pos(*base_game, &pt);
        if (!main_view->vfptr->is_inside(main_view, pt.x, pt.y)
            || RGE_Main_View__pick1(
                (RGE_Main_View*)main_view,
                0x28u,
                0,
                pt.x - main_view->render_rect.left,
                pt.y - main_view->render_rect.top,
                &pick_info,
                0,
                1) != 51)
        {
            return;
        }
        RGE_Main_View__fixup_pick_info((RGE_Main_View*)main_view, &pick_info);
        sel_col1_in = pick_info.x;
        sel_col2_in = pick_info.x;
        sel_row1_in = pick_info.y;
        sel_row2_in = pick_info.y;
    }

    if (sel_col1_in == sel_col2_in && sel_row1_in == sel_row2_in)
        TRIBE_Main_View__draw_wall_outline(main_view, sel_col1_in, sel_row1_in, wall_master_object, 2);
    else
        wall_build_draw_wall_outlines(main_view, wall_master_object, sel_col1_in, sel_row1_in, sel_col2_in, sel_row2_in);

    TDrawArea__SetClipRect(main_view->render_area, NULL);
}

void make_wall_lines(
    TRIBE_Command* command,
    TRIBE_Player* player,
    TRIBE_Master_Building_Object* master_obj,
    RGE_Static_Object** wall_list,
    int* num_walls,
    int col1,
    int row1,
    int col2,
    int row2,
    int unique_id,
    bool reverse_order)
{
    //diag from col1, row1
    int sq_side = min(abs(col2 - col1), abs(row2 - row1));

    int delta_col = sign(col2 - col1);
    int delta_row = sign(row2 - row1);

    int vertex_col = col1 + delta_col * sq_side;
    int vertex_row = row1 + delta_row * sq_side;

    if (reverse_order)
    {
        std::swap(col2, col1);
        std::swap(row2, row1);
    }

    TRIBE_Command__place_line_of_walls(
        command,
        player,
        master_obj,
        unique_id,
        col1,
        row1,
        vertex_col,
        vertex_row,
        wall_list,
        num_walls,
        0,
        0,
        0
    );

    TRIBE_Command__place_line_of_walls(
        command,
        player,
        master_obj,
        unique_id,
        vertex_col,
        vertex_row,
        col2,
        row2,
        wall_list,
        num_walls,
        0,
        0,
        0
    );
}

void make_wall_lines_editor(
    TRIBE_Main_View* main_view,
    TRIBE_Player* player,
    TRIBE_Master_Building_Object* master_obj,
    int col1,
    int row1,
    int col2,
    int row2,
    bool reverse_order)
{
    //diag from col1, row1
    int sq_side = min(abs(col2 - col1), abs(row2 - row1));

    int delta_col = sign(col2 - col1);
    int delta_row = sign(row2 - row1);

    int vertex_col = col1 + delta_col * sq_side;
    int vertex_row = row1 + delta_row * sq_side;

    if (reverse_order)
    {
        std::swap(col2, col1);
        std::swap(row2, row1);
    }

    TRIBE_Main_View__place_line_of_walls(
        main_view,
        player,
        master_obj,
        col1,
        row1,
        vertex_col,
        vertex_row
    );

    TRIBE_Main_View__place_line_of_walls(
        main_view,
        player,
        master_obj,
        vertex_col,
        vertex_row,
        col2,
        row2
    );
}

RGE_Static_Object** __stdcall wall_build_do_wall_placement_command(
    TRIBE_Command* command,
    TRIBE_Player* player,
    TRIBE_Master_Building_Object* master_obj,
    TRIBE_Command_Build_Wall* order,
    int* numWalls)
{
    int col1 = order->x1;
    int row1 = order->y1;
    int col2 = order->x2;
    int row2 = order->y2;

    if (cd.largeMaps)
    {
        col1 |= (*((unsigned char*)order + 0xA) & 0xF) << 8;
        row1 |= (*((unsigned char*)order + 0xA) & 0xF0) << 4;
        col2 |= (*((unsigned char*)order + 0xB) & 0xF) << 8;
        row2 |= (*((unsigned char*)order + 0xB) & 0xF0) << 4;
    }

    int max_num_walls = abs(col2 - col1) + 1 + abs(row2 - row1) + 1;
    RGE_Static_Object** wall_list = (RGE_Static_Object**) operator_new_internal(sizeof(RGE_Static_Object*) * max_num_walls);
    *numWalls = 0;

    int col = abs(col2 - col1);
    int row = abs(row2 - row1);

    int large_size = max(col, row);
    int small_size = min(col, row);

    if (large_size > small_size * 2) //line + diag pattern
        make_wall_lines(command, player, master_obj, wall_list, numWalls, col2, row2, col1, row1, order->unique_id, true);
    else //diag + line pattern
        make_wall_lines(command, player, master_obj, wall_list, numWalls, col1, row1, col2, row2, order->unique_id, false);

    return wall_list;
}

__declspec(naked) void wall_build_on_do_command() //005BA9F6
{
    __asm
    {
        mov     al, [ebx + 21h]
        cmp     al, 0Fh
        jl      wall_build_do_old_command

        mov     eax, [esp + 10h] //TRIBE_Command
        lea     ecx, [esp + 2Ch] //num_walls
        push    ecx
        push    esi
        push    ebp
        push    ebx
        push    eax
        call    wall_build_do_wall_placement_command

        mov     edi, eax

        mov     eax, 005BAB51h
        jmp     eax

wall_build_do_old_command:
        xor     eax, eax
        xor     ecx, ecx
        mov     al, [esi + 6]
        mov     edi, 005BA9FDh
        jmp     edi
    }
}

int* const allow_user_commands = (int* const)0x00690D40;

int __fastcall TRIBE_Main_View__command_place_multi_object_new(
    TRIBE_Main_View* main_view,
    DWORD dummy,
    int x1,
    int y1,
    int x2,
    int y2,
    int reset_mode)
{
    UNREFERENCED_PARAMETER(dummy);

    int retval = 0;
    if (!allow_user_commands)
        return 0;
    short master_obj_id = (*base_game)->master_obj_id;
    if (master_obj_id != -1)
    {
        TRIBE_Player* player = (TRIBE_Player*)main_view->player;
        if ((*base_game)->prog_mode != 7)
        {
            if (player->master_objects[master_obj_id])
            {
                if (TRIBE_Player__command_make_wall(player, master_obj_id, x1, y1, x2, y2))
                    retval = 1;
            }
            if (reset_mode)
                (*base_game)->vfptr->set_game_mode((*base_game), 0, 0);
        }
        else
        {
            TRIBE_Master_Building_Object* master_obj = (TRIBE_Master_Building_Object*)main_view->player->master_objects[master_obj_id];
            if (master_obj)
            {
                int col = abs(x2 - x1);
                int row = abs(y2 - y1);

                int large_size = max(col, row);
                int small_size = min(col, row);

                if (large_size > small_size * 2)
                    make_wall_lines_editor(main_view, player, master_obj, x2, y2, x1, y1, true);
                else
                    make_wall_lines_editor(main_view, player, master_obj, x1, y1, x2, y2, false);
                retval = 1;
            }
        }
    }
    RGE_View__set_selection_area((RGE_View*)main_view, -1, -1, -1, -1);
    main_view->vfptr->set_redraw(main_view, 1);
    return retval;
}

#pragma optimize( "s", on )
void setWallBuildHooks()
{
    setHook((void*)0x005FC180, TRIBE_Main_View__draw_multi_object_outline_new);
    setHook((void*)0x005BA9F6, wall_build_on_do_command);
    setHook((void*)0x005FCB20, TRIBE_Main_View__command_place_multi_object_new);
}
#pragma optimize( "", on )
