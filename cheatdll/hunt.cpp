#include "stdafx.h"
#include "hunt.h"
#include "casts.h"

extern int current_loaded_version;

bool check_hunt_idle_timer(RGE_Static_Object* obj)
{
    UNIT_EXTRA* ud = createUnitExtra(obj);
    ud->keepUnitExtra = true;
    if (ud->animalTimer > 2)
        return true;
    else
    {
        ud->animalTimer++;
        return false;
    }
}

void reset_hunt_idle_timer(RGE_Static_Object* obj)
{
    UNIT_EXTRA* ud = createUnitExtra(obj);
    ud->animalTimer = 0;
}

RGE_Static_Object* hunt_find_scary_object(TribeHuntedAnimalUnitAIModule* unit_ai)
{
    int objects[50];
    int n_objects = 0;
    RGE_Static_Object* closest_obj = NULL;
    float min_dist = 99999.0f;

    RGE_Game_World* world = (RGE_Game_World*)unit_ai->objectValue->owner->world;

    UnitAIModule__unitsInSight((UnitAIModule*)unit_ai, -1, objects, &n_objects, 50);

    for (int i = 0; i < n_objects; i++)
    {
        RGE_Static_Object* obj = RGE_Game_World__object(world, objects[i]);
        short object_group = obj->master_obj->object_group;
        bool scary_object;
        switch (object_group)
        {
        case 0:
        case 1:     //nerf
        case 2:     //fambaa
        case 4:     //wild animal
        case 6:     //wall
        case 7:     //farm
        case 8:     //gate
        case 9:     //aa turret
        case 10:    //turret
        case 18:    //building
        case 19:    //doppleganger
        case 20:    //other
        case 21:    //command base
        case 22:    //cliff
        case 23:    //fish
        case 25:    //shore fish
        case 26:    //engine
        case 27:    //fruit bush
        case 28:    //holocron
        case 29:    //nova
        case 30:    //ore
        case 31:    //carbon
        case 42:    //misc
            scary_object = false;
            break;
        default:
            scary_object = true;
            break;
        }
        if (scary_object)
        {
            float dist = RGE_Static_Object__distance_to_object((RGE_Static_Object*)unit_ai->objectValue, obj);
            if (dist < min_dist)
            {
                min_dist = dist;
                closest_obj = obj;
            }
        }
    }

    return closest_obj;
}

int __stdcall hunt_process_idle_new(TribeHuntedAnimalUnitAIModule* unit_ai)
{
    RGE_Static_Object* closest_obj = hunt_find_scary_object(unit_ai);
    if (!closest_obj)
    {
        if (!check_hunt_idle_timer((RGE_Static_Object*)unit_ai->objectValue))
            return 5;

        if (rand_internal() % 7)
            return 5;

        float x_dist = unit_ai->objectValue->world_x - unit_ai->originX;
        float y_dist = unit_ai->objectValue->world_y - unit_ai->originY;
        if (sqrt(x_dist * x_dist + y_dist * y_dist) <= 2.0f)
        {
            int x_rnd_offset = rand_internal() % 6 - 3;
            int y_rnd_offset = rand_internal() % 6 - 3;
            float x_param = x_rnd_offset + unit_ai->objectValue->world_x;
            float y_param = y_rnd_offset + unit_ai->objectValue->world_y;
            unit_ai->vfptr->moveTo_3(
                unit_ai,
                x_param,
                y_param,
                unit_ai->objectValue->world_z,
                0.5f,
                1,
                1);
        }
        else
        {
            unit_ai->vfptr->moveTo_3(
                unit_ai,
                unit_ai->originX,
                unit_ai->originY,
                unit_ai->objectValue->world_z,
                0.5f,
                1,
                1);
        }
        return 6;
    }
    else
    {
        reset_hunt_idle_timer((RGE_Static_Object*)unit_ai->objectValue);

        float speed_value = unit_ai->objectValue->vfptr->maximumSpeed(unit_ai->objectValue) * 3.0f;
        float x_diff = closest_obj->world_x - unit_ai->objectValue->world_x;
        float y_diff = closest_obj->world_y - unit_ai->objectValue->world_y;
        float dist = sqrt(x_diff * x_diff + y_diff * y_diff);
        float x_evasive_param = unit_ai->objectValue->world_x - x_diff / dist * speed_value;
        float y_evasive_param = unit_ai->objectValue->world_y - y_diff / dist * speed_value;
        unit_ai->vfptr->evasiveMoveTo(
            unit_ai,
            x_evasive_param,
            y_evasive_param,
            unit_ai->objectValue->world_z,
            1);
        return 6;
    }
}

__declspec(naked) void on_hunt_process_idle() //005B217A
{
    __asm
    {
        mov     eax, current_loaded_version
        cmp     eax, 6
        jl      hunt_process_idle_old
        push    esi
        call    hunt_process_idle_new
        pop     edi
        pop     esi
        add     esp, 8
        ret     8

hunt_process_idle_old:
        mov     edx, [esi]
        push    0
        push    0FFFFFFFFh
        mov     eax, 005B2180h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setHuntHooks()
{
    setHook((void*)0x005B217A, on_hunt_process_idle);
}
#pragma optimize( "", on )
