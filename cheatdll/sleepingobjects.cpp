#include "stdafx.h"
#include "sleepingobjects.h"

extern int current_loaded_version;

bool __fastcall is_sleeping_object(TRIBE_Building_Object* obj)
{
    if (current_loaded_version < 9)
        return obj->master_obj->id == 72;   //light wall
    else
        return obj->master_obj->id == 70 || obj->master_obj->object_group == 6;
}

__declspec(naked) void on_sleep() //00554C17
{
    __asm
    {
        mov     ecx, esi
        call    is_sleeping_object
        test    al, al
        jnz     set_sleep

        mov     eax, 00554E00h
        jmp     eax

set_sleep:
        mov     edx, 00554C25h
        jmp     edx
    }
}

void __stdcall do_sleeping_objects_update(RGE_Player* player)
{
    for (int i = 0; i < player->sleeping_objects->Number_of_objects; i++)
    {
        RGE_Static_Object* obj = player->sleeping_objects->List[i];
        if (obj->type == 80 && obj->object_state == 2)
        {
            if (obj->vfptr->is_shielded(obj) && obj->master_obj->hp >= obj->sp)
            {
                obj->vfptr->gbg_doShieldRecharge(obj);
            }
            else if (obj->sp > 0.0f)
            {
                obj->vfptr->gbg_doShieldDropoff(obj);
            }
        }
    }
}

__declspec(naked) void on_player_update_list() //004C1985
{
    __asm
    {
        call    RGE_Player_Object_List__Update
        push    esi
        call    do_sleeping_objects_update

        mov     eax, 004C198Ah
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setSleepingObjectsHooks()
{
    setHook((void*)0x00554C17, on_sleep);
    setHook((void*)0x004C1985, on_player_update_list);
}
#pragma optimize( "", on )
