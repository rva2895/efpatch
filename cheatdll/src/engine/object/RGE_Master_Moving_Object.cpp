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
}
