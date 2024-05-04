#include "stdafx.h"
#include "shield.h"

extern int current_loaded_version;

bool __fastcall gbg_is_shielded_new(RGE_Static_Object* obj)
{
    int x = obj->world_x;
    int y = obj->world_y;

    if (obj->inside_obj)
    {
        x = obj->inside_obj->world_x;
        y = obj->inside_obj->world_y;
    }
    
    if ((obj->master_obj->object_flags & 0x40) != 0)
        return true;

    switch (obj->master_obj->object_group)
    {
    case 6:
    case 59:
        return false;
    case 43:
    case 48:
    case 62:
    case 63:
    case 64:
        return obj->owner->attributes[38] > 0.0f && obj->master_obj->id != 1273;
    default:
        break;
    }

    switch (obj->master_obj->id)
    {
    case 4:
    case 977:
        if (obj->owner->attributes[51] > 0.0f)
            return true;
        break;
    case 12:
        if (obj->owner->attributes[33] > 0.0f)
            return true;
        break;
    case 4069:
    case 4778:
    case 5904:
        if (current_loaded_version >= 7)
            return false;
        break;
    default:
        break;
    }

    return RGE_Tile__gbg_get_shield_info(&obj->owner->world->map->map_row_offset[y][x], obj->owner->id);
}

#pragma optimize( "s", on )
void setShieldHooks()
{
    setHook((void*)0x0054BC40, gbg_is_shielded_new);
}
#pragma optimize( "", on )
