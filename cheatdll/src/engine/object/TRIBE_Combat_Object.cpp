#include "stdafx.h"

// .rdata vftable of TRIBE_Combat_Object (lst :995245; slot 0 =
// vector_deleting_destructor)
static TRIBE_Combat_ObjectVtbl* const rge_tribe_combat_object_vftable =
    (TRIBE_Combat_ObjectVtbl*)0x006635B0;

// Replacement for TRIBE_Combat_Object::TRIBE_Combat_Object (runtime
// ctor). Original: @ 0x00559E00 (idb_functions.h:3982). Range
// [0x559E00, 0x559F00) (lst :577341/:577435, retn 18h = 6 stack args),
// returns this. ABI: __thiscall - this_ in ECX. Callers: the
// TRIBE_Building_Object runtime ctors +3E (the runtime ladder,
// lst :577342/:577343) and TRIBE_Master_Combat_Object__make_new_master
// (owned, hook-routed).
// Decode (asm 0x559E00-0x559F00): base runtime Combat ctor with
// do_setup = 0 (hooked -> routes), then the TRIBE_Combat(70) extension
// init in asm order 0x559E4C-0x559EB4 (all fields census-cited
// idb_types.h :16441-16458): town_bell_target_id/type/action/id2
// @+0x194/198/19C/1A0 = -1, decay_timer @+0x188 (WORD) = 0,
// town_bell_target_x/y @+0x1A4/1A8 = -1.0f, raider_build_countdown
// @+0x18C = 0, inside_garrison_count @+0x190 (byte) = 0, town_bell_flag
// @+0x191 (byte) = 0, num_builders/num_healers/own_master @+0x1AC/1AD/
// 1AE (bytes) = 0, current_volley_fire_amount @+0x1B0 = 0, next_volley/
// using_special_attack_animation @+0x1B4/1B5 (bytes) = 0,
// locked_down_count @+0x1B8 = 0. THEN the vfptr store (asm 0x559EBA,
// after the field block) and attr_amt[6] @+0x17C cleared (three dword
// stores = the 6 i16 elements, asm 0x559EC0-0x559EC9 via ecx =
// esi+17Ch). user_released @444 and berserker_timer @448 are NOT
// stored by the original (faithful). IF do_setup: the derived
// TRIBE_Combat setup (:3986, same-class call).
// SEH fold: chunk 0x6449B0 = mov ecx, [ebp-10h] / jmp
// RGE_Combat_Object__destructor (0x443B10 - HOOKED -> routes; the
// IMMEDIATE base level - runtime ladder evidence).
TRIBE_Combat_Object* __fastcall TRIBE_Combat_Object__TRIBE_Combat_Object_new(
    TRIBE_Combat_Object* this_, DWORD dummy,
    TRIBE_Master_Combat_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z, int do_setup)
{
    TRIBE_Combat_Object* result = this_;
    __try
    {
        RGE_Combat_Object__RGE_Combat_Object(
            (RGE_Combat_Object*)this_, (RGE_Master_Combat_Object*)tobj, obj_owner, x, y, z, 0);
        this_->town_bell_target_id = -1;                    // asm 0x559E4C (@+0x194)
        this_->town_bell_target_type = -1;                  // asm 0x559E52 (@+0x198)
        this_->town_bell_action = -1;                       // asm 0x559E58 (@+0x19C)
        this_->town_bell_target_id2 = -1;                   // asm 0x559E5E (@+0x1A0)
        this_->decay_timer = 0;                             // asm 0x559E69 (WORD store, @+0x188)
        this_->town_bell_target_x = -1.0f;                  // asm 0x559E70 (@+0x1A4)
        this_->town_bell_target_y = -1.0f;                  // asm 0x559E76 (@+0x1A8)
        this_->raider_build_countdown = 0;                  // asm 0x559E7E (@+0x18C)
        this_->inside_garrison_count = 0;                   // asm 0x559E84 (byte, @+0x190)
        this_->town_bell_flag = 0;                          // asm 0x559E8A (byte, @+0x191)
        this_->num_builders = 0;                            // asm 0x559E90 (byte, @+0x1AC)
        this_->num_healers = 0;                             // asm 0x559E96 (byte, @+0x1AD)
        this_->own_master = 0;                              // asm 0x559E9C (byte, @+0x1AE)
        this_->current_volley_fire_amount = 0.0f;           // asm 0x559EA2 (@+0x1B0)
        this_->next_volley = 0;                             // asm 0x559EA8 (byte, @+0x1B4)
        this_->using_special_attack_animation = 0;          // asm 0x559EAE (byte, @+0x1B5)
        this_->locked_down_count = 0;                       // asm 0x559EB4 (@+0x1B8)
        this_->vfptr = rge_tribe_combat_object_vftable;     // asm 0x559EBA (after the field block)
        for (size_t i = 0; i < _countof(this_->attr_amt); ++i)   // asm 0x559EC0-0x559EC9 (3 dword stores)
            this_->attr_amt[i] = 0;
        if (do_setup != 0)
            TRIBE_Combat_Object__setup(this_, tobj, obj_owner, x, y, z);   // asm 0x559EE7 (:3986)
        result = this_;
    }
    __except (RGE_Combat_Object__destructor((RGE_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x6449B0
    {
    }
    return result;
}

// Replacement for TRIBE_Combat_Object::vector_deleting_destructor.
// Original: @ 0x00559F10 (idb_functions.h:3983). Range [0x559F10,
// 0x559F2B) (lst :577444/:577462, retn 4), void* result. ABI:
// __thiscall. Virtual: vftable slot 0 (lst :577445). Body: the
// TRIBE_Combat destructor (IN BATCH - the direct call routes into
// TRIBE_Combat_Object__destructor_new per the hook-routing ruling),
// then flags & 1 -> operator_delete, return this. No SEH.
void* __fastcall TRIBE_Combat_Object__vector_deleting_destructor_new(
    TRIBE_Combat_Object* this_, DWORD dummy, unsigned int flags)
{
    TRIBE_Combat_Object__destructor(this_);
    if (flags & 1)
        operator_delete_internal(this_);
    return this_;
}

// Replacement for TRIBE_Combat_Object::TRIBE_Combat_Object3 (runtime
// ctor_read; the IDA name is numbered, the signature is the ctor_read
// shape). Original: @ 0x00559F30 (idb_functions.h:3984). Range
// [0x559F30, 0x55A03B) (lst :577471/:577563, retn 0Ch = 3 stack args),
// returns this. ABI: __thiscall - this_ in ECX. Callers: the
// TRIBE_Building_Object runtime read +2D (the ladder) and
// TRIBE_Object_List__Load +AE (lst :577472/:577473).
// Decode (asm 0x559F30-0x55A03B): base runtime Combat ctor_read with
// do_setup = 0 (hooked -> routes), the SAME extension init block (asm
// 0x559F6B-0x559FD3, identical store set), vfptr, attr_amt clear, IF
// do_setup the derived TRIBE_Combat setup_read (:3987, same-class
// call), THEN the own-master speed resync: if own_master @+0x1AE != 0
// AND master_obj->object_group (word @+0x1E) == 35 -> master_obj->speed
// @+0xD0 = owner->master_objects[master_obj->id]->speed @+0xD0 (asm
// 0x559FFC-0x55A021; the pseudocode's master_objects[id][1].vfptr
// rendering = +0xD0 = the master's speed field, byte-offset verified).
// SEH fold: chunk 0x6449D0 -> RGE_Combat_Object__destructor (HOOKED,
// routes).
TRIBE_Combat_Object* __fastcall TRIBE_Combat_Object__TRIBE_Combat_Object3_new(
    TRIBE_Combat_Object* this_, DWORD dummy, int infile, RGE_Game_World* gworld, int do_setup)
{
    TRIBE_Combat_Object* result = this_;
    __try
    {
        RGE_Combat_Object__RGE_Combat_Object_read((RGE_Combat_Object*)this_, infile, gworld, 0);
        this_->town_bell_target_id = -1;                    // asm 0x559F6B (@+0x194)
        this_->town_bell_target_type = -1;                  // asm 0x559F71 (@+0x198)
        this_->town_bell_action = -1;                       // asm 0x559F77 (@+0x19C)
        this_->town_bell_target_id2 = -1;                   // asm 0x559F7D (@+0x1A0)
        this_->decay_timer = 0;                             // asm 0x559F88 (WORD store, @+0x188)
        this_->town_bell_target_x = -1.0f;                  // asm 0x559F8F (@+0x1A4)
        this_->town_bell_target_y = -1.0f;                  // asm 0x559F95 (@+0x1A8)
        this_->raider_build_countdown = 0;                  // asm 0x559F9D (@+0x18C)
        this_->inside_garrison_count = 0;                   // asm 0x559FA3 (byte, @+0x190)
        this_->town_bell_flag = 0;                          // asm 0x559FA9 (byte, @+0x191)
        this_->num_builders = 0;                            // asm 0x559FAF (byte, @+0x1AC)
        this_->num_healers = 0;                             // asm 0x559FB5 (byte, @+0x1AD)
        this_->own_master = 0;                              // asm 0x559FBB (byte, @+0x1AE)
        this_->current_volley_fire_amount = 0.0f;           // asm 0x559FC1 (@+0x1B0)
        this_->next_volley = 0;                             // asm 0x559FC7 (byte, @+0x1B4)
        this_->using_special_attack_animation = 0;          // asm 0x559FCD (byte, @+0x1B5)
        this_->locked_down_count = 0;                       // asm 0x559FD3 (@+0x1B8)
        this_->vfptr = rge_tribe_combat_object_vftable;     // asm 0x559FD9
        for (size_t i = 0; i < _countof(this_->attr_amt); ++i)   // asm 0x559FDF-0x559FE8
            this_->attr_amt[i] = 0;
        if (do_setup != 0)
            TRIBE_Combat_Object__setup_read(this_, infile, gworld);   // asm 0x559FF7 (:3987)
        if (this_->own_master != 0)                         // asm 0x559FFC (byte @+0x1AE)
        {
            TRIBE_Master_Combat_Object* master = this_->master_obj;
            if (master->object_group == 35)                 // asm 0x55A007 (word @+0x1E, 0x23)
            {
                TRIBE_Master_Combat_Object* own =
                    (TRIBE_Master_Combat_Object*)this_->owner->master_objects[master->id];   // asm 0x55A011-0x55A018
                master->speed = own->speed;                 // asm 0x55A01B-0x55A021 (+0xD0 <-> +0xD0)
            }
        }
        result = this_;
    }
    __except (RGE_Combat_Object__destructor((RGE_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x6449D0
    {
    }
    return result;
}

// Replacement for TRIBE_Combat_Object::destructor. Original:
// @ 0x0055A040 (idb_functions.h:3985). Range [0x55A040, 0x55A105)
// (lst :577572/:577651), void, no stack args. ABI: __thiscall. Callers:
// the vdel above and the TRIBE_Building_Object destructor +24C (the
// runtime ladder, hook-routed; lst :577573/:577574).
// Decode (asm 0x55A040-0x55A105): vfptr store, THEN the own-master
// teardown: if own_master @+0x1AE != 0: own = master_obj @+0x14;
// own_master = 0 (asm 0x55A086); master_obj = owner->master_objects
// [own->id] (asm 0x55A07C-0x55A08C - id read movsx word @+0x18,
// master_objects @+0x74); if (own != 0) own->vfptr->__vecDelDtor(own, 1)
// (TRIBE_Master_Combat vtbl slot 5 = +0x14, asm 0x55A092-0x55A096 - the
// OWN master object is DELETED). THEN the population-counter block:
// if vfptr->gbg_isCivilianUnit (slot 9 = +0x24, :16477, asm call
// 0x55A09D): owner->attributes[37] -= 1.0f (attributes float* @+0xAC on
// TRIBE_Player, [37] = +0x94, asm 0x55A0A4-0x55A0B9); else if
// vfptr->gbg_isMilitaryUnit (slot 8 = +0x20, :16476, asm call
// 0x55A0C5): owner->attributes[40] -= 1.0f ([40] = +0xA0, asm
// 0x55A0CC-0x55A0E1). Then the runtime Combat destructor (HOOKED ->
// routes).
// SEH fold: chunk 0x6449F0 -> RGE_Combat_Object__destructor (HOOKED,
// routes) - the throw-during-destruction unwind runs the FULL base
// destructor, immediate base level.
void __fastcall TRIBE_Combat_Object__destructor_new(TRIBE_Combat_Object* this_, DWORD dummy)
{
    __try
    {
        this_->vfptr = rge_tribe_combat_object_vftable;      // asm 0x55A05D
        if (this_->own_master != 0)                          // asm 0x55A063-0x55A071 (byte @+0x1AE)
        {
            TRIBE_Master_Combat_Object* own = this_->master_obj;   // asm 0x55A073
            this_->own_master = 0;                           // asm 0x55A086
            this_->master_obj =
                (TRIBE_Master_Combat_Object*)this_->owner->master_objects[own->id];   // asm 0x55A07C-0x55A08C
            if (own != 0)                                    // asm 0x55A07A/0x55A090 (saved flags)
                own->vfptr->__vecDelDtor(own, 1);            // asm 0x55A092-0x55A096 (slot 5 = +0x14)
        }
        if (this_->vfptr->gbg_isCivilianUnit(this_))         // asm 0x55A09D (slot 9 = +0x24)
        {
            this_->owner->attributes[37] = this_->owner->attributes[37] - 1.0f;   // asm 0x55A0A4-0x55A0B9
        }
        else if (this_->vfptr->gbg_isMilitaryUnit(this_))    // asm 0x55A0C5 (slot 8 = +0x20)
        {
            this_->owner->attributes[40] = this_->owner->attributes[40] - 1.0f;   // asm 0x55A0CC-0x55A0E1
        }
        RGE_Combat_Object__destructor((RGE_Combat_Object*)this_);   // asm 0x55A0F1 - HOOKED, routes
    }
    __except (RGE_Combat_Object__destructor((RGE_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x6449F0
    {
    }
}

void install_TRIBE_Combat_Object()
{
    setHook((void*)0x00559E00, TRIBE_Combat_Object__TRIBE_Combat_Object_new);
    setHook((void*)0x00559F10, TRIBE_Combat_Object__vector_deleting_destructor_new);
    setHook((void*)0x00559F30, TRIBE_Combat_Object__TRIBE_Combat_Object3_new);
    setHook((void*)0x0055A040, TRIBE_Combat_Object__destructor_new);
}
