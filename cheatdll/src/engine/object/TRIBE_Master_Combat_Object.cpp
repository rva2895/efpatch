#include "stdafx.h"
#include "TRIBE_Master_Combat_Object.h"

// .rdata vftable of TRIBE_Master_Combat_Object
// (battlegrounds_x2.lst: .rdata:0066738C TRIBE_Master_Combat_Object__vftable)
static TRIBE_Master_Combat_ObjectVtbl* const tribe_master_combat_object_vftable =
    (TRIBE_Master_Combat_ObjectVtbl*)0x0066738C;

// TRIBE_Task_List vftable (.rdata:00667414) — the subtype marker create_task_list
// stores so the game can distinguish TRIBE task lists from plain RGE ones
static RGE_Task_ListVtbl* const tribe_task_list_vftable =
    (RGE_Task_ListVtbl*)0x00667414;

// Replacement for TRIBE_Master_Combat_Object::TRIBE_Master_Combat_Object
// (the copy constructor)
// Original: @ 0x005C8340 (idb_functions.h:5120). ABI: __thiscall — this_
// in ECX, 2 stack args (retn 8), returns this. SEH ctor-unwind frame
// folded per the make_new_obj pattern: on exception the base RGE Master
// Combat destructor runs on this and the exception propagates (asm
// unwind chunk 0x6466A0: mov ecx, [ebp-10h] / jmp
// RGE_Master_Combat_Object__destructor; handler = __CxxFrameHandler
// dispatch, unwind-only scope). Body: base RGE Combat ctor1 with
// do_setup = 0 (asm 0x5C8365), the TRIBE vfptr store (asm 0x5C8378),
// if (do_setup) the derived setup (asm 0x5C8383 — load-bearing guard,
// no collapse). Caller coverage: TRIBE_Master_Building_Object__make_
// new_master calls this ctor directly (the Building tier builds on it).
TRIBE_Master_Combat_Object* __fastcall TRIBE_Master_Combat_Object__TRIBE_Master_Combat_Object_new(
    TRIBE_Master_Combat_Object* this_, DWORD dummy,
    TRIBE_Master_Combat_Object* other_object, int do_setup)
{
    TRIBE_Master_Combat_Object* result = this_;
    __try
    {
        RGE_Master_Combat_Object__RGE_Master_Combat_Object(
            (RGE_Master_Combat_Object*)this_, (RGE_Master_Combat_Object*)other_object, 0);
        this_->vfptr = tribe_master_combat_object_vftable;
        if (do_setup)
            TRIBE_Master_Combat_Object__setup(this_, other_object);
        result = this_;
    }
    __except (RGE_Master_Combat_Object__destructor((RGE_Master_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for TRIBE_Master_Combat_Object::scalar_deleting_destructor
// Original: @ 0x005C83A0 (idb_functions.h:5121). ABI: __thiscall — this_
// in ECX, 1 stack arg (retn 4), void* result (this). Destructor + the
// game-side operator_delete on flags & 1, returning this (operator_
// delete_internal wrapper per the Static scalar_deleting precedent).
void* __fastcall TRIBE_Master_Combat_Object__scalar_deleting_destructor_new(
    TRIBE_Master_Combat_Object* this_, DWORD dummy, unsigned int flags)
{
    TRIBE_Master_Combat_Object__destructor(this_);
    if ((flags & 1) != 0)
        operator_delete_internal(this_);
    return this_;
}

// Replacement for TRIBE_Master_Combat_Object::TRIBE_Master_Combat_Object2
// Original: @ 0x005C83C0 (idb_functions.h:5122). ABI: __thiscall — this_
// in ECX, 4 stack args (retn 10h), returns this. SEH fold (unwind chunk
// 0x6466C0, same base-destructor action). Base RGE Combat ctor2 with
// do_setup = 0 (asm 0x5C83F1), TRIBE vfptr store (asm 0x5C8404), if
// (do_setup) the derived setup2 (asm 0x5C8411). Caller coverage:
// TRIBE_Combat_Object__setup_read calls ctor2 directly (the runtime
// binary-load path); the Building ctor2 chain builds on it.
TRIBE_Master_Combat_Object* __fastcall TRIBE_Master_Combat_Object__TRIBE_Master_Combat_Object2_new(
    TRIBE_Master_Combat_Object* this_, DWORD dummy,
    int infile, RGE_Sprite** sprites, RGE_Sound** sounds, int do_setup)
{
    TRIBE_Master_Combat_Object* result = this_;
    __try
    {
        RGE_Master_Combat_Object__RGE_Master_Combat_Object2(
            (RGE_Master_Combat_Object*)this_, infile, sprites, sounds, 0);
        this_->vfptr = tribe_master_combat_object_vftable;
        if (do_setup)
            TRIBE_Master_Combat_Object__setup2(this_, infile, sprites, sounds);
        result = this_;
    }
    __except (RGE_Master_Combat_Object__destructor((RGE_Master_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for TRIBE_Master_Combat_Object::TRIBE_Master_Combat_Object3
// Original: @ 0x005C8430 (idb_functions.h:5123). ABI: __thiscall — this_
// in ECX, 5 stack args (retn 14h), returns this. SEH fold (unwind chunk
// 0x6466E0, same base-destructor action). Base RGE Combat ctor3 with
// do_setup = 0; if (do_setup) the derived setup3. Caller coverage:
// TRIBE_Master_Player__load_object calls ctor3 directly.
TRIBE_Master_Combat_Object* __fastcall TRIBE_Master_Combat_Object__TRIBE_Master_Combat_Object3_new(
    TRIBE_Master_Combat_Object* this_, DWORD dummy,
    FILE* infile, RGE_Sprite** sprites, RGE_Sound** sounds, __int16 in_id, int do_setup)
{
    TRIBE_Master_Combat_Object* result = this_;
    __try
    {
        RGE_Master_Combat_Object__RGE_Master_Combat_Object3(
            (RGE_Master_Combat_Object*)this_, infile, sprites, sounds, in_id, 0);
        this_->vfptr = tribe_master_combat_object_vftable;
        if (do_setup)
            TRIBE_Master_Combat_Object__setup3(this_, infile, sprites, sounds, in_id);
        result = this_;
    }
    __except (RGE_Master_Combat_Object__destructor((RGE_Master_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for TRIBE_Master_Combat_Object::TRIBE_Master_Combat_Object4
// Original: @ 0x005C84B0 (idb_functions.h:5124). ABI: __thiscall — this_
// in ECX, 5 stack args (retn 14h), returns this. SEH fold (unwind chunk
// 0x646700, same base-destructor action). Base RGE Combat ctor4 with
// do_setup = 0; if (do_setup) the derived setup4.
TRIBE_Master_Combat_Object* __fastcall TRIBE_Master_Combat_Object__TRIBE_Master_Combat_Object4_new(
    TRIBE_Master_Combat_Object* this_, DWORD dummy,
    int infile, RGE_Master_Static_Object** list, RGE_Sprite** sprites, RGE_Sound** sounds, int do_setup)
{
    TRIBE_Master_Combat_Object* result = this_;
    __try
    {
        RGE_Master_Combat_Object__RGE_Master_Combat_Object4(
            (RGE_Master_Combat_Object*)this_, infile, list, sprites, sounds, 0);
        this_->vfptr = tribe_master_combat_object_vftable;
        if (do_setup)
            TRIBE_Master_Combat_Object__setup4(this_, infile, list, sprites, sounds);
        result = this_;
    }
    __except (RGE_Master_Combat_Object__destructor((RGE_Master_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for TRIBE_Master_Combat_Object::setup
// Original: @ 0x005C8530 (idb_functions.h:5125). ABI: __thiscall — this_
// in ECX, 1 stack arg (retn 4), int result (always 1).
// Derived override: base RGE Combat setup call (through the idb constant
// — routes into the replacement per the HOOK-ROUTING RULING), master_type
// forced to RGE_MASTER_TYPE_COMBAT (70, asm 0x5C8542 byte store @+4),
// then the Combat(70) extension copies. Extension offsets 398-452
// verified by the mechanical struct walk AND by this function's own asm
// copy block (0x18E/0x190/0x192/0x194/0x198/0x19C/0x1A0/0x1A4/0x1A8/
// 0x1AC/0x1B0/0x1B4/0x1B8/0x1BC/0x1C0/0x1C1/0x1C4): build_inventory[3]
// (6-byte Attribute_Cost elements, asm dword+word pair copies), then 16
// scalars. Caller coverage: TRIBE_Master_Building_Object__setup calls
// this setup directly (Building extends this tier).
int __fastcall TRIBE_Master_Combat_Object__setup_new(
    TRIBE_Master_Combat_Object* this_, DWORD dummy,
    TRIBE_Master_Combat_Object* other_object)
{
    RGE_Master_Combat_Object__setup((RGE_Master_Combat_Object*)this_, (RGE_Master_Combat_Object*)other_object);
    this_->master_type = RGE_MASTER_TYPE_COMBAT;
    for (size_t i = 0; i < _countof(this_->build_inventory); ++i)
        this_->build_inventory[i] = other_object->build_inventory[i];
    this_->build_pts_required = other_object->build_pts_required;
    this_->id_of_building_obj = other_object->id_of_building_obj;
    this_->button_location = other_object->button_location;
    this_->orig_pierce_armor = other_object->orig_pierce_armor;
    this_->rear_attack_modifier = other_object->rear_attack_modifier;
    this_->flank_attack_modifier = other_object->flank_attack_modifier;
    this_->tribe_unit_type = other_object->tribe_unit_type;
    this_->volley_fire_amount = other_object->volley_fire_amount;
    this_->max_attacks_in_volley = other_object->max_attacks_in_volley;
    this_->volley_x_spread = other_object->volley_x_spread;
    this_->volley_y_spread = other_object->volley_y_spread;
    this_->volley_start_spread_adjustment = other_object->volley_start_spread_adjustment;
    this_->volley_missile_id = other_object->volley_missile_id;
    this_->special_attack_sprite_id = other_object->special_attack_sprite_id;
    this_->special_attack_flag = other_object->special_attack_flag;
    this_->hero_flag = other_object->hero_flag;
    this_->garrison_sprite = other_object->garrison_sprite;
    return 1;
}

// Replacement for TRIBE_Master_Combat_Object::setup2
// Original: @ 0x005C8660 (idb_functions.h:5126). ABI: __thiscall — this_
// in ECX, 3 stack args (retn 0Ch), int result (always 1).
// Base call, master_type forced, then the stream sequence: build_
// inventory x3 (rge_read 6 bytes = sizeof(Attribute_Cost) each),
// build_pts_required (2), id_of_building_obj (2), button_location (1),
// rear_attack_modifier (4), flank_attack_modifier (4), tribe_unit_type
// (1), hero_flag (1), then the garrison index read into a named local
// (the original reuses the sounds PARAM SLOT as the read buffer — asm
// scratch; resolved guarded against the SPRITES array) and the tail:
// volley_fire_amount (4), max_attacks_in_volley (1), volley_x_spread (4),
// volley_y_spread (4), volley_start_spread_adjustment (4), volley_missile_
// id (4, int field), special_attack_sprite_id (4, int field),
// special_attack_flag (1), orig_pierce_armor (2) — read LAST despite its
// struct position (stream order, faithful).
int __fastcall TRIBE_Master_Combat_Object__setup2_new(
    TRIBE_Master_Combat_Object* this_, DWORD dummy,
    int infile, RGE_Sprite** sprites, RGE_Sound** sounds)
{
    RGE_Master_Combat_Object__setup2((RGE_Master_Combat_Object*)this_, infile, sprites, sounds);
    this_->master_type = RGE_MASTER_TYPE_COMBAT;
    for (size_t i = 0; i < _countof(this_->build_inventory); ++i)
        rge_read(infile, &this_->build_inventory[i], sizeof(this_->build_inventory[i]));
    rge_read(infile, &this_->build_pts_required, sizeof(this_->build_pts_required));
    rge_read(infile, &this_->id_of_building_obj, sizeof(this_->id_of_building_obj));
    rge_read(infile, &this_->button_location, sizeof(this_->button_location));
    rge_read(infile, &this_->rear_attack_modifier, sizeof(this_->rear_attack_modifier));
    rge_read(infile, &this_->flank_attack_modifier, sizeof(this_->flank_attack_modifier));
    rge_read(infile, &this_->tribe_unit_type, sizeof(this_->tribe_unit_type));
    rge_read(infile, &this_->hero_flag, sizeof(this_->hero_flag));
    int garrison_ix;
    rge_read(infile, &garrison_ix, sizeof(garrison_ix));
    this_->garrison_sprite = (garrison_ix < 0) ? 0 : sprites[garrison_ix];
    rge_read(infile, &this_->volley_fire_amount, sizeof(this_->volley_fire_amount));
    rge_read(infile, &this_->max_attacks_in_volley, sizeof(this_->max_attacks_in_volley));
    rge_read(infile, &this_->volley_x_spread, sizeof(this_->volley_x_spread));
    rge_read(infile, &this_->volley_y_spread, sizeof(this_->volley_y_spread));
    rge_read(infile, &this_->volley_start_spread_adjustment, sizeof(this_->volley_start_spread_adjustment));
    rge_read(infile, &this_->volley_missile_id, sizeof(this_->volley_missile_id));
    rge_read(infile, &this_->special_attack_sprite_id, sizeof(this_->special_attack_sprite_id));
    rge_read(infile, &this_->special_attack_flag, sizeof(this_->special_attack_flag));
    rge_read(infile, &this_->orig_pierce_armor, sizeof(this_->orig_pierce_armor));
    return 1;
}

// Replacement for TRIBE_Master_Combat_Object::setup3
// Original: @ 0x005C87C0 (idb_functions.h:5127). ABI: __thiscall — this_
// in ECX, 4 stack args (retn 10h), int result (always 1).
// Base call, master_type forced, then build_inventory x3 via ONE fscanf
// per element ("%hd %hd %hd" into type/amount + a flag scratch — the
// scratch is the in_id PARAM SLOT in the original; named local here;
// byte-narrowed into .flag), then a 16-conversion fscanf (asm string at
// 0x5C887E): build_pts_required, id_of_building_obj, button_location
// scratch (%hd, byte-narrowed), rear/flank (%f), tribe_unit_type scratch
// (%hd, byte-narrowed), volley_fire_amount (%f), max_attacks_in_volley
// scratch (%hd, byte-narrowed), the three volley spreads (%f),
// volley_missile_id (%d, int field), special_attack_sprite_id (%d, int
// field), then four int scratches (special_attack_flag, hero, garrison
// index, special sprite index — the original reuses the sounds/infile
// PARAM SLOTS for the last two). hero_flag = (hero scratch == 1) (asm
// 0x5C888E word compare). special_attack_flag: a conditional store
// (scratch >= 0 ? scratch : 0, asm 0x5C88A5-0x4888B3) immediately
// overwritten by an UNCONDITIONAL zero (asm 0x4888C0) — both stores kept
// verbatim; the final value is always 0. garrison_sprite: guarded
// sprites[__int16-truncated scratch] (asm 0x4888BB-0x4888D7). Then the
// armor derivations: orig_pierce_armor = -1 -> max value among
// armor[].type == 3 (signed i16 compares), falling back to base_armor
// (@304, asm 0x48893F); orig_armor = 0 -> max value among armor[].type
// == 4, NO fallback (asm 0x4894D-0x48985) — this OVERWRITES the base
// RGE Combat setup3's orig_armor derivation (later write wins, faithful).
int __fastcall TRIBE_Master_Combat_Object__setup3_new(
    TRIBE_Master_Combat_Object* this_, DWORD dummy,
    FILE* infile, RGE_Sprite** sprites, RGE_Sound** sounds, __int16 in_id)
{
    RGE_Master_Combat_Object__setup3((RGE_Master_Combat_Object*)this_, infile, sprites, sounds, in_id);
    this_->master_type = RGE_MASTER_TYPE_COMBAT;
    for (size_t i = 0; i < _countof(this_->build_inventory); ++i)
    {
        __int16 flag_s;
        fscanf_internal(infile, "%hd %hd %hd",
            &this_->build_inventory[i].type,
            &this_->build_inventory[i].amount,
            &flag_s);
        this_->build_inventory[i].flag = (unsigned __int8)flag_s;
    }
    __int16 button_s;
    __int16 tribe_s;
    __int16 max_attacks_s;
    int special_flag_s;
    int hero_s;
    int garrison_s;
    fscanf_internal(infile, "%hd %hd %hd %f %f %hd %f %hd %f %f %f %d %d %d %d %d",
        &this_->build_pts_required,
        &this_->id_of_building_obj,
        &button_s,
        &this_->rear_attack_modifier,
        &this_->flank_attack_modifier,
        &tribe_s,
        &this_->volley_fire_amount,
        &max_attacks_s,
        &this_->volley_x_spread,
        &this_->volley_y_spread,
        &this_->volley_start_spread_adjustment,
        &this_->volley_missile_id,
        &this_->special_attack_sprite_id,
        &special_flag_s,
        &hero_s,
        &garrison_s);
    this_->hero_flag = (hero_s == 1) ? 1 : 0;
    this_->special_attack_flag = (special_flag_s >= 0) ? (unsigned __int8)special_flag_s : 0;
    this_->special_attack_flag = 0;  // asm 0x4888C0 — unconditional zero OVERWRITES the conditional above
    __int16 garrison_ix = (__int16)garrison_s;
    this_->garrison_sprite = (garrison_ix < 0) ? 0 : sprites[garrison_ix];
    this_->button_location = (unsigned __int8)button_s;
    this_->tribe_unit_type = (unsigned __int8)tribe_s;
    this_->orig_pierce_armor = -1;
    if (this_->armor && this_->armor_num > 0)
    {
        for (int i = 0; i < this_->armor_num; ++i)
        {
            if (this_->armor[i].type == 3 && this_->armor[i].value > this_->orig_pierce_armor)
                this_->orig_pierce_armor = this_->armor[i].value;
        }
    }
    if (this_->orig_pierce_armor == -1)
        this_->orig_pierce_armor = this_->base_armor;
    this_->orig_armor = 0;
    if (this_->armor && this_->armor_num > 0)
    {
        for (int j = 0; j < this_->armor_num; ++j)
        {
            if (this_->armor[j].type == 4 && this_->armor[j].value > this_->orig_armor)
                this_->orig_armor = this_->armor[j].value;
        }
    }
    return 1;
}

// Replacement for TRIBE_Master_Combat_Object::setup4
// Original: @ 0x005C89A0 (idb_functions.h:5128). ABI: __thiscall — this_
// in ECX, 4 stack args (retn 10h), int result (always 1).
// Base call, then list[copy_id] (copy_id was stream-read by the base
// setup4; UNCONDITIONAL sign-extended index, no negative guard —
// faithful). BASE-POINTER CORRECTION per the template rule: the IDA
// v7[1].field / v7[2].field accesses (v7 typed RGE_Master_Static_Object*,
// sizeof 208) land exactly on the walked Combat(70) extension offsets:
// create_doppleganger_on_death@190 -> 398 build_pts_required,
// draw_flag@192 -> 400 id_of_building_obj, &draw_color+1@194 -> 402
// button_location, outline_radius_x@196 -> 404 orig_pierce_armor,
// outline_radius_y@200 -> 408 rear_attack_modifier, outline_radius_z@204
// -> 412 flank_attack_modifier, v7[2].vfptr@416 -> tribe_unit_type,
// master_type@420 -> volley_fire_amount, name@424 -> max_attacks_in_
// volley, gbg_name2@428 -> volley_x_spread, unit_line@432 ->
// volley_y_spread, string_id@436 -> volley_start_spread_adjustment,
// id@440 -> volley_missile_id, base_id@444 -> special_attack_sprite_id,
// sprite@448 -> special_attack_flag, BYTE1(sprite)@449 -> hero_flag,
// sprite2@452 -> garrison_sprite. The list element is re-typed
// TRIBE_Master_Combat_Object* accordingly. The seventeen extension
// copies (incl. build_inventory[3]) happen when the reference is
// non-null; then the stream tail re-reads build_inventory x3 (6 bytes
// each), build_pts_required (2), volley_fire_amount (4),
// max_attacks_in_volley (1) — OVERWRITING the copied values (later
// write wins).
int __fastcall TRIBE_Master_Combat_Object__setup4_new(
    TRIBE_Master_Combat_Object* this_, DWORD dummy,
    int infile, RGE_Master_Static_Object** list, RGE_Sprite** sprites, RGE_Sound** sounds)
{
    RGE_Master_Combat_Object__setup4((RGE_Master_Combat_Object*)this_, infile, list, sprites, sounds);
    TRIBE_Master_Combat_Object* other = (TRIBE_Master_Combat_Object*)list[this_->copy_id];
    this_->master_type = RGE_MASTER_TYPE_COMBAT;
    if (other)
    {
        for (size_t i = 0; i < _countof(this_->build_inventory); ++i)
            this_->build_inventory[i] = other->build_inventory[i];
        this_->build_pts_required = other->build_pts_required;
        this_->id_of_building_obj = other->id_of_building_obj;
        this_->button_location = other->button_location;
        this_->orig_pierce_armor = other->orig_pierce_armor;
        this_->rear_attack_modifier = other->rear_attack_modifier;
        this_->flank_attack_modifier = other->flank_attack_modifier;
        this_->tribe_unit_type = other->tribe_unit_type;
        this_->volley_fire_amount = other->volley_fire_amount;
        this_->max_attacks_in_volley = other->max_attacks_in_volley;
        this_->volley_x_spread = other->volley_x_spread;
        this_->volley_y_spread = other->volley_y_spread;
        this_->volley_start_spread_adjustment = other->volley_start_spread_adjustment;
        this_->volley_missile_id = other->volley_missile_id;
        this_->special_attack_sprite_id = other->special_attack_sprite_id;
        this_->special_attack_flag = other->special_attack_flag;
        this_->hero_flag = other->hero_flag;
        this_->garrison_sprite = other->garrison_sprite;
    }
    for (size_t j = 0; j < _countof(this_->build_inventory); ++j)
        rge_read(infile, &this_->build_inventory[j], sizeof(this_->build_inventory[j]));
    rge_read(infile, &this_->build_pts_required, sizeof(this_->build_pts_required));
    rge_read(infile, &this_->volley_fire_amount, sizeof(this_->volley_fire_amount));
    rge_read(infile, &this_->max_attacks_in_volley, sizeof(this_->max_attacks_in_volley));
    return 1;
}

// Replacement for TRIBE_Master_Combat_Object::destructor
// Original: @ 0x005C8B30 (idb_functions.h:5129). ABI: __thiscall — this_
// in ECX, no stack args, void return, no dummy.
// The asm stores the TRIBE vftable then TAIL-JUMPS the base RGE Combat
// destructor; the replacement calls it (call vs tail-jmp is not
// observable). Base call routes through the idb constant -> hooked
// replacement per the HOOK-ROUTING RULING. Note: the ctor SEH unwind
// funclets (0x6466A0/0x6466C0/0x6466E0/0x646700) jump into the BASE
// destructor directly — the exception path routes through the RGE_
// Combat destructor replacement, and this destructor's hook covers the
// virtual-dispatch path.
void __fastcall TRIBE_Master_Combat_Object__destructor_new(TRIBE_Master_Combat_Object* this_)
{
    this_->vfptr = tribe_master_combat_object_vftable;
    RGE_Master_Combat_Object__destructor((RGE_Master_Combat_Object*)this_);
}

// Replacement for TRIBE_Master_Combat_Object::create_task_list
// Original: @ 0x005C8B40 (idb_functions.h:5130). ABI: __thiscall — this_
// in ECX, no stack args, RGE_Task_List* result. this_ is unused by the
// body (the factory is shape-independent). SEH fold per the make_new_obj
// precedent (FUNCTION CHUNK 0x646720: operator_delete frees the
// allocation on exception). operator_new(0xC) — sizeof(RGE_Task_List) =
// 12 walker-verified ({vfptr, RGE_Task** list, __int16 list_num} rounded
// to align 4) — null-checked, then the RGE_Task_List constructor
// (idb_functions.h:4915, 0x005B9400) and the TRIBE_Task_List vftable
// store (.rdata:00667414 — the subtype marker). Caller coverage: both
// the TRIBE_Master_Combat_Object and TRIBE_Master_Building_Object
// vftables reference this slot (asm DATA XREFs).
RGE_Task_List* __fastcall TRIBE_Master_Combat_Object__create_task_list_new(
    TRIBE_Master_Combat_Object* this_)
{
    RGE_Task_List* new_list = (RGE_Task_List*)operator_new_internal(sizeof(RGE_Task_List));
    RGE_Task_List* result = 0;
    __try
    {
        if (new_list)
        {
            RGE_Task_List__RGE_Task_List(new_list);
            new_list->vfptr = tribe_task_list_vftable;
            result = new_list;
        }
    }
    __except (operator_delete_internal(new_list), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for TRIBE_Master_Combat_Object::copy_obj
// Original: @ 0x005C8B90 (idb_functions.h:5131). ABI: __thiscall — this_
// in ECX, 1 stack arg (retn 4), void return. No SEH frame (plain
// function). Base RGE Combat copy_obj call (through the idb constant ->
// our hooked replacement per the HOOK-ROUTING RULING; the asm's own
// XREF shows TRIBE_Master_Building_Object__copy_obj forwards into it),
// then the Combat(70) extension copies. BASE-POINTER CORRECTION per the
// template rule: the IDA source[1].field / source[2].field accesses
// (source typed RGE_Master_Static_Object*, sizeof 208) land exactly on
// the walked extension offsets 398-452 (the same mapping as setup4 —
// create_doppleganger_on_death@190 -> build_pts_required, ..., sprite2
// @452 -> garrison_sprite); the source is re-typed
// TRIBE_Master_Combat_Object*. The build_inventory loop copies the three
// 6-byte Attribute_Cost elements (the asm's dword+word pair copies).
void __fastcall TRIBE_Master_Combat_Object__copy_obj_new(
    TRIBE_Master_Combat_Object* this_, DWORD dummy,
    RGE_Master_Static_Object* source)
{
    RGE_Master_Combat_Object__copy_obj((RGE_Master_Combat_Object*)this_, source);
    TRIBE_Master_Combat_Object* src = (TRIBE_Master_Combat_Object*)source;
    for (size_t i = 0; i < _countof(this_->build_inventory); ++i)
        this_->build_inventory[i] = src->build_inventory[i];
    this_->build_pts_required = src->build_pts_required;
    this_->id_of_building_obj = src->id_of_building_obj;
    this_->button_location = src->button_location;
    this_->orig_pierce_armor = src->orig_pierce_armor;
    this_->rear_attack_modifier = src->rear_attack_modifier;
    this_->flank_attack_modifier = src->flank_attack_modifier;
    this_->tribe_unit_type = src->tribe_unit_type;
    this_->volley_fire_amount = src->volley_fire_amount;
    this_->max_attacks_in_volley = src->max_attacks_in_volley;
    this_->volley_x_spread = src->volley_x_spread;
    this_->volley_y_spread = src->volley_y_spread;
    this_->volley_start_spread_adjustment = src->volley_start_spread_adjustment;
    this_->volley_missile_id = src->volley_missile_id;
    this_->special_attack_sprite_id = src->special_attack_sprite_id;
    this_->special_attack_flag = src->special_attack_flag;
    this_->hero_flag = src->hero_flag;
    this_->garrison_sprite = src->garrison_sprite;
}

// Replacement for TRIBE_Master_Combat_Object::modify
// Original: @ 0x005C8CB0 (idb_functions.h:5132). ABI: __thiscall — this_
// in ECX, 2 stack args (retn 8), void return. No SEH frame (plain
// function). Switch restored from the jump table off_5C8D80 (@0x5C8D80,
// 8 entries over flag-100 — asm `add eax, 0FFFFFF9Ch`; the ASCII flag
// namespace): 'd' (100) and 'g'/'h'/'i'/'j' (103-106) share the
// build_inventory block (asm 0x5C8CD4); 'e' (101) -> build_pts_required
// SET; 'f' (102) -> volley_fire_amount SET; 'k' (107) ->
// max_attacks_in_volley SET; default -> the base RGE Combat modify
// (hooked -> routes per the HOOK-ROUTING RULING; asm 0x5C8D6B). The
// inner selector switch off_5C8DA0 over flag-'d': 0 -> -1 (ALL entries),
// 3 -> 0, 4 -> 1, 5 -> 3, 6 -> 2, default -> selector stays 0 (unreachable
// for the admitted flags). The inventory loop (asm 0x5C8CFC-0x5C8D2A):
// for each of the 3 entries, if entry.flag != 0 && (selector == -1 ||
// entry.type == selector) -> entry.amount = (i16)__ftol(amount) (asm
// fld/__ftol/mov [esi+2], ax). Caller coverage:
// TRIBE_Master_Building_Object__modify forwards into it.
void __fastcall TRIBE_Master_Combat_Object__modify_new(
    TRIBE_Master_Combat_Object* this_, DWORD dummy,
    float amount, unsigned __int8 flag)
{
    switch (flag)
    {
    case 'd':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    {
        __int16 selector = 0;
        switch (flag - 'd')  // asm inner table off_5C8DA0 over flag-100
        {
        case 0:
            selector = -1;  // 'd' — all entries
            break;
        case 3:
            selector = 0;   // 'g'
            break;
        case 4:
            selector = 1;   // 'h'
            break;
        case 5:
            selector = 3;   // 'i'
            break;
        case 6:
            selector = 2;   // 'j'
            break;
        }
        for (size_t i = 0; i < _countof(this_->build_inventory); ++i)
        {
            Attribute_Cost* entry = &this_->build_inventory[i];
            if (entry->flag && (selector == -1 || entry->type == selector))
                entry->amount = (__int16)(int)amount;
        }
        break;
    }
    case 'e':
        this_->build_pts_required = (__int16)(int)amount;
        break;
    case 'f':
        this_->volley_fire_amount = amount;
        break;
    case 'k':
        this_->max_attacks_in_volley = (unsigned __int8)(__int16)(int)amount;
        break;
    default:
        RGE_Master_Combat_Object__modify((RGE_Master_Combat_Object*)this_, amount, flag);
        break;
    }
}

// Replacement for TRIBE_Master_Combat_Object::modify_delta
// Original: @ 0x005C8DC0 (idb_functions.h:5133). Same ABI. Same switch
// skeleton (tables off_5C8E9C/off_5C8EBC, asm 0x5C8DD1 `add eax,
// 0FFFFFF9Ch`): the inventory block ADDs (__i16 add of ftol's low word,
// asm `*v4 += (__int64)amount` rendering), 'e' adds to
// build_pts_required, 'f' adds to volley_fire_amount (float), 'k' adds
// to max_attacks_in_volley; default -> base RGE Combat modify_delta.
void __fastcall TRIBE_Master_Combat_Object__modify_delta_new(
    TRIBE_Master_Combat_Object* this_, DWORD dummy,
    float amount, unsigned __int8 flag)
{
    switch (flag)
    {
    case 'd':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    {
        __int16 selector = 0;
        switch (flag - 'd')
        {
        case 0:
            selector = -1;
            break;
        case 3:
            selector = 0;
            break;
        case 4:
            selector = 1;
            break;
        case 5:
            selector = 3;
            break;
        case 6:
            selector = 2;
            break;
        }
        for (size_t i = 0; i < _countof(this_->build_inventory); ++i)
        {
            Attribute_Cost* entry = &this_->build_inventory[i];
            if (entry->flag && (selector == -1 || entry->type == selector))
                entry->amount = (__int16)(entry->amount + (__int16)(int)amount);
        }
        break;
    }
    case 'e':
        this_->build_pts_required = (__int16)(this_->build_pts_required + (__int16)(int)amount);
        break;
    case 'f':
        this_->volley_fire_amount = amount + this_->volley_fire_amount;
        break;
    case 'k':
        this_->max_attacks_in_volley = (unsigned __int8)(__int16)(this_->max_attacks_in_volley + (__int16)(int)amount);
        break;
    default:
        RGE_Master_Combat_Object__modify_delta((RGE_Master_Combat_Object*)this_, amount, flag);
        break;
    }
}

// Replacement for TRIBE_Master_Combat_Object::modify_percent
// Original: @ 0x005C8EE0 (idb_functions.h:5134). Same ABI. Same switch
// skeleton: the inventory block computes amount = (i16)(int)(old *
// amount + 0.5) — the pseudocode prints a plain 0.5 (numerically
// identical to the family's 0.5f — exact in both widths); 'e'/'k' use
// the same (i16)(int)(old * amount + 0.5f) rounding; 'f' multiplies
// volley_fire_amount; default -> base RGE Combat modify_percent.
void __fastcall TRIBE_Master_Combat_Object__modify_percent_new(
    TRIBE_Master_Combat_Object* this_, DWORD dummy,
    float amount, unsigned __int8 flag)
{
    switch (flag)
    {
    case 'd':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    {
        __int16 selector = 0;
        switch (flag - 'd')
        {
        case 0:
            selector = -1;
            break;
        case 3:
            selector = 0;
            break;
        case 4:
            selector = 1;
            break;
        case 5:
            selector = 3;
            break;
        case 6:
            selector = 2;
            break;
        }
        for (size_t i = 0; i < _countof(this_->build_inventory); ++i)
        {
            Attribute_Cost* entry = &this_->build_inventory[i];
            if (entry->flag && (selector == -1 || entry->type == selector))
                entry->amount = (__int16)(int)((double)entry->amount * amount + 0.5f);
        }
        break;
    }
    case 'e':
        this_->build_pts_required = (__int16)(int)((double)this_->build_pts_required * amount + 0.5f);
        break;
    case 'f':
        this_->volley_fire_amount = amount * this_->volley_fire_amount;
        break;
    case 'k':
        this_->max_attacks_in_volley = (unsigned __int8)(__int16)(int)((double)this_->max_attacks_in_volley * amount + 0.5f);
        break;
    default:
        RGE_Master_Combat_Object__modify_percent((RGE_Master_Combat_Object*)this_, amount, flag);
        break;
    }
}

// Replacement for TRIBE_Master_Combat_Object::small_save
// Original: @ 0x005C9030 (idb_functions.h:5135). ABI: __thiscall — this_
// in ECX, 1 stack arg (retn 4), void return. No SEH frame (plain
// function). Base RGE Combat small_save call (hooked -> routes), then
// build_inventory x3 (w6 = sizeof(Attribute_Cost) each), build_pts_
// required (w2), volley_fire_amount (w4), max_attacks_in_volley (w1).
void __fastcall TRIBE_Master_Combat_Object__small_save_new(
    TRIBE_Master_Combat_Object* this_, DWORD dummy, int outfile)
{
    RGE_Master_Combat_Object__small_save((RGE_Master_Combat_Object*)this_, outfile);
    for (size_t i = 0; i < _countof(this_->build_inventory); ++i)
        rge_write(outfile, &this_->build_inventory[i], sizeof(this_->build_inventory[i]));
    rge_write(outfile, &this_->build_pts_required, sizeof(this_->build_pts_required));
    rge_write(outfile, &this_->volley_fire_amount, sizeof(this_->volley_fire_amount));
    rge_write(outfile, &this_->max_attacks_in_volley, sizeof(this_->max_attacks_in_volley));
}

// Replacement for TRIBE_Master_Combat_Object::save
// Original: @ 0x005C9090 (idb_functions.h:5136). Same ABI. The garrison
// id local is prepared BEFORE the base call (asm order): garrison_sprite
// -> RGE_Sprite::id @+0x82, sign-extended into an int local (unlike the
// low-word id locals elsewhere, THIS one streams all 4 bytes — asm w4
// write). Base RGE Combat save (hooked -> routes), then 18 writes in asm
// order: build_inventory x3 (w6), build_pts_required (w2),
// id_of_building_obj (w2), button_location (w1), rear_attack_modifier
// (w4), flank_attack_modifier (w4), tribe_unit_type (w1), hero_flag (w1),
// garrison id (w4), volley_fire_amount (w4), max_attacks_in_volley (w1),
// volley_x_spread (w4), volley_y_spread (w4), volley_start_spread_
// adjustment (w4), volley_missile_id (w4), special_attack_sprite_id
// (w4), special_attack_flag (w1), orig_pierce_armor (w2).
void __fastcall TRIBE_Master_Combat_Object__save_new(
    TRIBE_Master_Combat_Object* this_, DWORD dummy, int outfile)
{
    int garrison_id = -1;
    if (this_->garrison_sprite)
        garrison_id = this_->garrison_sprite->id;
    RGE_Master_Combat_Object__save((RGE_Master_Combat_Object*)this_, outfile);
    for (size_t i = 0; i < _countof(this_->build_inventory); ++i)
        rge_write(outfile, &this_->build_inventory[i], sizeof(this_->build_inventory[i]));
    rge_write(outfile, &this_->build_pts_required, sizeof(this_->build_pts_required));
    rge_write(outfile, &this_->id_of_building_obj, sizeof(this_->id_of_building_obj));
    rge_write(outfile, &this_->button_location, sizeof(this_->button_location));
    rge_write(outfile, &this_->rear_attack_modifier, sizeof(this_->rear_attack_modifier));
    rge_write(outfile, &this_->flank_attack_modifier, sizeof(this_->flank_attack_modifier));
    rge_write(outfile, &this_->tribe_unit_type, sizeof(this_->tribe_unit_type));
    rge_write(outfile, &this_->hero_flag, sizeof(this_->hero_flag));
    rge_write(outfile, &garrison_id, sizeof(garrison_id));
    rge_write(outfile, &this_->volley_fire_amount, sizeof(this_->volley_fire_amount));
    rge_write(outfile, &this_->max_attacks_in_volley, sizeof(this_->max_attacks_in_volley));
    rge_write(outfile, &this_->volley_x_spread, sizeof(this_->volley_x_spread));
    rge_write(outfile, &this_->volley_y_spread, sizeof(this_->volley_y_spread));
    rge_write(outfile, &this_->volley_start_spread_adjustment, sizeof(this_->volley_start_spread_adjustment));
    rge_write(outfile, &this_->volley_missile_id, sizeof(this_->volley_missile_id));
    rge_write(outfile, &this_->special_attack_sprite_id, sizeof(this_->special_attack_sprite_id));
    rge_write(outfile, &this_->special_attack_flag, sizeof(this_->special_attack_flag));
    rge_write(outfile, &this_->orig_pierce_armor, sizeof(this_->orig_pierce_armor));
}

// Replacement for TRIBE_Master_Combat_Object::make_new_obj
// Original: @ 0x005C91F0 (idb_functions.h:5137). ABI: __thiscall — this_
// in ECX, 4 stack args (retn 10h), RGE_Static_Object* result.
// DELTA vs the RGE_ factories: NO RECYCLE PATH — Combat(70) runtime
// objects are never recycled; the asm goes straight to operator_new.
// ALLOCATION VERIFIED: sizeof(TRIBE_Combat_Object) = 452 = 0x1C4 —
// sequential walk over idb_types.h:16324-16432 (the RGE_Combat_Object
// 380 prefix + the 70-tier runtime extension: attr_amt[6] @380,
// decay_timer @392, raider_build_countdown @396, garrison/town-bell
// block @400-431, current_volley_fire_amount @432, next_volley @436,
// using_special_attack_animation @437, locked_down_count @440,
// user_released @444, berserker_timer @448 -> 452). The runtime ctor
// constant: TRIBE_Combat_Object__TRIBE_Combat_Object @ idb_functions.h
// :3982 (0x00559E00), do_setup = 1 (asm 0x5C923F). SEH ctor-unwind
// frame folded per the make_new_obj precedent (FUNCTION CHUNK 0x646740:
// operator_delete frees the allocation on exception).
RGE_Static_Object* __fastcall TRIBE_Master_Combat_Object__make_new_obj_new(
    TRIBE_Master_Combat_Object* this_, DWORD dummy,
    RGE_Player* owner, float x, float y, float z)
{
    RGE_Static_Object* result = 0;
    TRIBE_Combat_Object* new_obj = 0;
    __try
    {
        new_obj = (TRIBE_Combat_Object*)operator_new_internal(sizeof(TRIBE_Combat_Object));
        if (new_obj)
            result = (RGE_Static_Object*)TRIBE_Combat_Object__TRIBE_Combat_Object(new_obj, this_, owner, x, y, z, 1);
    }
    __except (operator_delete_internal(new_obj), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for TRIBE_Master_Combat_Object::make_new_master
// Original: @ 0x005C9270 (idb_functions.h:5138). ABI: __thiscall — this_
// in ECX, no stack args, RGE_Master_Static_Object* result.
// Master-template factory: operator_new(0x1C8) — sizeof(TRIBE_Master_
// Combat_Object) = 456 = 0x1CC walker-verified — null-checked, then the
// base RGE Combat COPY-CTOR with do_setup = 0 (asm 0x5C92AB), the TRIBE
// vfptr store (asm 0x5C92B8), and the DERIVED setup (asm 0x5C92BE —
// hooked -> routes into our replacement; it sets master_type = 70 and
// copies the whole Combat(70) extension from this). NO master_type store
// and NO extension copies here (the setup does everything). SEH fold per
// the make_new_master precedent (FUNCTION CHUNK 0x646760 — the standard
// two-entry shape, folded into the operator_delete filter).
RGE_Master_Static_Object* __fastcall TRIBE_Master_Combat_Object__make_new_master_new(
    TRIBE_Master_Combat_Object* this_)
{
    TRIBE_Master_Combat_Object* new_master =
        (TRIBE_Master_Combat_Object*)operator_new_internal(sizeof(TRIBE_Master_Combat_Object));
    RGE_Master_Static_Object* result = 0;
    __try
    {
        if (new_master)
        {
            RGE_Master_Combat_Object__RGE_Master_Combat_Object(
                (RGE_Master_Combat_Object*)new_master, (RGE_Master_Combat_Object*)this_, 0);
            new_master->vfptr = tribe_master_combat_object_vftable;
            TRIBE_Master_Combat_Object__setup(new_master, this_);
            result = (RGE_Master_Static_Object*)new_master;
        }
    }
    __except (operator_delete_internal(new_master), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for TRIBE_Master_Combat_Object::check_placement
// Original: @ 0x005C92E0 (idb_functions.h:5139). ABI: __thiscall — this_
// in ECX, 12 stack args (retn 30h), unsigned __int8 result.
// Decode (asm 0x5C92E0-0x5C976F, pseudocode .c:392521-392735): the shared
// implementation TRIBE_Master_Building_Object__check_placement calls twice
// (+6A/+483). Phases per asm:
//   base call  RGE_Master_Static_Object__check_placement (:1924) with
//              check_objects = 0 and check_movible_objects = 0 FORCED
//              (asm 0x5C92FB/0x5C9309); result banked (byte store).
//   !check_objects -> return base result.
//   radius select: construction_radius_flag -> construction_radius_x/y
//              (@108/112) else radius_x/y (@60/64) (asm 0x5C934B/0x5C935B).
//   both <= 0 -> return 0 (fcomp const_f_zero / test ah,41h / jz -> xor
//              al,al — asm 0x5C9369-0x5C9394).
//   scan rect = (i16)(x-8)/(y-8) .. (i16)(x+8)/(y+8) (ftol, asm
//              0x5C9397-0x5C93DF); negative word -> 0; >= map_width/
//              height (map @+8/+12) -> width/height - 1.
//   object_group == 27 -> radius_z (@68) = 1.0 (asm 0x5C9422-0x5C9430).
//   !can_be_built_on_flag && radius_z <= 0 -> return base result (asm
//              0x5C9430-0x5C9446). empty rect -> return base result.
//   scan: row-major, tile = map_row_offset[row] + col*32 (asm
//              0x5C946D-0x5C947D); count byte @tile+13, object array
//              @tile+20 (asm 0x5C948D/0x5C949D).
//   per object: ignore_obj / object_state @+88 >= 7 -> skip; master_obj
//              @+20; fly-over: other master movement_type @+120 == 1 AND
//              this movement_type == 1 -> |dx| < other radius_x @+60 +
//              my x-radius AND other radius_y @+64 + my y-radius > |dy|
//              -> return 6 (asm 0x5C94BB-0x5C953C); can_be_built_on
//              @+91 == 1 -> blocker-is-enterable flag; master_type @+4
//              == 80 -> skip when object_group @+30 not in {34,36} AND
//              runtime desolid_flag @+565 set (asm 0x5C9541-0x5C956C);
//              check_movible_objects == 1 -> skip master_type == 70;
//              == 2 -> skip master_type == 70 with speed @+180 > 0 (asm
//              0x5C9572-0x5C959F).
//   block test: radius_z @+68 — object_group == 27, OR (radius_z == 0
//              AND master_type == 80 AND id @+24 != 50) -> treat as 1.0
//              (asm 0x5C95A5-0x5C95D6 — the group-27 jz routes to the
//              substitution, verified after the fld); 4 > 0 tests; x:
//              |dx| < other radius_x + my x-radius (strict, C0-only —
//              asm 0x5C9677); y: other radius_y + my y-radius > |dy|
//              (0x41 = C0|C3 — asm 0x5C968E) -> obstruction.
//   on obstruction: obstructionObject = obj->id @+16; prog_mode @476 ==
//              7 -> RGE_Game_World__object (:6086) re-fetch; master_
//              obj_id (base_game @+0x428, i16) != -1 AND fetched master_
//              obj->master_type >= 70 -> TRIBE_Combat_Object__
//              canPlayerObjectBeEntered (:4033, obj, get_player
//              (:689), master_obj_id, 0) == 1 -> return 9; blocker
//              can_be_built_on == 0 -> return 6; else continue scanning.
unsigned __int8 __fastcall TRIBE_Master_Combat_Object__check_placement_new(
    TRIBE_Master_Combat_Object* this_, DWORD dummy,
    RGE_Player* player, float x, float y, int* obstructionObject,
    unsigned __int8 check_vis, unsigned __int8 check_flat,
    unsigned __int8 check_overlay, unsigned __int8 construction_radius_flag,
    unsigned __int8 can_be_built_on_flag, unsigned __int8 check_objects,
    RGE_Static_Object* ignore_obj, unsigned __int8 check_movible_objects)
{
    RGE_Map* map = player->world->map;
    unsigned __int8 base_result = RGE_Master_Static_Object__check_placement(
        (RGE_Master_Static_Object*)this_, player, x, y, obstructionObject,
        check_vis, check_flat, check_overlay, construction_radius_flag,
        can_be_built_on_flag, 0, ignore_obj, 0);
    if (!check_objects)
        return base_result;

    float x_radius;
    float y_radius;
    if (construction_radius_flag)
    {
        x_radius = this_->construction_radius_x;
        y_radius = this_->construction_radius_y;
    }
    else
    {
        x_radius = this_->radius_x;
        y_radius = this_->radius_y;
    }
    if (x_radius <= 0.0f && y_radius <= 0.0f)
        return 0;

    __int16 col_lo = (__int16)(int)(x - 8.0f);
    __int16 row_lo = (__int16)(int)(y - 8.0f);
    __int16 col_hi = (__int16)(int)(x + 8.0f);
    __int16 row_hi = (__int16)(int)(y + 8.0f);
    if (col_lo < 0)
        col_lo = 0;
    if (col_hi >= map->map_width)
        col_hi = map->map_width - 1;
    if (row_lo < 0)
        row_lo = 0;
    if (row_hi >= map->map_height)
        row_hi = map->map_height - 1;
    if (this_->object_group == 27)
        this_->radius_z = 1.0f;
    if (!can_be_built_on_flag && this_->radius_z <= 0.0f)
        return base_result;
    if (row_lo > row_hi)
        return base_result;

    for (__int16 row = row_lo; row <= row_hi; ++row)
    {
        for (__int16 col = col_lo; col <= col_hi; ++col)
        {
            const RGE_Tile& tile = map->map_row_offset[row][col];
            if (!tile.TileObject_Count)
                continue;
            RGE_Static_Object** objp = tile.Object_List;
            for (int i = 0; i < tile.TileObject_Count; ++i, ++objp)
            {
                RGE_Static_Object* obj = *objp;
                if (obj == ignore_obj)
                    continue;
                if (obj->object_state >= 7)
                    continue;
                TRIBE_Master_Combat_Object* master = (TRIBE_Master_Combat_Object*)obj->master_obj;
                int blocker_is_enterable = 0;
                if (master->movement_type == 1 && this_->movement_type == 1)
                {
                    float dx = obj->world_x - x;
                    float dy = obj->world_y - y;
                    if (dy < 0.0f) dy = -dy;
                    if (dx < 0.0f) dx = -dx;
                    if (dx < master->radius_x + x_radius && master->radius_y + y_radius > dy)
                        return 6;
                    continue;
                }
                if (master->can_be_built_on == 1)
                    blocker_is_enterable = 1;
                unsigned __int8 other_type = master->master_type;
                if (other_type == 80)
                {
                    __int16 grp = master->object_group;
                    if (grp != 34 && grp != 36 && ((TRIBE_Building_Object*)obj)->desolid_flag)
                        continue;
                }
                if (check_movible_objects == 1)
                {
                    if (other_type == 70)
                        continue;
                }
                else if (check_movible_objects == 2 && other_type == 70
                    && ((TRIBE_Combat_Object*)obj)->speed > 0.0f)   // runtime-type evidence: master_type == 70 -> TRIBE_Combat_Object runtime
                {
                    continue;
                }
                float rz = master->radius_z;
                const float orx = master->radius_x;
                const float ory = master->radius_y;
                if (master->object_group == 27 || (rz == 0.0f && other_type == 80 && master->id != 50))
                    rz = 1.0f;
                if (orx > 0.0f && ory > 0.0f && rz > 0.0f && this_->radius_z > 0.0f)
                {
                    float dx = obj->world_x - x;
                    float dy = obj->world_y - y;
                    if (dy < 0.0f) dy = -dy;
                    if (dx < 0.0f) dx = -dx;
                    if (dx < orx + x_radius && ory + y_radius > dy)
                    {
                        if (obstructionObject)
                            *obstructionObject = obj->id;
                        if ((*base_game)->prog_mode == 7)
                        {
                            TRIBE_Combat_Object* runtime = (TRIBE_Combat_Object*)RGE_Game_World__object(
                                (RGE_Game_World*)(*base_game)->world, obj->id);
                            if ((*base_game)->master_obj_id != -1
                                && runtime->master_obj->master_type >= 70)
                            {
                                if (TRIBE_Combat_Object__canPlayerObjectBeEntered(
                                        runtime, (TRIBE_Player*)RGE_Base_Game__get_player((*base_game)),
                                        (*base_game)->master_obj_id, 0) == 1)
                                    return 9;
                            }
                        }
                        if (!blocker_is_enterable)
                            return 6;
                    }
                }
            }
        }
    }
    return base_result;
}

// NOTE: TRIBE_Master_Combat_Object::check_placement is now OWNED (above);
// its caller TRIBE_Master_Building_Object__check_placement @ 0x005C7B50
// plus Building draw/alignment/check_placement_for_units remain on the
// dedicated decode loop, per the RGE check_placement precedent.

void install_TRIBE_Master_Combat_Object()
{
    setHook((void*)0x005C8340, TRIBE_Master_Combat_Object__TRIBE_Master_Combat_Object_new);
    setHook((void*)0x005C83A0, TRIBE_Master_Combat_Object__scalar_deleting_destructor_new);
    setHook((void*)0x005C83C0, TRIBE_Master_Combat_Object__TRIBE_Master_Combat_Object2_new);
    setHook((void*)0x005C8430, TRIBE_Master_Combat_Object__TRIBE_Master_Combat_Object3_new);
    setHook((void*)0x005C84B0, TRIBE_Master_Combat_Object__TRIBE_Master_Combat_Object4_new);
    setHook((void*)0x005C8530, TRIBE_Master_Combat_Object__setup_new);
    setHook((void*)0x005C8660, TRIBE_Master_Combat_Object__setup2_new);
    setHook((void*)0x005C87C0, TRIBE_Master_Combat_Object__setup3_new);
    setHook((void*)0x005C89A0, TRIBE_Master_Combat_Object__setup4_new);
    setHook((void*)0x005C8B30, TRIBE_Master_Combat_Object__destructor_new);
    setHook((void*)0x005C8B40, TRIBE_Master_Combat_Object__create_task_list_new);
    setHook((void*)0x005C8B90, TRIBE_Master_Combat_Object__copy_obj_new);
    setHook((void*)0x005C8CB0, TRIBE_Master_Combat_Object__modify_new);
    setHook((void*)0x005C8DC0, TRIBE_Master_Combat_Object__modify_delta_new);
    setHook((void*)0x005C8EE0, TRIBE_Master_Combat_Object__modify_percent_new);
    setHook((void*)0x005C9030, TRIBE_Master_Combat_Object__small_save_new);
    setHook((void*)0x005C9090, TRIBE_Master_Combat_Object__save_new);
    setHook((void*)0x005C91F0, TRIBE_Master_Combat_Object__make_new_obj_new);
    setHook((void*)0x005C9270, TRIBE_Master_Combat_Object__make_new_master_new);
    setHook((void*)0x005C92E0, TRIBE_Master_Combat_Object__check_placement_new);
}
