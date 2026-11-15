#include "stdafx.h"

// .rdata vftable of RGE_Missile_Object (lst :990078)
static RGE_Missile_ObjectVtbl* const rge_missile_object_vftable =
    (RGE_Missile_ObjectVtbl*)0x0065B808;

// Replacement for RGE_Missile_Object::RGE_Missile_Object (runtime ctor).
// Original: @ 0x0049BA40 (idb_functions.h:2059). Range [0x49BA40,
// 0x49BAE6) (lst :265707/:265785, retn 18h = 6 stack args), returns
// this. ABI: __thiscall - this_ in ECX. Caller: RGE_Master_Missile_
// Object__make_new_obj +9D (owned, hook-routed; lst :265708).
// Decode (asm 0x49BA40-0x49BAE6): base runtime Combat ctor with
// do_setup = 0 (hooked -> routes), the Missile vfptr store, then IF
// do_setup: ghost_mode @+0x180 (byte) = 1 (asm 0x49BAA7), fired_from_id
// @+0x184 = -1 (asm 0x49BAAE), the runtime Combat setup (:1074,
// unhooked), own_master @+0x188 (byte) = 0 (asm 0x49BABF), type @+0x5E
// = 60 (0x3C - stored TWICE inside the branch, asm 0x49BAC6 + 0x49BAC9
// - the original's inner and outer source lines; the else path stores
// it once, asm 0x49BACE). max_range @+0x17C is NOT stored by the ctor
// (setup_read streams it). SEH fold: chunk 0x642550 -> RGE_Combat_
// Object__destructor (0x443B10 - HOOKED -> routes; the IMMEDIATE base
// level - runtime ladder evidence).
RGE_Missile_Object* __fastcall RGE_Missile_Object__RGE_Missile_Object_new(
    RGE_Missile_Object* this_, DWORD dummy,
    RGE_Master_Combat_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z, int do_setup)
{
    RGE_Missile_Object* result = this_;
    __try
    {
        RGE_Combat_Object__RGE_Combat_Object(
            (RGE_Combat_Object*)this_, tobj, obj_owner, x, y, z, 0);
        this_->vfptr = rge_missile_object_vftable;       // asm 0x49BA90
        if (do_setup != 0)
        {
            this_->ghost_mode = 1;                       // asm 0x49BAA7 (byte @+0x180)
            this_->fired_from_id = -1;                   // asm 0x49BAAE (@+0x184)
            RGE_Combat_Object__setup((RGE_Combat_Object*)this_, tobj, obj_owner, x, y, z);   // asm 0x49BAB8 (:1074)
            this_->own_master = 0;                       // asm 0x49BABF (byte @+0x188)
            this_->type = 60;                            // asm 0x49BAC6 (inner store)
        }
        this_->type = 60;                                // asm 0x49BAC9/0x49BACE (outer store, both paths)
        result = this_;
    }
    __except (RGE_Combat_Object__destructor((RGE_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x642550
    {
    }
    return result;
}

// Replacement for RGE_Missile_Object::scalar_deleting_destructor.
// Original: @ 0x0049BAF0 (idb_functions.h:2060). Range [0x49BAF0,
// 0x49BB0B) (lst :265794/:265812, retn 4), void* result. ABI:
// __thiscall. Virtual: vftable slot 0 (lst :265795). Body: the Missile
// destructor (IN BATCH - the direct call routes into
// RGE_Missile_Object__destructor_new per the hook-routing ruling),
// then flags & 1 -> operator_delete, return this. No SEH.
void* __fastcall RGE_Missile_Object__scalar_deleting_destructor_new(
    RGE_Missile_Object* this_, DWORD dummy, unsigned int flags)
{
    RGE_Missile_Object__destructor(this_);
    if (flags & 1)
        operator_delete_internal(this_);
    return this_;
}

// Replacement for RGE_Missile_Object::RGE_Missile_Object_read (runtime
// ctor_read). Original: @ 0x0049BB10 (idb_functions.h:2061). Range
// [0x49BB10, 0x49BB76) (lst :265821/:265873, retn 0Ch = 3 stack args),
// returns this. ABI: __thiscall - this_ in ECX. Callers: RGE_Object_
// List__load +17C and RGE_Player_Object_List__Load +17C (lst :265822/
// :265823).
// Decode (asm 0x49BB10-0x49BB76): base runtime Combat ctor_read with
// do_setup = 0 (hooked -> routes), the Missile vfptr store, IF do_setup
// the derived Missile setup_read (unhooked - it owns the max_range/
// fired_from_id/own_master stream block incl. the own-master
// construction), then type @+0x5E = 60 UNCONDITIONALLY after the guard
// (asm 0x49BB63). SEH fold: chunk 0x642570 -> RGE_Combat_Object__
// destructor (HOOKED, routes).
RGE_Missile_Object* __fastcall RGE_Missile_Object__RGE_Missile_Object_read_new(
    RGE_Missile_Object* this_, DWORD dummy, int infile, RGE_Game_World* gworld, int do_setup)
{
    RGE_Missile_Object* result = this_;
    __try
    {
        RGE_Combat_Object__RGE_Combat_Object_read((RGE_Combat_Object*)this_, infile, gworld, 0);
        this_->vfptr = rge_missile_object_vftable;       // asm 0x49BB4E
        if (do_setup != 0)
            RGE_Missile_Object__setup_read(this_, infile, gworld);   // asm 0x49BB5A
        this_->type = 60;                                // asm 0x49BB63 - unconditional
        result = this_;
    }
    __except (RGE_Combat_Object__destructor((RGE_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x642570
    {
    }
    return result;
}

// Replacement for RGE_Missile_Object::destructor. Original:
// @ 0x0049BB80 (idb_functions.h:2062). Range [0x49BB80, 0x49BBF7)
// (lst :265882/:265933), void, no stack args. ABI: __thiscall. Caller:
// the scalar deleting destructor above (lst :265883).
// Decode (asm 0x49BB80-0x49BBF7): vfptr store, then the OWN-MASTER
// teardown (same shape as TRIBE_Combat's): if own_master @+0x188 != 0:
// own = master_obj @+0x14; own_master = 0 (asm 0x49BBC6); master_obj =
// owner->master_objects[own->id] (asm 0x49BBBC-0x49BBCC); if (own != 0)
// own->vfptr->__vecDelDtor(own, 1) (RGE_Master_Missile vtbl slot 5 =
// +0x14, asm 0x49BBD2-0x49BBD6) - the own master is DELETED. Then the
// runtime Combat destructor (HOOKED -> routes). SEH fold: chunk
// 0x642590 -> RGE_Combat_Object__destructor (HOOKED, routes).
void __fastcall RGE_Missile_Object__destructor_new(RGE_Missile_Object* this_, DWORD dummy)
{
    __try
    {
        this_->vfptr = rge_missile_object_vftable;       // asm 0x49BB9D
        if (this_->own_master != 0)                      // asm 0x49BBA3-0x49BBB1 (byte @+0x188)
        {
            RGE_Master_Missile_Object* own = this_->master_obj;   // asm 0x49BBB3
            this_->own_master = 0;                       // asm 0x49BBC6
            this_->master_obj =
                (RGE_Master_Missile_Object*)this_->owner->master_objects[own->id];   // asm 0x49BBBC-0x49BBCC
            if (own != 0)                                // asm 0x49BBBA/0x49BBD0 (saved flags)
                own->vfptr->__vecDelDtor(own, 1);        // asm 0x49BBD2-0x49BBD6 (slot 5 = +0x14)
        }
        RGE_Combat_Object__destructor((RGE_Combat_Object*)this_);   // asm 0x49BBE3 - HOOKED, routes
    }
    __except (RGE_Combat_Object__destructor((RGE_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x642590
    {
    }
}

// Replacement for RGE_Missile_Object::recycle_out_of_game. Original:
// @ 0x0049BC00 (idb_functions.h:2063). Range [0x49BC00, 0x49BC3A)
// (lst :265942/:265968), void, no stack args. ABI: __thiscall. Virtual:
// vftable slot (lst :265943).
// Decode (asm 0x49BC00-0x49BC3A): the OWN-MASTER teardown with NO
// vfptr store (same reads: own_master @+0x188, master_obj @+0x14,
// owner @+0x18 -> master_objects @+0x74, own->id word @+0x18; the
// saved-flags jz 0x49BC29 skips only the delete), then the runtime
// Action recycle (:122 - HOOKED -> routes; skips the Combat level,
// which has no own recycle - consistent with the vftable slotting
// recorded at the Combat round). No SEH.
void __fastcall RGE_Missile_Object__recycle_out_of_game_new(RGE_Missile_Object* this_, DWORD dummy)
{
    if (this_->own_master != 0)                          // asm 0x49BC03-0x49BC0B (byte @+0x188)
    {
        RGE_Master_Missile_Object* own = this_->master_obj;   // asm 0x49BC0D
        this_->own_master = 0;                           // asm 0x49BC1F
        this_->master_obj =
            (RGE_Master_Missile_Object*)this_->owner->master_objects[own->id];   // asm 0x49BC10-0x49BC26
        if (own != 0)                                    // asm 0x49BC13/0x49BC29 (saved flags)
            own->vfptr->__vecDelDtor(own, 1);            // asm 0x49BC2B-0x49BC2F (slot 5 = +0x14)
    }
    RGE_Action_Object__recycle_out_of_game((RGE_Action_Object*)this_);   // asm 0x49BC34 (:122) - HOOKED, routes
}

void install_RGE_Missile_Object()
{
    setHook((void*)0x0049BA40, RGE_Missile_Object__RGE_Missile_Object_new);
    setHook((void*)0x0049BAF0, RGE_Missile_Object__scalar_deleting_destructor_new);
    setHook((void*)0x0049BB10, RGE_Missile_Object__RGE_Missile_Object_read_new);
    setHook((void*)0x0049BB80, RGE_Missile_Object__destructor_new);
    setHook((void*)0x0049BC00, RGE_Missile_Object__recycle_out_of_game_new);
}
