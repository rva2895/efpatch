#include "stdafx.h"
#include "RGE_Master_Combat_Object.h"

// .rdata vftable of RGE_Master_Combat_Object
// (battlegrounds_x2.lst: .rdata:0065AFF8 RGE_Master_Combat_Object__vftable)
static RGE_Master_Combat_ObjectVtbl* const rge_master_combat_object_vftable =
    (RGE_Master_Combat_ObjectVtbl*)0x0065AFF8;

// Replacement for RGE_Master_Combat_Object::setup
// Original: RGE_Master_Combat_Object__setup @ 0x00488D30 (idb_functions.h:1832).
// ABI: __thiscall — this_ in ECX, 1 stack arg (retn 4), int result (always 1).
// Derived override: base Action setup call (through the idb constant —
// routes into the replacement per the HOOK-ROUTING RULING), master_type
// forced to RGE_MASTER_TYPE_COMBAT_BASE (0x32), then the Combat
// extension fields: the two Armor/Weapon tables deep-copied
// (calloc_internal(count, 4) — sizeof(RGE_Armor_Weapon_Info) = {i16 type, i16
// value} = 4, verified; the asm bulk copy is the qmemcpy intrinsic,
// rep movsd + tail movsb) and the scalar block. Extension offsets
// 300-376 verified by the mechanical walker (calibrated on the base +
// Moving + Action chains). The IDA "v10 possibly undefined" base-call
// arg was a tracking artifact — the asm forwards the real other_object.
int __fastcall RGE_Master_Combat_Object__setup_new(
    RGE_Master_Combat_Object* this_, DWORD dummy,
    RGE_Master_Combat_Object* other_object)
{
    RGE_Master_Action_Object__setup((RGE_Master_Action_Object*)this_, (RGE_Master_Action_Object*)other_object);
    this_->master_type = RGE_MASTER_TYPE_COMBAT_BASE;
    this_->fight_sprite = other_object->fight_sprite;
    this_->base_armor = other_object->base_armor;
    this_->weapon_num = other_object->weapon_num;
    if (this_->weapon_num <= 0)
    {
        this_->weapon = 0;
    }
    else
    {
        this_->weapon = (RGE_Armor_Weapon_Info*)calloc_internal(this_->weapon_num, sizeof(RGE_Armor_Weapon_Info));
        memcpy(this_->weapon, other_object->weapon, this_->weapon_num * sizeof(RGE_Armor_Weapon_Info));
    }
    this_->armor_num = other_object->armor_num;
    if (this_->armor_num <= 0)
    {
        this_->armor = 0;
    }
    else
    {
        this_->armor = (RGE_Armor_Weapon_Info*)calloc_internal(this_->armor_num, sizeof(RGE_Armor_Weapon_Info));
        memcpy(this_->armor, other_object->armor, this_->armor_num * sizeof(RGE_Armor_Weapon_Info));
    }
    this_->defense_terrain_bonus = other_object->defense_terrain_bonus;
    this_->weapon_range = other_object->weapon_range;
    this_->area_effect_range = other_object->area_effect_range;
    this_->speed_of_attack = other_object->speed_of_attack;
    this_->missile_id = other_object->missile_id;
    this_->base_hit_chance = other_object->base_hit_chance;
    this_->break_off_combat = other_object->break_off_combat;
    this_->fire_missile_at_frame = other_object->fire_missile_at_frame;
    this_->weapon_offset_x = other_object->weapon_offset_x;
    this_->weapon_offset_y = other_object->weapon_offset_y;
    this_->weapon_offset_z = other_object->weapon_offset_z;
    this_->area_effect_level = other_object->area_effect_level;
    this_->minimum_weapon_range = other_object->minimum_weapon_range;
    this_->orig_armor = other_object->orig_armor;
    this_->orig_weapon = other_object->orig_weapon;
    this_->orig_weapon_range = other_object->orig_weapon_range;
    this_->orig_speed_of_attack = other_object->orig_speed_of_attack;
    this_->missed_missile_spread = other_object->missed_missile_spread;
    return 1;
}

// Replacement for RGE_Master_Combat_Object::setup2
// Original: RGE_Master_Combat_Object__setup2 @ 0x00488F00 (idb_functions.h:1833).
// ABI: __thiscall — this_ in ECX, 3 stack args (retn 0Ch), int result (always 1).
// Base call, master_type forced, then the stream sequence: base_armor,
// weapon_num + weapon table (calloc_internal(count, 4) + BULK rge_read of
// count * 4 bytes — asm push ecx after shl ecx,2), armor_num + armor
// table (same), then the scalar block (defense_terrain_bonus through
// minimum_weapon_range), the guarded fight_sprite resolution (the i16
// index read into the sounds PARAM SLOT — asm 0x488FFB; named local
// here; resolved against the SPRITES array), and the four orig_* reads.
int __fastcall RGE_Master_Combat_Object__setup2_new(
    RGE_Master_Combat_Object* this_, DWORD dummy,
    int infile, RGE_Sprite** sprites, RGE_Sound** sounds)
{
    RGE_Master_Action_Object__setup2((RGE_Master_Action_Object*)this_, infile, sprites, sounds);
    this_->master_type = RGE_MASTER_TYPE_COMBAT_BASE;
    rge_read(infile, &this_->base_armor, sizeof(this_->base_armor));
    rge_read(infile, &this_->weapon_num, sizeof(this_->weapon_num));
    if (this_->weapon_num <= 0)
    {
        this_->weapon = 0;
    }
    else
    {
        this_->weapon = (RGE_Armor_Weapon_Info*)calloc_internal(this_->weapon_num, sizeof(RGE_Armor_Weapon_Info));
        rge_read(infile, this_->weapon, this_->weapon_num * sizeof(RGE_Armor_Weapon_Info));
    }
    rge_read(infile, &this_->armor_num, sizeof(this_->armor_num));
    if (this_->armor_num <= 0)
    {
        this_->armor = 0;
    }
    else
    {
        this_->armor = (RGE_Armor_Weapon_Info*)calloc_internal(this_->armor_num, sizeof(RGE_Armor_Weapon_Info));
        rge_read(infile, this_->armor, this_->armor_num * sizeof(RGE_Armor_Weapon_Info));
    }
    rge_read(infile, &this_->defense_terrain_bonus, sizeof(this_->defense_terrain_bonus));
    rge_read(infile, &this_->weapon_range, sizeof(this_->weapon_range));
    rge_read(infile, &this_->area_effect_range, sizeof(this_->area_effect_range));
    rge_read(infile, &this_->speed_of_attack, sizeof(this_->speed_of_attack));
    rge_read(infile, &this_->missile_id, sizeof(this_->missile_id));
    rge_read(infile, &this_->base_hit_chance, sizeof(this_->base_hit_chance));
    rge_read(infile, &this_->break_off_combat, sizeof(this_->break_off_combat));
    rge_read(infile, &this_->fire_missile_at_frame, sizeof(this_->fire_missile_at_frame));
    rge_read(infile, &this_->weapon_offset_x, sizeof(this_->weapon_offset_x));
    rge_read(infile, &this_->weapon_offset_y, sizeof(this_->weapon_offset_y));
    rge_read(infile, &this_->weapon_offset_z, sizeof(this_->weapon_offset_z));
    rge_read(infile, &this_->area_effect_level, sizeof(this_->area_effect_level));
    rge_read(infile, &this_->minimum_weapon_range, sizeof(this_->minimum_weapon_range));
    rge_read(infile, &this_->missed_missile_spread, sizeof(this_->missed_missile_spread));
    __int16 fight_sprite_ix;
    rge_read(infile, &fight_sprite_ix, sizeof(fight_sprite_ix));
    this_->fight_sprite = (fight_sprite_ix < 0) ? 0 : sprites[fight_sprite_ix];
    rge_read(infile, &this_->orig_armor, sizeof(this_->orig_armor));
    rge_read(infile, &this_->orig_weapon, sizeof(this_->orig_weapon));
    rge_read(infile, &this_->orig_weapon_range, sizeof(this_->orig_weapon_range));
    rge_read(infile, &this_->orig_speed_of_attack, sizeof(this_->orig_speed_of_attack));
    return 1;
}

// Replacement for RGE_Master_Combat_Object::setup3
// Original: RGE_Master_Combat_Object__setup3 @ 0x00489110 (idb_functions.h:1834).
// ABI: __thiscall — this_ in ECX, 4 stack args (retn 10h), int result (always 1).
// Base call, master_type forced, then ONE fscanf with 18 conversions
// (asm 0x4891D1-0x489209): the %hd scratch targets are the in_id/
// infile/sounds PARAM SLOTS plus one local slot (original) — named
// locals here; fight_sprite index first, then base_armor/armor_num/
// weapon_num/defense_terrain_bonus direct, three %f fields, missile_id/
// base_hit_chance direct, break_off_combat/fire_missile_at_frame/
// area_effect_level scratches (byte-narrowed), weapon offsets (%f x3),
// minimum_weapon_range/missed_missile_spread (%f). Then the two armor/
// weapon table read loops ("%hd %hd" per 4-byte element) and the
// orig_* derivation chains in indexed form (no walked pointers):
// orig_armor = max value among type==4 armors from -1, falling back to
// base_armor then max over all armor values; orig_weapon = max value
// among type==4||type==3 weapons from 0 (the weapon null-check is
// loop-invariant and hoisted), falling back to max over all weapon
// values when the result is 0. Finally orig_weapon_range/
// orig_speed_of_attack mirror weapon_range/speed_of_attack.
int __fastcall RGE_Master_Combat_Object__setup3_new(
    RGE_Master_Combat_Object* this_, DWORD dummy,
    FILE* infile, RGE_Sprite** sprites, RGE_Sound** sounds, __int16 in_id)
{
    RGE_Master_Action_Object__setup3((RGE_Master_Action_Object*)this_, infile, sprites, sounds, in_id);
    this_->master_type = RGE_MASTER_TYPE_COMBAT_BASE;
    __int16 fight_sprite_ix;
    __int16 break_off_combat_s;
    __int16 fire_missile_at_frame_s;
    __int16 area_effect_level_s;
    fscanf_internal(infile, "%hd %hd %hd %hd %hd %f %f %f %hd %hd %hd %hd %f %f %f %hd %f %f",
        &fight_sprite_ix,
        &this_->base_armor,
        &this_->armor_num,
        &this_->weapon_num,
        &this_->defense_terrain_bonus,
        &this_->weapon_range,
        &this_->area_effect_range,
        &this_->speed_of_attack,
        &this_->missile_id,
        &this_->base_hit_chance,
        &break_off_combat_s,
        &fire_missile_at_frame_s,
        &this_->weapon_offset_x,
        &this_->weapon_offset_y,
        &this_->weapon_offset_z,
        &area_effect_level_s,
        &this_->minimum_weapon_range,
        &this_->missed_missile_spread);
    this_->fight_sprite = (fight_sprite_ix < 0) ? 0 : sprites[fight_sprite_ix];
    this_->area_effect_level = (unsigned __int8)area_effect_level_s;
    this_->break_off_combat = (unsigned __int8)break_off_combat_s;
    this_->fire_missile_at_frame = (unsigned __int8)fire_missile_at_frame_s;
    this_->orig_armor = -1;
    if (this_->armor)
    {
        for (int i = 0; i < this_->armor_num; ++i)
        {
            if (this_->armor[i].type == 4 && this_->armor[i].value > this_->orig_armor)
                this_->orig_armor = this_->armor[i].value;
        }
    }
    if (this_->orig_armor == -1)
    {
        this_->orig_armor = this_->base_armor;
        if (this_->armor && this_->armor_num > 0)
        {
            for (int j = 0; j < this_->armor_num; ++j)
            {
                if (this_->armor[j].value > this_->orig_armor)
                    this_->orig_armor = this_->armor[j].value;
            }
        }
    }
    this_->orig_weapon = 0;
    if (this_->weapon && this_->weapon_num > 0)
    {
        for (int k = 0; k < this_->weapon_num; ++k)
        {
            if ((this_->weapon[k].type == 4 || this_->weapon[k].type == 3)
                && this_->weapon[k].value > this_->orig_weapon)
                this_->orig_weapon = this_->weapon[k].value;
        }
        if (this_->orig_weapon == 0)
        {
            for (int m = 0; m < this_->weapon_num; ++m)
            {
                if (this_->weapon[m].value > this_->orig_weapon)
                    this_->orig_weapon = this_->weapon[m].value;
            }
        }
    }
    this_->orig_weapon_range = this_->weapon_range;
    this_->orig_speed_of_attack = this_->speed_of_attack;
    return 1;
}

// Replacement for RGE_Master_Combat_Object::setup4
// Original: RGE_Master_Combat_Object__setup4 @ 0x004893D0 (idb_functions.h:1835).
// ABI: __thiscall — this_ in ECX, 4 stack args (retn 10h), int result (always 1).
// Base call, then list[copy_id] (copy_id was stream-read by the base
// setup4; UNCONDITIONAL sign-extended index, no negative guard —
// faithful). BASE-POINTER CORRECTION per the template rule: the IDA
// v6[1].field accesses at +0x12C..+0x178 are the referenced master's
// Combat extension fields (offsets 300-376, walker-confirmed; mapping
// presented in the review round); the list element is re-typed
// RGE_Master_Combat_Object* accordingly. The twenty extension fields
// copy when the reference is non-null (NO table copies — the tables
// stream-read fresh); then the tail: base_armor, weapon_num + weapon
// bulk read, armor_num + armor bulk read, speed_of_attack, weapon_range,
// base_hit_chance, missile_id, defense_terrain_bonus, weapon_range
// AGAIN (overwrites), area_effect_range, minimum_weapon_range —
// later-write-wins order replicated.
int __fastcall RGE_Master_Combat_Object__setup4_new(
    RGE_Master_Combat_Object* this_, DWORD dummy,
    int infile, RGE_Master_Static_Object** list, RGE_Sprite** sprites, RGE_Sound** sounds)
{
    RGE_Master_Action_Object__setup4((RGE_Master_Action_Object*)this_, infile, list, sprites, sounds);
    RGE_Master_Combat_Object* other = (RGE_Master_Combat_Object*)list[this_->copy_id];
    this_->master_type = RGE_MASTER_TYPE_COMBAT_BASE;
    if (other)
    {
        this_->fight_sprite = other->fight_sprite;
        this_->base_armor = other->base_armor;
        this_->defense_terrain_bonus = other->defense_terrain_bonus;
        this_->weapon_range = other->weapon_range;
        this_->area_effect_range = other->area_effect_range;
        this_->speed_of_attack = other->speed_of_attack;
        this_->missile_id = other->missile_id;
        this_->base_hit_chance = other->base_hit_chance;
        this_->break_off_combat = other->break_off_combat;
        this_->fire_missile_at_frame = other->fire_missile_at_frame;
        this_->weapon_offset_x = other->weapon_offset_x;
        this_->weapon_offset_y = other->weapon_offset_y;
        this_->weapon_offset_z = other->weapon_offset_z;
        this_->area_effect_level = other->area_effect_level;
        this_->minimum_weapon_range = other->minimum_weapon_range;
        this_->orig_armor = other->orig_armor;
        this_->orig_weapon = other->orig_weapon;
        this_->orig_weapon_range = other->orig_weapon_range;
        this_->orig_speed_of_attack = other->orig_speed_of_attack;
        this_->missed_missile_spread = other->missed_missile_spread;
    }
    rge_read(infile, &this_->base_armor, sizeof(this_->base_armor));
    rge_read(infile, &this_->weapon_num, sizeof(this_->weapon_num));
    if (this_->weapon_num <= 0)
    {
        this_->weapon = 0;
    }
    else
    {
        this_->weapon = (RGE_Armor_Weapon_Info*)calloc_internal(this_->weapon_num, sizeof(RGE_Armor_Weapon_Info));
        rge_read(infile, this_->weapon, this_->weapon_num * sizeof(RGE_Armor_Weapon_Info));
    }
    rge_read(infile, &this_->armor_num, sizeof(this_->armor_num));
    if (this_->armor_num <= 0)
    {
        this_->armor = 0;
    }
    else
    {
        this_->armor = (RGE_Armor_Weapon_Info*)calloc_internal(this_->armor_num, sizeof(RGE_Armor_Weapon_Info));
        rge_read(infile, this_->armor, this_->armor_num * sizeof(RGE_Armor_Weapon_Info));
    }
    rge_read(infile, &this_->speed_of_attack, sizeof(this_->speed_of_attack));
    rge_read(infile, &this_->weapon_range, sizeof(this_->weapon_range));
    rge_read(infile, &this_->base_hit_chance, sizeof(this_->base_hit_chance));
    rge_read(infile, &this_->missile_id, sizeof(this_->missile_id));
    rge_read(infile, &this_->defense_terrain_bonus, sizeof(this_->defense_terrain_bonus));
    rge_read(infile, &this_->weapon_range, sizeof(this_->weapon_range));
    rge_read(infile, &this_->area_effect_range, sizeof(this_->area_effect_range));
    rge_read(infile, &this_->minimum_weapon_range, sizeof(this_->minimum_weapon_range));
    return 1;
}

// Replacement for RGE_Master_Combat_Object::RGE_Master_Combat_Object2
// Original: RGE_Master_Combat_Object__RGE_Master_Combat_Object2 @ 0x00488BC0
// (idb_functions.h:1829). ABI: __thiscall — this_ in ECX, 4 stack args
// (retn 10h), returns this. SEH ctor-unwind frame folded per the
// make_new_obj pattern: on exception the ACTION destructor runs on this
// (the immediate base — its dtor chains through Moving/Animated to
// Static) and the exception propagates (asm unwind chunk 0x6420F0:
// mov ecx, [ebp-10h] / jmp RGE_Master_Action_Object__destructor;
// handler = __CxxFrameHandler dispatch, unwind-only scope). Base Action
// ctor2 called with do_setup = 0. The Combat extension has no ctor
// defaults — the tables and scalars are the setups' job; if (do_setup)
// the derived setup (Combat::setup2 — loads the Armor/Weapon tables).
// The if (do_setup) is load-bearing (no guard collapse).
RGE_Master_Combat_Object* __fastcall RGE_Master_Combat_Object__RGE_Master_Combat_Object2_new(
    RGE_Master_Combat_Object* this_, DWORD dummy,
    int infile, RGE_Sprite** sprites, RGE_Sound** sounds, int do_setup)
{
    RGE_Master_Combat_Object* result = this_;
    __try
    {
        RGE_Master_Action_Object__RGE_Master_Action_Object2((RGE_Master_Action_Object*)this_, infile, sprites, sounds, 0);
        this_->vfptr = rge_master_combat_object_vftable;
        if (do_setup)
            RGE_Master_Combat_Object__setup2(this_, infile, sprites, sounds);
        result = this_;
    }
    __except (RGE_Master_Action_Object__destructor((RGE_Master_Action_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Combat_Object::RGE_Master_Combat_Object3
// Original: RGE_Master_Combat_Object__RGE_Master_Combat_Object3 @ 0x00488C30
// (idb_functions.h:1830). ABI: __thiscall — this_ in ECX, 5 stack args
// (retn 14h), returns this. Same SEH fold (unwind chunk 0x642110, same
// Action-destructor action). Base Action ctor3 with do_setup = 0;
// if (do_setup) Combat::setup3 (text format incl. the orig_* derivation
// chains).
RGE_Master_Combat_Object* __fastcall RGE_Master_Combat_Object__RGE_Master_Combat_Object3_new(
    RGE_Master_Combat_Object* this_, DWORD dummy,
    FILE* infile, RGE_Sprite** sprites, RGE_Sound** sounds, __int16 in_id, int do_setup)
{
    RGE_Master_Combat_Object* result = this_;
    __try
    {
        RGE_Master_Action_Object__RGE_Master_Action_Object3((RGE_Master_Action_Object*)this_, infile, sprites, sounds, in_id, 0);
        this_->vfptr = rge_master_combat_object_vftable;
        if (do_setup)
            RGE_Master_Combat_Object__setup3(this_, infile, sprites, sounds, in_id);
        result = this_;
    }
    __except (RGE_Master_Action_Object__destructor((RGE_Master_Action_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Combat_Object::RGE_Master_Combat_Object4
// Original: RGE_Master_Combat_Object__RGE_Master_Combat_Object4 @ 0x00488CB0
// (idb_functions.h:1831). ABI: __thiscall — this_ in ECX, 5 stack args
// (retn 14h), returns this. Same SEH fold (unwind chunk 0x642130,
// confirmed: jmp RGE_Master_Action_Object__destructor). Base Action
// ctor4 with do_setup = 0; if (do_setup) Combat::setup4.
// Caller coverage: RGE_Master_Missile_Object2/3/4 and
// Player__load_master_object/load_object call these ctors — the
// Missile tier routes through these hooks.
RGE_Master_Combat_Object* __fastcall RGE_Master_Combat_Object__RGE_Master_Combat_Object4_new(
    RGE_Master_Combat_Object* this_, DWORD dummy,
    int infile, RGE_Master_Static_Object** list, RGE_Sprite** sprites, RGE_Sound** sounds, int do_setup)
{
    RGE_Master_Combat_Object* result = this_;
    __try
    {
        RGE_Master_Action_Object__RGE_Master_Action_Object4((RGE_Master_Action_Object*)this_, infile, list, sprites, sounds, 0);
        this_->vfptr = rge_master_combat_object_vftable;
        if (do_setup)
            RGE_Master_Combat_Object__setup4(this_, infile, list, sprites, sounds);
        result = this_;
    }
    __except (RGE_Master_Action_Object__destructor((RGE_Master_Action_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Combat_Object::destructor
// Original: RGE_Master_Combat_Object__destructor @ 0x00489630 (idb_functions.h:1836).
// ABI: __thiscall — this_ in ECX, no stack args, void return, no dummy.
// Combat vftable store FIRST (asm order), then the Armor/Weapon table
// frees with free_internal (the asm calls the game's _free — heap
// pairing per the CRT rule; the tables were allocated with
// calloc_internal), pointers zeroed, then the base Action destructor
// (called, not tail-jmp — it uses an esi frame).
void __fastcall RGE_Master_Combat_Object__destructor_new(RGE_Master_Combat_Object* this_)
{
    this_->vfptr = rge_master_combat_object_vftable;
    if (this_->weapon)
    {
        free_internal(this_->weapon);
        this_->weapon = 0;
    }
    if (this_->armor)
    {
        free_internal(this_->armor);
        this_->armor = 0;
    }
    RGE_Master_Action_Object__destructor((RGE_Master_Action_Object*)this_);
}

// Replacement for RGE_Master_Combat_Object::vector_deleting_destructor
// Original: RGE_Master_Combat_Object__vector_deleting_destructor @ 0x00488BA0.
// ABI: __thiscall — this_ in ECX, 1 stack arg (retn 4), void* result (this).
// Destructor + operator_delete_internal on flags & 1, returning this.
void* __fastcall RGE_Master_Combat_Object__vector_deleting_destructor_new(
    RGE_Master_Combat_Object* this_, DWORD dummy, unsigned int flags)
{
    RGE_Master_Combat_Object__destructor_new(this_);
    if ((flags & 1) != 0)
        operator_delete_internal(this_);
    return this_;
}

// Replacement for RGE_Master_Combat_Object::make_new_obj
// Original: RGE_Master_Combat_Object__make_new_obj @ 0x00489680
// (idb_functions.h:1837). ABI: __thiscall — this_ in ECX, 4 stack args
// (retn 10h), RGE_Static_Object* result.
// Same factory ladder: recyclable @188 (asm 0x48969E), recycle via
// owner->world @0x8C (asm 0x4896AB), guarded dispatch slot @0x74 (asm
// 0x4896D2), else operator_new(0x17C) + the RGE_Combat_Object runtime
// ctor (do_setup = 1; asm 0x48971D).
// ALLOCATION VERIFIED: sizeof(RGE_Combat_Object) = 380 = 0x17C —
// sequential walk over idb_types.h:7318-7406 (the RGE_Action_Object
// prefix through Selected_Group_Info = 324; combat tail VUR_Ptrs[9] x4
// = 36 @324, Unified_Map_Value @360, Multi_Unified_Points/Large_Object_
// Radius @364-365, attack_count @368, attack_timer @372, four u8 @376).
// SEH fold per the make_new_obj precedent (FUNCTION CHUNK 0x642150,
// same operator_delete shape).
RGE_Static_Object* __fastcall RGE_Master_Combat_Object__make_new_obj_new(
    RGE_Master_Combat_Object* this_, DWORD dummy,
    RGE_Player* owner, float x, float y, float z)
{
    RGE_Static_Object* result = 0;
    RGE_Combat_Object* new_obj = 0;
    __try
    {
        if (this_->recyclable)
        {
            RGE_Static_Object* recycled =
                RGE_Game_World__recycle_object_in_to_game(owner->world, this_->master_type);
            if (recycled)
            {
                recycled->recycle_in_to_game((RGE_Master_Static_Object*)this_, owner, x, y, z);
                result = recycled;
            }
        }
        if (!result)
        {
            new_obj = (RGE_Combat_Object*)operator_new_internal(sizeof(RGE_Combat_Object));
            if (new_obj)
                result = (RGE_Static_Object*)RGE_Combat_Object__RGE_Combat_Object(new_obj, this_, owner, x, y, z, 1);
        }
    }
    __except (operator_delete_internal(new_obj), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Combat_Object::make_new_master
// Original: RGE_Master_Combat_Object__make_new_master @ 0x00489750
// (idb_functions.h:1838). ABI: __thiscall — this_ in ECX, no stack args,
// RGE_Master_Static_Object* result.
// Master-template factory: operator_new(0x17C) — sizeof(RGE_Master_
// Combat_Object) = 380 = 0x17C walker-verified — null-checked, then the
// base Action COPY-CTOR with do_setup = 0 (asm 0x48978B), the Combat
// vfptr store (asm 0x489798), and the DERIVED setup (Combat::setup —
// asm 0x48979E; hooked -> routes into our replacement; it sets
// master_type = 50 and copies/deep-copies the whole Combat extension
// from this). NO master_type store and NO extension copies here (the
// asm has none between the vfptr store and the return).
// SEH fold per the make_new_master precedent (FUNCTION CHUNK 0x642170;
// its second entry 0x64217B jmps RGE_Master_Action_Object__destructor —
// the immediate base level; folds into the same filter).
RGE_Master_Static_Object* __fastcall RGE_Master_Combat_Object__make_new_master_new(
    RGE_Master_Combat_Object* this_)
{
    RGE_Master_Combat_Object* new_master =
        (RGE_Master_Combat_Object*)operator_new_internal(sizeof(RGE_Master_Combat_Object));
    RGE_Master_Static_Object* result = 0;
    __try
    {
        if (new_master)
        {
            RGE_Master_Action_Object__RGE_Master_Action_Object(
                (RGE_Master_Action_Object*)new_master, (RGE_Master_Action_Object*)this_, 0);
            new_master->vfptr = rge_master_combat_object_vftable;
            RGE_Master_Combat_Object__setup(new_master, this_);
            result = (RGE_Master_Static_Object*)new_master;
        }
    }
    __except (operator_delete_internal(new_master), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Combat_Object::copy_obj
// Original: RGE_Master_Combat_Object__copy_obj @ 0x004897C0
// (idb_functions.h:1839). ABI: __thiscall — this_ in ECX, 1 stack arg
// (retn 4), void return. No SEH frame (plain function).
// Base Action copy_obj call (idb constant -> our hooked Action copy_obj
// replacement per the HOOK-ROUTING RULING; the asm's own XREF shows the
// Missile tier calls through here too), then the Combat extension:
// BASE-POINTER CORRECTION per the template rule — the IDA renderings
// read source through RGE_Master_Static_Object* (source[1].button_pict/
// portrait_pict/center_tile_req1/available/tile_req1/construction_
// radius_y/elevation_flag/movement_type/attribute_amount_held[0..2]/
// attribute_max_amount/attribute_rot/multiple_attribute_mod/attribute_
// type_held[1]/attribute_flag/combat_level/map_draw_level/map_color/
// damage_sprites/occlusion_flag at Static offsets 92/96/104/98/100/112/
// 116/120/128/132/136/140/144/148/152/124/156/160/164/168 and on) —
// the byte offsets land on the Combat extension fields @300-376
// (walker-confirmed), so the source is re-typed RGE_Master_Combat_
// Object*. WIDTH RESOLUTIONS from the asm: missile_id is a WORD move
// (0x4898F7-0x4898FE; the pseudocode's float rendering was a
// mis-typing), area_effect_level is a BYTE move stored out of order
// after the weapon offsets (asm 0x489951), and the Armor/Weapon tables
// are freed + reallocated + deep-copied (free_internal /
// calloc_internal(count, sizeof(RGE_Armor_Weapon_Info) = 4) / memcpy —
// the asm bulk copy is the qmemcpy intrinsic, rep movsd + zero-tail
// movsb, 0x489817-0x489849 and 0x489884-0x4898B6).
void __fastcall RGE_Master_Combat_Object__copy_obj_new(
    RGE_Master_Combat_Object* this_, DWORD dummy,
    RGE_Master_Static_Object* source)
{
    RGE_Master_Action_Object__copy_obj((RGE_Master_Action_Object*)this_, source);
    RGE_Master_Combat_Object* src = (RGE_Master_Combat_Object*)source;
    this_->fight_sprite = src->fight_sprite;
    this_->base_armor = src->base_armor;
    this_->weapon_num = src->weapon_num;
    if (this_->weapon)
    {
        free_internal(this_->weapon);
    }
    if (this_->weapon_num <= 0)
    {
        this_->weapon = 0;
    }
    else
    {
        this_->weapon = (RGE_Armor_Weapon_Info*)calloc_internal(this_->weapon_num, sizeof(RGE_Armor_Weapon_Info));
        memcpy(this_->weapon, src->weapon, this_->weapon_num * sizeof(RGE_Armor_Weapon_Info));
    }
    this_->armor_num = src->armor_num;
    if (this_->armor)
    {
        free_internal(this_->armor);
    }
    if (this_->armor_num <= 0)
    {
        this_->armor = 0;
    }
    else
    {
        this_->armor = (RGE_Armor_Weapon_Info*)calloc_internal(this_->armor_num, sizeof(RGE_Armor_Weapon_Info));
        memcpy(this_->armor, src->armor, this_->armor_num * sizeof(RGE_Armor_Weapon_Info));
    }
    this_->defense_terrain_bonus = src->defense_terrain_bonus;
    this_->weapon_range = src->weapon_range;
    this_->area_effect_range = src->area_effect_range;
    this_->speed_of_attack = src->speed_of_attack;
    this_->missile_id = src->missile_id;
    this_->base_hit_chance = src->base_hit_chance;
    this_->break_off_combat = src->break_off_combat;
    this_->fire_missile_at_frame = src->fire_missile_at_frame;
    this_->weapon_offset_x = src->weapon_offset_x;
    this_->weapon_offset_y = src->weapon_offset_y;
    this_->weapon_offset_z = src->weapon_offset_z;
    // asm stores area_effect_level here, after the weapon offsets
    // (0x489951), out of the pseudocode's listed order
    this_->area_effect_level = src->area_effect_level;
    this_->minimum_weapon_range = src->minimum_weapon_range;
    this_->orig_armor = src->orig_armor;
    this_->orig_weapon = src->orig_weapon;
    this_->orig_weapon_range = src->orig_weapon_range;
    this_->orig_speed_of_attack = src->orig_speed_of_attack;
    this_->missed_missile_spread = src->missed_missile_spread;
}

// Replacement for RGE_Master_Combat_Object::modify
// Original: @ 0x004899B0 (idb_functions.h:1840). __thiscall, retn 8,
// void. No SEH frame (plain function). Switch restored from the jump
// table off_489AD4 (@0x489AD4, 15 entries over flag-8, flags 8..22;
// 13/14/17/19/21 -> default). All int conversions are __ftol (template
// rule); float fields take the raw amount dword; i16 fields take ftol's
// LOW WORD as a SET.
void __fastcall RGE_Master_Combat_Object__modify_new(
    RGE_Master_Combat_Object* this_, DWORD dummy,
    float amount, unsigned __int8 flag)
{
    switch (flag)
    {
    case 8:
    {
        int packed = (int)amount;
        if (this_->armor)
        {
            unsigned int index = (unsigned int)((packed >> 8) & 0xFF); // asm 0x4899E4 mov cl,ah
            if ((int)index < (int)this_->armor_num)                    // asm 0x489A23 cmp/jge
                this_->armor[index].value = (unsigned __int16)(unsigned __int8)packed; // 0x489A2B-0x489A31
        }
        break;
    }
    case 9:
    {
        int packed = (int)amount;
        if (this_->weapon)
        {
            unsigned int index = (unsigned int)((packed >> 8) & 0xFF); // asm 0x489A0E
            if ((int)index < (int)this_->weapon_num)
                this_->weapon[index].value = (unsigned __int16)(unsigned __int8)packed; // 0x489A31 shape
        }
        break;
    }
    case 10:
        this_->speed_of_attack = amount;  // raw dword, asm 0x489A37-0x489A3D
        break;
    case 11:
        this_->base_hit_chance = (__int16)(int)amount;  // ftol + word store, asm 0x489A47-0x489A50
        break;
    case 12:
        this_->weapon_range = amount;  // asm 0x489A54-0x489A5B
        break;
    case 15:
        this_->base_armor = (__int16)(int)amount;  // asm 0x489A69-0x489A72
        break;
    case 16:
        this_->missile_id = (__int16)(int)amount;  // asm 0x489A7D-0x489A86
        break;
    case 18:
        this_->defense_terrain_bonus = (__int16)(int)amount;  // asm 0x489A91-0x489A9A
        break;
    case 20:
        this_->minimum_weapon_range = amount;  // asm 0x489AA5-0x489AA9
        break;
    case 22:
        this_->area_effect_range = amount;  // asm 0x489AB3-0x489AB7
        break;
    default:
        RGE_Master_Action_Object__modify((RGE_Master_Action_Object*)this_, amount, flag);  // asm 0x489AC9
        break;
    }
}

// Replacement for RGE_Master_Combat_Object::modify_delta
// Original: @ 0x00489B10 (idb_functions.h:1841). Same ABI. Switch from
// off_489CDC (@0x489CDC, 17 entries over flag-6). Flags 8/9 SEARCH the
// table by .type == BYTE1(ftol amount) (asm 0x489B6C loop, movsx type vs
// zero-extended byte; not-found -> function return 0x489B81), then
// value += (u8)BYTE0 (16-bit add, asm 0x489BDC-0x489BE7). Flag 18 is a
// SET (mov, asm 0x489C8A) — NOT an add. i16 adds are 16-bit adds of
// ftol's low word (asm 0x489C2E/0x489C5C/0x489C73).
void __fastcall RGE_Master_Combat_Object__modify_delta_new(
    RGE_Master_Combat_Object* this_, DWORD dummy,
    float amount, unsigned __int8 flag)
{
    switch (flag)
    {
    case 6:
        this_->turn_speed = amount + this_->turn_speed;  // asm 0x489BF7-0x489C01
        break;
    case 8:
    {
        int packed = (int)amount;
        RGE_Armor_Weapon_Info* table = this_->armor;
        if (table && this_->armor_num > 0)
        {
            unsigned __int8 type = (unsigned __int8)((packed >> 8) & 0xFF);
            for (int i = 0; i < this_->armor_num; ++i)
            {
                if (table[i].type == type)
                {
                    table[i].value = (__int16)(table[i].value + (unsigned __int8)packed);
                    break;
                }
            }
        }
        break;
    }
    case 9:
    {
        int packed = (int)amount;
        RGE_Armor_Weapon_Info* table = this_->weapon;
        if (table && this_->weapon_num > 0)
        {
            unsigned __int8 type = (unsigned __int8)((packed >> 8) & 0xFF);
            for (int i = 0; i < this_->weapon_num; ++i)
            {
                if (table[i].type == type)
                {
                    table[i].value = (__int16)(table[i].value + (unsigned __int8)packed);
                    break;
                }
            }
        }
        break;
    }
    case 10:
        this_->speed_of_attack = amount + this_->speed_of_attack;  // asm 0x489C0E-0x489C19
        break;
    case 11:
        this_->base_hit_chance = (__int16)(this_->base_hit_chance + (__int16)(int)amount);  // asm 0x489C25-0x489C2E
        break;
    case 12:
        this_->weapon_range = amount + this_->weapon_range;  // asm 0x489C3C-0x489C47
        break;
    case 15:
        this_->base_armor = (__int16)(this_->base_armor + (__int16)(int)amount);  // asm 0x489C53-0x489C5C
        break;
    case 16:
        this_->missile_id = (__int16)(this_->missile_id + (__int16)(int)amount);  // asm 0x489C6A-0x489C73
        break;
    case 18:
        this_->defense_terrain_bonus = (__int16)(int)amount;  // SET — asm 0x489C81-0x489C8A
        break;
    case 20:
        this_->minimum_weapon_range = amount + this_->minimum_weapon_range;  // asm 0x489C98-0x489CA3
        break;
    case 22:
        this_->area_effect_range = amount + this_->area_effect_range;  // asm 0x489CAF-0x489CBA
        break;
    default:
        RGE_Master_Action_Object__modify_delta((RGE_Master_Action_Object*)this_, amount, flag);  // asm 0x489CCE
        break;
    }
}

// Replacement for RGE_Master_Combat_Object::modify_percent
// Original: @ 0x00489D20 (idb_functions.h:1842). Same ABI. Switch from
// off_489F90 (@0x489F90, 17 entries over flag-6). Flags 8/9: by-TYPE
// search, then value = (i16)(int)(BYTE0(ftol a) * 0.01f * old + 0.5f) —
// x87 sequence fild/fmul flt_6565FC/fild/fmulp/fadd const_f_half/ftol/
// word store (asm 0x489DA3-0x489E4F; flt_6565FC = float32(0.01) per the
// pseudocode rendering 0.0099999998, const_f_half = 0.5f; all operands
// exact in double). The i16 fields use the RAW amount as the multiplier
// (asm 0x489E96 fmul [esp+amount] — no 0.01f factor), with +0.5f
// rounding.
void __fastcall RGE_Master_Combat_Object__modify_percent_new(
    RGE_Master_Combat_Object* this_, DWORD dummy,
    float amount, unsigned __int8 flag)
{
    switch (flag)
    {
    case 6:
        this_->turn_speed = amount * this_->turn_speed;  // asm 0x489E59-0x489E64
        break;
    case 8:
    {
        int packed = (int)amount;
        RGE_Armor_Weapon_Info* table = this_->armor;
        if (table && this_->armor_num > 0)
        {
            unsigned __int8 type = (unsigned __int8)((packed >> 8) & 0xFF);
            for (int i = 0; i < this_->armor_num; ++i)
            {
                if (table[i].type == type)
                {
                    double percent = (double)(unsigned __int8)packed * 0.01f;
                    double old = (double)table[i].value;
                    table[i].value = (__int16)(int)(percent * old + 0.5f);
                    break;
                }
            }
        }
        break;
    }
    case 9:
    {
        int packed = (int)amount;
        RGE_Armor_Weapon_Info* table = this_->weapon;
        if (table && this_->weapon_num > 0)
        {
            unsigned __int8 type = (unsigned __int8)((packed >> 8) & 0xFF);
            for (int i = 0; i < this_->weapon_num; ++i)
            {
                if (table[i].type == type)
                {
                    double percent = (double)(unsigned __int8)packed * 0.01f;
                    double old = (double)table[i].value;
                    table[i].value = (__int16)(int)(percent * old + 0.5f);
                    break;
                }
            }
        }
        break;
    }
    case 10:
        this_->speed_of_attack = amount * this_->speed_of_attack;  // asm 0x489E70-0x489E7B
        break;
    case 11:
        this_->base_hit_chance = (__int16)(int)((double)this_->base_hit_chance * amount + 0.5f);  // asm 0x489E87-0x489EA5
        break;
    case 12:
        this_->weapon_range = amount * this_->weapon_range;  // asm 0x489EB3-0x489EBE
        break;
    case 15:
        this_->base_armor = (__int16)(int)((double)this_->base_armor * amount + 0.5f);  // asm 0x489ECA-0x489EE8
        break;
    case 16:
        this_->missile_id = (__int16)(int)((double)this_->missile_id * amount + 0.5f);  // asm 0x489EF6-0x489F14
        break;
    case 18:
        this_->defense_terrain_bonus = (__int16)(int)((double)this_->defense_terrain_bonus * amount + 0.5f);  // asm 0x489F22-0x489F40
        break;
    case 20:
        this_->minimum_weapon_range = amount * this_->minimum_weapon_range;  // asm 0x489F4E-0x489F59
        break;
    case 22:
        this_->area_effect_range = amount * this_->area_effect_range;  // asm 0x489F65-0x489F70
        break;
    default:
        RGE_Master_Action_Object__modify_percent((RGE_Master_Action_Object*)this_, amount, flag);  // asm 0x489F84
        break;
    }
}

// Replacement for RGE_Master_Combat_Object::small_save
// Original: @ 0x00489FE0 (idb_functions.h:1843). __thiscall, 1 stack arg
// (retn 4), void. No SEH. Base Action small_save (hooked -> routes),
// then: base_armor w2, weapon_num w2, WEAPON TABLE if weapon_num > 0
// (SIGNED count guard, asm 0x48A012 jle; size = count*4, asm movsx+shl 2),
// armor_num w2, ARMOR TABLE if armor_num > 0, speed_of_attack w4,
// weapon_range w4, base_hit_chance w2, missile_id w2,
// defense_terrain_bonus w2, weapon_range w4 AGAIN (asm 0x48A0A5 —
// later-write-wins), area_effect_range w4, minimum_weapon_range w4.
// NOTE: the table guards are COUNT-only here — no pointer check (the
// original would stream from a null pointer on count>0/null, faithful).
void __fastcall RGE_Master_Combat_Object__small_save_new(
    RGE_Master_Combat_Object* this_, DWORD dummy, int outfile)
{
    RGE_Master_Action_Object__small_save((RGE_Master_Action_Object*)this_, outfile);
    rge_write(outfile, &this_->base_armor, sizeof(this_->base_armor));
    rge_write(outfile, &this_->weapon_num, sizeof(this_->weapon_num));
    if (this_->weapon_num > 0)
        rge_write(outfile, this_->weapon, this_->weapon_num * sizeof(RGE_Armor_Weapon_Info));
    rge_write(outfile, &this_->armor_num, sizeof(this_->armor_num));
    if (this_->armor_num > 0)
        rge_write(outfile, this_->armor, this_->armor_num * sizeof(RGE_Armor_Weapon_Info));
    rge_write(outfile, &this_->speed_of_attack, sizeof(this_->speed_of_attack));
    rge_write(outfile, &this_->weapon_range, sizeof(this_->weapon_range));
    rge_write(outfile, &this_->base_hit_chance, sizeof(this_->base_hit_chance));
    rge_write(outfile, &this_->missile_id, sizeof(this_->missile_id));
    rge_write(outfile, &this_->defense_terrain_bonus, sizeof(this_->defense_terrain_bonus));
    rge_write(outfile, &this_->weapon_range, sizeof(this_->weapon_range));
    rge_write(outfile, &this_->area_effect_range, sizeof(this_->area_effect_range));
    rge_write(outfile, &this_->minimum_weapon_range, sizeof(this_->minimum_weapon_range));
}

// Replacement for RGE_Master_Combat_Object::save
// Original: @ 0x0048A0E0 (idb_functions.h:1844). Same ABI. Base Action
// save, then fight_sprite @300 -> RGE_Sprite::id @+0x82 (asm 0x48A0F9)
// or -1 (asm 0x48A500), written late (w2 at 0x48A243). Table guards are
// POINTER-only here (asm 0x48A134/0x48A15D jz — no count guard).
// 24 writes in asm order; area_effect_level lands AFTER the weapon
// offsets (asm 0x48A214).
void __fastcall RGE_Master_Combat_Object__save_new(
    RGE_Master_Combat_Object* this_, DWORD dummy, int outfile)
{
    __int16 fight_id = -1;
    RGE_Master_Action_Object__save((RGE_Master_Action_Object*)this_, outfile);
    if (this_->fight_sprite)
        fight_id = this_->fight_sprite->id;
    else
        fight_id = -1;
    rge_write(outfile, &this_->base_armor, sizeof(this_->base_armor));
    rge_write(outfile, &this_->weapon_num, sizeof(this_->weapon_num));
    if (this_->weapon)
        rge_write(outfile, this_->weapon, this_->weapon_num * sizeof(RGE_Armor_Weapon_Info));
    rge_write(outfile, &this_->armor_num, sizeof(this_->armor_num));
    if (this_->armor)
        rge_write(outfile, this_->armor, this_->armor_num * sizeof(RGE_Armor_Weapon_Info));
    rge_write(outfile, &this_->defense_terrain_bonus, sizeof(this_->defense_terrain_bonus));
    rge_write(outfile, &this_->weapon_range, sizeof(this_->weapon_range));
    rge_write(outfile, &this_->area_effect_range, sizeof(this_->area_effect_range));
    rge_write(outfile, &this_->speed_of_attack, sizeof(this_->speed_of_attack));
    rge_write(outfile, &this_->missile_id, sizeof(this_->missile_id));
    rge_write(outfile, &this_->base_hit_chance, sizeof(this_->base_hit_chance));
    rge_write(outfile, &this_->break_off_combat, sizeof(this_->break_off_combat));
    rge_write(outfile, &this_->fire_missile_at_frame, sizeof(this_->fire_missile_at_frame));
    rge_write(outfile, &this_->weapon_offset_x, sizeof(this_->weapon_offset_x));
    rge_write(outfile, &this_->weapon_offset_y, sizeof(this_->weapon_offset_y));
    rge_write(outfile, &this_->weapon_offset_z, sizeof(this_->weapon_offset_z));
    // asm stores area_effect_level here, after the weapon offsets (0x48A214)
    rge_write(outfile, &this_->area_effect_level, sizeof(this_->area_effect_level));
    rge_write(outfile, &this_->minimum_weapon_range, sizeof(this_->minimum_weapon_range));
    rge_write(outfile, &this_->missed_missile_spread, sizeof(this_->missed_missile_spread));
    rge_write(outfile, &fight_id, sizeof(fight_id));
    rge_write(outfile, &this_->orig_armor, sizeof(this_->orig_armor));
    rge_write(outfile, &this_->orig_weapon, sizeof(this_->orig_weapon));
    rge_write(outfile, &this_->orig_weapon_range, sizeof(this_->orig_weapon_range));
    rge_write(outfile, &this_->orig_speed_of_attack, sizeof(this_->orig_speed_of_attack));
}

void install_RGE_Master_Combat_Object()
{
    setHook((void*)0x00488D30, RGE_Master_Combat_Object__setup_new);
    setHook((void*)0x00488F00, RGE_Master_Combat_Object__setup2_new);
    setHook((void*)0x00489110, RGE_Master_Combat_Object__setup3_new);
    setHook((void*)0x004893D0, RGE_Master_Combat_Object__setup4_new);
    setHook((void*)0x00488BC0, RGE_Master_Combat_Object__RGE_Master_Combat_Object2_new);
    setHook((void*)0x00488C30, RGE_Master_Combat_Object__RGE_Master_Combat_Object3_new);
    setHook((void*)0x00488CB0, RGE_Master_Combat_Object__RGE_Master_Combat_Object4_new);
    setHook((void*)0x00489630, RGE_Master_Combat_Object__destructor_new);
    setHook((void*)0x00488BA0, RGE_Master_Combat_Object__vector_deleting_destructor_new);
    setHook((void*)0x00489680, RGE_Master_Combat_Object__make_new_obj_new);
    setHook((void*)0x00489750, RGE_Master_Combat_Object__make_new_master_new);
    setHook((void*)0x004897C0, RGE_Master_Combat_Object__copy_obj_new);
    setHook((void*)0x004899B0, RGE_Master_Combat_Object__modify_new);
    setHook((void*)0x00489B10, RGE_Master_Combat_Object__modify_delta_new);
    setHook((void*)0x00489D20, RGE_Master_Combat_Object__modify_percent_new);
    setHook((void*)0x00489FE0, RGE_Master_Combat_Object__small_save_new);
    setHook((void*)0x0048A0E0, RGE_Master_Combat_Object__save_new);
}
