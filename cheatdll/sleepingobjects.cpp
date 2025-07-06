#include "stdafx.h"
#include "sleepingobjects.h"
#include "harbor.h"

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

bool __fastcall TRIBE_Building_Object__gbg_is_regenerating_bldg_new(TRIBE_Building_Object* obj)
{
    if (current_loaded_version >= 9)
    {
        if (obj->object_state == 2)
        {
            switch (obj->master_obj->object_group)
            {
            case 6:  //wall
            case 7:  //farm
            case 8:  //gate
            case 9:  //aa turret
            case 10: //turret
            case 18: //building
                return true;
            default:
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        switch (obj->master_obj->id)
        {
        case 68:    //food proc cntr
        case 319:   //animal nursery
        case 562:   //carbon proc cntr
        case 323:   //nova proc cntr
        case 584:   //ore proc cntr
        case 87:    //troop cntr
        case 101:   //mech factory
        case 45:    //shipyard
        case 104:   //jedi temple
        case 49:    //hvy wep factory
        case 317:   //airbase
        case 82:    //fortress
        case 103:   //war center
        case 209:   //research center
        case 84:    //spaceport
        case 530:   //tatooine spaceport
        case 598:   //sentry post
        case 70:    //prefab shelter
        case 79:    //light turret
        case 234:   //medium turret
        case 235:   //adv turret
        case 236:   //aa turret
        case 196:   //adv aa turret
        case 1001:  //underwater prefab shelter
        case 335:   //shield generator
        case 109:   //command center
        case HARBOR_ID_COMPLETE:
            return true;
        default:
            return false;
        }
    }
}

void __stdcall do_sleeping_objects_update(RGE_Player* player)
{
    if (current_loaded_version >= 9)
    {
        for (int i = 0; i < player->sleeping_objects->Number_of_objects; i++)
        {
            RGE_Static_Object* obj = player->sleeping_objects->List[i];
            if (obj->type == 80 && obj->object_state == 2)
            {
                //shields
                if (obj->vfptr->is_shielded(obj) && obj->master_obj->hp >= obj->sp)
                {
                    obj->vfptr->gbg_doShieldRecharge(obj);
                }
                else if (obj->sp > 0.0f)
                {
                    obj->vfptr->gbg_doShieldDropoff(obj);
                }

                //regeneration
                if (obj->owner->culture == 2)
                    TRIBE_Building_Object__gbg_do_regenerate((TRIBE_Building_Object*)obj);
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

    setHook((void*)0x00554F70, TRIBE_Building_Object__gbg_is_regenerating_bldg_new);
}
#pragma optimize( "", on )
