#include "stdafx.h"
#include "script.h"
#include "rms_tokens.h"

void __stdcall RGE_RMM_Script_Controller__setup_base_land_new(RGE_RMM_Script_Controller* this_, int land_index, int owner)
{
    this_->object_info.lands[land_index].player_id = owner;
    this_->object_info.lands[land_index].id = owner > 0;
    this_->object_info.land_num = land_index + 1;
    this_->land_info.land[land_index].wall_1_avoidance_line = 0;
    this_->land_info.land[land_index].wall_2_avoidance_line = 0;
    this_->land_info.land[land_index].wall_3_avoidance_line = this_->map_width;
    this_->land_info.land[land_index].wall_4_avoidance_line = this_->map_height;
    this_->land_info.land[land_index].clumpiness_factor = 8;
    this_->land_info.land[land_index].zone = owner;
    this_->land_info.land[land_index].area = 0;
    this_->land_info.land[land_index].wall_fade = 20;
    this_->land_info.land[land_index].base_size = 3;
    this_->land_info.land[land_index].x = -1;
    this_->land_info.land[land_index].y = -1;
    /*(_DWORD*)v4 + 63660)*/ this_->land_info.land[land_index].land_size = this_->map_width * this_->map_height;
    this_->min_place_distance[land_index] = -1;
}

unsigned __int8 __stdcall RGE_RMM_Script_Controller__do_land_command_new(RGE_RMM_Script_Controller* this_, RGE_RMM_Token* command)
{
    int v2; // edx
    int v4; // eax
    RGE_RMM_Script_ControllerVtbl* v5; // edx
    unsigned __int8 result; // al
    int v7; // edi
    RGE_Game_World* v8; // ecx
    int v9; // eax
    int v10; // ecx
    int v11; // eax
    int v12; // ecx
    int v13; // eax

    int v23; // ebp
    int v24; // ebx
    int v25; // edi
    RGE_Player* v26; // ecx
    int v27; // edi

    int v34; // eax
    int v35; // edx
    int v36; // eax
    int* v37; // eax

    if (!this_->inside_braces)
    {
        switch (command->value)
        {
        case 0x13:  //base_terrain
            result = 1;
            this_->land_info.base_terrain = this_->constants[0]->value;
            return result;
        case 0x14:  //create_player_lands
            v7 = this_->land_info.land_num;
            v8 = this_->world;
            this_->land_index1 = v7;
            v9 = v8->player_num + v7 - 2;
            for (this_->land_index2 = v9; v7 <= this_->land_index2; ++v7)
                this_->vfptr->setup_base_land(this_, v7, v7 - this_->land_index1 + 1);
            v10 = this_->land_info.land_num;
            v11 = this_->world->player_num - 1;
            this_->placing_player_land = 1;
            this_->placing_land = 0;
            this_->land_info.land_num = v11 + v10;
            return 1;
        case 0x20:  //create_land
            v4 = this_->land_info.land_num;
            v5 = this_->vfptr;
            this_->land_index1 = v4;
            this_->land_index2 = v4;
            this_->land_info.land_num = v4 + 1;
            v5->setup_base_land(this_, this_->land_index1, 0);
            this_->placing_player_land = 0;
            this_->placing_land = 1;
            return 1;
        default:
            return 1;
        }
    }
    v12 = this_->land_index1;
    if (v12 < 0 || (v13 = this_->land_index2, v13 < 0))
    {
        return 1;
    }
    v2 = 0;
    switch (command->value)
    {
    case 0x12:  //land_percent
        if (v12 > v13)
            return 1;
        do
        {
            this_->land_info.land[v12].land_size = this_->param_numbers[0]
                * this_->map_width
                * this_->map_height
                / (100
                    * (v13 - this_->land_index1 + 1));
            v13 = this_->land_index2;
            ++v12;
        } while (v12 <= v13);
        result = 1;
        break;
    case 0x15:  //terrain_type
        if (v12 > v13)
            return 1;
        do
        {
            this_->land_info.land[v12].terrain_type = this_->constants[0]->value;
            ++v12;
        } while (v12 <= this_->land_index2);
        result = 1;
        break;
    case 0x16:  //base_size
        if (v12 > v13)
            return 1;
        do
        {
            this_->land_info.land[v12].base_size = this_->param_numbers[0];
            ++v12;
        } while (v12 <= this_->land_index2);
        result = 1;
        break;
    case 0x17:  //left_border
        if (v12 > v13)
            return 1;
        do
        {
            this_->land_info.land[v12].wall_1_avoidance_line = this_->param_numbers[0] * this_->map_width / 100;
            ++v12;
        } while (v12 <= this_->land_index2);
        result = 1;
        break;
    case 0x18:  //right_border
        if (v12 > v13)
            return 1;
        do
        {
            this_->land_info.land[v12].wall_3_avoidance_line = this_->map_width - this_->param_numbers[0] * this_->map_width / 100;
            ++v12;
        } while (v12 <= this_->land_index2);
        result = 1;
        break;
    case 0x19:  //top_border
        if (v12 > v13)
            return 1;
        do
        {
            this_->land_info.land[v12].wall_2_avoidance_line = this_->param_numbers[0] * this_->map_height / 100;
            ++v12;
        } while (v12 <= this_->land_index2);
        result = 1;
        break;
    case 0x1A:  //bottom_border
        if (v12 > v13)
            return 1;
        do
        {
            this_->land_info.land[v12].wall_4_avoidance_line = this_->map_height - this_->param_numbers[0] * this_->map_height / 100;
            ++v12;
        } while (v12 <= this_->land_index2);
        result = 1;
        break;
    case 0x1B:  //border_fuzziness
        if (v12 > v13)
            return 1;
        do
        {
            this_->land_info.land[v12].wall_fade = this_->param_numbers[0];
            ++v12;
        } while (v12 <= this_->land_index2);
        result = 1;
        break;
    case 0x1C:  //zone
        if (v12 > v13)
            return 1;
        do
        {
            this_->land_info.land[v12].zone = LOBYTE(this_->param_numbers[0]) + 10;
            ++v12;
        } while (v12 <= this_->land_index2);
        result = 1;
        break;
    case 0x1D:  //set_zone_by_team
        v23 = this_->land_index1;
        if (v12 > v13)
            return 1;
        do
        {
            v24 = 1;
            v25 = v23 - this_->land_index1 + 1;
            if (v25 > 1)
            {
                do
                {
                    v26 = this_->world->players[v24];
                    if (v26->vfptr->isAlly(v26, v25))
                        break;
                    ++v24;
                } while (v24 < v25);
            }
            this_->land_info.land[v23].zone = v24 + 1;
            ++v23;
        } while (v23 <= this_->land_index2);
        result = 1;
        break;
    case 0x1E:  //set_zone_randomly
        v27 = this_->land_index1;
        if (v12 > v13)
            return 1;
        do
        {
            this_->land_info.land[v27].zone = (this_->world->player_num - 1) * rand_internal() / 0x7FFF + 2;
            ++v27;
        } while (v27 <= this_->land_index2);
        result = 1;
        break;
    case 0x1F:  //other_zone_avoidance_distance
        if (v12 > v13)
            return 1;
        do
        {
            this_->land_info.land[v12].area = this_->param_numbers[0];
            ++v12;
        } while (v12 <= this_->land_index2);
        result = 1;
        break;
    case 0x21:  //assign_to_player
        if (this_->placing_land)
        {
            v34 = this_->param_numbers[0];
            v35 = this_->world->player_num;
            if (v34 < v35)
            {
                this_->object_info.lands[v12].player_id = v34;
                v37 = &this_->object_info.lands[this_->land_index1].id;
                if (*v37)
                    return 1;
                *v37 = 1;
                result = 1;
            }
            else
            {
                if (this_->placing_land)
                {
                    v36 = this_->object_info.land_num - 1;
                    --this_->land_info.land_num;
                    this_->object_info.land_num = v36;
                }
                this_->placing_land = 0;
                result = 1;
            }
        }
        else
        {
            result = 1;
        }
        break;
    case 0x47:  //land_position
        if (!this_->placing_land)
            return 1;
        this_->land_info.land[v12].x = this_->map_width * this_->param_numbers[0] / 100;
        this_->land_info.land[this_->land_index1].y = this_->map_height * this_->param_numbers[1] / 100;
        result = 1;
        break;
    case 0x48:  //land_id
        if (v12 > v13)
            return 1;
        do
        {
            this_->object_info.lands[v12].id = this_->param_numbers[0] + 10;
            ++v12;
        } while (v12 <= this_->land_index2);
        result = 1;
        break;
    case 0x49:  //clumping_factor
        if (v12 > v13)
            return 1;
        do
        {
            this_->land_info.land[v12].clumpiness_factor = this_->param_numbers[0];
            ++v12;
        } while (v12 <= this_->land_index2);
        result = 1;
        break;
    case 0x4A:  //number_of_tiles
        if (v12 > v13)
            return 1;
        do
        {
            this_->land_info.land[v12].land_size = this_->param_numbers[0];
            ++v12;
        } while (v12 <= this_->land_index2);
        result = 1;
        break;
    case 0x56:  //min_placement_distance
        if (v12 > v13)
            return 1;
        do
        {
            this_->min_place_distance[v12] = this_->param_numbers[0];
            ++v12;
        } while (v12 <= this_->land_index2);
        result = 1;
        break;
    /*case RMS_LAND_BASE_ELEV:
        if (v12 > v13)
            return 1;
        do
        {
            this_->land_info.land[v12].elev = this_->param_numbers[0];
            ++v12;
        } while (v12 <= this_->land_index2);
        result = 1;
        break;*/
    default:
        result = 1;
        break;
    }
    return result;
}

unsigned __int8 __fastcall TRIBE_RMM_Script_Controller__do_player_positioning_new(TRIBE_RMM_Script_Controller* this_)
{
    RGE_RMM_Script_Controller* v1; // ebx
    int v2; // ebp
    int v3; // edi
    int v4; // esi
    RGE_Player* v5; // ecx
    int v7; // eax
    int v8; // esi
    int v9; // ebp
    int v10; // edi
    int* v11; // edx
    //int* v12; // ebx
    int v13; // eax
    int v14; // ebx
    int v15; // eax
    int v16; // eax
    BOOL v17; // zf
    int v18; // ebx
    int v19; // ebp
    int* v20; // esi
    int v21; // eax
    int v22; // edi
    int v23; // eax
    int* v24; // eax
    int v25; // ecx
    int v26; // edi
    RGE_Game_World* v27; // eax
    int v28; // esi
    RGE_Player* v29; // ecx
    //int v30; // ebx
    int* v31; // esi
    int v32; // eax
    int v33; // ecx
    //int* v34; // esi
    int v35; // ecx
    int v36; // edi
    int v37; // edi
    char v38; // [esp+13h] [ebp-251h]
    //int* v39; // [esp+14h] [ebp-250h]
    int* v40; // [esp+14h] [ebp-250h]
    //int* v41; // [esp+14h] [ebp-250h]
    //int v42; // [esp+18h] [ebp-24Ch]
    int* v43; // [esp+18h] [ebp-24Ch]
    int* v44; // [esp+18h] [ebp-24Ch]
    int v46; // [esp+20h] [ebp-244h]
    int v47; // [esp+24h] [ebp-240h]
    int v48; // [esp+28h] [ebp-23Ch]
    int v49; // [esp+2Ch] [ebp-238h]
    //int v50; // [esp+2Ch] [ebp-238h]
    int v51; // [esp+30h] [ebp-234h]
    int* v52; // [esp+34h] [ebp-230h]
    int v53; // [esp+38h] [ebp-22Ch]
    int v54; // [esp+3Ch] [ebp-228h]
    int v55; // [esp+40h] [ebp-224h]
    int v56; // [esp+44h] [ebp-220h]
    int v57; // [esp+48h] [ebp-21Ch]
    int v58; // [esp+4Ch] [ebp-218h]
    int v59; // [esp+50h] [ebp-214h]
    int v60[8]; // [esp+54h] [ebp-210h] BYREF
    char v61[100]; // [esp+74h] [ebp-1F0h] BYREF
    int v62[99]; // [esp+D8h] [ebp-18Ch] BYREF

    v1 = (RGE_RMM_Script_Controller*)this_;
    v2 = 1;
    if (this_->world->player_num > 1)
    {
        v3 = 1;
        do
        {
            v4 = 1;
            if (v3 > 1)
            {
                do
                {
                    v5 = v1->world->players[v3];
                    if (v5->vfptr->isAlly(v5, v4))
                        break;
                    ++v4;
                } while (v4 < v2);
            }
            ++v2;
            ++v3;
        } while (v2 < v1->world->player_num);
    }
    if (TRIBE_Game__randomizePositions((TRIBE_Game*)*base_game))
        return RGE_RMM_Script_Controller__do_player_positioning(v1);
    v7 = v1->land_info.land_num;
    v8 = v1->map_width;
    v9 = v1->map_height;
    v10 = 0;
    v48 = 0;
    v46 = 0;

    for (int i = 0; i < v1->land_info.land_num; i++)
    {
        if (v1->object_info.lands[i].player_id > 0)
        {
            ++v10;
            v13 = -2 * v1->land_info.land[i].base_size;
            if (v13 + v1->land_info.land[i].wall_3_avoidance_line - v1->land_info.land[i].wall_1_avoidance_line < v8)
                v8 = v13 + v1->land_info.land[i].wall_3_avoidance_line - v1->land_info.land[i].wall_1_avoidance_line;
            v14 = v1->land_info.land[i].wall_2_avoidance_line;
            if (v13 + v1->land_info.land[i].wall_4_avoidance_line - v14 < v9)
                v9 = v13 + v1->land_info.land[i].wall_4_avoidance_line - v14;
            v15 = v1->land_info.land[i].base_size;
            if (v15 + v1->land_info.land[i].wall_1_avoidance_line > v48)
                v48 = v15 + v1->land_info.land[i].wall_1_avoidance_line;
            v16 = v14 + v15;
            if (v16 > v46)
                v46 = v16;
        }
    }
    if (v10 < 1)
        v10 = 1;

    v18 = 6 * v8 / 10;
    v47 = 6 * v9 / 10;
    v56 = v18;
    v19 = 2 * v8 / 10;
    v51 = v18 + v47;
    v20 = v60;
    v57 = v18 + v18 + v47;
    v59 = 2 * (v18 + v47) / v10;
    v21 = v59 / v10;
    v22 = 0;
    v55 = v21;
    while (1)
    {
        v38 = 1;
        v23 = rand_internal();
        *v20 = 8 * v23 / 0x7FFF + 1;
        if (8 * v23 / 0x7FFF == 8)
            *v20 = 8;
        if (v22 <= 0)
            goto LABEL_71;
        v24 = v60;
        v25 = v22;
        do
        {
            if (*v24 == *v20)
                v38 = 0;
            ++v24;
            --v25;
        } while (v25);
        if (v38)
        {
        LABEL_71:
            ++v22;
            ++v20;
            if (v22 >= 8)
                break;
        }
    }
    if (this_->land_info.land_num > 0)
        memset(v61, 0, this_->land_info.land_num);
    v49 = 8;
    v26 = 2 * v51 * rand_internal() / 0x7FFF;
    v40 = v60;
    do
    {
        v43 = v60;
        v54 = 8;
        v53 = *v40;
        do
        {
            v27 = this_->world;
            v28 = *v43;
            v58 = *v43;
            if (*v43 < v27->player_num)
            {
                v29 = v27->players[v28];
                if (v29->vfptr->isAlly(v29, v53) || v28 == v53)
                {
                    if (this_->land_info.land_num > 0)
                    {
                        v31 = v62;
                        for (int i = 0; i < this_->land_info.land_num; i++)
                        {
                            if (!v61[i] && this_->object_info.lands[i].player_id > 0 && this_->object_info.lands[i].player_id == v58)
                            {
                                v61[i] = 1;
                                v32 = v55 * rand_internal() / 0x7FFF - v55 / 2 + v26;
                                *v31 = v32;
                                if (v32 < 0)
                                    *v31 = 2 * v51 + v32;
                                v33 = 2 * v51;
                                if (*v31 > 2 * v51)
                                    *v31 -= v33;
                                v26 += v59;
                                if (v26 >= v33)
                                    v26 -= v33;
                            }
                            ++v31;
                        }
                    }
                    v18 = v56;
                }
            }
            v17 = v54 == 1;
            ++v43;
            --v54;
        } while (!v17);
        v17 = v49 == 1;
        ++v40;
        --v49;
    } while (!v17);
    //v50 = 0;
    if (this_->land_info.land_num > 0)
    {
        v44 = v62;
        for (int i = 0; i < this_->land_info.land_num; i++)
        {
            if (this_->object_info.lands[i].player_id > 0)
            {
                //TRIBE_Player_Tech__do_tech(((TRIBE_World*)this_->world)->players[this_->object_info.lands[i].player_id]->tech_tree, 33); //holonet
                //this_->world->players[this_->object_info.lands[i].player_id];
                v35 = *v44;
                if (*v44 >= v18)
                {
                    if (v35 < v51)
                    {
                        v37 = v19 / 2;
                        this_->land_info.land[i].x = v48 + v18 + v19 / 2 + v19;
                        this_->land_info.land[i].y = v46 + v19 + v35 - v18;

                        this_->land_info.land[i].x += v19 * rand_internal() / 0x7FFF - v37;
                    }
                    else if (v35 >= v57)
                    {
                        v37 = v19 / 2;
                        this_->land_info.land[i].x = v19 / 2 + v48;
                        this_->land_info.land[i].y = v46 + v47 + v19 + v57 - v35;

                        this_->land_info.land[i].x += v19 * rand_internal() / 0x7FFF - v37;
                    }
                    else
                    {
                        this_->land_info.land[i].x = v48 + v18 + v19 + v51 - v35;
                        v36 = v19 / 2;
                        this_->land_info.land[i].y = v46 + v47 + v19 / 2 + v19;

                        this_->land_info.land[i].y += v19 * rand_internal() / 0x7FFF - v36;
                    }
                }
                else
                {
                    this_->land_info.land[i].x = v48 + v19 + v35;
                    v36 = v19 / 2;
                    this_->land_info.land[i].y = v19 / 2 + v46;

                    this_->land_info.land[i].y += v19 * rand_internal() / 0x7FFF - v36;
                }
            }
            ++v44;
        }
    }
    return 1;
}

__declspec(naked) unsigned __int8 __stdcall RGE_RMM_Script_Controller__do_land_command_new_asm(RGE_RMM_Script_Controller* this_, RGE_RMM_Token* command)
{
    __asm
    {
        mov     eax, [esp]
        mov     [esp], ecx
        push    eax
        mov     eax, RGE_RMM_Script_Controller__do_land_command_new
        jmp     eax
    }
}

__declspec(naked) unsigned __int8 __stdcall RGE_RMM_Script_Controller__setup_base_land_new_asm(RGE_RMM_Script_Controller* this_, int land_index, int owner)
{
    __asm
    {
        mov     eax, [esp]
        mov     [esp], ecx
        push    eax
        mov     eax, RGE_RMM_Script_Controller__setup_base_land_new
        jmp     eax
    }
}

void setScriptRMSHooks()
{
    setHook((void*)0x004E2C40, RGE_RMM_Script_Controller__do_land_command_new_asm);
    setHook((void*)0x004E33C0, RGE_RMM_Script_Controller__setup_base_land_new_asm);
    setHook((void*)0x004E76A0, TRIBE_RMM_Script_Controller__do_player_positioning_new);
}
