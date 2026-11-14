#include "stdafx.h"
#include "RGE_Master_Doppleganger_Object.h"

// .rdata vftable of RGE_Master_Doppleganger_Object
// (battlegrounds_x2.lst: .rdata:0065B100 RGE_Master_Doppleganger_Object__vftable)
static RGE_Master_Doppleganger_ObjectVtbl* const rge_master_doppleganger_object_vftable =
    (RGE_Master_Doppleganger_ObjectVtbl*)0x0065B100;

// Replacement for RGE_Master_Doppleganger_Object::RGE_Master_Doppleganger_Object2
// Original: RGE_Master_Doppleganger_Object__RGE_Master_Doppleganger_Object2 @ 0x0048A370.
// ABI: __thiscall — this_ in ECX, 4 stack args (retn 10h), returns this.
// Doppleganger masters are Animated masters with a vfptr swap and no
// extra fields (0xD4 = the Animated size). SEH ctor-unwind frame folded
// per the make_new_obj pattern: on exception the ANIMATED destructor
// runs on this (asm unwind chunk 0x642190: mov ecx, [ebp-10h] / jmp
// RGE_Master_Animated_Object__destructor; handler = __CxxFrameHandler
// dispatch, unwind-only scope). Base Animated ctor2 called with
// do_setup = 0; if (do_setup) the Animated setup (which chains to
// Static) and master_type forced to RGE_MASTER_TYPE_DOPPELGANGER
// (0x19). The if (do_setup) is load-bearing (no guard collapse).
RGE_Master_Doppleganger_Object* __fastcall RGE_Master_Doppleganger_Object__RGE_Master_Doppleganger_Object2_new(
    RGE_Master_Doppleganger_Object* this_, DWORD dummy,
    int infile, RGE_Sprite** sprites, RGE_Sound** sounds, int do_setup)
{
    RGE_Master_Doppleganger_Object* result = this_;
    __try
    {
        RGE_Master_Animated_Object__RGE_Master_Animated_Object2((RGE_Master_Animated_Object*)this_, infile, sprites, sounds, 0);
        this_->vfptr = rge_master_doppleganger_object_vftable;
        if (do_setup)
        {
            RGE_Master_Animated_Object__setup2((RGE_Master_Animated_Object*)this_, infile, sprites, sounds);
            this_->master_type = RGE_MASTER_TYPE_DOPPELGANGER;
        }
        result = this_;
    }
    __except (RGE_Master_Animated_Object__destructor((RGE_Master_Animated_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Doppleganger_Object::RGE_Master_Doppleganger_Object3
// Original: RGE_Master_Doppleganger_Object__RGE_Master_Doppleganger_Object3 @ 0x0048A3F0.
// ABI: __thiscall — this_ in ECX, 5 stack args (retn 14h), returns this.
// Same SEH fold (unwind chunk 0x6421B0, same Animated-destructor
// action). Base Animated ctor3 with do_setup = 0; if (do_setup) the
// Animated setup + master_type forced + recyclable forced to 1
// (asm 0x48A454: byte store @0xBC = 188).
RGE_Master_Doppleganger_Object* __fastcall RGE_Master_Doppleganger_Object__RGE_Master_Doppleganger_Object3_new(
    RGE_Master_Doppleganger_Object* this_, DWORD dummy,
    FILE* infile, RGE_Sprite** sprites, RGE_Sound** sounds, __int16 in_id, int do_setup)
{
    RGE_Master_Doppleganger_Object* result = this_;
    __try
    {
        RGE_Master_Animated_Object__RGE_Master_Animated_Object3((RGE_Master_Animated_Object*)this_, infile, sprites, sounds, in_id, 0);
        this_->vfptr = rge_master_doppleganger_object_vftable;
        if (do_setup)
        {
            RGE_Master_Animated_Object__setup3((RGE_Master_Animated_Object*)this_, infile, sprites, sounds, in_id);
            this_->master_type = RGE_MASTER_TYPE_DOPPELGANGER;
            this_->recyclable = 1;
        }
        result = this_;
    }
    __except (RGE_Master_Animated_Object__destructor((RGE_Master_Animated_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Doppleganger_Object::RGE_Master_Doppleganger_Object4
// Original: RGE_Master_Doppleganger_Object__RGE_Master_Doppleganger_Object4 @ 0x0048A480.
// ABI: __thiscall — this_ in ECX, 5 stack args (retn 14h), returns this.
// Same SEH fold (unwind chunk 0x6421D0, same Animated-destructor
// action). Base Animated ctor4 with do_setup = 0; if (do_setup) the
// Animated setup + master_type forced + fog_flag forced to 3
// (asm 0x48A4E4: byte store @0x75 = 117).
// Caller coverage: RGE_Player__load_master_object calls ctor4 directly.
RGE_Master_Doppleganger_Object* __fastcall RGE_Master_Doppleganger_Object__RGE_Master_Doppleganger_Object4_new(
    RGE_Master_Doppleganger_Object* this_, DWORD dummy,
    int infile, RGE_Master_Static_Object** list, RGE_Sprite** sprites, RGE_Sound** sounds, int do_setup)
{
    RGE_Master_Doppleganger_Object* result = this_;
    __try
    {
        RGE_Master_Animated_Object__RGE_Master_Animated_Object4((RGE_Master_Animated_Object*)this_, infile, list, sprites, sounds, 0);
        this_->vfptr = rge_master_doppleganger_object_vftable;
        if (do_setup)
        {
            RGE_Master_Animated_Object__setup4((RGE_Master_Animated_Object*)this_, infile, list, sprites, sounds);
            this_->master_type = RGE_MASTER_TYPE_DOPPELGANGER;
            this_->fog_flag = 3;
        }
        result = this_;
    }
    __except (RGE_Master_Animated_Object__destructor((RGE_Master_Animated_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Doppleganger_Object::destructor
// Original: RGE_Master_Doppleganger_Object__destructor @ 0x0048A500.
// ABI: __thiscall — this_ in ECX, no stack args, void return, no dummy.
// Doppleganger vftable store, then tail-jmp to the Animated destructor
// (Doppleganger masters add no resources beyond the Animated fields);
// the replacement calls it per the HOOK-ROUTING RULING.
void __fastcall RGE_Master_Doppleganger_Object__destructor_new(RGE_Master_Doppleganger_Object* this_)
{
    this_->vfptr = rge_master_doppleganger_object_vftable;
    RGE_Master_Animated_Object__destructor((RGE_Master_Animated_Object*)this_);
}

// Replacement for RGE_Master_Doppleganger_Object::vector_deleting_destructor
// Original: RGE_Master_Doppleganger_Object__vector_deleting_destructor @ 0x0048A350.
// ABI: __thiscall — this_ in ECX, 1 stack arg (retn 4), void* result (this).
// Destructor + operator_delete_internal on flags & 1, returning this.
void* __fastcall RGE_Master_Doppleganger_Object__vector_deleting_destructor_new(
    RGE_Master_Doppleganger_Object* this_, DWORD dummy, unsigned int flags)
{
    RGE_Master_Doppleganger_Object__destructor_new(this_);
    if ((flags & 1) != 0)
        operator_delete_internal(this_);
    return this_;
}

// Replacement for RGE_Master_Doppleganger_Object::make_new_obj
// Original: RGE_Master_Doppleganger_Object__make_new_obj @ 0x0048A510
// (idb_functions.h:1851). ABI: __thiscall — this_ in ECX, 5 stack args
// (retn 14h — owner, x, y, z, obj_to_dopple), RGE_Static_Object* result.
// The factory ladder with ONE DELTA: the guarded recycle dispatch goes
// through slot @0x370 — NOT the standard @0x74 recycle_in_to_game —
// because the recycled Doppleganger runtime object's 7-argument
// recycle_in_to_game_2 takes the extra obj_to_dopple parameter (asm
// 0x48A567: call dword ptr [edx+370h] with pushes obj_to_dopple/z/y/x/
// owner/master; the pseudocode's "vfptr[1].play_unload_sound" rendering
// is an IDA offset artifact). SLOT GROUNDED: recycle_in_to_game_2 =
// member 220 of RGE_Doppleganger_ObjectVtbl (idb_types.h:21758; line
// 21758 - struct-open 21538 = 220 entries -> 220*4 = 880 = 0x370), and
// its parameter list (Doppleganger*, Master_Static*, Player*, float,
// float, float, RGE_Static_Object*) matches the push sequence exactly.
// BASE->DERIVED CAST EVIDENCE for the recycled object: the pool comes
// from recycle_object_in_to_game(world, master_type = 25) whose entries
// are RGE_Doppleganger_Object instances (runtime ctor stores the
// Doppleganger vftable), and the @0x370 dispatch itself only resolves
// through that vtbl.
// ALLOCATION VERIFIED: sizeof(RGE_Doppleganger_Object) = 220 = 0xDC —
// sequential walk over idb_types.h:21470-21530 (ei @144 = 36; tail
// doppledObjectID @216 -> 220).
// SEH fold per the make_new_obj precedent (FUNCTION CHUNK 0x6421F0,
// same operator_delete shape).
RGE_Static_Object* __fastcall RGE_Master_Doppleganger_Object__make_new_obj_new(
    RGE_Master_Doppleganger_Object* this_, DWORD dummy,
    RGE_Player* owner, float x, float y, float z,
    RGE_Static_Object* obj_to_dopple)
{
    RGE_Static_Object* result = 0;
    RGE_Doppleganger_Object* new_obj = 0;
    __try
    {
        if (this_->recyclable)
        {
            RGE_Static_Object* recycled =
                RGE_Game_World__recycle_object_in_to_game(owner->world, this_->master_type);
            if (recycled)
            {
                ((RGE_Doppleganger_Object*)recycled)->vfptr->recycle_in_to_game_2(
                    (RGE_Doppleganger_Object*)recycled,
                    (RGE_Master_Static_Object*)this_, owner, x, y, z, obj_to_dopple);
                result = recycled;
            }
        }
        if (!result)
        {
            new_obj = (RGE_Doppleganger_Object*)operator_new_internal(sizeof(RGE_Doppleganger_Object));
            if (new_obj)
                result = (RGE_Static_Object*)RGE_Doppleganger_Object__RGE_Doppleganger_Object(
                    new_obj, this_, owner, x, y, z, 1, obj_to_dopple);
        }
    }
    __except (operator_delete_internal(new_obj), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Doppleganger_Object::make_new_master
// Original: RGE_Master_Doppleganger_Object__make_new_master @ 0x0048A5F0
// (idb_functions.h:1852). ABI: __thiscall — this_ in ECX, no stack args,
// RGE_Master_Static_Object* result.
// Master-template factory: operator_new(0xD4) — sizeof(RGE_Master_
// Doppleganger_Object) = 212 = 0xD4 (the Animated master size; no extra
// fields) — null-checked, then the base Animated COPY-CTOR with
// do_setup = 0 (asm 0x48A62B), the Doppleganger vfptr swap (asm
// 0x48A638), the base Animated SETUP (asm 0x48A63E; hooked -> routes
// into our replacement), master_type = 25 (asm 0x48A643) and
// fog_flag = 3 (asm 0x48A647, byte @0x75 = 117).
// SEH fold per the make_new_master precedent (FUNCTION CHUNK 0x642210;
// second entry 0x64221B jmps RGE_Master_Animated_Object__destructor).
RGE_Master_Static_Object* __fastcall RGE_Master_Doppleganger_Object__make_new_master_new(
    RGE_Master_Doppleganger_Object* this_)
{
    RGE_Master_Doppleganger_Object* new_master =
        (RGE_Master_Doppleganger_Object*)operator_new_internal(sizeof(RGE_Master_Doppleganger_Object));
    RGE_Master_Static_Object* result = 0;
    __try
    {
        if (new_master)
        {
            RGE_Master_Animated_Object__RGE_Master_Animated_Object(
                (RGE_Master_Animated_Object*)new_master, (RGE_Master_Animated_Object*)this_, 0);
            new_master->vfptr = rge_master_doppleganger_object_vftable;
            RGE_Master_Animated_Object__setup(
                (RGE_Master_Animated_Object*)new_master, (RGE_Master_Animated_Object*)this_);
            new_master->master_type = RGE_MASTER_TYPE_DOPPELGANGER;
            new_master->fog_flag = 3;
            result = (RGE_Master_Static_Object*)new_master;
        }
    }
    __except (operator_delete_internal(new_master), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Doppleganger_Object::copy_obj
// Original: RGE_Master_Doppleganger_Object__copy_obj @ 0x0048A660
// (idb_functions.h:1853). ABI: __thiscall — this_ in ECX, 1 stack arg
// (retn 4), void return. No SEH frame (plain function).
// Pure forwarder: the original body is exactly call Animated copy_obj /
// retn 4 (asm 0x48A665-0x48A66A — a call, not a tail-jmp; the
// replacement's call-then-return form is identical). Doppleganger
// masters add no fields, so there is nothing beyond the base copy.
void __fastcall RGE_Master_Doppleganger_Object__copy_obj_new(
    RGE_Master_Doppleganger_Object* this_, DWORD dummy,
    RGE_Master_Static_Object* source)
{
    RGE_Master_Animated_Object__copy_obj((RGE_Master_Animated_Object*)this_, source);
}

// Replacement for RGE_Master_Doppleganger_Object::small_save
// Original: @ 0x0048A670 (idb_functions.h:1854). Pure forwarder: call
// Animated small_save / retn 4 (asm 0x48A675; a call, not a tail-jmp).
void __fastcall RGE_Master_Doppleganger_Object__small_save_new(
    RGE_Master_Doppleganger_Object* this_, DWORD dummy, int outfile)
{
    RGE_Master_Animated_Object__small_save((RGE_Master_Animated_Object*)this_, outfile);
}

// Replacement for RGE_Master_Doppleganger_Object::save
// Original: @ 0x0048A680 (idb_functions.h:1855). Pure forwarder (asm
// 0x48A685).
void __fastcall RGE_Master_Doppleganger_Object__save_new(
    RGE_Master_Doppleganger_Object* this_, DWORD dummy, int outfile)
{
    RGE_Master_Animated_Object__save((RGE_Master_Animated_Object*)this_, outfile);
}

void install_RGE_Master_Doppleganger_Object()
{
    setHook((void*)0x0048A370, RGE_Master_Doppleganger_Object__RGE_Master_Doppleganger_Object2_new);
    setHook((void*)0x0048A3F0, RGE_Master_Doppleganger_Object__RGE_Master_Doppleganger_Object3_new);
    setHook((void*)0x0048A480, RGE_Master_Doppleganger_Object__RGE_Master_Doppleganger_Object4_new);
    setHook((void*)0x0048A500, RGE_Master_Doppleganger_Object__destructor_new);
    setHook((void*)0x0048A350, RGE_Master_Doppleganger_Object__vector_deleting_destructor_new);
    setHook((void*)0x0048A510, RGE_Master_Doppleganger_Object__make_new_obj_new);
    setHook((void*)0x0048A5F0, RGE_Master_Doppleganger_Object__make_new_master_new);
    setHook((void*)0x0048A660, RGE_Master_Doppleganger_Object__copy_obj_new);
    setHook((void*)0x0048A670, RGE_Master_Doppleganger_Object__small_save_new);
    setHook((void*)0x0048A680, RGE_Master_Doppleganger_Object__save_new);
}
