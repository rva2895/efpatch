#include "stdafx.h"
#include "RGE_Master_Moving_Object.h"

// .rdata vftable of RGE_Master_Moving_Object
// (battlegrounds_x2.lst: .rdata:0065B304 RGE_Master_Moving_Object__vftable)
static RGE_Master_Moving_ObjectVtbl* const rge_master_moving_object_vftable =
    (RGE_Master_Moving_ObjectVtbl*)0x0065B304;

// Replacement for RGE_Master_Moving_Object::setup
// Original: RGE_Master_Moving_Object__setup @ 0x0048AFE0 (idb_functions.h:1876).
// ABI: __thiscall — this_ in ECX, 1 stack arg (retn 4), int result (always 1).
// Derived override: base Animated setup call (through the idb constant —
// routes into the replacement per the HOOK-ROUTING RULING), master_type
// forced to RGE_MASTER_TYPE_MOVING (0x1E), the eight Moving extension
// fields copied from other_object, then the turn-radius initialization:
// mTurnRadius = 0, mStationaryYawRevolutionTime = 0, and mTurnRadiusSpeed
// / mMaximumYawPerSecondMoving / mMaximumYawPerSecondStationary =
// SINGLE_MAXIMUM_VALUE (FLT_MAX), followed by the virtual dispatch
// vfptr->setTurnRadiusOffTurnSpeed(this) — slot @0x64 = entry 25 of
// RGE_Master_Moving_ObjectVtbl (position arithmetic: vtbl fields start at
// idb_types.h:14962, member at 14987). Extension field offsets
// 212-236 verified by the mechanical walker.
int __fastcall RGE_Master_Moving_Object__setup_new(
    RGE_Master_Moving_Object* this_, DWORD dummy,
    RGE_Master_Moving_Object* other_object)
{
    RGE_Master_Animated_Object__setup((RGE_Master_Animated_Object*)this_, (RGE_Master_Animated_Object*)other_object);
    this_->master_type = RGE_MASTER_TYPE_MOVING;
    this_->move_sprite = other_object->move_sprite;
    this_->run_sprite = other_object->run_sprite;
    this_->turn_speed = other_object->turn_speed;
    this_->size_class = other_object->size_class;
    this_->obj_trail_id = other_object->obj_trail_id;
    this_->obj_trail_options = other_object->obj_trail_options;
    this_->obj_trail_spacing = other_object->obj_trail_spacing;
    this_->move_algorithem = other_object->move_algorithem;
    this_->mTurnRadius = 0.0f;
    this_->mStationaryYawRevolutionTime = 0.0f;
    this_->mTurnRadiusSpeed = FLT_MAX;
    this_->mMaximumYawPerSecondMoving = FLT_MAX;
    this_->mMaximumYawPerSecondStationary = FLT_MAX;
    this_->vfptr->setTurnRadiusOffTurnSpeed(this_);
    return 1;
}

// Replacement for RGE_Master_Moving_Object::setup2
// Original: RGE_Master_Moving_Object__setup2 @ 0x0048B090 (idb_functions.h:1877).
// ABI: __thiscall — this_ in ECX, 3 stack args (retn 0Ch), int result (always 1).
// Base call, master_type forced, then the stream sequence: two i16 sprite
// indices (the original reuses the sounds/sprites PARAM SLOTS as the read
// buffers — asm 0x48B0AB/0x48B0BC; named locals here; the resolutions use
// the saved pointer registers, so the params stay live), the six
// extension fields, the two guarded sprite resolutions, then the five
// m* floats streamed (no FLT_MAX initialization in this path).
int __fastcall RGE_Master_Moving_Object__setup2_new(
    RGE_Master_Moving_Object* this_, DWORD dummy,
    int infile, RGE_Sprite** sprites, RGE_Sound** sounds)
{
    RGE_Master_Animated_Object__setup2((RGE_Master_Animated_Object*)this_, infile, sprites, sounds);
    this_->master_type = RGE_MASTER_TYPE_MOVING;
    __int16 move_sprite_ix;
    __int16 run_sprite_ix;
    rge_read(infile, &move_sprite_ix, sizeof(move_sprite_ix));
    rge_read(infile, &run_sprite_ix, sizeof(run_sprite_ix));
    rge_read(infile, &this_->turn_speed, sizeof(this_->turn_speed));
    rge_read(infile, &this_->size_class, sizeof(this_->size_class));
    rge_read(infile, &this_->obj_trail_id, sizeof(this_->obj_trail_id));
    rge_read(infile, &this_->obj_trail_options, sizeof(this_->obj_trail_options));
    rge_read(infile, &this_->obj_trail_spacing, sizeof(this_->obj_trail_spacing));
    rge_read(infile, &this_->move_algorithem, sizeof(this_->move_algorithem));
    this_->move_sprite = (move_sprite_ix < 0) ? 0 : sprites[move_sprite_ix];
    this_->run_sprite = (run_sprite_ix < 0) ? 0 : sprites[run_sprite_ix];
    rge_read(infile, &this_->mTurnRadius, sizeof(this_->mTurnRadius));
    rge_read(infile, &this_->mTurnRadiusSpeed, sizeof(this_->mTurnRadiusSpeed));
    rge_read(infile, &this_->mMaximumYawPerSecondMoving, sizeof(this_->mMaximumYawPerSecondMoving));
    rge_read(infile, &this_->mStationaryYawRevolutionTime, sizeof(this_->mStationaryYawRevolutionTime));
    rge_read(infile, &this_->mMaximumYawPerSecondStationary, sizeof(this_->mMaximumYawPerSecondStationary));
    return 1;
}

// Replacement for RGE_Master_Moving_Object::setup3
// Original: RGE_Master_Moving_Object__setup3 @ 0x0048B1B0 (idb_functions.h:1878).
// ABI: __thiscall — this_ in ECX, 4 stack args (retn 10h), int result (always 1).
// Base call, master_type forced, then ONE fscanf "%hd %hd %f %hd %hd %hd
// %f %hd" (asm 0x48B1D1-0x48B209): the %hd scratch targets are the
// infile/in_id/sprites/sounds PARAM SLOTS (original) — named locals here —
// interleaved with direct field reads (turn_speed %f, obj_trail_id %hd,
// obj_trail_spacing %f). Byte-field stores narrow the i16 scratches;
// the two sprite indices resolve guarded; then the turn-radius
// initialization in the asm's order and the vtable dispatch.
int __fastcall RGE_Master_Moving_Object__setup3_new(
    RGE_Master_Moving_Object* this_, DWORD dummy,
    FILE* infile, RGE_Sprite** sprites, RGE_Sound** sounds, __int16 in_id)
{
    RGE_Master_Animated_Object__setup3((RGE_Master_Animated_Object*)this_, infile, sprites, sounds, in_id);
    this_->master_type = RGE_MASTER_TYPE_MOVING;
    __int16 move_sprite_ix; __int16 run_sprite_ix;
    __int16 size_class_s; __int16 obj_options_s; __int16 move_algo_s;
    fscanf_internal(infile, "%hd %hd %f %hd %hd %hd %f %hd",
        &move_sprite_ix, &run_sprite_ix, &this_->turn_speed, &size_class_s,
        &this_->obj_trail_id, &obj_options_s, &this_->obj_trail_spacing, &move_algo_s);
    this_->size_class = (unsigned __int8)size_class_s;
    this_->obj_trail_options = (unsigned __int8)obj_options_s;
    this_->move_algorithem = (unsigned __int8)move_algo_s;
    this_->move_sprite = (move_sprite_ix < 0) ? 0 : sprites[move_sprite_ix];
    this_->run_sprite = (run_sprite_ix < 0) ? 0 : sprites[run_sprite_ix];
    this_->mTurnRadius = 0.0f;
    this_->mStationaryYawRevolutionTime = 0.0f;
    this_->mTurnRadiusSpeed = FLT_MAX;
    this_->mMaximumYawPerSecondMoving = FLT_MAX;
    this_->mMaximumYawPerSecondStationary = FLT_MAX;
    this_->vfptr->setTurnRadiusOffTurnSpeed(this_);
    return 1;
}

// Replacement for RGE_Master_Moving_Object::setup4
// Original: RGE_Master_Moving_Object__setup4 @ 0x0048B2A0 (idb_functions.h:1879).
// ABI: __thiscall — this_ in ECX, 4 stack args (retn 10h), int result (always 1).
// Base call, then list[copy_id] (copy_id was stream-read by the base
// setup4; UNCONDITIONAL sign-extended index, no negative guard —
// faithful). BASE-POINTER CORRECTION per the template rule: the IDA
// v6[1].field accesses at +0xD4..+0xEC are the referenced master's
// Moving extension fields (walker-confirmed offsets 212-236); the list
// element is re-typed RGE_Master_Moving_Object* accordingly. The eight
// extension fields copy when the reference is non-null; then the
// turn-radius constants (FLT_MAX x3 on mTurnRadiusSpeed/
// mMaximumYawPerSecondMoving/mMaximumYawPerSecondStationary, zero on
// mTurnRadius/mStationaryYawRevolutionTime), the vtable dispatch, and
// the final turn_speed stream read — OVERWRITING the copied turn_speed
// (later write wins; asm 0x48B35D-0x48B367).
int __fastcall RGE_Master_Moving_Object__setup4_new(
    RGE_Master_Moving_Object* this_, DWORD dummy,
    int infile, RGE_Master_Static_Object** list, RGE_Sprite** sprites, RGE_Sound** sounds)
{
    RGE_Master_Animated_Object__setup4((RGE_Master_Animated_Object*)this_, infile, list, sprites, sounds);
    RGE_Master_Moving_Object* other = (RGE_Master_Moving_Object*)list[this_->copy_id];
    this_->master_type = RGE_MASTER_TYPE_MOVING;
    if (other)
    {
        this_->move_sprite = other->move_sprite;
        this_->run_sprite = other->run_sprite;
        this_->turn_speed = other->turn_speed;
        this_->size_class = other->size_class;
        this_->obj_trail_id = other->obj_trail_id;
        this_->obj_trail_options = other->obj_trail_options;
        this_->obj_trail_spacing = other->obj_trail_spacing;
        this_->move_algorithem = other->move_algorithem;
    }
    this_->mTurnRadiusSpeed = FLT_MAX;
    this_->mMaximumYawPerSecondMoving = FLT_MAX;
    this_->mMaximumYawPerSecondStationary = FLT_MAX;
    this_->mTurnRadius = 0.0f;
    this_->mStationaryYawRevolutionTime = 0.0f;
    this_->vfptr->setTurnRadiusOffTurnSpeed(this_);
    rge_read(infile, &this_->turn_speed, sizeof(this_->turn_speed));
    return 1;
}

// Replacement for RGE_Master_Moving_Object::RGE_Master_Moving_Object2
// Original: RGE_Master_Moving_Object__RGE_Master_Moving_Object2 @ 0x0048AE10
// (idb_functions.h:1873). ABI: __thiscall — this_ in ECX, 4 stack args
// (retn 10h), returns this. SEH ctor-unwind frame folded per the
// make_new_obj pattern: on exception the ANIMATED destructor runs on
// this (the immediate base — its dtor chains to Static) and the
// exception propagates (asm unwind chunk 0x6422F0: mov ecx, [ebp-10h] /
// jmp RGE_Master_Animated_Object__destructor; handler = __CxxFrameHandler
// dispatch, unwind-only scope). Base Animated ctor2 called with
// do_setup = 0; then the five m* yaw defaults INLINE (FLT_MAX x3 on
// mTurnRadiusSpeed/mMaximumYawPerSecondMoving/mMaximumYawPerSecond-
// Stationary, zeros on mTurnRadius/mStationaryYawRevolutionTime — the
// asm order 0xF4/0xF8/0x100 then 0xF0/0xFC), Moving vfptr store, and
// if (do_setup) the DERIVED setup (Moving::setup2 — which re-reads all
// five m* floats from the stream, overwriting the defaults). No
// master_type store here (the derived setup sets it).
RGE_Master_Moving_Object* __fastcall RGE_Master_Moving_Object__RGE_Master_Moving_Object2_new(
    RGE_Master_Moving_Object* this_, DWORD dummy,
    int infile, RGE_Sprite** sprites, RGE_Sound** sounds, int do_setup)
{
    RGE_Master_Moving_Object* result = this_;
    __try
    {
        RGE_Master_Animated_Object__RGE_Master_Animated_Object2((RGE_Master_Animated_Object*)this_, infile, sprites, sounds, 0);
        this_->mTurnRadiusSpeed = FLT_MAX;
        this_->mMaximumYawPerSecondMoving = FLT_MAX;
        this_->mMaximumYawPerSecondStationary = FLT_MAX;
        this_->mTurnRadius = 0.0f;
        this_->mStationaryYawRevolutionTime = 0.0f;
        this_->vfptr = rge_master_moving_object_vftable;
        if (do_setup)
            RGE_Master_Moving_Object__setup2(this_, infile, sprites, sounds);
        result = this_;
    }
    __except (RGE_Master_Animated_Object__destructor((RGE_Master_Animated_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Moving_Object::RGE_Master_Moving_Object3
// Original: RGE_Master_Moving_Object__RGE_Master_Moving_Object3 @ 0x0048AEA0
// (idb_functions.h:1874). ABI: __thiscall — this_ in ECX, 5 stack args
// (retn 14h), returns this. Same SEH fold (unwind chunk 0x642310, same
// Animated-destructor action). Base Animated ctor3 with do_setup = 0;
// same inline m* defaults; if (do_setup) Moving::setup3.
RGE_Master_Moving_Object* __fastcall RGE_Master_Moving_Object__RGE_Master_Moving_Object3_new(
    RGE_Master_Moving_Object* this_, DWORD dummy,
    FILE* infile, RGE_Sprite** sprites, RGE_Sound** sounds, __int16 in_id, int do_setup)
{
    RGE_Master_Moving_Object* result = this_;
    __try
    {
        RGE_Master_Animated_Object__RGE_Master_Animated_Object3((RGE_Master_Animated_Object*)this_, infile, sprites, sounds, in_id, 0);
        this_->mTurnRadiusSpeed = FLT_MAX;
        this_->mMaximumYawPerSecondMoving = FLT_MAX;
        this_->mMaximumYawPerSecondStationary = FLT_MAX;
        this_->mTurnRadius = 0.0f;
        this_->mStationaryYawRevolutionTime = 0.0f;
        this_->vfptr = rge_master_moving_object_vftable;
        if (do_setup)
            RGE_Master_Moving_Object__setup3(this_, infile, sprites, sounds, in_id);
        result = this_;
    }
    __except (RGE_Master_Animated_Object__destructor((RGE_Master_Animated_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Moving_Object::RGE_Master_Moving_Object4
// Original: RGE_Master_Moving_Object__RGE_Master_Moving_Object4 @ 0x0048AF40
// (idb_functions.h:1875). ABI: __thiscall — this_ in ECX, 5 stack args
// (retn 14h), returns this. Same SEH fold (unwind chunk 0x642330, same
// Animated-destructor action). Base Animated ctor4 with do_setup = 0;
// same inline m* defaults; if (do_setup) Moving::setup4.
// Caller coverage: RGE_Master_Player__load_master_object / load_object
// call these ctors directly (the player-unit load path); the Action
// ctor chain builds on them.
RGE_Master_Moving_Object* __fastcall RGE_Master_Moving_Object__RGE_Master_Moving_Object4_new(
    RGE_Master_Moving_Object* this_, DWORD dummy,
    int infile, RGE_Master_Static_Object** list, RGE_Sprite** sprites, RGE_Sound** sounds, int do_setup)
{
    RGE_Master_Moving_Object* result = this_;
    __try
    {
        RGE_Master_Animated_Object__RGE_Master_Animated_Object4((RGE_Master_Animated_Object*)this_, infile, list, sprites, sounds, 0);
        this_->mTurnRadiusSpeed = FLT_MAX;
        this_->mMaximumYawPerSecondMoving = FLT_MAX;
        this_->mMaximumYawPerSecondStationary = FLT_MAX;
        this_->mTurnRadius = 0.0f;
        this_->mStationaryYawRevolutionTime = 0.0f;
        this_->vfptr = rge_master_moving_object_vftable;
        if (do_setup)
            RGE_Master_Moving_Object__setup4(this_, infile, list, sprites, sounds);
        result = this_;
    }
    __except (RGE_Master_Animated_Object__destructor((RGE_Master_Animated_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Moving_Object::destructor
// Original: RGE_Master_Moving_Object__destructor @ 0x0048B380 (idb_functions.h:1880).
// ABI: __thiscall — this_ in ECX, no stack args, void return, no dummy.
// Moving vftable store, then tail-jmp to the Animated destructor; the
// replacement calls it (call vs tail-jmp not observable). Base call
// routes through the idb constant -> hooked replacement per the
// HOOK-ROUTING RULING.
void __fastcall RGE_Master_Moving_Object__destructor_new(RGE_Master_Moving_Object* this_)
{
    this_->vfptr = rge_master_moving_object_vftable;
    RGE_Master_Animated_Object__destructor((RGE_Master_Animated_Object*)this_);
}

// Replacement for RGE_Master_Moving_Object::scalar_deleting_destructor
// Original: RGE_Master_Moving_Object__scalar_deleting_destructor @ 0x0048ADF0.
// ABI: __thiscall — this_ in ECX, 1 stack arg (retn 4), void* result (this).
// Same body as the vector_deleting variants: destructor, then
// operator_delete_internal on flags & 1, returning this.
void* __fastcall RGE_Master_Moving_Object__scalar_deleting_destructor_new(
    RGE_Master_Moving_Object* this_, DWORD dummy, unsigned int flags)
{
    RGE_Master_Moving_Object__destructor_new(this_);
    if ((flags & 1) != 0)
        operator_delete_internal(this_);
    return this_;
}

// Replacement for RGE_Master_Moving_Object::make_new_obj
// Original: RGE_Master_Moving_Object__make_new_obj @ 0x0048B390.
// ABI: __thiscall — this_ in ECX, 5 stack args (retn 10h), RGE_Static_Object* result.
// The runtime-object factory: recyclable -> recycle_object_in_to_game +
// guarded vtable dispatch (recycle_in_to_game slot @0x74 = entry 29,
// grounded in the Static/Animated make_new_obj rounds) -> the recycled
// object; else operator_new(0x13C) + RGE_Moving_Object runtime ctor
// (do_setup = 1). ALLOCATION VERIFIED: sizeof(RGE_Moving_Object) =
// 316 = 0x13C — sequential hand-walk over idb_types.h:6690-6767
// (pack 8; corrected after the first walk's 324 miscount, which
// skipped the embedded LOS_ExploreInfo ei and shifted subsequent
// fields). The runtime ctor constant: RGE_Moving_Object__RGE_Moving_
// Object @ idb_functions.h:2122 -> 0x004A0540. SEH ctor-unwind frame
// folded per the make_new_obj precedent (FUNCTION CHUNK 0x642350:
// operator_delete frees the allocation on exception).
RGE_Static_Object* __fastcall RGE_Master_Moving_Object__make_new_obj_new(
    RGE_Master_Moving_Object* this_, DWORD dummy,
    RGE_Player* owner, float x, float y, float z)
{
    RGE_Static_Object* result = 0;
    RGE_Moving_Object* new_obj = 0;
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
            new_obj = (RGE_Moving_Object*)operator_new_internal(sizeof(RGE_Moving_Object));
            if (new_obj)
                result = (RGE_Static_Object*)RGE_Moving_Object__RGE_Moving_Object(new_obj, this_, owner, x, y, z, 1);
        }
    }
    __except (operator_delete_internal(new_obj), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Moving_Object::make_new_master
// Original: RGE_Master_Moving_Object__make_new_master @ 0x0048B5B0.
// ABI: __thiscall — this_ in ECX, no stack args, RGE_Master_Static_
// Object* result. Master-template factory: operator_new(0x104) —
// sizeof(RGE_Master_Moving_Object) = 0x104 walker-verified — null-
// checked, then the base ANIMATED copy-ctor with do_setup = 0, the
// five m* yaw defaults INLINE (the same set the ctors write:
// mTurnRadiusSpeed/mMaxYawMoving/mMaxYawStationary = FLT_MAX,
// mTurnRadius/mStationaryYawRevTime = 0 — asm 0x48B5F3-0x48B620,
// offsets 0xF4/0xF8/0x100 then 0xF0/0xFC), Moving vfptr store, and
// the DERIVED setup (Moving::setup — which re-reads all five m*
// floats from the template, overwriting the defaults; also sets
// master_type = 30). No SEH frame (plain function). No master_type
// store here (the derived setup sets it).
RGE_Master_Static_Object* __fastcall RGE_Master_Moving_Object__make_new_master_new(
    RGE_Master_Moving_Object* this_)
{
    RGE_Master_Moving_Object* new_master =
        (RGE_Master_Moving_Object*)operator_new_internal(sizeof(RGE_Master_Moving_Object));
    RGE_Master_Static_Object* result = 0;
    __try
    {
        if (new_master)
        {
            RGE_Master_Animated_Object__RGE_Master_Animated_Object(
                (RGE_Master_Animated_Object*)new_master, (RGE_Master_Animated_Object*)this_, 0);
            new_master->mTurnRadiusSpeed = FLT_MAX;
            new_master->mMaximumYawPerSecondMoving = FLT_MAX;
            new_master->mMaximumYawPerSecondStationary = FLT_MAX;
            new_master->mTurnRadius = 0.0f;
            new_master->mStationaryYawRevolutionTime = 0.0f;
            new_master->vfptr = rge_master_moving_object_vftable;
            RGE_Master_Moving_Object__setup((RGE_Master_Moving_Object*)new_master,
                (RGE_Master_Moving_Object*)this_);
            result = (RGE_Master_Static_Object*)new_master;
        }
    }
    __except (operator_delete_internal(new_master), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Moving_Object::copy_obj
// Original: RGE_Master_Moving_Object__copy_obj @ 0x0048B660 (idb_functions.h:1889).
// ABI: __thiscall — this_ in ECX, 1 stack arg (retn 8), void return.
// No SEH frame (plain function). Base Animated copy_obj call, then
// the eight Moving extension copies. BASE-POINTER CORRECTION per the
// template rule: the IDA source[1].field accesses (source[1].master_
// type/name/gbg_name2/unit_line/min_tech_level/string_id/id/base_id =
// source + 208 + Static offsets 4/8/12/16/18/20/24/28) are the
// referenced master's Moving extension fields @212-236 — walker-
// confirmed; the source argument is re-typed RGE_Master_Moving_
// Object* accordingly. The original's base call targets the Animated
// copy_obj via the idb constant (unhooked — executes the original:
// Static copy + speed copy; the speed copy is redundant here since
// this function's copies cover the extension, and the base chain
// then completes through the original Animated/Static levels).
void __fastcall RGE_Master_Moving_Object__copy_obj_new(
    RGE_Master_Moving_Object* this_, DWORD dummy,
    RGE_Master_Static_Object* source)
{
    RGE_Master_Animated_Object__copy_obj(
        (RGE_Master_Animated_Object*)this_, source);
    this_->move_sprite = ((RGE_Master_Moving_Object*)source)->move_sprite;
    this_->run_sprite = ((RGE_Master_Moving_Object*)source)->run_sprite;
    this_->turn_speed = ((RGE_Master_Moving_Object*)source)->turn_speed;
    this_->size_class = ((RGE_Master_Moving_Object*)source)->size_class;
    this_->obj_trail_id = ((RGE_Master_Moving_Object*)source)->obj_trail_id;
    this_->obj_trail_options = ((RGE_Master_Moving_Object*)source)->obj_trail_options;
    this_->obj_trail_spacing = ((RGE_Master_Moving_Object*)source)->obj_trail_spacing;
    this_->move_algorithem = ((RGE_Master_Moving_Object*)source)->move_algorithem;
}

// Replacement for RGE_Master_Moving_Object::modify
// Original: RGE_Master_Moving_Object__modify @ 0x0048B6E0
// (idb_functions.h:1890). ABI: __thiscall — this_ in ECX, 2 stack args
// (retn 8), void return. No SEH frame (plain function).
// Derived override: flag == 6 -> turn_speed = amount (asm 0x48B6E4 cmp
// al,6 / 0x48B6FA store @0xDC = 220, the Moving extension field);
// otherwise the base Animated modify (idb constant -> our hooked
// replacement per the HOOK-ROUTING RULING).
void __fastcall RGE_Master_Moving_Object__modify_new(
    RGE_Master_Moving_Object* this_, DWORD dummy,
    float amount, unsigned __int8 flag)
{
    if (flag == 6)
        this_->turn_speed = amount;
    else
        RGE_Master_Animated_Object__modify((RGE_Master_Animated_Object*)this_, amount, flag);
}

// Replacement for RGE_Master_Moving_Object::modify_delta
// Original: RGE_Master_Moving_Object__modify_delta @ 0x0048B710
// (idb_functions.h:1891). ABI: same as modify (retn 8, void). No SEH.
// flag == 6 -> turn_speed = amount + turn_speed (asm fld/fadd/fstp
// 0x48B726-0x48B730); otherwise the base Animated modify_delta (idb
// constant @0x488A10 — HOOKED since the modify-family batch; routes
// into our replacement per the HOOK-ROUTING RULING).
void __fastcall RGE_Master_Moving_Object__modify_delta_new(
    RGE_Master_Moving_Object* this_, DWORD dummy,
    float amount, unsigned __int8 flag)
{
    if (flag == 6)
        this_->turn_speed = amount + this_->turn_speed;
    else
        RGE_Master_Animated_Object__modify_delta((RGE_Master_Animated_Object*)this_, amount, flag);
}

// Replacement for RGE_Master_Moving_Object::modify_percent
// Original: RGE_Master_Moving_Object__modify_percent @ 0x0048B740
// (idb_functions.h:1892). ABI: same as modify (retn 8, void). No SEH.
// flag == 6 -> turn_speed = amount * turn_speed (asm fld/fmul/fstp
// 0x48B756-0x48B760); otherwise the base Animated modify_percent (idb
// constant @0x488A40 — HOOKED since the modify-family batch; routes
// into our replacement per the HOOK-ROUTING RULING).
void __fastcall RGE_Master_Moving_Object__modify_percent_new(
    RGE_Master_Moving_Object* this_, DWORD dummy,
    float amount, unsigned __int8 flag)
{
    if (flag == 6)
        this_->turn_speed = amount * this_->turn_speed;
    else
        RGE_Master_Animated_Object__modify_percent((RGE_Master_Animated_Object*)this_, amount, flag);
}

// Replacement for RGE_Master_Moving_Object::small_save
// Original: @ 0x0048B770 (idb_functions.h:1893). __thiscall, 1 stack arg
// (retn 4), void. No SEH. Base Animated small_save (hooked -> routes),
// then turn_speed (asm 0x48B788, w4 @0xDC = 220).
void __fastcall RGE_Master_Moving_Object__small_save_new(
    RGE_Master_Moving_Object* this_, DWORD dummy, int outfile)
{
    RGE_Master_Animated_Object__small_save((RGE_Master_Animated_Object*)this_, outfile);
    rge_write(outfile, &this_->turn_speed, sizeof(this_->turn_speed));
}

// Replacement for RGE_Master_Moving_Object::save
// Original: @ 0x0048B7A0 (idb_functions.h:1894). Same ABI. The sprite-id
// locals are prepared BEFORE the base call (asm 0x48B7AA-0x48B7DA):
// move_sprite @212 / run_sprite @216 -> RGE_Sprite::id @+0x82 = 130
// (asm 0x48B7B4/0x48B7CF word reads); low-word-only stream writes (the
// __int16 local form is byte-identical, per the Action save note).
// Thirteen writes after the base Animated save: move id w2 (0x48B7EE),
// run id w2 (0x48B7FB), turn_speed w4 (0x48B80A), size_class w1
// (0x48B819), obj_trail_id w2 (0x48B828), obj_trail_options w1
// (0x48B837), obj_trail_spacing w4 (0x48B846), move_algorithem w1
// (0x48B855), then the five m* floats w4 (0x48B864-0x48B8A0: mTurnRadius
// @0xF0, mTurnRadiusSpeed @0xF4, mMaximumYawPerSecondMoving @0xF8,
// mStationaryYawRevolutionTime @0xFC, mMaximumYawPerSecondStationary
// @0x100).
void __fastcall RGE_Master_Moving_Object__save_new(
    RGE_Master_Moving_Object* this_, DWORD dummy, int outfile)
{
    __int16 move_id = -1;
    __int16 run_id = -1;
    if (this_->move_sprite)
        move_id = this_->move_sprite->id;
    if (this_->run_sprite)
        run_id = this_->run_sprite->id;
    RGE_Master_Animated_Object__save((RGE_Master_Animated_Object*)this_, outfile);
    rge_write(outfile, &move_id, sizeof(move_id));
    rge_write(outfile, &run_id, sizeof(run_id));
    rge_write(outfile, &this_->turn_speed, sizeof(this_->turn_speed));
    rge_write(outfile, &this_->size_class, sizeof(this_->size_class));
    rge_write(outfile, &this_->obj_trail_id, sizeof(this_->obj_trail_id));
    rge_write(outfile, &this_->obj_trail_options, sizeof(this_->obj_trail_options));
    rge_write(outfile, &this_->obj_trail_spacing, sizeof(this_->obj_trail_spacing));
    rge_write(outfile, &this_->move_algorithem, sizeof(this_->move_algorithem));
    rge_write(outfile, &this_->mTurnRadius, sizeof(this_->mTurnRadius));
    rge_write(outfile, &this_->mTurnRadiusSpeed, sizeof(this_->mTurnRadiusSpeed));
    rge_write(outfile, &this_->mMaximumYawPerSecondMoving, sizeof(this_->mMaximumYawPerSecondMoving));
    rge_write(outfile, &this_->mStationaryYawRevolutionTime, sizeof(this_->mStationaryYawRevolutionTime));
    rge_write(outfile, &this_->mMaximumYawPerSecondStationary, sizeof(this_->mMaximumYawPerSecondStationary));
}

void install_RGE_Master_Moving_Object()
{
    setHook((void*)0x0048AFE0, RGE_Master_Moving_Object__setup_new);
    setHook((void*)0x0048B090, RGE_Master_Moving_Object__setup2_new);
    setHook((void*)0x0048B1B0, RGE_Master_Moving_Object__setup3_new);
    setHook((void*)0x0048B2A0, RGE_Master_Moving_Object__setup4_new);
    setHook((void*)0x0048AE10, RGE_Master_Moving_Object__RGE_Master_Moving_Object2_new);
    setHook((void*)0x0048AEA0, RGE_Master_Moving_Object__RGE_Master_Moving_Object3_new);
    setHook((void*)0x0048AF40, RGE_Master_Moving_Object__RGE_Master_Moving_Object4_new);
    setHook((void*)0x0048B380, RGE_Master_Moving_Object__destructor_new);
    setHook((void*)0x0048ADF0, RGE_Master_Moving_Object__scalar_deleting_destructor_new);
    setHook((void*)0x0048B390, RGE_Master_Moving_Object__make_new_obj_new);
    setHook((void*)0x0048B5B0, RGE_Master_Moving_Object__make_new_master_new);
    setHook((void*)0x0048B660, RGE_Master_Moving_Object__copy_obj_new);
    setHook((void*)0x0048B6E0, RGE_Master_Moving_Object__modify_new);
    setHook((void*)0x0048B710, RGE_Master_Moving_Object__modify_delta_new);
    setHook((void*)0x0048B740, RGE_Master_Moving_Object__modify_percent_new);
    setHook((void*)0x0048B770, RGE_Master_Moving_Object__small_save_new);
    setHook((void*)0x0048B7A0, RGE_Master_Moving_Object__save_new);
}
