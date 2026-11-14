#include "stdafx.h"
#include "TRIBE_Master_Building_Object.h"

// Save-format version gate (definition: casts.cpp:15) — the bldg_heal_rate
// patch (bldg_heal_rate_save.cpp) extends the Building(80) master stream
// with a garrison_heal_rate field, version-gated on the read side.
extern int current_save_game_version;

// .rdata vftable of TRIBE_Master_Building_Object
// (battlegrounds_x2.lst: .rdata:00667280 TRIBE_Master_Building_Object__vftable)
static TRIBE_Master_Building_ObjectVtbl* const tribe_master_building_object_vftable =
    (TRIBE_Master_Building_ObjectVtbl*)0x00667280;

// Replacement for TRIBE_Master_Building_Object::scalar_deleting_destructor
// Original: @ 0x005C6A30 (idb_functions.h:5098). ABI: __thiscall — this_
// in ECX, 1 stack arg (retn 4), void* result (this). Destructor + the
// game-side operator_delete on flags & 1, returning this (operator_
// delete_internal wrapper per the Static scalar_deleting precedent).
void* __fastcall TRIBE_Master_Building_Object__scalar_deleting_destructor_new(
    TRIBE_Master_Building_Object* this_, DWORD dummy, unsigned int flags)
{
    TRIBE_Master_Building_Object__destructor(this_);
    if ((flags & 1) != 0)
        operator_delete_internal(this_);
    return this_;
}

// Replacement for TRIBE_Master_Building_Object::TRIBE_Master_Building_Object2
// Original: @ 0x005C6A50 (idb_functions.h:5099). ABI: __thiscall — this_
// in ECX, 4 stack args (retn 10h), returns this. SEH ctor-unwind frame
// folded per the make_new_obj pattern: on exception the base TRIBE
// Master Combat destructor runs on this and the exception propagates
// (asm unwind chunk 0x6465E0: mov ecx, [ebp-10h] / jmp
// TRIBE_Master_Combat_Object__destructor; handler = __CxxFrameHandler
// dispatch, unwind-only scope). Body: base TRIBE Master Combat ctor2
// with do_setup = 0 (asm +31 calls it), the Building vfptr store
// (.rdata:00667280), if (do_setup) the derived setup2 (load-bearing
// guard, no collapse). Caller coverage: TRIBE_Combat_Object__setup_read
// and the Master_Player load path route through this tier.
TRIBE_Master_Building_Object* __fastcall TRIBE_Master_Building_Object__TRIBE_Master_Building_Object2_new(
    TRIBE_Master_Building_Object* this_, DWORD dummy,
    int infile, RGE_Sprite** sprites, RGE_Sound** sounds, int do_setup)
{
    TRIBE_Master_Building_Object* result = this_;
    __try
    {
        TRIBE_Master_Combat_Object__TRIBE_Master_Combat_Object2(
            (TRIBE_Master_Combat_Object*)this_, infile, sprites, sounds, 0);
        this_->vfptr = tribe_master_building_object_vftable;
        if (do_setup)
            TRIBE_Master_Building_Object__setup2(this_, infile, sprites, sounds);
        result = this_;
    }
    __except (TRIBE_Master_Combat_Object__destructor((TRIBE_Master_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for TRIBE_Master_Building_Object::TRIBE_Master_Building_Object3
// Original: @ 0x005C6AC0 (idb_functions.h:5100). ABI: __thiscall — this_
// in ECX, 5 stack args (retn 14h), returns this. SEH fold (unwind chunk
// 0x646600, same base-destructor action). Base TRIBE Master Combat
// ctor3 with do_setup = 0; if (do_setup) the derived setup3.
TRIBE_Master_Building_Object* __fastcall TRIBE_Master_Building_Object__TRIBE_Master_Building_Object3_new(
    TRIBE_Master_Building_Object* this_, DWORD dummy,
    FILE* infile, RGE_Sprite** sprites, RGE_Sound** sounds, __int16 in_id, int do_setup)
{
    TRIBE_Master_Building_Object* result = this_;
    __try
    {
        TRIBE_Master_Combat_Object__TRIBE_Master_Combat_Object3(
            (TRIBE_Master_Combat_Object*)this_, infile, sprites, sounds, in_id, 0);
        this_->vfptr = tribe_master_building_object_vftable;
        if (do_setup)
            TRIBE_Master_Building_Object__setup3(this_, infile, sprites, sounds, in_id);
        result = this_;
    }
    __except (TRIBE_Master_Combat_Object__destructor((TRIBE_Master_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for TRIBE_Master_Building_Object::TRIBE_Master_Building_Object4
// Original: @ 0x005C6B40 (idb_functions.h:5101). ABI: __thiscall — this_
// in ECX, 5 stack args (retn 14h), returns this. SEH fold (unwind chunk
// 0x646620, same base-destructor action). Base TRIBE Master Combat
// ctor4 with do_setup = 0; if (do_setup) the derived setup4.
TRIBE_Master_Building_Object* __fastcall TRIBE_Master_Building_Object__TRIBE_Master_Building_Object4_new(
    TRIBE_Master_Building_Object* this_, DWORD dummy,
    int infile, RGE_Master_Static_Object** list, RGE_Sprite** sprites, RGE_Sound** sounds, int do_setup)
{
    TRIBE_Master_Building_Object* result = this_;
    __try
    {
        TRIBE_Master_Combat_Object__TRIBE_Master_Combat_Object4(
            (TRIBE_Master_Combat_Object*)this_, infile, list, sprites, sounds, 0);
        this_->vfptr = tribe_master_building_object_vftable;
        if (do_setup)
            TRIBE_Master_Building_Object__setup4(this_, infile, list, sprites, sounds);
        result = this_;
    }
    __except (TRIBE_Master_Combat_Object__destructor((TRIBE_Master_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for TRIBE_Master_Building_Object::setup
// Original: @ 0x005C6BC0 (idb_functions.h:5102). ABI: __thiscall — this_
// in ECX, 1 stack arg (retn 4), int result (always 1).
// Derived override: base TRIBE Master Combat setup call (through the idb
// constant — routes into the replacement per the HOOK-ROUTING RULING),
// master_type forced to RGE_MASTER_TYPE_BUILDING (80), then the
// Building(80) extension copies. Extension offsets 456-551 verified by
// the mechanical struct walk AND by setup4/copy_obj's own v6[2].*
// base-pointer arithmetic (undead_sprite->460, los->468/470, obj_max->
// 472/474, radius_x/y->476-482, button_pict region->508 linked_y,
// construction_radius_x/y->524/526/528, death_sprite->456,
// elevation_flag->532, movement_type->536, attribute_type_held[1]->540,
// attribute_amount_held[0]->544). Copy order per the pseudocode
// (construction_sound LAST). Caller coverage:
// TRIBE_Master_Combat_Object__make_new_master's Building counterpart
// and the save path route through here.
int __fastcall TRIBE_Master_Building_Object__setup_new(
    TRIBE_Master_Building_Object* this_, DWORD dummy,
    TRIBE_Master_Building_Object* other_object)
{
    TRIBE_Master_Combat_Object__setup((TRIBE_Master_Combat_Object*)this_, (TRIBE_Master_Combat_Object*)other_object);
    this_->master_type = RGE_MASTER_TYPE_BUILDING;
    this_->construction_sprite = other_object->construction_sprite;
    this_->snow_sprite = other_object->snow_sprite;
    this_->building_connect_flag = other_object->building_connect_flag;
    this_->building_facet = other_object->building_facet;
    this_->build_and_go_away = other_object->build_and_go_away;
    this_->on_build_make_obj = other_object->on_build_make_obj;
    this_->on_build_make_tile = other_object->on_build_make_tile;
    this_->on_build_make_overlay = other_object->on_build_make_overlay;
    this_->on_build_make_tech = other_object->on_build_make_tech;
    this_->can_burn = other_object->can_burn;
    for (size_t i = 0; i < _countof(this_->linked_id); ++i)
    {
        this_->linked_id[i] = other_object->linked_id[i];
        this_->linked_x[i] = other_object->linked_x[i];
        this_->linked_y[i] = other_object->linked_y[i];
    }
    this_->construction_id = other_object->construction_id;
    this_->open_close_id = other_object->open_close_id;
    this_->open_close_sound = other_object->open_close_sound;
    this_->construction_sound = other_object->construction_sound;
    this_->garrison_units = other_object->garrison_units;
    this_->garrison_heal_rate = other_object->garrison_heal_rate;
    this_->garrison_repair_rate = other_object->garrison_repair_rate;
    this_->salvage_id = other_object->salvage_id;
    for (size_t i = 0; i < _countof(this_->pillage_attr); ++i)
        this_->pillage_attr[i] = other_object->pillage_attr[i];
    return 1;
}

// Replacement for TRIBE_Master_Building_Object::setup2
// Original: @ 0x005C6D30 (idb_functions.h:5103). ABI: __thiscall — this_
// in ECX, 3 stack args (retn 0Ch), int result (always 1).
// Base call, master_type forced, then the stream sequence: two __int16
// sprite-index scratches first (construction/snow — named locals; the
// resolves are DEFERRED to the tail, asm order), building_connect_flag
// (1), building_facet (2), build_and_go_away (1), the on_build_make_*
// block (2 x4), can_burn (1), the linked triplets x4 (id w2, x w4,
// y w4 per element), construction_id (2), open_close_id (2), two
// __int16 sound-index scratches (open_close/construction — named
// locals), garrison_units (1), garrison_heal_rate (4),
// garrison_repair_rate (4), salvage_id (2), pillage_attr x6 (1 each) —
// then the four guarded resolves (sprites vs SPRITES, sounds vs SOUNDS).
int __fastcall TRIBE_Master_Building_Object__setup2_new(
    TRIBE_Master_Building_Object* this_, DWORD dummy,
    int infile, RGE_Sprite** sprites, RGE_Sound** sounds)
{
    TRIBE_Master_Combat_Object__setup2((TRIBE_Master_Combat_Object*)this_, infile, sprites, sounds);
    this_->master_type = RGE_MASTER_TYPE_BUILDING;
    __int16 construction_sprite_ix;
    __int16 snow_sprite_ix;
    rge_read(infile, &construction_sprite_ix, sizeof(construction_sprite_ix));
    rge_read(infile, &snow_sprite_ix, sizeof(snow_sprite_ix));
    rge_read(infile, &this_->building_connect_flag, sizeof(this_->building_connect_flag));
    rge_read(infile, &this_->building_facet, sizeof(this_->building_facet));
    rge_read(infile, &this_->build_and_go_away, sizeof(this_->build_and_go_away));
    rge_read(infile, &this_->on_build_make_obj, sizeof(this_->on_build_make_obj));
    rge_read(infile, &this_->on_build_make_tile, sizeof(this_->on_build_make_tile));
    rge_read(infile, &this_->on_build_make_overlay, sizeof(this_->on_build_make_overlay));
    rge_read(infile, &this_->on_build_make_tech, sizeof(this_->on_build_make_tech));
    rge_read(infile, &this_->can_burn, sizeof(this_->can_burn));
    for (size_t i = 0; i < _countof(this_->linked_id); ++i)
    {
        rge_read(infile, &this_->linked_id[i], sizeof(this_->linked_id[i]));
        rge_read(infile, &this_->linked_x[i], sizeof(this_->linked_x[i]));
        rge_read(infile, &this_->linked_y[i], sizeof(this_->linked_y[i]));
    }
    rge_read(infile, &this_->construction_id, sizeof(this_->construction_id));
    rge_read(infile, &this_->open_close_id, sizeof(this_->open_close_id));
    __int16 open_close_sound_ix;
    __int16 construction_sound_ix;
    rge_read(infile, &open_close_sound_ix, sizeof(open_close_sound_ix));
    rge_read(infile, &construction_sound_ix, sizeof(construction_sound_ix));
    rge_read(infile, &this_->garrison_units, sizeof(this_->garrison_units));
    rge_read(infile, &this_->garrison_heal_rate, sizeof(this_->garrison_heal_rate));
    rge_read(infile, &this_->garrison_repair_rate, sizeof(this_->garrison_repair_rate));
    rge_read(infile, &this_->salvage_id, sizeof(this_->salvage_id));
    for (size_t i = 0; i < _countof(this_->pillage_attr); ++i)
        rge_read(infile, &this_->pillage_attr[i], sizeof(this_->pillage_attr[i]));
    this_->open_close_sound = (open_close_sound_ix < 0) ? 0 : sounds[open_close_sound_ix];
    this_->construction_sprite = (construction_sprite_ix < 0) ? 0 : sprites[construction_sprite_ix];
    this_->snow_sprite = (snow_sprite_ix < 0) ? 0 : sprites[snow_sprite_ix];
    this_->construction_sound = (construction_sound_ix < 0) ? 0 : sounds[construction_sound_ix];
    return 1;
}

// Replacement for TRIBE_Master_Building_Object::setup3
// Original: @ 0x005C6F50 (idb_functions.h:5104). ABI: __thiscall — this_
// in ECX, 4 stack args (retn 10h), int result (always 1).
// Base call, master_type forced, then THREE fscanf calls: a
// 10-conversion "%hd" block (construction/snow sprite scratches, the
// building_connect_flag scratch — the infile PARAM SLOT in the original,
// building_facet DIRECT, build_and_go_away scratch, the on_build_make_*
// block DIRECT, can_burn scratch); the linked triplets x4 as
// "%hd %f %f" per element; an 8-conversion block (construction_id/
// open_close_id DIRECT, two sound-index scratches, garrison_units
// scratch, garrison_heal/repair DIRECT, salvage_id DIRECT); pillage x6
// "%hd" into the in_id scratch (byte-narrowed). All byte stores and the
// four guarded resolves are DEFERRED to the tail (asm order 0x5C7030+).
int __fastcall TRIBE_Master_Building_Object__setup3_new(
    TRIBE_Master_Building_Object* this_, DWORD dummy,
    FILE* infile, RGE_Sprite** sprites, RGE_Sound** sounds, __int16 in_id)
{
    TRIBE_Master_Combat_Object__setup3((TRIBE_Master_Combat_Object*)this_, infile, sprites, sounds, in_id);
    this_->master_type = RGE_MASTER_TYPE_BUILDING;
    __int16 construction_sprite_s;
    __int16 snow_sprite_s;
    __int16 connect_s;
    __int16 build_and_go_s;
    __int16 can_burn_s;
    fscanf_internal(infile, "%hd %hd %hd %hd %hd %hd %hd %hd %hd %hd",
        &construction_sprite_s,
        &snow_sprite_s,
        &connect_s,
        &this_->building_facet,
        &build_and_go_s,
        &this_->on_build_make_obj,
        &this_->on_build_make_tile,
        &this_->on_build_make_overlay,
        &this_->on_build_make_tech,
        &can_burn_s);
    for (size_t i = 0; i < _countof(this_->linked_id); ++i)
        fscanf_internal(infile, "%hd %f %f",
            &this_->linked_id[i],
            &this_->linked_x[i],
            &this_->linked_y[i]);
    __int16 open_close_sound_ix;
    __int16 construction_sound_ix;
    __int16 garrison_units_s;
    fscanf_internal(infile, "%hd %hd %hd %hd %hd %f %f %hd",
        &this_->construction_id,
        &this_->open_close_id,
        &open_close_sound_ix,
        &construction_sound_ix,
        &garrison_units_s,
        &this_->garrison_heal_rate,
        &this_->garrison_repair_rate,
        &this_->salvage_id);
    for (size_t i = 0; i < _countof(this_->pillage_attr); ++i)
    {
        __int16 pillage_s;
        fscanf_internal(infile, "%hd", &pillage_s);
        this_->pillage_attr[i] = (unsigned __int8)pillage_s;
    }
    this_->building_connect_flag = (unsigned __int8)connect_s;
    this_->build_and_go_away = (unsigned __int8)build_and_go_s;
    this_->open_close_sound = (open_close_sound_ix < 0) ? 0 : sounds[open_close_sound_ix];
    this_->construction_sprite = (construction_sprite_s < 0) ? 0 : sprites[construction_sprite_s];
    this_->snow_sprite = (snow_sprite_s < 0) ? 0 : sprites[snow_sprite_s];
    this_->construction_sound = (construction_sound_ix < 0) ? 0 : sounds[construction_sound_ix];
    this_->can_burn = (unsigned __int8)can_burn_s;
    this_->garrison_units = (unsigned __int8)garrison_units_s;
    return 1;
}

// Replacement for TRIBE_Master_Building_Object::setup4
// Original: @ 0x005C7120 (idb_functions.h:5105). ABI: __thiscall — this_
// in ECX, 4 stack args (retn 10h), int result (always 1).
// Base call, then list[copy_id] (copy_id was stream-read by the base
// setup4; UNCONDITIONAL sign-extended index, no negative guard —
// faithful). BASE-POINTER CORRECTION per the template rule: the IDA
// v6[2].field accesses (v6 typed RGE_Master_Static_Object*, sizeof 208;
// v6[2] = +416) land exactly on the walked Building(80) extension
// offsets: undead_sprite@44 -> 460 construction_sprite, undead_flag@48
// -> 464 snow_sprite, los@52/54 -> 468 building_connect_flag + 470
// building_facet, obj_max@56/58 -> 472 build_and_go_away + 474
// on_build_make_obj, radius_x@60/62 -> 476 on_build_make_tile + 478
// on_build_make_overlay, radius_y@64/66 -> 480 on_build_make_tech + 482
// can_burn, button_pict@92 -> 508 linked_y walk, construction_radius_x@
// 108/110 -> 524 construction_id + 526 open_close_id, construction_
// radius_y@112 -> 528 open_close_sound, death_sprite@40 -> 456
// construction_sound, elevation_flag@116 -> 532 garrison_units,
// movement_type@120 -> 536 garrison_heal_rate, attribute_type_held[1]@
// 124 -> 540 garrison_repair_rate, attribute_amount_held[0]@128 -> 544
// salvage_id. The list element is re-typed TRIBE_Master_Building_
// Object* accordingly. The extension copies (incl. linked x4 and
// pillage x6) happen when the reference is non-null; then the stream
// tail re-reads building_facet ONLY (w2 — overwrites the copied facet,
// later write wins).
int __fastcall TRIBE_Master_Building_Object__setup4_new(
    TRIBE_Master_Building_Object* this_, DWORD dummy,
    int infile, RGE_Master_Static_Object** list, RGE_Sprite** sprites, RGE_Sound** sounds)
{
    TRIBE_Master_Combat_Object__setup4((TRIBE_Master_Combat_Object*)this_, infile, list, sprites, sounds);
    TRIBE_Master_Building_Object* other = (TRIBE_Master_Building_Object*)list[this_->copy_id];
    this_->master_type = RGE_MASTER_TYPE_BUILDING;
    if (other)
    {
        this_->construction_sprite = other->construction_sprite;
        this_->snow_sprite = other->snow_sprite;
        this_->building_connect_flag = other->building_connect_flag;
        this_->building_facet = other->building_facet;
        this_->build_and_go_away = other->build_and_go_away;
        this_->on_build_make_obj = other->on_build_make_obj;
        this_->on_build_make_tile = other->on_build_make_tile;
        this_->on_build_make_overlay = other->on_build_make_overlay;
        this_->on_build_make_tech = other->on_build_make_tech;
        this_->can_burn = other->can_burn;
        for (size_t i = 0; i < _countof(this_->linked_id); ++i)
        {
            this_->linked_id[i] = other->linked_id[i];
            this_->linked_x[i] = other->linked_x[i];
            this_->linked_y[i] = other->linked_y[i];
        }
        this_->construction_id = other->construction_id;
        this_->open_close_id = other->open_close_id;
        this_->open_close_sound = other->open_close_sound;
        this_->construction_sound = other->construction_sound;
        this_->garrison_units = other->garrison_units;
        this_->garrison_heal_rate = other->garrison_heal_rate;
        this_->garrison_repair_rate = other->garrison_repair_rate;
        this_->salvage_id = other->salvage_id;
        for (size_t j = 0; j < _countof(this_->pillage_attr); ++j)
            this_->pillage_attr[j] = other->pillage_attr[j];
    }
    rge_read(infile, &this_->building_facet, sizeof(this_->building_facet));
    // BAKED-IN PATCH (bldg_heal_rate_save.cpp:45, hook point 0x005C72AF —
    // the instruction after the facet read): the bldg_heal_rate patch
    // extends the Building(80) xref stream with a version-gated
    // garrison_heal_rate read (stub bldg_master_small_read: save-format
    // version >= 3 carries 4 extra bytes per Building master here).
    // Without this read the stream desyncs by 4 bytes per Building master
    // (the consistency-test crash). Stub: bldg_master_small_read.
    if (current_save_game_version >= 3)
        rge_read(infile, &this_->garrison_heal_rate, sizeof(this_->garrison_heal_rate));
    return 1;
}

// Replacement for TRIBE_Master_Building_Object::destructor
// Original: @ 0x005C72C0 (idb_functions.h:5106). ABI: __thiscall — this_
// in ECX, no stack args, void return, no dummy.
// The asm stores the Building vftable then jumps the base TRIBE Master
// Combat destructor; the replacement calls it (call vs tail-jmp is not
// observable). Base call routes through the idb constant -> hooked
// replacement per the HOOK-ROUTING RULING.
void __fastcall TRIBE_Master_Building_Object__destructor_new(TRIBE_Master_Building_Object* this_)
{
    this_->vfptr = tribe_master_building_object_vftable;
    TRIBE_Master_Combat_Object__destructor((TRIBE_Master_Combat_Object*)this_);
}

// Replacement for TRIBE_Master_Building_Object::copy_obj
// Original: @ 0x005C72D0 (idb_functions.h:5107). ABI: __thiscall — this_
// in ECX, 1 stack arg (retn 4), void return. No SEH frame (plain
// function). Base TRIBE Master Combat copy_obj call (through the idb
// constant -> our hooked replacement per the HOOK-ROUTING RULING), then
// the Building(80) extension copies. BASE-POINTER CORRECTION per the
// template rule: the IDA source[2].field accesses (source typed
// RGE_Master_Static_Object*, sizeof 208; source[2] = +416) land exactly
// on the walked extension offsets 456-551 (the same mapping as setup4);
// the source is re-typed TRIBE_Master_Building_Object*. Copy order per
// the pseudocode (construction_sound third); NO null guard (faithful).
// The original streams on_build_make_tile+overlay as one dword (416+60)
// — replicated as the two named-field copies (identical bytes).
void __fastcall TRIBE_Master_Building_Object__copy_obj_new(
    TRIBE_Master_Building_Object* this_, DWORD dummy,
    RGE_Master_Static_Object* source)
{
    TRIBE_Master_Combat_Object__copy_obj((TRIBE_Master_Combat_Object*)this_, source);
    TRIBE_Master_Building_Object* src = (TRIBE_Master_Building_Object*)source;
    this_->construction_sprite = src->construction_sprite;
    this_->snow_sprite = src->snow_sprite;
    this_->construction_sound = src->construction_sound;
    this_->building_connect_flag = src->building_connect_flag;
    this_->building_facet = src->building_facet;
    this_->build_and_go_away = src->build_and_go_away;
    this_->on_build_make_obj = src->on_build_make_obj;
    this_->on_build_make_tile = src->on_build_make_tile;
    this_->on_build_make_overlay = src->on_build_make_overlay;
    this_->on_build_make_tech = src->on_build_make_tech;
    this_->can_burn = src->can_burn;
    for (size_t i = 0; i < _countof(this_->linked_id); ++i)
    {
        this_->linked_id[i] = src->linked_id[i];
        this_->linked_x[i] = src->linked_x[i];
        this_->linked_y[i] = src->linked_y[i];
    }
    this_->construction_id = src->construction_id;
    this_->open_close_id = src->open_close_id;
    this_->open_close_sound = src->open_close_sound;
    this_->garrison_units = src->garrison_units;
    this_->garrison_heal_rate = src->garrison_heal_rate;
    this_->garrison_repair_rate = src->garrison_repair_rate;
    this_->salvage_id = src->salvage_id;
    for (size_t j = 0; j < _countof(this_->pillage_attr); ++j)
        this_->pillage_attr[j] = src->pillage_attr[j];
}

// Replacement for TRIBE_Master_Building_Object::modify
// Original: @ 0x005C7430 (idb_functions.h:5108). ABI: __thiscall — this_
// in ECX, 2 stack args (retn 8), void return. No SEH frame (plain
// function). flag == 17 -> building_facet = (i16)__ftol(amount); flag ==
// 108 -> garrison_heal_rate = amount (float); default -> the base TRIBE
// Master Combat modify (hooked -> routes per the HOOK-ROUTING RULING).
// Caller coverage: TRIBE_Master_Building_Object__modify is the vtable
// entry Building masters dispatch through.
void __fastcall TRIBE_Master_Building_Object__modify_new(
    TRIBE_Master_Building_Object* this_, DWORD dummy,
    float amount, unsigned __int8 flag)
{
    if (flag == 17)
        this_->building_facet = (__int16)(int)amount;
    else if (flag == 108)
        this_->garrison_heal_rate = amount;
    else
        TRIBE_Master_Combat_Object__modify((TRIBE_Master_Combat_Object*)this_, amount, flag);
}

// Replacement for TRIBE_Master_Building_Object::modify_delta
// Original: @ 0x005C7470 (idb_functions.h:5109). Same ABI. flag == 17 ->
// building_facet += (i16)__ftol(amount); flag == 108 -> garrison_heal_
// rate += amount; default -> base TRIBE Master Combat modify_delta.
void __fastcall TRIBE_Master_Building_Object__modify_delta_new(
    TRIBE_Master_Building_Object* this_, DWORD dummy,
    float amount, unsigned __int8 flag)
{
    if (flag == 17)
        this_->building_facet = (__int16)(this_->building_facet + (__int16)(int)amount);
    else if (flag == 108)
        this_->garrison_heal_rate = amount + this_->garrison_heal_rate;
    else
        TRIBE_Master_Combat_Object__modify_delta((TRIBE_Master_Combat_Object*)this_, amount, flag);
}

// Replacement for TRIBE_Master_Building_Object::modify_percent
// Original: @ 0x005C74C0 (idb_functions.h:5110). Same ABI. ASYMMETRY:
// percent handles ONLY flag == 108 (garrison_heal_rate *= amount) — no
// building_facet case (faithful); default -> base TRIBE Master Combat
// modify_percent.
void __fastcall TRIBE_Master_Building_Object__modify_percent_new(
    TRIBE_Master_Building_Object* this_, DWORD dummy,
    float amount, unsigned __int8 flag)
{
    if (flag == 108)
        this_->garrison_heal_rate = amount * this_->garrison_heal_rate;
    else
        TRIBE_Master_Combat_Object__modify_percent((TRIBE_Master_Combat_Object*)this_, amount, flag);
}

// Replacement for TRIBE_Master_Building_Object::small_save
// Original: @ 0x005C74F0 (idb_functions.h:5111). ABI: __thiscall — this_
// in ECX, 1 stack arg (retn 4), void return. No SEH frame (plain
// function). Base TRIBE Master Combat small_save call (hooked -> routes),
// then building_facet (asm, w2).
void __fastcall TRIBE_Master_Building_Object__small_save_new(
    TRIBE_Master_Building_Object* this_, DWORD dummy, int outfile)
{
    TRIBE_Master_Combat_Object__small_save((TRIBE_Master_Combat_Object*)this_, outfile);
    rge_write(outfile, &this_->building_facet, sizeof(this_->building_facet));
    // BAKED-IN PATCH (bldg_heal_rate_save.cpp:44, hook point 0x005C750D —
    // the small-save tail/epilogue): the patch adds an UNCONDITIONAL
    // garrison_heal_rate write to the small-save tail (stub bldg_master_
    // small_save — no version gate on the write side; the paired setup4
    // read is version-gated >= 3). Stub: bldg_master_small_save.
    rge_write(outfile, &this_->garrison_heal_rate, sizeof(this_->garrison_heal_rate));
}

// Replacement for TRIBE_Master_Building_Object::save
// Original: @ 0x005C7520 (idb_functions.h:5112). ABI: __thiscall — this_
// in ECX, 1 stack arg (retn 4), void return. No SEH frame (plain
// function). Base TRIBE Master Combat save call (hooked -> routes), then
// the four id locals are prepared (construction/snow sprites via
// RGE_Sprite::id @+0x82, construction/open_close sounds via RGE_Sound::
// id @+0xC — __int16 locals, low-word stream writes per the established
// form) and 21 writes in asm order: construction sprite id w2, snow
// sprite id w2, building_connect_flag w1, building_facet w2,
// build_and_go_away w1, on_build_make_* w2 x4, can_burn w1, linked
// triplets x4 (id w2, x w4, y w4), construction_id w2, open_close_id
// w2, open_close sound id w2, construction sound id w2, garrison_units
// w1, garrison_heal_rate w4, garrison_repair_rate w4, salvage_id w2,
// pillage_attr x6 w1.
void __fastcall TRIBE_Master_Building_Object__save_new(
    TRIBE_Master_Building_Object* this_, DWORD dummy, int outfile)
{
    TRIBE_Master_Combat_Object__save((TRIBE_Master_Combat_Object*)this_, outfile);
    __int16 construction_sprite_id = -1;
    __int16 snow_sprite_id = -1;
    __int16 construction_sound_id = -1;
    __int16 open_close_sound_id = -1;
    if (this_->construction_sprite)
        construction_sprite_id = this_->construction_sprite->id;
    if (this_->snow_sprite)
        snow_sprite_id = this_->snow_sprite->id;
    if (this_->construction_sound)
        construction_sound_id = this_->construction_sound->id;
    if (this_->open_close_sound)
        open_close_sound_id = this_->open_close_sound->id;
    rge_write(outfile, &construction_sprite_id, sizeof(construction_sprite_id));
    rge_write(outfile, &snow_sprite_id, sizeof(snow_sprite_id));
    rge_write(outfile, &this_->building_connect_flag, sizeof(this_->building_connect_flag));
    rge_write(outfile, &this_->building_facet, sizeof(this_->building_facet));
    rge_write(outfile, &this_->build_and_go_away, sizeof(this_->build_and_go_away));
    rge_write(outfile, &this_->on_build_make_obj, sizeof(this_->on_build_make_obj));
    rge_write(outfile, &this_->on_build_make_tile, sizeof(this_->on_build_make_tile));
    rge_write(outfile, &this_->on_build_make_overlay, sizeof(this_->on_build_make_overlay));
    rge_write(outfile, &this_->on_build_make_tech, sizeof(this_->on_build_make_tech));
    rge_write(outfile, &this_->can_burn, sizeof(this_->can_burn));
    for (size_t i = 0; i < _countof(this_->linked_id); ++i)
    {
        rge_write(outfile, &this_->linked_id[i], sizeof(this_->linked_id[i]));
        rge_write(outfile, &this_->linked_x[i], sizeof(this_->linked_x[i]));
        rge_write(outfile, &this_->linked_y[i], sizeof(this_->linked_y[i]));
    }
    rge_write(outfile, &this_->construction_id, sizeof(this_->construction_id));
    rge_write(outfile, &this_->open_close_id, sizeof(this_->open_close_id));
    rge_write(outfile, &open_close_sound_id, sizeof(open_close_sound_id));
    rge_write(outfile, &construction_sound_id, sizeof(construction_sound_id));
    rge_write(outfile, &this_->garrison_units, sizeof(this_->garrison_units));
    rge_write(outfile, &this_->garrison_heal_rate, sizeof(this_->garrison_heal_rate));
    rge_write(outfile, &this_->garrison_repair_rate, sizeof(this_->garrison_repair_rate));
    rge_write(outfile, &this_->salvage_id, sizeof(this_->salvage_id));
    for (size_t j = 0; j < _countof(this_->pillage_attr); ++j)
        rge_write(outfile, &this_->pillage_attr[j], sizeof(this_->pillage_attr[j]));
}

// Replacement for TRIBE_Master_Building_Object::make_new_obj2
// Original: @ 0x005C7710 (idb_functions.h:5113). ABI: __thiscall — this_
// in ECX, 5 stack args (retn 14h), RGE_Static_Object* result. The uID
// variant. DELTA vs the RGE_ factories: NO RECYCLE PATH (Building
// runtime objects never recycle). ALLOCATION VERIFIED: sizeof(TRIBE_
// Building_Object) = 632 = 0x278 — sequential walk over idb_types.h:
// 9726-9880 (the TRIBE_Combat_Object 452 prefix + the Building runtime
// extension: burning @452, last_burn_time @456, save_damage_percent/
// captured_unit_count @460-461, extra/research actions @464-471,
// villager_count/linked_owner/linked_child[4] @472-495, garrison/
// production-queue block @496-519, relic/raider/gather-point block
// @520-555, wonder_node/build_pts @556-563, frame/order/snow flags
// @564-573, gate endpoints @576-611, building_terrain_type through
// gbg_building_unknown_4 @612-631 -> 632). The runtime ctor constant:
// TRIBE_Building_Object__TRIBE_Building_Object @ idb_functions.h:3907
// (0x005531A0), 8 args incl. uID, do_setup = 1 (asm). SEH fold per the
// make_new_obj precedent (chunk 0x646640: operator_delete frees the
// allocation; make_new_obj2 and make_new_obj SHARE one compiler-merged
// handler label 0x64664B).
RGE_Static_Object* __fastcall TRIBE_Master_Building_Object__make_new_obj2_new(
    TRIBE_Master_Building_Object* this_, DWORD dummy,
    RGE_Player* owner, float x, float y, float z, int uID)
{
    RGE_Static_Object* result = 0;
    TRIBE_Building_Object* new_obj = 0;
    __try
    {
        new_obj = (TRIBE_Building_Object*)operator_new_internal(sizeof(TRIBE_Building_Object));
        if (new_obj)
            result = (RGE_Static_Object*)TRIBE_Building_Object__TRIBE_Building_Object(new_obj, this_, owner, x, y, z, uID, 1);
    }
    __except (operator_delete_internal(new_obj), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for TRIBE_Master_Building_Object::make_new_obj
// Original: @ 0x005C7790 (idb_functions.h:5114). ABI: __thiscall — this_
// in ECX, 4 stack args (retn 10h), RGE_Static_Object* result.
// No recycle path; operator_new(0x278) — sizeof(TRIBE_Building_Object)
// = 632 walker-verified — and the SECOND runtime ctor (no uID):
// TRIBE_Building_Object__TRIBE_Building_Object2 @ idb_functions.h:3911
// (0x00553410), do_setup = 1 (asm). SEH fold per the make_new_obj
// precedent (chunk 0x646660, same operator_delete shape).
RGE_Static_Object* __fastcall TRIBE_Master_Building_Object__make_new_obj_new(
    TRIBE_Master_Building_Object* this_, DWORD dummy,
    RGE_Player* owner, float x, float y, float z)
{
    RGE_Static_Object* result = 0;
    TRIBE_Building_Object* new_obj = 0;
    __try
    {
        new_obj = (TRIBE_Building_Object*)operator_new_internal(sizeof(TRIBE_Building_Object));
        if (new_obj)
            result = (RGE_Static_Object*)TRIBE_Building_Object__TRIBE_Building_Object2(new_obj, this_, owner, x, y, z, 1);
    }
    __except (operator_delete_internal(new_obj), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for TRIBE_Master_Building_Object::make_new_master
// Original: @ 0x005C7810 (idb_functions.h:5115). ABI: __thiscall — this_
// in ECX, no stack args, RGE_Master_Static_Object* result.
// Master-template factory: operator_new(0x228) — sizeof(TRIBE_Master_
// Building_Object) = 552 = 0x228 walker-verified — null-checked, then
// the base TRIBE Master Combat COPY-CTOR with do_setup = 0 (asm; the
// Building tier extends the Combat(70) one), the Building vfptr store,
// and the DERIVED setup (hooked -> routes into our replacement; it sets
// master_type = 80 and copies the whole Building(80) extension from
// this). NO master_type store and NO extension copies here. SEH fold per
// the make_new_master precedent (chunk 0x646680 — the standard
// two-entry shape, folded into the operator_delete filter).
RGE_Master_Static_Object* __fastcall TRIBE_Master_Building_Object__make_new_master_new(
    TRIBE_Master_Building_Object* this_)
{
    TRIBE_Master_Building_Object* new_master =
        (TRIBE_Master_Building_Object*)operator_new_internal(sizeof(TRIBE_Master_Building_Object));
    RGE_Master_Static_Object* result = 0;
    __try
    {
        if (new_master)
        {
            TRIBE_Master_Combat_Object__TRIBE_Master_Combat_Object(
                (TRIBE_Master_Combat_Object*)new_master, (TRIBE_Master_Combat_Object*)this_, 0);
            new_master->vfptr = tribe_master_building_object_vftable;
            TRIBE_Master_Building_Object__setup(new_master, this_);
            result = (RGE_Master_Static_Object*)new_master;
        }
    }
    __except (operator_delete_internal(new_master), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// NOTE: the four large gameplay functions are intentionally NOT replaced
// in this batch — each is a dedicated multi-phase decode (per the RGE
// check_placement precedent) and they cross-call:
//   draw                    @ 0x005C7880
//   alignment               @ 0x005C7B30
//   check_placement         @ 0x005C7B50  (calls TRIBE_Master_Combat_
//                             Object__check_placement twice, +6A/+483)
//   check_placement_for_units @ 0x005C8000

// Replacement for TRIBE_Master_Building_Object::alignment
// Original: @ 0x005C7B30 (idb_functions.h:5117). ABI: __thiscall — this_
// in ECX, 4 stack args (retn 10h), unsigned __int8 result. No SEH frame
// (plain function). Pure forwarder: base RGE Master Static alignment
// with center_flag FORCED to 1 (asm push 1 at 0x5C7B38; the original
// center_flag stack arg is unused). Call vs tail-jmp not observable.
unsigned __int8 __fastcall TRIBE_Master_Building_Object__alignment_new(
    TRIBE_Master_Building_Object* this_, DWORD dummy,
    float* col, float* row, RGE_Game_World* world, unsigned __int8 center_flag)
{
    return RGE_Master_Static_Object__alignment((RGE_Master_Static_Object*)this_, col, row, world, 1);
}

// NOTE: the three remaining large gameplay functions are intentionally
// NOT replaced yet — each is a dedicated multi-phase decode (per the RGE
// check_placement precedent) and they cross-call:
//   draw                              @ 0x005C7880
//   check_placement                   @ 0x005C7B50  (calls TRIBE_Master_
//                                       Combat_Object__check_placement
//                                       twice, +6A/+483)
//   check_placement_for_units         @ 0x005C8000
// TRIBE_Master_Combat_Object::check_placement @ 0x005C92E0 (the shared
// implementation) IS owned — see the TRIBE_Master_Combat_Object.cpp.

void install_TRIBE_Master_Building_Object()
{
    setHook((void*)0x005C6A30, TRIBE_Master_Building_Object__scalar_deleting_destructor_new);
    setHook((void*)0x005C6A50, TRIBE_Master_Building_Object__TRIBE_Master_Building_Object2_new);
    setHook((void*)0x005C6AC0, TRIBE_Master_Building_Object__TRIBE_Master_Building_Object3_new);
    setHook((void*)0x005C6B40, TRIBE_Master_Building_Object__TRIBE_Master_Building_Object4_new);
    setHook((void*)0x005C6BC0, TRIBE_Master_Building_Object__setup_new);
    setHook((void*)0x005C6D30, TRIBE_Master_Building_Object__setup2_new);
    setHook((void*)0x005C6F50, TRIBE_Master_Building_Object__setup3_new);
    setHook((void*)0x005C7120, TRIBE_Master_Building_Object__setup4_new);
    setHook((void*)0x005C72C0, TRIBE_Master_Building_Object__destructor_new);
    setHook((void*)0x005C72D0, TRIBE_Master_Building_Object__copy_obj_new);
    setHook((void*)0x005C7430, TRIBE_Master_Building_Object__modify_new);
    setHook((void*)0x005C7470, TRIBE_Master_Building_Object__modify_delta_new);
    setHook((void*)0x005C74C0, TRIBE_Master_Building_Object__modify_percent_new);
    setHook((void*)0x005C74F0, TRIBE_Master_Building_Object__small_save_new);
    setHook((void*)0x005C7520, TRIBE_Master_Building_Object__save_new);
    setHook((void*)0x005C7B30, TRIBE_Master_Building_Object__alignment_new);
    setHook((void*)0x005C7710, TRIBE_Master_Building_Object__make_new_obj2_new);
    setHook((void*)0x005C7790, TRIBE_Master_Building_Object__make_new_obj_new);
    setHook((void*)0x005C7810, TRIBE_Master_Building_Object__make_new_master_new);
}
