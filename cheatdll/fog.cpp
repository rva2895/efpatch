#include "stdafx.h"
#include "fog.h"

/*
int* Global_Draw_Flags = (int*)0x0077FA60;

void (__cdecl* ASMSet_Shadowing)(unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4) =
    (void (__cdecl*)(unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4))0x00651780;

void __stdcall RGE_View__draw_terrain_shape2(RGE_View* this_, int x, int y, TShape* shape, int index, unsigned __int8 visible, unsigned __int8 draw_attribute, int draw_normal, int draw_fog)
{
    Shape_Info* v10; // eax
    int v11; // ebp
    int v12; // edx
    unsigned __int8 v13; // al
    int v14; // edi
    int v15; // ebp

    if (shape)
    {
        v10 = &shape->shape_info[index];
        v11 = v10->Hotspot_X;
        v12 = v10->Hotspot_Y;
        v13 = draw_attribute;
        v14 = x - v11;
        v15 = y - v12;
        if ((draw_attribute & 0x10) == 16)
        {
            *Global_Draw_Flags = 1;
            ASMSet_Shadowing(16711935, 671098880, -16711936, 2621480);
            TShape__shape_draw(shape, this_->cur_render_area, v14, v15, index, 0);
        }
        else
        {
            if ((draw_attribute & 0x20) == 32)
            {
                ASMSet_Shadowing(16711935, 1442862592, -16711936, 5636182);
                v13 = draw_attribute;
            }
            if ((v13 & 0x80) == 0x80)
            {
                ASMSet_Shadowing(16711935, 268439552 * (v13 & 0xF) + 67109888, -16711936, 268439552 * (v13 & 0xF) + 67109888);
                v13 = draw_attribute;
            }
            if (this_->map->fog_flag && visible == 0x80)
                *Global_Draw_Flags = 1;
            if (draw_fog)
            {
                this_->cur_render_area->CurSpanList = this_->Terrain_Fog_Clip_Mask;
                *Global_Draw_Flags = 1;
                ASMSet_Shadowing(0xFF00FF, 0x02000200, 0xFF00FF00, 0x020002);
                TShape__shape_draw(shape, this_->cur_render_area, x, y, index, 0);
                int ss = this_->world->color_table_num;
                ASMSet_Shadowing(0xFF00FF, 0, 0xFF00FF00, 0);
                v13 = draw_attribute;
                *Global_Draw_Flags = 0;
            }
            if ((v13 & 0xA0) != 0)
                *Global_Draw_Flags = 1;
            if (draw_normal)
            {
                this_->cur_render_area->CurSpanList = this_->Terrain_Clip_Mask;
                TShape__shape_draw(shape, this_->cur_render_area, x, y, index, 0);
            }
            this_->cur_render_area->CurSpanList = this_->cur_render_area->SpanList;
        }
        ASMSet_Shadowing(0xFF00FF, 0, 0xFF00FF00, 0);
        *Global_Draw_Flags = 0;
    }
}

__declspec(naked) void RGE_View__draw_terrain_shape2_wr()
{
    __asm
    {
        mov     eax, [esp]
        mov     [esp], ecx
        push    eax
        jmp     RGE_View__draw_terrain_shape2
    }
}
*/

#define MAKE_UINT32(a,b,c,d) (((d) << 24) | ((c) << 16) | ((b) << 8) | (a))

unsigned int* const shadowing = (unsigned int* const)0x007A4060;
unsigned int fog_param_a2;
unsigned int fog_param_a4;

void __cdecl ASMSet_Shadowing_new(int a1, int a2, int a3, int a4) //00651780
{
    if (a1 == 0xFF00FF
        && a2 == 0
        && a3 == 0xFF00FF00
        && a4 == 0)
    {
        a2 = fog_param_a2;
        a4 = fog_param_a4;
    }

    shadowing[0] = a1;
    shadowing[2] = a1;
    shadowing[4] = a2;
    shadowing[6] = a2;
    shadowing[1] = a3;
    shadowing[3] = a3;
    shadowing[5] = a4;
    shadowing[7] = a4;
}

#pragma optimize( "s", on )
void setFogOfWarHooks(int version)
{
    unsigned __int8 c;
    if (version == VER_EF)
        c = 2;
    else
        c = 129;

    fog_param_a2 = MAKE_UINT32(0, c, 0, c);
    fog_param_a4 = MAKE_UINT32(c, 0, c, 0);
    setHook((void*)0x00651780, ASMSet_Shadowing_new);
}
#pragma optimize( "", on )
