#include "stdafx.h"
#include "RGE_Master_Missile_Object.h"

// .rdata vftable of RGE_Master_Missile_Object
// (battlegrounds_x2.lst: .rdata:0065B1FC RGE_Master_Missile_Object__vftable)
static RGE_Master_Missile_ObjectVtbl* const rge_master_missile_object_vftable =
    (RGE_Master_Missile_ObjectVtbl*)0x0065B1FC;

// Replacement for RGE_Master_Missile_Object::RGE_Master_Missile_Object2
// Original: RGE_Master_Missile_Object__RGE_Master_Missile_Object2 @ 0x0048A6B0.
// ABI: __thiscall — this_ in ECX, 4 stack args (retn 10h), returns this.
// Derived override: base Combat setup2 call (through the idb constant —
// routes into the replacement per the HOOK-ROUTING RULING), master_type
// forced to RGE_MASTER_TYPE_MISSILE (0x3C), then the six Missile
// extension reads: five u8 fields @380-384 (missile_type, targetting_
// type, missile_hit_info, missile_die_info, area_effect_specials) and
// ballistics_ratio @388 (float) — all sizeof-form. Extension offsets
// verified by the mechanical walker (struct total 0x188). SEH
// ctor-unwind frame folded per the make_new_obj pattern: on exception
// the COMBAT destructor runs on this and the exception propagates
// (asm unwind chunk 0x642230: jmp RGE_Master_Combat_Object__destructor).
RGE_Master_Missile_Object* __fastcall RGE_Master_Missile_Object__RGE_Master_Missile_Object2_new(
    RGE_Master_Missile_Object* this_, DWORD dummy,
    int infile, RGE_Sprite** sprites, RGE_Sound** sounds, int do_setup)
{
    RGE_Master_Missile_Object* result = this_;
    __try
    {
        RGE_Master_Combat_Object__RGE_Master_Combat_Object2((RGE_Master_Combat_Object*)this_, infile, sprites, sounds, 0);
        this_->vfptr = rge_master_missile_object_vftable;
        if (do_setup)
        {
            RGE_Master_Combat_Object__setup2((RGE_Master_Combat_Object*)this_, infile, sprites, sounds);
            this_->master_type = RGE_MASTER_TYPE_MISSILE;
            rge_read(infile, &this_->missile_type, sizeof(this_->missile_type));
            rge_read(infile, &this_->targetting_type, sizeof(this_->targetting_type));
            rge_read(infile, &this_->missile_hit_info, sizeof(this_->missile_hit_info));
            rge_read(infile, &this_->missile_die_info, sizeof(this_->missile_die_info));
            rge_read(infile, &this_->area_effect_specials, sizeof(this_->area_effect_specials));
            rge_read(infile, &this_->ballistics_ratio, sizeof(this_->ballistics_ratio));
        }
        result = this_;
    }
    __except (RGE_Master_Combat_Object__destructor((RGE_Master_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Missile_Object::RGE_Master_Missile_Object3
// Original: RGE_Master_Missile_Object__RGE_Master_Missile_Object3 @ 0x0048A780.
// ABI: __thiscall — this_ in ECX, 5 stack args (retn 14h), returns this.
// Base call, master_type forced, recyclable forced to 1 (asm 0x48A806:
// byte store @0xBC = 188), then ONE fscanf "%hd %hd %hd %hd %hd %f"
// (asm 0x48A7E0-0x48A811): the %hd scratch targets are the do_setup/
// sprites/in_id/sounds/infile PARAM SLOTS (original) — named locals
// here — byte-narrowed into the five u8 fields (asm byte stores
// 0x48A816-0x48A845); ballistics_ratio read directly (%f).
RGE_Master_Missile_Object* __fastcall RGE_Master_Missile_Object__RGE_Master_Missile_Object3_new(
    RGE_Master_Missile_Object* this_, DWORD dummy,
    FILE* infile, RGE_Sprite** sprites, RGE_Sound** sounds, __int16 in_id, int do_setup)
{
    RGE_Master_Missile_Object* result = this_;
    __try
    {
        RGE_Master_Combat_Object__RGE_Master_Combat_Object3((RGE_Master_Combat_Object*)this_, infile, sprites, sounds, in_id, 0);
        this_->vfptr = rge_master_missile_object_vftable;
        if (do_setup)
        {
            RGE_Master_Combat_Object__setup3((RGE_Master_Combat_Object*)this_, infile, sprites, sounds, in_id);
            this_->recyclable = 1;
            this_->master_type = RGE_MASTER_TYPE_MISSILE;
            __int16 missile_type_s; __int16 targetting_s; __int16 hit_s;
            __int16 die_s; __int16 specials_s;
            fscanf_internal(infile, "%hd %hd %hd %hd %hd %f",
                &missile_type_s, &targetting_s, &hit_s, &die_s, &specials_s,
                &this_->ballistics_ratio);
            this_->missile_type = (unsigned __int8)missile_type_s;
            this_->targetting_type = (unsigned __int8)targetting_s;
            this_->missile_hit_info = (unsigned __int8)hit_s;
            this_->missile_die_info = (unsigned __int8)die_s;
            this_->area_effect_specials = (unsigned __int8)specials_s;
        }
        result = this_;
    }
    __except (RGE_Master_Combat_Object__destructor((RGE_Master_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Missile_Object::RGE_Master_Missile_Object4
// Original: RGE_Master_Missile_Object__RGE_Master_Missile_Object4 @ 0x0048A870.
// ABI: __thiscall — this_ in ECX, 5 stack args (retn 14h), returns this.
// Base call, then list[copy_id] (copy_id was stream-read by the base
// setup4; UNCONDITIONAL sign-extended index, no negative guard —
// faithful). BASE-POINTER CORRECTION per the template rule: the IDA
// v7[1].field accesses are the referenced master's Missile extension
// fields (walker-confirmed): the four bytes of v7[1].object_flags
// (@552-555) -> missile_type/targetting_type/missile_hit_info/
// missile_die_info; v7[1].help_string_id low byte (@556) ->
// area_effect_specials; v7[1].help_page_id (@560) -> ballistics_ratio.
// The list element is re-typed RGE_Master_Missile_Object* accordingly.
// Six extension fields copy when the reference is non-null; then the
// tail rge_read OVERWRITES targetting_type (later write wins;
// asm 0x48A936+ region per the pseudocode's final read).
RGE_Master_Missile_Object* __fastcall RGE_Master_Missile_Object__RGE_Master_Missile_Object4_new(
    RGE_Master_Missile_Object* this_, DWORD dummy,
    int infile, RGE_Master_Static_Object** list, RGE_Sprite** sprites, RGE_Sound** sounds, int do_setup)
{
    RGE_Master_Missile_Object* result = this_;
    __try
    {
        RGE_Master_Combat_Object__RGE_Master_Combat_Object4((RGE_Master_Combat_Object*)this_, infile, list, sprites, sounds, 0);
        this_->vfptr = rge_master_missile_object_vftable;
        if (do_setup)
        {
            RGE_Master_Combat_Object__setup4((RGE_Master_Combat_Object*)this_, infile, list, sprites, sounds);
            RGE_Master_Missile_Object* other = (RGE_Master_Missile_Object*)list[this_->copy_id];
            this_->master_type = RGE_MASTER_TYPE_MISSILE;
            if (other)
            {
                this_->missile_type = other->missile_type;
                this_->targetting_type = other->targetting_type;
                this_->missile_hit_info = other->missile_hit_info;
                this_->missile_die_info = other->missile_die_info;
                this_->area_effect_specials = other->area_effect_specials;
                this_->ballistics_ratio = other->ballistics_ratio;
            }
            rge_read(infile, &this_->targetting_type, sizeof(this_->targetting_type));
        }
        result = this_;
    }
    __except (RGE_Master_Combat_Object__destructor((RGE_Master_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Missile_Object::destructor
// Original: RGE_Master_Missile_Object__destructor @ 0x0048A950.
// ABI: __thiscall — this_ in ECX, no stack args, void return, no dummy.
// Missile vftable store, then tail-jmp to the Combat destructor (which
// frees the Armor/Weapon tables and chains to Action); the replacement
// calls it per the HOOK-ROUTING RULING.
void __fastcall RGE_Master_Missile_Object__destructor_new(RGE_Master_Missile_Object* this_)
{
    this_->vfptr = rge_master_missile_object_vftable;
    RGE_Master_Combat_Object__destructor((RGE_Master_Combat_Object*)this_);
}

// Replacement for RGE_Master_Missile_Object::vector_deleting_destructor
// Original: RGE_Master_Missile_Object__vector_deleting_destructor @ 0x0048A690.
// ABI: __thiscall — this_ in ECX, 1 stack arg (retn 4), void* result (this).
// Destructor + operator_delete_internal on flags & 1, returning this.
void* __fastcall RGE_Master_Missile_Object__vector_deleting_destructor_new(
    RGE_Master_Missile_Object* this_, DWORD dummy, unsigned int flags)
{
    RGE_Master_Missile_Object__destructor_new(this_);
    if ((flags & 1) != 0)
        operator_delete_internal(this_);
    return this_;
}

void install_RGE_Master_Missile_Object()
{
    setHook((void*)0x0048A6B0, RGE_Master_Missile_Object__RGE_Master_Missile_Object2_new);
    setHook((void*)0x0048A780, RGE_Master_Missile_Object__RGE_Master_Missile_Object3_new);
    setHook((void*)0x0048A870, RGE_Master_Missile_Object__RGE_Master_Missile_Object4_new);
    setHook((void*)0x0048A950, RGE_Master_Missile_Object__destructor_new);
    setHook((void*)0x0048A690, RGE_Master_Missile_Object__vector_deleting_destructor_new);
}
