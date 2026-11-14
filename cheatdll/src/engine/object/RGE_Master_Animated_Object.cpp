#include "stdafx.h"
#include "RGE_Master_Animated_Object.h"

// .rdata vftable of RGE_Master_Animated_Object
// (battlegrounds_x2.lst: .rdata:0065AF00 RGE_Master_Animated_Object__vftable)
static RGE_Master_Animated_ObjectVtbl* const rge_master_animated_object_vftable =
    (RGE_Master_Animated_ObjectVtbl*)0x0065AF00;

// Replacement for RGE_Master_Animated_Object::setup
// Original: RGE_Master_Animated_Object__setup @ 0x00488760 (idb_functions.h:1813).
// ABI: __thiscall — this_ in ECX, 1 stack arg (retn 4), int result (always 1).
// Thin derived override: calls the base Static setup (through the idb
// constant — under setHook's in-place entry patch that routes into the
// replacement; see the HOOK-ROUTING RULING in DECOMP_STATE.md), forces
// master_type to RGE_MASTER_TYPE_ANIMATED (0x14), and copies the single
// Animated extension field: float speed @208 (the flattened struct
// mirrors the base layout through outline_radius_z @204, then speed;
// struct total 0xD4). Extension offsets verified by the mechanical
// struct walk.
int __fastcall RGE_Master_Animated_Object__setup_new(
    RGE_Master_Animated_Object* this_, DWORD dummy,
    RGE_Master_Animated_Object* other_object)
{
    RGE_Master_Static_Object__setup((RGE_Master_Static_Object*)this_, (RGE_Master_Static_Object*)other_object);
    this_->master_type = RGE_MASTER_TYPE_ANIMATED;
    this_->speed = other_object->speed;
    return 1;
}

// Replacement for RGE_Master_Animated_Object::setup2
// Original: RGE_Master_Animated_Object__setup2 @ 0x00488790 (idb_functions.h:1814).
// ABI: __thiscall — this_ in ECX, 3 stack args (retn 0Ch), int result (always 1).
// Base call, master_type forced, then ONE 4-byte extension read into
// speed (asm 0x4887AA-0x4887B8: push 4 / lea edx, [esi+0D0h] / rge_read).
int __fastcall RGE_Master_Animated_Object__setup2_new(
    RGE_Master_Animated_Object* this_, DWORD dummy,
    int infile, RGE_Sprite** sprites, RGE_Sound** sounds)
{
    RGE_Master_Static_Object__setup2((RGE_Master_Static_Object*)this_, infile, sprites, sounds);
    this_->master_type = RGE_MASTER_TYPE_ANIMATED;
    rge_read(infile, &this_->speed, sizeof(this_->speed));
    return 1;
}

// Replacement for RGE_Master_Animated_Object::setup3
// Original: RGE_Master_Animated_Object__setup3 @ 0x004887D0 (idb_functions.h:1815).
// ABI: __thiscall — this_ in ECX, 4 stack args (retn 10h), int result (always 1).
// Base call, master_type forced, then ONE "%f" extension read into
// speed (asm 0x4887F3-0x488800: fscanf_internal(infile, "%f", this+0xD0)).
int __fastcall RGE_Master_Animated_Object__setup3_new(
    RGE_Master_Animated_Object* this_, DWORD dummy,
    FILE* infile, RGE_Sprite** sprites, RGE_Sound** sounds, __int16 in_id)
{
    RGE_Master_Static_Object__setup3((RGE_Master_Static_Object*)this_, infile, sprites, sounds, in_id);
    this_->master_type = RGE_MASTER_TYPE_ANIMATED;
    fscanf_internal(infile, "%f", &this_->speed);
    return 1;
}

// Replacement for RGE_Master_Animated_Object::setup4
// Original: RGE_Master_Animated_Object__setup4 @ 0x00488820 (idb_functions.h:1816).
// ABI: __thiscall — this_ in ECX, 4 stack args (retn 10h), int result (always 1).
// Base call, master_type forced, then ONE 4-byte extension read into
// speed (asm 0x48883F-0x48884D).
// Caller coverage note: RGE_Master_Doppleganger_Object derives from this
// class with no extra master fields (its masters allocate 0xD4 = the
// Animated size) and its constructors call these Animated setups — the
// hooks cover Doppleganger loading traffic too.
int __fastcall RGE_Master_Animated_Object__setup4_new(
    RGE_Master_Animated_Object* this_, DWORD dummy,
    int infile, RGE_Master_Static_Object** list, RGE_Sprite** sprites, RGE_Sound** sounds)
{
    RGE_Master_Static_Object__setup4((RGE_Master_Static_Object*)this_, infile, list, sprites, sounds);
    this_->master_type = RGE_MASTER_TYPE_ANIMATED;
    rge_read(infile, &this_->speed, sizeof(this_->speed));
    return 1;
}

// Replacement for RGE_Master_Animated_Object::RGE_Master_Animated_Object2
// Original: RGE_Master_Animated_Object__RGE_Master_Animated_Object2 @ 0x004885C0
// (idb_functions.h:1904). ABI: __thiscall — this_ in ECX, 4 stack args
// (retn 10h), returns this. SEH ctor-unwind frame folded per the
// make_new_obj pattern: on exception the Static destructor runs on this
// and the exception propagates (asm unwind chunk 0x642030: mov ecx,
// [ebp-10h] / jmp RGE_Master_Static_Object__destructor; handler =
// __CxxFrameHandler dispatch, unwind-only scope entry). Base Static
// ctor2 called with do_setup = 0; the do_setup body inlines the
// Animated extension op (Static setup2 + speed read + master_type)
// exactly as the asm sequences it.
RGE_Master_Animated_Object* __fastcall RGE_Master_Animated_Object__RGE_Master_Animated_Object2_new(
    RGE_Master_Animated_Object* this_, DWORD dummy,
    int infile, RGE_Sprite** sprites, RGE_Sound** sounds, int do_setup)
{
    RGE_Master_Animated_Object* result = this_;
    __try
    {
        RGE_Master_Static_Object__RGE_Master_Static_Object2((RGE_Master_Static_Object*)this_, infile, sprites, sounds, 0);
        this_->vfptr = rge_master_animated_object_vftable;
        if (do_setup)
        {
            RGE_Master_Static_Object__setup2((RGE_Master_Static_Object*)this_, infile, sprites, sounds);
            rge_read(infile, &this_->speed, sizeof(this_->speed));
            this_->master_type = RGE_MASTER_TYPE_ANIMATED;
        }
        result = this_;
    }
    __except (RGE_Master_Static_Object__destructor((RGE_Master_Static_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Animated_Object::RGE_Master_Animated_Object3
// Original: RGE_Master_Animated_Object__RGE_Master_Animated_Object3 @ 0x00488640
// (idb_functions.h:1905). ABI: __thiscall — this_ in ECX, 5 stack args
// (retn 14h), returns this. Same SEH fold (unwind chunk 0x642050, same
// Static-destructor action). Base Static ctor3 with do_setup = 0; the
// do_setup body = Static setup3 + fscanf_internal("%f") speed read + master_type.
RGE_Master_Animated_Object* __fastcall RGE_Master_Animated_Object__RGE_Master_Animated_Object3_new(
    RGE_Master_Animated_Object* this_, DWORD dummy,
    FILE* infile, RGE_Sprite** sprites, RGE_Sound** sounds, __int16 in_id, int do_setup)
{
    RGE_Master_Animated_Object* result = this_;
    __try
    {
        RGE_Master_Static_Object__RGE_Master_Static_Object3((RGE_Master_Static_Object*)this_, infile, sprites, sounds, in_id, 0);
        this_->vfptr = rge_master_animated_object_vftable;
        if (do_setup)
        {
            RGE_Master_Static_Object__setup3((RGE_Master_Static_Object*)this_, infile, sprites, sounds, in_id);
            fscanf_internal(infile, "%f", &this_->speed);
            this_->master_type = RGE_MASTER_TYPE_ANIMATED;
        }
        result = this_;
    }
    __except (RGE_Master_Static_Object__destructor((RGE_Master_Static_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Animated_Object::RGE_Master_Animated_Object4
// Original: RGE_Master_Animated_Object__RGE_Master_Animated_Object4 @ 0x004886D0
// (idb_functions.h:1906). ABI: __thiscall — this_ in ECX, 5 stack args
// (retn 14h), returns this. Same SEH fold (unwind chunk 0x642070, same
// Static-destructor action). Base Static ctor4 with do_setup = 0; the
// do_setup body = Static setup4 + speed read + master_type.
RGE_Master_Animated_Object* __fastcall RGE_Master_Animated_Object__RGE_Master_Animated_Object4_new(
    RGE_Master_Animated_Object* this_, DWORD dummy,
    int infile, RGE_Master_Static_Object** list, RGE_Sprite** sprites, RGE_Sound** sounds, int do_setup)
{
    RGE_Master_Animated_Object* result = this_;
    __try
    {
        RGE_Master_Static_Object__RGE_Master_Static_Object4((RGE_Master_Static_Object*)this_, infile, list, sprites, sounds, 0);
        this_->vfptr = rge_master_animated_object_vftable;
        if (do_setup)
        {
            RGE_Master_Static_Object__setup4((RGE_Master_Static_Object*)this_, infile, list, sprites, sounds);
            rge_read(infile, &this_->speed, sizeof(this_->speed));
            this_->master_type = RGE_MASTER_TYPE_ANIMATED;
        }
        result = this_;
    }
    __except (RGE_Master_Static_Object__destructor((RGE_Master_Static_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Animated_Object::destructor
// Original: RGE_Master_Animated_Object__destructor @ 0x00488860 (idb_functions.h:1817).
// ABI: __thiscall — this_ in ECX, no stack args, void return, no dummy.
// The asm stores the Animated vftable then TAIL-JUMPS the Static
// destructor; the replacement calls it (call vs tail-jmp is not
// observable). Base call routes through the idb constant -> hooked
// replacement per the HOOK-ROUTING RULING. Note: the ctor SEH unwind
// funclets (0x642030/0x642050/0x642070) jump into THIS function — the
// hooked replacement covers the exception path too.
void __fastcall RGE_Master_Animated_Object__destructor_new(RGE_Master_Animated_Object* this_)
{
    this_->vfptr = rge_master_animated_object_vftable;
    RGE_Master_Static_Object__destructor((RGE_Master_Static_Object*)this_);
}

// Replacement for RGE_Master_Animated_Object::vector_deleting_destructor
// Original: RGE_Master_Animated_Object__vector_deleting_destructor @ 0x004885A0.
// ABI: __thiscall — this_ in ECX, 1 stack arg (retn 4), void* result (this).
// Calls the (hooked) destructor, then operator_delete_internal on
// flags & 1 (the asm calls the game's operator_delete; the wrapper is
// the game-side deleter per the Static scalar_deleting precedent).
void* __fastcall RGE_Master_Animated_Object__vector_deleting_destructor_new(
    RGE_Master_Animated_Object* this_, DWORD dummy, unsigned int flags)
{
    RGE_Master_Animated_Object__destructor_new(this_);
    if ((flags & 1) != 0)
        operator_delete_internal(this_);
    return this_;
}

void install_RGE_Master_Animated_Object()
{
    setHook((void*)0x00488760, RGE_Master_Animated_Object__setup_new);
    setHook((void*)0x00488790, RGE_Master_Animated_Object__setup2_new);
    setHook((void*)0x004887D0, RGE_Master_Animated_Object__setup3_new);
    setHook((void*)0x00488820, RGE_Master_Animated_Object__setup4_new);
    setHook((void*)0x004885C0, RGE_Master_Animated_Object__RGE_Master_Animated_Object2_new);
    setHook((void*)0x00488640, RGE_Master_Animated_Object__RGE_Master_Animated_Object3_new);
    setHook((void*)0x004886D0, RGE_Master_Animated_Object__RGE_Master_Animated_Object4_new);
    setHook((void*)0x00488860, RGE_Master_Animated_Object__destructor_new);
    setHook((void*)0x004885A0, RGE_Master_Animated_Object__vector_deleting_destructor_new);
}
