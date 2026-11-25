#include "stdafx.h"
#include "..\object\RGE_Action_Types.h"
#include "..\object\RGE_Master_Static_Object.h"

extern int current_loaded_version;   // dataload.cpp:14 - the DAT/save version (the HARBOR_DROP_OFF bake gate in enter_target_new)

// the RGE_Action_Enter vftable (.rdata:00654738, lst :986170; slot 0 =
// the scalar deleting destructor)
static RGE_Action_EnterVtbl* const rge_action_enter_vftable =
    (RGE_Action_EnterVtbl*)0x00654738;

void __fastcall RGE_Action_Enter__destructor_new(RGE_Action_Enter*, DWORD);   // the forward declaration - the scalar deleting destructor's intra-class call

// Replacement for RGE_Action_Enter::RGE_Action_Enter2 (the read ctor).
// Original: @ 0x00402EE0 (idb_functions.h:24). Range [0x402EE0, 0x402F41)
// (lst :3662/:3700, retn 8 = 2 stack args), RGE_Action_Enter* return.
// ABI: __thiscall. NOT virtual. Callers: RGE_Action_List__create_action2
// +11F, TRIBE_Action_List__create_action+3EF (the load paths). The SEH:
// the trivial unwind (the chunk @0x640700 - no owned allocations).
// Decode (asm 0x402EE0-0x402F40): RGE_Action::RGE_Action2(this, infile,
// obj_in, 1) - the BASE read ctor with do_setup = 1 (the base setup2
// stream load runs; the idb constant HOOKED -> RGE_Action2_new routes),
// THEN the vfptr store (the Enter static vftable @0x00654738),
// action_type = 3 (word @+4 - the RGE_ACTION_TYPE_ENTER registry value),
// rge_read(mFirstTimeEnter @+0x40, 4) - the Enter extension read AFTER
// the base load; return this.
RGE_Action_Enter* __fastcall RGE_Action_Enter__RGE_Action_Enter2_new(
    RGE_Action_Enter* this_, DWORD dummy, int infile,
    RGE_Action_Object* obj_in)
{
    RGE_Action__RGE_Action2((RGE_Action*)this_, infile, obj_in, 1);   // asm 0x402F0A (:170 - the idb constant, HOOKED -> routes; do_setup = 1; the derived->base cast)
    this_->vfptr = rge_action_enter_vftable;                     // asm 0x402F1E (the Enter static vftable @0x00654738)
    this_->action_type = RGE_ACTION_TYPE_ENTER;                  // asm 0x402F24 (mov word @+4, 3 - the ENTER registry value)
    rge_read(infile, &this_->mFirstTimeEnter, sizeof(this_->mFirstTimeEnter));   // asm 0x402F2A (@+0x40, 4 - the Enter extension read AFTER the base load)
    return this_;                                                // asm 0x402F33 (mov eax, esi)
}

// Replacement for RGE_Action_Enter::scalar_deleting_destructor.
// Original: @ 0x00402F50 (idb_functions.h:25). Range [0x402F50, 0x402F6B)
// (lst :3712/:3730, retn 4 = 1 stack arg), void* return (this). ABI:
// __thiscall. Virtual: the Enter vftable slot 0 (DATA XREF lst :3716).
// Decode (asm 0x402F50-0x402F6A): destructor(this) - the DIRECT call to
// the Enter destructor (INTRA-CLASS: the entry is HOOKED -> routes into
// destructor_new); flags & 1 -> operator_delete(this) - the bare
// operator_delete maps to operator_delete_internal (the wrapper rule);
// return this.
void* __fastcall RGE_Action_Enter__scalar_deleting_destructor_new(
    RGE_Action_Enter* this_, DWORD dummy, unsigned int flags)
{
    RGE_Action_Enter__destructor_new(this_, dummy);              // asm 0x402F53 (the DIRECT intra-class call to the sibling replacement - the entry HOOKED -> routes)
    if (flags & 1)                                               // asm 0x402F58
        operator_delete_internal(this_);                         // asm 0x402F60 (the bare operator_delete -> the wrapper rule)
    return this_;                                                // asm 0x402F68 (mov eax, esi)
}

// Replacement for RGE_Action_Enter::RGE_Action_Enter (the ctor).
// Original: @ 0x00402F70 (idb_functions.h:26). Range [0x402F70, 0x402FFD)
// (lst :3732/:3763, retn 10h = 3 stack args), RGE_Action_Enter* return.
// ABI: __thiscall. NOT virtual. Callers: RGE_Combat_Object__enter+17F
// (the owned enter_new's ctor call - HOOKED -> routes) +
// TRIBE_Action_List__create_task_action+600 (the anchor's case 13 -
// HOOKED -> routes). The SEH: the trivial unwind (@0x640720).
// Decode (asm 0x402F70-0x402FFD): RGE_Action::RGE_Action(this, obj_in,
// 1) - the BASE ctor with do_setup = 1 (the idb constant HOOKED ->
// routes; the base create_action_list dispatch runs the DERIVED
// override); THEN mFirstTimeEnter = 1 (dword @+0x40 - BEFORE the vfptr
// store, the asm order), the vfptr store (the Enter static vftable),
// action_type = 3 (the ENTER registry value), task = task_in (@+0x30);
// THE LINKED-BUILDING REDIRECT: target_obj_in non-null AND
// master_obj->master_type @+4 == 80 AND ((TRIBE_Building_Object*)
// target)->linked_owner @+0x1DC != 0 (the walker-verified field - the
// desolid_flag@565 + sizeof-632 calibrations) -> new_obj =
// linked_owner (the gate/compiled-building consolidation); then
// set_target_obj(new_obj) (:182 - HOOKED -> routes), and
// calculateTargetPoint(this, &target_x @+0x20, &target_y @+0x24)
// (:33 - the idb constant, the A3c target - routes when owned);
// return this.
RGE_Action_Enter* __fastcall RGE_Action_Enter__RGE_Action_Enter_new(
    RGE_Action_Enter* this_, DWORD dummy, RGE_Action_Object* obj_in,
    RGE_Task* task_in, RGE_Static_Object* target_obj_in)
{
    RGE_Action__RGE_Action((RGE_Action*)this_, obj_in, 1);       // asm 0x402F94 (:172 - the idb constant, HOOKED -> routes; do_setup = 1 - the base create_action_list dispatch runs the DERIVED override; the derived->base cast)
    this_->mFirstTimeEnter = 1;                                  // asm 0x402FAB (dword @+0x40 - BEFORE the vfptr store, the asm order)
    this_->vfptr = rge_action_enter_vftable;                     // asm 0x402FB2 (the Enter static vftable)
    this_->action_type = RGE_ACTION_TYPE_ENTER;                  // asm 0x402FB8 (mov word @+4, 3)
    this_->task = task_in;                                       // asm 0x402FBE (@+0x30)
    RGE_Static_Object* enter_target = target_obj_in;
    if (target_obj_in && target_obj_in->master_obj->master_type == RGE_MASTER_TYPE_BUILDING)   // asm 0x402FC6 (cmp byte [edx+4], 50h)
    {
        RGE_Static_Object* linked =
            (RGE_Static_Object*)((TRIBE_Building_Object*)target_obj_in)->linked_owner;   // asm 0x402FCC (mov ecx, [eax+1DCh] - the Building linked_owner @476, walker-verified)
        if (linked)                                              // asm 0x402FD4
            enter_target = linked;                               // asm 0x402FD6 (the linked-building redirect - enter the LINKED OWNER building)
    }
    RGE_Action__set_target_obj((RGE_Action*)this_, enter_target);   // asm 0x402FDB (:182 - HOOKED -> routes; the derived->base cast)
    RGE_Action_Enter__calculateTargetPoint(this_, &this_->target_x, &this_->target_y);   // asm 0x402FEA (:33 - the idb constant, the A3c target - routes when owned; &target_x @+0x20 / &target_y @+0x24)
    return this_;                                                // asm 0x402FF3 (mov eax, esi)
}

// Replacement for RGE_Action_Enter::destructor. Original: @ 0x00403010
// (idb_functions.h:27). Range [0x403010, 0x403020) (retn = no stack
// args), void. ABI: __thiscall. Virtual: the Enter vftable slot 0's
// body (the scalar deleting destructor above calls it DIRECTLY).
// Callers: the derived/related destructors + the scalar deleting
// destructor (the DIRECT intra-class call). Decode (asm 0x403010-
// 0x40301E): the vfptr store (the Enter static vftable);
// RGE_Action::destructor(this) - the BASE destructor (the idb constant
// HOOKED -> routes; the base's set_target_obj/2(0) + the sub_actions
// cleanup). NO Enter-specific teardown.
void __fastcall RGE_Action_Enter__destructor_new(
    RGE_Action_Enter* this_, DWORD dummy)
{
    this_->vfptr = rge_action_enter_vftable;                     // asm 0x403015
    RGE_Action__destructor((RGE_Action*)this_);                  // asm 0x403018 (:173 - the idb constant, HOOKED -> routes; the derived->base cast)
}

// ============ RUNTIME METHODS (batch A3b) ============

// Replacement for RGE_Action_Enter::first_in_stack. Original:
// @ 0x00403020 (idb_functions.h:28). Range [0x403020, 0x4030D8) (lst
// region; retn 4 = 1 stack arg), void. ABI: __thiscall. Virtual: the
// Enter vftable slot 24 = +0x60 (the set_state dispatches verified
// [eax+60h] in the asm). Decode (asm 0x403020-0x4030D8): obj->inside_obj
// @+0x2C -> set_state(13 = Failed); else the target gate: target_obj
// @+0x10 null -> set_state(4 = Goto); more_room (the Static vtbl slot
// 83 = +0x14C) true -> set_state(4 = Goto); else the cant-do chain:
// master_type @+4 == 80 (BUILDING) -> msg 3065; else object_group @+30
// == 53 (the War Machine registry) -> msg 3069; else msg 3066 - all
// three + set_state(11 = Move). THE PLAYER ARG: the asm chain
// mov ecx, base_game / mov edx, [ecx] / mov eax, [edx+0A0h] = the GAME
// OBJECT's field @+0xA0 = save_music_track_from (int @160, walked:
// soundManager@152 -> save_music_type@156 -> pad -> @160) - the .c's
// `owner->id` render = the IDA def-use aliasing artifact (the stale
// register-symbol reuse); the faithful arg = the game field verbatim.
// THE DISPLAY DISPATCH: the Base_Game vtbl slot 18 = +0x48 (the
// corrected line-arith - the brace-drift fix), the asm [edx+48h] ✓.
void __fastcall RGE_Action_Enter__first_in_stack_new(
    RGE_Action_Enter* this_, DWORD dummy, unsigned __int8 first_time)
{
    if (this_->obj->inside_obj)                                  // asm 0x403023-0x40302B (obj @+0x8 -> inside_obj @+0x2C)
    {
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);             // asm 0x403033 (the Enter vtbl slot 24 = +0x60; 13 = Failed)
        return;                                                  // asm 0x403037
    }
    if (!this_->target_obj)                                      // asm 0x40303D (@+0x10)
    {
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_GOTO);               // asm 0x4030D4 (4 = Goto)
        return;                                                  // asm 0x4030D8
    }
    if (this_->target_obj->vfptr->more_room(this_->target_obj, (RGE_Static_Object*)this_->obj))   // asm 0x403048 (slot 83 = +0x14C)
    {
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_GOTO);               // asm 0x4030CE-0x4030D4 (4 = Goto - the room to enter)
        return;                                                  // asm 0x4030D8
    }
    if (this_->target_obj->master_obj->master_type == RGE_MASTER_TYPE_BUILDING)   // asm 0x403058 (cmp byte @+4, 50h)
    {
        (*base_game)->vfptr->display_cant_do_msg(*base_game, 3065, (*base_game)->save_music_track_from);   // asm 0x403061-0x403078 (0xBF9 = 3065; the player arg = the game field @+0xA0 - the base-pointer resolution: the .c's owner->id render = the IDA def-use aliasing artifact)
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_MOVE);               // asm 0x403081 (11 = Move)
        return;                                                  // asm 0x403085
    }
    if (this_->target_obj->master_obj->object_group == 53)       // asm 0x403088-0x40308D (cmp word @+0x1E, 35h - the War Machine registry)
    {
        (*base_game)->vfptr->display_cant_do_msg(*base_game, 3069, (*base_game)->save_music_track_from);   // asm 0x4030A4-0x4030B2 (0xBFD = 3069)
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_MOVE);               // asm 0x4030B2 (11 = Move)
        return;                                                  // asm 0x4030B6
    }
    (*base_game)->vfptr->display_cant_do_msg(*base_game, 3066, (*base_game)->save_music_track_from);   // asm 0x4030B9-0x4030CB (0xBFA = 3066)
    this_->vfptr->set_state(this_, RGE_ACTION_STATE_MOVE);                   // asm 0x4030C5 (11 = Move)
}

// Replacement for RGE_Action_Enter::set_state_move. Original:
// @ 0x004030E0 (idb_functions.h:29). Range [0x4030E0, 0x403198) (lst
// region; retn = no stack args), void. ABI: __thiscall. Virtual: the
// Enter vftable (the set_state case-11 dispatch, asm 0x4031EB).
// Decode (asm 0x4030E0-0x403198): calculateTargetPoint(&target_x,
// &target_y) (:33); operator_new(0x44) (the walked sizeof(RGE_Action_
// Move_To) = 68); RGE_Action_Move_To(obj, target_x, target_y,
// target_z, 0.0, task->move_sprite @+0x30); the SEH allocation fold
// (the chunk @0x640740); the action non-null -> add_action (:67) +
// setSubAction(1) + mTargetMovedState @+0x3D = 2; else set_state(13 =
// Failed) via the Enter vtbl slot 24.
void __fastcall RGE_Action_Enter__set_state_move_new(
    RGE_Action_Enter* this_, DWORD dummy)
{
    RGE_Action_Enter__calculateTargetPoint(this_, &this_->target_x, &this_->target_y);   // asm 0x403103 (:33 - the idb constant, the A3c target - routes when owned)
    RGE_Action_Move_To* allocated = (RGE_Action_Move_To*)operator_new_internal(sizeof(RGE_Action_Move_To));   // asm 0x40310A (push 44h; the walked sizeof = 68)
    __try
    {
        if (allocated)
            RGE_Action_Move_To__RGE_Action_Move_To(allocated, this_->obj, this_->target_x, this_->target_y, this_->target_z, 0.0f, this_->task->move_sprite);   // asm 0x40313B (:100 - the idb constant, routes when the Move_To tier lands)
    }
    __except (operator_delete_internal(allocated), EXCEPTION_CONTINUE_SEARCH)   // the SEH fold: the chunk @0x640740
    {
    }
    if (allocated)                                               // asm 0x403146
    {
        RGE_Action_List__add_action(this_->sub_actions, (RGE_Action*)allocated);   // asm 0x403173 (:67)
        RGE_Action__setSubAction((RGE_Action*)allocated, 1);     // asm 0x40317C (:190)
        allocated->mTargetMovedState = 2;                        // asm 0x403185 (@+0x3D = 2)
        return;                                                  // asm 0x40318B
    }
    this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);     // asm 0x403158 (the Enter vtbl slot 24 = +0x60; 13 = Failed)
}

// Replacement for RGE_Action_Enter::set_state. Original: @ 0x004031A0
// (idb_functions.h:30). Range [0x4031A0, 0x403530) = 912 bytes (lst
// region; retn 4 = 1 stack arg), void. ABI: __thiscall. Virtual: the
// Enter vftable slot 24 = +0x60. Two-pass bar: 912 bytes < 4096; the
// gotos = the IDA switch renders; 0 asm back-edges - single-pass.
// Decode (asm 0x4031A0-0x40352F): delete_list(sub_actions) (:62) +
// state = new_state (@+0xC); the 14-case jump table (byte_403518/
// jpt_4031E2): case 11 -> set_state_move (the intra-batch); case 4 ->
// THE GOTO ARM (the big one - see below); cases 1/13/14 -> the cleanup
// (the second sub_actions delete + the base-sprite restore + the 505
// notify gated on !subActionValue); case 2 -> timer = 1.0 + the
// base-sprite restore; default -> nothing.
// THE GOTO ARM (asm 0x4031F5-0x40352F): the more_room/cant-do chain
// (the msg 3065/3069/3066 + set_state(11), the player arg = the game
// field @+0xA0 per the base-pointer resolution); the enter-in-range
// fast path: distance_to_object <= task->work_range @+0x24 (fcomp/
// test ah,41h/jz - the {<=, NaN} proceed - THE X87_LE_NAN MACRO SITE)
// && enter_target() (:32) -> set_state(1 = Done); else the Move_To
// arms: the calculateTargetPoint + the negative-coordinate split
// (fld/fcomp const_f_zero/test ah,1/jnz - the {<, NaN} arms - THE
// X87_LT_NAN MACRO SITES): the negative/NaN -> Move_To2(obj, target,
// 0.0, move_sprite); else -> Move_To(obj, target_x, target_y,
// target_z, 0.0, move_sprite); the building path -> Move_To2(obj,
// target, work_range, move_sprite); the shared tail: add_action +
// setSubAction(1) + mTargetMovedState = 2 + the moveTo dispatch on the
// TARGET (the Combat vtbl +0x1EC = slot 123, the owner-verified 6-arg
// moveTo - the calculateTargetPoint-result-driven arms); the
// no-allocation path -> set_state(13).
void __fastcall RGE_Action_Enter__set_state_new(
    RGE_Action_Enter* this_, DWORD dummy, unsigned __int8 new_state)
{
    RGE_Action_List__delete_list(this_->sub_actions);            // asm 0x4031BF (:62)
    this_->state = new_state;                                    // asm 0x4031C8 (@+0xC)
    switch (new_state)                                           // asm 0x4031CB-0x4031E2 (the 14-case jump table)
    {
    case RGE_ACTION_STATE_DONE:                                  // DONE (1)
    case RGE_ACTION_STATE_FAILED:                                // FAILED (13)
    case RGE_ACTION_STATE_INVALIDATED:                           // INVALIDATED (14)
    {
        if (this_->sub_actions)                                  // asm (the second sub_actions delete)
            RGE_Action_List__delete_list(this_->sub_actions);
        RGE_Sprite* base_sprite = this_->obj->master_obj->vfptr->get_sprite(this_->obj->master_obj, (RGE_Static_Object*)this_->obj);   // asm (the master get_sprite virtual - slot 1 = +0x4)
        this_->obj->vfptr->new_sprite(this_->obj, base_sprite);   // asm (the obj new_sprite virtual - slot 44 = +0xB0)
        if (!this_->subActionValue)                              // asm (the gate)
            this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id, 505, RGE_UNITAI_ACTION_ENTER, 0, 0);   // asm (the obj notify virtual - the Static vtbl slot 75 = +0x12C; the mType 505 blocked-entry + the ENTER action payload - the registry candidates)
        return;                                                  // asm
    }
    case RGE_ACTION_STATE_WAIT:                                  // WAIT (2)
    {
        this_->timer = 1.0f;                                     // asm (@+0x2C)
        RGE_Sprite* base_sprite = this_->obj->master_obj->vfptr->get_sprite(this_->obj->master_obj, (RGE_Static_Object*)this_->obj);
        this_->obj->vfptr->new_sprite(this_->obj, base_sprite);
        return;                                                  // asm
    }
    case RGE_ACTION_STATE_GOTO:                                  // GOTO (4) - the big arm
    {
        int isBuilding = 0;
        if (this_->target_obj)                                   // asm 0x4031F5 (@+0x10)
        {
            if (this_->target_obj->master_obj->master_type == RGE_MASTER_TYPE_BUILDING)   // asm 0x403207 (cmp byte @+4, 50h)
                isBuilding = 1;                                  // asm 0x40320D
            if (!this_->target_obj->vfptr->more_room(this_->target_obj, (RGE_Static_Object*)this_->obj))   // asm 0x40321A-0x403228 (slot 83 = +0x14C)
            {
                if (isBuilding)                                  // asm 0x40322E
                    (*base_game)->vfptr->display_cant_do_msg(*base_game, 3065, (*base_game)->save_music_track_from);   // asm 0x403232-0x40324C (0xBF9 = 3065; the player arg = the game field @+0xA0 - the base-pointer resolution)
                else
                {
                    if (this_->target_obj->master_obj->object_group == 53)   // asm 0x40325D-0x40326C (cmp word @+0x1E, 35h - the War Machine registry)
                        (*base_game)->vfptr->display_cant_do_msg(*base_game, 3069, (*base_game)->save_music_track_from);   // asm 0x40327A-0x40328B (0xBFD = 3069)
                    else
                        (*base_game)->vfptr->display_cant_do_msg(*base_game, 3066, (*base_game)->save_music_track_from);   // asm 0x403299-0x4032B3 (0xBFA = 3066)
                }
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_MOVE);   // asm (slot 24; 11 = Move)
                return;                                          // asm -> def_4031E2
            }
        }
        if (this_->target_obj
            && X87_LE_NAN(RGE_Static_Object__distance_to_object((RGE_Static_Object*)this_->obj, this_->target_obj), this_->task->work_range)   // asm 0x4032C4-0x4032DF (fcomp work_range @+0x24; test ah,41h/jz -> the {<=, NaN} proceed - THE X87_LE_NAN MACRO SITE)
            && RGE_Action_Enter__enter_target(this_))              // asm 0x4032E3 (:32 - the idb constant, the A3c target - routes when owned)
        {
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_DONE);   // asm 0x4032F2 (1 = Done)
            return;                                              // asm -> def_4031E2
        }
        int calc_result = 0;
        RGE_Action_Move_To* move_action = 0;
        if (!isBuilding)                                         // asm 0x4032FA (test ebx)
        {
            calc_result = RGE_Action_Enter__calculateTargetPoint(this_, &this_->target_x, &this_->target_y);   // asm 0x403358 (:33)
            if (X87_LT_NAN(this_->target_x, 0.0f) || X87_LT_NAN(this_->target_y, 0.0f))   // asm 0x40335D-0x40337E (fld/fcomp const_f_zero/test ah,1/jnz - the {<, NaN} arms - THE X87_LT_NAN MACRO SITES: the negative/NaN coords -> the target-object form)
            {
                move_action = (RGE_Action_Move_To*)operator_new_internal(sizeof(RGE_Action_Move_To));   // asm 0x4033BB (push 44h; the walked sizeof = 68)
                __try
                {
                    if (move_action)
                        RGE_Action_Move_To__RGE_Action_Move_To2(move_action, this_->obj, this_->target_obj, 0.0f, this_->task->move_sprite);   // asm 0x4033E8 (:42 - the idb constant, routes when the Move_To tier lands)
                }
                __except (operator_delete_internal(move_action), EXCEPTION_CONTINUE_SEARCH)
                {
                }
            }
            else
            {
                move_action = (RGE_Action_Move_To*)operator_new_internal(sizeof(RGE_Action_Move_To));   // asm 0x403380 (push 44h)
                __try
                {
                    if (move_action)
                        RGE_Action_Move_To__RGE_Action_Move_To(move_action, this_->obj, this_->target_x, this_->target_y, this_->target_z, 0.0f, this_->task->move_sprite);   // asm 0x4033B4 (the coordinate form; the idb constant, routes when owned)
                }
                __except (operator_delete_internal(move_action), EXCEPTION_CONTINUE_SEARCH)
                {
                }
            }
        }
        else
        {
            move_action = (RGE_Action_Move_To*)operator_new_internal(sizeof(RGE_Action_Move_To));   // asm 0x4032FE (push 44h)
            __try
            {
                if (move_action)
                    RGE_Action_Move_To__RGE_Action_Move_To2(move_action, this_->obj, this_->target_obj, this_->task->work_range, this_->task->move_sprite);   // asm 0x403329 (the building path: the work_range approach)
            }
            __except (operator_delete_internal(move_action), EXCEPTION_CONTINUE_SEARCH)
            {
            }
        }
        if (move_action)                                         // asm 0x4033FF
        {
            RGE_Action_List__add_action(this_->sub_actions, (RGE_Action*)move_action);   // asm 0x403415 (:67)
            RGE_Action__setSubAction((RGE_Action*)move_action, 1);   // asm 0x40341E (:190)
            move_action->mTargetMovedState = 2;                  // asm 0x403423 (@+0x3D)
            if (!isBuilding)                                     // asm 0x403423 (test ebx)
            {
                if (this_->target_obj)                           // asm 0x40342F (@+0x10)
                {
                    if (calc_result == 1)                        // asm 0x40343A
                        ((RGE_Combat_Object*)this_->target_obj)->vfptr->moveTo((RGE_Combat_Object*)this_->target_obj, this_->target_x, this_->target_y, this_->target_obj->world_z, 0.0f, 1, 0);   // asm 0x403450 (the target moveTo dispatch - the Combat vtbl +0x1EC = slot 123, the owner-verified 6-arg moveTo; the args: the calculated enter point + the target's own z)
                    else if (calc_result == 2)                   // asm 0x40345B
                        ((RGE_Combat_Object*)this_->target_obj)->vfptr->moveTo((RGE_Combat_Object*)this_->target_obj, this_->obj->world_x, this_->obj->world_y, this_->target_obj->world_z, 0.0f, 1, 0);   // asm 0x40347B (the args: the unit's own position - the pickup)
                }
            }
        }
        else
        {
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);   // asm 0x403409 (slot 24; 13 = Failed)
        }
        return;                                                  // asm -> def_4031E2
    }
    case RGE_ACTION_STATE_MOVE:                                  // MOVE (11)
        RGE_Action_Enter__set_state_move_new(this_, dummy);      // asm 0x4031EB (the intra-batch - OWNED)
        return;                                                  // asm
    default:
        return;                                                  // asm def_4031E2
    }
}

// ============ RUNTIME METHODS (batch A3c - the Enter remainder) ============

// Replacement for RGE_Action_Enter::update. Original: @ 0x00403530
// (idb_functions.h:31). Range [0x403530, 0x40370A) (lst :4402/:4646,
// retn = no stack args), unsigned __int8 return. ABI: __thiscall.
// Virtual: the Enter vftable slot 10 = +0x28 (DATA XREF lst :4402; the
// member line-arith 28654 - 28644 = 10). Callers: the action-list
// update dispatch (the list vtbl slot 4 = +0x10 - the owned list code).
// Decode (asm 0x403530-0x403709): the per-tick enter driver. THE RETIRE
// GATES (each = set_target_obj(0) + set_state(11 Move)): (1) the
// invalid target - targetID @+0x18 != -1 && the world object is gone
// (RGE_Game_World__object :6086 on obj->owner->world @+0x8C), OR
// target_obj non-null && (object_state @+0x58 >= 3, OR not the obj
// owner's ally (the owner vtbl isAlly slot 9 = +0x24,
// idb_types.h:7154, on target->owner->id @+0xA0) &&
// target->owner->id != 0); (2) target2ID @+0x1C != -1 && its object
// gone -> set_target_obj2(0) (slot 23 = +0x5C) only; (3) the target
// present but !more_room (slot 83 = +0x14C). THEN the state machine
// (byte_403724/off_40370C, 13 cases): Done -> return 1; Wait -> the
// timer tick (see below); Goto -> the target dead (null or is_dying
// :3848) -> the retire pair; else the SUB-ACTION state switch
// (sub_actions->update(), the list vtbl slot 4 = +0x10; the and-0FFh =
// the declared u8 return read, the mask folds): sub Done -> the
// distance gate (distance_to_object :3871, the DOUBLE return consumed
// unrounded by fcomp vs task->work_range @+0x24; ordered > ->
// set_state(2 Wait); else enter_target (:32, the idb constant - OWNED,
// routes) -> set_state(1 Done) / set_state(2 Wait)); sub Search -> the
// first-time re-drive (mFirstTimeEnter @+0x40 == 1 -> 0 +
// set_state(4); else set_state(2)); sub Goto/Goto2 -> the retire pair;
// default -> through; Move -> the sub update result Done or Search ->
// set_state(1); Failed -> return 3; default -> return 0. NOTE: state
// Search(3) itself falls to DEFAULT (return 0) - the byte_403724[2] = 5
// arm; only Goto(4) enters the driver body (the desync fix: the first
// write labeled the arm case Search - the tables inverted).
// FLOAT FORMS: the Wait tick computes timer - world_time_delta_seconds
// (world @+0xA8 = 168, walker-verified) at 53-bit (fsub), STORES the
// field rounded (fst @0x4036E2, st0 kept unrounded) and compares the
// UNROUNDED diff (fcomp const_f_zero; test ah,41h/jz - the {<=,NaN}
// set fires set_state(4)) - the rule-(a)/(e) mixed model: the double
// diff feeds the compare, the float store feeds the field.
// RESTRUCTURE: the redundant target_obj re-test at 0x40355E (the
// decompiler's optimized-away conditional, .c :47239) collapses into
// the 0x403551 test (nothing clobbers eax between).
unsigned __int8 __fastcall RGE_Action_Enter__update_new(
    RGE_Action_Enter* this_, DWORD dummy)
{
    RGE_Action_Object* obj = this_->obj;
    if ((this_->targetID != -1                                     // asm 0x403533
         && !RGE_Game_World__object((RGE_Game_World*)obj->owner->world, this_->targetID))   // asm 0x40353B-0x40354F (:6086 @0x623DB0; the TRIBE_World* -> RGE_Game_World* dual-naming family cast, the is_idle precedent)
        || (this_->target_obj                                      // asm 0x403551
            && (this_->target_obj->object_state >= 3               // asm 0x403558 (cmp byte @+0x58, 3; jnb - unsigned)
                || (!obj->owner->vfptr->isAlly(obj->owner, this_->target_obj->owner->id)   // asm 0x403562-0x403577 (slot 9 = +0x24; target->owner->id @+0xA0)
                    && this_->target_obj->owner->id != 0))))       // asm 0x40357B-0x403589
    {
        this_->vfptr->set_target_obj(this_, 0);                    // asm 0x40358B (slot 22 = +0x58)
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_MOVE);     // asm 0x403594 (slot 24 = +0x60; 11 = Move)
    }
    if (this_->target2ID != -1                                     // asm 0x40359D
        && !RGE_Game_World__object((RGE_Game_World*)obj->owner->world, this_->target2ID))   // asm 0x4035A5-0x4035B9 (the family cast)
        this_->vfptr->set_target_obj2(this_, 0);                   // asm 0x4035BB (slot 23 = +0x5C)
    if (this_->target_obj                                          // asm 0x4035C3
        && !this_->target_obj->vfptr->more_room(this_->target_obj, (RGE_Static_Object*)obj))   // asm 0x4035CA-0x4035D6 (slot 83 = +0x14C)
    {
        this_->vfptr->set_target_obj(this_, 0);                    // asm 0x4035DA
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_MOVE);     // asm 0x4035E3 (11 = Move)
    }
    switch (this_->state)                                          // asm 0x4035EC-0x4035FF (byte_403724/off_40370C)
    {
    case RGE_ACTION_STATE_DONE:                                    // 1 -> loc_403703
        return 1;                                                  // asm 0x403703 (mov al, 1)
    case RGE_ACTION_STATE_WAIT:                                    // 2 -> loc_4036CD
    {
        double diff = (double)this_->timer
            - (double)obj->owner->world->world_time_delta_seconds;   // asm 0x4036D0-0x4036DF (fld timer @+0x2C; fsub world @+0xA8 = 168 - the 53-bit difference)
        this_->timer = (float)diff;                                // asm 0x4036E2 (fst dword [esi+2Ch] - the store-round; st0 stays unrounded)
        if (X87_LE_NAN(diff, 0.0))                                 // asm 0x4036E5-0x4036F0 (fcomp const_f_zero; test ah,41h/jz - the compare consumes the UNROUNDED diff; {<=,NaN} -> the tick fires)
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_GOTO); // asm 0x4036F6 (4 = Goto)
        return 0;                                                  // asm 0x4036FF / loc_403664
    }
    case RGE_ACTION_STATE_GOTO:                                    // 4 -> loc_403628 (byte_403724[3] = 2 - THE ENTER DRIVER ARM; the .c's case 4u - the arm runs when the state = Goto, fired by the WAIT-timer expiry and the first-in-stack re-drive)
    {
        if (!this_->target_obj                                     // asm 0x403628
            || RGE_Static_Object__is_dying(this_->target_obj))     // asm 0x40362F (:3848 @0x550AD0)
        {
            this_->vfptr->set_target_obj(this_, 0);                // asm 0x403652
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_MOVE); // asm 0x40365B (11 = Move)
            return 0;                                              // asm -> loc_403664
        }
        switch (this_->sub_actions->vfptr->update(this_->sub_actions))   // asm 0x403638 (the list vtbl slot 4 = +0x10; and eax,0FFh - the declared u8 return, the mask folds per the mask-fold rule)
        {
        case RGE_ACTION_STATE_DONE:                                // sub 1 -> loc_40368F
        {
            double dist = RGE_Static_Object__distance_to_object((RGE_Static_Object*)obj, this_->target_obj);   // asm 0x40368F-0x403696 (:3871 - the DOUBLE return consumed unrounded)
            if (dist > this_->task->work_range)                    // asm 0x40369B-0x4036A6 (fcomp task->work_range @+0x24; test ah,41h/jz - the ordered > case = the literal MATCH row; a NaN dist falls to the enter)
            {
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_WAIT);   // asm loc_403682 (2 = Wait)
                return 0;                                          // asm 0x40368B
            }
            if (RGE_Action_Enter__enter_target(this_))             // asm 0x4036AA (:32 - the idb constant, OWNED -> routes)
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_DONE);   // asm 0x4036B3 (1 = Done)
            else
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_WAIT);   // asm 0x4036C0 (2 = Wait)
            return 0;                                              // asm 0x4036BE / 0x4036CB
        }
        case RGE_ACTION_STATE_SEARCH:                              // sub 3 -> loc_403668
        {
            if (this_->mFirstTimeEnter == 1)                       // asm 0x403668 (cmp dword @+0x40, 1)
            {
                this_->mFirstTimeEnter = 0;                        // asm 0x403674 (mov dword [esi+40h], 0 - staged between the push and the set_state call)
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_GOTO);   // asm 0x40367B (4 = Goto - the re-drive)
            }
            else
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_WAIT);   // asm loc_403682 (2 = Wait)
            return 0;                                              // asm 0x40367E / 0x40368B
        }
        case RGE_ACTION_STATE_GOTO:                                // sub 4 -> off_403734[3] = loc_403652
        case RGE_ACTION_STATE_GOTO_2:                              // sub 5 -> off_403734[4]
            this_->vfptr->set_target_obj(this_, 0);                // asm 0x403652
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_MOVE); // asm 0x40365B (11 = Move)
            return 0;                                              // asm -> loc_403664
        default:
            return 0;                                              // asm loc_403664 (sub 2 / >5)
        }
    }
    case RGE_ACTION_STATE_MOVE:                                    // 11 -> loc_403606
    {
        unsigned __int8 sub = this_->sub_actions->vfptr->update(this_->sub_actions);   // asm 0x403606 (slot 4; the and-0FFh mask folds)
        if (sub == RGE_ACTION_STATE_DONE || sub == RGE_ACTION_STATE_SEARCH)   // asm 0x40360E-0x403619 (dec eax; jz / sub eax,2; jnz)
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_DONE); // asm 0x40361B (1 = Done)
        return 0;                                                  // asm 0x403624
    }
    case RGE_ACTION_STATE_FAILED:                                  // 13 -> loc_403707
        return 3;                                                  // asm 0x403707 (mov al, 3)
    default:
        return 0;                                                  // asm loc_403664 (xor al, al)
    }
}

// Replacement for RGE_Action_Enter::enter_target. Original: @ 0x00403750
// (idb_functions.h:32). Range [0x403750, 0x403A33) (lst :4673/:4969,
// retn = no stack args), int return. ABI: __thiscall. NOT slotted - the
// direct idb constant (the owned update/set_state call sites route
// through the hook). Decode (asm 0x403750-0x403A32): THE ENTER
// EXECUTOR. (1) THE WORKER DEPOSIT - a Worker (group 58; the baked
// HARBOR_DROP_OFF patch admits Trawler group 14 at DAT >= 8) carrying a
// resource (attribute_amount_held @+0x54 > 0) into a Building (master
// type 80, not Turret 10 / Fortress 9 / master id 82) banks it: the
// amount rounds up ((int)(amount + 0.5) at 53-bit, rule (f)), the
// field re-stores, the owner vtbl banks it (add_attribute_num slot 32
// = +0x80, the updated float; trackUnitGather slot 24 = +0x60, the
// .c's (__int64) render = the artifact - the member is (int,int,int)),
// the carried attribute clears (set_attribute slot 62 = +0xF8, (-1,
// 0.0)). The attribute index: types 15/16/17 bank as 0; types 0-3 bank
// as themselves; anything else skips the whole block. (2) THE
// BUILDING-HP GATE - a Building whose master hp * 0.2 (flt_65479C =
// 0.2f, binary-verified CD CC 4C 3E) >= the target's hp refuses (msg
// 3051, the 505 notify when !subActionValue, set_state(13), return 0).
// (3) THE ENTER ATTEMPT - the selected bit captured first, enter_obj
// (slot 84 = +0x150) unless the target already has an inside_obj; the
// obj ending up NOT inside -> the 3065/3069/3066 chain +
// set_target_obj(0) + set_state(13), return 0. (4) THE SUCCESS PATH -
// the enter-completion sound (world->gbg_unknown_3 @+0x19C = 412
// indexes world->sounds @+0x3C; gbg_get_civ_override slot 2 = +0x8
// feeds the player_id; RGE_Sound__play :3662 - the stub's push -1
// before the this-only callee is play's OWN culture arg surviving as
// the lingering push), an ownerless target changes hands
// (change_ownership slot 47 = +0xBC when target->owner->id == 0), the
// 506 + 617 notify when !subActionValue, set_state(1), and the
// reselect (the saved selected bit -> get_player :689 ->
// select_one_object :2617 when sel_count @+0x26C = 0). Return 1.
int __fastcall RGE_Action_Enter__enter_target_new(
    RGE_Action_Enter* this_, DWORD dummy)
{
    RGE_Action_Object* obj = this_->obj;
    if (obj->master_obj->object_group == 58                        // asm 0x40375C-0x403764 (cmp word @+0x1E, 3Ah - the Worker registry)
        || (obj->master_obj->object_group == 14                    // asm (the stub's cmp word @+1Eh, 14 - the Utility Trawler registry - THE BAKED HARBOR_DROP_OFF extension)
            && current_loaded_version >= 8))                       // asm (the stub's cmp edx,8; jge - the signed version gate)
    {
        RGE_Master_Static_Object* target_master = this_->target_obj->master_obj;   // asm 0x40376A-0x40376D
        __int16 target_group = target_master->object_group;        // asm 0x403770 (mov ax, [ecx+1Eh])
        if (target_group != 10 && target_group != 9                // asm 0x403774/0x40377E (Turret / Fortress/AA Turret registry)
            && target_master->id != 82                             // asm 0x403788 (cmp word @+0x18, 52h - the master id 82 registry candidate)
            && obj->attribute_amount_held > 0.0f                   // asm 0x403799-0x4037A1 (fld @+0x54; fcomp const_f_zero; test ah,41h/jnz - the literal MATCH row: {<=0,NaN} skips)
            && target_master->master_type == RGE_MASTER_TYPE_BUILDING)   // asm 0x4037A7 (cmp byte @+4, 50h)
        {
            __int16 held_type = obj->attribute_type_held;          // asm 0x4037B1 (mov bp, [edi+5Ch])
            __int16 attribute_index;
            if (held_type == 15 || held_type == 16 || held_type == 17)   // asm 0x4037B5-0x4037C5
                attribute_index = 0;                               // asm 0x4037C7 (xor ebp, ebp)
            else if (held_type == 0 || held_type == 1
                || held_type == 2 || held_type == 3)               // asm 0x4037CB-0x4037E0
                attribute_index = held_type;
            else
                goto after_deposit;                                // asm 0x4037DC-0x4037E0 (jnz loc_403843)
            int deposit = (int)((double)obj->attribute_amount_held + 0.5);   // asm 0x4037E2-0x4037EB (fld; fadd const_f_half; __ftol - the 53-bit add feeds the conversion, rule (f))
            obj->attribute_amount_held = (float)deposit;           // asm 0x4037F6-0x4037FA (fild; fstp - the int back into the field)
            obj->owner->vfptr->add_attribute_num(obj->owner, attribute_index, obj->attribute_amount_held, 1);   // asm 0x4037FD-0x40380A (slot 32 = +0x80; the amount = the UPDATED float passed as the declared float)
            obj->owner->vfptr->trackUnitGather(obj->owner, obj->id, attribute_index, (int)obj->attribute_amount_held);   // asm 0x403813-0x403831 (slot 24 = +0x60; the int amount)
            obj->vfptr->set_attribute(obj, -1, 0.0f);              // asm 0x403834-0x40383D (slot 62 = +0xF8; the carried attribute (-1, 0.0))
        }
    }
after_deposit:
    RGE_Static_Object* target = this_->target_obj;
    if (target->master_obj->master_type == RGE_MASTER_TYPE_BUILDING)   // asm 0x403843-0x403849
    {
        if ((double)target->master_obj->hp * (double)0.2f >= (double)target->hp)   // asm 0x40384F-0x403869 (fild the i16 hp @+0x32; fmul flt_65479C = 0.2f, binary-verified CD CC 4C 3E; fcomp target->hp @+0x3C; test ah,1/jnz - the {<,NaN} skip = the literal >= MATCH row)
        {
            (*base_game)->vfptr->display_cant_do_msg(*base_game, 3051, obj->owner->id);   // asm 0x40386B-0x403885 (0xBEB = 3051; the slot 18 = +0x48 dispatch)
            if (!this_->subActionValue)                        // asm 0x403888 (al @+0x3C)
                obj->vfptr->notify(obj, obj->id, obj->id, 505, RGE_UNITAI_ACTION_ENTER, 0, 0);   // asm 0x403893-0x4038AB (slot 120 = +0x1E0; the mType 505 blocked-entry + the ENTER action payload - the registry candidates)
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);   // asm loc_403A21 (13 = Failed)
            return 0;                                          // asm 0x4038BD / 0x403A2D
        }
    }
    // the selected bit is captured BEFORE the attempt (the tail reselect)
    unsigned __int8 saved_selected = obj->selected;            // asm 0x4038CC (mov bl, [ecx+46h])
    if (!target->inside_obj)                                   // asm 0x4038C7-0x4038CF ([edx+2Ch])
        obj->vfptr->enter_obj(obj, target, 0);                 // asm 0x4038D1-0x4038D6 (slot 84 = +0x150)
    if (!obj->inside_obj)                                      // asm 0x4038DC-0x4038E4 (the obj failed to get inside)
    {
        if (target->master_obj->master_type == RGE_MASTER_TYPE_BUILDING)   // asm 0x4039CB-0x4039D5 (the target master re-fetch)
            (*base_game)->vfptr->display_cant_do_msg(*base_game, 3065, obj->owner->id);   // asm 0x4039D7-0x4039EE (0xBF9)
        else if (target->master_obj->object_group == 53)       // asm 0x4039F3 (cmp word @+0x1E, 35h - the War Machine registry)
            (*base_game)->vfptr->display_cant_do_msg(*base_game, 3069, obj->owner->id);   // asm 0x403A09 (0xBFD)
        else
            (*base_game)->vfptr->display_cant_do_msg(*base_game, 3066, obj->owner->id);   // asm 0x403A10 (0xBFA)
        this_->vfptr->set_target_obj(this_, 0);                // asm 0x403A18 (slot 22 = +0x58)
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);   // asm loc_403A21 (13 = Failed)
        return 0;                                              // asm 0x403A2D
    }
    // the enter-completion cue - the world's sound index + the 3d pan/volume
    int pan = 0, volume = 0;
    if (RGE_Static_Object__get_3d_sound_info((RGE_Static_Object*)obj, &pan, &volume))   // asm 0x4038EA-0x4038FD (:3893 @0x552A50 - (obj, &pan, &volume); the derived->ancestor cast)
    {
        int snd_idx = obj->owner->world->gbg_unknown_3;        // asm 0x4038FF-0x403911 ([world+19Ch] = 412 - the walker-verified field, tasks @0x18C anchor)
        if (snd_idx != -1 && obj->owner->world->sounds[snd_idx])   // asm 0x403911-0x40391D (sounds @+0x3C; the element test)
        {
            int civ = obj->vfptr->gbg_get_civ_override(obj);   // asm 0x40391F-0x403923 (slot 2 = +0x8 - this-only; the stub's push -1 here is play's OWN culture arg surviving the this-only callee = the lingering push)
            RGE_Sound__play(obj->owner->world->sounds[snd_idx], 1, pan, volume, (short)civ, -1);   // asm 0x403926-0x403945 (:3662 @0x544400 - the 5-stack-arg prototype (1, pan, volume, player_id, culture -1))
        }
    }
    if (!target->owner->id)                                    // asm 0x40394A-0x403958 (target->owner->id @+0xA0)
        target->vfptr->change_ownership(target, (RGE_Player*)obj->owner);   // asm 0x40395A-0x403963 (slot 47 = +0xBC; the TRIBE_Player* -> RGE_Player* dual-naming cast, the C2 precedent)
    if (!this_->subActionValue)                                // asm 0x403969
        obj->vfptr->notify(obj, obj->id, obj->id, 506, RGE_UNITAI_ACTION_ENTER, 0, 0);   // asm 0x403970-0x403988 (slot 120; 0x269 = 506 - the success capture; 505 above = the blocked entry)
    this_->vfptr->set_state(this_, RGE_ACTION_STATE_DONE);     // asm 0x40398E-0x403994 (1 = Done)
    if (saved_selected & 1)                                    // asm 0x403997 (test bl, 1)
    {
        RGE_Player* player = RGE_Base_Game__get_player(*base_game);   // asm 0x40399C-0x4039A2 (:689 @0x428750)
        if (!player->sel_count)                                // asm 0x4039A7 ([eax+26Ch] = 620 - the walker-verified offset)
            RGE_Player__select_one_object(player, target, 1);  // asm 0x4039B1-0x4039B9 (:2617 @0x4C2ED0)
    }
    return 1;                                                  // asm 0x4039C1 (mov eax, 1)
}

// Replacement for RGE_Action_Enter::calculateTargetPoint. Original:
// @ 0x00403A40 (idb_functions.h:33). Range [0x403A40, 0x403D23) (lst
// :4978/:5266, retn 8 = 2 stack args), int return. ABI: __thiscall.
// NOT slotted - the direct idb constant (the owned ctor /
// set_state_move / set_state call sites route through the hook).
// Decode (asm 0x403A40-0x403D22): the approach-point resolver. Null
// target -> 0 with NOTHING written. !more_room (slot 83 = +0x14C) ->
// the target's raw position, 0. maximumSpeed (slot 98 = +0x188, the
// DOUBLE return consumed unrounded) <= 0 or NaN -> the raw position, 0
// (the {<=,NaN} set - the literal <= would take NaN to the CONTINUE
// path = divergent, the macro required). State Wait/None (actionState
// slot 112 = +0x1C0 - the target's action is idle/waiting) &&
// more_room == 1 -> THE MOVING-TARGET POINT: the obj/target tile pair
// (the ftol quartet), then the target master object_flags bit 1
// (@+0xAC test 2) splits: set -> firstTileAlongLine (slot 147 =
// +0x24C) (obj -> target, 2/2 then 37/37) - a hit returns the tile
// center (+0.5 both axes), 1; both miss -> the obj tile terrain must
// be shore 2 or shallow 37 AND the target not steering (the state
// Move/Goto/Search set) for the obj-side floor point, else the raw
// position, 0; the floor point = floor(obj world_x/world_y) + 0.5 (the
// REAL CRT _floor on the double-widened float - the .c's "floor(v11);
// *tX = v11 + 0.5" render drops the floor, asm wins), 1. The
// occupied/steering arms (the state not Wait/None or the second
// more_room != 1): a steering target (Move/Goto/Search) -> its own
// action's target (get_action :78 on the actions list @+0x13C = 316 -
// the RGE_Action_Object extension; the .c's target_obj[1].mGroupID
// render = the base-pointer artifact; targetX/targetY :186/:187, the
// DOUBLE returns store-rounded by fstp) else the raw position;
// return 0.
int __fastcall RGE_Action_Enter__calculateTargetPoint_new(
    RGE_Action_Enter* this_, DWORD dummy, float* tX, float* tY)
{
    RGE_Action_Object* obj = this_->obj;
    RGE_Static_Object* target = this_->target_obj;
    if (!target)                                               // asm 0x403A47-0x403A4C
        return 0;                                              // asm loc_403D1C (NOTHING written - the null-target exit)
    if (!target->vfptr->more_room(target, (RGE_Static_Object*)obj))   // asm 0x403A52-0x403A60 (slot 83 = +0x14C)
    {
        *tX = target->world_x;                                 // asm loc_403CC4 (world_x @+0x48)
        *tY = target->world_y;                                 // asm (world_y @+0x4C)
        return 0;                                              // asm 0x403CE3
    }
    double speed = target->vfptr->maximumSpeed(target);        // asm 0x403A66-0x403A6B (slot 98 = +0x188 - the DOUBLE return captured unrounded)
    if (X87_LE_NAN(speed, 0.0))                                // asm 0x403A71-0x403A7F (fcomp const_f_zero; test ah,41h/jz - the {<=,NaN} set = this arm; a NaN speed returns the raw position in the original)
    {
        *tX = target->world_x;                                 // asm 0x403A81-0x403A96
        *tY = target->world_y;
        return 0;                                              // asm 0x403A9D
    }
    if (target->vfptr->actionState(target) == RGE_ACTION_STATE_WAIT        // asm 0x403AA0-0x403AAA (slot 112 = +0x1C0; cmp al,2)
        || target->vfptr->actionState(target) == RGE_ACTION_STATE_NONE)    // asm 0x403AAC-0x403AB9 (the second call; test al)
    {
        if (target->vfptr->more_room(target, (RGE_Static_Object*)obj) == 1)   // asm 0x403ABF-0x403AD0 (cmp al, 1)
        {
            XYPoint obj_pt;                                    // asm var_8/var_4 (the ftol quartet)
            XYPoint tgt_pt;                                    // asm var_10/var_C
            XYPoint out_pt;                                    // asm var_18/var_14 (each call's own fetch - the copy-out rule)
            obj_pt.x = (int)obj->world_x;                      // asm 0x403ADA-0x403AE2 (__ftol @+0x48)
            obj_pt.y = (int)obj->world_y;                      // asm 0x403AE6-0x403AEF (@+0x4C)
            tgt_pt.x = (int)target->world_x;                   // asm 0x403AF5-0x403AFD
            tgt_pt.y = (int)target->world_y;                   // asm 0x403B01-0x403B09
            if (target->master_obj->object_flags & 2)          // asm 0x403B0D-0x403B18 (test byte @+0xAC, 2)
            {
                if (obj->vfptr->firstTileAlongLine(obj, &obj_pt, &tgt_pt, &out_pt, 2, 2, 1) == 1)   // asm 0x403B1E-0x403B40 (slot 147 = +0x24C)
                {
                    *tX = (float)out_pt.x + 0.5f;              // asm 0x403B42-0x403B56 (fild + fadd const_f_half + fstp - the tile-center form; the exact-sum addss = rule (d) float-safe)
                    *tY = (float)out_pt.y + 0.5f;              // asm 0x403B58-0x403B62
                    return 1;                                  // asm 0x403B67
                }
                if (obj->vfptr->firstTileAlongLine(obj, &obj_pt, &tgt_pt, &out_pt, 37, 37, 1) == 1)   // asm 0x403B6A-0x403B8D (0x25 = 37)
                {
                    *tX = (float)out_pt.x + 0.5f;              // asm 0x403B8F-0x403B9D
                    *tY = (float)out_pt.y + 0.5f;              // asm 0x403B9F-0x403BA3 (fild var_14) -> the shared fadd/fstp tail loc_403C7E
                    return 1;                                  // asm eax = 1 -> loc_403C94
                }
                // the obj tile must be shore 2 or shallow 37 and the
                // target not steering for the obj-side floor point
                RGE_Map* map = obj->owner->world->map;         // asm 0x403BBC-0x403BC6 (the chain obj->owner->world->map @+0x34; the asm re-walks the chain for the second call - the local is trivially equivalent)
                if (RGE_Map__get_terrain(map, (int)obj->world_x, (int)obj->world_y) != 2   // asm 0x403BB6-0x403BD0 (:1994 @0x495B50; mapcol = ftol(x) = param1 - the map col/row convention; the int args truncate to the callee's __int16 reads, the shipped form)
                    && RGE_Map__get_terrain(map, (int)obj->world_x, (int)obj->world_y) != 37   // asm 0x403BD2-0x403BFA (the SECOND call - both kept, asm 0x403BC9/0x403BF3)
                    || target->vfptr->actionState(target) == RGE_ACTION_STATE_MOVE   // asm 0x403BFC-0x403C09 (cmp al, 0Bh)
                    || target->vfptr->actionState(target) == RGE_ACTION_STATE_GOTO   // asm 0x403C0B-0x403C18
                    || target->vfptr->actionState(target) == RGE_ACTION_STATE_SEARCH)   // asm 0x403C1A-0x403C27
                {
                    *tX = target->world_x;                     // asm loc_403C31
                    *tY = target->world_y;
                    return 0;                                  // asm 0x403C50
                }
            }
            *tX = (float)(floor((double)obj->world_x) + 0.5);  // asm loc_403C53/loc_403C56/0x403C5C (fstp var_28 double; call _floor; fadd const_f_half; fstp [tX] - THE .c's "floor(v11); *tX = v11 + 0.5" render drops the floor - the asm floors)
            *tY = (float)(floor((double)obj->world_y) + 0.5);  // asm 0x403C6D-0x403C7E (the same form; the shared tail)
            return 1;                                          // asm 0x403C89 (mov eax, 1) -> 0x403C94
        }
    }
    // the occupied/steering arms - reached when the state isn't
    // Wait/None or the second more_room isn't exactly 1 (loc_403C97)
    if (target->vfptr->actionState(target) == RGE_ACTION_STATE_MOVE   // asm 0x403C97-0x403CA4
        || target->vfptr->actionState(target) == RGE_ACTION_STATE_GOTO   // asm 0x403CA6-0x403CB3
        || target->vfptr->actionState(target) == RGE_ACTION_STATE_SEARCH)   // asm 0x403CB5-0x403CC2
    {
        // the target is steering - the approach point = the target's
        // own action's target (the actions list @+0x13C = 316 is the
        // RGE_Action_Object extension - the .c's target_obj[1].mGroupID
        // render = the base-pointer artifact; the up-cast = the original's
        // unconditional [target+13Ch] read, the set_state_new
        // (RGE_Combat_Object*)target dispatch precedent)
        RGE_Action* a = RGE_Action_List__get_action(((RGE_Action_Object*)target)->actions);   // asm 0x403CE6-0x403CEF (:78 @0x406EA0)
        *tX = (float)RGE_Action__targetX(a);                   // asm 0x403CF4-0x403CFF (:186 - the DOUBLE return, the fstp m32 store-round, rule (e))
        RGE_Action* a2 = RGE_Action_List__get_action(((RGE_Action_Object*)target)->actions);  // asm 0x403D01-0x403D0A (the second fetch - both kept)
        *tY = (float)RGE_Action__targetY(a2);                  // asm 0x403D0F-0x403D1A (:187)
    }
    else
    {
        *tX = target->world_x;                                 // asm loc_403CC4
        *tY = target->world_y;
    }
    return 0;                                                  // asm loc_403D1C (xor eax, eax)
}

void install_RGE_Action_Enter()
{
    setHook((void*)0x00402EE0, RGE_Action_Enter__RGE_Action_Enter2_new);
    setHook((void*)0x00402F50, RGE_Action_Enter__scalar_deleting_destructor_new);
    setHook((void*)0x00402F70, RGE_Action_Enter__RGE_Action_Enter_new);
    setHook((void*)0x00403010, RGE_Action_Enter__destructor_new);
    setHook((void*)0x00403020, RGE_Action_Enter__first_in_stack_new);
    setHook((void*)0x004030E0, RGE_Action_Enter__set_state_move_new);
    setHook((void*)0x004031A0, RGE_Action_Enter__set_state_new);
    setHook((void*)0x00403530, RGE_Action_Enter__update_new);
    setHook((void*)0x00403750, RGE_Action_Enter__enter_target_new);
    setHook((void*)0x00403A40, RGE_Action_Enter__calculateTargetPoint_new);
}
