#include "stdafx.h"
#include "TRIBE_Master_Building_Object.h"

// Save-format version gate (definition: casts.cpp:15) — the bldg_heal_rate
// patch (bldg_heal_rate_save.cpp) extends the Building(80) master stream
// with a garrison_heal_rate field, version-gated on the read side.
extern int current_save_game_version;

// Group-31 terrain gate helper from the onCheckPlacement_carbon patch
// (terrain.cpp:1026, __fastcall): version-gated - the legacy terrain set
// {10,13,17,18,19,20,21} below DAT v9, isCarbon(terrain) at v9+.
extern bool __fastcall checkCarbonTerrainBuildingBlock(unsigned __int8 terrain);

// Sort-line scratch global used by the sprite draw path
// (.data:0077FA64; repo constant: test.cpp:1418).
extern int* const SDI_Draw_Line;

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

// Replacement for TRIBE_Master_Building_Object::check_placement
// Original: TRIBE_Master_Building_Object__check_placement @ 0x005C7B50
// (idb_functions.h:5118). Range [0x5C7B50, 0x5C7FDF) (lst :763746 proc /
// :764208 endp, retn 30h = 12 stack args). ABI: __thiscall - this_ in ECX,
// unsigned __int8 result. Virtual: entry in
// .rdata:TRIBE_Master_Building_Object__vftable (lst :763747 DATA XREF);
// no direct callers in the lst - the entry hook covers all dispatch.
// Decode (asm 0x5C7B50-0x5C7FDE vs pseudocode .c:391470-391705,
// instruction-level, every gate reconciled):
//   - object_group 34/36 (Undeployed Cannon / Cannon) -> full-argument
//     forward to the base TRIBE_Master_Combat_Object__check_placement
//     (tail 0x5C7F99): cannons run the Combat(70) placement rules
//     entirely.
//   - else base pre-pass (asm push order 0x5C7B96-0x5C7BBA): original
//     player/col/row/obstructionObject/check_vis/check_flat/check_overlay/
//     construction_radius_flag, can_be_built_on_flag = 0,
//     check_objects = OUR check_movible_objects, ignore_obj, 0. Non-zero
//     result returns directly. (IDA's frame labels arg_24/check_objects
//     are swapped vs the prototype; the push order decides: +28 =
//     check_objects, +2C = ignore_obj, +30 = check_movible_objects.)
//   - when the ORIGINAL check_objects (+28) is set: one extra +-8.0 tile
//     object scan (flt_657DC0 = 8.0, lst :987837; __ftol conversions and
//     i16-truncated clamps 0x5C7BF5-0x5C7C7C; row-major over
//     map_row_offset, 32-byte tiles, TileObject_Count @13, Object_List
//     @20): ignore_obj / object_state >= 7 gates; id-50 master gate
//     (semantics not yet supplied — DECOMP_GAME_FACTS.md; moving units
//     with speed @208 never block); Gate(8)-vs-own-Wall(6) units;
//     desolid TRIBE_Master_Building_Object blockers (skipped unless
//     mutual allies; allies fall into the own-moving gate); own moving
//     state-2 units (skipped unless
//     not-mutual-allies OR gameType 6, AND owner id non-zero and !=
//     player id, asm 0x5C7D88-0x5C7DFD); can_be_built_on == 1 skips;
//     group-31 terrain gate (BAKED-IN PATCH, below); radius_z
//     substitution (type-80 id!=50 / Fish 23 / Shore Fish 25 -> 1.0,
//     0x5C7E49-0x5C7E86); gate orx > 0 && ory > 0 && (rz > 0 ||
//     id == 50 || Fruit Bush group 27, asm 0x5C7EBB-0x5C7EC6); AABB
//     |dx| < orx + x_radius && ory + y_radius > |dy| (strict < / >,
//     test ah,1 / test ah,41h at 0x5C7EE2/0x5C7F08/0x5C7F21) ->
//     *obstructionObject = obj->id, return 6. All exhausted paths return
//     the base result (0 at every reachable point; asm xor al,al at
//     0x5C7F77).
// BAKED-IN PATCH (terrain.cpp:1452, hook point 0x005C7E2C - the group-31
// switch decode at 0x5C7E29-0x5C7E42): the patched binary routes the
// terrain gate through checkCarbonTerrainBuildingBlock (terrain.cpp:1026;
// version-gated: legacy set {10,13,17,18,19,20,21} below DAT v9 - exactly
// the original jump table byte_5C7FEC/off_5C7FE4 - and isCarbon at v9+).
// The replacement calls the helper directly: identical behavior on every
// DAT version. The patch call site is retired in terrain.cpp.
unsigned __int8 __fastcall TRIBE_Master_Building_Object__check_placement_new(
    TRIBE_Master_Building_Object* this_, DWORD dummy,
    RGE_Player* player, float col, float row, int* obstructionObject,
    unsigned __int8 check_vis, unsigned __int8 check_flat,
    unsigned __int8 check_overlay, unsigned __int8 construction_radius_flag,
    unsigned __int8 can_be_built_on_flag, unsigned __int8 check_objects,
    RGE_Static_Object* ignore_obj, unsigned __int8 check_movible_objects)
{
    RGE_Map* map = player->world->map;   // RGE_Player::world @140 (0x8C), RGE_Game_World::map @52 (0x34)

    // Groups 34/36 (Undeployed Cannon / Cannon): cannons use the base
    // Combat placement rules entirely — full-argument forward
    // (asm 0x5C7B76/0x5C7B80 -> 0x5C7F99, all 12 stack args verbatim).
    if (this_->object_group == 34 || this_->object_group == 36)
    {
        return TRIBE_Master_Combat_Object__check_placement(
            (TRIBE_Master_Combat_Object*)this_, player, col, row,
            obstructionObject, check_vis, check_flat, check_overlay,
            construction_radius_flag, can_be_built_on_flag, check_objects,
            ignore_obj, check_movible_objects);
    }

    // Base pre-pass (asm push order 0x5C7B96-0x5C7BBA): the base runs
    // with can_be_built_on_flag = 0, check_objects = OUR
    // check_movible_objects, check_movible_objects = 0.
    unsigned __int8 base_result = TRIBE_Master_Combat_Object__check_placement(
        (TRIBE_Master_Combat_Object*)this_, player, col, row,
        obstructionObject, check_vis, check_flat, check_overlay,
        construction_radius_flag, 0, check_movible_objects, ignore_obj, 0);
    if (base_result)
        return base_result;
    if (!check_objects)   // the ORIGINAL check_objects (+28 byte, asm 0x5C7BC7)
        return base_result;

    float x_radius;
    float y_radius;
    if (construction_radius_flag)
    {
        x_radius = this_->construction_radius_x;   // @108 (0x6C)
        y_radius = this_->construction_radius_y;   // @112 (0x70)
    }
    else
    {
        x_radius = this_->radius_x;                // @60 (0x3C)
        y_radius = this_->radius_y;                // @64 (0x40)
    }

    // Scan rectangle +-8.0 tiles (flt_657DC0 = 8.0, lst :987837);
    // __ftol float->int32 + i16 truncation (asm 0x5C7BF5-0x5C7C44).
    __int16 scan_col0 = (__int16)(int)(col - 8.0f);
    __int16 scan_row0 = (__int16)(int)(row - 8.0f);
    __int16 scan_col1 = (__int16)(int)(col + 8.0f);
    __int16 scan_row1 = (__int16)(int)(row + 8.0f);
    if (scan_col0 < 0)                          // test bx,bx / jge idiom
        scan_col0 = 0;
    if (scan_col1 >= map->map_width)            // movsx di vs [map+8]
        scan_col1 = map->map_width - 1;
    if (scan_row0 < 0)
        scan_row0 = 0;
    if (scan_row1 >= map->map_height)           // vs [map+0xC]
        scan_row1 = map->map_height - 1;
    if (scan_row0 > scan_row1)                  // cmp si / jg -> return 0
        return base_result;

    for (__int16 ry = scan_row0; ry <= scan_row1; ++ry)
    {
        for (__int16 rx = scan_col0; rx <= scan_col1; ++rx)
        {
            const RGE_Tile& tile = map->map_row_offset[ry][rx];
            if (!tile.TileObject_Count)
                continue;
            RGE_Static_Object** obj_list = tile.Object_List;
            for (int i = 0; i < tile.TileObject_Count; ++i)
            {
                RGE_Static_Object* obj = obj_list[i];
                if (obj == ignore_obj)
                    continue;
                if (obj->object_state >= 7)     // @88, byte
                    continue;
                TRIBE_Master_Combat_Object* master =
                    (TRIBE_Master_Combat_Object*)obj->master_obj;   // @20
                // Id-50 master (semantics not yet supplied —
                // DECOMP_GAME_FACTS.md): moving units never block
                // placement. Runtime-type evidence: master_type >= 30 ->
                // the Animated-extension speed @208 exists in every tier
                // from Moving up (flattened structs mirror their base
                // prefix; walker-calibrated).
                if (this_->id == 50
                    && master->master_type >= RGE_MASTER_TYPE_MOVING
                    && master->speed > 0.0f)
                    continue;
                // Gate (8) placement: the player's own Wall (6) segments
                // never block — gates are built over walls.
                // (owner/player dual typing: identical field prefixes;
                // identity compare only.)
                if (this_->object_group == 8
                    && master->object_group == 6
                    && obj->owner == (TRIBE_Player*)player)
                    continue;
                // Desolid Building(80) blockers (group not 34/36):
                // skipped unless player and owner are mutual allies;
                // allies fall through into the own-moving gate below.
                // (NO gameType gate here — the gameType call at
                // 0x5C7DD3 belongs to the own-moving gate.)
                // Runtime-type evidence: master_type == 80 ->
                // TRIBE_Building_Object (factory identity,
                // DECOMP_HIERARCHY.md); desolid_flag @565.
                if (master->master_type == RGE_MASTER_TYPE_BUILDING
                    && master->object_group != 34
                    && master->object_group != 36
                    && ((TRIBE_Building_Object*)obj)->desolid_flag)
                {
                    if (!player->vfptr->isAlly(player, obj->owner->id)          // vtbl slot 9 (+0x24), idb_types.h:7128
                        || !obj->owner->vfptr->isAlly(obj->owner, player->id))  // TRIBE_PlayerVtbl slot 9 (+0x24), idb_types.h:1102
                    {
                        continue;
                    }
                }
                // Own moving units (state 2): skipped unless not-mutual-
                // allies OR gameType 6, AND the owner is a real other
                // player (asm 0x5C7D88-0x5C7DFD).
                if (master->master_type >= RGE_MASTER_TYPE_MOVING
                    && obj->object_state == 2
                    && master->speed > 0.0f)
                {
                    if (player->vfptr->isAlly(player, obj->owner->id)
                        && obj->owner->vfptr->isAlly(obj->owner, player->id)
                        && TRIBE_Game__gameType((TRIBE_Game*)*base_game) != 6)
                    {
                        continue;
                    }
                    if (obj->owner->id == 0 || obj->owner->id == player->id)
                        continue;
                }
                if (master->can_be_built_on == 1)   // @91
                    continue;
                // Tree/Carbon (31) blockers: terrain-gated.
                // BAKED: onCheckPlacement_carbon (terrain.cpp:1452) @ 0x005C7E2C -
                // the patched binary replaces the terrain_type switch
                // (jump table byte_5C7FEC/off_5C7FE4) with
                // checkCarbonTerrainBuildingBlock (terrain.cpp:1026):
                // legacy set {10,13,17,18,19,20,21} below DAT v9,
                // isCarbon at v9+.
                if (master->object_group == 31
                    && obj->object_state > 2
                    && obj->tile                                    // @40
                    && !checkCarbonTerrainBuildingBlock(obj->tile->terrain_type))   // @5
                {
                    continue;
                }
                float rz = master->radius_z;        // @68 (0x44)
                const float orx = master->radius_x; // @60 (0x3C)
                const float ory = master->radius_y; // @64 (0x40)
                const __int16 grp = master->object_group;
                // Fish (23) / Shore Fish (25): rz forced to 1.0 so
                // zero-radius fish still block placement.
                if (rz == 0.0f
                    && ((master->master_type == RGE_MASTER_TYPE_BUILDING && master->id != 50)
                        || grp == 23
                        || grp == 25))
                {
                    rz = 1.0f;                      // asm 0x5C7E80 fld const_f_one
                }
                // Fruit Bush (27) blockers bypass the rz > 0 test
                // (id-50 master too).
                if (orx > 0.0f && ory > 0.0f        // test ah,41h (strict >)
                    && (rz > 0.0f || master->id == 50 || grp == 27))
                {
                    float dx = obj->world_x - col;  // @72
                    float dy = obj->world_y - row;  // @76
                    if (dy < 0.0f) dy = -dy;        // test ah,1
                    if (dx < 0.0f) dx = -dx;
                    if (dx < orx + x_radius && ory + y_radius > dy)
                    {
                        if (obstructionObject)
                            *obstructionObject = obj->id;   // @16
                        return 6;
                    }
                }
            }
        }
    }
    return base_result;   // asm xor al,al at 0x5C7F77; base_result is 0 on every reachable path
}

// Replacement for TRIBE_Master_Building_Object::check_placement_for_units
// Original: @ 0x005C8000 (idb_functions.h:5119). Range [0x5C8000, 0x5C832F)
// (lst :764224 proc / :764524 endp, retn 14h = 5 stack args). ABI:
// __thiscall - this_ in ECX, unsigned __int8 result. Direct caller:
// TRIBE_Action_Build__set_state+58D (lst :764225, call site :599063).
// Decode (asm 0x5C8000-0x5C832E vs pseudocode .c:391707-391858,
// instruction-level): +-8.0 scan rectangle (flt_657DC0 = 8.0, __ftol + i16
// truncation/clamps 0x5C8039-0x5C80B5) using the placed object's OWN
// radius_x/radius_y (no construction-radius branch). Per object:
// ignore_obj / ignore_obj2 / object_state >= 7 gates; id-50 master gate
// (semantics not yet supplied - DECOMP_GAME_FACTS.md; moving units with
// speed @208 never block); desolid TRIBE_Building_Object gate (SIMPLER
// than check_placement: no group 34/36 exclusion, mutual allies alone
// pass - no gameType call, asm 0x5C8160-0x5C81A3); can_be_built_on == 1
// skips; blocker gate orx > 0 && ory > 0 && (rz > 0 || id == 50 ||
// Fruit Bush group 27) as a positive wrap (asm skip-chain 0x5C81B6-
// 0x5C81FA, truth-table-equivalent restructure; see the shipped RGE /
// Combat scan forms); AABB |obj->world_x - col| < radius_x &&
// |obj->world_y - row| < radius_y (both strict <, test ah,1 at
// 0x5C823E/0x5C8251). NO early exit: a collision sets the found flag and
// the scan runs to completion (several units can be affected); result =
// found ? 6 : 0. Collision action (asm 0x5C8256-0x5C82C7):
// obj->vfptr->get_target_obj(obj) (RGE_Static_ObjectVtbl slot 82 =
// +0x148, idb_types.h:4013) != ignore_obj AND master_type >= 30 AND
// object_state == 2 -> RGE_Moving_Object::evacuate_area(obj,
// (int)(col - radius_x - 2.0), (int)(row - radius_y - 1.0),
// (int)(radius_x + col + 1.0), (int)(radius_y + row + 1.0))
// (idb_functions.h:2209 @ 0x004A6BA0, unhooked - the asm's direct call;
// flt_6552F4 = 2.0, lst :986657; arg order verified against the asm push
// order, float left-association (a + b) + 1.0 preserved).
// Runtime-type evidence: master_type >= 30 -> the runtime object is
// RGE_Moving_Object or a derived tier (flat structs mirror their base
// prefix; walker-calibrated) - the evacuate_area cast.
unsigned __int8 __fastcall TRIBE_Master_Building_Object__check_placement_for_units_new(
    TRIBE_Master_Building_Object* this_, DWORD dummy,
    RGE_Player* player, float col, float row,
    RGE_Static_Object* ignore_obj, RGE_Static_Object* ignore_obj2)
{
    RGE_Map* map = player->world->map;   // RGE_Player::world @140 (0x8C), RGE_Game_World::map @52 (0x34)

    const float radius_x = this_->radius_x;   // @60 (0x3C)
    const float radius_y = this_->radius_y;   // @64 (0x40)

    // Scan rectangle +-8.0 tiles; __ftol float->int32 + i16 truncation
    // (asm 0x5C8039-0x5C8077), clamps identical to check_placement.
    __int16 scan_col0 = (__int16)(int)(col - 8.0f);
    __int16 scan_row0 = (__int16)(int)(row - 8.0f);
    __int16 scan_col1 = (__int16)(int)(col + 8.0f);
    __int16 scan_row1 = (__int16)(int)(row + 8.0f);
    if (scan_col0 < 0)
        scan_col0 = 0;
    if (scan_col1 >= map->map_width)
        scan_col1 = map->map_width - 1;
    if (scan_row0 < 0)
        scan_row0 = 0;
    if (scan_row1 >= map->map_height)
        scan_row1 = map->map_height - 1;
    if (scan_row0 > scan_row1)                  // cmp si,ax / jg -> return 0
        return 0;

    unsigned __int8 found = 0;                  // var_29
    for (__int16 ry = scan_row0; ry <= scan_row1; ++ry)
    {
        for (__int16 rx = scan_col0; rx <= scan_col1; ++rx)
        {
            const RGE_Tile& tile = map->map_row_offset[ry][rx];
            if (!tile.TileObject_Count)
                continue;
            RGE_Static_Object** obj_list = tile.Object_List;
            for (int i = 0; i < tile.TileObject_Count; ++i)
            {
                RGE_Static_Object* obj = obj_list[i];
                if (obj == ignore_obj || obj == ignore_obj2)
                    continue;
                if (obj->object_state >= 7)     // @88, byte
                    continue;
                TRIBE_Master_Combat_Object* master =
                    (TRIBE_Master_Combat_Object*)obj->master_obj;   // @20
                // Id-50 master (semantics not yet supplied - DECOMP_GAME_
                // FACTS.md): moving units never block. Runtime-type
                // evidence: master_type >= 30 -> the Animated-extension
                // speed @208 exists in every tier from Moving up.
                if (this_->id == 50
                    && master->master_type >= RGE_MASTER_TYPE_MOVING
                    && master->speed > 0.0f)
                    continue;
                // Desolid Building(80) blockers: mutual allies may place
                // over them (simpler than check_placement - no group
                // 34/36 exclusion, no gameType gate). Runtime-type
                // evidence: master_type == 80 -> TRIBE_Building_Object
                // (factory identity); desolid_flag @565.
                if (master->master_type == RGE_MASTER_TYPE_BUILDING
                    && ((TRIBE_Building_Object*)obj)->desolid_flag)
                {
                    if (player->vfptr->isAlly(player, obj->owner->id)          // vtbl slot 9 (+0x24), idb_types.h:7128
                        && obj->owner->vfptr->isAlly(obj->owner, player->id))  // TRIBE_PlayerVtbl slot 9 (+0x24), idb_types.h:1102
                    {
                        continue;
                    }
                }
                if (master->can_be_built_on == 1)   // @91
                    continue;
                // Blocker gate (positive wrap of the asm skip-chain
                // 0x5C81B6-0x5C81FA): Fruit Bush (27) bypasses the
                // rz > 0 test (id-50 master too).
                if (master->radius_x > 0.0f          // test ah,41h (strict >)
                    && master->radius_y > 0.0f
                    && (master->radius_z > 0.0f || master->id == 50
                        || master->object_group == 27))
                {
                    float dx = obj->world_x - col;   // @72
                    float dy = obj->world_y - row;   // @76
                    if (dy < 0.0f) dy = -dy;         // test ah,1
                    if (dx < 0.0f) dx = -dx;
                    if (dx < radius_x && dy < radius_y)   // both strict <
                    {
                        if (obj->vfptr->get_target_obj(obj) != ignore_obj   // slot 82 (+0x148), idb_types.h:4013
                            && master->master_type >= RGE_MASTER_TYPE_MOVING
                            && obj->object_state == 2)
                        {
                            // Runtime-type evidence: master_type >= 30 ->
                            // RGE_Moving_Object or derived (prefix-
                            // mirrored flat structs).
                            RGE_Moving_Object__evacuate_area(
                                (RGE_Moving_Object*)obj,
                                (int)(col - radius_x - 2.0f),   // flt_6552F4 = 2.0
                                (int)(row - radius_y - 1.0f),
                                (int)(radius_x + col + 1.0f),
                                (int)(radius_y + row + 1.0f));
                        }
                        found = 1;   // asm 0x5C82CC; NO early exit
                    }
                }
            }
        }
    }
    return found ? 6 : 0;   // asm 0x5C8312-0x5C8323 / xor al,al at 0x5C8329
}

// Replacement for TRIBE_Master_Building_Object::draw
// Original: TRIBE_Master_Building_Object__draw @ 0x005C7880
// (idb_functions.h:5116). Range [0x5C7880, 0x5C7B23) (lst :763432 proc /
// :763711 endp, retn 20h = 8 stack args), void result. ABI: __thiscall -
// this_ in ECX. Virtual: entry in .rdata:TRIBE_Master_Building_Object__
// vftable (lst :763433 DATA XREF).
// Decode (asm 0x5C7880-0x5C7B22 vs pseudocode .c:391315-391461,
// instruction-level):
//   1. sprite = this->vfptr->get_sprite(this, 0) - the Building vtbl
//      slot 1 (+0x04, idb_types.h:15845; the pushed 0 is a null
//      RGE_Static_Object*). POLYMORPHIC sprite selection (buildings
//      under construction render the construction sprite) - unlike the
//      Static base draw, which reads this_->sprite directly.
//   2. facet selection (asm 0x5C78A0-0x5C78C1): building_connect_flag
//      (@468) set -> (facet param != 0) ? facet - 1 : 2; else
//      building_facet (@470).
//   3. RGE_Sprite__draw(sprite, facet_sel, frame, x, y, sx = x, sy = y,
//      player_color, render_area, min_draw_level 0x0A, debugColumn 0).
//   4. Connected-neighbor overlay (asm 0x5C78FE-0x5C79B8), gated on
//      on_build_make_obj (@474) != 0xFFFF and sprite->draw_list_num
//      (@66) > 1: walk sprite->draw_list (@68) - an array of
//      RGE_Picture_List (idb_types.h:6356-6363; sprite @4, offset_x @8,
//      offset_y @10, stride sizeof = 16, asm-calibrated). Per entry:
//      skip entry->sprite == self (asm 0x5C7950); skip offset_y == 0
//      (asm 0x5C7955; offset_x is NOT tested); then
//      SDI_Draw_Line = saved + offset_y (asm 0x5C795D-0x5C7969 - the
//      repo constant test.cpp:1418 = .data:0077FA64, lst :1105270) and
//      RGE_Sprite__draw(entry->sprite, facet_sel, frame, x + offset_x,
//      y + offset_y, same sx/sy, player_color, render_area, 0x0A, 0)
//      (entry->facet @12 is unused by the asm). The global is restored
//      to the saved value on every exit path that entered the loop
//      (asm 0x5C79B4-0x5C79B8); the count <= 1 path exits before any
//      modification (asm 0x5C790E).
//      RESTRUCTURES (documented): the asm re-checks count <= 0 after
//      count > 1 already gated the block (0x5C7920-0x5C7929) - dead
//      guard, collapsed per the redundant-guard rule (count > 1
//      implies count > 0); the asm tests the computed entry ADDRESS
//      each iteration (0x5C793F-0x5C794A) - the compiler's partial
//      hoist of the null-draw_list guard - hoisted here to a single
//      entries null-check (identical exit path through the restore;
//      the in-loop deref at i > 0 with a null table is UB in the
//      original).
//   5. draw_frame != 0: selection-outline pass identical to the Static
//      base draw - project the rectangle corners (+rx,-ry), (+rx,+ry),
//      (-rx,+ry), (-rx,-ry) (radius_x/radius_y @60/64) through
//      RGE_Map::get_point on (*base_game)->world->map, then two
//      DClipInfo_List::AddGDINode outline nodes into SDI_List: a +1
//      offset pass with DrawColor 0 and SortLine y + 1, then the exact
//      pass with frame_color and SortLine y; GDI_Type 1, DrawLevel
//      0x0B, Object_ID = *SDI_Object_ID on both.
//   RESTRUCTURE (documented, Static draw precedent): the original
//   reuses the facet/frame/draw_frame/player_color/render_area/x/y
//   param stack slots as the get_point out buffers; the replacement
//   uses dedicated locals (params consumed before reuse - identical
//   behavior).
void __fastcall TRIBE_Master_Building_Object__draw_new(
    TRIBE_Master_Building_Object* this_, DWORD dummy,
    TDrawArea* render_area, __int16 x, __int16 y, RGE_Color_Table* player_color,
    int facet, int frame, int draw_frame, unsigned __int8 frame_color)
{
    RGE_Sprite* sprite = this_->vfptr->get_sprite(this_, 0);   // vtbl slot 1 (+0x04), idb_types.h:15845
    if (!sprite)
        return;

    int facet_sel = this_->building_facet;                     // @470 (0x1D6)
    if (this_->building_connect_flag)                          // @468 (0x1D4)
    {
        if (facet)
            facet_sel = facet - 1;
        else
            facet_sel = 2;                                     // asm 0x5C78B9
    }

    RGE_Sprite__draw(sprite, facet_sel, frame, x, y, x, y, player_color, render_area, 0x0A, 0);

    // Connected-neighbor overlay (asm 0x5C78FE-0x5C79B8).
    if (this_->on_build_make_obj != -1)                        // @474, word vs 0xFFFF (asm 0x5C78F0)
    {
        if (sprite->draw_list_num > 1)                         // @66 (asm 0x5C7902-0x5C7906)
        {
            const int saved_draw_line = *SDI_Draw_Line;        // asm 0x5C7914-0x5C791A
            RGE_Picture_List* entries = sprite->draw_list;     // @68
            // The asm tests the computed entry address per iteration
            // (0x5C793F-0x5C794A) - the compiler's partial hoist of the
            // null-draw_list guard; hoisted here (identical exit path
            // through the restore; an in-loop deref at i > 0 with a
            // null table is UB in the original).
            if (entries)
            {
                for (int i = 0; i < sprite->draw_list_num; ++i)
                {
                    const RGE_Picture_List& entry = entries[i];
                    if (entry.sprite == sprite)                // self-reference skip (asm 0x5C7950)
                        continue;
                    if (entry.offset_y == 0)                   // asm 0x5C7955
                        continue;
                    *SDI_Draw_Line = saved_draw_line + entry.offset_y;   // asm 0x5C795D-0x5C7969
                    RGE_Sprite__draw(entry.sprite, facet_sel, frame,
                        x + entry.offset_x, y + entry.offset_y,
                        x + entry.offset_x, y + entry.offset_y,
                        player_color, render_area, 0x0A, 0);   // asm 0x5C7961-0x5C798C
                }
            }
            *SDI_Draw_Line = saved_draw_line;                  // restore (asm 0x5C79B4-0x5C79B8)
        }
    }

    if (draw_frame)                                            // asm 0x5C79BE-0x5C79C4
    {
        RGE_Map* map = (*base_game)->world->map;               // world @0x420, map @0x34
        const float outline_rx = this_->radius_x;              // @60 (0x3C)
        const float outline_ry = this_->radius_y;              // @64 (0x40)
        __int16 c1x; __int16 c1y;
        __int16 c2x; __int16 c2y;
        __int16 c3x; __int16 c3y;
        __int16 c4x; __int16 c4y;
        RGE_Map__get_point(map, &c1x, &c1y,  outline_rx, -outline_ry, 0.0f, x, y);
        RGE_Map__get_point(map, &c2x, &c2y,  outline_rx,  outline_ry, 0.0f, x, y);
        RGE_Map__get_point(map, &c3x, &c3y, -outline_rx,  outline_ry, 0.0f, x, y);
        RGE_Map__get_point(map, &c4x, &c4y, -outline_rx, -outline_ry, 0.0f, x, y);
        DClipInfo_List__AddGDINode(*SDI_List, 1, y + 1,
            c1x + 1, c1y + 1, c2x + 1, c2y + 1, c3x + 1, c3y + 1, c4x + 1, c4y + 1,
            0x0B, 0, *SDI_Object_ID);
        DClipInfo_List__AddGDINode(*SDI_List, 1, y,
            c1x, c1y, c2x, c2y, c3x, c3y, c4x, c4y,
            0x0B, frame_color, *SDI_Object_ID);
    }
}

// BUILDING TIER COMPLETE: all 22 exports of TRIBE_Master_Building_Object
// are owned and hook-routed. The shared base implementation
// TRIBE_Master_Combat_Object::check_placement @ 0x005C92E0 is owned in
// TRIBE_Master_Combat_Object.cpp.

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
    setHook((void*)0x005C7B50, TRIBE_Master_Building_Object__check_placement_new);
    setHook((void*)0x005C8000, TRIBE_Master_Building_Object__check_placement_for_units_new);
    setHook((void*)0x005C7710, TRIBE_Master_Building_Object__make_new_obj2_new);
    setHook((void*)0x005C7790, TRIBE_Master_Building_Object__make_new_obj_new);
    setHook((void*)0x005C7810, TRIBE_Master_Building_Object__make_new_master_new);
    setHook((void*)0x005C7880, TRIBE_Master_Building_Object__draw_new);
}
