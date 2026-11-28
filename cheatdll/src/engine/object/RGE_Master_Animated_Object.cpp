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

// Replacement for RGE_Master_Animated_Object::make_new_obj
// Original: RGE_Master_Animated_Object__make_new_obj @ 0x00488870 (idb_functions.h:1912-region).
// ABI: __thiscall — this_ in ECX, 5 stack args (retn 10h), RGE_Static_Object* result.
// The runtime-object factory: recyclable -> recycle_object_in_to_game +
// guarded vtable dispatch (recycle_in_to_game slot @0x74 = entry 29,
// grounded in the Static make_new_obj round) -> the recycled object;
// else operator_new(0xB8) + RGE_Animated_Object ctor (do_setup = 1).
// ALLOCATION VERIFIED: sizeof(RGE_Animated_Object) = 184 = 0xB8
// (walker total 148 + the embedded LOS_ExploreInfo ei = 9 fields x 4
// = 36; LOSLastActions = typedef int). SEH ctor-unwind frame folded
// per the make_new_obj precedent: on exception operator_delete_internal
// frees the allocation and the exception propagates (asm funclet
// 0x642090: mov eax, [ebp+4] / call operator_delete).
RGE_Static_Object* __fastcall RGE_Master_Animated_Object__make_new_obj_new(
    RGE_Master_Animated_Object* this_, DWORD dummy,
    RGE_Player* owner, float x, float y, float z)
{
    RGE_Static_Object* result = 0;
    RGE_Animated_Object* new_obj = 0;
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
            new_obj = (RGE_Animated_Object*)operator_new_internal(sizeof(RGE_Animated_Object));
            if (new_obj)
                result = (RGE_Static_Object*)RGE_Animated_Object__RGE_Animated_Object(new_obj, this_, owner, x, y, z, 1);
        }
    }
    __except (operator_delete_internal(new_obj), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Animated_Object::make_new_master
// Original: RGE_Master_Animated_Object__make_new_master @ 0x00488940 (idb_functions.h:1913).
// ABI: __thiscall — this_ in ECX, no stack args, RGE_Master_Static_Object* result.
// Master-template factory: operator_new(0xD4) — sizeof(RGE_Master_
// Animated_Object) = 212 = 0xD4 walker-verified — null-checked, then
// the base Static COPY-CTOR with do_setup = 0, Animated vfptr store,
// the base Static SETUP (template copy from this), master_type = 20,
// and the Animated extension copy speed = this->speed. BASE-POINTER
// CORRECTION per the template rule: the IDA rendering
// v2[1].vfptr = LODWORD(this->speed) is the Animated extension store
// @208 (walker-confirmed) — not a vtable write. SEH fold: the asm
// unwind chunk 0x6420B0 operator_deletes the allocation (the chunk's
// second entry — the destructor-chained path — folds into the same
// filter per the make_new_obj precedent).
RGE_Master_Static_Object* __fastcall RGE_Master_Animated_Object__make_new_master_new(
    RGE_Master_Animated_Object* this_)
{
    RGE_Master_Animated_Object* new_master =
        (RGE_Master_Animated_Object*)operator_new_internal(sizeof(RGE_Master_Animated_Object));
    RGE_Master_Static_Object* result = 0;
    __try
    {
        if (new_master)
        {
            RGE_Master_Static_Object__RGE_Master_Static_Object(
                (RGE_Master_Static_Object*)new_master, (RGE_Master_Static_Object*)this_, 0);
            new_master->vfptr = rge_master_animated_object_vftable;
            RGE_Master_Static_Object__setup(
                (RGE_Master_Static_Object*)new_master, (RGE_Master_Static_Object*)this_);
            new_master->master_type = RGE_MASTER_TYPE_ANIMATED;
            new_master->speed = this_->speed;
            result = (RGE_Master_Static_Object*)new_master;
        }
    }
    __except (operator_delete_internal(new_master), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Animated_Object::copy_obj
// Original: RGE_Master_Animated_Object__copy_obj @ 0x004889C0.
// ABI: __thiscall — this_ in ECX, 1 stack arg (retn 8), void return.
// No SEH frame (plain function). Base Static copy_obj call, then the
// Animated extension copy: asm mov eax, [edi+0xD0] / mov [esi+0xD0]
// = this->speed = source->speed. BASE-POINTER CORRECTION per the
// template rule: the IDA rendering LODWORD(this->speed) =
// source[1].vfptr is source's Animated extension field @208 — not a
// vtable read.
void __fastcall RGE_Master_Animated_Object__copy_obj_new(
    RGE_Master_Animated_Object* this_, DWORD dummy,
    RGE_Master_Static_Object* source)
{
    RGE_Master_Static_Object__copy_obj((RGE_Master_Static_Object*)this_, source);
    this_->speed = ((RGE_Master_Animated_Object*)source)->speed;
}

// Replacement for RGE_Master_Animated_Object::modify
// Original: RGE_Master_Animated_Object__modify @ 0x004889E0
// (idb_functions.h:1821). ABI: __thiscall — this_ in ECX, 2 stack args
// (retn 8), void return. No SEH frame (plain function).
// Derived override: flag == 5 -> speed = amount (asm 0x4889E4 cmp al,5 /
// 0x4889FA store @0xD0 = 208, the Animated extension field); otherwise
// the base Static modify (idb constant -> hooked replacement per the
// HOOK-ROUTING RULING).
void __fastcall RGE_Master_Animated_Object__modify_new(
    RGE_Master_Animated_Object* this_, DWORD dummy,
    float amount, unsigned __int8 flag)
{
    if (flag == 5)
        this_->speed = amount;
    else
        RGE_Master_Static_Object__modify((RGE_Master_Static_Object*)this_, amount, flag);
}

// Replacement for RGE_Master_Animated_Object::modify_delta
// Original: @ 0x00488A10 (idb_functions.h:1822). __thiscall, retn 8, void.
// No SEH frame (plain function). flag == 5 -> speed += amount (fld/fadd/
// fstp @0xD0 = 208, asm 0x488A26-0x488A30); else the base Static
// modify_delta (idb constant -> hooked replacement per the HOOK-ROUTING
// RULING).
void __fastcall RGE_Master_Animated_Object__modify_delta_new(
    RGE_Master_Animated_Object* this_, DWORD dummy,
    float amount, unsigned __int8 flag)
{
    if (flag == 5)
        this_->speed = amount + this_->speed;
    else
        RGE_Master_Static_Object__modify_delta((RGE_Master_Static_Object*)this_, amount, flag);
}

// Replacement for RGE_Master_Animated_Object::modify_percent
// Original: @ 0x00488A40 (idb_functions.h:1823). flag == 5 -> speed *=
// amount (fld/fmul/fstp 0x488A56-0x488A60); else base Static
// modify_percent (hooked -> routes).
void __fastcall RGE_Master_Animated_Object__modify_percent_new(
    RGE_Master_Animated_Object* this_, DWORD dummy,
    float amount, unsigned __int8 flag)
{
    if (flag == 5)
        this_->speed = amount * this_->speed;
    else
        RGE_Master_Static_Object__modify_percent((RGE_Master_Static_Object*)this_, amount, flag);
}

// Replacement for RGE_Master_Animated_Object::small_save
// Original: @ 0x00488A70 (idb_functions.h:1824). __thiscall, 1 stack arg
// (retn 4), void. No SEH. Base Static small_save (hooked -> routes), then
// speed (asm 0x488A80, w4).
void __fastcall RGE_Master_Animated_Object__small_save_new(
    RGE_Master_Animated_Object* this_, DWORD dummy, int outfile)
{
    RGE_Master_Static_Object__small_save((RGE_Master_Static_Object*)this_, outfile);
    rge_write(outfile, &this_->speed, sizeof(this_->speed));
}

// Replacement for RGE_Master_Animated_Object::save
// Original: @ 0x00488AA0 (idb_functions.h:1825). Base Static save
// (hooked -> routes), then speed (asm 0x488AB0, w4).
void __fastcall RGE_Master_Animated_Object__save_new(
    RGE_Master_Animated_Object* this_, DWORD dummy, int outfile)
{
    RGE_Master_Static_Object__save((RGE_Master_Static_Object*)this_, outfile);
    rge_write(outfile, &this_->speed, sizeof(this_->speed));
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
    setHook((void*)0x00488870, RGE_Master_Animated_Object__make_new_obj_new);
    setHook((void*)0x00488940, RGE_Master_Animated_Object__make_new_master_new);
    setHook((void*)0x004889C0, RGE_Master_Animated_Object__copy_obj_new);
    setHook((void*)0x004889E0, RGE_Master_Animated_Object__modify_new);
    setHook((void*)0x00488A10, RGE_Master_Animated_Object__modify_delta_new);
    setHook((void*)0x00488A40, RGE_Master_Animated_Object__modify_percent_new);
    setHook((void*)0x00488A70, RGE_Master_Animated_Object__small_save_new);
    setHook((void*)0x00488AA0, RGE_Master_Animated_Object__save_new);
}
