#include "stdafx.h"
#include "..\object\RGE_Action_Types.h"
#include "..\object\RGE_Master_Static_Object.h"

// the RGE_Action_Transport vftable (.rdata:00655288 per the .c; slot 0 =
// the SHARED family scalar deleting destructor; the tier owns no
// destructor of its own). THE STRUCT: the base Action mirror ONLY
// (64 = 0x40 bytes - the DECOMP_HIERARCHY verified prefix identity; NO
// extension fields).
static RGE_Action_TransportVtbl* const rge_action_transport_vftable =
    (RGE_Action_TransportVtbl*)0x00655288;

// Replacement for RGE_Action_Transport::RGE_Action_Transport2 (the read
// ctor). Original: @ 0x00409F00 (idb_functions.h:162). Range
// [0x409F00, 0x409F25) (lst :17135/:17155, retn 8 = 2 stack args),
// RGE_Action_Transport* return. ABI: __thiscall. NOT virtual. Callers:
// TRIBE_Action_List__create_action +3AF (the load path). NO SEH
// (plain). Decode (asm 0x409F00-0x409F24): RGE_Action::RGE_Action2
// (the idb constant HOOKED -> routes), the vfptr store
// (.rdata:00655288), action_type = 12 (the TRANSPORT registry value);
// return this.
RGE_Action_Transport* __fastcall RGE_Action_Transport__RGE_Action_Transport2_new(
    RGE_Action_Transport* this_, DWORD dummy, int infile,
    RGE_Action_Object* obj_in)
{
    RGE_Action__RGE_Action2((RGE_Action*)this_, infile, obj_in, 1);   // asm 0x409F11 (:170 - the idb constant, HOOKED -> routes; do_setup = 1; the derived->base cast)
    this_->vfptr = rge_action_transport_vftable;                      // asm 0x409F16
    this_->action_type = RGE_ACTION_TYPE_TRANSPORT;                   // asm 0x409F1C (mov word @+4, 0Ch)
    return this_;                                                     // asm 0x409F22 (mov eax, esi)
}

// Replacement for RGE_Action_Transport::RGE_Action_Transport (the
// ctor). Original: @ 0x00409F30 (idb_functions.h:163). Range
// [0x409F30, 0x409F6A) (lst :17164/:17193, retn 14h = 5 stack args),
// RGE_Action_Transport* return. ABI: __thiscall. NOT virtual. Callers:
// RGE_Combat_Object__transport +90 (the OWNED transport_new - routes) +
// RGE_Combat_Object__unload +8D (the OWNED unload_new - routes). NO SEH
// (plain). Decode (asm 0x409F30-0x409F69): RGE_Action::RGE_Action
// (:172 - HOOKED -> routes); task @+0x30, target_y @+0x24, the vfptr,
// action_type, target_x @+0x20, target_z @+0x28 (the compiler's
// out-of-source-order schedule reconciled); return this.
RGE_Action_Transport* __fastcall RGE_Action_Transport__RGE_Action_Transport_new(
    RGE_Action_Transport* this_, DWORD dummy, RGE_Action_Object* obj_in,
    RGE_Task* task_in, float x, float y, float z)
{
    RGE_Action__RGE_Action((RGE_Action*)this_, obj_in, 1);        // asm 0x409F3A (:172 - HOOKED -> routes)
    this_->task = task_in;                                        // asm 0x409F4B ([esi+30h])
    this_->target_y = y;                                          // asm 0x409F52 ([esi+24h])
    this_->vfptr = rge_action_transport_vftable;                  // asm 0x409F55
    this_->action_type = RGE_ACTION_TYPE_TRANSPORT;               // asm 0x409F5B (mov word @+4, 0Ch)
    this_->target_x = x;                                          // asm 0x409F61 ([esi+20h])
    this_->target_z = z;                                          // asm 0x409F64 ([esi+28h])
    return this_;                                                 // asm 0x409F67 (mov eax, esi)
}

// Replacement for RGE_Action_Transport::first_in_stack. Original:
// @ 0x00409F70. Range [0x409F70, 0x409FB3) (lst :17202/:17239, retn 4 =
// 1 stack arg), void. ABI: __thiscall. Virtual: the Transport vftable
// (the DATA XREF lst :17203). NO SEH. Decode (asm 0x409F70-0x409FB2):
// first_time (the arg IS consumed) && target_x >= 0.0 ordered (fld/
// fcomp const_f_zero; test ah,1/jnz - the {<,NaN} set -> the Wait path;
// the literal >= MATCH row) && target_y >= 0.0 (the same) ->
// set_state(11 Move); else -> set_state(2 Wait).
void __fastcall RGE_Action_Transport__first_in_stack_new(
    RGE_Action_Transport* this_, DWORD dummy, unsigned __int8 first_time)
{
    if (first_time                                                // asm 0x409F76 (the arg IS consumed)
        && this_->target_x >= 0.0f                                // asm 0x409F7B-0x409F86 (test ah,1/jnz - the literal >= MATCH row)
        && this_->target_y >= 0.0f)                               // asm 0x409F8B-0x409F96
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_MOVE);    // asm 0x409F9C (11 = Move)
    else
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_WAIT);    // asm 0x409FA6/0x409FB0 (2 = Wait)
}

// Replacement for RGE_Action_Transport::set_state. Original:
// @ 0x00409FC0. Range [0x409FC0, 0x40A1B8) = 512 bytes (lst :17248/
// :17459, retn 4 = 1 stack arg), void. ABI: __thiscall. Virtual: the
// Transport vftable (the DATA XREF lst :17249). THE SEH: the
// allocation-unwind fold (the chunk @0x640AD0, 2B bytes - the three
// Move_To creation sites share it; the var_4 state 0/1/2 marks the
// sprite source per site). THE ARM FORM: the asm sub CHAIN (sub 2 ->
// Wait(2); sub 2 more -> Goto(4); sub 7 more -> Move(11) falls into
// the SHARED Goto/Move block; else default) - the .c's
// `!v5 || v5 == 7` render = the Goto(4) and Move(11) arms SHARE the
// block. NO jump table (nothing to diff). Decode (asm 0x409FC0-0x40A1B7):
// delete_list(sub_actions) FIRST; state = new_state; Wait(2): the
// base-sprite restore (get_sprite -> new_sprite) ONLY; Goto(4)/Move(11)
// (the shared block loc_40A007): ma = get_map_analysis((*base_game)->
// world->map_analysis_system @+0x74 = 116, obj->master_obj) (:2047
// @0x49B470); the target staging (target_x/y -> the new_x/new_y
// out-params via the asm's own fst/fstp copies); ma non-null &&
// find_closest_pathable_point(ma, obj->world_x @+0x48, obj->world_y
// @+0x4C, target_x, target_y, &new_x, &new_y) (:2023 @0x4981F0) ->
// target_x/y = the resolved pair (target_z UNTOUCHED). THE THREE-CASE
// SPRITE/ALLOC CASCADE: task non-null -> alloc + the ctor with
// task->move_sprite @+0x34; task null -> the master get_move_sprite
// (slot 30 = +0x78, the M4 cast) TEST: non-null -> alloc + the ctor
// with get_move_sprite (the SECOND call); null -> alloc + the ctor
// with get_sprite (slot 1) - the range = 0.30000001f (flt_6552F0,
// BINARY-VERIFIED 9A 99 99 3E); the null-alloc -> set_state(2 Wait);
// else add_action (:67) THEN setSubAction(1) (:190) - THE ORDER (the
// Explore contrast). Move(11) has NO sprite restore (the Wait-only).
void __fastcall RGE_Action_Transport__set_state_new(
    RGE_Action_Transport* this_, DWORD dummy, unsigned __int8 new_state)
{
    RGE_Action_List__delete_list(this_->sub_actions);             // asm 0x409FDF (:62 - FIRST)
    this_->state = new_state;                                     // asm 0x409FE8 ([esi+0Ch])
    if (new_state == RGE_ACTION_STATE_WAIT)                       // asm 0x409FF0-0x409FF3 (sub eax, 2; jz -> loc_40A190)
    {
        RGE_Sprite* base_sprite = this_->obj->master_obj->vfptr->get_sprite(this_->obj->master_obj, (RGE_Static_Object*)this_->obj);   // asm 0x40A19B (slot 1)
        this_->obj->vfptr->new_sprite(this_->obj, base_sprite);   // asm 0x40A1A2 (slot 44 = +0xB0)
        return;                                                   // asm loc_40A1A8
    }
    if (new_state == RGE_ACTION_STATE_GOTO                        // asm 0x409FF9 (sub eax, 2; jz)
        || new_state == RGE_ACTION_STATE_MOVE)                    // asm 0x409FFE-0x40A001 (sub eax, 7; jnz -> the default return)
    {
        // THE SHARED GOTO/MOVE BLOCK (loc_40A007)
        RGE_Map_Analysis* ma = RGE_Map_Analysis_System__get_map_analysis((*base_game)->world->map_analysis_system, (RGE_Master_Static_Object*)this_->obj->master_obj);   // asm 0x40A01C (:2047 @0x49B470; world->map_analysis_system @+0x74 = 116; the master cast)
        float new_x = this_->target_x;                            // asm 0x40A021-0x40A02A (the staging fld/fst/fstp)
        float new_y = this_->target_y;                            // asm 0x40A02E-0x402036
        if (ma                                                    // asm 0x40A039 (test eax)
            && RGE_Map_Analysis__find_closest_pathable_point(ma, this_->obj->world_x, this_->obj->world_y, this_->target_x, this_->target_y, &new_x, &new_y))   // asm 0x40A05C (:2023 @0x4981F0 - (ma, sx, sy, dx, dy, &new_x, &new_y))
        {
            this_->target_x = new_x;                              // asm 0x40A06D ([esi+20h])
            this_->target_y = new_y;                              // asm 0x40A070 ([esi+24h] - target_z UNTOUCHED)
        }
        RGE_Action_Move_To* created = 0;
        unsigned __int8 sprite_case = 0;                          // asm var_4 = the SEH state 0 (the task->move_sprite site)
        if (this_->task)                                          // asm 0x40A076 (test [esi+30h])
        {
            created = (RGE_Action_Move_To*)operator_new_internal(sizeof(RGE_Action_Move_To));   // asm 0x40A07C (push 44h = 68 walked)
            if (created)                                          // asm 0x40A08A
                RGE_Action_Move_To__RGE_Action_Move_To(created, this_->obj, this_->target_x, this_->target_y, this_->target_z, 0.3f, this_->task->move_sprite);   // asm 0x40A0B6 (the range = flt_6552F0 = 0.3f BINARY-VERIFIED)
        }
        else if (((RGE_Master_Action_Object*)this_->obj->master_obj)->vfptr->get_move_sprite((RGE_Master_Action_Object*)this_->obj->master_obj, (RGE_Static_Object*)this_->obj))   // asm 0x40A0C9 (slot 30 = +0x78 = 120 decimal; the member this_ = RGE_Master_Action_Object* - the IDB's inherited-slot typing; the layout-safe Action-master cast)
        {
            sprite_case = 1;                                      // asm 0x40A0E0 (var_4 = 1)
            created = (RGE_Action_Move_To*)operator_new_internal(sizeof(RGE_Action_Move_To));   // asm 0x40A0D2
            if (created)                                          // asm 0x40A0E0
            {
                RGE_Sprite* sprite = ((RGE_Master_Action_Object*)this_->obj->master_obj)->vfptr->get_move_sprite((RGE_Master_Action_Object*)this_->obj->master_obj, (RGE_Static_Object*)this_->obj);   // asm 0x40A0F5 (the SECOND call = the fetch)
                RGE_Action_Move_To__RGE_Action_Move_To(created, this_->obj, this_->target_x, this_->target_y, this_->target_z, 0.3f, sprite);   // asm 0x40A110
            }
        }
        else
        {
            sprite_case = 2;                                      // asm 0x407127-region: 0x40A127 (var_4 = 2)
            created = (RGE_Action_Move_To*)operator_new_internal(sizeof(RGE_Action_Move_To));   // asm 0x40A117
            if (created)                                          // asm 0x40A125
            {
                RGE_Sprite* sprite = this_->obj->master_obj->vfptr->get_sprite(this_->obj->master_obj, (RGE_Static_Object*)this_->obj);   // asm 0x40A13A (slot 1 = +0x4)
                RGE_Action_Move_To__RGE_Action_Move_To(created, this_->obj, this_->target_x, this_->target_y, this_->target_z, 0.3f, sprite);   // asm 0x40A110 (the shared ctor tail)
            }
        }
        if (!created)                                             // asm 0x40A14B
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_WAIT);// asm 0x40A155 (2 = Wait)
        else
        {
            RGE_Action_List__add_action(this_->sub_actions, (RGE_Action*)created);   // asm 0x40A16F (:67 - BEFORE the setSubAction, the asm order)
            RGE_Action__setSubAction((RGE_Action*)created, 1);    // asm 0x40A178 (:190)
        }
        return;                                                   // asm 0x40A168/0x40A18D
    }
    // default (states 1/3/5/7-10/12): the bare return (asm loc_40A1A8)
}

// Replacement for RGE_Action_Transport::update. Original: @ 0x0040A1C0.
// Range [0x40A1C0, 0x40A442) = 642 bytes (lst :17468/:17764, retn = no
// stack args), unsigned __int8 return. ABI: __thiscall. Virtual: the
// Transport vftable (DATA XREF lst :17468). NO SEH. THE SWITCH-TABLE
// BYTE CHECK: byte_40A45C = db 0,5,5,1, 5,2,5,5, 5,5,3,5, 4 (state-1..
// 13): Done(1) -> return 1; Goto(4) -> the sub gate -> set_state(6
// Work); Work(6) -> THE DROP-OFF MACHINERY; Move(11) -> the sub gate ->
// set_state(2 Wait); Failed(13) -> the 505/614 notify -> Wait(2) ->
// return 3; the rest -> default. Decode (asm 0x40A1C0-0x40A441): the
// Goto/Move gates (the asm range form cmp 1/jz + cmp 2/jbe + cmp 5/ja =
// {1} U {3,4,5} - THE .c's EXPLICIT case 1/3/5 LIST DROPS sub-4 (the
// render artifact - asm wins)); Work(6) loc_40A228: THE ZONE RESOLVE -
// obj->objects @+0x30 = 48 (the RGE_New_Object_List) non-null &&
// target_x != the -1.0f sentinel && Number_of_objects @+4 > 0 (the
// signed jle) && List[0] non-null -> zone = lookupZone(List[0], ftol
// target_x, ftol target_y) (:3883 @0x552610 - the (x, y) arg order;
// the u8 and-0FFh); else zone = -1; goal = {ftol obj->world_x/y};
// carried = objects->List[0]; zone != -1 && carried &&
// !withinRangeOfZone(carried, zone, 1.5, &goal) (:3885 @0x552730) ->
// SKIP the drop-off (loc_40A3DB = the fail notify); else: objects &&
// Number_of_objects > 0 -> next_to_drop_off_terrain (the SELF vtbl
// dispatch slot 25 = +0x64 = 100 decimal - the OWNED replacement
// routes) != 0 -> the SECOND zone resolve + drop_held_objects(obj,
// zone) (:3877 @0x552010); success -> the OWNER-LEVEL notify
// (TRIBE_PlayerVtbl member 75 = +0x12C = 300 decimal): notify(owner,
// obj->id, obj->id, 518, RGE_UNITAI_ACTION_TRANSPORT, 0, 0) gated on
// objects->Number_of_objects > 0; fail -> the same with 519 (loc_
// 40A3DB); the next_to_drop == 0 path: THE TERRAIN CHECK - target_x !=
// the sentinel && get_terrain (:1994) != 1 && != 22 && != 23 (three
// separate get_terrain calls, the asm's exact shape) -> the 519 fail
// notify; else -> the bare set_state(2) (NO notify). ALL paths join
// set_state(2 Wait) -> return 0. THE TERRAIN IDS 1/22/23 = the
// drop-off terrain set (the terrain-id registry candidates). Failed:
// notify(obj, id, id, 505, RGE_UNITAI_ACTION_TRANSPORT, 0, 0) (slot
// 120 = +0x1E0) -> Wait(2) -> return 3.
unsigned __int8 __fastcall RGE_Action_Transport__update_new(
    RGE_Action_Transport* this_, DWORD dummy)
{
    switch (this_->state)                                         // asm 0x40A1C8-0x40A1D9 (byte_40A45C/jpt_40A1D9 - THE EMITTED TABLE MUST EQUAL db 0,5,5,1,5,2,5,5,5,5,3,5,4)
    {
    case RGE_ACTION_STATE_DONE:                                   // 1 -> loc_40A40E
        return 1;                                                 // asm 0x40A40E (mov al, 1)
    case RGE_ACTION_STATE_GOTO:                                   // 4 -> loc_40A204
    {
        unsigned __int8 sub = this_->sub_actions->vfptr->update(this_->sub_actions);   // asm 0x40A209 (slot 4)
        if (sub == RGE_ACTION_STATE_DONE                          // asm 0x40A20C (cmp al, 1; jz)
            || (sub > RGE_ACTION_STATE_WAIT && sub <= 5))         // asm 0x40A210-0x40A216 (cmp al,2; jbe / cmp al,5; ja - the {3,4,5} range form)
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_WORK);// asm 0x40A21E (6 = Work)
        return 0;                                                 // asm 0x408-region: 0x40A221
    }
    case RGE_ACTION_STATE_WORK:                                   // 6 -> loc_40A228 - THE DROP-OFF MACHINERY
    {
        // THE GOTO-FREE JOIN: `failed` carries the 519 decision - the
        // asm's fail_notify (loc_40A3DB/DE, 3 inbound) and set_wait
        // (loc_40A3FC, 4 inbound) tails; EVERY path lands on the single
        // set_state(2 Wait) tail
        bool failed = false;
        int zone = -1;                                            // asm loc_40A26E (or ebx, -1)
        if (this_->obj->objects                                   // asm 0x40A22D-0x402232 ([ecx+30h] = 48)
            && this_->target_x != -1.0f                           // asm 0x40A234 (cmp dword, 0BF800000h - the sentinel)
            && this_->obj->objects->Number_of_objects > 0         // asm 0x40703D-region: 0x40A23D-0x402242 ([eax+4] jle - the signed)
            && this_->obj->objects->List[0])                      // asm 0x40A244-0x40A24A (mov edx,[eax]; mov edi,[edx])
            zone = RGE_Static_Object__lookupZone(this_->obj->objects->List[0], (int)this_->target_x, (int)this_->target_y);   // asm 0x402B-region: 0x40A260 (:3883; the ftol pair y-then-x pushed = the (x, y) arg order; the and-0FFh)
        XYPoint goal;                                             // asm 0x40A271-0x40A288 (the ftol pair of the obj world_x/y)
        goal.x = (int)this_->obj->world_x;
        goal.y = (int)this_->obj->world_y;
        RGE_Static_Object* carried = this_->obj->objects ? this_->obj->objects->List[0] : 0;   // asm 0x402A8C-region: 0x40A28C-0x40A29B
        if (zone != -1 && carried                                 // asm 0x40A29B-0x4022A2
            && !RGE_Static_Object__withinRangeOfZone(carried, (unsigned __int8)zone, 1.5f, &goal))   // asm 0x40A2AF (:3885 @0x552730 - the range 1.5)
            failed = true;                                        // asm 0x402B6 -> loc_40A3DB (the not-at-zone skip)
        else if (!this_->obj->objects                             // asm 0x40A2BC-0x4022C4
            || this_->obj->objects->Number_of_objects <= 0)
            ;                                                     // asm -> loc_40A3FC (the set_state(2), no notify - the bare Wait)
        else if (this_->vfptr->next_to_drop_off_terrain(this_))   // asm 0x40A2D9 (the SELF VIRTUAL dispatch slot 25 = +0x64 = 100 decimal - the named form; the OWNED replacement routes)
        {
            // near the drop-off terrain: the second zone resolve + THE DROP
            zone = -1;                                            // asm loc_40A326
            if (this_->obj->objects                               // asm 0x402EA-region: 0x40A2EA
                && this_->target_x != -1.0f                       // asm 0x40A2EE
                && this_->obj->objects->Number_of_objects > 0     // asm 0x40A2FA-0x4022FC
                && this_->obj->objects->List[0])                  // asm 0x40A300-0x40A304
                zone = RGE_Static_Object__lookupZone(this_->obj->objects->List[0], (int)this_->target_x, (int)this_->target_y);   // asm 0x40A31A
            if (RGE_Static_Object__drop_held_objects((RGE_Static_Object*)this_->obj, zone))   // asm 0x40A32D (:3877 @0x552010)
            {
                if (this_->obj->objects                           // asm 0x40A340-0x402352
                    && this_->obj->objects->Number_of_objects > 0)// asm 0x40A348-0x40334D
                    this_->obj->owner->vfptr->notify(this_->obj->owner, this_->obj->id, this_->obj->id, 518, RGE_UNITAI_ACTION_TRANSPORT, 0, 0);   // asm 0x40359-0x40369 (push 0/0/266h/206h = (518, 614); the OWNER vtbl member 75 = +0x12C = 300 decimal - the owner-level notify)
            }
            else
                failed = true;                                    // asm 0x40337 -> loc_40A3DE (the drop failed -> 519)
        }
        else if (this_->target_x != -1.0f)                        // asm loc_40A36E (cmp eax, -1.0; the sentinel -> the fail)
        {
            RGE_Map* map = this_->obj->owner->world->map;         // asm 0x40367-0x40371 (world @+0x8C -> map @+0x34)
            if (RGE_Map__get_terrain(map, (int)this_->target_x, (int)this_->target_y) != 1   // asm 0x40378-0x4039F (the first of THREE calls - the asm's exact shape)
                && RGE_Map__get_terrain(map, (int)this_->target_x, (int)this_->target_y) != 22   // asm 0x403A1-0x403BC (16h = 22)
                && RGE_Map__get_terrain(map, (int)this_->target_x, (int)this_->target_y) != 23)   // asm 0x403BE-0x403D9 (17h = 23)
                failed = true;                                    // asm -> loc_40A3DB (the mismatch -> 519)
            // the drop-off terrain (1/22/23) -> the bare Wait, NO notify
        }
        else
            failed = true;                                        // asm 0x40A382 (the sentinel -> loc_40A3DB)
        if (failed)                                               // asm loc_40A3DB/loc_40A3DE
            this_->obj->owner->vfptr->notify(this_->obj->owner, this_->obj->id, this_->obj->id, 519, RGE_UNITAI_ACTION_TRANSPORT, 0, 0);   // asm 0x403EA-0x403F6 (0x207 = 519 - the fail notify)
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_WAIT);    // asm loc_40A3FC (2 = Wait - THE shared tail)
        return 0;                                                 // asm 0x403FD-region: 0x40A407
    }
    case RGE_ACTION_STATE_MOVE:                                   // 11 -> loc_40A1E0
    {
        unsigned __int8 sub = this_->sub_actions->vfptr->update(this_->sub_actions);   // asm 0x40A1E5 (slot 4)
        if (sub == RGE_ACTION_STATE_DONE                          // asm 0x40A1E8 (cmp al, 1; jz)
            || (sub > RGE_ACTION_STATE_WAIT && sub <= 5))         // asm 0x40A1EC-0x40A1F2 (cmp al,2; jbe / cmp al,5; ja - the {3,4,5} range form - THE .c's case list drops sub-4, the render artifact)
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_WAIT);// asm loc_40A1F4 (2 = Wait)
        return 0;                                                 // asm def_40A1D9
    }
    case RGE_ACTION_STATE_FAILED:                                 // 13 -> loc_40A415
        this_->obj->vfptr->notify(this_->obj, this_->obj->id, this_->obj->id, 505, RGE_UNITAI_ACTION_TRANSPORT, 0, 0);   // asm 0x40A42D (slot 120 = +0x1E0; 0x1F9 = 505, 0x266 = 614)
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_WAIT);    // asm 0x40A439 (2 = Wait)
        return 3;                                                 // asm 0x40A43C (mov al, 3)
    default:
        return 0;                                                 // asm def_40A1D9
    }
}

// Replacement for RGE_Action_Transport::move_to. Original: @ 0x0040A470
// (idb_functions.h:167). Range [0x40A470, 0x40A498) (lst :17785/:17798,
// retn 10h = 4 stack args), int return. ABI: __thiscall. Virtual: the
// Transport vftable (the DATA XREF lst :17785). NO SEH. THE obj_in arg
// UNCONSUMED (zero reads - kept for ABI). Decode (asm 0x40A470-0x404097):
// target_x/y/z = the args; target2ID @+0x1C = 0 (the transport's
// target2ID = the drop-off direction flag: move_to clears it, work
// sets it); set_state(11 Move); return 1.
int __fastcall RGE_Action_Transport__move_to_new(
    RGE_Action_Transport* this_, DWORD dummy, RGE_Static_Object* obj_in,
    float x, float y, float z)
{
    this_->target_x = x;                                          // asm 0x40A478 ([ecx+20h])
    this_->target_y = y;                                          // asm 0x40A47B-0x40A47F ([ecx+24h])
    this_->target_z = z;                                          // asm 0x40A47D-0x40A480 ([ecx+28h])
    this_->target2ID = 0;                                         // asm 0x40A484 (mov dword [ecx+1Ch], 0)
    this_->vfptr->set_state(this_, RGE_ACTION_STATE_MOVE);        // asm 0x40A490 (11 = Move)
    return 1;                                                     // asm 0x403-region: 0x40A493
}

// Replacement for RGE_Action_Transport::work. Original: @ 0x0040A4A0.
// Range [0x40A4A0, 0x40A603) = 355 bytes (lst :17813/:17964, retn 10h =
// 4 stack args), int return. ABI: __thiscall. Virtual: the Transport
// vftable (the DATA XREF lst :17813). NO SEH. Decode (asm 0x40A4A0-
// 0x40A602): target2ID @+0x1C = 1 (the drop-off direction); THE ZONE
// RESOLVE: lookupZone(obj, ftol x, ftol y) (:3883) overridden by the
// carried unit's lookupZone (the List[0] read - the obj->objects
// @+0x30 = 48 chain); dist = distance_to_position(obj, x, y, obj->
// world_z @+0x50) (:3873 - the z = the unit's OWN); goal = {ftol obj->
// world_x/y}; THE PICKUP PROXIMITY: zone != -1 && carried &&
// withinRangeOfZone(carried, zone, 1.5, &goal) == 1 && dist <= 0.3f
// (flt_6552F0 BINARY-VERIFIED; the literal <= MATCH row - the fcomp/
// test ah,41h/jz targets the general path) -> target_x/y/z = the args
// + set_state(6 Work) -> return 1; THE GENERAL PATH: X87_LT_NAN(x,
// 0.0) / X87_LT_NAN(y, 0.0) (the {<,NaN} set -> the bare return 1 - the
// macros required) else target_x/y/z = the args; obj_in != obj (the
// POINTER compare) -> set_state(11 Move); == -> set_state(4 Goto);
// return 1.
int __fastcall RGE_Action_Transport__work_new(
    RGE_Action_Transport* this_, DWORD dummy, RGE_Static_Object* obj_in,
    float x, float y, float z)
{
    this_->target2ID = 1;                                         // asm 0x40A4AA (mov dword [esi+1Ch], 1)
    RGE_Static_Object* carried = 0;                               // asm 0x40A4B1-0x40A4BE (the List[0] chain, null-guarded)
    if (this_->obj->objects)
        carried = this_->obj->objects->List[0];
    unsigned __int8 zone = RGE_Static_Object__lookupZone((RGE_Static_Object*)this_->obj, (int)x, (int)y);   // asm 0x40A4DD (:3883; the ftol pair y-then-x pushed = the (x, y) order; the and-0FFh)
    if (carried)                                                  // asm 0x40A4E7-0x40A4ED
        zone = RGE_Static_Object__lookupZone(carried, (int)x, (int)y);   // asm 0x40A4F3 (the carried override)
    float dist = (float)RGE_Static_Object__distance_to_position((RGE_Static_Object*)this_->obj, x, y, this_->obj->world_z);   // asm 0x40A512 (:3873 @0x551C60; the z = the unit's OWN world_z @+0x50)
    XYPoint goal;                                                 // asm 0x40A51E-0x40A532
    goal.x = (int)this_->obj->world_x;
    goal.y = (int)this_->obj->world_y;
    if (zone != -1                                                // asm 0x40A53B-0x40A53F (cmp eax, -1)
        && carried                                                // asm 0x40A541-0x40A543
        && RGE_Static_Object__withinRangeOfZone(carried, zone, 1.5f, &goal) == 1)   // asm 0x40A552 (:3885; the range 1.5)
    {
        if (X87_LE_NAN(dist, 0.3f))                               // asm 0x40A55C-0x40A56B (fcomp flt_6552F0; test ah,41h/jz - the {<=,NaN} set FALLS THROUGH to the pickup: the NaN dist FIRES in the original - the macro required; the literal <= would take the general path = DIVERGENT)
        {
            this_->target_x = x;                                  // asm 0x40A579 ([esi+20h])
            this_->target_y = y;                                  // asm 0x40A585 ([esi+24h])
            this_->target_z = z;                                  // asm 0x40A57E ([esi+28h])
            this_->vfptr->set_state(this_, RGE_ACTION_STATE_WORK);// asm 0x40A588 (6 = Work)
            return 1;                                             // asm 0x40A58C (mov eax, 1)
        }
    }
    if (X87_LT_NAN(x, 0.0) || X87_LT_NAN(y, 0.0))                 // asm 0x40A598-0x40A5B8 (fcomp const_d_zero; test ah,1/jnz - the {<,NaN} set -> the bare return; the macros required)
        return 1;                                                 // asm loc_40A5F9 (mov eax, 1 - NO state change)
    this_->target_x = x;                                          // asm 0x40A5C6 ([esi+20h])
    this_->target_y = y;                                          // asm 0x40A5CD ([esi+24h])
    this_->target_z = z;                                          // asm 0x40A5D3 ([esi+28h])
    if (obj_in != (RGE_Static_Object*)this_->obj)                 // asm 0x40A5D6-0x40A5D8 (cmp eax, ecx - the POINTER compare)
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_MOVE);    // asm 0x40A5DC (11 = Move)
    else
        this_->vfptr->set_state(this_, RGE_ACTION_STATE_GOTO);    // asm 0x40A5F2 (4 = Goto)
    return 1;                                                     // asm 0x404-region: 0x40A5E4/0x40A5FA
}

// Replacement for RGE_Action_Transport::next_to_drop_off_terrain.
// Original: @ 0x0040A610. Range [0x40A610, 0x40A7EA) = 474 bytes (lst
// :17973/:18182, retn = no stack args), int return. ABI: __thiscall.
// Virtual: the Transport vftable (the DATA XREF lst :17974). NO SEH.
// Decode (asm 0x40A610-0x40A7E9): task && task->terrain_id @+0xE = 14
// (i16) == 0xFFFF (-1) -> return 1 (the no-terrain-restriction task);
// THE SCAN RECT: col = ftol(obj->world_x - master->radius_x @+0x3C -
// 2.0f (flt_6552F4 BINARY-VERIFIED)); row = ftol(world_y - radius_y
// @+0x40 - 2.0f); right = ftol(radius_x + world_x + 2.0f); bottom =
// ftol(world_y + radius_y + 2.0f); THE CLAMPS: col < 0 -> 0; right >=
// map_width @+8 -> width-1; row < 0 -> 0; bottom >= map_height @+0xC ->
// height-1; THE ZONE: target_x != the sentinel -> lookupZone(obj,
// ftol x, ftol y) overridden by the carried unit's lookupZone; THE
// SCAN (the row-major for pair): zone == -1 -> (master object_flags
// @+0xAC bit 2 CLEAR -> true) OR get_terrain(map, col, row) == 2 OR ==
// 4 -> return 1 (the flagged master accepts ANY terrain, the unflagged
// requires the shore terrains 2/4 - the terrain-id registry
// candidates); zone != -1 -> the carried unit's lookupZone(col, row)
// == zone -> return 1; the exhaustion -> return 0.
int __fastcall RGE_Action_Transport__next_to_drop_off_terrain_new(
    RGE_Action_Transport* this_, DWORD dummy)
{
    if (this_->task && this_->task->terrain_id == -1)             // asm 0x40A61D-0x40A629 (test eax; cmp word [eax+0Eh], 0FFFFh; jnz continues)
        return 1;                                                 // asm loc_40A62B (mov eax, 1)
    RGE_Static_Object* obj = (RGE_Static_Object*)this_->obj;      // asm 0x40A638 (mov esi, [ebp+8]; the derived->ancestor cast)
    RGE_Master_Static_Object* master = obj->master_obj;           // asm 0x40A63B (mov edi, [esi+14h])
    int col = (int)(obj->world_x - master->radius_x - 2.0);       // asm 0x40A63E-0x40A64F (fld/fsub/fsub flt_6552F4/__ftol - the 53-bit chain then the store)
    int row = (int)(obj->world_y - master->radius_y - 2.0);       // asm 0x40A652-0x40A65F
    int right = (int)(master->radius_x + obj->world_x + 2.0);     // asm 0x40A664-0x40A677
    int bottom = (int)(obj->world_y + master->radius_y + 2.0);    // asm 0x40A677-0x40A687
    RGE_Map* map = obj->owner->world->map;                        // asm 0x40A68E-0x40A6A4 (owner->world @+0x8C -> map @+0x34)
    if (col < 0)                                                  // asm 0x40A69F-0x40A6A8
        col = 0;                                                  // asm 0x40A6AA
    if (right >= map->map_width)                                  // asm 0x40A6B2-0x40A6BB (map_width @+8)
        right = map->map_width - 1;                               // asm 0x40A6BD-0x40A6BE
    if (row < 0)                                                  // asm 0x40A6C2-0x40A6C6
        row = 0;                                                  // asm 0x40A6C6
    if (bottom >= map->map_height)                                // asm 0x40A6C8-0x40A6CD (map_height @+0xC)
        bottom = map->map_height - 1;                             // asm 0x40A6CF-0x40A6D0
    int zone = -1;                                                // asm loc_40A704
    if (this_->target_x != -1.0f)                                 // asm 0x40A6DC-0x40A6DE (cmp eax, edi = -1.0)
        zone = RGE_Static_Object__lookupZone(obj, (int)this_->target_x, (int)this_->target_y);   // asm 0x40A6F4 (:3883)
    RGE_Static_Object* carried = 0;                               // asm 0x40A70C-0x40A71C
    if (obj->objects)
        carried = obj->objects->List[0];
    if (this_->target_x != -1.0f                                  // asm 0x40A722 (cmp [esi+20h], edi)
        && carried)                                               // asm 0x40A727-0x40A729
        zone = RGE_Static_Object__lookupZone(carried, (int)this_->target_x, (int)this_->target_y);   // asm 0x40A73F (the carried override)
    for (int row_i = row; row_i <= bottom; row_i++)               // asm 0x40A74D-0x40A755 / 0x40A7D4-0x40A7DB
    {
        for (int col_i = col; col_i <= right; col_i++)            // asm 0x40A75B-0x40A765 / 0x40A7CB-0x40A7D2
        {
            if (zone == -1)                                       // asm 0x40A767 (cmp [esp+var_18], -1)
            {
                if ((master->object_flags & 2) == 0               // asm 0x40A792-0x40A79F (test byte [eax+0ACh], 2 - the SHIP UNIT flag (the owner-supplied bit registry): the ship transports require the shore terrains in the scan rect; the non-ships accept any)
                    || RGE_Map__get_terrain(map, col_i, row_i) == 2   // asm 0x40A7AD (the first call)
                    || RGE_Map__get_terrain(map, col_i, row_i) == 4)  // asm 0x40A7BE (the SECOND call - the asm's exact shape)
                    return 1;                                     // asm -> loc_40A62B (mov eax, 1)
            }
            else if (carried                                      // asm 0x40A76E-0x40A770
                && RGE_Static_Object__lookupZone(carried, col_i, row_i) == zone)   // asm 0x40A776 (:3883)
                return 1;                                         // asm -> loc_40A62B
        }
    }
    return 0;                                                     // asm loc_40A7E1 (xor eax, eax)
}

void install_RGE_Action_Transport()
{
    setHook((void*)0x00409F00, RGE_Action_Transport__RGE_Action_Transport2_new);
    setHook((void*)0x00409F30, RGE_Action_Transport__RGE_Action_Transport_new);
    setHook((void*)0x00409F70, RGE_Action_Transport__first_in_stack_new);
    setHook((void*)0x00409FC0, RGE_Action_Transport__set_state_new);
    setHook((void*)0x0040A1C0, RGE_Action_Transport__update_new);
    setHook((void*)0x0040A470, RGE_Action_Transport__move_to_new);
    setHook((void*)0x0040A4A0, RGE_Action_Transport__work_new);
    setHook((void*)0x0040A610, RGE_Action_Transport__next_to_drop_off_terrain_new);
}
