#include "stdafx.h"
#include "RGE_Master_Static_Object.h"

// .rdata vftable of RGE_Animated_Object (lst :987149)
static RGE_Animated_ObjectVtbl* const rge_animated_object_vftable =
    (RGE_Animated_ObjectVtbl*)0x006566A0;

// Replacement for RGE_Animated_Object::RGE_Animated_Object (runtime ctor)
// Original: @ 0x00422980 (idb_functions.h:538). Range [0x422980,
// 0x422A3D) (lst :62193/:62283, retn 18h = 6 stack args), returns this.
// ABI: __thiscall - this_ in ECX. Callers: RGE_Master_Animated_Object__
// make_new_obj (owned, hook-routed), RGE_Doppleganger_Object__RGE_
// Doppleganger_Object+3E (lst :62194), RGE_Object_List__load paths.
// Decode (asm 0x422980-0x422A3C): base Static runtime ctor with
// do_setup = 0 (hooked -> routes), the Animated vfptr store, then the
// do_setup body: the RUNTIME Static setup (tobj, obj_owner, x, y, z);
// type @+0x5E = 0x14 (the runtime object's tier mirror - idb field
// `type`, asm 0x4229F1); speed @+0xB4 = 0 (the Animated extension,
// asm 0x4229F5); set_sleep_flag(0); and the sprite-list bootstrap:
// if sprite_list @+0x24 == 0 -> sprite_list = vfptr->create_sprite_list
// (RGE_Static_ObjectVtbl slot +0x2A8 = index 170, idb_types.h:4105),
// sprite = 0, then master->vfptr->get_sprite(master, this) (the MASTER
// vtbl slot +4) and vfptr->new_sprite(this, sprite) (slot +0xB0).
// SEH fold: unwind chunk 0x640F70 = mov ecx, [ebp-10h] / jmp
// RGE_Static_Object__destructor (0x54B2E0 - HOOKED -> routes into
// RGE_Static_Object__destructor_new per the hook-routing ruling).
RGE_Animated_Object* __fastcall RGE_Animated_Object__RGE_Animated_Object_new(
    RGE_Animated_Object* this_, DWORD dummy,
    RGE_Master_Animated_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z, int do_setup)
{
    RGE_Animated_Object* result = this_;
    __try
    {
        RGE_Static_Object__RGE_Static_Object(
            (RGE_Static_Object*)this_, (RGE_Master_Static_Object*)tobj, obj_owner, x, y, z, 0);
        this_->vfptr = rge_animated_object_vftable;
        if (do_setup != 0)
        {
            RGE_Static_Object__setup((RGE_Static_Object*)this_, (RGE_Master_Static_Object*)tobj, obj_owner, x, y, z);
            this_->type = RGE_MASTER_TYPE_ANIMATED;                                   // asm 0x4229F1 ? runtime tier mirror
            this_->speed = 0.0f;                                  // asm 0x4229F5 ? the Animated extension @0xB4
            RGE_Static_Object__set_sleep_flag((RGE_Static_Object*)this_, 0);          // idb_functions.h:3891
            if (this_->sprite_list == 0)                          // @+0x24 (asm 0x422A00)
            {
                this_->sprite_list = this_->vfptr->create_sprite_list(this_);   // slot +0x2A8
                this_->sprite = 0;                                // asm 0x422A15
                RGE_Master_Animated_Object* master = this_->master_obj;
                RGE_Sprite* new_sprite = master->vfptr->get_sprite(master, (RGE_Static_Object_idb*)this_);   // asm 0x422A1D ? master vtbl slot +4; result NOT stored (passes to new_sprite)
                this_->vfptr->new_sprite(this_, new_sprite);      // asm 0x422A23 ? slot +0xB0
            }
        }
        result = this_;
    }
    __except (RGE_Static_Object__destructor((RGE_Static_Object*)this_), EXCEPTION_CONTINUE_SEARCH)
    {
    }
    return result;
}

// Replacement for RGE_Animated_Object::vector_deleting_destructor
// Original: @ 0x0046FE70 (idb_functions.h:1481). Range [0x46FE70,
// 0x46FE8B) (lst :189152/:189171, retn 4), void* result. ABI:
// __thiscall. Virtual: vftable DATA XREF (lst :189153) - also the
// Doppleganger runtime vftable's slot (lst :189154). The runtime
// Animated class has NO destructor body of its own: the vdel calls
// j_RGE_Static_Object__destructor (the thunk -> 0x54B2E0 -> HOOKED ->
// routes into RGE_Static_Object__destructor_new per the hook-routing
// ruling), then flags & 1 -> operator_delete, return this.
void* __fastcall RGE_Animated_Object__vector_deleting_destructor_new(
    RGE_Animated_Object* this_, DWORD dummy, unsigned int flags)
{
    RGE_Static_Object__destructor((RGE_Static_Object*)this_);
    if (flags & 1)
        operator_delete_internal(this_);
    return this_;
}

// ============ RUNTIME METHODS (batch M1) ============

// Replacement for RGE_Animated_Object::setup. Original: @ 0x00422C30
// (idb_functions.h:548). Range [0x422C30, 0x422CA0), returns int (1).
// Decode (.c:73958-73980): Static setup (:1910-region) + type=20 +
// speed=0 + set_sleep_flag(0) + the sprite-list bootstrap (same as the
// ctor's; create_sprite_list -> master get_sprite -> new_sprite).
int __fastcall RGE_Animated_Object__setup_new(
    RGE_Animated_Object* this_, DWORD dummy,
    RGE_Master_Animated_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z)
{
    RGE_Static_Object__setup((RGE_Static_Object*)this_,
        (RGE_Master_Static_Object*)tobj, obj_owner, x, y, z);
    this_->type = RGE_MASTER_TYPE_ANIMATED;                                // .c:73966
    this_->speed = 0.0f;                             // .c:73967
    RGE_Static_Object__set_sleep_flag((RGE_Static_Object*)this_, 0);   // .c:73968
    if (this_->sprite_list == 0)                     // .c:73969
    {
        this_->sprite_list = this_->vfptr->create_sprite_list(this_);   // .c:73971
        RGE_Master_Animated_Object* master = this_->master_obj;         // .c:73972
        this_->sprite = 0;                                           // .c:73974
        RGE_Sprite* new_sprite = master->vfptr->get_sprite(master, (RGE_Static_Object_idb*)this_);   // .c:73976
        this_->vfptr->new_sprite(this_, new_sprite);                 // .c:73977
    }
    return 1;
}

// Replacement for RGE_Animated_Object::setup_read. Original:
// @ 0x00422CA0 (idb_functions.h:549). Range [0x422CA0, 0x422CD0),
// returns int (1). Decode (.c:73983-73989): Static setup_read +
// type=20 + speed rge_read(4).
int __fastcall RGE_Animated_Object__setup_read_new(
    RGE_Animated_Object* this_, DWORD dummy, int infile, RGE_Game_World* gworld)
{
    RGE_Static_Object__setup_read((RGE_Static_Object*)this_, infile, gworld);   // .c:73985
    this_->type = RGE_MASTER_TYPE_ANIMATED;                                // .c:73986
    rge_read(infile, &this_->speed, sizeof(this_->speed));   // .c:73987
    return 1;
}

// Replacement for RGE_Animated_Object::recycle_in_to_game. Original:
// @ 0x00422BF0 (idb_functions.h:546-region). Range [0x422BF0,
// 0x422C30). Decode (.c:73950-73955): Static recycle + speed=0 +
// set_sleep_flag(0).
void __fastcall RGE_Animated_Object__recycle_in_to_game_new(
    RGE_Animated_Object* this_, DWORD dummy,
    RGE_Master_Static_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z)
{
    RGE_Static_Object__recycle_in_to_game((RGE_Static_Object*)this_, tobj, obj_owner, x, y, z);   // .c:73952
    this_->speed = 0.0f;                             // .c:73953
    RGE_Static_Object__set_sleep_flag((RGE_Static_Object*)this_, 0);   // .c:73954
}

// Replacement for RGE_Animated_Object::update. Original: @ 0x00422CD0
// (idb_functions.h:550-region). Range [0x422CD0, 0x422DF0), u8 return.
// Decode (.c:73992-74046): base Static update; state==3 -> the master
// death-sprite gate -> set_object_state(4/5); state==4 && !is_animating
// -> state=5 + speed=0; sprite_list->update(world_time_delta_seconds,
// speed); the sprite2 fidget-swap BOTH directions with rand_internal() > 0x3FFF
// (the common.h:154 wrapper - the owner-directed rand form, 2026-11-21).
unsigned __int8 __fastcall RGE_Animated_Object__update_new(RGE_Animated_Object* this_, DWORD dummy)
{
    unsigned __int8 result = RGE_Static_Object__update((RGE_Static_Object*)this_);   // .c:74002
    if (this_->object_state == 3)                    // .c:74003
    {
        if (this_->master_obj->vfptr->get_death_sprite(this_->master_obj, (RGE_Static_Object_idb*)this_) != 0)   // .c:74005
            this_->vfptr->set_object_state(this_, 4);   // .c:74006
        else
            this_->vfptr->set_object_state(this_, 5);   // .c:74008
    }
    else if (this_->object_state == 4                // .c:74010
        && this_->sprite_list->vfptr->is_animating(this_->sprite_list, this_->sprite) == 0)
    {
        this_->object_state = 5;                     // .c:74012
        this_->speed = 0.0f;                         // .c:74013
    }
    this_->sprite_list->vfptr->update(this_->sprite_list,
        this_->owner->world->world_time_delta_seconds,   // .c:74017 (TRIBE_World :988)
        this_->speed);                               // .c:74015-0x49C1-analog
    if (this_->master_obj->vfptr->get_sprite2(this_->master_obj, (RGE_Static_Object_idb*)this_) != 0)   // .c:74019
    {
        if (this_->sprite == this_->master_obj->vfptr->get_sprite2(this_->master_obj, (RGE_Static_Object_idb*)this_))   // .c:74022
        {
            if (this_->sprite_list->vfptr->looped(this_->sprite_list, this_->sprite) && rand_internal() > 0x3FFF)   // .c:74024
                this_->vfptr->new_sprite(this_,
                    this_->master_obj->vfptr->get_sprite2(this_->master_obj, (RGE_Static_Object_idb*)this_));   // .c:74027
        }
        else if (this_->sprite == this_->master_obj->vfptr->get_sprite(this_->master_obj, (RGE_Static_Object_idb*)this_))   // .c:74035
        {
            if (this_->sprite_list->vfptr->looped(this_->sprite_list, this_->sprite) && rand_internal() > 0x3FFF)   // .c:74037
                this_->vfptr->new_sprite(this_,
                    this_->master_obj->vfptr->get_sprite(this_->master_obj, (RGE_Static_Object_idb*)this_));   // .c:74041
        }
    }
    return result;
}

// Replacement for RGE_Animated_Object::save. Original: @ 0x00422DF0
// (idb_functions.h). Range [0x422DF0, 0x422E20). Decode (.c:74049-
// 74053): Static save + speed rge_write(4).
void __fastcall RGE_Animated_Object__save_new(
    RGE_Animated_Object* this_, DWORD dummy, int outfile)
{
    RGE_Static_Object__save((RGE_Static_Object*)this_, outfile);   // .c:74051
    rge_write(outfile, &this_->speed, sizeof(this_->speed));   // .c:74052
}

// Replacement for RGE_Animated_Object::transform. Original:
// @ 0x00422E20. Range [0x422E20, 0x422E30). PURE FORWARDER (.c:74056-
// 74059).
void __fastcall RGE_Animated_Object__transform_new(
    RGE_Animated_Object* this_, DWORD dummy, RGE_Master_Static_Object* obj)
{
    RGE_Static_Object__transform((RGE_Static_Object*)this_, obj);
}

// Replacement for RGE_Animated_Object::set_being_worked_on. Original:
// @ 0x00422E30. Range [0x422E30, 0x422E50). PURE FORWARDER (.c:74062-
// 74065).
void __fastcall RGE_Animated_Object__set_being_worked_on_new(
    RGE_Animated_Object* this_, DWORD dummy, RGE_Action_Object* work_obj,
    __int16 action_type, unsigned __int8 attack_flag)
{
    RGE_Static_Object__set_being_worked_on((RGE_Static_Object*)this_, work_obj, action_type, attack_flag);
}

// Replacement for RGE_Animated_Object::release_being_worked_on.
// Original: @ 0x00422E50. Range [0x422E50, 0x422E60). Decode
// (.c:74068-74075): worker_num-- if nonzero.
void __fastcall RGE_Animated_Object__release_being_worked_on_new(
    RGE_Animated_Object* this_, DWORD dummy, RGE_Static_Object* caller)
{
    unsigned __int8 count = this_->worker_num;
    if (count != 0)
        this_->worker_num = count - 1;
}

// Replacement for RGE_Animated_Object::getSpeed. Original: @ 0x00408B70
// SEMANTIC: the unit's movement-speed getter - returns the Animated extension's speed field (the live animation rate the sprite-list update consumes).
// (idb_functions.h). Range [0x408B70, 0x408B80). PURE GETTER (.c:
// 51396-region: return speed @+0xB4). DOUBLE RETURN (FLOAT PRECISION
// corollary (h)): the original is fld dword ptr [ecx+0B4h]; retn
// (asm 0x408B70-0x408B76) - callers consume st0 at full x87 precision;
// a float-typed return truncates at the 24-bit call boundary. The m32
// load is exact, so the double return is bit-identical for this body.
double __fastcall RGE_Animated_Object__getSpeed_new(RGE_Animated_Object* this_, DWORD dummy)
{
    return this_->speed;
}

// Replacement for RGE_Animated_Object::RGE_Animated_Object_read.
// Original: @ 0x00422B70 (idb_functions.h:546). Range [0x422B70,
// 0x422BE5) (retn 0Ch = 3 stack args), void. SEH chunk 0x640F90 =
// mov ecx, [ebp-10h]; jmp RGE_Static_Object__destructor (the
// throw-during-construction fold - the HOOKED runtime Static
// destructor). Completes the Animated class (12/12 exports owned):
// the owned Doppleganger/Moving read-ctors' base calls (the 0x422B70
// call sites in RGE_Doppleganger_Object__RGE_Doppleganger_Object_read
// +2D / RGE_Moving_Object__RGE_Moving_Object_read +2D) now route
// through this hook per the hook-routing ruling.
// Decode (asm 0x422B70-0x422BE5): base RGE_Static_Object_read
// (:3787, HOOKED -> routes) with do_setup = 0 (asm 0x422B91 push 0);
// vfptr = RGE_Animated_Object::vftable (0x006566A0, asm 0x422BAE -
// stored UNCONDITIONALLY, BEFORE the do_setup branch); IF do_setup:
// RGE_Static_Object__setup_read (:3804) + type @+0x5E = 0x14 (asm
// 0x422BC9, the runtime tier mirror) + rge_read (:3003, infile,
// &speed @+0xB4, 4 = sizeof(float), asm 0x422BBF-0x422BCD).
void __fastcall RGE_Animated_Object__RGE_Animated_Object_read_new(
    RGE_Animated_Object* this_, DWORD dummy, int infile, RGE_Game_World* gworld, int do_setup)
{
    __try
    {
        RGE_Static_Object__RGE_Static_Object_read(
            (RGE_Static_Object*)this_, infile, gworld, 0);    // asm 0x422B91-0x422B9B (do_setup = 0; HOOKED -> routes)
        this_->vfptr = rge_animated_object_vftable;           // asm 0x422BAE (0x006566A0)
        if (do_setup != 0)                                    // asm 0x422BAC-0x422BB4
        {
            RGE_Static_Object__setup_read((RGE_Static_Object*)this_, infile, gworld);   // asm 0x422BBA (:3804)
            this_->type = RGE_MASTER_TYPE_ANIMATED;                               // asm 0x422BC9 (byte @+0x5E = 20)
            rge_read(infile, &this_->speed, sizeof(this_->speed));   // asm 0x422BBF-0x422BCD (sizeof = 4)
        }
    }
    __except (RGE_Static_Object__destructor((RGE_Static_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x640F90
    {
    }
}

void install_RGE_Animated_Object()
{
    setHook((void*)0x00422980, RGE_Animated_Object__RGE_Animated_Object_new);
    setHook((void*)0x0046FE70, RGE_Animated_Object__vector_deleting_destructor_new);
    setHook((void*)0x00422C30, RGE_Animated_Object__setup_new);
    setHook((void*)0x00422CA0, RGE_Animated_Object__setup_read_new);
    setHook((void*)0x00422BF0, RGE_Animated_Object__recycle_in_to_game_new);
    setHook((void*)0x00422CD0, RGE_Animated_Object__update_new);
    setHook((void*)0x00422DF0, RGE_Animated_Object__save_new);
    setHook((void*)0x00422E20, RGE_Animated_Object__transform_new);
    setHook((void*)0x00422E30, RGE_Animated_Object__set_being_worked_on_new);
    setHook((void*)0x00422E50, RGE_Animated_Object__release_being_worked_on_new);
    setHook((void*)0x00408B70, RGE_Animated_Object__getSpeed_new);
    setHook((void*)0x00422B70, RGE_Animated_Object__RGE_Animated_Object_read_new);
}
