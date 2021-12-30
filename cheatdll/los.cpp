#include "stdafx.h"
#include "los.h"

void __stdcall delete_unit_los_data(UNIT_LOS_DATA* los_data)
{
    /*UNIT_LOS_DATA* current = los_data;
    do
    {
        UNIT_LOS_DATA* next = current->next;
        free(current);
        current = next;
    } while (current);*/
    free(los_data);
}

__declspec(naked) void los_constructor_default() //0054B33F
{
    __asm
    {
        mov     edx, [esp + 1Ch]
        cmp     edx, eax
        mov     [esi + 90h], eax
        mov     edx, 0054B345h
        jmp     edx
    }
}

__declspec(naked) void los_constructor_read() //0054B61D
{
    __asm
    {
        mov     [esi + 0Ch], eax
        mov     [esi + 1Ch], eax
        mov     [esi + 90h], eax
        mov     ecx, 0054B623h
        jmp     ecx
    }
}

__declspec(naked) void los_destructor() //0054B691
{
    __asm
    {
        mov     eax, [esi + 90h]
        test    eax, eax
        jz      los_destructor_no_free
        push    eax
        call    delete_unit_los_data
        mov     dword ptr [esi + 90h], 0

los_destructor_no_free:
        mov     eax, [esi + 18h]
        xor     ebx, ebx
        cmp     eax, ebx
        mov     ecx, 0054B698h
        jmp     ecx
    }
}

__declspec(naked) void los_recycle_out() //0054C0A7
{
    __asm
    {
        mov     eax, [esi + 90h]
        test    eax, eax
        jz      los_recycle_no_free
        push    eax
        call    delete_unit_los_data
        mov     dword ptr [esi + 90h], 0

los_recycle_no_free:
        mov     eax, [esi + 14h]
        cmp     [eax + 0BCh], bl
        mov     ecx, 0054C0B0h
        jmp     ecx
    }
}

bool __stdcall is_los_save_option_enabled(UNIT* unit)
{
    return *((unsigned char*)unit->player + 0x21) >= 0xD;
}

bool __stdcall is_los_save_option_enabled2(void* player)
{
    return *((unsigned char*)player + 0x21) >= 0xD;
}

void __stdcall los_read_save2(UNIT_LOS_DATA** los_data, void* infile)
{
    bool flag;
    deflate_read(infile, &flag, sizeof(flag));
    if (flag)
    {
        /*UNIT_LOS_DATA** next_ptr = los_data;
        bool next_flag;
        do
        {
            *next_ptr = (UNIT_LOS_DATA*)malloc(sizeof(UNIT_LOS_DATA));
            deflate_read(infile, &(*next_ptr)->x1, sizeof((*next_ptr)->x1));
            deflate_read(infile, &(*next_ptr)->y1, sizeof((*next_ptr)->y1));
            deflate_read(infile, &(*next_ptr)->x2, sizeof((*next_ptr)->x2));
            deflate_read(infile, &(*next_ptr)->y2, sizeof((*next_ptr)->y2));
            deflate_read(infile, &(*next_ptr)->radius, sizeof((*next_ptr)->radius));
            deflate_read(infile, &(*next_ptr)->square, sizeof((*next_ptr)->square));
            deflate_read(infile, &(*next_ptr)->sq_type, sizeof((*next_ptr)->sq_type));
            deflate_read(infile, &next_flag, sizeof(next_flag));
            next_ptr = &(*next_ptr)->next;
        } while (next_flag);
        *next_ptr = NULL;*/

        if (!*los_data)
            *los_data = (UNIT_LOS_DATA*)malloc(sizeof(UNIT_LOS_DATA));
        deflate_read(infile, &(*los_data)->x, sizeof((*los_data)->x));
        deflate_read(infile, &(*los_data)->y, sizeof((*los_data)->y));
        deflate_read(infile, &(*los_data)->radius, sizeof((*los_data)->radius));
    }
}

void __stdcall los_write_save2(UNIT_LOS_DATA* los_data, void* outfile)
{
    bool flag = los_data;
    deflate_write(outfile, &flag, sizeof(flag));
    if (flag)
    {
        /*UNIT_LOS_DATA* next_ptr = los_data;
        bool next_flag;
        do
        {
            deflate_write(outfile, &next_ptr->x1, sizeof(next_ptr->x1));
            deflate_write(outfile, &next_ptr->y1, sizeof(next_ptr->y1));
            deflate_write(outfile, &next_ptr->x2, sizeof(next_ptr->x2));
            deflate_write(outfile, &next_ptr->y2, sizeof(next_ptr->y2));
            deflate_write(outfile, &next_ptr->radius, sizeof(next_ptr->radius));
            deflate_write(outfile, &next_ptr->square, sizeof(next_ptr->square));
            deflate_write(outfile, &next_ptr->sq_type, sizeof(next_ptr->sq_type));
            next_flag = next_ptr->next;
            deflate_write(outfile, &next_flag, sizeof(next_flag));
            next_ptr = next_ptr->next;
        } while (next_ptr);*/

        deflate_write(outfile, &los_data->x, sizeof(los_data->x));
        deflate_write(outfile, &los_data->y, sizeof(los_data->y));
        deflate_write(outfile, &los_data->radius, sizeof(los_data->radius));
    }
}

__declspec(naked) void los_read_save() //0054C7C2
{
    __asm
    {
        push    esi
        call    is_los_save_option_enabled
        test    al, al
        jz      skip_los_read_save
        lea     eax, [esi + 90h]
        push    edi
        push    eax
        call    los_read_save2

skip_los_read_save:
        mov     dword ptr [esi + 30h], 0
        mov     eax, 0054C7C9h
        jmp     eax
    }
}

__declspec(naked) void los_write_save() //0054F766
{
    __asm
    {
        push    ebp
        call    is_los_save_option_enabled
        test    al, al
        jz      skip_los_write_save
        mov     eax, [ebp + 90h]
        push    edi
        push    eax
        call    los_write_save2

skip_los_write_save:
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        add     esp, 0Ch
        ret     4
    }
}

__declspec(naked) void los_write_save_this_fix() //0054F757
{
    __asm
    {
        mov     ebp, esi
        mov     esi, [esi + 24h]
        test    esi, esi
        mov     eax, 0054F75Ch
        jmp     eax
    }
}

void __stdcall explore_log(bool explore, bool sq, int* tx1, int* ty1, int* tx2, int* ty2, int* radius, int* Square, UNIT_LOS_DATA** los_data)
{
    /*
    //if (!no_fog_log)
    //if (obj == 9350 10813)
    {
        log("MAP UPDATE: %s, tx=%d, ty=%d, radius=%d, Square=%d, UpdateMap=%d, id=%d, ",
            explore ? "explore" : "unexplore", *tx, *ty, *radius, *Square, UpdateMap, id);
        if (*los_data)
            log("UNIT_LOS_DATA: x=%d, y=%d, radius=%d, square=%d",
                (*los_data)->x, (*los_data)->y, (*los_data)->radius, (int)(*los_data)->square);
        else
            log("UNIT_LOS_DATA: none");
        if (!*los_data)
        {
            *los_data = (UNIT_LOS_DATA*)malloc(sizeof(UNIT_LOS_DATA));
            memset(*los_data, 0, sizeof(UNIT_LOS_DATA));
        }
        log("SPECIAL MAP UPDATE: explore=%d, sq=%d", (int)explore, (int)sq);
        log("SPECIAL MAP_UPDATE: requested: tx1=%d, ty1=%d, tx2=%d, ty2=%d, radius=%d, square=%d",
            *tx1, *ty1, (tx2 ? *tx2 : 0), (ty2 ? *ty2 : 0), (radius ? *radius : 0), (Square ? *Square : 0));
        log("SPECIAL MAP_UPDATE:    stored: tx1=%d, ty1=%d, tx2=%d, ty2=%d, radius=%d, square=%d",
            (*los_data)->x1, (*los_data)->y1, (*los_data)->x2, (*los_data)->y2, (*los_data)->radius, (*los_data)->square);
        chat("Object = %d", obj);
        pause_game2();
        log("SPECIAL MAP UPDATE: explore=%d, sq=%d", (int)explore, (int)sq);
        log("SPECIAL MAP_UPDATE: requested: tx1=%d, ty1=%d, tx2=%d, ty2=%d, radius=%d, square=%d",
            *tx1, *ty1, (tx2 ? *tx2 : 0), (ty2 ? *ty2 : 0), (radius ? *radius : 0), (Square ? *Square : 0));
    }
    if (explore)
    {
        UNIT_LOS_DATA* new_los_data = (UNIT_LOS_DATA*)malloc(sizeof(UNIT_LOS_DATA));
        new_los_data->x1 = *tx1;
        new_los_data->y1 = *ty1;
        new_los_data->sq_type = sq;
        if (sq)
        {
            new_los_data->x2 = *tx2;
            new_los_data->y2 = *ty2;
            new_los_data->radius = 0;
            new_los_data->square = 0;
        }
        else
        {
            new_los_data->x2 = 0;
            new_los_data->y2 = 0;
            new_los_data->radius = *radius;
            new_los_data->square = *Square;
        }
        new_los_data->next = *los_data;
        *los_data = new_los_data;
        if (!*los_data)
            *los_data = (UNIT_LOS_DATA*)malloc(sizeof(UNIT_LOS_DATA));
        (*los_data)->x1 = *tx1;
        (*los_data)->y1 = *ty1;
        if (sq)
        {
            (*los_data)->x2 = *tx2;
            (*los_data)->y2 = *ty2;
            (*los_data)->radius = 0;
            (*los_data)->square = 0;
        }
        else
        {
            (*los_data)->x2 = 0;
            (*los_data)->y2 = 0;
            (*los_data)->radius = *radius;
            (*los_data)->square = *Square;
        }
    }
    else
    {
        if (*los_data)
        {
            if (
                sq != (*los_data)->sq_type
                || *tx1 != (*los_data)->x1
                || *ty1 != (*los_data)->y1
                || (tx2 ? *tx2 : 0) != (*los_data)->x2
                || (ty2 ? *ty2 : 0) != (*los_data)->y2
                || (radius ? *radius : 0) != (*los_data)->radius
                || (Square ? *Square : 0) != (*los_data)->square
                )
            {
                log("MAP_UPDATE: requested: sq_type=%d, tx1=%d, ty1=%d, tx2=%d, ty2=%d, radius=%d, square=%d",
                    (int)sq, *tx1, *ty1, (tx2 ? *tx2 : 0), (ty2 ? *ty2 : 0), (radius ? *radius : 0), (Square ? *Square : 0));
                log("MAP_UPDATE:    stored: sq_type=%d, tx1=%d, ty1=%d, tx2=%d, ty2=%d, radius=%d, square=%d",
                    (int)(*los_data)->sq_type, (*los_data)->x1, (*los_data)->y1, (*los_data)->x2, (*los_data)->y2, (*los_data)->radius, (*los_data)->square);
                chat("Object = %d", obj);
                //pause_game2();
            }
            *tx1 = (*los_data)->x1;
            *ty1 = (*los_data)->y1;
            if (sq)
            {
                *tx2 = (*los_data)->x2;
                *ty2 = (*los_data)->y2;
            }
            else
            {
                *radius = (*los_data)->radius;
                *Square = (*los_data)->square;
            }
            UNIT_LOS_DATA* new_top = (*los_data)->next;
            free(*los_data);
            *los_data = new_top;
        }
        else
        {
            log("MAP_UPDATE: requested: sq_type=%d, tx1=%d, ty1=%d, tx2=%d, ty2=%d, radius=%d, square=%d",
                (int)sq, *tx1, *ty1, (tx2 ? *tx2 : 0), (ty2 ? *ty2 : 0), (radius ? *radius : 0), (Square ? *Square : 0));
            log("MAP_UPDATE:    stored: none");
        }
    }*/
    if (!sq && Square && !*Square)
    {
        if (explore)
        {
            if (!*los_data)
                *los_data = (UNIT_LOS_DATA*)malloc(sizeof(UNIT_LOS_DATA));
            (*los_data)->x = *tx1;
            (*los_data)->y = *ty1;
            (*los_data)->radius = *radius;
        }
        else
        {
            if (*los_data)
            {
                if (*radius == (*los_data)->radius)
                {
                    *tx1 = (*los_data)->x;
                    *ty1 = (*los_data)->y;
                }
            }
        }
    }
}

__declspec(naked) void on_explore() //0055163E
{
    __asm
    {
        push    ecx
        mov     eax, [esp + 1Ch]
        push    eax     //los_data
        //mov     eax, [esp + 1Ch]
        //mov     obj, eax
        lea     eax, [esp + 14h]
        push    eax     //square
        lea     eax, [esp + 14h]
        push    eax     //radius
        push    0       //ty2
        push    0       //tx2
        lea     eax, [esp + 1Ch]
        push    eax     //ty1
        lea     eax, [esp + 1Ch]
        push    eax     //tx1
        push    0       //sq
        push    1       //explore
        call    explore_log
        pop     ecx

        mov     eax, 00615620h
        call    eax
        mov     eax, 00551643h
        jmp     eax
    }
}

__declspec(naked) void on_unexplore() //00551766
{
    __asm
    {
        push    ecx
        mov     eax, [esp + 1Ch]
        push    eax     //los_data
        //mov     eax, [esp + 1Ch]
        //mov     obj, eax
        lea     eax, [esp + 14h]
        push    eax     //square
        lea     eax, [esp + 14h]
        push    eax     //radius
        push    0       //ty2
        push    0       //tx2
        lea     eax, [esp + 1Ch]
        push    eax     //ty1
        lea     eax, [esp + 1Ch]
        push    eax     //tx1
        push    0       //sq
        push    0       //explore
        call    explore_log
        pop     ecx

        mov     eax, 006158D0h
        call    eax
        mov     eax, 0055176Bh
        jmp     eax
    }
}

__declspec(naked) void on_explore_sq() //005516C6
{
    __asm
    {
        push    ecx
        mov     eax, [esp + 1Ch]
        push    eax     //los_data
        //mov     eax, [esp + 1Ch]
        //mov     obj, eax
        push    0       //square
        push    0       //radius
        lea     eax, [esp + 1Ch]
        push    eax     //ty2
        lea     eax, [esp + 1Ch]
        push    eax     //tx2
        lea     eax, [esp + 1Ch]
        push    eax     //ty1
        lea     eax, [esp + 1Ch]
        push    eax     //tx1
        push    1       //sq
        push    1       //explore
        call    explore_log
        pop     ecx
        mov     eax, 006159F0h
        call    eax
        mov     eax, 005516CBh
        jmp     eax
    }
}

__declspec(naked) void on_unexplore_sq() //005517DC
{
    __asm
    {
        push    ecx
        mov     eax, [esp + 1Ch]
        push    eax     //los_data
        //mov     eax, [esp + 1Ch]
        //mov     obj, eax
        push    0       //square
        push    0       //radius
        lea     eax, [esp + 1Ch]
        push    eax     //ty2
        lea     eax, [esp + 1Ch]
        push    eax     //tx2
        lea     eax, [esp + 1Ch]
        push    eax     //ty1
        lea     eax, [esp + 1Ch]
        push    eax     //tx1
        push    1       //sq
        push    0       //explore
        call    explore_log
        pop     ecx
        mov     eax, 00615BE0h
        call    eax
        mov     eax, 005517E1h
        jmp     eax
    }
}

//RGE_Visible_Map expansion
__declspec(naked) void visible_map_constructor1_1() //006151EF
{
    __asm
    {
        shl     eax, 1
        push    eax
        mov     eax, 00632D33h
        call    eax //_calloc
        mov     [esi + 10h], eax
        mov     eax, [esi + 18h]
        //shl     eax, 1
        //mov     ecx, 006151FBh
        //jmp     ecx
        push    006151FBh
        ret
    }
}

__declspec(naked) void visible_map_constructor1_2() //00615217
{
    __asm
    {
        imul    ecx, eax
        shl     ecx, 1
        mov     edx, [esi]
        lea     ecx, [ecx + edi * 1]
        //mov     edi, 0061521Eh
        //jmp     edi
        push    0061521Eh
        ret
    }
}

__declspec(naked) void visible_map_constructor1_3() //0061522F
{
    __asm
    {
        imul    ecx, [esi + 14h]
        shl     ecx, 1
        mov     edx, ecx
        push    00615235h
        ret
    }
}

__declspec(naked) void visible_map_constructor2_1() //006152FE
{
    __asm
    {
        shl     eax, 1
        push    eax
        mov     eax, 00632D33h
        call    eax //_calloc
        //mov     ecx, 00615304h
        //jmp     ecx
        push    00615304h
        ret
    }
}

__declspec(naked) void visible_map_constructor2_2() //00615324
{
    __asm
    {
        imul    ecx, eax
        shl     ecx, 1
        add     ecx, edx
        mov     edx, [esi]
        mov     [edx + eax * 4], ecx
        //mov     ecx, 0061532Eh
        //jmp     ecx
        push    0061532Eh
        ret
    }
}

void __stdcall read_visible_map(int x, int y, unsigned __int16* visible_map, void* player, void* infile)
{
    if (is_los_save_option_enabled2(player))
        deflate_read(infile, visible_map, x * y * 2);
    else
    {
        unsigned __int8* temp_map = (unsigned char*)malloc(x * y);
        deflate_read(infile, temp_map, x * y);
        for (int i = 0; i < (x * y); i++)
            visible_map[i] = temp_map[i] == 0xFF ? 0xFFFF : temp_map[i];
        free(temp_map);
    }
}

__declspec(naked) void visible_map_constructor2_3() //00615335
{
    __asm
    {
        mov     ecx, [esi + 8]
        push    ebx
        push    ecx
        mov     edx, [esi + 10h]
        mov     eax, [edi]
        mov     ecx, [ebp + 0]
        push    edx
        push    eax
        push    ecx
        call    read_visible_map
        //mov     eax, 00615355h
        //jmp     eax
        push    00615355h
        ret
    }
}

void __stdcall write_visible_map(int x, int y, unsigned __int16* visible_map, void* player, void* outfile)
{
    if (is_los_save_option_enabled2(player))
        deflate_write(outfile, visible_map, x * y * 2);
    else
    {
        unsigned __int8* temp_map = (unsigned char*)malloc(x * y);
        for (int i = 0; i < (x * y); i++)
            temp_map[i] = visible_map[i] == 0xFFFF ? 0xFF : (visible_map[i] >= 0xF9 ? 0xF9 : (unsigned __int8)visible_map[i]);
        deflate_write(outfile, temp_map, x * y);
        free(temp_map);
    }
}

__declspec(naked) void visible_map_save() //00615411
{
    __asm
    {
        mov     ecx, [esi + 8]
        push    edi
        push    ecx
        mov     edx, [esi + 10h]
        mov     ecx, [ebp + 0]
        push    edx
        push    eax
        push    ebx
        call    write_visible_map
        //mov     eax, 0061541Fh
        //jmp     eax
        push    0061541Fh
        ret
    }
}

__declspec(naked) void visible_map_set_all_1() //00615473
{
    __asm
    {
        push    ebx
        mov     bx, [esp + 8]
        //mov     edx, 00615478h
        //jmp     edx
        push    00615478h
        ret
    }
}
__declspec(naked) void visible_map_set_all_2() //0061548C
{
    __asm
    {
        mov     edx, [edx + esi * 4]
        mov     [edx + eax * 2], bx
        inc     eax
        //mov     edx, 00615494h
        //jmp     edx
        push    00615494h
        ret
    }
}

__declspec(naked) void visible_map_explore_all_1() //0061555E
{
    __asm
    {
        mov     ecx, [eax]
        cmp     word ptr [ecx + edi * 2], 0FFFFh
        //mov     edx, 00615564h
        //jmp     edx
        push    00615564h
        ret
    }
}

__declspec(naked) void visible_map_explore_all_2() //0061556A
{
    __asm
    {
        mov     word ptr [edx + edi * 2], 0
        mov     eax, [esi + 1Ch]
        //mov     ecx, 00615571h
        //jmp     ecx
        push    00615571h
        ret
    }
}

__declspec(naked) void visible_map_explore_terrain_1() //00615732
{
    __asm
    {
        lea     eax, [eax + edi * 2]
        mov     [esp + 48h], eax
        mov     edx, [edx + ebp * 4]
        add     edx, ebx
        //mov     ebx, 0061573Dh
        //jmp     ebx
        push    0061573Dh
        ret
    }
}

__declspec(naked) void visible_map_explore_terrain_2() //0061574C
{
    __asm
    {
        mov     bx, [eax]
        inc     bx
        jnz     loc_615835
        //mov     edx, 00615756h
        //jmp     edx
        push    00615756h
        ret

loc_615835:
        //mov     edx, 00615835h
        //jmp     edx
        push    00615835h
        ret
    }
}

__declspec(naked) void visible_map_explore_terrain_3() //0061582C
{
    __asm
    {
        mov     ecx, [esi + 2Ch]
        mov     bx, 1
        //mov     edx, 00615831h
        //jmp     edx
        push    00615831h
        ret
    }
}

__declspec(naked) void visible_map_explore_terrain_4() //00615835
{
    __asm
    {
        cmp     bx, 1
        jnz     loc_6158C5
        //mov     edx, 0061583Eh
        //jmp     edx
        push    0061583Eh
        ret

loc_6158C5:
        //mov     edx, 006158C5h
        //jmp     edx
        push    006158C5h
        ret
    }
}

__declspec(naked) void visible_map_explore_terrain_5() //00615878
{
    __asm
    {
        mov     [eax], bx
        mov     ebx, [esp + 10h]
        add     ebx, 4
        add     eax, 2
        mov     [esp + 10h], ebx
        //mov     ebx, 00615886h
        //jmp     ebx
        push    00615886h
        ret
    }
}

__declspec(naked) void visible_map_explore_terrain_6() //006158C5
{
    __asm
    {
        cmp     bx, 7D00h
        jnz     loc_615878
        mov     bx, 7CFFh

loc_615878:
        //mov     edx, 00615878h
        //jmp     edx
        push    00615878h
        ret
    }
}

__declspec(naked) void visible_map_explore_terrain_7() //006157D0
{
    __asm
    {
        mov     ecx, [esp + 24h]
        cmp     word ptr [eax + ecx * 2], 0FFFFh
        push    006157D8h
        ret
    }
}

__declspec(naked) void visible_map_explore_terrain_8() //00615809
{
    __asm
    {
        mov     word ptr [eax], 1
        mov     eax, [esp + 34h]
        push    00615810h
        ret
    }
}

__declspec(naked) void visible_map_unexplore_terrain_1() //00615966
{
    __asm
    {
        lea     ebx, [ebx + esi * 4]
        lea     eax, [eax + esi * 2]
        push    0061596Bh
        ret
    }
}

__declspec(naked) void visible_map_unexplore_terrain_2() //00615992
{
    __asm
    {
        mov     dx, [eax]
        dec     dx
        cmp     dx, 0FFFFh
        jnz     loc_61599F
        xor     dx, dx
        jmp     loc_6159A3

loc_61599F:
        test    dx, dx
        jnz     loc_6159B4

loc_6159A3:
        push    006159A3h
        ret

loc_6159B4:
        push    006159B4h
        ret
    }
}

__declspec(naked) void visible_map_unexplore_terrain_3() //006159B4
{
    __asm
    {
        mov     [eax], dx
        add     eax, 2
        add     ebx, 4
        push    006159BAh
        ret
    }
}

__declspec(naked) void visible_map_explore_terrain_sq_1() //00615AE3
{
    __asm
    {
        mov     eax, [esp + 34h]
        lea     ebx, [ebx + edx * 2]
        push    00615AE9h
        ret
    }
}

__declspec(naked) void visible_map_explore_terrain_sq_2() //00615B10
{
    __asm
    {
        mov     ax, [ebx]
        inc     ax
        jnz     loc_615B64
        push    00615B16h
        ret

loc_615B64:
        cmp     ax, 1
        jnz     loc_615B83
        push    00615B68h
        ret

loc_615B83:
        push    00615B83h
        ret
    }
}

__declspec(naked) void visible_map_explore_terrain_sq_3() //00615B58
{
    __asm
    {
        or      ecx, eax
        mov     ax, 1
        mov     [edi], ecx
        push    00615B5Eh
        ret
    }
}

__declspec(naked) void visible_map_explore_terrain_sq_4() //00615B83
{
    __asm
    {
        cmp     ax, 7D00h
        jnz     loc_615B89
        mov     ax, 7CFFh

loc_615B89:
        push    00615B89h
        ret
    }
}

__declspec(naked) void visible_map_explore_terrain_sq_5() //00615B89
{
    __asm
    {
        mov     [ebx], ax
        mov     eax, [esp + 10h]
        add     eax, 20h
        add     ebx, 2
        push    00615B93h
        ret
    }
}

__declspec(naked) void visible_map_unexplore_terrain_sq_1() //00615C79
{
    __asm
    {
        mov     edx, [esp + 28h]
        lea     eax, [eax + ecx * 2]
        push    00615C7Fh
        ret
    }
}

__declspec(naked) void visible_map_unexplore_terrain_sq_2() //00615CA3
{
    __asm
    {
        mov     dx, [eax]
        dec     dx
        cmp     dx, 0FFFFh
        jnz     loc_615CB0
        xor     dx, dx
        jmp     loc_615CB4

loc_615CB0:
        test    dx, dx
        jnz     loc_615CCD

loc_615CB4:
        push    00615CB4h
        ret

loc_615CCD:
        push    00615CCDh
        ret
    }
}

__declspec(naked) void visible_map_unexplore_terrain_sq_3() //00615CCD
{
    __asm
    {
        mov     [eax], dx
        add     eax, 2
        add     esi, 4
        push    00615CD3h
        ret
    }
}

__declspec(naked) void visible_map_reveal_terrain_sq_1() //00615DF6
{
    __asm
    {
        lea     ebp, [ebp + ecx * 2]
        shl     ecx, 2
        push    00615DFBh
        ret
    }
}

__declspec(naked) void visible_map_reveal_terrain_sq_2() //00615E2A
{
    __asm
    {
        cmp     word ptr [ebp + 0], 0FFFFh
        jnz     loc_615E91
        push    00615E30h
        ret

loc_615E91:
        push    00615E91h
        ret
    }
}

__declspec(naked) void visible_map_reveal_terrain_sq_3() //00615E89
{
    __asm
    {
        mov     ecx, [esp + 28h]
        mov     word ptr [ebp + 0], 0
        push    00615E91h
        ret
    }
}

__declspec(naked) void visible_map_reveal_terrain_sq_4() //00615E91
{
    __asm
    {
        mov     edx, [esp + 2Ch]
        add     ebp, 2
        push    00615E96h
        ret
    }
}


//1400062
//1407082

void (__thiscall* WorldPlayer__new_attribute_num)(void* this_, __int16 attribute, float amount) =
    (void (__thiscall*)(void*, __int16, float))0x005D3820;

__declspec(naked) void __fastcall reveal_test(void* player)
{
    __asm
    {
        mov     edx, [ecx + 8Ch]
        mov     dword ptr [ecx + 15Ch], 0
        mov     eax, [edx + 0ACh]
        mov     dword ptr [eax + 50 * 4], 0
        mov     eax, [edx]
        call    dword ptr [eax + 110h]
        ret
    }
}
void __stdcall print_explore(void* player, int current, int total)
{
    int id = (int)*((unsigned char*)player + 0xA0);
    chat("Player %d, explore: %d / %d (%.2f%%)", id, current, total, (float)current * 100.0f / total);
    //if (id == 2)
    //    WorldPlayer__new_attribute_num(player, 50, 1.0f);
    reveal_test(player);
}

__declspec(naked) void handle_explore() //005CEB00
{
    __asm
    {
        mov     ecx, [ebp + 88h]
        push    dword ptr[ecx + 20h]
        push    dword ptr[ecx + 1Ch]
        push    ebp
        call    print_explore
        mov     ecx, [ebp + 88h]
        fild    dword ptr [ecx + 1Ch]
        fidiv   dword ptr [ecx + 20h]
        mov     eax, 005CEB0Bh
        jmp     eax
    }
}

__declspec(naked) void unlock_teams() //0042C710
{
    __asm
    {
        xor     al, al
        mov     [ecx + 9CEh], al
        ret
    }
}

void __stdcall print_explore2(int e, bool sq)
{
    FILE* f = fopen("explore.txt", "at");
    if (f)
    {
        fprintf(f, "%d - %d, sq=%s\n", e, e + 1, sq ? "true" : "false");
        fclose(f);
    }
}

__declspec(naked) void check_explore() //0061575A
{
    __asm
    {
        mov     ecx, [esi + 1Ch]
        inc     edx
        push    edx
        push    ecx
        push    0
        push    ecx
        call    print_explore2
        pop     ecx
        pop     edx
        inc     ecx
        mov     [esi + 1Ch], ecx

        push    00615762h
        ret
    }
}

__declspec(naked) void check_explore_sq() //00615B1A
{
    __asm
    {
        mov     eax, [esi + 1Ch]
        push    ecx
        mov     ecx, [esi + 8]
        push    ecx
        push    edx
        push    eax
        push    1
        push    eax
        call    print_explore2
        pop     eax
        pop     edx
        pop     ecx
        push    00615B21h
        ret
    }
}

#pragma optimize( "s", on )
void setLOSHooks()
{
    //writeWord(0x0042C710, 0xC032);
    //writeDword(0x0042C712, 0x90909090);
    //writeByte(0x005D382C, 0xEB);

    //setHook((void*)0x0061575A, check_explore);
    //setHook((void*)0x00615B1A, check_explore_sq);

    //setHook((void*)0x005CEB00, handle_explore);
    //setHook((void*)0x0042C710, unlock_teams);

    
    setHook((void*)0x0054B33F, los_constructor_default);
    setHook((void*)0x0054B61D, los_constructor_read);
    setHook((void*)0x0054B691, los_destructor);
    setHook((void*)0x0054C0A7, los_recycle_out);
    setHook((void*)0x0054C7C2, los_read_save);
    setHook((void*)0x0054F766, los_write_save);
    setHook((void*)0x0054F757, los_write_save_this_fix);

    setHook((void*)0x0055163E, on_explore);
    setHook((void*)0x00551766, on_unexplore);
    setHook((void*)0x005516C6, on_explore_sq);
    setHook((void*)0x005517DC, on_unexplore_sq);

    //LoS depth
    setHook((void*)0x006151EF, visible_map_constructor1_1);
    setHook((void*)0x00615217, visible_map_constructor1_2);
    setHook((void*)0x0061522F, visible_map_constructor1_3);
    setHook((void*)0x006152FE, visible_map_constructor2_1);
    setHook((void*)0x00615324, visible_map_constructor2_2);
    setHook((void*)0x00615335, visible_map_constructor2_3);
    setHook((void*)0x00615411, visible_map_save);
    setHook((void*)0x00615473, visible_map_set_all_1);
    setHook((void*)0x0061548C, visible_map_set_all_2);
    setHook((void*)0x0061555E, visible_map_explore_all_1);
    setHook((void*)0x0061556A, visible_map_explore_all_2);
    setHook((void*)0x00615732, visible_map_explore_terrain_1);
    setHook((void*)0x0061574C, visible_map_explore_terrain_2);
    setHook((void*)0x0061582C, visible_map_explore_terrain_3);
    setHook((void*)0x00615835, visible_map_explore_terrain_4);
    setHook((void*)0x00615878, visible_map_explore_terrain_5);
    setHook((void*)0x006158C5, visible_map_explore_terrain_6);
    setHook((void*)0x006157D0, visible_map_explore_terrain_7);
    setHook((void*)0x00615809, visible_map_explore_terrain_8);
    setHook((void*)0x00615966, visible_map_unexplore_terrain_1);
    setHook((void*)0x00615992, visible_map_unexplore_terrain_2);
    setHook((void*)0x006159B4, visible_map_unexplore_terrain_3);
    setHook((void*)0x00615AE3, visible_map_explore_terrain_sq_1);
    setHook((void*)0x00615B10, visible_map_explore_terrain_sq_2);
    setHook((void*)0x00615B58, visible_map_explore_terrain_sq_3);
    setHook((void*)0x00615B83, visible_map_explore_terrain_sq_4);
    setHook((void*)0x00615B89, visible_map_explore_terrain_sq_5);
    setHook((void*)0x00615C79, visible_map_unexplore_terrain_sq_1);
    setHook((void*)0x00615CA3, visible_map_unexplore_terrain_sq_2);
    setHook((void*)0x00615CCD, visible_map_unexplore_terrain_sq_3);
    setHook((void*)0x00615DF6, visible_map_reveal_terrain_sq_1);
    setHook((void*)0x00615E2A, visible_map_reveal_terrain_sq_2);
    setHook((void*)0x00615E89, visible_map_reveal_terrain_sq_3);
    setHook((void*)0x00615E91, visible_map_reveal_terrain_sq_4);
}
#pragma optimize( "", on )
