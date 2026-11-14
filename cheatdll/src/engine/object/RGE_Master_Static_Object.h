#pragma once

// Master object type discriminants for RGE_Master_Static_Object::master_type
// (idb_types.h:1206, unsigned __int8). Values supplied by project owner
// 2026-11-14; corroborated by raw-value usage in advcheat.cpp:77,
// advtriggereffect.cpp:415-426. IDA did not export this enum.
enum RGE_MASTER_TYPE
{
    RGE_MASTER_TYPE_STATIC       = 10,
    RGE_MASTER_TYPE_TREE         = 15,
    RGE_MASTER_TYPE_ANIMATED     = 20,
    RGE_MASTER_TYPE_DOPPELGANGER = 25,
    RGE_MASTER_TYPE_MOVING       = 30,
    RGE_MASTER_TYPE_ACTION       = 40,
    RGE_MASTER_TYPE_COMBAT_BASE  = 50,
    RGE_MASTER_TYPE_MISSILE      = 60,
    RGE_MASTER_TYPE_COMBAT       = 70,
    RGE_MASTER_TYPE_BUILDING     = 80,
};

// RGE_Master_Static_Object::object_flags (idb_types.h:1269, unsigned int),
// bits 8-15 = object_civ: the civ this master object is defined for
// (owner ruling 2026-11-14).
inline unsigned __int8 get_object_civ(const RGE_Master_Static_Object* master)
{
    return (unsigned __int8)((master->object_flags >> 8) & 0xFF);
}
