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
}
