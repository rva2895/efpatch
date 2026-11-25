#include "stdafx.h"
#include "..\object\RGE_Action_Types.h"
#include "..\object\RGE_Master_Static_Object.h"

// the RGE_Action_Move_To vftable (.rdata:00654D5C, lst :986367; slot 0 =
// the VECTOR deleting destructor - the Enter tier slots the scalar form)
static RGE_Action_Move_ToVtbl* const rge_action_move_to_vftable =
    (RGE_Action_Move_ToVtbl*)0x00654D5C;

void __fastcall RGE_Action_Move_To__destructor_new(RGE_Action_Move_To*, DWORD);   // the forward declaration - the vdel's intra-class call

// Replacement for RGE_Action_Move_To::RGE_Action_Move_To3 (the read ctor).
// Original: @ 0x00408190 (idb_functions.h:98). Range [0x408190, 0x4081F1)
// (lst :13183/:13226, retn 8 = 2 stack args), RGE_Action_Move_To* return.
// ABI: __thiscall. NOT virtual. Callers: RGE_Action_List__create_action2
// +1DF (the load path). The SEH: the trivial unwind (the chunk @0x6409B0).
// Decode (asm 0x408190-0x4081F0): RGE_Action::RGE_Action2(this, infile,
// obj_in, 1) - the BASE read ctor (the idb constant HOOKED -> routes),
// the vfptr store (the Move_To static vftable), action_type = 1 (the
// MOVE_TO registry value), rge_read(mRange @+0x40, 4); return this.
RGE_Action_Move_To* __fastcall RGE_Action_Move_To__RGE_Action_Move_To3_new(
    RGE_Action_Move_To* this_, DWORD dummy, int infile,
    RGE_Action_Object* obj_in)
{
    RGE_Action__RGE_Action2((RGE_Action*)this_, infile, obj_in, 1);   // asm 0x4081BA (:170 - the idb constant, HOOKED -> routes; do_setup = 1; the derived->base cast)
    this_->vfptr = rge_action_move_to_vftable;                        // asm 0x4081CE (the Move_To static vftable)
    this_->action_type = RGE_ACTION_TYPE_MOVE_TO;                     // asm 0x4081D4 (mov word @+4, 1)
    rge_read(infile, &this_->range, sizeof(this_->range));            // asm 0x4081DA (@+0x40, 4)
    return this_;                                                     // asm 0x4081E3 (mov eax, esi)
}

// Replacement for RGE_Action_Move_To::vector_deleting_destructor.
// Original: @ 0x00408200 (idb_functions.h:99). Range [0x408200, 0x40821B)
// (lst :13235/:13253, retn 4 = 1 stack arg), void* return (this). ABI:
// __thiscall. Virtual: the Move_To vftable slot 0 (DATA XREF lst :13236).
// Decode (asm 0x408200-0x40821A): destructor(this) - the DIRECT
// intra-class call (the entry HOOKED -> routes); flags & 1 ->
// operator_delete (the wrapper rule); return this. NO SEH.
void* __fastcall RGE_Action_Move_To__vector_deleting_destructor_new(
    RGE_Action_Move_To* this_, DWORD dummy, unsigned int flags)
{
    RGE_Action_Move_To__destructor_new(this_, dummy);             // asm 0x408203 (the DIRECT intra-class call - the entry HOOKED -> routes)
    if (flags & 1)                                                // asm 0x408208
        operator_delete_internal(this_);                          // asm 0x408210 (the bare operator_delete -> the wrapper rule)
    return this_;                                                 // asm 0x408218 (mov eax, esi)
}

// Replacement for RGE_Action_Move_To::RGE_Action_Move_To (the
// coordinate ctor). Original: @ 0x00408220 (idb_functions.h:100). Range
// [0x408220, 0x4082B9) (lst :13262/:13336, retn 18h = 6 stack args),
// RGE_Action_Move_To* return. ABI: __thiscall. NOT virtual. Callers:
// RGE_Action_Attack__set_state +A8/+ED and the family set_state arms
// (the owned Attack tier routes when owned). The SEH: the trivial
// unwind (the chunk @0x6409D0). Decode (asm 0x408220-0x4082B8):
// RGE_Action::RGE_Action(this, obj_in, 1); the field stores (z @+0x28,
// x @+0x20, the vfptr, action_type, y @+0x24, range @+0x40, sprite
// @+0x38 - the compiler's out-of-source-order schedule reconciled);
// THE SPRITE BOOTSTRAP (when sprite_in == 0): get_move_sprite(master,
// obj) (the master vtbl slot 30 = +0x78 - DECLARED ONLY IN THE MOVING+
// MASTER VTBLs, the M4 lowest-declaring-tier cast) as the TEST; the
// SECOND dispatch feeds the field: non-null -> get_move_sprite AGAIN,
// null -> get_sprite (slot 1 = +0x4, the base vtbl, no cast); sprite =
// the fetch. THE TWO-CALL SHAPE is real (the .c's v8-test/v10-fetch).
RGE_Action_Move_To* __fastcall RGE_Action_Move_To__RGE_Action_Move_To_new(
    RGE_Action_Move_To* this_, DWORD dummy, RGE_Action_Object* obj_in,
    float x, float y, float z, float range_in, RGE_Sprite* sprite_in)
{
    RGE_Action__RGE_Action((RGE_Action*)this_, obj_in, 1);        // asm 0x408244 (:172 - the idb constant, HOOKED -> routes; do_setup = 1; the derived->base cast)
    this_->target_z = z;                                          // asm 0x408255 ([esi+28h])
    this_->target_x = x;                                          // asm 0x40825C ([esi+20h])
    this_->vfptr = rge_action_move_to_vftable;                    // asm 0x40826D
    this_->action_type = RGE_ACTION_TYPE_MOVE_TO;                 // asm 0x408273 (mov word @+4, 1)
    this_->target_y = y;                                          // asm 0x408279 ([esi+24h])
    this_->range = range_in;                                      // asm 0x40827C ([esi+40h])
    this_->sprite = sprite_in;                                    // asm 0x40827F ([esi+38h])
    if (!sprite_in)                                               // asm 0x408282 (test eax)
    {
        RGE_Sprite* test = ((RGE_Master_Moving_Object*)obj_in->master_obj)->vfptr->get_move_sprite((RGE_Master_Moving_Object*)obj_in->master_obj, (RGE_Static_Object*)obj_in);   // asm 0x408288-0x40828D (slot 30 = +0x78; the M4 lowest-declaring-tier cast)
        if (test)                                                 // asm 0x408292
            this_->sprite = ((RGE_Master_Moving_Object*)obj_in->master_obj)->vfptr->get_move_sprite((RGE_Master_Moving_Object*)obj_in->master_obj, (RGE_Static_Object*)obj_in);   // asm 0x40829B-0x4082A3 (the SECOND dispatch = the fetch)
        else
            this_->sprite = obj_in->master_obj->vfptr->get_sprite(obj_in->master_obj, (RGE_Static_Object*)obj_in);   // asm 0x40829D (slot 1 = +0x4 - the base vtbl, no cast)
    }
    return this_;                                                 // asm 0x4082AC (mov eax, esi)
}

// Replacement for RGE_Action_Move_To::RGE_Action_Move_To2 (the target
// ctor). Original: @ 0x004082C0 (idb_functions.h:101). Range
// [0x4082C0, 0x408368) (lst :13345/:13427, retn 10h = 4 stack args),
// RGE_Action_Move_To* return. ABI: __thiscall. NOT virtual. Callers:
// RGE_Action_Attack__set_state +12E, RGE_Action_Enter__set_state +189
// (the OWNED set_state_new - routes). The SEH: the trivial unwind (the
// chunk @0x6409F0). Decode (asm 0x4082C0-0x408367): RGE_Action::RGE_
// Action(this, obj_in, 1); the vfptr; action_type; set_target_obj
// (target_obj_in) (:182 - HOOKED -> routes); IF the target:
// target_x/y/z @+0x20/24/28 = the target's world_x/y/z @+0x48/4C/50;
// range/sprite; the SAME sprite bootstrap as the coordinate ctor.
RGE_Action_Move_To* __fastcall RGE_Action_Move_To__RGE_Action_Move_To2_new(
    RGE_Action_Move_To* this_, DWORD dummy, RGE_Action_Object* obj_in,
    RGE_Static_Object* target_obj_in, float range_in, RGE_Sprite* sprite_in)
{
    RGE_Action__RGE_Action((RGE_Action*)this_, obj_in, 1);        // asm 0x4082E5 (:172 - HOOKED -> routes)
    this_->vfptr = rge_action_move_to_vftable;                    // asm 0x4082F9
    this_->action_type = RGE_ACTION_TYPE_MOVE_TO;                 // asm 0x4082FF (mov word @+4, 1)
    RGE_Action__set_target_obj((RGE_Action*)this_, target_obj_in);   // asm 0x408305 (:182 - HOOKED -> routes; the derived->base cast)
    if (target_obj_in)                                            // asm 0x40830C
    {
        this_->target_x = target_obj_in->world_x;                 // asm 0x40830E ([edi+48h] -> [esi+20h])
        this_->target_y = target_obj_in->world_y;                 // asm 0x408314 ([edi+4Ch] -> [esi+24h])
        this_->target_z = target_obj_in->world_z;                 // asm 0x40831A ([edi+50h] -> [esi+28h])
    }
    this_->range = range_in;                                      // asm 0x40832A ([esi+40h])
    this_->sprite = sprite_in;                                    // asm 0x40832D ([esi+38h])
    if (!sprite_in)                                               // asm 0x408330
    {
        RGE_Sprite* test = ((RGE_Master_Moving_Object*)obj_in->master_obj)->vfptr->get_move_sprite((RGE_Master_Moving_Object*)obj_in->master_obj, (RGE_Static_Object*)obj_in);   // asm 0x40833B (slot 30 = +0x78)
        if (test)                                                 // asm 0x408340
            this_->sprite = ((RGE_Master_Moving_Object*)obj_in->master_obj)->vfptr->get_move_sprite((RGE_Master_Moving_Object*)obj_in->master_obj, (RGE_Static_Object*)obj_in);   // asm 0x408350 (the fetch)
        else
            this_->sprite = obj_in->master_obj->vfptr->get_sprite(obj_in->master_obj, (RGE_Static_Object*)obj_in);   // asm 0x408348 (slot 1)
    }
    return this_;                                                 // asm 0x40835A (mov eax, esi)
}

// Replacement for RGE_Action_Move_To::destructor. Original:
// @ 0x00408370 (idb_functions.h:102). Range [0x408370, 0x4083C4)
// (lst :13436/:13469, retn = no stack args), void. ABI: __thiscall.
// Virtual: the vftable's destructor body (the vdel calls it DIRECTLY).
// The SEH: the trivial unwind (the chunk @0x640A10). Decode (asm
// 0x408370-0x4083C3): the vfptr store; obj->vfptr->stopMoving (the
// runtime vtbl slot 193 = +0x304, idb_types.h:4137 - the OWNED
// stopMoving_new routes); RGE_Action::destructor(this) (:173 - HOOKED
// -> routes). NO Move_To-specific teardown.
void __fastcall RGE_Action_Move_To__destructor_new(
    RGE_Action_Move_To* this_, DWORD dummy)
{
    this_->vfptr = rge_action_move_to_vftable;                    // asm 0x40838D
    this_->obj->vfptr->stopMoving(this_->obj);                    // asm 0x4083A0 (slot 193 = +0x304 = 772 decimal - the OWNED stopMoving_new routes)
    RGE_Action__destructor((RGE_Action*)this_);                   // asm 0x4083B0 (:173 - the idb constant, HOOKED -> routes; the derived->base cast)
}

// Replacement for RGE_Action_Move_To::first_in_stack. Original:
// @ 0x004083D0 (idb_functions.h:103). Range [0x4083D0, 0x408428)
// (lst :13478/:13524, retn 4 = 1 stack arg), void. ABI: __thiscall.
// Virtual: the Move_To vftable (the DATA XREF lst :13479). The
// first_time arg is UNCONSUMED (zero reads of the arg slot - kept for
// ABI). Decode (asm 0x4083D0-0x408427): dist = distance_to_object(obj,
// target_obj) (:3871 - the DOUBLE return captured unrounded); ordered
// > 0.01 (flt_654978, BINARY-VERIFIED 0A D7 23 3C = float 0.01; the
// literal > MATCH row - the jz targets the ordered-> path) ->
// speed = maximumSpeed(obj) (slot 98 = +0x188, the DOUBLE return);
// the C3-ONLY equality idiom (test ah,40h / jz -> the ordered != path):
// the taken set = {== 0, NaN} = X87_EQ_NAN (the is_idle precedent - the
// .c's literal == would take NaN to set_state(3) = DIVERGENT) ->
// == 0 or NaN -> set_state(13 Failed); else -> set_state(3 Search).
// dist <= 0.01 or NaN -> set_state(1 Done).
void __fastcall RGE_Action_Move_To__first_in_stack_new(
    RGE_Action_Move_To* this_, DWORD dummy, unsigned __int8 first_time)
{
    double dist = RGE_Static_Object__distance_to_object((RGE_Static_Object*)this_->obj, this_->target_obj);   // asm 0x4083D6-0x4083DF (:3871 @0x551B80)
    if (dist > 0.01f)                                             // asm 0x4083DF-0x4083EA (fcomp flt_654978; test ah,41h/jz - the ordered-> path = the literal MATCH row)
    {
        double speed = this_->obj->vfptr->maximumSpeed(this_->obj);   // asm 0x4083FE (slot 98 = +0x188 - the DOUBLE return captured unrounded)
        if (X87_EQ_NAN(speed, 0.0))                               // asm 0x40840A-0x40840F (fcomp const_f_zero; test ah,40h - the C3-only {==,NaN} set; the macro required)
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);   // asm 0x408417 (13 = Failed)
        else
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_SEARCH);   // asm 0x408424 (3 = Search)
    }
    else
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_DONE);    // asm 0x4083F2 (1 = Done)
}

// Replacement for RGE_Action_Move_To::set_state. Original:
// @ 0x00408430 (idb_functions.h:104). Range [0x408430, 0x408490)
// (lst :13533/:13585, retn 4 = 1 stack arg), void. ABI: __thiscall.
// Virtual: the Move_To vftable slot (DATA XREF lst :13533). THE
// SWITCH-TABLE BYTE CHECK: byte_4084A0 = db 0,2,2,1, 2,2,2,2, 2,2,2,2,
// 0,0 (state-1..14) - Done(1)/Failed(13)/Invalidated(14) -> the
// base-sprite restore; Goto(4) -> the action-sprite swap + the SELF
// update dispatch; the rest -> default. Decode (asm 0x408430-0x40848F):
// state = new_state; Done/Failed/Invalidated: subActionValue == 0 ->
// get_sprite(master, obj) (slot 1) -> new_sprite(obj, it) (slot 44 =
// +0xB0); Goto: new_sprite(obj, this->sprite @+0x38) + the SELF vtbl
// update dispatch (the Move_To vtbl slot 10 = +0x28); default ->
// return.
void __fastcall RGE_Action_Move_To__set_state_new(
    RGE_Action_Move_To* this_, DWORD dummy, unsigned __int8 new_state)
{
    this_->state = new_state;                                     // asm 0x408437 ([esi+0Ch])
    switch (new_state)                                            // asm 0x40843A-0x40844D (byte_4084A0/jpt_40844D - THE EMITTED TABLE MUST EQUAL db 0,2,2,1,2,2,2,2,2,2,2,2,0,0)
    {
    case RGE_ACTION_STATE_DONE:                                   // 1 -> loc_40846E
    case RGE_ACTION_STATE_FAILED:                                 // 13
    case RGE_ACTION_STATE_INVALIDATED:                            // 14
        if (!this_->subActionValue)                               // asm 0x408466 (al @+0x3C)
        {
            RGE_Sprite* base_sprite = this_->obj->master_obj->vfptr->get_sprite(this_->obj->master_obj, (RGE_Static_Object*)this_->obj);   // asm 0x408481 (slot 1 = +0x4)
            this_->obj->vfptr->new_sprite(this_->obj, base_sprite);   // asm 0x408488 (slot 44 = +0xB0)
        }
        return;                                                   // asm def_40844D
    case RGE_ACTION_STATE_GOTO:                                   // 4 -> loc_408454
        this_->obj->vfptr->new_sprite(this_->obj, this_->sprite); // asm 0x40845D ([esi+38h]; slot 44)
        this_->vfptr->update(this_);                              // asm 0x408467 (the SELF vtbl dispatch - the Move_To vtbl slot 10 = +0x28 = 40 decimal)
        return;                                                   // asm 0x40846B
    default:
        return;                                                   // asm def_40844D
    }
}

// Replacement for RGE_Action_Move_To::update. Original: @ 0x004084B0
// (idb_functions.h:105). Range [0x4084B0, 0x408880) = 976 bytes
// (lst :13603/:14040, retn = no stack args), unsigned __int8 return.
// ABI: __thiscall. Virtual: the Move_To vftable slot 10 = +0x28 (DATA
// XREF lst :13603). THE SWITCH-TABLE BYTE CHECK: byte_408898 = db 0,4,1,
// 2, 4,4,4,4, 4,4,4,4, 3 (state-1..13) - Done(1) -> the arrival notify;
// Search(3) -> THE RE-PATH RESOLVER; Goto(4) -> the drift band; Failed
// (13) -> the failure notify; the rest -> default. Decode (asm
// 0x4084B0-0x40887F): THE RETIRE GATE (0x4084B4): (target_obj &&
// object_state @+0x58 >= 7) || (targetID @+0x18 != -1 && the world
// object gone (:6086 on obj->owner->world, the TRIBE_World* ->
// RGE_Game_World* dual-naming cast)) -> set_target_obj(0) (slot 22);
// subActionValue @+0x3C == 1 -> return 5; notify(obj, id, id, 507,
// MOVE, targetID, 0) (slot 120 = +0x1E0 - the lost-target notify, p1 =
// the MOVE action 610, p2 = the retired targetID, the registry
// candidate) -> return 4. target2ID @+0x1C -> set_target_obj2(0) (slot
// 23). Done: notify(506, MOVE, 0, 0) when !subActionValue -> return 1.
// Search: the distance gate - the target form (distance_to_object
// :3871) or the position form (distance_to_position :3873 on
// target_x/y/z) vs range @+0x40: ordered > -> THE RE-PATH; {<=, NaN}
// -> the patrol gate: unitAI (:261) null or currentOrder (:448) == 711
// (RGE_UNITAI_ORDER_PATROL, the derived registry) or == 733 (UNKNOWN -
// the registry candidate) -> the re-path; else set_state(1 Done) ->
// return 0. THE RE-PATH (loc_40857E, the 5-inbound join): the live
// target re-sync (target_x/y/z = the target's world_x/y/z); the patrol
// locals zeroed (patrol 0, forcedWaypoint -1, linear 0, oneWay 0);
// unitAI && (order == 711 || 733) -> patrol = 1; hasPatrolInformation
// (:529) == 1 -> forcedWaypoint = mPatrolCurrentWaypoint @+0x184 (the
// direct field read), linear = getPatrolLinear (:532), oneWay =
// getPatrolOneWay (:531, u8), zeroPatrolInformation (:533); else ->
// THE LINEARIZATION: ddy*ddy + ddx*ddx >= 9.0 (flt_654DD0,
// BINARY-VERIFIED 00 00 10 41 = 9.0f; the 53-bit fmul/faddp chain on
// the UNROUNDED differences - the literal >= MATCH row: the {<9.0,NaN}
// set clears linear), order == 733 -> oneWay = 1. THE ADDMOVE
// (loc_408702): obj->vfptr->addMove (slot 226 = +0x388 - the M12a-OWNED
// addMove_new routes) with (target ? target->id : -1, x, y, z, range,
// patrol, linear, forcedWaypoint, oneWay) - the M12a-corrected
// 9-stack-arg prototype arg-for-arg; == 1 -> set_state(4 Goto); else
// set_state(13 Failed); return 0. Goto: the DRIFT BAND (loc_40875B) -
// the mixed-precision skip-chain: ddx = target_x - target->world_x
// UNROUNDED on st0 (gates 1-2 compare it); ddy = target_y -
// target->world_y STORE-ROUNDED through the float local (fstp var_4
// @0x408772; gates 3-4 reload it): X87_LT_NAN(ddx, -0.5) -> moved
// (dbl_654DC8, BINARY-VERIFIED -0.5; test ah,1 - the {<,NaN} set, the
// macro); ddx > +0.5 -> moved (dbl_654DC0, BINARY-VERIFIED +0.5; the
// literal > MATCH row - the NaN consumed by gate 1); X87_LT_NAN(ddy,
// -0.5f) -> moved (the ROUNDED dy; the macro); ddy <= +0.5f -> LABEL_45
// (the literal <= MATCH row - the ordered set; the NaN consumed by gate
// 3); else (ddy > +0.5) -> moved. THE MOVED CHECK (loc_4087B6):
// mTargetMovedState @+0x3D == 0 -> notify(obj, id, id, 508, 0, 0, 0)
// (the target-moved notify, all-zero payloads, the registry candidate)
// -> set_state(3 Search) -> return 0; == 1 -> set_state(3 Search) ->
// return 0; else -> LABEL_45. LABEL_45 (loc_4087F8 - the null-target +
// the moved-else joins): obj->vfptr->intelligentMovementUpdate (slot
// 198 = +0x318 - the SHIPPED doIntelligentMovementUpdate_new routes):
// == 1 -> set_state(13 Failed) -> return 0; == 2 -> set_state(1 Done)
// -> return 0; else -> return 0. Failed: notify(505, MOVE, 0, 0) when
// !subActionValue -> return 3. default -> return 0.
unsigned __int8 __fastcall RGE_Action_Move_To__update_new(
    RGE_Action_Move_To* this_, DWORD dummy)
{
    // THE RETIRE GATE (asm 0x4084B4-0x4084DF)
    if ((this_->target_obj && this_->target_obj->object_state >= 7)   // asm 0x4084B4-0x4084BF (cmp byte @+0x58, 7; jnb - unsigned)
        || (this_->targetID != -1                                     // asm 0x4084C1-0x4084C7
            && !RGE_Game_World__object((RGE_Game_World*)this_->obj->owner->world, this_->targetID)))   // asm 0x4084C9-0x4084DD (:6086; the family cast)
    {
        this_->vfptr->set_target_obj(this_, 0);                   // asm 0x4084E5 (slot 22 = +0x58)
        if (this_->subActionValue == 1)                           // asm 0x4084E8 (cmp byte @+0x3C, 1 EXACT)
            return 5;                                             // asm 0x4084EE (mov al, 5)
        this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id, 507, RGE_UNITAI_ACTION_MOVE, this_->targetID, 0);   // asm 0x4084F3-0x40850D (slot 120 = +0x1E0; 0x1FB = 507, 0x262 = 610; p2 = the retired targetID)
        return 4;                                                 // asm 0x408513 (mov al, 4)
    }
    if (this_->target2ID != -1                                    // asm 0x408518
        && !RGE_Game_World__object((RGE_Game_World*)this_->obj->owner->world, this_->target2ID))   // asm 0x408520-0x408534
        this_->vfptr->set_target_obj2(this_, 0);                  // asm 0x408536 (slot 23 = +0x5C; push eax = 0)
    switch (this_->state)                                         // asm 0x40853E-0x408555 (byte_408898/jpt_408555 - THE EMITTED TABLE MUST EQUAL db 0,4,1,2,4,4,4,4,4,4,4,4,3)
    {
    case RGE_ACTION_STATE_DONE:                                   // 1 -> loc_40882D
        if (!this_->subActionValue)                               // asm 0x408830
            this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id, 506, RGE_UNITAI_ACTION_MOVE, 0, 0);   // asm 0x408834-0x40884C (0x1FA = 506, 0x262 = 610)
        return 1;                                                 // asm 0x408852 (mov al, 1)
    case RGE_ACTION_STATE_SEARCH:                                 // 3 -> loc_40855C
    {
        // the distance gate: ordered > range -> the re-path;
        // {<= range, NaN} -> the patrol gate
        if (this_->target_obj)                                    // asm 0x40855F
        {
            double dist = RGE_Static_Object__distance_to_object((RGE_Static_Object*)this_->obj, this_->target_obj);   // asm 0x40856B (:3871)
            if (dist > this_->range)                              // asm 0x408570-0x408578 (fcomp range @+0x40; test ah,41h/jnz - the {<=,NaN} fall = the literal MATCH row)
                break;                                            // asm -> loc_40857E (the re-path)
        }
        else
        {
            double dist = RGE_Static_Object__distance_to_position((RGE_Static_Object*)this_->obj, this_->target_x, this_->target_y, this_->target_z);   // asm 0x408648-0x40865C (:3873 @0x551C60)
            if (dist > this_->range)                              // asm 0x40865C-0x408664 (test ah,41h/jz - the ordered > -> the re-path)
                break;                                            // asm -> loc_40857E
        }
        // THE PATROL GATE (loc_40866A): a non-patrol order with a live
        // unitAI means the move already arrived - Done
        UnitAIModule* gate_ai = RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj);   // asm 0x40867A (:261 @0x40C840; the derived->ancestor cast)
        if (gate_ai)                                              // asm 0x408672
        {
            if (UnitAIModule__currentOrder(gate_ai) != RGE_UNITAI_ORDER_PATROL   // asm 0x408684-0x40868E (cmp 2C7h = 711)
                && UnitAIModule__currentOrder(gate_ai) != 733)    // asm 0x40869E-0x4086A8 (cmp 2DDh = 733 - UNKNOWN, the registry candidate)
            {
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_DONE);   // asm 0x4086B0 (1 = Done)
                return 0;                                         // asm 0x4086B7
            }
            // a patrol/733 order -> the re-path (asm jz -> loc_40857E)
        }
        // unitAI null -> the re-path (asm 0x408674 jz -> loc_40857E)
        // fall into THE RE-PATH
        break;
    }
    case RGE_ACTION_STATE_FAILED:                                 // 13 -> loc_408857 (NOTE: state Move(11) has NO case arm - byte_408898[10] = 4 = default -> return 0; adding the label would divert the table entry into this arm - the Enter-desync class inverted)
        if (!this_->subActionValue)                               // asm 0x40885A
            this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id, 505, RGE_UNITAI_ACTION_MOVE, 0, 0);   // asm 0x40885E-0x408876 (0x1F9 = 505, 0x262 = 610)
        return 3;                                                 // asm 0x40887C (mov al, 3)
    case RGE_ACTION_STATE_GOTO:                                   // 4 -> loc_40875B
    {
        int move_result;
        bool moved = false;
        if (this_->target_obj)                                    // asm 0x40875E (the null-target -> LABEL_45 directly)
        {
            // THE DRIFT BAND - the mixed-precision skip-chain
            double ddx = (double)this_->target_x - (double)this_->target_obj->world_x;   // asm 0x408766-0x408769 - UNROUNDED on st0 (gates 1-2)
            float ddy = (float)((double)this_->target_y - (double)this_->target_obj->world_y);   // asm 0x40876C-0x408772 - the fstp var_4 STORE-ROUND (gates 3-4 reload it)
            if (X87_LT_NAN(ddx, -0.5)                             // asm 0x40877E-0x408781 (test ah,1/jnz - the {<,NaN} set -> the moved check; the macro required)
                || ddx > 0.5)                                     // asm 0x40878B-0x40878E (test ah,41h/jz - the ordered > = the literal MATCH row; the NaN consumed by gate 1)
                moved = true;                                     // asm -> loc_4087B6
            else if (X87_LT_NAN(ddy, -0.5f))                      // asm 0x40879C-0x40879F (the ROUNDED dy; {<,NaN} -> moved; the macro)
                moved = true;
            else if (ddy <= 0.5f)                                  // asm 0x4087AD-0x4087B0 (test ah,41h/jnz -> LABEL_45 - the ordered {<=} set = the literal MATCH row; the NaN consumed by gate 3)
                moved = false;                                    // asm -> loc_4087F8 (LABEL_45)
            else
                moved = true;                                     // asm 0x4087B2 (dy > +0.5 ordered)
        }
        if (moved)                                                 // loc_4087B6 (the moved check)
        {
            unsigned __int8 moved_state = this_->mTargetMovedState;   // asm 0x4087B6 (al @+0x3D)
            if (moved_state == 0)                                  // asm 0x4087B9 (test al, al)
            {
                this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id, 508, 0, 0, 0);   // asm 0x4087BD-0x4087D2 (slot 120; 0x1FC = 508 - the target-moved notify, all-zero payloads)
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_SEARCH);   // asm 0x4087DE (3 = Search)
                return 0;                                          // asm 0x4087E1
            }
            if (moved_state == 1)                                  // asm 0x4087E6 (cmp al, 1)
            {
                this_->vfptr->set_state(this_, RGE_ACTION_STATE_SEARCH);   // asm 0x4087F0 (3 = Search)
                return 0;                                          // asm 0x4087F3
            }
            // else: fall into LABEL_45 (asm 0x4087E8 jnz -> loc_4087F8)
        }
        // LABEL_45 (loc_4087F8) - the movement tick (the null-target +
        // the moved-else joins)
        move_result = this_->obj->vfptr->intelligentMovementUpdate(this_->obj);   // asm 0x4087FD (slot 198 = +0x318 = 792 decimal - the SHIPPED doIntelligentMovementUpdate_new routes)
        if (move_result == 1)                                      // asm 0x408803
        {
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);   // asm 0x40880E (13 = Failed)
            return 0;                                              // asm 0x408811
        }
        if (move_result == 2)                                      // asm 0x408816
        {
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_DONE); // asm 0x408825 (1 = Done)
            return 0;                                              // asm 0x408828
        }
        return 0;                                                  // asm def_408555
    }
    default:
        return 0;                                                  // asm def_408555 (xor al, al)
    }
    // THE RE-PATH (loc_40857E - the 5-inbound join: the two ordered->
    // distance falls + the patrol gate's three re-path edges)
    if (this_->target_obj)                                         // asm 0x40857E-0x408583
    {
        this_->target_x = this_->target_obj->world_x;              // asm 0x408585 ([eax+48h] -> [esi+20h])
        this_->target_y = this_->target_obj->world_y;              // asm 0x40858B ([eax+4Ch] -> [esi+24h])
        this_->target_z = this_->target_obj->world_z;              // asm 0x408591 ([eax+50h] -> [esi+28h])
    }
    int patrol = 0;                                                // asm 0x40859D (xor ebp, ebp)
    int forced_waypoint = -1;                                      // asm 0x40859F (or ebx, -1)
    int linear = 0;                                                // asm 0x4085A2 (xor edi, edi)
    unsigned __int8 one_way = 0;                                   // asm 0x4085A4 (mov byte [var_4], 0)
    UnitAIModule* ai = RGE_Static_Object__unitAI((RGE_Static_Object*)this_->obj);      // asm 0x4085A9 (:261 @0x40C840; the derived->ancestor cast)
    if (ai)                                                        // asm 0x4085AE
    {
        if (UnitAIModule__currentOrder(ai) == RGE_UNITAI_ORDER_PATROL   // asm 0x4085C0-0x4085CA (:448 @0x419160; cmp 2C7h)
            || UnitAIModule__currentOrder(ai) == 733)              // asm 0x4085CC-0x4085E0 (cmp 2DDh - UNKNOWN, the registry candidate)
        {
            patrol = 1;                                            // asm 0x4085E9 (mov ebp, 1)
            if (UnitAIModule__hasPatrolInformation(ai) == 1)       // asm 0x4085F5-0x4085FF (:529 @0x422080)
            {
                forced_waypoint = ai->mPatrolCurrentWaypoint;      // asm 0x40860D ([eax+184h] = 388 - the direct field read)
                linear = UnitAIModule__getPatrolLinear(ai);        // asm 0x40861A (:532 @0x4222A0)
                one_way = UnitAIModule__getPatrolOneWay(ai);       // asm 0x40862B (:531 @0x422280 - the u8)
                UnitAIModule__zeroPatrolInformation(ai);           // asm 0x40863E (:533 @0x4222C0)
            }
            else
            {
                // THE LINEARIZATION - the 53-bit chain on the
                // unrounded differences
                double ddx = (double)this_->target_x - (double)this_->obj->world_x;   // asm 0x4086B1-0x4086BF (fld/fsub)
                double ddy = (double)this_->target_y - (double)this_->obj->world_y;   // asm 0x4086C2-0x4086C5
                linear = (ddy * ddy + ddx * ddx >= 9.0);           // asm 0x4086CD-0x4086E6 (fmul x2 + faddp at 53-bit; fcomp flt_654DD0 = 9.0f BINARY-VERIFIED 00 00 10 41; test ah,1/jz - the literal >= MATCH row: the {<9.0,NaN} set clears linear)
                if (UnitAIModule__currentOrder(ai) == 733)         // asm 0x4086EA-0x4086FB
                    one_way = 1;                                   // asm 0x4086FD (mov byte [var_4], 1)
            }
        }
    }
    // THE ADDMOVE (loc_408702)
    int added = this_->obj->vfptr->addMove(this_->obj, this_->target_obj ? this_->target_obj->id : -1, this_->target_x, this_->target_y, this_->target_z, this_->range, patrol, linear, forced_waypoint, one_way);   // asm 0x408710-0x408731 (slot 226 = +0x388 = 904 decimal; the M12a-OWNED addMove_new routes; the 9-stack-arg prototype arg-for-arg)
    if (added == 1)                                                // asm 0x408739
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_GOTO);     // asm 0x408745 (4 = Goto)
    else
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_FAILED);   // asm 0x408753 (13 = Failed)
    return 0;                                                      // asm 0x408748 / 0x408756
}

// Replacement for RGE_Action_Move_To::copy_obj_sprites. Original:
// @ 0x004088B0 (idb_functions.h:106). Range [0x4088B0, 0x408973)
// (lst :14060/:14164, retn 0Ch = 3 stack args), void. ABI: __thiscall.
// Virtual: the Move_To vftable (DATA XREF lst :14061). NO SEH. Decode
// (asm 0x4088B0-0x408972): THE 4-WAY SPRITE REMAP (the transform/copy
// sprite re-binding): sprite @+0x38 null -> return; == the OWN master's
// get_sprite (slot 1 = +0x4, arg 0) -> the SOURCE master's get_sprite;
// == the own get_move_sprite (slot 30 = +0x78 - the Moving-master
// lowest-declaring-tier cast, the M4 form) -> the source's
// get_move_sprite; == the own get_fight_sprite (slot 33 = +0x84 - the
// .c's vfptr[1].do_i_care_about_power render = the artifact, the C2
// documented class; the Combat-master lowest-declaring-tier cast, the
// C2 form) -> the source's get_fight_sprite; else the RGE_Task trio:
// old_task && new_task -> move_sprite @+0x34 / work_sprite @+0x38 /
// work_sprite2 @+0x3C comparisons -> the new_task counterparts.
void __fastcall RGE_Action_Move_To__copy_obj_sprites_new(
    RGE_Action_Move_To* this_, DWORD dummy, RGE_Master_Action_Object* source,
    RGE_Task* old_task, RGE_Task* new_task)
{
    if (!this_->sprite)                                            // asm 0x4088B4-0x4088B9 ([esi+38h])
        return;                                                    // asm loc_408971
    if (this_->sprite == this_->obj->master_obj->vfptr->get_sprite(this_->obj->master_obj, 0))   // asm 0x4088C4-0x4088CF (slot 1 = +0x4; the arg = 0)
    {
        this_->sprite = source->vfptr->get_sprite(source, 0);      // asm 0x4088D9 (slot 1 on the source)
        return;                                                    // asm 0x4088DF
    }
    if (this_->sprite == ((RGE_Master_Moving_Object*)this_->obj->master_obj)->vfptr->get_move_sprite((RGE_Master_Moving_Object*)this_->obj->master_obj, 0))   // asm 0x4088EE (slot 30 = +0x78; the M4 cast)
    {
        this_->sprite = ((RGE_Master_Moving_Object*)source)->vfptr->get_move_sprite((RGE_Master_Moving_Object*)source, 0);   // asm 0x4088FE (the source cast - the sprite getters sit in the Moving+ master vtbls)
        return;                                                    // asm 0x408904
    }
    if (this_->sprite == ((RGE_Master_Combat_Object*)this_->obj->master_obj)->vfptr->get_fight_sprite((RGE_Master_Combat_Object*)this_->obj->master_obj, 0))   // asm 0x408916 (slot 33 = +0x84; the C2 lowest-declaring-tier cast)
    {
        this_->sprite = ((RGE_Master_Combat_Object*)source)->vfptr->get_fight_sprite((RGE_Master_Combat_Object*)source, 0);   // asm 0x408928
        return;                                                    // asm 0x40892E
    }
    if (old_task && new_task)                                      // asm 0x40893A/0x408942
    {
        if (this_->sprite == old_task->move_sprite)                 // asm 0x408946 ([eax+34h] = 52)
            this_->sprite = new_task->move_sprite;                  // asm 0x40894B ([ecx+34h])
        else if (this_->sprite == old_task->work_sprite)            // asm 0x408956 ([eax+38h] = 56)
            this_->sprite = new_task->work_sprite;                  // asm 0x40895B ([ecx+38h])
        else if (this_->sprite == old_task->work_sprite2)           // asm 0x408966 ([eax+3Ch] = 60)
            this_->sprite = new_task->work_sprite2;                 // asm 0x40896B ([ecx+3Ch])
    }
}

void install_RGE_Action_Move_To()
{
    setHook((void*)0x00408190, RGE_Action_Move_To__RGE_Action_Move_To3_new);
    setHook((void*)0x00408200, RGE_Action_Move_To__vector_deleting_destructor_new);
    setHook((void*)0x00408220, RGE_Action_Move_To__RGE_Action_Move_To_new);
    setHook((void*)0x004082C0, RGE_Action_Move_To__RGE_Action_Move_To2_new);
    setHook((void*)0x00408370, RGE_Action_Move_To__destructor_new);
    setHook((void*)0x004083D0, RGE_Action_Move_To__first_in_stack_new);
    setHook((void*)0x00408430, RGE_Action_Move_To__set_state_new);
    setHook((void*)0x004084B0, RGE_Action_Move_To__update_new);
    setHook((void*)0x004088B0, RGE_Action_Move_To__copy_obj_sprites_new);
}
