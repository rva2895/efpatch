#include "stdafx.h"
#include "RGE_Master_Static_Object.h"

// ============================================================================
// RGE_Animated_Object.cpp - the converted runtime Animated tier (the ladder
// conversion, 2026-11-28; the forwarder-first route). The 12 exports owned:
// the ctor pair (the placement-new chain), the slot-0 vdel, the 9 slot
// overrides, and the appended setup virtual (the slot 219).
// THE CONVERSION NOTES:
// - the ctor/ctor_read SEH folds (chunks 0x640F70/0x640F90 -> the Static
//   destructor) are subsumed by the compiler-generated ctor-unwind of the
//   C++ chain (the throwing base ctor/setup destroys the base subobject -
//   the same destructor, the same game-heap pairing through the base
//   tripwire bodies).
// - the explicit vfptr stores (0x006566A0) are the compiler's implicit
//   vftable stores now (at the ctor entry for the base; before the derived
//   body for the Animated - the original's own order).
// - the MASTER-tier dispatches (master_obj->vfptr->...) keep the vtbl form
//   (the master tier unconverted) with the _idb boundary casts on the
//   object arguments; the sprite_list dispatches keep the vtbl form (the
//   Active_Sprite_List tier unconverted).
// - the this-side virtuals are the named virtuals (the dispatch through
//   the object's current vftable - byte-identical to the old vfptr loads
//   against our emitted table).
// ============================================================================

// ----------------------------------------------------------------------------
// RGE_Animated_Object::RGE_Animated_Object - the runtime ctor. Original:
// @ 0x00422980 (idb_functions.h:538). Range [0x422980, 0x422A3D) (retn 18h
// = 6 stack args). Decode (asm 0x422980-0x422A3C): the base Static ctor
// with do_setup = 0 ALWAYS, then the do_setup body: the RUNTIME Static
// setup + type @+0x5E = 0x14 (the tier mirror, asm 0x4229F1) + speed
// @+0xB4 = 0 (asm 0x4229F5) + set_sleep_flag(0) + the sprite-list
// bootstrap (sprite_list == 0 -> create_sprite_list (slot 170) ->
// sprite = 0 -> the master get_sprite (the MASTER vtbl slot +4; the
// result passes to new_sprite, NEVER stored - the bootstrap form) ->
// new_sprite (slot +0xB0)).
// ----------------------------------------------------------------------------
RGE_Animated_Object::RGE_Animated_Object(RGE_Master_Animated_Object* tobj, TRIBE_Player* obj_owner,
                                         float x, float y, float z, int do_setup)
    : RGE_Static_Object((RGE_Master_Static_Object*)tobj, obj_owner, x, y, z, 0)   // asm (do_setup = 0 ALWAYS)
{
    if (do_setup != 0)
    {
        RGE_Static_Object::setup((RGE_Master_Static_Object*)tobj, (RGE_Player*)obj_owner, x, y, z);   // the RUNTIME Static setup (the dual-naming cast)
        type = RGE_MASTER_TYPE_ANIMATED;                                   // asm 0x4229F1 (the tier mirror)
        speed = 0.0f;                                                      // asm 0x4229F5 (the extension @0xB4)
        RGE_Static_Object::set_sleep_flag(0);                              // idb_functions.h:3891
        if (sprite_list == 0)                                              // @+0x24 (asm 0x422A00)
        {
            sprite_list = create_sprite_list();                            // slot 170 (asm 0x422A0B-region)
            sprite = 0;                                                    // asm 0x422A15
            RGE_Sprite* fetched = master_obj->vfptr->get_sprite(master_obj, (RGE_Static_Object_idb*)this);   // asm 0x422A1D (the MASTER vtbl slot +4; the result passes to new_sprite, NEVER stored)
            new_sprite(fetched);                                           // asm 0x422A23 (slot +0xB0)
        }
    }
}

// ----------------------------------------------------------------------------
// RGE_Animated_Object::vector_deleting_destructor - the slot-0 override.
// Original: @ 0x0046FE70 (idb_functions.h:1481). Range [0x46FE70,
// 0x46FE8B) (retn 4), void* result. The runtime Animated class has NO
// destructor body of its own: the vdel calls j_RGE_Static_Object__
// destructor (the thunk -> the HOOKED Static destructor entry -> routes),
// then flags & 1 -> operator_delete, return this. The qualified
// pseudo-dtor reproduces it exactly.
// ----------------------------------------------------------------------------
void* RGE_Animated_Object::vector_deleting_destructor(unsigned int flags)
{
    // the body = the base vdel implementation verbatim (the Animated layer
    // adds nothing): the qualified call = the direct base member
    return RGE_Static_Object::vector_deleting_destructor(flags);   // asm: j_RGE_Static_Object__destructor + flags&1 -> operator_delete; mov eax, esi
}

// ----------------------------------------------------------------------------
// RGE_Animated_Object::recycle_in_to_game - the slot-29 override. Original:
// @ 0x00422BF0 (idb_functions.h:546-region). Range [0x422BF0, 0x422C30).
// Decode (.c:73950-73955): the Static recycle + speed = 0 + set_sleep_flag(0).
// ----------------------------------------------------------------------------
void RGE_Animated_Object::recycle_in_to_game(RGE_Master_Static_Object* tobj, RGE_Player* obj_owner,
                                             float x, float y, float z)
{
    RGE_Static_Object::recycle_in_to_game(tobj, obj_owner, x, y, z);   // .c:73952
    speed = 0.0f;                                                      // .c:73953
    RGE_Static_Object::set_sleep_flag(0);                              // .c:73954
}

// ----------------------------------------------------------------------------
// RGE_Animated_Object::update - the slot-37 override. Original:
// @ 0x00422CD0 (idb_functions.h:550-region). Range [0x422CD0, 0x422DF0),
// u8 return. Decode (.c:73992-74046): the base Static update; state == 3
// -> the master death-sprite gate -> set_object_state(4/5); state == 4 &&
// !is_animating -> state = 5 + speed = 0; sprite_list->update(world_time_
// delta_seconds, speed); the sprite2 fidget-swap BOTH directions with
// rand_internal() > 0x3FFF (the common.h wrapper - the owner-directed
// rand form, 2026-11-21).
// ----------------------------------------------------------------------------
unsigned __int8 RGE_Animated_Object::update()
{
    unsigned __int8 result = RGE_Static_Object::update();              // .c:74002
    if (object_state == 3)                                             // .c:74003
    {
        if (master_obj->vfptr->get_death_sprite(master_obj, (RGE_Static_Object_idb*)this) != 0)   // .c:74005 (the MASTER vtbl slot +0xC)
            set_object_state(4);                                       // .c:74006 (slot 53)
        else
            set_object_state(5);                                       // .c:74008
    }
    else if (object_state == 4                                         // .c:74010
        && sprite_list->vfptr->is_animating(sprite_list, sprite) == 0) // .c:74011
    {
        object_state = 5;                                              // .c:74012
        speed = 0.0f;                                                  // .c:74013
    }
    sprite_list->vfptr->update(sprite_list,
        owner->world->world_time_delta_seconds,                        // .c:74017 (TRIBE_World :988)
        speed);                                                        // .c:74015
    if (master_obj->vfptr->get_sprite2(master_obj, (RGE_Static_Object_idb*)this) != 0)   // .c:74019 (the MASTER vtbl slot +0x84)
    {
        if (sprite == master_obj->vfptr->get_sprite2(master_obj, (RGE_Static_Object_idb*)this))   // .c:74022
        {
            if (sprite_list->vfptr->looped(sprite_list, sprite) && rand_internal() > 0x3FFF)   // .c:74024
                new_sprite(master_obj->vfptr->get_sprite2(master_obj, (RGE_Static_Object_idb*)this));   // .c:74027 (slot 44)
        }
        else if (sprite == master_obj->vfptr->get_sprite(master_obj, (RGE_Static_Object_idb*)this))   // .c:74035
        {
            if (sprite_list->vfptr->looped(sprite_list, sprite) && rand_internal() > 0x3FFF)   // .c:74037
                new_sprite(master_obj->vfptr->get_sprite(master_obj, (RGE_Static_Object_idb*)this));   // .c:74041 (slot 44)
        }
    }
    return result;
}

// ----------------------------------------------------------------------------
// RGE_Animated_Object::save - the slot-40 override. Original: @ 0x00422DF0
// (idb_functions.h). Range [0x422DF0, 0x422E20). Decode (.c:74049-74053):
// the Static save + speed rge_write(4).
// ----------------------------------------------------------------------------
void RGE_Animated_Object::save(int outfile)
{
    RGE_Static_Object::save(outfile);                                  // .c:74051
    rge_write(outfile, &speed, sizeof(speed));                         // .c:74052
}

// ----------------------------------------------------------------------------
// RGE_Animated_Object::transform - the slot-51 override. Original:
// @ 0x00422E20. Range [0x422E20, 0x422E30). PURE FORWARDER (.c:74056-74059).
// ----------------------------------------------------------------------------
void RGE_Animated_Object::transform(RGE_Master_Static_Object* obj)
{
    RGE_Static_Object::transform(obj);                                 // .c:74058
}

// ----------------------------------------------------------------------------
// RGE_Animated_Object::set_being_worked_on - the slot-79 override.
// Original: @ 0x00422E30. Range [0x422E30, 0x422E50). PURE FORWARDER
// (.c:74062-74065).
// ----------------------------------------------------------------------------
void RGE_Animated_Object::set_being_worked_on(RGE_Action_Object* work_obj, __int16 action_type,
                                              unsigned __int8 attack_flag)
{
    RGE_Static_Object::set_being_worked_on(work_obj, action_type, attack_flag);   // .c:74064
}

// ----------------------------------------------------------------------------
// RGE_Animated_Object::release_being_worked_on - the slot-80 override.
// Original: @ 0x00422E50. Range [0x422E50, 0x422E60). Decode
// (.c:74068-74075): worker_num-- if nonzero.
// ----------------------------------------------------------------------------
void RGE_Animated_Object::release_being_worked_on(RGE_Static_Object* caller)
{
    unsigned __int8 count = worker_num;
    if (count != 0)                                                    // .c:74071
        worker_num = count - 1;                                        // .c:74072
}

// ----------------------------------------------------------------------------
// RGE_Animated_Object::getSpeed - the slot-96 override. Original:
// @ 0x00408B70. Range [0x408B70, 0x408B80). PURE GETTER: the Animated
// extension's speed field. DOUBLE RETURN (the FLOAT PRECISION corollary
// (h)): the original is fld dword ptr [ecx+0B4h]; retn (asm 0x408B70-
// 0x408B76) - the callers consume st0 at the full x87 precision; the m32
// load is exact, so the double return is bit-identical for this body.
// ----------------------------------------------------------------------------
double RGE_Animated_Object::getSpeed()
{
    return speed;                                                      // asm fld dword ptr [ecx+0B4h]
}

// ----------------------------------------------------------------------------
// RGE_Animated_Object::setup_read - the slot-167 override. Original:
// @ 0x00422CA0 (idb_functions.h:549). Range [0x422CA0, 0x422CD0), returns
// int (1). Decode (.c:73983-73989): the Static setup_read + type = 20 +
// speed rge_read(4).
// ----------------------------------------------------------------------------
int RGE_Animated_Object::setup_read(int infile, RGE_Game_World* gworld)
{
    RGE_Static_Object::setup_read(infile, gworld);                     // .c:73985
    type = RGE_MASTER_TYPE_ANIMATED;                                   // .c:73986
    rge_read(infile, &speed, sizeof(speed));                           // .c:73987
    return 1;
}

// ----------------------------------------------------------------------------
// RGE_Animated_Object::setup - THE NEW APPENDED VIRTUAL (the slot 219; the
// hide-and-append - the base's slot-168 setup stays inherited; see the
// header comment). Original: @ 0x00422C30 (idb_functions.h:548). Range
// [0x422C30, 0x422CA0), returns int (1). Decode (.c:73958-73980): the
// Static setup + type = 20 + speed = 0 + set_sleep_flag(0) + the
// sprite-list bootstrap (the same form as the ctor's).
// ----------------------------------------------------------------------------
int RGE_Animated_Object::setup(RGE_Master_Animated_Object* tobj, RGE_Player* obj_owner,
                               float x, float y, float z)
{
    RGE_Static_Object::setup((RGE_Master_Static_Object*)tobj, obj_owner, x, y, z);   // .c:73963
    type = RGE_MASTER_TYPE_ANIMATED;                                   // .c:73966
    speed = 0.0f;                                                      // .c:73967
    RGE_Static_Object::set_sleep_flag(0);                              // .c:73968
    if (sprite_list == 0)                                              // .c:73969
    {
        sprite_list = create_sprite_list();                            // slot 170 (.c:73971)
        RGE_Master_Animated_Object* master = (RGE_Master_Animated_Object*)master_obj;   // .c:73972 (the family downcast - the Animated master for the Animated unit)
        sprite = 0;                                                    // .c:73974
        RGE_Sprite* fetched = master->vfptr->get_sprite(master, (RGE_Static_Object_idb*)this);   // .c:73976 (the MASTER vtbl slot +4)
        new_sprite(fetched);                                           // slot 44 (.c:73977)
    }
    return 1;
}

// ----------------------------------------------------------------------------
// RGE_Animated_Object::RGE_Animated_Object - the stream ctor overload.
// Original: @ 0x00422B70 (idb_functions.h:546-region). Range [0x422B70,
// 0x422BE5) (retn 0Ch = 3 stack args). Decode (asm 0x422B70-0x422BE5):
// the base RGE_Static_Object_read with do_setup = 0 (asm 0x422B91 push 0);
// the vftable store UNCONDITIONALLY, BEFORE the do_setup branch (asm
// 0x422BAE); IF do_setup: the Static setup_read + type = 0x14 (asm
// 0x422BC9) + rge_read speed (sizeof = 4, asm 0x422BBF-0x422BCD).
// ----------------------------------------------------------------------------
RGE_Animated_Object::RGE_Animated_Object(int infile, RGE_Game_World* gworld, int do_setup)
    : RGE_Static_Object(infile, gworld, 0)                             // asm 0x422B91-0x422B9B (do_setup = 0 ALWAYS)
{
    if (do_setup != 0)                                                 // asm 0x422BAC-0x422BB4
    {
        RGE_Static_Object::setup_read(infile, gworld);                 // asm 0x422BBA (:3804)
        type = RGE_MASTER_TYPE_ANIMATED;                               // asm 0x422BC9 (the byte @+0x5E = 20)
        rge_read(infile, &speed, sizeof(speed));                       // asm 0x422BBF-0x422BCD (sizeof = 4)
    }
}

// ---- the placement-new trampolines (the ::new global form - no
// class-deallocation lookup in the throwing-ctor cleanup; see the
// migration doc 15.3) ----
static RGE_Animated_Object* __fastcall RGE_Animated_Object_ctor_trampoline(
    RGE_Animated_Object* this_, DWORD dummy, RGE_Master_Animated_Object* tobj, TRIBE_Player* obj_owner,
    float x, float y, float z, int do_setup)
{ return ::new (this_) RGE_Animated_Object(tobj, obj_owner, x, y, z, do_setup); }

static RGE_Animated_Object* __fastcall RGE_Animated_Object_ctor_read_trampoline(
    RGE_Animated_Object* this_, DWORD dummy, int infile, RGE_Game_World* gworld, int do_setup)
{ return ::new (this_) RGE_Animated_Object(infile, gworld, do_setup); }

// ---- the qualified-call trampolines (the recursion trap: the unqualified
// form on a derived instance would re-enter these) ----
static void* __fastcall RGE_Animated_Object_vdel_trampoline(
    RGE_Animated_Object* this_, DWORD dummy, unsigned int flags)
{ return this_->RGE_Animated_Object::vector_deleting_destructor(flags); }

static int __fastcall RGE_Animated_Object_setup_trampoline(
    RGE_Animated_Object* this_, DWORD dummy, RGE_Master_Animated_Object* tobj, TRIBE_Player* obj_owner,
    float x, float y, float z)
{ return this_->RGE_Animated_Object::setup(tobj, (RGE_Player*)obj_owner, x, y, z); }   // the dual-naming cast

static int __fastcall RGE_Animated_Object_setup_read_trampoline(
    RGE_Animated_Object* this_, DWORD dummy, int infile, RGE_Game_World* gworld)
{ return this_->RGE_Animated_Object::setup_read(infile, gworld); }

static void __fastcall RGE_Animated_Object_recycle_in_to_game_trampoline(
    RGE_Animated_Object* this_, DWORD dummy, RGE_Master_Static_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z)
{ this_->RGE_Animated_Object::recycle_in_to_game(tobj, obj_owner, x, y, z); }

static unsigned __int8 __fastcall RGE_Animated_Object_update_trampoline(
    RGE_Animated_Object* this_, DWORD dummy)
{ return this_->RGE_Animated_Object::update(); }

static void __fastcall RGE_Animated_Object_save_trampoline(
    RGE_Animated_Object* this_, DWORD dummy, int outfile)
{ this_->RGE_Animated_Object::save(outfile); }

static void __fastcall RGE_Animated_Object_transform_trampoline(
    RGE_Animated_Object* this_, DWORD dummy, RGE_Master_Static_Object* obj)
{ this_->RGE_Animated_Object::transform(obj); }

static void __fastcall RGE_Animated_Object_set_being_worked_on_trampoline(
    RGE_Animated_Object* this_, DWORD dummy, RGE_Action_Object* work_obj, __int16 action_type,
    unsigned __int8 attack_flag)
{ this_->RGE_Animated_Object::set_being_worked_on(work_obj, action_type, attack_flag); }

static void __fastcall RGE_Animated_Object_release_being_worked_on_trampoline(
    RGE_Animated_Object* this_, DWORD dummy, RGE_Static_Object* caller)
{ this_->RGE_Animated_Object::release_being_worked_on(caller); }

static double __fastcall RGE_Animated_Object_getSpeed_trampoline(
    RGE_Animated_Object* this_, DWORD dummy)
{ return this_->RGE_Animated_Object::getSpeed(); }

// ============ THE INSTALLER (the same 12 addresses; the trampoline targets) ============
void install_RGE_Animated_Object()
{
    setHook((void*)0x00422980, RGE_Animated_Object_ctor_trampoline);
    setHook((void*)0x0046FE70, RGE_Animated_Object_vdel_trampoline);
    setHook((void*)0x00422C30, RGE_Animated_Object_setup_trampoline);
    setHook((void*)0x00422CA0, RGE_Animated_Object_setup_read_trampoline);
    setHook((void*)0x00422BF0, RGE_Animated_Object_recycle_in_to_game_trampoline);
    setHook((void*)0x00422CD0, RGE_Animated_Object_update_trampoline);
    setHook((void*)0x00422DF0, RGE_Animated_Object_save_trampoline);
    setHook((void*)0x00422E20, RGE_Animated_Object_transform_trampoline);
    setHook((void*)0x00422E30, RGE_Animated_Object_set_being_worked_on_trampoline);
    setHook((void*)0x00422E50, RGE_Animated_Object_release_being_worked_on_trampoline);
    setHook((void*)0x00408B70, RGE_Animated_Object_getSpeed_trampoline);
    setHook((void*)0x00422B70, RGE_Animated_Object_ctor_read_trampoline);
}
