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

// Replacement for RGE_Master_Missile_Object::make_new_obj
// Original: RGE_Master_Missile_Object__make_new_obj @ 0x0048A960
// (idb_functions.h:1861). ABI: __thiscall — this_ in ECX, 4 stack args
// (retn 10h), RGE_Static_Object* result.
// Same factory ladder: recyclable @188 (asm 0x48A97E), recycle via
// owner->world @0x8C (asm 0x48A98B), guarded dispatch slot @0x74 (asm
// 0x48A9B2), else operator_new(0x18C) + the RGE_Missile_Object runtime
// ctor (do_setup = 1; asm 0x48A9FD; the tobj argument is cast to the
// constant's RGE_Master_Combat_Object* parameter — ancestor cast,
// layout-safe prefix).
// ALLOCATION VERIFIED: sizeof(RGE_Missile_Object) = 396 = 0x18C —
// sequential walk over idb_types.h:20935-21027 (the RGE_Combat_Object
// 380 prefix + max_range @380, ghost_mode @384, fired_from_id @388,
// own_master @392 -> 396 rounded to the pack-capped align 4).
// SEH fold per the make_new_obj precedent (FUNCTION CHUNK 0x642290,
// same operator_delete shape).
RGE_Static_Object* __fastcall RGE_Master_Missile_Object__make_new_obj_new(
    RGE_Master_Missile_Object* this_, DWORD dummy,
    RGE_Player* owner, float x, float y, float z)
{
    RGE_Static_Object* result = 0;
    RGE_Missile_Object* new_obj = 0;
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
            new_obj = (RGE_Missile_Object*)operator_new_internal(sizeof(RGE_Missile_Object));
            if (new_obj)
                result = (RGE_Static_Object*)RGE_Missile_Object__RGE_Missile_Object(
                    new_obj, (RGE_Master_Combat_Object*)this_, owner, x, y, z, 1);
        }
    }
    __except (operator_delete_internal(new_obj), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Missile_Object::make_new_master
// Original: RGE_Master_Missile_Object__make_new_master @ 0x0048AA30
// (idb_functions.h:1862). ABI: __thiscall — this_ in ECX, no stack args,
// RGE_Master_Static_Object* result.
// Master-template factory: operator_new(0x188) — sizeof(RGE_Master_
// Missile_Object) = 392 = 0x188 walker-verified — null-checked, then the
// base Combat COPY-CTOR with do_setup = 0 (asm 0x48AA6B), the Missile
// vfptr store (asm 0x48AA78), the DERIVED setup (Combat::setup — asm
// 0x48AA7E; hooked -> routes into our replacement; sets master_type = 50
// and the whole Combat extension from this), then master_type = 60 (asm
// 0x48AA83) and the SIX Missile extension copies in the asm's order:
// missile_type @380, targetting_type @381, missile_hit_info @382,
// missile_die_info @383, area_effect_specials @384 (five byte moves,
// 0x48AA87-0x48AABD), ballistics_ratio @388 dword (0x48AAC3).
// SEH fold per the make_new_master precedent (FUNCTION CHUNK 0x6422B0;
// second entry 0x6422BB jmps RGE_Master_Combat_Object__destructor).
RGE_Master_Static_Object* __fastcall RGE_Master_Missile_Object__make_new_master_new(
    RGE_Master_Missile_Object* this_)
{
    RGE_Master_Missile_Object* new_master =
        (RGE_Master_Missile_Object*)operator_new_internal(sizeof(RGE_Master_Missile_Object));
    RGE_Master_Static_Object* result = 0;
    __try
    {
        if (new_master)
        {
            RGE_Master_Combat_Object__RGE_Master_Combat_Object(
                (RGE_Master_Combat_Object*)new_master, (RGE_Master_Combat_Object*)this_, 0);
            new_master->vfptr = rge_master_missile_object_vftable;
            RGE_Master_Combat_Object__setup(
                (RGE_Master_Combat_Object*)new_master, (RGE_Master_Combat_Object*)this_);
            new_master->master_type = RGE_MASTER_TYPE_MISSILE;
            new_master->missile_type = this_->missile_type;
            new_master->targetting_type = this_->targetting_type;
            new_master->missile_hit_info = this_->missile_hit_info;
            new_master->missile_die_info = this_->missile_die_info;
            new_master->area_effect_specials = this_->area_effect_specials;
            new_master->ballistics_ratio = this_->ballistics_ratio;
            result = (RGE_Master_Static_Object*)new_master;
        }
    }
    __except (operator_delete_internal(new_master), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Master_Missile_Object::copy_obj
// Original: RGE_Master_Missile_Object__copy_obj @ 0x0048AAF0
// (idb_functions.h:1863). ABI: __thiscall — this_ in ECX, 1 stack arg
// (retn 4), void return. No SEH frame (plain function).
// Base Combat copy_obj call (idb constant -> our hooked Combat copy_obj
// replacement per the HOOK-ROUTING RULING), then the six Missile
// extension copies. BASE-POINTER CORRECTION per the template rule: the
// IDA renderings read source through RGE_Master_Static_Object*
// (source[1].object_flags @+344, help_string_id @+176, help_page_id
// @+180 = source + 208 + 344/176/180) — the byte offsets land on the
// Missile extension fields @380-388 (walker-confirmed), so the source is
// re-typed RGE_Master_Missile_Object*. WIDTH RESOLUTION from the asm:
// missile_type/targetting_type/missile_hit_info/missile_die_info are
// FOUR SEPARATE byte moves (0x48AAFE-0x48AB28; the pseudocode's single
// dword read of object_flags was an IDA merge), then area_effect_specials
// byte (0x48AB2E) and ballistics_ratio dword (0x48AB3A).
void __fastcall RGE_Master_Missile_Object__copy_obj_new(
    RGE_Master_Missile_Object* this_, DWORD dummy,
    RGE_Master_Static_Object* source)
{
    RGE_Master_Combat_Object__copy_obj((RGE_Master_Combat_Object*)this_, source);
    RGE_Master_Missile_Object* src = (RGE_Master_Missile_Object*)source;
    this_->missile_type = src->missile_type;
    this_->targetting_type = src->targetting_type;
    this_->missile_hit_info = src->missile_hit_info;
    this_->missile_die_info = src->missile_die_info;
    this_->area_effect_specials = src->area_effect_specials;
    this_->ballistics_ratio = src->ballistics_ratio;
}

// Replacement for RGE_Master_Missile_Object::modify
// Original: @ 0x0048AB50 (idb_functions.h:1864). __thiscall, retn 8,
// void. No SEH frame (plain function). flag == 19 -> targetting_type =
// (u8)ftol amount (BYTE store, asm 0x48AB5D-0x48AB66; the pseudocode's
// (__int64) rendering was the ftol artifact); the base Combat modify
// call is UNCONDITIONAL (asm 0x48AB74 — no else).
void __fastcall RGE_Master_Missile_Object__modify_new(
    RGE_Master_Missile_Object* this_, DWORD dummy,
    float amount, unsigned __int8 flag)
{
    if (flag == 19)
        this_->targetting_type = (unsigned __int8)(int)amount;
    RGE_Master_Combat_Object__modify((RGE_Master_Combat_Object*)this_, amount, flag);
}

// Replacement for RGE_Master_Missile_Object::modify_delta
// Original: @ 0x0048AB80 (idb_functions.h:1865). flag == 19 -> byte store
// (asm 0x48AB8D-0x48AB96); base Combat modify_delta UNCONDITIONAL
// (asm 0x48ABA4).
void __fastcall RGE_Master_Missile_Object__modify_delta_new(
    RGE_Master_Missile_Object* this_, DWORD dummy,
    float amount, unsigned __int8 flag)
{
    if (flag == 19)
        this_->targetting_type = (unsigned __int8)(int)amount;
    RGE_Master_Combat_Object__modify_delta((RGE_Master_Combat_Object*)this_, amount, flag);
}

// Replacement for RGE_Master_Missile_Object::modify_percent
// Original: @ 0x0048ABB0 (idb_functions.h:1866). flag == 19 -> byte store
// (asm 0x48ABBD-0x48ABC6); base Combat modify_percent UNCONDITIONAL
// (asm 0x48ABD4).
void __fastcall RGE_Master_Missile_Object__modify_percent_new(
    RGE_Master_Missile_Object* this_, DWORD dummy,
    float amount, unsigned __int8 flag)
{
    if (flag == 19)
        this_->targetting_type = (unsigned __int8)(int)amount;
    RGE_Master_Combat_Object__modify_percent((RGE_Master_Combat_Object*)this_, amount, flag);
}

// Replacement for RGE_Master_Missile_Object::small_save
// Original: @ 0x0048ABE0 (idb_functions.h:1867). Base Combat small_save
// (hooked -> routes), then targetting_type (asm 0x48ABF0, w1).
void __fastcall RGE_Master_Missile_Object__small_save_new(
    RGE_Master_Missile_Object* this_, DWORD dummy, int outfile)
{
    RGE_Master_Combat_Object__small_save((RGE_Master_Combat_Object*)this_, outfile);
    rge_write(outfile, &this_->targetting_type, sizeof(this_->targetting_type));
}

// Replacement for RGE_Master_Missile_Object::save
// Original: @ 0x0048AC10 (idb_functions.h:1868). Base Combat save
// (hooked -> routes), then six extension writes: missile_type w1
// (0x48AC28), targetting_type w1 (0x48AC37), missile_hit_info w1
// (0x48AC46), missile_die_info w1 (0x48AC55), area_effect_specials w1
// (0x48AC64), ballistics_ratio w4 (0x48AC73).
void __fastcall RGE_Master_Missile_Object__save_new(
    RGE_Master_Missile_Object* this_, DWORD dummy, int outfile)
{
    RGE_Master_Combat_Object__save((RGE_Master_Combat_Object*)this_, outfile);
    rge_write(outfile, &this_->missile_type, sizeof(this_->missile_type));
    rge_write(outfile, &this_->targetting_type, sizeof(this_->targetting_type));
    rge_write(outfile, &this_->missile_hit_info, sizeof(this_->missile_hit_info));
    rge_write(outfile, &this_->missile_die_info, sizeof(this_->missile_die_info));
    rge_write(outfile, &this_->area_effect_specials, sizeof(this_->area_effect_specials));
    rge_write(outfile, &this_->ballistics_ratio, sizeof(this_->ballistics_ratio));
}

void install_RGE_Master_Missile_Object()
{
    setHook((void*)0x0048A6B0, RGE_Master_Missile_Object__RGE_Master_Missile_Object2_new);
    setHook((void*)0x0048A780, RGE_Master_Missile_Object__RGE_Master_Missile_Object3_new);
    setHook((void*)0x0048A870, RGE_Master_Missile_Object__RGE_Master_Missile_Object4_new);
    setHook((void*)0x0048A950, RGE_Master_Missile_Object__destructor_new);
    setHook((void*)0x0048A690, RGE_Master_Missile_Object__vector_deleting_destructor_new);
    setHook((void*)0x0048A960, RGE_Master_Missile_Object__make_new_obj_new);
    setHook((void*)0x0048AA30, RGE_Master_Missile_Object__make_new_master_new);
    setHook((void*)0x0048AAF0, RGE_Master_Missile_Object__copy_obj_new);
    setHook((void*)0x0048AB50, RGE_Master_Missile_Object__modify_new);
    setHook((void*)0x0048AB80, RGE_Master_Missile_Object__modify_delta_new);
    setHook((void*)0x0048ABB0, RGE_Master_Missile_Object__modify_percent_new);
    setHook((void*)0x0048ABE0, RGE_Master_Missile_Object__small_save_new);
    setHook((void*)0x0048AC10, RGE_Master_Missile_Object__save_new);
}
