#include "stdafx.h"
#include "..\object\RGE_Action_Types.h"

// the RGE_Action vftable (.rdata:00655378, lst :986658; slot 0 = the
// vector deleting destructor; slot 3 = create_action_list - the base
// vtbl, idb_types.h:15978+3)
static RGE_ActionVtbl* const rge_action_vftable =
    (RGE_ActionVtbl*)0x00655378;

// Replacement for RGE_Action::RGE_Action2 (the read ctor). Original:
// @ 0x0040A7F0 (idb_functions.h:170). Range [0x40A7F0, 0x40A822) (lst
// :18191/:18211, retn 0Ch = 3 stack args), RGE_Action* return. ABI:
// __thiscall. NOT virtual (the plain ctor - the derived read ctors call
// it: RGE_Action_Attack__RGE_Action_Attack3+2A, RGE_Action_Bird__RGE_
// Action_Bird2+11, ...). Decode (asm 0x40A7F0-0x40A821): targetID = -1
// (dword @+0x18), target2ID = -1 (@+0x1C), mTargetMovedState = 0 (byte
// @+0x3D), the vfptr store (the static vftable @0x00655378),
// do_setup -> setup2(this, infile, obj_in) (:174); return this.
RGE_Action* __fastcall RGE_Action__RGE_Action2_new(
    RGE_Action* this_, DWORD dummy, int infile, RGE_Action_Object* obj_in,
    int do_setup)
{
    this_->targetID = -1;                                        // asm 0x40A7F3-0x40A7F6 (mov dword @+0x18, -1)
    this_->target2ID = -1;                                       // asm 0x40A7F9-0x40A7FC (@+0x1C)
    this_->mTargetMovedState = 0;                                // asm 0x40A800 (mov byte @+0x3D, 0)
    this_->vfptr = rge_action_vftable;                           // asm 0x40A806 (the static vftable)
    if (do_setup)                                                // asm 0x40A804
        RGE_Action__setup2(this_, infile, obj_in);               // asm 0x40A818 (:175 - the idb constant: the entry is HOOKED, the call routes into setup2_new - the hook-routing ruling, the derived-ctor base-call precedent)
    return this_;                                                // asm 0x40A81F (mov eax, esi)
}

// Replacement for RGE_Action::vector_deleting_destructor. Original:
// @ 0x0040A830 (idb_functions.h:171). Range [0x40A830, 0x40A870) (lst
// :18214/:18235, retn 4 = 1 stack arg), void* return (this). ABI:
// __thiscall. Virtual: the Action vftable slot 0 (DATA XREF lst :18215).
// Decode (asm 0x40A830-0x40A86F): the vfptr store; set_target_obj(0)
// (:182) / set_target_obj2(0) (:183); sub_actions @+0x34 non-null ->
// the __vecDelDtor virtual (the List vftable slot 0, flags = 1) + null;
// flags & 1 -> operator_delete(this) - the bare operator_delete maps to
// operator_delete_internal (the wrapper rule); return this.
void* __fastcall RGE_Action__vector_deleting_destructor_new(
    RGE_Action* this_, DWORD dummy, unsigned int flags)
{
    this_->vfptr = rge_action_vftable;                           // asm 0x40A835
    RGE_Action__set_target_obj(this_, 0);                        // asm 0x40A83B (:182, arg 0)
    RGE_Action__set_target_obj2(this_, 0);                       // asm 0x40A844 (:183, arg 0)
    if (this_->sub_actions)                                      // asm 0x40A849 (@+0x34)
    {
        this_->sub_actions->vfptr->__vecDelDtor(this_->sub_actions, 1);   // asm 0x40A854 (the List vftable slot 0)
        this_->sub_actions = 0;                                  // asm 0x40A856
    }
    if (flags & 1)                                               // asm 0x40A85D
        operator_delete_internal(this_);                         // asm 0x40A865 (the bare operator_delete -> the wrapper rule)
    return this_;                                                // asm 0x40A86D (mov eax, esi)
}

// Replacement for RGE_Action::RGE_Action (the ctor). Original:
// @ 0x0040A880 (idb_functions.h:172). Range [0x40A880, 0x40A8DA) (lst
// :18243/:18267, retn 8 = 2 stack args), RGE_Action* return. ABI:
// __thiscall. NOT virtual. Callers: the derived ctors
// (RGE_Action_Attack__RGE_Action_Attack+27, ..._Attack2+24, ...).
// Decode (asm 0x40A880-0x40A8D9): the -1/-1/0 head (as RGE_Action2);
// do_setup -> the field-zero block in the asm order: action_type = 0
// (word @+4), obj = obj_in (@+8), state = 0 (@+0xC), target_obj/2 = 0
// (@+0x10/14), target_x/y/z = 0.0 (@+0x20/24/28), timer = 0.0 (@+0x2C),
// task = 0 (@+0x30), sub_actions = create_action_list(this, obj_in)
// (@+0x34), sprite = 0 (@+0x38), subActionValue = 0 (@+0x3C),
// mTargetMovedState = 0 (@+0x3D); return this.
// THE SUB_ACTIONS CALL: the original calls through the STATIC vftable's
// slot (call ds:RGE_Action__vftable.create_action_list, asm 0x40A8C4 -
// the constant content = the base implementation, RGE_Action__create_
// action_list @0x40AA90) -> the faithful form = the direct idb-constant
// call per the hook-routing ruling.
RGE_Action* __fastcall RGE_Action__RGE_Action_new(
    RGE_Action* this_, DWORD dummy, RGE_Action_Object* obj_in, int do_setup)
{
    this_->targetID = -1;                                        // asm 0x40A884-0x40A889
    this_->target2ID = -1;                                       // asm 0x40A88C
    this_->mTargetMovedState = 0;                                // asm 0x40A893 (bl = 0)
    this_->vfptr = rge_action_vftable;                           // asm 0x40A898
    if (do_setup)                                                // asm 0x40A89E
    {
        this_->action_type = 0;                                  // asm 0x40A8A4 (mov word @+4, bx)
        this_->obj = obj_in;                                     // asm 0x40A8A9 (@+8)
        this_->state = 0;                                        // asm 0x40A8AC (@+0xC)
        this_->target_obj = 0;                                   // asm 0x40A8AF (@+0x10)
        this_->target_obj2 = 0;                                  // asm 0x40A8B2 (@+0x14)
        this_->target_x = 0.0f;                                  // asm 0x40A8B5 (@+0x20)
        this_->target_y = 0.0f;                                  // asm 0x40A8B8 (@+0x24)
        this_->target_z = 0.0f;                                  // asm 0x40A8BB (@+0x28)
        this_->timer = 0.0f;                                     // asm 0x40A8BE (@+0x2C)
        this_->task = 0;                                         // asm 0x40A8C1 (@+0x30)
        this_->sub_actions = RGE_Action__create_action_list(this_, obj_in);   // asm 0x40A8C4 (the static-vftable slot call -> the direct constant, the hook-routing ruling; @+0x34; :176)
        this_->sprite = 0;                                       // asm 0x40A8CD (@+0x38)
        this_->subActionValue = 0;                               // asm 0x40A8D0 (@+0x3C)
        this_->mTargetMovedState = 0;                            // asm 0x40A8D3 (@+0x3D)
    }
    return this_;                                                // asm 0x40A8D6 (mov eax, esi)
}

// Replacement for RGE_Action::destructor. Original: @ 0x0040A8E0
// (idb_functions.h:173). Range [0x40A8E0, 0x40A910) (lst :18270+),
// void, no stack args. ABI: __thiscall. Virtual: the Action vftable
// slot 0. Callers: the derived destructors (Attack/Enter/... +6 - the
// ladder). Decode: the vfptr store; set_target_obj/2(0); the sub_actions
// __vecDelDtor(1) + null. NO delete (the caller deletes).
void __fastcall RGE_Action__destructor_new(RGE_Action* this_, DWORD dummy)
{
    this_->vfptr = rge_action_vftable;                           // asm 0x40A8E5
    RGE_Action__set_target_obj(this_, 0);                        // asm (:182)
    RGE_Action__set_target_obj2(this_, 0);                       // asm (:183)
    if (this_->sub_actions)                                      // asm (@+0x34)
    {
        this_->sub_actions->vfptr->__vecDelDtor(this_->sub_actions, 1);   // the List vftable slot 0
        this_->sub_actions = 0;
    }
}

// Replacement for RGE_Action::setup2 (the stream loader). Original:
// @ 0x0040A910 (idb_functions.h:174). Range [0x40A910, 0x40AA40) (retn
// 8 = 2 stack args), int return (1). ABI: __thiscall. NOT virtual.
// Callers: RGE_Action2 (above), TRIBE_Action_Repair__TRIBE_Action_
// Repair+54 (the ladder, lst :10075 region). Decode (asm 0x40A910-
// 0x40AA3E): action_type = 0 (word @+4); obj = obj_in (@+8);
// sub_actions = THE VIRTUAL create_action_list (call [edx+0Ch], the
// base vtbl slot 3 - the vfptr at this point is the DERIVED class's
// (stored by the ctor first), so the derived override runs - the named
// dispatch fidelity); then the sizeof-form stream reads in the asm
// order: state @+0xC (1), the target-id staging (4) with the == -1 ->
// 0 else the raw-value store (the id-fixup form, faithful), target_obj
// @+0x10 / target_obj2 @+0x14, targetID @+0x18 (4), target2ID @+0x1C,
// target_x/y/z @+0x20/24/28 (4 x3), timer @+0x2C (4),
// mTargetMovedState @+0x3D (1), the task-id staging (2) with == -1 ->
// task = 0 else task = obj->master_obj->tasks->list[id] (the Action
// master tasks @260, warm), subActionValue @+0x3C (1),
// RGE_Action_List__load(sub_actions, infile) (:59), the sprite-id
// staging (2) -> sprite = get_sprite_pointer(obj, id) (:3829); the
// 0x40AA30-region tail: return 1.
int __fastcall RGE_Action__setup2_new(
    RGE_Action* this_, DWORD dummy, int infile, RGE_Action_Object* obj_in)
{
    this_->action_type = 0;                                      // asm 0x40A91F (mov word @+4, bx)
    this_->obj = obj_in;                                         // asm 0x40A923 (@+8)
    this_->sub_actions = this_->vfptr->create_action_list(this_, obj_in);   // asm 0x40A926 (call [edx+0Ch] - the base vtbl slot 3 = +0xC; the DERIVED override runs - the named-dispatch fidelity; the explicit-this form)
    rge_read(infile, &this_->state, sizeof(this_->state));       // asm 0x40A937 (@+0xC, 1)
    int id = 0;
    rge_read(infile, &id, sizeof(id));                           // asm 0x40A944 (the staging slot, 4)
    this_->target_obj = (id == -1) ? 0 : (RGE_Static_Object*)id;   // asm 0x40A94D-0x40A957 (the id-fixup form, faithful)
    rge_read(infile, &id, sizeof(id));                           // asm 0x40A962
    this_->target_obj2 = (id == -1) ? 0 : (RGE_Static_Object*)id;   // asm 0x40A96B-0x40A975 (@+0x14)
    rge_read(infile, &this_->targetID, sizeof(this_->targetID));   // asm 0x40A97F (@+0x18, 4)
    rge_read(infile, &this_->target2ID, sizeof(this_->target2ID));   // asm 0x40A984 (@+0x1C, 4)
    rge_read(infile, &this_->target_x, sizeof(this_->target_x));   // asm 0x40A990 (@+0x20, 4)
    rge_read(infile, &this_->target_y, sizeof(this_->target_y));   // asm 0x40A99C (@+0x24, 4)
    rge_read(infile, &this_->target_z, sizeof(this_->target_z));   // asm 0x40A9A8 (@+0x28, 4)
    rge_read(infile, &this_->timer, sizeof(this_->timer));       // asm 0x40A9B4 (@+0x2C, 4)
    rge_read(infile, &this_->mTargetMovedState, sizeof(this_->mTargetMovedState));   // asm (@+0x3D, 1)
    __int16 sprite_id = -1;
    rge_read(infile, &sprite_id, sizeof(sprite_id));             // asm (the task-id staging, 2)
    if (sprite_id == -1)                                         // asm (cmp -1)
        this_->task = 0;                                         // asm (@+0x30)
    else
        this_->task = this_->obj->master_obj->tasks->list[sprite_id];   // asm (the Action master tasks @260 -> the Task_List list[id]; the warm ledger walk)
    rge_read(infile, &this_->subActionValue, sizeof(this_->subActionValue));   // asm (@+0x3C, 1)
    RGE_Action_List__load(this_->sub_actions, infile);           // asm (:59)
    rge_read(infile, &sprite_id, sizeof(sprite_id));             // asm (the sprite-id staging, 2)
    this_->sprite = RGE_Static_Object__get_sprite_pointer((RGE_Static_Object*)this_->obj, sprite_id);   // asm (:3829; the up-cast - the verified Action_Object subset Static_Object ladder)
    return 1;                                                    // asm (mov eax, 1)
}

// Replacement for RGE_Action::setup. Original: @ 0x0040AA40
// (idb_functions.h:175). Range [0x40AA40, 0x40AA82) (lst region; retn 4
// = 1 stack arg), int return (1). ABI: __thiscall. NOT virtual. The
// vftable DATA XREF: the DERIVED vftables slot the BASE implementation
// (RGE_Action_Attack__vftable, RGE_Action_Bird__vftable, ... - the
// lst DATA XREFs at the proc head). Decode (asm 0x40AA40-0x40AA81):
// action_type = 0 (word @+4); obj = obj_in (@+8); state = 0 (@+0xC);
// target_obj/2 = 0 (@+0x10/14); target_x/y/z = 0.0 (@+0x20/24/28);
// timer = 0.0 (@+0x2C); task = 0 (@+0x30); sub_actions = the VIRTUAL
// create_action_list (call [edx+0Ch], the base vtbl slot 3 - the named
// dispatch); sprite = 0 (@+0x38); subActionValue = 0 (@+0x3C);
// mTargetMovedState = 0 (@+0x3D); return 1.
int __fastcall RGE_Action__setup_new(
    RGE_Action* this_, DWORD dummy, RGE_Action_Object* obj_in)
{
    this_->action_type = 0;                                      // asm 0x40AA4D (mov word @+4, bx)
    this_->obj = obj_in;                                         // asm 0x40AA51 (@+8)
    this_->state = 0;                                            // asm 0x40AA54 (@+0xC)
    this_->target_obj = 0;                                       // asm 0x40AA57 (@+0x10)
    this_->target_obj2 = 0;                                      // asm 0x40AA5A (@+0x14)
    this_->target_x = 0.0f;                                      // asm 0x40AA5D (@+0x20)
    this_->target_y = 0.0f;                                      // asm 0x40AA60 (@+0x24)
    this_->target_z = 0.0f;                                      // asm 0x40AA63 (@+0x28)
    this_->timer = 0.0f;                                         // asm 0x40AA66 (@+0x2C)
    this_->task = 0;                                             // asm 0x40AA69 (@+0x30)
    this_->sub_actions = this_->vfptr->create_action_list(this_, obj_in);   // asm 0x40AA6C (call [edx+0Ch] - the base vtbl slot 3 = +0xC; the DERIVED override runs; the explicit-this form)
    this_->sprite = 0;                                           // asm 0x40AA72 (@+0x38)
    this_->subActionValue = 0;                                   // asm 0x40AA75 (@+0x3C)
    this_->mTargetMovedState = 0;                                // asm 0x40AA78 (@+0x3D)
    return 1;                                                    // asm 0x40AA7C (mov eax, 1)
}

// ============ RUNTIME METHODS (batch A2a) ============

// Replacement for RGE_Action::create_action_list. Original: @ 0x0040AA90
// (idb_functions.h:176). Range [0x40AA90, 0x40AAF1) (lst region; retn 4 =
// 1 stack arg), RGE_Action_List* return. ABI: __thiscall. Virtual: the
// Action vftable slot 3 (the DERIVED vftables slot it: RGE_Action_Attack/
// Bird/... - the lst DATA XREFs). Decode (asm 0x40AA90-0x40AAF0):
// operator_new(0xC) (the walked sizeof(RGE_Action_List) = 12), the SEH
// allocation fold (the unwind chunk @0x640B00), then
// RGE_Action_List__RGE_Action_List(allocated, this->obj @+0x8) (:56);
// returns the allocation (the ctor's this).
RGE_Action_List* __fastcall RGE_Action__create_action_list_new(
    RGE_Action* this_, DWORD dummy, RGE_Action_Object* obj_in)
{
    RGE_Action_List* allocated = (RGE_Action_List*)operator_new_internal(sizeof(RGE_Action_List));   // asm 0x40AAA9 (push 0Ch; the walked sizeof = 12)
    __try
    {
        if (allocated)
            RGE_Action_List__RGE_Action_List(allocated, this_->obj);   // asm 0x40AAC9 (:56; obj @+0x8)
    }
    __except (operator_delete_internal(allocated), EXCEPTION_CONTINUE_SEARCH)   // the SEH fold: the unwind chunk @0x640B00
    {
    }
    return allocated;                                                // asm 0x40AACE/0x40AAE6
}

// Replacement for RGE_Action::rehook (the id-fixup recursion). Original:
// @ 0x0040AB00 (idb_functions.h:60... the RGE_Action_List__rehook is the
// :60 sibling; the base's own idb line verified at write-prep). Range
// [0x40AB00, 0x40AB4A). void, no stack args. ABI: __thiscall. Virtual:
// the Action vftable slot 4. Decode (asm 0x40AB00-0x40AB49): the
// target_obj @+0x10 (the RAW-ID store from setup2 - the pointer slot
// holds the id until rehooked) -> get_object_pointer(obj @+0x8, the id)
// (:3828) -> re-stored; non-null -> targetID @+0x18 = resolved->id
// (@+0x10); target_obj2 @+0x14 -> target2ID @+0x1C the same; the
// sub_actions @+0x34 recursion = the TAIL-JMP (asm 0x40AB45 jmp
// RGE_Action_List__rehook - the replacement's tail call).
void __fastcall RGE_Action__rehook_new(RGE_Action* this_, DWORD dummy)
{
    if (this_->target_obj)                                       // asm 0x40AB06-0x40AB08 (@+0x10)
    {
        this_->target_obj = RGE_Static_Object__get_object_pointer((RGE_Static_Object*)this_->obj, (int)this_->target_obj);   // asm 0x40AB0E (:3828; the id-fixup: the pointer slot holds the raw id)
        if (this_->target_obj)                                   // asm 0x40AB18
            this_->targetID = this_->target_obj->id;             // asm 0x40AB1A-0x40AB1D (@+0x18; id @+0x10)
    }
    if (this_->target_obj2)                                      // asm 0x40AB23 (@+0x14)
    {
        this_->target_obj2 = RGE_Static_Object__get_object_pointer((RGE_Static_Object*)this_->obj, (int)this_->target_obj2);   // asm 0x40AB2B
        if (this_->target_obj2)                                  // asm 0x40AB35
            this_->target2ID = this_->target_obj2->id;           // asm 0x40AB37-0x40AB3A (@+0x1C)
    }
    if (this_->sub_actions)                                      // asm 0x40AB3D (@+0x34)
        RGE_Action_List__rehook(this_->sub_actions);             // asm 0x40AB45 (the TAIL-JMP recursion - the replacement's tail call)
}

// Replacement for RGE_Action::save. Original: @ 0x0040AB50
// (idb_functions.h:61). Range [0x40AB50, 0x40AC70) (retn 4 = 1 stack
// arg), void. ABI: __thiscall. NOT virtual. Callers: the derived saves
// (Attack/Gather/... +9 - the ladder). Decode (asm 0x40AB50-0x40AC69):
// the write-mirror of setup2 in the exact field order - action_type
// @+4 (2), state @+0xC (1), the target ids: the id-or-(-1) stagings
// (the target->id @+0x10 int, the 4-byte writes), targetID @+0x18 (4),
// target2ID @+0x1C (4), target_x/y/z/timer @+0x20/24/28/2C (4 x4),
// mTargetMovedState @+0x3D (1), the task id staging (the i16 @+2, the
// 2-byte write), subActionValue @+0x3C (1), RGE_Action_List__save
// (:61), the sprite id staging (the i16 @+130, 2). THE STAGING FORMS:
// the int stage for the 4-byte id writes, the short stage for the
// 2-byte writes (the asm's dword staging only escapes its LOW word -
// the written bytes identical).
void __fastcall RGE_Action__save_new(
    RGE_Action* this_, DWORD dummy, int outfile)
{
    int stage32 = 0;                                             // the dword staging (the asm's buffer)
    short stage16 = 0;                                           // the i16 staging (the 2-byte writes)
    rge_write(outfile, &this_->action_type, sizeof(this_->action_type));   // asm 0x40AB5A (@+4, 2)
    rge_write(outfile, &this_->state, sizeof(this_->state));     // asm 0x40AB6D (@+0xC, 1)
    stage32 = this_->target_obj ? this_->target_obj->id : -1;    // asm 0x40AB72-0x40AB85 (the resolved id @+0x10)
    rge_write(outfile, &stage32, sizeof(stage32));               // asm 0x40AB91 (4)
    stage32 = this_->target_obj2 ? this_->target_obj2->id : -1;  // asm 0x40AB96-0x40ABA6
    rge_write(outfile, &stage32, sizeof(stage32));               // asm 0x40ABB2 (4)
    rge_write(outfile, &this_->targetID, sizeof(this_->targetID));   // asm 0x40ABB9 (@+0x18, 4)
    rge_write(outfile, &this_->target2ID, sizeof(this_->target2ID));   // asm 0x40ABC5 (@+0x1C, 4)
    rge_write(outfile, &this_->target_x, sizeof(this_->target_x));   // asm 0x40ABD1 (@+0x20, 4)
    rge_write(outfile, &this_->target_y, sizeof(this_->target_y));   // asm 0x40ABDD (@+0x24, 4)
    rge_write(outfile, &this_->target_z, sizeof(this_->target_z));   // asm 0x40ABE9 (@+0x28, 4)
    rge_write(outfile, &this_->timer, sizeof(this_->timer));     // asm 0x40ABF5 (@+0x2C, 4)
    rge_write(outfile, &this_->mTargetMovedState, sizeof(this_->mTargetMovedState));   // asm 0x40AC01 (@+0x3D, 1)
    stage16 = this_->task ? this_->task->id : -1;                // asm 0x40AC0B-0x40AC18 (mov dx, [eax+2] - the i16 id @+2)
    rge_write(outfile, &stage16, sizeof(stage16));               // asm 0x40AC20 (2)
    rge_write(outfile, &this_->subActionValue, sizeof(this_->subActionValue));   // asm 0x40AC2D (@+0x3C, 1)
    RGE_Action_List__save(this_->sub_actions, outfile);          // asm (:61)
    stage16 = this_->sprite ? this_->sprite->id : -1;            // asm (the sprite id @+130, i16)
    rge_write(outfile, &stage16, sizeof(stage16));               // asm (2)
}

// Replacement for RGE_Action::type. Original: @ 0x0040AC70
// (idb_functions.h region). Range [0x40AC70, 0x40AC74) (mov ax,
// [ecx+4] / retn). ABI: __thiscall. Virtual: the Action vftable slot 6.
short __fastcall RGE_Action__type_new(RGE_Action* this_, DWORD dummy)
{
    return this_->action_type;                                   // asm 0x40AC70 (mov ax, [ecx+4])
}

// Replacement for RGE_Action::inside_obj_update. Original:
// @ 0x0040ACB0 (idb_functions.h region). Range [0x40ACB0, 0x40ACE2+),
// unsigned __int8 return (1). ABI: __thiscall. Virtual: the Action
// vftable slot 8 (+0x20). Decode (asm 0x40ACB0-0x40ACE1): the two
// target sweeps - target_obj @+0x10 non-null AND object_state @+0x58
// (walked) >= 7 (the unsigned byte cmp/jb) -> the set_target_obj
// NAMED dispatch (the base vtbl slot 22 = +0x58, line-arith verified);
// target_obj2 @+0x14 the same (slot 23 = +0x5C); return 1.
unsigned __int8 __fastcall RGE_Action__inside_obj_update_new(
    RGE_Action* this_, DWORD dummy)
{
    if (this_->target_obj && this_->target_obj->object_state >= 7)   // asm 0x40ACB3-0x40ACBE (cmp byte [eax+58h], 7/jb)
        this_->vfptr->set_target_obj(this_, 0);                  // asm 0x40ACC6 (slot 22 = +0x58; the explicit-this form)
    if (this_->target_obj2 && this_->target_obj2->object_state >= 7)   // asm 0x40ACC9-0x40ACD4 (@+0x14)
        this_->vfptr->set_target_obj2(this_, 0);                 // asm 0x40ACDC (slot 23 = +0x5C)
    return 1;                                                    // asm 0x40ACDF (mov al, 1)
}

// Replacement for RGE_Action::update. Original: @ 0x0040ACF0
// (idb_functions.h:66 is the LIST update; the base's own idb line
// verified at write-prep). Range [0x40ACF0, 0x40AD34+), unsigned
// __int8 return. ABI: __thiscall. Virtual: the Action vftable slot 10
// (+0x28). Decode (asm 0x40ACF0-0x40AD33): the two target sweeps (as
// inside_obj_update - the slots 22/23); then sub_actions @+0x34 ->
// list @+8 (the walked RGE_Action_List field) non-null ->
// RGE_Action_List::update (the List vftable slot 4 = +0x10, the
// explicit-this form; the List update's u8 return passes through) ->
// else THE RAW STATE BYTE (asm 0x40AD30 mov al, [esi+0Ch] - NO compare
// in the instruction stream: the 3-byte mov then pop esi. THE .c's
// `state == 1` RENDER IS ASM-CONTRADICTED - rule 6, the asm
// authoritative).
unsigned __int8 __fastcall RGE_Action__update_new(
    RGE_Action* this_, DWORD dummy)
{
    if (this_->target_obj && this_->target_obj->object_state >= 7)   // asm 0x40ACF3-0x40ACFE
        this_->vfptr->set_target_obj(this_, 0);                  // asm 0x40AD06 (slot 22 = +0x58)
    if (this_->target_obj2 && this_->target_obj2->object_state >= 7)   // asm 0x40AD09-0x40AD14 (@+0x14)
        this_->vfptr->set_target_obj2(this_, 0);                 // asm 0x40AD1C (slot 23 = +0x5C)
    if (this_->sub_actions->list)                                // asm 0x40AD22 (@+8 - the walked RGE_Action_List::list)
        return this_->sub_actions->vfptr->update(this_->sub_actions);   // asm 0x40AD2B (the List vftable slot 4 = +0x10)
    return this_->state;                                         // asm 0x40AD30 (mov al, [esi+0Ch] - the raw state byte; the .c's state == 1 render asm-contradicted)
}

// ============ RUNTIME METHODS (batch A2b) ============

// Replacement for RGE_Action::set_target_obj. Original: @ 0x0040AD40
// (idb_functions.h:182... the full set at write-prep). Range
// [0x40AD40, 0x40ADAF) (retn 4 = 1 stack arg), void. ABI: __thiscall.
// Virtual: the Action vftable slot 22 (+0x58). Callers: the target
// sweeps (inside_obj_update/update - the slots verified), the mission
// setters. Decode (asm 0x40AD40-0x40ADAE): the RELEASE gate - the old
// target non-null AND != new -> RGE_Game_World__object (obj @+0x8 ->
// owner @+0x18 -> world @+0x8C, the id @+0x18) -> non-null ->
// release_being_worked_on(resolved, obj) - the Static vtbl slot 80 =
// +0x140 (line-arith 4024-3944, eye-counted); the CHANGE gate -
// target_obj != new -> the store; new non-null -> targetID @+0x18 =
// new->id @+0x10 + set_being_worked_on(new, obj, action_type, 0) - the
// Static vtbl slot 79 = +0x13C (line-arith 4023-3944; the i16
// action_type @+4); null -> targetID = -1.
void __fastcall RGE_Action__set_target_obj_new(
    RGE_Action* this_, DWORD dummy, RGE_Static_Object* new_obj)
{
    if (this_->target_obj && this_->target_obj != new_obj)       // asm 0x40AD48-0x40AD51
    {
        RGE_Static_Object* resolved = RGE_Game_World__object((RGE_Game_World*)this_->obj->owner->world, this_->targetID);   // asm 0x40AD63 (the dual-naming cast - the update_new precedent)
        if (resolved)                                            // asm 0x40AD68
            resolved->vfptr->release_being_worked_on(resolved, (RGE_Static_Object*)this_->obj);   // asm 0x40AD74 (the Static vtbl slot 80 = +0x140; the explicit-this form)
    }
    if (this_->target_obj != new_obj)                            // asm 0x40AD7A-0x40AD7D
    {
        this_->target_obj = new_obj;                             // asm 0x40AD81
        if (new_obj)                                             // asm 0x40AD84
        {
            this_->targetID = new_obj->id;                       // asm 0x40AD86-0x40AD8D (id @+0x10 -> @+0x18)
            new_obj->vfptr->set_being_worked_on(new_obj, this_->obj, this_->action_type, 0);   // asm 0x40AD9B (the Static vtbl slot 79 = +0x13C; the i16 action_type @+4)
        }
        else
            this_->targetID = -1;                                // asm 0x40ADA6
    }
}

// Replacement for RGE_Action::set_target_obj2. Original: @ 0x0040ADC0
// (idb_functions.h region). Range [0x40ADC0, 0x40AE2F), void. ABI:
// __thiscall. Virtual: the Action vftable slot 23 (+0x5C). Callers:
// RGE_Action_Missile__RGE_Action_Missile+85, the vector deleting
// destructor (the null clear), the sweeps. THE set_target_obj TWIN
// (target_obj2 @+0x14, target2ID @+0x1C; the release id = target2ID).
void __fastcall RGE_Action__set_target_obj2_new(
    RGE_Action* this_, DWORD dummy, RGE_Static_Object* new_obj)
{
    if (this_->target_obj2 && this_->target_obj2 != new_obj)     // asm 0x40ADC8-region
    {
        RGE_Static_Object* resolved = RGE_Game_World__object((RGE_Game_World*)this_->obj->owner->world, this_->target2ID);
        if (resolved)
            resolved->vfptr->release_being_worked_on(resolved, (RGE_Static_Object*)this_->obj);   // the Static vtbl slot 80 = +0x140
    }
    if (this_->target_obj2 != new_obj)
    {
        this_->target_obj2 = new_obj;
        if (new_obj)
        {
            this_->target2ID = new_obj->id;
            new_obj->vfptr->set_being_worked_on(new_obj, this_->obj, this_->action_type, 0);   // the Static vtbl slot 79 = +0x13C
        }
        else
            this_->target2ID = -1;
    }
}

// Replacement for RGE_Action::copy_obj. Original: @ 0x0040AE40
// (idb_functions.h:184). Range [0x40AE40, 0x40AF20) (retn 4), void.
// ABI: __thiscall. Virtual: the Action vftable slot 17. Callers: the
// derived copy_objs (the ladder) + RGE_Action_Object. Decode (asm
// 0x40AE40-0x40AF1A): the task re-match - this->task @+0x30 non-null
// AND source->tasks @+0x104 (the Action master) non-null -> the
// BACKWARD scan of tasks->list (@+4) from list_num-1 (@+8, movsx i16)
// for the matching action_type (@+6); no match -> the bare return
// (asm 0x40AE88 - NOTHING copied); matched -> the sprite-match
// conditional (obj->sprite @+0x1C vs the task's move/work/work2/carry
// sprites @+52/56/60/64 -> new_sprite on the match, the obj new_sprite
// virtual slot 44) -> copy_obj_sprites x2 (the base vtbl slot 18 =
// +0x48 / the List vtbl slot 7 = +0x1C) -> task = the matched task.
// The null-task/null-tasks path: copy_obj_sprites x2 with (source, 0,
// 0). THE REDUNDANT-GUARD note: the asm 0x40AE8E test esi,esi/jl is
// dead (the scan exits esi < 0 via its own jge-fail return) - the IDA
// note in the .c agrees; not reproduced.
void __fastcall RGE_Action__copy_obj_new(
    RGE_Action* this_, DWORD dummy, RGE_Master_Action_Object* source)
{
    if (this_->task && source->tasks)                            // asm 0x40AE49-0x40AE5C (@+0x30 / @+0x104)
    {
        RGE_Task** list = source->tasks->list;                   // asm 0x40AE66 (@+4)
        int idx = source->tasks->list_num - 1;                   // asm 0x40AE62 (movsx word @+8, dec)
        if (idx < 0)                                             // asm 0x40AE6A (js)
            return;                                              // asm 0x40AE88-region (the bare return)
        __int16 want_type = this_->task->action_type;            // asm 0x40AE70 (@+6)
        while (list[idx]->action_type != want_type)              // asm 0x40AE78-0x40AE86 (the BACKWARD scan; list : RGE_Task** - the declared type, no cast needed)
        {
            if (--idx < 0)                                       // asm 0x40AE80-0x40AE84
                return;                                          // asm 0x40AE88 (no match - the bare return)
        }
        RGE_Task* matched = list[idx];                           // asm 0x40AE96
        RGE_Sprite* match = 0;
        if (this_->obj->sprite == matched->move_sprite)          // asm 0x40AE9E-0x40AEA6 (the task @+52)
            match = matched->move_sprite;
        else if (this_->obj->sprite == matched->work_sprite)     // asm (@+56)
            match = matched->work_sprite;
        else if (this_->obj->sprite == matched->work_sprite2)    // asm (@+60)
            match = matched->work_sprite2;
        else if (this_->obj->sprite == matched->carry_sprite)    // asm (@+64)
            match = matched->carry_sprite;
        if (match)                                               // asm 0x40AEBF (test ebx; the null matched-sprite skips)
            this_->obj->vfptr->new_sprite(this_->obj, match);    // asm 0x40AEC3+ (the obj new_sprite virtual - slot 44 = +0xB0)
        this_->vfptr->copy_obj_sprites(this_, source, this_->task, matched);   // asm (the base vtbl slot 18 = +0x48; the explicit-this form)
        this_->sub_actions->vfptr->copy_obj_sprites(this_->sub_actions, source, this_->task, matched);   // asm (the List vtbl slot 7 = +0x1C)
        this_->task = matched;                                   // asm (the task re-bind)
        return;
    }
    this_->vfptr->copy_obj_sprites(this_, source, 0, 0);         // asm loc_40AF01 (the null path)
    this_->sub_actions->vfptr->copy_obj_sprites(this_->sub_actions, source, 0, 0);   // asm
}

// Replacement for RGE_Action::get_state_name. Original: @ 0x0040AF20
// (idb_functions.h:185). Range [0x40AF20, 0x40B040), void. ABI:
// __thiscall. Virtual: the Action vftable slot 19. Decode: the
// 16-state string switch (the RGE_ACTION_STATE registry values 0-15 =
// None/Done/Wait/Search/Goto/Goto2/Work/Work2/Return/Turn/Delay/Move/
// Attack/Failed/Invalidated/MoveNoSearch; default Unknown) -> strcpy.
// THE strcpy: stateless (the caller-provided buffer, no CRT state) -
// the first use in the owned tree, noted.
void __fastcall RGE_Action__get_state_name_new(
    RGE_Action* this_, DWORD dummy, char* state_name)
{
    const char* name;
    switch (this_->state)                                        // asm (the byte @+0xC switch)
    {
    case RGE_ACTION_STATE_NONE:                                  // 0
        name = "None";
        break;
    case RGE_ACTION_STATE_DONE:                                  // 1
        name = "Done";
        break;
    case RGE_ACTION_STATE_WAIT:                                  // 2
        name = "Wait";
        break;
    case RGE_ACTION_STATE_SEARCH:                                // 3
        name = "Search";
        break;
    case RGE_ACTION_STATE_GOTO:                                  // 4
        name = "Goto";
        break;
    case RGE_ACTION_STATE_GOTO_2:                                // 5
        name = "Goto2";
        break;
    case RGE_ACTION_STATE_WORK:                                  // 6
        name = "Work";
        break;
    case RGE_ACTION_STATE_WORK_2:                                // 7
        name = "Work2";
        break;
    case RGE_ACTION_STATE_RETURN:                                // 8
        name = "Return";
        break;
    case RGE_ACTION_STATE_TURN:                                  // 9
        name = "Turn";
        break;
    case RGE_ACTION_STATE_DELAY:                                 // 10
        name = "Delay";
        break;
    case RGE_ACTION_STATE_MOVE:                                  // 11
        name = "Move";
        break;
    case RGE_ACTION_STATE_ATTACK:                                // 12
        name = "Attack";
        break;
    case RGE_ACTION_STATE_FAILED:                                // 13
        name = "Failed";
        break;
    case RGE_ACTION_STATE_INVALIDATED:                           // 14
        name = "Invalidated";
        break;
    case RGE_ACTION_STATE_MOVE_NO_SEARCH:                        // 15
        name = "MoveNoSearch";
        break;
    default:
        name = "Unknown";
        break;
    }
    strcpy(state_name, name);
}

// Replacement for RGE_Action::targetX. Original: @ 0x0040B040
// (idb_functions.h:186). Range [0x40B040, 0x40B050), the double return
// (the st0 exact-widening - the fld m32/retn 4, rule (h)). ABI:
// __thiscall. NOT virtual. targetY @0x0040B050 (:187) and targetZ
// @0x0040B060 (:188) - the same shape.
double __fastcall RGE_Action__targetX_new(RGE_Action* this_, DWORD dummy)
{
    return this_->target_x;                                      // asm (fld dword [this+0x20])
}

double __fastcall RGE_Action__targetY_new(RGE_Action* this_, DWORD dummy)
{
    return this_->target_y;                                      // asm (fld dword [this+0x24])
}

double __fastcall RGE_Action__targetZ_new(RGE_Action* this_, DWORD dummy)
{
    return this_->target_z;                                      // asm (fld dword [this+0x28])
}

// Replacement for RGE_Action::setSubAction. Original: @ 0x0040B070
// (idb_functions.h:189). Range [0x40B070, 0x40B080), void. ABI:
// __thiscall. NOT virtual.
void __fastcall RGE_Action__setSubAction_new(
    RGE_Action* this_, DWORD dummy, unsigned __int8 v)
{
    this_->subActionValue = v;                                   // asm (@+0x3C)
}

// Replacement for RGE_Action::hasSubActionOfType. Original:
// @ 0x0040B080 (idb_functions.h:190). Range [0x40B080, 0x40B0A0), int
// return. ABI: __thiscall. NOT virtual. Decode: sub_actions non-null ->
// RGE_Action_List__have_action2(sub_actions, type) (:73); else 0.
int __fastcall RGE_Action__hasSubActionOfType_new(
    RGE_Action* this_, DWORD dummy, int type)
{
    if (this_->sub_actions)                                      // asm (@+0x34)
        return RGE_Action_List__have_action2(this_->sub_actions, type);   // asm (:73)
    return 0;
}

// Replacement for RGE_Action::add_sub_action. Original: @ 0x0040B0A0
// (idb_functions.h:191). Range [0x40B0A0, 0x40B0D0), void. ABI:
// __thiscall. NOT virtual. Decode: deleteList -> RGE_Action_List__
// delete_list(sub_actions) (:62); RGE_Action_List__add_action
// (sub_actions, action) (:67); action->subActionValue = 1.
void __fastcall RGE_Action__add_sub_action_new(
    RGE_Action* this_, DWORD dummy, RGE_Action* action, unsigned __int8 deleteList)
{
    if (deleteList)                                              // asm (the caller-flag)
        RGE_Action_List__delete_list(this_->sub_actions);        // asm (:62)
    RGE_Action_List__add_action(this_->sub_actions, action);     // asm (:67)
    action->subActionValue = 1;                                  // asm (the sub-action marker)
}

// Replacement for RGE_Action::get_target_obj. Original: @ 0x0040B0D0
// (idb_functions.h:192). Range [0x40B0D0, 0x40B0F0), RGE_Static_
// Object* return. ABI: __thiscall. NOT virtual. Decode: targetID < 0
// -> 0; else RGE_Game_World__object (obj @+0x8 -> owner @+0x18 ->
// world @+0x8C, the id @+0x18) (:6086).
RGE_Static_Object* __fastcall RGE_Action__get_target_obj_new(
    RGE_Action* this_, DWORD dummy)
{
    if (this_->targetID < 0)                                     // asm
        return 0;
    return RGE_Game_World__object((RGE_Game_World*)this_->obj->owner->world, this_->targetID);   // asm (:6086; the dual-naming cast)
}

// Replacement for RGE_Action::get_target_obj2. Original: @ 0x0040B0F0
// (idb_functions.h:193). Range [0x40B0F0, 0x40B110). The target_obj
// twin with target2ID @+0x1C.
RGE_Static_Object* __fastcall RGE_Action__get_target_obj2_new(
    RGE_Action* this_, DWORD dummy)
{
    if (this_->target2ID < 0)
        return 0;
    return RGE_Game_World__object((RGE_Game_World*)this_->obj->owner->world, this_->target2ID);
}

// ============ RUNTIME METHODS (batch A2c - the outlier) ============

// Replacement for RGE_Action::set_state. Original: @ 0x005724C0
// (idb_functions.h:4262 - the FAR outlier from the base cluster; the
// linker placed it in the 0x5724xx region next to the Make_Tech code).
// Range [0x5724C0, 0x5724C7) (lst region; retn 4 = 1 stack arg), void.
// ABI: __thiscall. Virtual: the Action vftable slot 24. Decode (asm
// 0x5724C0-0x5724C6): state @+0xC = new_state (mov al, [esp+4] /
// mov [ecx+0Ch], al).
void __fastcall RGE_Action__set_state_new(
    RGE_Action* this_, DWORD dummy, char new_state)
{
    this_->state = new_state;                                    // asm 0x5724C4 (mov [ecx+0Ch], al)
}

void install_RGE_Action()
{
    setHook((void*)0x0040A7F0, RGE_Action__RGE_Action2_new);
    setHook((void*)0x0040A830, RGE_Action__vector_deleting_destructor_new);
    setHook((void*)0x0040A880, RGE_Action__RGE_Action_new);
    setHook((void*)0x0040A8E0, RGE_Action__destructor_new);
    setHook((void*)0x0040A910, RGE_Action__setup2_new);
    setHook((void*)0x0040AA40, RGE_Action__setup_new);
    setHook((void*)0x0040AA90, RGE_Action__create_action_list_new);
    setHook((void*)0x0040AB00, RGE_Action__rehook_new);
    setHook((void*)0x0040AB50, RGE_Action__save_new);
    setHook((void*)0x0040AC70, RGE_Action__type_new);
    setHook((void*)0x0040ACB0, RGE_Action__inside_obj_update_new);
    setHook((void*)0x0040ACF0, RGE_Action__update_new);
    setHook((void*)0x0040AD40, RGE_Action__set_target_obj_new);
    setHook((void*)0x0040ADC0, RGE_Action__set_target_obj2_new);
    setHook((void*)0x0040AE40, RGE_Action__copy_obj_new);
    setHook((void*)0x0040AF20, RGE_Action__get_state_name_new);
    setHook((void*)0x0040B040, RGE_Action__targetX_new);
    setHook((void*)0x0040B050, RGE_Action__targetY_new);
    setHook((void*)0x0040B060, RGE_Action__targetZ_new);
    setHook((void*)0x0040B070, RGE_Action__setSubAction_new);
    setHook((void*)0x0040B080, RGE_Action__hasSubActionOfType_new);
    setHook((void*)0x0040B0A0, RGE_Action__add_sub_action_new);
    setHook((void*)0x0040B0D0, RGE_Action__get_target_obj_new);
    setHook((void*)0x0040B0F0, RGE_Action__get_target_obj2_new);
    setHook((void*)0x005724C0, RGE_Action__set_state_new);
}
