#include "stdafx.h"
#include "..\object\RGE_Action_Types.h"
#include "..\object\RGE_Master_Static_Object.h"

// the RGE_Action_Explore vftable (.rdata:00654820, lst :986199; slot 0 =
// the SHARED family scalar deleting destructor; the tier owns no
// destructor of its own)
static RGE_Action_ExploreVtbl* const rge_action_explore_vftable =
    (RGE_Action_ExploreVtbl*)0x00654820;

// Replacement for RGE_Action_Explore::RGE_Action_Explore2 (the read
// ctor). Original: @ 0x00403D30 (idb_functions.h:34). Range
// [0x403D30, 0x403D55) (lst :5275/:5295, retn 8 = 2 stack args),
// RGE_Action_Explore* return. ABI: __thiscall. NOT virtual. Callers:
// RGE_Action_List__create_action2 +DF (the load path). NO SEH (plain).
// Decode (asm 0x403D30-0x403D54): RGE_Action::RGE_Action2 (the idb
// constant HOOKED -> routes), the vfptr store (.rdata:00654820),
// action_type = 4 (the EXPLORE registry value); return this.
RGE_Action_Explore* __fastcall RGE_Action_Explore__RGE_Action_Explore2_new(
    RGE_Action_Explore* this_, DWORD dummy, int infile,
    RGE_Action_Object* obj_in)
{
    RGE_Action__RGE_Action2((RGE_Action*)this_, infile, obj_in, 1);   // asm 0x403D41 (:170 - the idb constant, HOOKED -> routes; do_setup = 1; the derived->base cast)
    this_->vfptr = rge_action_explore_vftable;                        // asm 0x403D46
    this_->action_type = RGE_ACTION_TYPE_EXPLORE;                     // asm 0x403D4C (mov word @+4, 4)
    return this_;                                                     // asm 0x403D52 (mov eax, esi)
}

// Replacement for RGE_Action_Explore::RGE_Action_Explore (the ctor).
// Original: @ 0x00403D60 (idb_functions.h:35). Range [0x403D60, 0x403D9A)
// (lst :5304/:5333, retn 14h = 5 stack args), RGE_Action_Explore*
// return. ABI: __thiscall. NOT virtual. Callers: RGE_Action_List__
// create_task_action +1D3 (the OWNED anchor - routes) +
// RGE_Combat_Object__explore +D0 (the OWNED explore_new - routes).
// NO SEH (plain). Decode (asm 0x403D60-0x403D99): RGE_Action::RGE_Action
// (:172 - HOOKED -> routes); task @+0x30, target_y @+0x24, the vfptr,
// action_type, target_x @+0x20, target_z @+0x28 (the compiler's
// out-of-source-order schedule reconciled); return this.
RGE_Action_Explore* __fastcall RGE_Action_Explore__RGE_Action_Explore_new(
    RGE_Action_Explore* this_, DWORD dummy, RGE_Action_Object* obj_in,
    RGE_Task* task_in, float x, float y, float z)
{
    RGE_Action__RGE_Action((RGE_Action*)this_, obj_in, 1);        // asm 0x403D6A (:172 - the idb constant, HOOKED -> routes; do_setup = 1; the derived->base cast)
    this_->task = task_in;                                        // asm 0x403D7B ([esi+30h])
    this_->target_y = y;                                          // asm 0x403D82 ([esi+24h])
    this_->vfptr = rge_action_explore_vftable;                    // asm 0x403D85
    this_->action_type = RGE_ACTION_TYPE_EXPLORE;                 // asm 0x403D8B (mov word @+4, 4)
    this_->target_x = x;                                          // asm 0x403D91 ([esi+20h])
    this_->target_z = z;                                          // asm 0x403D94 ([esi+28h])
    return this_;                                                 // asm 0x403D97 (mov eax, esi)
}

// Replacement for RGE_Action_Explore::first_in_stack. Original:
// @ 0x00403DA0. Range [0x403DA0, 0x403DBA) (lst :5342/:5360, retn 4 = 1
// stack arg), void. ABI: __thiscall. Virtual: the Explore vftable (the
// DATA XREF lst :5343). NO SEH. THE first_time arg UNCONSUMED (zero
// reads - kept for ABI). THE SENTINEL: the raw dword compare
// (cmp [ecx+20h], -1.0 = 0xBF800000) vs the -1.0f float sentinel
// (set_state(2 Wait) writes the trio = -1.0f = the "no destination"
// marker): the float == -1.0f compare is VALUE-IDENTICAL to the bits
// compare universally (the exact sentinel - one bit pattern; NaN's bits
// differ and the float == reads unordered-false = the same not-equal
// result). == -1.0f -> set_state(3 Search); else -> set_state(11 Move).
void __fastcall RGE_Action_Explore__first_in_stack_new(
    RGE_Action_Explore* this_, DWORD dummy, unsigned __int8 first_time)
{
    if (this_->target_x == -1.0f)                                 // asm 0x403DA7-region (cmp dword [ecx+20h], 0BF800000h - the raw-bits sentinel; the float compare = value-identical)
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_SEARCH);  // asm 0x403DAB (3 = Search)
    else
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_MOVE);    // asm 0x403DB7 (11 = Move)
}

// Replacement for RGE_Action_Explore::set_state. Original: @ 0x00403DC0.
// Range [0x403DC0, 0x403F36) (lst :5369/:5540, retn 4 = 1 stack arg),
// void. ABI: __thiscall. Virtual: the Explore vftable (the DATA XREF
// lst :5369). THE SEH: the allocation-unwind fold (the chunk @0x640790,
// the Move_To creation's operator_delete - the standard class). THE
// SWITCH-TABLE BYTE CHECK: byte_403F50 = db 0,1,2,4, 4,4,4,4, 4,4,3,4,
// 0,0 (state-1..14): Done(1)/Failed(13)/Invalidated(14) -> loc_403E20
// (the base-sprite restore); Wait(2) -> loc_403E4B (the restore + the
// target clears + the -1.0f sentinel reset); Search(3) -> loc_403E02
// (the 514/605 notify -> the restore); Move(11) -> loc_403E96 (THE
// MOVE_TO CREATION); the rest -> default. Decode (asm 0x403DC0-0x403F35):
// delete_list(sub_actions) FIRST (:62); state = new_state; Search(3):
// notify(obj, id, id, 514, RGE_UNITAI_ACTION_EXPLORE, 0, 0) (slot 120
// = +0x1E0; the 5xx candidate - the expedition-started notify) -> the
// restore; Move(11): sprite = task && task->move_sprite @+0x34 ?
// move_sprite : master get_move_sprite (slot 30 = +0x78 - the M4
// lowest-declaring-tier cast); operator_new(0x44) = sizeof(RGE_Action_
// Move_To); the ctor (:100 - HOOKED -> routes) with (obj, x, y, z,
// 0.0, sprite); null -> set_state(13 Failed); else setSubAction(1)
// (:190) THEN add_action (:67) - THE ORDER (and NO mTargetMovedState
// write - the Enter set_state_move contrast). THE GOTO-FREE JOIN: the
// four labels share the arm - the terminal states' table entries point
// DIRECTLY at loc_403E20 (the restore); Search's entry points at
// loc_403E02 (the notify) which FALLS INTO the restore - the written
// compare reproduces the asm's two paths with ONE restore emission.
void __fastcall RGE_Action_Explore__set_state_new(
    RGE_Action_Explore* this_, DWORD dummy, unsigned __int8 new_state)
{
    RGE_Action_List__delete_list(this_->sub_actions);             // asm 0x403DDC (:62 - FIRST, before the state store)
    this_->state = new_state;                                     // asm 0x403DE5 ([esi+0Ch])
    switch (new_state)                                            // asm 0x403DE8-0x403DFB (byte_403F50/jpt_403DFB - THE EMITTED TABLE MUST EQUAL db 0,1,2,4,4,4,4,4,4,4,3,4,0,0)
    {
    case RGE_ACTION_STATE_DONE:                                   // 1 -> loc_403E20 (the join)
    case RGE_ACTION_STATE_FAILED:                                 // 13 -> loc_403E20
    case RGE_ACTION_STATE_INVALIDATED:                            // 14 -> loc_403E20
    case RGE_ACTION_STATE_SEARCH:                                 // 3 -> loc_403E02 (the notify) FALLING INTO loc_403E20 (the asm's own fall-through join)
        if (new_state == RGE_ACTION_STATE_SEARCH)                 // the asm's two paths: the terminal states JUMP past the notify (the table entry 0 -> loc_403E20 directly); Search falls through it (loc_403E02 -> loc_403E20) - this compare is the goto-free join's structural cost (the /Fa positional-diff class)
            this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id, 514, RGE_UNITAI_ACTION_EXPLORE, 0, 0);   // asm 0x403E0E-0x403E1A (0x202 = 514, 0x25D = 605 - the expedition-started notify, the 5xx candidate)
        {                                                         // loc_403E20 (the shared restore - ONE emission site, the asm's 2-inbound join)
            RGE_Sprite* base_sprite = this_->obj->master_obj->vfptr->get_sprite(this_->obj->master_obj, (RGE_Static_Object_idb*)this_->obj);   // asm 0x403E2B (slot 1)
            this_->obj->vfptr->new_sprite(this_->obj, base_sprite);   // asm 0x403E32 (slot 44)
        }
        return;                                                   // asm def_403DFB
    case RGE_ACTION_STATE_WAIT:                                   // 2 -> loc_403E4B
    {
        RGE_Sprite* base_sprite = this_->obj->master_obj->vfptr->get_sprite(this_->obj->master_obj, (RGE_Static_Object_idb*)this_->obj);   // asm 0x403E56 (slot 1)
        this_->obj->vfptr->new_sprite(this_->obj, base_sprite);   // asm 0x403E5D (slot 44 = +0xB0)
        this_->vfptr->set_target_obj(this_, 0);                   // asm 0x403E69 (slot 22 = +0x58)
        this_->vfptr->set_target_obj2(this_, 0);                  // asm 0x403E72 (slot 23 = +0x5C)
        this_->target_x = -1.0f;                                  // asm 0x403E75 (mov eax, -1.0 = 0BF800000h)
        this_->target_y = -1.0f;                                  // asm 0x403E7D
        this_->target_z = -1.0f;                                  // asm 0x403E80
        return;                                                   // asm 0x403E93
    }
    case RGE_ACTION_STATE_MOVE:                                   // 11 -> loc_403E96
    {
        RGE_Sprite* move_sprite;
        if (this_->task && this_->task->move_sprite)              // asm 0x403E99-0x403EA2 (test eax; test [eax+34h])
            move_sprite = this_->task->move_sprite;               // asm 0x403E9D
        else
            move_sprite = ((RGE_Master_Moving_Object*)this_->obj->master_obj)->vfptr->get_move_sprite((RGE_Master_Moving_Object*)this_->obj->master_obj, (RGE_Static_Object_idb*)this_->obj);   // asm 0x403EAD (slot 30 = +0x78 = 120 decimal; the M4 lowest-declaring-tier cast)
        RGE_Action_Move_To* created = (RGE_Action_Move_To*)operator_new_internal(sizeof(RGE_Action_Move_To));   // asm 0x403EB4 (push 44h = 68 walked)
        __try
        {
            if (created)                                          // asm 0x403EC0
                RGE_Action_Move_To__RGE_Action_Move_To(created, this_->obj, this_->target_x, this_->target_y, this_->target_z, 0.0f, move_sprite);   // asm 0x403EE1 (:100 - the idb constant, HOOKED -> routes; range = 0.0)
        }
        __except (operator_delete_internal(created), EXCEPTION_CONTINUE_SEARCH)   // the allocation-unwind fold: the chunk @0x640790
        {
        }
        if (!created)                                             // asm 0x403EEC-0x403EEE
        {
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);   // asm 0x403EFE (13 = Failed)
            return;                                               // asm 0x403F11
        }
        RGE_Action__setSubAction((RGE_Action*)created, 1);        // asm 0x403F18 (:190 - BEFORE the add_action, the asm order)
        RGE_Action_List__add_action(this_->sub_actions, (RGE_Action*)created);   // asm 0x403F21 (:67)
        return;                                                   // asm 0x403F36
    }
    default:
        return;                                                   // asm def_403DFB
    }
}

// Replacement for RGE_Action_Explore::update. Original: @ 0x00403F60.
// Range [0x403F60, 0x404001) (lst :5560/:5653, retn = no stack args),
// unsigned __int8 return. ABI: __thiscall. Virtual: the Explore vftable
// (DATA XREF lst :5560). NO SEH. THE ARM FORM: the asm dec/sub CHAIN
// (dec -> Done(1); sub 0Ah -> Move(11); sub 2 -> Failed(13); else) - NO
// jump table (nothing to diff). Decode (asm 0x403F60-0x404000): Done ->
// return 1; Move(11): the SUB-ACTION PARTITION (the list vtbl update
// slot 4 = +0x10; the 5-case table off_404004 = {Search, default,
// Search, the target-clear, Search} for sub 1..5): sub Done(1)/Search
// (3)/Goto_Completed(5) -> set_state(3 Search); sub Goto(4) ->
// set_target_obj(0) + set_state(11 Move); the rest -> return 0;
// Failed(13): THE FAILED NOTIFY - notify(obj, id, id, 505,
// currentAction(unitAI(obj)), 0, 0) - the p1 = the module's CURRENT
// action code (the runtime value, UnitAIModule__currentAction :5586
// @0x5F4D70) - CALLED UNCONDITIONALLY ON A POSSIBLY-NULL unitAI (the
// original's own shape - NO invented null check) -> set_state(2 Wait)
// -> return 3; default -> return 0.
unsigned __int8 __fastcall RGE_Action_Explore__update_new(
    RGE_Action_Explore* this_, DWORD dummy)
{
    switch (this_->state)                                         // asm 0x403F64-0x403F78 (the dec/sub chain - no table)
    {
    case RGE_ACTION_STATE_DONE:                                   // 1 -> loc_403FFD
        return 1;                                                 // asm 0x403FFD (mov al, 1)
    case RGE_ACTION_STATE_MOVE:                                   // 11 -> loc_403FBE
    {
        unsigned __int8 sub = this_->sub_actions->vfptr->update(this_->sub_actions);   // asm 0x403FC3 (the list vtbl slot 4 = +0x10; and eax,0FFh - the declared u8 return, the mask folds)
        switch (sub)                                              // asm 0x403FCB-0x403FD1 (off_404004 - THE EMITTED PARTITION MUST EQUAL {Search, default, Search, the clear, Search})
        {
        case RGE_ACTION_STATE_GOTO:                               // sub 4 -> loc_403FD8
            this_->vfptr->set_target_obj(this_, 0);               // asm 0x403FDE (slot 22)
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_MOVE);// asm 0x403FE7 (11 = Move)
            return 0;                                             // asm 0x403FEA
        case RGE_ACTION_STATE_DONE:                               // sub 1 -> loc_403FEF
        case RGE_ACTION_STATE_SEARCH:                             // sub 3
        case 5:                                                   // sub 5 -> loc_403FEF (the sub-action's Goto_Completed state - BEYOND the RGE_ACTION_STATE registry's named set; the raw 5)
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_SEARCH);   // asm 0x403FF5 (3 = Search)
            return 0;                                             // asm -> loc_403FF8
        default:
            return 0;                                             // asm loc_403FF8
        }
    }
    case RGE_ACTION_STATE_FAILED:                                 // 13 -> loc_403F7A
    {
        RGE_Action_Object* obj = this_->obj;
        UnitAIModule* ai = RGE_Static_Object__unitAI((RGE_Static_Object*)obj);   // asm 0x403F91 (:261 - the derived->ancestor cast)
        int current = UnitAIModule__currentAction(ai);            // asm 0x403F98 (:5586 @0x5F4D70 - CALLED UNCONDITIONALLY, the possibly-null unitAI = the original's own shape - NO invented null check)
        obj->vfptr->notify(obj, obj->id, obj->id, 505, current, 0, 0);   // asm 0x403FA7 (slot 120 = +0x1E0; 0x1F9 = 505; p1 = the current action code)
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_WAIT);    // asm 0x403FB3 (2 = Wait)
        return 3;                                                 // asm 0x403FB9 (mov al, 3)
    }
    default:
        return 0;                                                 // asm loc_403FF8
    }
}

// Replacement for RGE_Action_Explore::work. Original: @ 0x00404020
// (idb_functions.h:39). Range [0x404020, 0x404072) (lst :5668/:5705,
// retn 10h = 4 stack args), int return. ABI: __thiscall. Virtual: the
// Explore vftable (the DATA XREF lst :5668). NO SEH. THE obj_in arg
// UNCONSUMED (zero reads - kept for ABI). Decode (asm 0x404020-0x404071):
// the coordinate gates: X87_LT_NAN(x, 0.0f) (test ah,1/jnz - the
// {<,NaN} set -> the Search path - THE MACRO REQUIRED: a NaN x takes
// the Search in the original; the literal < would take the move =
// DIVERGENT) or X87_LT_NAN(y, 0.0f) -> set_state(3 Search) -> return 1;
// else target_x/y/z @+0x20/24/28 = the args + set_state(11 Move) ->
// return 1. THE .c's __stdcall cast-form on the set_state = the
// artifact (the standard 1-stack-arg thiscall).
int __fastcall RGE_Action_Explore__work_new(
    RGE_Action_Explore* this_, DWORD dummy, RGE_Static_Object* obj_in,
    float x, float y, float z)
{
    if (X87_LT_NAN(x, 0.0f) || X87_LT_NAN(y, 0.0f))               // asm 0x40402C-0x404040 (test ah,1/jnz - the {<,NaN} set -> the Search path; the macros required)
    {
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_SEARCH);  // asm 0x40406A (3 = Search)
        return 1;                                                 // asm 0x40406D (mov eax, 1)
    }
    this_->target_x = x;                                          // asm 0x40404A ([ecx+20h])
    this_->target_y = y;                                          // asm 0x40404D ([ecx+24h])
    this_->target_z = z;                                          // asm 0x404058 ([ecx+28h])
    this_->vfptr->set_state(this_, RGE_ACTION_STATE_MOVE);        // asm 0x40405B (11 = Move)
    return 1;                                                     // asm 0x40405E (mov eax, 1)
}

void install_RGE_Action_Explore()
{
    setHook((void*)0x00403D30, RGE_Action_Explore__RGE_Action_Explore2_new);
    setHook((void*)0x00403D60, RGE_Action_Explore__RGE_Action_Explore_new);
    setHook((void*)0x00403DA0, RGE_Action_Explore__first_in_stack_new);
    setHook((void*)0x00403DC0, RGE_Action_Explore__set_state_new);
    setHook((void*)0x00403F60, RGE_Action_Explore__update_new);
    setHook((void*)0x00404020, RGE_Action_Explore__work_new);
}
