#include "stdafx.h"
#include "trade.h"

bool find_by_master_ids_trade_flag = false;
int obj_capacity = 0;
int n_objects = 0;
UNIT** objects = NULL;
float loc_x = -1.0f;
float loc_y = -1.0f;

bool __stdcall get_new_trade_behavior(void* player)
{
    unsigned char o = *((unsigned char*)player + 0x21);
    switch (o)
    {
    case 0xB:
    case 0xC:
        return false;
    default:
        return true;
    }
}

__declspec(naked) void on_trade_find_call_1() //005730AB
{
    __asm
    {
        mov     find_by_master_ids_trade_flag, 1

        push    ecx
        mov     eax, [esi + 8]
        mov     eax, [eax + 18h]
        push    eax
        call    get_new_trade_behavior
        test    al, al
        jz      on_trade_find_call_1_skip

        mov     edx, [esi + 18h]
        push    edx
        mov     eax, [esi + 8]
        mov     eax, [eax + 18h]
        mov     ecx, [eax + 8Ch]
        call    BaseWorld__object
        mov     edx, [eax + 48h]
        mov     [esp + 0Ch], edx
        mov     edx, [eax + 4Ch]
        mov     [esp + 10h], edx

on_trade_find_call_1_skip:
        pop     ecx
        mov     eax, 004AF3E0h
        call    eax
        mov     find_by_master_ids_trade_flag, 0
        mov     ecx, 005730B0h
        jmp     ecx
    }
}

__declspec(naked) void on_trade_find_call_2() //005730D7
{
    __asm
    {
        mov     find_by_master_ids_trade_flag, 1

        push    ecx
        mov     eax, [esi + 8]
        mov     eax, [eax + 18h]
        push    eax
        call    get_new_trade_behavior
        test    al, al
        jz      on_trade_find_call_2_skip

        mov     edx, [esi + 18h]
        push    edx
        mov     eax, [esi + 8]
        mov     eax, [eax + 18h]
        mov     ecx, [eax + 8Ch]
        call    BaseWorld__object
        mov     edx, [eax + 48h]
        mov     [esp + 0Ch], edx
        mov     edx, [eax + 4Ch]
        mov     [esp + 10h], edx

on_trade_find_call_2_skip:
        pop     ecx
        mov     eax, 004AF3E0h
        call    eax
        mov     find_by_master_ids_trade_flag, 0
        mov     ecx, 005730DCh
        jmp     ecx
    }
}

void __stdcall add_found_unit(UNIT* unit, float x, float y)
{
    if (n_objects >= obj_capacity)
    {
        obj_capacity *= 2;
        UNIT** objects_new = (UNIT**)malloc(obj_capacity * sizeof(UNIT*));
        memcpy(objects_new, objects, sizeof(UNIT**) * n_objects);
        free(objects);
        objects = objects_new;
    }
    loc_x = x;
    loc_y = y;
    objects[n_objects++] = unit;
}

int n_calls = 0;

UNIT* __stdcall get_best_object()
{
    UNIT* best_object = NULL;
    if (n_objects > 0)
    {
        if (get_new_trade_behavior(objects[0]->player))
        {
            //find max dist
            float max_dist = 0.0f;
            for (int i = 0; i < n_objects; i++)
            {
                float dist = (objects[i]->x - loc_x) * (objects[i]->x - loc_x) + (objects[i]->y - loc_y) * (objects[i]->y - loc_y);
                if (dist > max_dist)
                {
                    max_dist = dist;
                    best_object = objects[i];
                }
            }
        }
        else
        {
            //old behavior - find closest
            float min_dist = -1.0f;
            for (int i = 0; i < n_objects; i++)
            {
                float dist = (objects[i]->x - loc_x) * (objects[i]->x - loc_x) + (objects[i]->y - loc_y) * (objects[i]->y - loc_y);
                if (min_dist < 0.0f || dist < min_dist)
                {
                    min_dist = dist;
                    best_object = objects[i];
                }
            }
        }
    }

    n_objects = 0;

    //chat("get_best_object() called, total: %d", n_calls++);

    return best_object;
}

__declspec(naked) void find_by_master_ids_trade() //004AF596
{
    __asm
    {
        mov     al, find_by_master_ids_trade_flag
        test    al, al
        jz      find_by_master_ids_trade_skip
        mov     eax, [esp + 6Ch]
        push    eax //y
        mov     eax, [esp + 6Ch]
        push    eax //x
        push    esi //unit
        call    add_found_unit
        mov     ecx, 004AF5DCh
        jmp     ecx

find_by_master_ids_trade_skip:
        fld     dword ptr [esp + 68h]
        fsub    dword ptr [esi + 48h]
        mov     eax, 004AF59Dh
        jmp     eax
    }
}

__declspec(naked) void find_by_master_ids_trade_end() //004AF5EE
{
    __asm
    {
        mov     cl, find_by_master_ids_trade_flag
        test    cl, cl
        jz      find_by_master_ids_trade_end_skip
        call    get_best_object

find_by_master_ids_trade_end_skip:
        mov     ecx, [esp + 50h]
        pop     edi
        pop     esi
        mov     ebx, 004AF5F4h
        jmp     ebx
    }
}

#pragma optimize( "s", on )
void setTradeHooks()
{
    objects = (UNIT**)malloc(0x100 * sizeof(UNIT*));
    obj_capacity = 0x100;

    setHook((void*)0x005730AB, on_trade_find_call_1);
    setHook((void*)0x005730D7, on_trade_find_call_2);
    setHook((void*)0x004AF596, find_by_master_ids_trade);
    setHook((void*)0x004AF5EE, find_by_master_ids_trade_end);
}
#pragma optimize( "", on )
