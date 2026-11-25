#include "stdafx.h"
#include "..\object\RGE_Action_Types.h"
#include "..\object\RGE_Master_Static_Object.h"

// the RGE_Action_Guard vftable (.rdata:006549FC, lst :986263; slot 0 =
// the SHARED family scalar deleting destructor - the same
// TRIBE_Action_Artifact__scalar_deleting_destructor the Bird/Explore
// vftables slot; the tier owns no destructor of its own and inherits
// the base RGE_Action__update)
static RGE_Action_GuardVtbl* const rge_action_guard_vftable =
    (RGE_Action_GuardVtbl*)0x006549FC;

// Replacement for RGE_Action_Guard::RGE_Action_Guard2 (the read ctor).
// Original: @ 0x00406180 (idb_functions.h:52). Range [0x406180, 0x4061A5)
// (lst :9271/:9291, retn 8 = 2 stack args), RGE_Action_Guard* return.
// ABI: __thiscall. NOT virtual. Callers: RGE_Action_List__create_action2
// +257 (the load path). NO SEH (plain - the only SEH-free ctor family
// member so far). Decode (asm 0x406180-0x4061A4): RGE_Action::RGE_
// Action2(this, infile, obj_in, 1) - the BASE read ctor (the idb
// constant HOOKED -> routes), the vfptr store (the Guard static
// vftable), action_type = 13 (the GUARD registry value); return this.
RGE_Action_Guard* __fastcall RGE_Action_Guard__RGE_Action_Guard2_new(
    RGE_Action_Guard* this_, DWORD dummy, int infile,
    RGE_Action_Object* obj_in)
{
    RGE_Action__RGE_Action2((RGE_Action*)this_, infile, obj_in, 1);   // asm 0x406191 (:170 - the idb constant, HOOKED -> routes; do_setup = 1; the derived->base cast)
    this_->vfptr = rge_action_guard_vftable;                          // asm 0x406196 (the Guard static vftable)
    this_->action_type = RGE_ACTION_TYPE_GUARD;                       // asm 0x40619C (mov word @+4, 0Dh)
    return this_;                                                     // asm 0x4061A2 (mov eax, esi)
}

// Replacement for RGE_Action_Guard::RGE_Action_Guard (the ctor).
// Original: @ 0x004061D0 (idb_functions.h:54). Range [0x4061D0, 0x40622A)
// (lst :9328/:9367, retn 8 = 2 stack args), RGE_Action_Guard* return.
// ABI: __thiscall. NOT virtual. Caller: RGE_Action_List__create_task_
// action +2A7 (the OWNED anchor - routes). The SEH: the trivial unwind
// (the chunk @0x640830). Decode (asm 0x4061D0-0x406229): RGE_Action::
// RGE_Action(this, obj_in, 1) (:172 - HOOKED -> routes); the vfptr +
// action_type stores staged across the set_target_obj call (the
// compiler's schedule - value-order irrelevant); set_target_obj
// (target_obj_in) (:182 - HOOKED -> routes); return this.
RGE_Action_Guard* __fastcall RGE_Action_Guard__RGE_Action_Guard_new(
    RGE_Action_Guard* this_, DWORD dummy, RGE_Action_Object* obj_in,
    RGE_Static_Object* target_obj_in)
{
    RGE_Action__RGE_Action((RGE_Action*)this_, obj_in, 1);        // asm 0x4061F4 (:172 - the idb constant, HOOKED -> routes; do_setup = 1; the derived->base cast)
    this_->vfptr = rge_action_guard_vftable;                      // asm 0x406208
    this_->action_type = RGE_ACTION_TYPE_GUARD;                   // asm 0x40620E (mov word @+4, 0Dh)
    RGE_Action__set_target_obj((RGE_Action*)this_, target_obj_in);   // asm 0x406214 (:182 - HOOKED -> routes; the derived->base cast)
    return this_;                                                 // asm 0x40621D (mov eax, esi)
}

// Replacement for RGE_Action_Guard::first_in_stack. Original:
// @ 0x00406230 (idb_functions.h:55). Range [0x406230, 0x4062A3)
// (lst :9376/:9430, retn 4 = 1 stack arg), void. ABI: __thiscall.
// Virtual: the Guard vftable (the DATA XREF lst :9377). NO SEH. Decode
// (asm 0x406230-0x4062A2): THE DEFEND-ORDER ISSUE: first_time (the arg
// IS consumed here) && unitAI(obj) (:261) && target_obj - all three
// gates -> THE UNITAI ORDER DISPATCH: unitAI->vfptr->order (slot 4 =
// +0x10, idb_types.h:7589 - the member declares ALL 11 params
// arg-for-arg the asm pushes: caller_id, order_code, target_id,
// target_owner_id, x, y, z, range, p9, p10, p11): (obj->owner->id
// @+0xA0, 701 = RGE_UNITAI_ORDER_DEFEND (DEFEND 601 + 100 - the derived
// registry), target->id @+0x10, target->owner->id, target world_x/y/z
// @+0x48/4C/50, 2.0f, 0, 0, 100). set_state(1 Done) UNCONDITIONALLY on
// every path (slot 24 = +0x60).
void __fastcall RGE_Action_Guard__first_in_stack_new(
    RGE_Action_Guard* this_, DWORD dummy, unsigned __int8 first_time)
{
    if (first_time                                                // asm 0x406235 (the arg IS consumed)
        && RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj)   // asm 0x40623E (:261; the derived->ancestor cast)
        && this_->target_obj)                                     // asm 0x40624C
    {
        UnitAIModule* ai = RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj);   // asm 0x406252 (the second call - the asm's own shape)
        ai->vfptr->order(ai,                                      // asm 0x406295 (slot 4 = +0x10; idb_types.h:7589 - the 11-param member)
            this_->obj->owner->id,                                // asm 0x406292 ([edx+0A0h])
            RGE_UNITAI_ORDER_DEFEND,                              // asm 0x406284 (push 2BDh = 701 - DEFEND 601 + 100, the derived registry)
            this_->target_obj->id,                                // asm 0x406283 ([edx+10h])
            this_->target_obj->owner->id,                         // asm 0x40627F (target->owner @+0x18 -> id @+0xA0)
            this_->target_obj->world_x,                           // asm 0x406270 ([edx+48h])
            this_->target_obj->world_y,                           // asm 0x40626C ([edx+4Ch])
            this_->target_obj->world_z,                           // asm 0x406268 ([edx+50h])
            2.0f,                                                 // asm 0x406263 (push 2.0 = 40000000h - the guard-order range parameter, the semantics UNKNOWN - the registry candidate)
            0, 0, 100);                                           // asm 0x40625C/0x40625E/0x40625A (push 0, 0, 64h)
    }
    this_->vfptr->set_state(this_, RGE_ACTION_STATE_DONE);        // asm 0x40629F (slot 24 = +0x60; 1 = Done - UNCONDITIONAL)
}

void install_RGE_Action_Guard()
{
    setHook((void*)0x00406180, RGE_Action_Guard__RGE_Action_Guard2_new);
    setHook((void*)0x004061D0, RGE_Action_Guard__RGE_Action_Guard_new);
    setHook((void*)0x00406230, RGE_Action_Guard__first_in_stack_new);
}
