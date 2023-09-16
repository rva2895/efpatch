#include "stdafx.h"
#include "land.h"

unsigned __int8 __fastcall RGE_RMM_Land_Generator__base_land_generate_new(RGE_RMM_Land_Generator* this_)
{
    //int* v2; // ebx
    int v3; // edi
    int v4; // ebp
    int v5; // eax
    int v6; // ecx
    float v7; // ebx
    int v8; // edi
    int v9; // ebp
    int v10; // eax
    int v11; // eax
    int v12; // eax
    int v13; // ebx
    int v14; // ebp
    //BOOL v15; // cc
    int v16; // eax
    //int v17; // ebp
    Map_Stack* v18; // ebx
    //int* v19; // edi
    int v20; // ebp
    int v21; // eax
    int v22; // ecx
    //int v23; // eax
    int v24; // eax
    //Map_Stack* v25; // ebp
    //unsigned __int8* v26; // edi
    unsigned __int8 v27; // bl
    unsigned __int8* v28; // edi
    unsigned __int8** v29; // edi
    //int v30; // edi
    //int* v31; // ebx
    float tot_cost; // [esp+0h] [ebp-B20h]
    float tot_costa; // [esp+0h] [ebp-B20h]
    float tot_costb; // [esp+0h] [ebp-B20h]
    float tot_costc; // [esp+0h] [ebp-B20h]
    int y; // [esp+14h] [ebp-B0Ch] BYREF
    int x; // [esp+18h] [ebp-B08h] BYREF
    //unsigned __int8 v39; // [esp+1Ch] [ebp-B04h]
    //unsigned __int8* v39_2;
    //int v40; // [esp+20h] [ebp-B00h]
    //Map_Stack* stack; // [esp+24h] [ebp-AFCh]
    //unsigned __int8 stack_2;
    float v42; // [esp+28h] [ebp-AF8h]
    //unsigned __int8* i; // [esp+2Ch] [ebp-AF4h]
    char v44; // [esp+33h] [ebp-AEDh]
    int v45; // [esp+34h] [ebp-AECh]
    int v46; // [esp+38h] [ebp-AE8h]
    //Map_Stack* v47; // [esp+3Ch] [ebp-AE4h]
    int v48; // [esp+40h] [ebp-AE0h]
    float cost; // [esp+44h] [ebp-ADCh] BYREF
    int v50; // [esp+48h] [ebp-AD8h]
    int v51[99]; // [esp+4Ch] [ebp-AD4h] BYREF
    //int v52[594]; // [esp+1D8h] [ebp-948h] BYREF

    Map_Stack stack_1[99];
    //Map_Stack stack_2[99];

    //v40 = 0;
    v50 = this_->map_width - 1;
    v48 = this_->map_height - 1;
    if (this_->info.land_num > 0)
    {
        //v2 = &this_->info.land[0].base_size;
        //stack = (Map_Stack*)v52;
        //v47 = (Map_Stack*)v51;
        for (int i = 0; i < this_->info.land_num; i++)
        {
            RGE_Random_Map_Module__init_stack((RGE_Random_Map_Module*)this_, &stack_1[i]);
            v3 = this_->info.land[i].x;
            v4 = this_->info.land[i].y;
            v51[i] = 0;
            v5 = this_->info.land[i].base_size;
            v6 = v3 - v5;
            v7 = v4 - v5;
            v8 = v5 + v3;
            v9 = v5 + v4;
            v45 = v6;
            if (v6 < 0)
            {
                v45 = 0;
                v6 = 0;
            }
            if (v7 < 0.0)
                v7 = 0.0;
            v10 = this_->map_width;
            if (v8 >= v10)
                v8 = v10 - 1;
            v11 = this_->map_height;
            if (v9 >= v11)
                v9 = v11 - 1;
            RGE_Map__set_terrain(this_->map, 0, 0, v6, v7, v8, v9, this_->info.land[i].terrain_type, 1u, 0);
            //RGE_Map__set_elev(this_->map, v6, v7, v8, v9, this_->info.land[i].elev, 0, 1);
            //RGE_Map__set_elev(this_->map, 10, 10, 20, 20, 6, 0, 1);
            v12 = v45;
            v42 = v7;
            //v39 = *((BYTE*)i + 8);
            for (v51[this_->info.land[i].zone] = (v8 - v45 + 1) * (v9 - v7 + 1); v42 <= v9; ++v42)
            {
                if (v12 <= v8)
                {
                    do
                        this_->search_map_rows[(int)v42][v12++] = this_->info.land[i].zone;
                    while (v12 <= v8);
                    v12 = v45;
                }
            }
            if (v12 > 0)
            {
                v46 = v7;
                if (v7 <= v9)
                {
                    v42 = (float)i;
                    do
                    {
                        RGE_Random_Map_Module__push_stack((RGE_Random_Map_Module*)this_, &stack_1[i], v12 - 1, v46++, v42, 0.0);
                        v12 = v45;
                    } while (v46 <= v9);
                }
            }
            if (v7 > 0)
            {
                v46 = v12;
                if (v12 <= v8)
                {
                    v42 = (float)i;
                    do
                        RGE_Random_Map_Module__push_stack((RGE_Random_Map_Module*)this_, &stack_1[i], v46++, v7 - 1, v42, 0.0);
                    while (v46 <= v8);
                    v12 = v45;
                }
            }
            if (v8 < this_->map_width - 1 && v7 <= v9)
            {
                v42 = (float)i;
                do
                {
                    RGE_Random_Map_Module__push_stack((RGE_Random_Map_Module*)this_, &stack_1[i], v8 + 1, v7, v42, 0.0);
                    ++v7;
                } while (v7 <= v9);
                v12 = v45;
            }
            if (v9 < this_->map_height - 1)
            {
                v13 = v12;
                if (v12 <= v8)
                {
                    v14 = v9 + 1;
                    v42 = (float)i;
                    do
                        RGE_Random_Map_Module__push_stack((RGE_Random_Map_Module*)this_, &stack_1[i], v13++, v14, v42, 0.0);
                    while (v13 <= v8);
                }
            }
        }
    }
    do
    {
        v16 = this_->info.land_num;
        //v17 = 0;
        v44 = 1;
        //v40 = 0;
        if (v16 <= 0)
            break;
        //v18 = &stack_1[0];
        //v19 = &this_->info.land[0].clumpiness_factor;
        for (int i = 0; i < this_->info.land_num; i++)
        {
            if (v51[i] < this_->info.land[i].land_size
                && RGE_Random_Map_Module__pop_stack((RGE_Random_Map_Module*)this_, &stack_1[i], &x, &y, &cost))
            {
                v44 = 0;
                unsigned __int8 chance = RGE_RMM_Land_Generator__chance(this_, x, y, i);
                if (chance > 100 * rand_internal() / 0x7FFF)
                {
                    this_->search_map_rows[y][x] = -1;
                }
                else
                {
                    RGE_Tile* v39_2 = &this_->map_row_offset[y][x];
                    //stack_2 = *((BYTE*)v19 - 24);
                    v20 = RGE_RMM_Land_Generator__check_terrain_and_zone(this_, this_->info.land[i].terrain_type, i, x, y);
                    if (this_->search_map_rows[y][x] == 0xFE && v20 > 0)
                    {
                        v39_2->terrain_type = this_->info.land[i].terrain_type;
                        //v39_2->height = this_->info.land[i].elev;
                        this_->search_map_rows[y][x] = this_->info.land[i].zone;
                        v21 = x;
                        if (x <= 0)
                        {
                            v22 = y;
                        }
                        else
                        {
                            v22 = y;
                            if (this_->search_map_rows[y][x - 1] == 0xFE)
                            {
                                tot_cost = (100 * rand_internal() / 0x7FFF - v20 * this_->info.land[i].clumpiness_factor + 250);
                                RGE_Random_Map_Module__push_stack(
                                    (RGE_Random_Map_Module*)this_,
                                    &stack_1[i],
                                    x - 1,
                                    y,
                                    0.0,
                                    tot_cost);
                                v22 = y;
                                v21 = x;
                            }
                        }
                        if (v21 < v50 && this_->search_map_rows[v22][v21 + 1] == 0xFE)
                        {
                            tot_costa = (100 * rand_internal() / 0x7FFF - v20 * this_->info.land[i].clumpiness_factor + 250);
                            RGE_Random_Map_Module__push_stack(
                                (RGE_Random_Map_Module*)this_,
                                &stack_1[i],
                                x + 1,
                                y,
                                0.0,
                                tot_costa);
                            v22 = y;
                            v21 = x;
                        }
                        if (v22 > 0 && this_->search_map_rows[v22 - 1][v21] == 0xFE)
                        {
                            tot_costb = (100 * rand_internal() / 0x7FFF - v20 * this_->info.land[i].clumpiness_factor + 250);
                            RGE_Random_Map_Module__push_stack(
                                (RGE_Random_Map_Module*)this_,
                                &stack_1[i],
                                x,
                                y - 1,
                                0.0,
                                tot_costb);
                            v22 = y;
                            v21 = x;
                        }
                        if (v22 < v48 && this_->search_map_rows[v22 + 1][v21] == 0xFE)
                        {
                            tot_costc = (100 * rand_internal() / 0x7FFF - v20 * this_->info.land[i].clumpiness_factor + 250);
                            RGE_Random_Map_Module__push_stack(
                                (RGE_Random_Map_Module*)this_,
                                &stack_1[i],
                                x,
                                y + 1,
                                0.0,
                                tot_costc);
                        }
                        ++v51[i];
                    }
                    //v17 = v40;
                }
            }
            //v23 = this_->info.land_num;
            //++v17;
            //v19 += 13;
            //v18 += 1;
            //v40 = v17;
            //++i;
        }
    } while (!v44);
    v24 = this_->info.land_num;
    //v40 = 0;
    if (v24 > 0)
    {
        for (int i = 0; i < this_->info.land_num; i++)
        {
            v27 = this_->info.land[i].zone;
            if (RGE_Random_Map_Module__pop_stack((RGE_Random_Map_Module*)this_, &stack_1[i], &x, &y, &cost))
            {
                do
                {
                    if (x >= 1 && (v28 = &this_->search_map_rows[y][x], *(v28 - 1) == v27) && x < v50 && v28[1] == v27
                        || y >= 1 && (v29 = this_->search_map_rows, v29[y - 1][x] == v27) && y < v48 && v29[y + 1][x] == v27)
                    {
                        this_->map_row_offset[y][x].terrain_type = this_->info.land[i].terrain_type;
                        //this_->map_row_offset[y][x].height = this_->info.land[i].elev;
                    }
                } while (RGE_Random_Map_Module__pop_stack((RGE_Random_Map_Module*)this_, &stack_1[i], &x, &y, &cost));
            }
        }
    }
    for (int i = 0; i < this_->info.land_num; i++)
        RGE_Random_Map_Module__deinit_stack((RGE_Random_Map_Module*)this_, &stack_1[i]);

    //RGE_Map__clean_elevation(this_->map, 0, 0, this_->map_width - 1, this_->map_height - 1, 8u);

    return 1;
}

void setLandRMSHooks()
{
    setHook((void*)0x004DA6F0, RGE_RMM_Land_Generator__base_land_generate_new);
}
