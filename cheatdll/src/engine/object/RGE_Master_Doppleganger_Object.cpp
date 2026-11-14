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

void install_RGE_Master_Doppleganger_Object()
{
    setHook((void*)0x0048A370, RGE_Master_Doppleganger_Object__RGE_Master_Doppleganger_Object2_new);
    setHook((void*)0x0048A3F0, RGE_Master_Doppleganger_Object__RGE_Master_Doppleganger_Object3_new);
    setHook((void*)0x0048A480, RGE_Master_Doppleganger_Object__RGE_Master_Doppleganger_Object4_new);
    setHook((void*)0x0048A500, RGE_Master_Doppleganger_Object__destructor_new);
    setHook((void*)0x0048A350, RGE_Master_Doppleganger_Object__vector_deleting_destructor_new);
}
