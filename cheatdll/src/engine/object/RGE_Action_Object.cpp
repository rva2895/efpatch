#include "stdafx.h"
#include "RGE_Action_Types.h"

// .rdata vftable of RGE_Action_Object (lst :986405)
static RGE_Action_ObjectVtbl* const rge_action_object_vftable =
    (RGE_Action_ObjectVtbl*)0x00654E54;

// Replacement for RGE_Action_Object::RGE_Action_Object (runtime ctor).
// Original: @ 0x00408980 (idb_functions.h:107). Range [0x408980, 0x408A2B)
// (lst :14173/:14249, retn 18h = 6 stack args), returns this. ABI:
// __thiscall - this_ in ECX. Callers: RGE_Combat_Object__RGE_Combat_Object
// +3D (the runtime ladder) and RGE_Master_Action_Object__make_new_obj +9D
// (owned, hook-routed; lst :14174/:14175).
// Decode (asm 0x408980-0x408A2B): base runtime Moving ctor with
// do_setup = 0 (hooked -> routes), the Action vfptr store, then IF
// do_setup: the runtime Moving setup (idb constant :2127 - the asm calls
// Moving's setup directly, the Animated-ctor shape), type @+0x5E = 40
// (0x28 - the ACTION runtime tier mirror, asm 0x4089F0), waiting @+0x140
// = 0 (asm 0x4089F4), actions @+0x13C = vfptr->create_action_list
// (slot +0x3B0, asm 0x4089FB), command_flag @+0x141 = 0, Selected_Group_
// Info @+0x142 (word) = 0.
// SEH fold: chunk 0x640A30 = mov ecx, [ebp-10h] / jmp RGE_Moving_Object__
// destructor (0x4A0800 - HOOKED -> routes into RGE_Moving_Object__
// destructor_new; the IMMEDIATE base level - runtime ladder evidence).
RGE_Action_Object* __fastcall RGE_Action_Object__RGE_Action_Object_new(
    RGE_Action_Object* this_, DWORD dummy,
    RGE_Master_Action_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z, int do_setup)
{
    RGE_Action_Object* result = this_;
    __try
    {
        RGE_Moving_Object__RGE_Moving_Object(
            (RGE_Moving_Object*)this_, (RGE_Master_Moving_Object*)tobj, obj_owner, x, y, z, 0);
        this_->vfptr = rge_action_object_vftable;                    // asm 0x4089D0
        if (do_setup != 0)
        {
            RGE_Moving_Object__setup(
                (RGE_Moving_Object*)this_, (RGE_Master_Moving_Object*)tobj, obj_owner, x, y, z);
            this_->type = 40;                                        // asm 0x4089F0 - runtime tier mirror
            this_->waiting = 0;                                      // asm 0x4089F4
            this_->actions = this_->vfptr->create_action_list(this_);   // slot +0x3B0 (asm 0x4089FB)
            this_->command_flag = 0;                                 // asm 0x408A07
            this_->Selected_Group_Info = 0;                          // asm 0x408A0E (word)
        }
        result = this_;
    }
    __except (RGE_Moving_Object__destructor((RGE_Moving_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x640A30
    {
    }
    return result;
}

// Replacement for RGE_Action_Object::vector_deleting_destructor.
// Original: @ 0x00408C50 (idb_functions.h:119). Range [0x408C50, 0x408C6B)
// (lst :14653/:14671, retn 4), void* result. ABI: __thiscall. Virtual:
// vftable DATA XREF (lst :14654). Body: the Action destructor (IN BATCH -
// the direct call routes into RGE_Action_Object__destructor_new per the
// hook-routing ruling), then flags & 1 -> operator_delete, return this.
// No SEH (plain function, push esi only).
void* __fastcall RGE_Action_Object__vector_deleting_destructor_new(
    RGE_Action_Object* this_, DWORD dummy, unsigned int flags)
{
    RGE_Action_Object__destructor(this_);
    if (flags & 1)
        operator_delete_internal(this_);
    return this_;
}

// Replacement for RGE_Action_Object::RGE_Action_Object_read (runtime
// ctor_read). Original: @ 0x00408C70 (idb_functions.h:120). Range
// [0x408C70, 0x408D1B) (lst :14680/:14751, retn 0Ch = 3 stack args),
// returns this. ABI: __thiscall - this_ in ECX. Callers: RGE_Combat_Object__
// RGE_Combat_Object_read +2B (the runtime ladder) and RGE_Object_List__
// load +1BD (lst :14681/:14682).
// Decode (asm 0x408C70-0x408D1B): base runtime Moving ctor_read with
// do_setup = 0 (hooked -> routes), the Action vfptr store, then IF
// do_setup: the runtime Moving setup_read (:2128), type = 40 (asm
// 0x408CC9), and the stream block: (1) -> waiting @+0x140, (1) ->
// command_flag @+0x141, (2) -> Selected_Group_Info @+0x142, then
// actions = vfptr->create_action_list and RGE_Action_List__load (:59,
// 0x4063B0). All read widths sizeof-exact.
// SEH fold: chunk 0x640A50 -> RGE_Moving_Object__destructor (HOOKED,
// routes) - the immediate base level.
RGE_Action_Object* __fastcall RGE_Action_Object__RGE_Action_Object_read_new(
    RGE_Action_Object* this_, DWORD dummy, int infile, RGE_Game_World* gworld, int do_setup)
{
    RGE_Action_Object* result = this_;
    __try
    {
        RGE_Moving_Object__RGE_Moving_Object_read((RGE_Moving_Object*)this_, infile, gworld, 0);
        this_->vfptr = rge_action_object_vftable;                    // asm 0x408CAE
        if (do_setup != 0)
        {
            RGE_Moving_Object__setup_read((RGE_Moving_Object*)this_, infile, gworld);
            this_->type = 40;                                        // asm 0x408CC9 - runtime tier mirror
            rge_read(infile, &this_->waiting, sizeof(this_->waiting));                        // asm 0x408CCD (1)
            rge_read(infile, &this_->command_flag, sizeof(this_->command_flag));              // asm 0x408CDC (1)
            rge_read(infile, &this_->Selected_Group_Info, sizeof(this_->Selected_Group_Info)); // asm 0x408CEB (2)
            this_->actions = this_->vfptr->create_action_list(this_);   // asm 0x408CF4 (slot +0x3B0)
            RGE_Action_List__load(this_->actions, infile);              // asm 0x408D03 (:59)
        }
        result = this_;
    }
    __except (RGE_Moving_Object__destructor((RGE_Moving_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x640A50
    {
    }
    return result;
}

// Replacement for RGE_Action_Object::destructor. Original: @ 0x00408D20
// (idb_functions.h:121). Range [0x408D20, 0x408D83) (lst :14760/:14799),
// void, no stack args. ABI: __thiscall. Callers: the vdel above and
// RGE_Combat_Object__destructor +86 (the runtime ladder, hook-routed;
// lst :14760/:14761).
// Decode (asm 0x408D20-0x408D83): vfptr store, then IF actions @+0x13C:
// actions->vfptr->__vecDelDtor(actions, 1) (RGE_Action_List vtbl slot 0
// = the scalar deleting destructor, asm 0x408D59; vftable .rdata:00654AE0
// lst :986295), actions = 0; then the runtime Moving destructor (HOOKED ->
// routes; runs its own vfptr store + initMovementData + Static destructor).
// SEH fold: chunk 0x640A70 -> RGE_Moving_Object__destructor (HOOKED,
// routes) - the throw-during-destruction unwind runs the FULL base
// destructor, immediate base level.
void __fastcall RGE_Action_Object__destructor_new(RGE_Action_Object* this_, DWORD dummy)
{
    __try
    {
        this_->vfptr = rge_action_object_vftable;                    // asm 0x408D3D
        if (this_->actions != 0)                                     // asm 0x408D43-0x408D53
        {
            this_->actions->vfptr->__vecDelDtor(this_->actions, 1);  // asm 0x408D55-0x408D59 (slot 0)
            this_->actions = 0;                                      // asm 0x408D5B
        }
        RGE_Moving_Object__destructor((RGE_Moving_Object*)this_);    // asm 0x408D6F - HOOKED, routes
    }
    __except (RGE_Moving_Object__destructor((RGE_Moving_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x640A70
    {
    }
}

// Replacement for RGE_Action_Object::recycle_out_of_game. Original:
// @ 0x00408D90 (idb_functions.h:122). Range [0x408D90, 0x408DAA)
// (lst :14808/:14824), void, no stack args. ABI: __thiscall. Virtual:
// vftable slot. Callers: RGE_Missile_Object__recycle_out_of_game +34 and
// TRIBE_Building_Object__recycle_out_of_game +190 - the downstream tiers
// call UP into this (lst :14809/:14810).
// Decode (asm 0x408D90-0x408DAA): IF actions: RGE_Action_List__delete_list
// (:62, 0x4064C0 - a delete_list, NOT the deleting destructor; the list
// object itself stays allocated, faithful), then RGE_Static_Object__
// recycle_out_of_game (:3802, 0x54C0A0 - HOOKED -> routes). NO actions = 0
// store, NO SEH (plain function, push esi only).
void __fastcall RGE_Action_Object__recycle_out_of_game_new(RGE_Action_Object* this_, DWORD dummy)
{
    if (this_->actions != 0)                                         // asm 0x408D93-0x408D9B
        RGE_Action_List__delete_list(this_->actions);                // asm 0x408D9D (:62)
    RGE_Static_Object__recycle_out_of_game((RGE_Static_Object*)this_);   // asm 0x408DA4 - HOOKED, routes
}

// Replacement for RGE_Action_Object::recycle_in_to_game. Original:
// @ 0x00408DB0 (idb_functions.h:123). Range [0x408DB0, 0x408DDB)
// (lst :14833/:14860, retn 14h = 5 stack args), void. ABI: __thiscall.
// Virtual: vftable DATA XREF (lst :14835). Caller: RGE_Combat_Object__
// recycle_in_to_game +55 (the runtime ladder, lst :14834).
// Decode (asm 0x408DB0-0x408DDB): base runtime Moving recycle_in_to_game
// (:2126, 0x4A0850 - NOT hooked, executes the original), then
// waiting @+0x140 = 0 (asm 0x408DD3). NO SEH.
void __fastcall RGE_Action_Object__recycle_in_to_game_new(
    RGE_Action_Object* this_, DWORD dummy,
    RGE_Master_Static_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z)
{
    RGE_Moving_Object__recycle_in_to_game((RGE_Moving_Object*)this_, tobj, obj_owner, x, y, z);
    this_->waiting = 0;                                              // asm 0x408DD3
}

// ============ RUNTIME METHODS (batch M2) ============

// Replacement for RGE_Action_Object::get_Selected_Groups. Original:
// @ 0x00408C30 (idb_functions.h:117). Decode (.c:51455-51457):
// return Selected_Group_Info (__int16 @+0x142 = 322).
__int16 __fastcall RGE_Action_Object__get_Selected_Groups_new(RGE_Action_Object* this_, DWORD dummy)
{
    return this_->Selected_Group_Info;
}

// Replacement for RGE_Static_Object::set_Selected_Groups (the IDA
// name is Static-named but the idb prototype's this_ is
// RGE_Action_Object* and the body operates on the Action object;
// the vftable slot served is the Action tier's). Original:
// @ 0x00408C40. Range [0x408C40, 0x408C50). Decode (.c:51461-51463):
// Selected_Group_Info @+0x142 (322) = new_groups.
void __fastcall RGE_Static_Object__set_Selected_Groups_new(
    RGE_Action_Object* this_, DWORD dummy, __int16 new_groups)
{
    this_->Selected_Group_Info = new_groups;
}

// ============ RUNTIME METHODS (batch A1) ============
// play_command_sound @0x00409610 / play_move_sound @0x00409620 are
// DEFERRED (Information Request 2026-11-22): the master vtbl slots type
// their 2nd argument RGE_Player* but every real argument in the binary is
// a runtime unit (dispatch slot +8 of the argument inside 0x488480/0x4884D0
// = gbg_get_civ_override on units). Decode restarts after the owner
// re-types the slots RGE_Static_Object* and regenerates.

// Replacement for RGE_Action_Object::rehook. Original: @ 0x00408F10
// (idb_functions.h:127). Range [0x408F10, 0x408F24) (lst :15012/:15022,
// retn = no stack args), void. ABI: __thiscall. Virtual: vftable DATA XREF
// (lst :15014). Callers: RGE_Missile_Object__rehook tail-jmp and
// TRIBE_Building_Object__rehook+3 - the downstream tiers call UP into this.
// Semantics: refresh the sprite/object bindings (base Static rehook), then
// let the action list rehook its actions.
// Decode (asm 0x408F10-0x408F23): base runtime Static rehook (idb constant
// :3827 @0x0054F4B0 - UNHOOKED, executes the original), then the Action
// extension rehook.
void __fastcall RGE_Action_Object__rehook_new(RGE_Action_Object* this_, DWORD dummy)
{
    RGE_Static_Object__rehook((RGE_Static_Object*)this_);        // asm 0x408F13
    RGE_Action_List__rehook(this_->actions);                     // asm 0x408F1E (:60); actions @+0x13C
}

// Replacement for RGE_Action_Object::transform. Original: @ 0x00408F80
// (idb_functions.h:129). Range [0x408F80, 0x408F99) (lst :15069/:15083,
// retn 4 = 1 stack arg), void. ABI: __thiscall. Virtual: vftable DATA XREF
// (lst :15070). Caller: RGE_Combat_Object__transform+5B (the runtime
// ladder).
// Semantics: re-target the object onto a new master (base Moving transform)
// and drop the current action list - the transformed unit stops acting.
// Decode (asm 0x408F80-0x408F98): base runtime Moving transform (idb
// constant :2132 @0x004A1090 - HOOKED -> routes into
// RGE_Moving_Object__transform_new per the hook-routing ruling), then
// RGE_Action_List__delete_list (delete_list, NOT the deleting destructor -
// the list object itself stays allocated; faithful to asm 0x408F93).
void __fastcall RGE_Action_Object__transform_new(
    RGE_Action_Object* this_, DWORD dummy, RGE_Master_Static_Object* obj)
{
    RGE_Moving_Object__transform((RGE_Moving_Object*)this_, obj);   // asm 0x408F88 - HOOKED, routes
    RGE_Action_List__delete_list(this_->actions);                   // asm 0x408F93 (:62)
}

// Replacement for RGE_Action_Object::destroy_obj. Original: @ 0x00408FA0
// (idb_functions.h:130). Range [0x408FA0, 0x408FB6) (lst :15092/:15103,
// retn = no stack args), void. ABI: __thiscall. Virtual: vftable DATA XREF
// (lst :15094). Caller: TRIBE_Building_Object__destroy_obj+3C (downstream
// tier calls up).
// Semantics: the dying unit's action list is deleted FIRST, then the base
// Static destroy runs (sprites, tile bindings).
// Decode (asm 0x408FA0-0x408FB5): delete_list then base
// RGE_Static_Object__destroy_obj (idb constant :3843 @0x005506B0 - UNHOOKED,
// executes the original). NO actions = 0 store (faithful).
void __fastcall RGE_Action_Object__destroy_obj_new(RGE_Action_Object* this_, DWORD dummy)
{
    RGE_Action_List__delete_list(this_->actions);                // asm 0x408FA9 (:62)
    RGE_Static_Object__destroy_obj((RGE_Static_Object*)this_);   // asm 0x408FB0 - UNHOOKED, original
}

// Replacement for RGE_Action_Object::set_action. Original: @ 0x004097E0
// (idb_functions.h:141). Range [0x4097E0, 0x4097F4) (lst :16129/:16138,
// retn 4), void. ABI: __thiscall. Virtual: vftable DATA XREF.
// Semantics: queue an action onto the list head (null action ignored).
// Decode (asm 0x4097E0-0x4097F3): guard the action pointer, then
// RGE_Action_List__add_action (:67 @0x00406BF0 - UNHOOKED).
// Join inventory: jz target locret_4097F4 has 2 inbound (jz 0x4097E6 + call
// fall-through); the joined block is only the retn - covered by the guard +
// implicit return.
void __fastcall RGE_Action_Object__set_action_new(
    RGE_Action_Object* this_, DWORD dummy, RGE_Action* action)
{
    if (action)                                                  // asm 0x4097E4-0x4097E6
        RGE_Action_List__add_action(this_->actions, action);     // asm 0x4097EF (:67)
}

// Replacement for RGE_Action_Object::set_end_action. Original: @ 0x00409830
// (idb_functions.h:143). Range [0x409830, 0x409844) (lst :16181/:16190,
// retn 4), void. ABI: __thiscall. Virtual: vftable DATA XREF.
// Semantics: queue an action onto the list END (runs after the current
// queue drains; null action ignored). Same shape as set_action with
// add_action_at_end (:68 @0x00406C40 - UNHOOKED); join inventory identical
// (locret_409844, 2 inbound, joined block = the retn).
void __fastcall RGE_Action_Object__set_end_action_new(
    RGE_Action_Object* this_, DWORD dummy, RGE_Action* action)
{
    if (action)                                                  // asm 0x409834-0x409836
        RGE_Action_List__add_action_at_end(this_->actions, action);   // asm 0x40983F (:68)
}

// Replacement for RGE_Action_Object::have_action. Original: @ 0x00409970
// (idb_functions.h:148). Range [0x409970, 0x409980) (lst :16371/:16376,
// retn = no stack args), unsigned __int8 return (AL). ABI: __thiscall.
// Virtual: vftable DATA XREF.
// Semantics: does the unit have any action queued.
// Decode (asm 0x409970-0x40997F): RGE_Action_List__have_action (:72
// @0x00406D90, u8 return) called with NO null guard on the list (the asm
// passes this->actions unconditionally - faithful), then test al,al /
// setnz al = the != 0 normalization. Single exit (retn @lst :16375).
unsigned __int8 __fastcall RGE_Action_Object__have_action_new(RGE_Action_Object* this_, DWORD dummy)
{
    return RGE_Action_List__have_action(this_->actions) != 0;    // asm 0x409976-0x40997D
}

// Replacement for RGE_Action_Object::gameActionType. Original: @ 0x00409990
// (idb_functions.h:149). Range [0x409990, 0x4099A7) (lst :16389/:16399,
// retn = no stack args), int return. ABI: __thiscall. Virtual: vftable DATA
// XREF.
// Semantics: the current action's type id, -1 when idle.
// Decode (asm 0x409990-0x4099A6): RGE_Action_List__get_action (:78
// @0x00406EA0); null -> -1 (or eax,-1 @0x40999F); else action_type
// (__int16 @+4, idb_types.h:15022) sign-extended (movsx word ptr [eax+4]
// @0x4099A3). Exit paths: 2 asm returns -> the single ternary return maps
// both. loc_4099A3 has 1 inbound (jnz 0x40999D) - not a join.
int __fastcall RGE_Action_Object__gameActionType_new(RGE_Action_Object* this_, DWORD dummy)
{
    RGE_Action* action = RGE_Action_List__get_action(this_->actions);   // asm 0x409996
    return action ? action->action_type : -1;                           // asm 0x40999B-0x4099A6
}

// Replacement for RGE_Action_Object::get_target_obj. Original: @ 0x00409A40
// (idb_functions.h:151). Range [0x409A40, 0x409A58) (retn = no stack args),
// RGE_Static_Object* return. ABI: __thiscall. Virtual: vftable DATA XREF.
// Semantics: the current action's target object, null when idle.
// Decode (asm 0x409A40-0x409A57): get_action (:78); null -> 0; else the
// NAMED virtual dispatch action->vfptr->get_target_obj(action)
// (RGE_ActionVtbl member 20 = +0x50, line-arith 15982-15962 = 20; asm
// jmp dword ptr [edx+50h] @0x409A53 - the compiler's tail-call form; the
// named dispatch is the mandated form per VIRTUAL-DISPATCH FIDELITY).
// loc_409A56 has 1 inbound (jz 0x409A4D) - not a join.
RGE_Static_Object* __fastcall RGE_Action_Object__get_target_obj_new(RGE_Action_Object* this_, DWORD dummy)
{
    RGE_Action* action = RGE_Action_List__get_action(this_->actions);   // asm 0x409A46
    if (!action)                                                        // asm 0x409A4B-0x409A4D
        return 0;                                                       // asm 0x409A56-0x409A58
    return action->vfptr->get_target_obj(action);                       // asm 0x409A4F-0x409A53 (slot +0x50)
}

// Replacement for RGE_Action_Object::searchRadius. Original: @ 0x00409B70
// (idb_functions.h:154). Range [0x409B70, 0x409B79) (lst :16656/:16658,
// retn = no stack args), DOUBLE return (st0 passthrough). ABI: __thiscall.
// Virtual: vftable DATA XREF.
// Semantics: the master's task search radius (how far the unit looks for
// work).
// Decode (asm 0x409B70-0x409B78): fld dword ptr [master_obj+10Ch] -
// master_obj @+0x14 (idb_types.h:7440), RGE_Master_Action_Object::
// search_radius float @268 = 0x10C (idb_types.h:10796, hand-counted;
// struct total 300 = 0x12C matches the hierarchy; the twin master's
// move_sound @+0x124 = 292 corroborates the same walk). Pure passthrough -
// no arithmetic, no compare, no store-round; the float->double return
// widening is exact (fld m32 == cvtps2pd - rule (h) satisfied, prototype
// already double). x87 audit: the only x87 site in the batch; no macros
// required. /Fa gate check: cvtss2sd expected, no ss-op chain feeding it.
double __fastcall RGE_Action_Object__searchRadius_new(RGE_Action_Object* this_, DWORD dummy)
{
    return this_->master_obj->search_radius;                     // asm 0x409B73 (fld [eax+10Ch])
}

// Replacement for RGE_Action_Object::hasActionOfType. Original:
// @ 0x00409BD0 (idb_functions.h:157). Range [0x409BD0, 0x409BE9)
// (retn 4 = 1 stack arg), int return. ABI: __thiscall. Virtual: vftable
// DATA XREF.
// Semantics: does the unit hold an action of the given type.
// Decode (asm 0x409BD0-0x409BE8): the LIST POINTER is null-checked
// (test ecx,ecx @0x409BD6 - the guard is on actions, not the action), then
// RGE_Action_List__have_action2 (:73 @0x00406DA0, int return). Exit paths:
// 2 asm returns (retn 4 @0x409BE4; xor/retn 4 @0x409BE7-0x409BE9) -> the
// written early-return + tail return map both. loc_409BE7 has 1 inbound.
int __fastcall RGE_Action_Object__hasActionOfType_new(
    RGE_Action_Object* this_, DWORD dummy, int type)
{
    if (!this_->actions)                                         // asm 0x409BD6-0x409BD8
        return 0;                                                // asm 0x409BE7-0x409BE9
    return RGE_Action_List__have_action2(this_->actions, type);  // asm 0x409BDF (:73)
}

// ============ RUNTIME METHODS (batch A2) ============

// Replacement for RGE_Action_Object::set_only_action. Original:
// @ 0x00409800 (idb_functions.h:142). Range [0x409800, 0x409823)
// (retn 4 = 1 stack arg), void. ABI: __thiscall. Virtual: vftable DATA
// XREF (Action/Combat/downstream vftables). Also dispatched INTERNALLY
// (change_ownership slot 231) - covered by this entry hook.
// Semantics: replace the unit's whole action queue with one action (or
// clear it): the list is deleted UNCONDITIONALLY first, then the new
// action is queued if non-null.
// Decode (asm 0x409800-0x409822): delete_list (:62 @0x004064C0 -
// UNHOOKED), guard the action pointer, add_action (:67 @0x00406BF0 -
// UNHOOKED). Join inventory: loc_409822 has 2 inbound (jz 0x409814 + call
// fall-through); the joined block is only pop/retn - covered by the guard
// + implicit return.
void __fastcall RGE_Action_Object__set_only_action_new(
    RGE_Action_Object* this_, DWORD dummy, RGE_Action* action)
{
    RGE_Action_List__delete_list(this_->actions);                // asm 0x409809 (:62)
    if (action)                                                  // asm 0x409812-0x409814
        RGE_Action_List__add_action(this_->actions, action);     // asm 0x40981D (:67)
}

// Replacement for RGE_Action_Object::stop. Original: @ 0x00409850
// (idb_functions.h:144). Range [0x409850, 0x40986E) (retn = no stack
// args), void. ABI: __thiscall. Virtual: vftable DATA XREF. Caller:
// RGE_Combat_Object__stop+3 (the runtime ladder calls UP into this).
// Semantics: ask the current action to stop; if nothing stopped (the list
// returns 0), the whole list is deleted (the unit is fully idle).
// Decode (asm 0x409850-0x40986D): RGE_Action_List__action_stop (:75
// @0x00406E20 - UNHOOKED, int return); test/jnz skips the delete_list
// (:62). Join inventory: loc_40986D has 2 inbound (jnz 0x409860 + call
// fall-through); joined block = pop/retn - covered by the single guard
// form.
void __fastcall RGE_Action_Object__stop_new(RGE_Action_Object* this_, DWORD dummy)
{
    if (!RGE_Action_List__action_stop(this_->actions))           // asm 0x409859-0x409860 (:75)
        RGE_Action_List__delete_list(this_->actions);            // asm 0x409868 (:62)
}

// Replacement for RGE_Action_Object::change_ownership. Original:
// @ 0x00409910 (idb_functions.h:146). Range [0x409910, 0x409934) (retn 4
// = 1 stack arg), void. ABI: __thiscall. Virtual: vftable DATA XREF.
// Semantics: hand the unit to a new player: stop the current actions,
// re-bind the base data (owner/sprite/visibility) to the new owner, then
// clear the queue (set_only_action with null).
// Decode (asm 0x409910-0x409933): NAMED virtual dispatch vfptr->stop
// (slot 72 = +0x120; RGE_Action_ObjectVtbl line-arith 11670-11598 = 72;
// asm 0x409915 call [eax+120h]); base RGE_Static_Object__change_ownership
// (:3837 @0x0054FF20 - UNHOOKED, executes the original; asm 0x409922);
// NAMED virtual dispatch vfptr->set_only_action (slot 231 = +0x39C;
// line-arith 11829-11598 = 231; asm 0x40992D call [edx+39Ch], argument 0).
// No jcc in the range - straight-line.
void __fastcall RGE_Action_Object__change_ownership_new(
    RGE_Action_Object* this_, DWORD dummy, RGE_Player* new_owner)
{
    this_->vfptr->stop(this_);                                   // asm 0x409913-0x409915 (slot +0x120)
    RGE_Static_Object__change_ownership((RGE_Static_Object*)this_, new_owner);   // asm 0x409922 - UNHOOKED, original
    this_->vfptr->set_only_action(this_, 0);                     // asm 0x409927-0x40992D (slot +0x39C)
}

// Replacement for RGE_Action_Object::copy_obj. Original: @ 0x00409940
// (idb_functions.h:147). Range [0x409940, 0x40995E) (retn 4 = 1 stack
// arg), void. ABI: __thiscall. Virtual: vftable DATA XREF. Caller:
// RGE_Combat_Object__copy_obj+38 (the runtime ladder).
// Semantics: copy the queued actions' state from a source master's
// templates (each queued action re-binds itself via the list's vtbl
// copy_obj), then the base Moving copy refreshes the master-driven
// fields.
// Decode (asm 0x409940-0x40995D): the LIST's NAMED virtual dispatch
// actions->vfptr->copy_obj (RGE_Action_ListVtbl member 6 = +0x18,
// line-arith 15052-15046 = 6; asm 0x409951 call [eax+18h]) - the slot
// implementation RGE_Action_List__copy_obj (:81 @0x00406EF0 - UNHOOKED)
// forwards source to each queued action's own copy_obj; then base
// RGE_Moving_Object__copy_obj (:2166 @0x004A2EF0 - HOOKED -> routes).
// CAST NOTE (owner-approved 2026-11-22): the list vtbl member declares
// arg 2 RGE_Master_Action_Object* and this site passes the RGE_Master_
// Static_Object*-typed parameter - a base->derived cast grounded by the
// master-tier layout-prefix chain (every tier that can arrive - Action 40
// / Combat_Base 50 / Missile 60 / Combat 70 / Building 80 - carries the
// exact 300-byte RGE_Master_Action_Object prefix, DECOMP_HIERARCHY.md;
// the consumer 0x406EF0 forwards source to the queued actions' copy_obj,
// which read Action-tier task-template fields), and IDA's own rendering
// of this site performs the same cast.
void __fastcall RGE_Action_Object__copy_obj_new(
    RGE_Action_Object* this_, DWORD dummy, RGE_Master_Static_Object* source)
{
    this_->actions->vfptr->copy_obj(this_->actions,
        (RGE_Master_Action_Object*)source);                      // asm 0x40994E-0x409951 (slot +0x18)
    RGE_Moving_Object__copy_obj((RGE_Moving_Object*)this_, source);   // asm 0x409957 - HOOKED, routes
}

// Replacement for RGE_Action_Object::actionState. Original: @ 0x00409B40
// (idb_functions.h:153). Range [0x409B40, 0x409B65) (retn = no stack
// args), unsigned __int8 return (AL). ABI: __thiscall. Virtual: vftable
// DATA XREF.
// Semantics: the current action's state byte, 0 when no action.
// Decode (asm 0x409B40-0x409B64): have_action (:72) with the EXACT == 1
// guard (cmp al,1 / jnz 0x409B50 - a literal transcription, NOT != 0),
// then get_action (:78) and the state byte read DIRECTLY with no null
// guard on the action (have_action()==1 gates it; faithful) - state =
// RGE_Action::state u8 @+0xC = 12 (idb_types.h:15024 region). Exit
// paths: 2 asm returns (al=state @0x409B61; xor al/retn @0x409B62-0x409B65)
// -> the written if/else maps both. loc_409B62: 1 inbound (no join).
unsigned __int8 __fastcall RGE_Action_Object__actionState_new(RGE_Action_Object* this_, DWORD dummy)
{
    if (RGE_Action_List__have_action(this_->actions) == 1)       // asm 0x409B49-0x409B50 (:72)
        return RGE_Action_List__get_action(this_->actions)->state;   // asm 0x409B58-0x409B5D (:78; state @+0xC)
    return 0;                                                    // asm 0x409B62-0x409B65
}

// Replacement for RGE_Action_Object::get_action_checksum. Original:
// @ 0x00409B80 (idb_functions.h:155). Range [0x409B80, 0x409BA8) (retn =
// no stack args), int return. ABI: __thiscall. Virtual: vftable DATA XREF.
// Semantics: a cheap change-detector for the current action: action_type
// + state, 0 when idle.
// Decode (asm 0x409B80-0x409BA7): esi seeded 0 = the shared default; the
// LIST null-checked (0x409B89) and get_action (:78) null-checked
// (0x409B92); success: ecx = state zero-extended (xor ecx,ecx /
// mov cl,[eax+0Ch]), eax = action_type sign-extended (movsx word [eax+4]),
// sum -> eax. Join inventory: loc_409BA5 has 2 inbound (jz 0x409B8B + jz
// 0x409B94); the joined block returns the seeded 0 - both written early
// returns map it. Exit paths: 2 asm returns.
int __fastcall RGE_Action_Object__get_action_checksum_new(RGE_Action_Object* this_, DWORD dummy)
{
    if (!this_->actions)                                         // asm 0x409B89-0x409B8B
        return 0;
    RGE_Action* action = RGE_Action_List__get_action(this_->actions);   // asm 0x409B8D (:78)
    if (!action)                                                 // asm 0x409B92-0x409B94
        return 0;                                                // asm 0x409BA5-0x409BA8
    return action->state + action->action_type;                  // asm 0x409B96-0x409BA4
}

// Replacement for RGE_Action_Object::actionType. Original: @ 0x00409BB0
// (idb_functions.h:156). Range [0x409BB0, 0x409BCF) (retn = no stack
// args), int return. ABI: __thiscall. Virtual: vftable DATA XREF.
// Semantics: the current action's type id, -1 when idle (both the no-list
// and no-action cases return -1).
// Decode (asm 0x409BB0-0x409BCE): the LIST null-checked first (or eax,-1
// @0x409BBA), then get_action (:78) null-checked (or eax,-1 @0x409BC7),
// else action_type (movsx word [eax+4] @0x409BCB). Exit paths: 3 asm
// returns -> the written early return + ternary map all three.
// loc_409BBE / loc_409BCB: 1 inbound each (no joins).
int __fastcall RGE_Action_Object__actionType_new(RGE_Action_Object* this_, DWORD dummy)
{
    if (!this_->actions)                                         // asm 0x409BB6-0x409BB8
        return -1;                                               // asm 0x409BBA-0x409BBD
    RGE_Action* action = RGE_Action_List__get_action(this_->actions);   // asm 0x409BBE (:78)
    return action ? action->action_type : -1;                    // asm 0x409BC3-0x409BCF
}

// Replacement for RGE_Action_Object::hasSubActionOfType. Original:
// @ 0x00409BF0 (idb_functions.h:158). Range [0x409BF0, 0x409C12) (retn 4
// = 1 stack arg), int return. ABI: __thiscall. Virtual: vftable DATA XREF.
// Semantics: does the current action hold a sub-action of the given type.
// Decode (asm 0x409BF0-0x409C11): the LIST null-checked, then
// RGE_Action_List__hasSubActionOfType (:74 @0x00406DE0 - UNHOOKED, int
// return); nonzero result NORMALIZED to 1 (mov eax,1 @0x409C08). Join
// inventory: loc_409C10 has 2 inbound (jz 0x409BF8 + jz 0x409C06); the
// joined block is the shared return-0 exit - covered by the &&
// short-circuit form. Exit paths: 2 asm returns.
int __fastcall RGE_Action_Object__hasSubActionOfType_new(
    RGE_Action_Object* this_, DWORD dummy, int type)
{
    return this_->actions && RGE_Action_List__hasSubActionOfType(this_->actions, type);   // asm 0x409BF6-0x409C0D (:74)
}

// Replacement for RGE_Action_Object::get_next_selected_group. Original:
// @ 0x00409EB0 (idb_functions.h:160). Range [0x409EB0, 0x409ED7) (retn 4
// = 1 stack arg), int return. ABI: __thiscall. Virtual: vftable DATA XREF.
// Semantics: the lowest set control-group bit ABOVE starting_group in the
// Selected_Group_Info bitmask (0 when none): shift the mask right past
// starting_group, then walk up bit by bit.
// Decode (asm 0x409EB0-0x409ED6): result = starting_group + 1; the mask
// SIGN-EXTENDED from the __int16 field (movsx word [ecx+142h] @0x409EB4)
// and shifted ARITHMETICALLY (sar - the C >> on the signed-promoted int;
// the asm truncates the shift count to the low byte via mov cl,al, the
// compiler's own variable-shift form for this same C expression). Loop:
// bit-set -> return result; else shift/++ and repeat until 0.
// Back-edge: jnz loc_409EC4 @0x409ED3 (1 back-edge; .c gotos 0 -
// single-pass). Join inventory: loc_409ED5 (return 0) has 2 inbound (jz
// 0x409EC2 + loop-exit fall-through); locret_409ED7 gets the bit-set jz +
// the xor fall-through - both covered by the written early returns + tail
// return.
int __fastcall RGE_Action_Object__get_next_selected_group_new(
    RGE_Action_Object* this_, DWORD dummy, int starting_group)
{
    int result = starting_group + 1;
    int v3 = this_->Selected_Group_Info >> (starting_group + 1);   // asm 0x409EB4-0x409EC0 (movsx + sar)
    if (!v3)                                                       // asm 0x409EC0-0x409EC2
        return 0;                                                  // asm 0x409ED5-0x409ED7
    while ((v3 & 1) != 1) {                                        // asm 0x409EC4-0x409ECC
        v3 >>= 1;                                                  // asm 0x409ECE
        ++result;                                                  // asm 0x409ED0
        if (!v3)                                                   // asm 0x409ED1-0x409ED3
            return 0;
    }
    return result;                                                 // asm 0x409ECC -> locret_409ED7
}

// Replacement for RGE_Action_Object::get_num_Selected_groups. Original:
// @ 0x00409EE0 (idb_functions.h:161). Range [0x409EE0, 0x409EFE) (retn =
// no stack args), int return. ABI: __thiscall. Virtual: vftable DATA XREF.
// Semantics: how many control-group bits are set in Selected_Group_Info.
// Decode (asm 0x409EE0-0x409EFD): the mask SIGN-EXTENDED (movsx word
// [ecx+142h]) and the loop SIGNED (test/jle @0x409EEB, sar per step) - a
// positive popcount walk (the field is a bitmask; a negative value never
// iterates - faithful). Back-edge: jg loc_409EED @0x409EFC (1 back-edge;
// .c gotos 0 - single-pass). Join inventory: locret_409EFE has 2 inbound
// (jle + loop-exit fall-through); the joined block is the retn - covered
// by the single tail return.
int __fastcall RGE_Action_Object__get_num_Selected_groups_new(RGE_Action_Object* this_, DWORD dummy)
{
    int v1 = this_->Selected_Group_Info;                         // asm 0x409EE0 (movsx word [ecx+142h])
    int result = 0;
    while (v1 > 0) {                                             // asm 0x409EE9-0x409EEB (jle)
        if ((v1 & 1) == 1)                                       // asm 0x409EED-0x409EF5
            ++result;
        v1 >>= 1;                                                // asm 0x409EF8 (sar)
    }
    return result;
}

// ============ RUNTIME METHODS (batch A2 follow-up: the sound pair) ============

// Replacement for RGE_Action_Object::play_command_sound. Original:
// @ 0x00409610 (idb_functions.h:136). Range [0x409610, 0x40961B)
// (retn = no stack args), void. ABI: __thiscall. Virtual: vftable DATA
// XREF (Action/Combat/downstream runtime vftables).
// Semantics: play the master's command (acknowledgement) sound for this
// unit - the unit is passed to the master so the sound is attributed to
// the unit's player/civ (the master implementation dispatches the
// argument's vtbl slot 8 = gbg_get_civ_override and feeds the result to
// RGE_Sound__play as player_id).
// Decode (asm 0x409610-0x40961A): NAMED virtual dispatch through the
// master's vtable slot 17 (+0x44; RGE_Master_Action_ObjectVtbl
// play_command_sound, post-regen line-arith 14874-14857 = 17; base
// RGE_Master_Static_ObjectVtbl :3602, member 17 = 3602-3585 = 17),
// passing THIS RUNTIME OBJECT as the argument - the cast is the VERIFIED
// derived->ancestor direction (every runtime struct carries the exact
// RGE_Static_Object layout prefix), matching the post-regen rendering
// ((RGE_Static_Object *)this). The slot's 2nd parameter was re-typed
// RGE_Player* -> RGE_Static_Object* in the owner's 2026-11-22 IDB
// regeneration (Information Request resolved; master impl 0x00488480
// now renders obj->vfptr->gbg_get_civ_override(obj)).
void __fastcall RGE_Action_Object__play_command_sound_new(RGE_Action_Object* this_, DWORD dummy)
{
    this_->master_obj->vfptr->play_command_sound(
        this_->master_obj, (RGE_Static_Object*)this_);           // asm 0x409613-0x409618 (slot +0x44)
}

// Replacement for RGE_Action_Object::play_move_sound. Original:
// @ 0x00409620 (idb_functions.h:137). Range [0x409620, 0x40962B)
// (retn = no stack args), void. ABI: __thiscall. Virtual: vftable DATA
// XREF.
// Twin of play_command_sound: master vtable slot 18 (+0x48; 14875-14857 =
// 18, :3603 member 18 = 3603-3585 = 18). Master impl 0x004884D0
// (idb_functions.h:1807), move_sound @+0x124.
void __fastcall RGE_Action_Object__play_move_sound_new(RGE_Action_Object* this_, DWORD dummy)
{
    this_->master_obj->vfptr->play_move_sound(
        this_->master_obj, (RGE_Static_Object*)this_);           // asm 0x409623-0x409628 (slot +0x48)
}

// ============ RUNTIME METHODS (batch A3) ============

// Replacement for RGE_Action_Object::setup. Original: @ 0x00408DE0
// (idb_functions.h:124). Range [0x408DE0, 0x408E34) (retn 14h = 5 stack
// args), returns 1. ABI: __thiscall. Callers: RGE_Combat_Object__RGE_
// Combat_Object+A5 and RGE_Combat_Object__setup+5B (the runtime ladder
// calls UP into this).
// Semantics: the Action-tier template setup - run the Moving setup, then
// (re)init the Action extension: the runtime tier id, a fresh empty
// action list, and the command/group state.
// Decode (asm 0x408DE0-0x408E33): base RGE_Moving_Object__setup (:2127 -
// HOOKED -> routes per the hook-routing ruling), then: type @+0x5E = 40
// (0x28), waiting @+0x140 = 0, actions @+0x13C = vfptr->create_action_
// list (slot 236 = +0x3B0, line-arith 11834-11598 = 236; asm 0x408E12
// call [edx+3B0h]), command_flag @+0x141 = 0, Selected_Group_Info
// @+0x142 (word) = 0, return 1. Store order replicated from the asm.
int __fastcall RGE_Action_Object__setup_new(
    RGE_Action_Object* this_, DWORD dummy,
    RGE_Master_Action_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z)
{
    RGE_Moving_Object__setup((RGE_Moving_Object*)this_,
        (RGE_Master_Moving_Object*)tobj, obj_owner, x, y, z);    // asm 0x408DFE - HOOKED, routes
    this_->type = 40;                                            // asm 0x408E07
    this_->waiting = 0;                                          // asm 0x408E0B
    this_->actions = this_->vfptr->create_action_list(this_);    // asm 0x408E12 (slot +0x3B0)
    this_->command_flag = 0;                                     // asm 0x408E1E
    this_->Selected_Group_Info = 0;                              // asm 0x408E25 (word)
    return 1;                                                    // asm 0x408E2E
}

// Replacement for RGE_Action_Object::create_action_list. Original:
// @ 0x00408EB0 (idb_functions.h:126). Range [0x408EB0, 0x408F0C) (retn =
// no stack args), returns the new RGE_Action_List*. ABI: __thiscall.
// Virtual: vftable DATA XREF; dispatched internally by the ctor / setup /
// setup_read (slot +0x3B0).
// Semantics: allocate the unit's action list and construct it bound to
// this unit.
// ALLOCATION VERIFIED: sizeof(RGE_Action_List) = 12 = 0xC (hand-count
// idb_types.h:11587-11591: vfptr@0, obj@4, list@8; matches
// operator_new(0xC) at asm 0x408EC9). SEH fold: chunk 0x640A90 =
// mov eax,[ebp-10h] / push / call operator_delete - a throw during the
// list construction frees the raw allocation (the make_new_obj fold
// precedent); folded below. Null allocation -> return 0 (asm
// loc_408EFB).
RGE_Action_List* __fastcall RGE_Action_Object__create_action_list_new(
    RGE_Action_Object* this_, DWORD dummy)
{
    RGE_Action_List* list = (RGE_Action_List*)operator_new_internal(sizeof(RGE_Action_List));   // asm 0x408ECB (0xC)
    __try
    {
        if (list)
            RGE_Action_List__RGE_Action_List(list, this_);           // asm 0x408EE6 (:56)
    }
    __except (operator_delete_internal(list), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x640A90
    {
    }
    return list;                                                 // asm 0x408EFA / loc_408EFB
}

// Replacement for RGE_Action_Object::save. Original: @ 0x00408F30
// (idb_functions.h:128). Range [0x408F30, 0x408F79) (retn 4 = 1 stack
// arg), void. ABI: __thiscall. Virtual: vftable DATA XREF. Caller:
// RGE_Combat_Object__save+9 (the runtime ladder).
// Semantics: append the Action extension to the unit save stream: the
// waiting/command flags and the selected-group bitmask, then the action
// list's own save.
// Decode (asm 0x408F30-0x408F78): base RGE_Moving_Object__save (:2129 -
// HOOKED -> routes), then rge_write (:3005) x3 - widths 1/1/2 =
// sizeof(targets) (waiting u8 @+0x140, command_flag u8 @+0x141,
// Selected_Group_Info i16 @+0x142), then RGE_Action_List__save (:61
// @0x00406480 - UNHOOKED) on actions. Straight-line, no jcc.
void __fastcall RGE_Action_Object__save_new(RGE_Action_Object* this_, DWORD dummy, int outfile)
{
    RGE_Moving_Object__save((RGE_Moving_Object*)this_, outfile);   // asm 0x408F39 - HOOKED, routes
    rge_write(outfile, &this_->waiting, sizeof(this_->waiting));                         // asm 0x408F48 (1)
    rge_write(outfile, &this_->command_flag, sizeof(this_->command_flag));               // asm 0x408F57 (1)
    rge_write(outfile, &this_->Selected_Group_Info, sizeof(this_->Selected_Group_Info)); // asm 0x408F66 (2)
    RGE_Action_List__save(this_->actions, outfile);                // asm 0x408F72 (:61)
}

// Replacement for RGE_Action_Object::set_task. Original: @ 0x00409720
// (idb_functions.h:139). Range [0x409720, 0x409779) (retn 4 = 1 stack
// arg), void. ABI: __thiscall. Callers: the TRIBE_Building runtime ctors
// (+1AA/+1A6 - the construction-completion default task) + vftable DATA
// XREF.
// Semantics: queue the master task with the given id as the unit's ONLY
// action - unless an action of that task's type is already queued.
// Decode (asm 0x409720-0x409778): RGE_Master_Action_Object__get_task
// (:1798, __int16 task_id; UNHOOKED); guard task != 0; have_action2 (:73)
// with the TASK's action_type (RGE_Task::action_type i16 @+6,
// idb_types.h:6593 - movsx @0x409737; distinct struct from RGE_Action);
// if absent: the LIST's NAMED vtbl dispatch create_task_action
// (RGE_Action_ListVtbl member 2 = +0x8, line-arith 15048-15046 = 2; asm
// 0x409765 call [edx+8]) with (task, 0, -1.0f, -1.0f, -1.0f, 0) - the
// .c's -1082130432 dints are the float -1.0 bit patterns (asm push -1.0
// x3, dd-constant rule); if created: the set_only_action named dispatch
// (slot 231 = +0x39C; asm 0x409771). RESTARTED from the regenerated
// files 2026-11-22 (IR#3: the create_task_action member now carries the
// trailing int - idb_types.h:15048/:20880; the regenerated renderings
// match the recorded asm decode). Join inventory: loc_409777 has 3
// inbound (task-null, have_action2 jnz, create-null); the joined block
// is only the epilogue - covered by the guard cascade + implicit return.
// The asm's eax=0 reuse for the null obj and the trailing 0 int is
// compiler scheduling - the written literals are the same values.
void __fastcall RGE_Action_Object__set_task_new(RGE_Action_Object* this_, DWORD dummy, __int16 task_id)
{
    RGE_Task* task = RGE_Master_Action_Object__get_task(this_->master_obj, task_id);   // asm 0x40972C (:1798; master_obj @+0x14)
    if (task && !RGE_Action_List__have_action2(this_->actions, task->action_type))     // asm 0x409733-0x409749 (:73)
    {
        RGE_Action* action = this_->actions->vfptr->create_task_action(   // asm 0x409765 (slot +0x8)
            this_->actions, task, 0, -1.0f, -1.0f, -1.0f, 0);
        if (action)                                                     // asm 0x409768-0x40976A
            this_->vfptr->set_only_action(this_, action);               // asm 0x409771 (slot +0x39C)
    }
}

// Replacement for RGE_Action_Object::setTaskByTaskID. Original:
// @ 0x00409780 (idb_functions.h:140). Range [0x409780, 0x4097D9) (retn 4
// = 1 stack arg), void. ABI: __thiscall. Virtual: vftable DATA XREF.
// Twin of set_task: the task comes from RGE_Master_Action_Object__
// getTaskByTaskID (:1799, int t; UNHOOKED) and the duplicate check uses
// the RAW t (asm push ebx - full int, no narrowing), otherwise identical
// (create_task_action slot +0x8 with the same argument set; the asm uses
// explicit push 0 for both the null obj and the trailing int; set_only_
// action slot 231). Restarted from the regenerated files (IR#3). Join
// inventory: loc_4097D6, 3 inbound, joined block = the epilogue.
void __fastcall RGE_Action_Object__setTaskByTaskID_new(RGE_Action_Object* this_, DWORD dummy, int t)
{
    RGE_Task* task = RGE_Master_Action_Object__getTaskByTaskID(this_->master_obj, t);   // asm 0x40978D (:1799)
    if (task && !RGE_Action_List__have_action2(this_->actions, t))     // asm 0x409794-0x4097A6 (:73)
    {
        RGE_Action* action = this_->actions->vfptr->create_task_action(   // asm 0x4097C4 (slot +0x8)
            this_->actions, task, 0, -1.0f, -1.0f, -1.0f, 0);
        if (action)                                                     // asm 0x4097C7-0x4097C9
            this_->vfptr->set_only_action(this_, action);               // asm 0x4097D0 (slot +0x39C)
    }
}

// ============ RUNTIME METHODS (batch A4) ============

// Replacement for RGE_Action_Object::setup_read. Original: @ 0x00408E40
// (idb_functions.h:125). Range [0x408E40, 0x408EA3) (retn 8 = 2 stack
// args), returns 1. ABI: __thiscall. Virtual: vftable DATA XREF. Caller:
// RGE_Combat_Object__setup_read+38 (the runtime ladder).
// Semantics: the load-path twin of setup - run the Moving setup_read,
// then stream the Action extension (the flags/group word), then a fresh
// action list whose contents are loaded from the same stream.
// Decode (asm 0x408E40-0x408EA2): base RGE_Moving_Object__setup_read
// (:2128 - HOOKED -> routes); type @+0x5E = 40 (0x28); rge_read (:3003)
// x3 sizeof-exact 1/1/2 -> waiting @+0x140 / command_flag @+0x141 /
// Selected_Group_Info @+0x142; the create_action_list NAMED dispatch
// (slot 236 = +0x3B0, line-arith 11834-11598 = 236; asm 0x408E88 call
// [edx+3B0h]) -> actions @+0x13C; RGE_Action_List__load (:59
// @0x004063B0 - UNHOOKED). Single exit, return 1.
int __fastcall RGE_Action_Object__setup_read_new(
    RGE_Action_Object* this_, DWORD dummy, int infile, RGE_Game_World* gworld)
{
    RGE_Moving_Object__setup_read((RGE_Moving_Object*)this_, infile, gworld);   // asm 0x408E4E (:2128) - HOOKED, routes
    this_->type = 40;                                                            // asm 0x408E5D
    rge_read(infile, &this_->waiting, sizeof(this_->waiting));                   // asm 0x408E61 (1)
    rge_read(infile, &this_->command_flag, sizeof(this_->command_flag));         // asm 0x408E70 (1)
    rge_read(infile, &this_->Selected_Group_Info, sizeof(this_->Selected_Group_Info));   // asm 0x408E7F (2)
    this_->actions = this_->vfptr->create_action_list(this_);                    // asm 0x408E88 (slot +0x3B0)
    RGE_Action_List__load(this_->actions, infile);                               // asm 0x408E97 (:59)
    return 1;                                                                    // asm 0x408E9D
}

// Replacement for RGE_Action_Object::update. Original: @ 0x00409870
// (idb_functions.h:145). Range [0x409870, 0x40990D) (retn = no stack
// args), unsigned __int8 return. ABI: __thiscall. Virtual: vftable DATA
// XREF. Caller: RGE_Combat_Object__update+50 (the runtime ladder).
// Semantics: the per-tick action driver: run the movement update; when
// the unit is NOT placed (state <= 2) drive the action list (or auto-queue
// the master's default task for placed-idle state 2); when placed with a
// stale list, drop it.
// Decode (asm 0x409870-0x40990C): base RGE_Moving_Object__update (:2142 -
// HOOKED -> routes) -> result; object_state @+0x58 <= 2?
//  > 2: actions->list @+8 (RGE_Action_List: vfptr/obj/list, :11587-11591)
//       non-null -> RGE_Action_List__delete_list (:62) [asm 0x40988F];
//  <= 2, list non-null: inside_obj @+0x2C non-null -> the inside_obj_
//       update NAMED dispatch (list vtbl member 3 = +0xC, line-arith
//       15049-15046 = 3; asm 0x4098DB call [eax+0Ch]) + early return;
//       else the update NAMED dispatch (member 4 = +0x10, 15050-15046 = 4;
//       asm 0x4098EC call [edx+10h]) -> result 3 or 1 ->
//       RGE_Action_List__remove_action (:69 @0x00406CC0 - UNHOOKED)
//       [asm 0x4098FD];
//  <= 2, list null, state == 2: master default_task @+0x108 (word,
//       zero-extended load asm 0x4098AE) != 0xFFFF -> the set_task NAMED
//       dispatch (runtime slot 228 = +0x390, line-arith 11826-11598 = 228;
//       asm 0x4098C0 call [edx+390h]) + early return.
// Every exit path runs RGE_Moving_Object__updateEnd (:2143 - HOOKED ->
// routes; asm 0x409896/0x4098C8/0x4098E0/loc_409902) and returns the
// movement result. Exit-path mapping: 4 asm return sites (0x40989D/
// 0x4098D1/0x4098E9/0x40990D) -> the written two arm returns + the shared
// tail (the >2 delete_list path falls to the shared tail - the identical
// delete_list/updateEnd/return sequence).
unsigned __int8 __fastcall RGE_Action_Object__update_new(RGE_Action_Object* this_, DWORD dummy)
{
    unsigned __int8 result = RGE_Moving_Object__update((RGE_Moving_Object*)this_);   // asm 0x409874 (:2142) - HOOKED, routes
    RGE_Action_List* actions = this_->actions;                                       // asm 0x409879
    unsigned __int8 state = this_->object_state;                                     // asm 0x409881 (@+0x58)
    if (state <= 2)
    {
        if (actions->list)
        {
            if (this_->inside_obj)
            {
                actions->vfptr->inside_obj_update(actions);                          // asm 0x4098DB (slot +0xC)
                RGE_Moving_Object__updateEnd((RGE_Moving_Object*)this_);             // asm 0x4098E0
                return result;
            }
            unsigned __int8 r = actions->vfptr->update(actions);                     // asm 0x4098EC (slot +0x10)
            if (r == RGE_ACTION_STATE_SEARCH || r == RGE_ACTION_STATE_DONE)          // asm 0x4098EF-0x4098F5 (cmp al,3 / cmp al,1; the list update propagates the action's state)
                RGE_Action_List__remove_action(this_->actions);                      // asm 0x4098FD (:69)
        }
        else if (state == 2)
        {
            __int16 default_task = this_->master_obj->default_task;                  // asm 0x4098AE (word @+0x108)
            if (default_task != -1)                                                  // asm 0x4098B5-0x4098B9
            {
                this_->vfptr->set_task(this_, default_task);                         // asm 0x4098C0 (slot +0x390)
                RGE_Moving_Object__updateEnd((RGE_Moving_Object*)this_);             // asm 0x4098C8
                return result;
            }
        }
    }
    else if (actions->list)
    {
        RGE_Action_List__delete_list(actions);                                       // asm 0x40988F (:62)
    }
    RGE_Moving_Object__updateEnd((RGE_Moving_Object*)this_);                         // asm loc_409902
    return result;                                                                   // asm 0x409909-0x40990D
}

// Replacement for RGE_Action_Object::is_idle. Original: @ 0x004099B0
// (idb_functions.h:150). Range [0x4099B0, 0x409A3F) (retn = no stack
// args), unsigned __int8 return. ABI: __thiscall. Virtual: vftable DATA
// XREF.
// Semantics: is the unit idle - grouped units are idle when their group's
// current command is -1; otherwise idle means no action, or an action
// resting in one of the no-op states (0/1/2/13/3/14), or a type-107
// action with no target and a -1 target x (an unanchored placeholder).
// Decode (asm 0x4099B0-0x409A3E): mGroupID @+0x88 != -1 ->
// RGE_Game_World__getUnitGroup (:6099; world = owner->world @+0x8C -
// RGE_Player hand-walk :699-:727 with the u8-checksum alignment pad; the
// walk cross-corroborates master_objects @+0x74) -> non-null group:
// return group->mCurrentCommand == -1 (BUnitGroup::mCurrentCommand
// @+0x33C = 828, hand-walk :3346-:3397 with BPosition=48/BPointLine=68/
// BUnitTypeStatisticManager=444; asm setz 0x4099DB). Else the action
// walk: get_action (:78) -> null: idle; state u8 @+0xC zero-extended ->
// {0,1,2,13,3,14}: idle (the asm jz chain 0x4099F5-0x409A10); type-107
// (word cmp 6Bh @0x409A12) with a null target (get_target_obj named
// dispatch, RGE_ActionVtbl slot 20 = +0x50, asm 0x409A1D) AND targetX ==
// -1.0: idle. THE X87 SITE: RGE_Action__targetX (:186, double return per
// rule (h)) vs flt_6545C8 = dd -1.0 (lst-verified); fcomp / fnstsw ax /
// test ah,40h / jnz -> the taken set {equal, NaN} -> the X87_EQ_NAN macro
// (x87_nan.h :21 - the documented match for this exact idiom). A literal
// `!= -1.0` would compile unordered-TRUE (NaN -> NOT idle) and diverge.
// Join inventory: loc_4099DF (2 inbound), loc_409A3C (8 inbound), loc_
// 409A38 (2 inbound) - all covered by the written guard cascade (the
// asm-shaped early-return form; the .c's nested-default render is IDA's
// restructure of the same cascade).
unsigned __int8 __fastcall RGE_Action_Object__is_idle_new(RGE_Action_Object* this_, DWORD dummy)
{
    if (this_->mGroupID != -1)                                             // asm 0x4099B9-0x4099BC
    {
        BUnitGroup* group = RGE_Game_World__getUnitGroup(
            (RGE_Game_World*)this_->owner->world, this_->mGroupID);        // asm 0x4099C8 (:6099; world @+0x8C)
        if (group)
            return group->mCurrentCommand == -1;                           // asm 0x4099D1-0x4099DB
    }
    RGE_Action* action = RGE_Action_List__get_action(this_->actions);      // asm 0x4099E5 (:78)
    if (!action)
        return 1;                                                          // asm 0x4099EE
    int state = action->state;                                             // asm 0x4099F0-0x4099F2 (u8 zero-extended)
    if (state == RGE_ACTION_STATE_NONE || state == RGE_ACTION_STATE_DONE
        || state == RGE_ACTION_STATE_WAIT || state == RGE_ACTION_STATE_FAILED
        || state == RGE_ACTION_STATE_SEARCH || state == RGE_ACTION_STATE_INVALIDATED)
        return 1;                                                          // asm 0x4099F5-0x409A10
    if (action->action_type != RGE_ACTION_TYPE_ARTIFACT)            // asm 0x409A12 (word cmp 6Bh)
        return 0;                                                          // asm loc_409A38
    if (action->vfptr->get_target_obj(action) != 0)                        // asm 0x409A1D (slot +0x50)
        return 0;
    if (X87_EQ_NAN(RGE_Action__targetX(action), -1.0f))                    // asm 0x409A26-0x409A36 (fcomp flt_6545C8; test ah,40h)
        return 1;
    return 0;                                                              // asm loc_409A38
}

extern bool __stdcall is_class_melee_range_convertable(short c);

// ============ RUNTIME METHODS (batch A5) ============

// Replacement for RGE_Action_Object::actionTypeIfOrdered. Original:
// @ 0x00409010 (idb_functions.h:132). Range [0x409010, 0x40915B) (retn
// 14h = 5 stack args), int return. ABI: __thiscall. Virtual: vftable DATA
// XREF.
// Semantics: the "what action type would this order produce" probe used
// by the command layer - resolves the master task for the order (or a
// task_by_group sibling master's task) and returns its action type, with
// the produced attack RANGE written to the out-param.
// Decode (asm 0x409010-0x40915B): *range = 0.0 (asm 0x409020); obj ==
// this -> return -1; the task-list get_target_task NAMED dispatch
// (RGE_Task_ListVtbl member 1 = +0x4, line-arith 6580-6579 = 1; asm
// 0x40904D call [edx+4]) with (-1, 0) = the wildcard form; no task ->
// the task_by_group sibling-master loop (task_by_group @+0x11C = 284;
// master_object_num @+0x70 / master_objects @+0x74 on the player, both
// walk-corroborated; the candidate gates: object_group word @+0x1E,
// master_type byte @+4, task_by_group byte @+0x11C; the candidate's own
// task list @+0x104 = 260, second get_target_task dispatch asm 0x4090C6)
// - loop exhausted -> return -1; task found -> asm 0x4090E4.
// BAKED PATCHES (both mandatory-folded per protocol step 3b, call sites
// retired in place):
// (1) conversion.cpp:167 @0x00409104 - the group gate becomes
//     is_class_melee_range_convertable(group) = {9,10,18} always + 57
//     at current_loaded_version >= 6 (conversion.cpp:105-118),
//     replacing the original {18,9,10}; // baked: conversion.cpp:167
//     @ 0x00409104.
// (2) buildingconv.cpp:48 @0x40912D - the zero-range block body
//     becomes: ordered attributes[28] > 0 -> *range = 1.0f, else
//     *range = attributes[28] * *range (the stub branches on the
//     surviving test ah,41h flags; the literal > compiles unordered-
//     false so NaN takes the multiply arm - matching the stub);
//     replaces the original `*range = 0.5f` block;
//     // baked: buildingconv.cpp:48 @ 0x0040912D.
// The task's action_type (__int16 @+6, RGE_Task - a distinct struct
// from RGE_Action, hand-walk :6590-:6608) sign-extended (movsx asm
// 0x409149); the non-104 path writes RGE_Task::work_range float @+0x24
// = 36 to *range (asm 0x409144; the hand-walk was /Fa-corrected -
// work_val2@32 was a phantom-pad miscount). Exit paths: 3 asm return
// sites (0x40913D folded block, 0x409151 plain-104, 0x40915B -1) ->
// the written returns map all three.
int __fastcall RGE_Action_Object__actionTypeIfOrdered_new(
    RGE_Action_Object* this_, DWORD dummy, RGE_Static_Object* obj,
    float x, float y, float z, float* range)
{
    *range = 0.0f;                                                  // asm 0x409020
    if (obj == (RGE_Static_Object*)this_)                           // asm 0x40901D-0x409026
        return -1;
    RGE_Task* task = this_->master_obj->tasks->vfptr->get_target_task(   // asm 0x40904D (slot +0x4)
        this_->master_obj->tasks, this_, this_->master_obj,
        obj, x, y, z, -1, 0);
    if (!task)
    {
        if (this_->master_obj->task_by_group != 0)                  // asm 0x40905D-0x409065 (@+0x11C)
        {
            TRIBE_Player* owner = this_->owner;
            for (int i = 0; i < owner->master_object_num; ++i)      // asm 0x40906E-0x4090DE
            {
                // base-pointer re-type: the .c's v11[1].los = +260 = tasks,
                // [eax+11Ch] = +284 = task_by_group - the Action-tier fields
                // through the RGE_Master_Static_Object*-typed array element
                RGE_Master_Action_Object* candidate = (RGE_Master_Action_Object*)owner->master_objects[i];
                if (!candidate)
                    continue;                                       // asm 0x409087
                if (candidate->object_group != this_->master_obj->object_group
                    || candidate->master_type != this_->master_obj->master_type
                    || candidate->task_by_group != this_->master_obj->task_by_group)
                    continue;                                       // asm 0x4090CF
                task = candidate->tasks->vfptr->get_target_task(    // asm 0x4090C6 (slot +0x4; tasks @+0x104)
                    candidate->tasks, this_, this_->master_obj,
                    obj, x, y, z, -1, 0);
                if (task)
                    break;
            }
            if (!task)
                return -1;                                          // asm 0x4090E2
        }
        else
            return -1;                                              // asm 0x409154
    }
    if (task->action_type == RGE_ACTION_TYPE_CONVERT)               // asm 0x4090E4-0x4090E9 (word cmp 68h; Convert)
    {
        *range = obj->owner->attributes[5];                         // asm 0x4090EB-0x4090FB (attributes @+0xAC, [5] = +0x14)
        if (is_class_melee_range_convertable(obj->master_obj->object_group))   // baked: conversion.cpp:167 @ 0x00409104 ({9,10,18} + 57 @v6+, replacing {18,9,10})
        {
            if (obj->owner->attributes[28] > 0.0f)                  // baked: buildingconv.cpp:48 @ 0x0040912D (the stub's ordered-> branch on the surviving test ah,41h flags)
                *range = 1.0f;                                      // asm stub 3F000000h
            else
                *range = obj->owner->attributes[28] * *range;       // asm stub fld [eax+70h] / fmul [edx] / fstp [edx]
        }
    }
    else
    {
        *range = task->work_range;                                  // asm 0x409140-0x409147 (float @+0x24 = 36; /Fa-corrected from work_val2)
    }
    return task->action_type;                                       // asm 0x409149 (movsx word @+6)
}

// Replacement for RGE_Action_Object::move_to. Original: @ 0x00409160
// (idb_functions.h:133). Range [0x409160, 0x409265) (retn 10h = 4 stack
// args), unsigned __int8 return. ABI: __thiscall. Virtual: vftable DATA
// XREF. Caller: TRIBE_Combat_Object__move_to+37 (the runtime ladder).
// Semantics: order a move - the list's fast path first (an existing
// move action is re-targeted); else build a Move_To action (target-form
// for an object, position-form for coordinates) and make it the only
// action; command_flag counts the order either way (the .c's ++ outside
// the if; asm loc_40924B).
// ALLOCATION VERIFIED: sizeof(RGE_Action_Move_To) = 68 = 0x44
// (hand-count idb_types.h:20756-20776: the RGE_Action mirror prefix
// 0x40 + float range @64; matches operator_new(0x44) asm 0x4091AF).
// SEH fold: chunk 0x640AB0 = mov eax,[ebp+4] / push / call
// operator_delete (the raw-alloc free on ctor throw; the
// create_action_list fold precedent). The type() NAMED dispatch
// (RGE_ActionVtbl member 6 = +0x18, line-arith 15968-15962 = 6) has a
// RESTRUCTURE note: the original pushes (obj,x,y,z) before the call,
// but the dispatched callee (RGE_Action__type @0x0040AC70) ends in a
// BARE retn - caller-clean, reads only this->action_type - and the 4
// pre-pushed args are setUnitAIAction's args 2-5 staged across the
// call (the push ecx after it completes the 5-arg frame). The written
// named forms build each frame independently: identical callee, values,
// and net stack (proven: the type callee ignores its stack; both
// setUnitAIAction argument sets are the same live variables).
// Join inventory: the shared SEH epilogue loc_409253 (all returns
// funnel; the written returns map the 3 asm value-paths).
unsigned __int8 __fastcall RGE_Action_Object__move_to_new(
    RGE_Action_Object* this_, DWORD dummy, RGE_Static_Object* obj,
    float x, float y, float z)
{
    if (obj != (RGE_Static_Object*)this_)                           // asm 0x40917E-0x409181
    {
        if (RGE_Action_List__action_move_to(this_->actions, obj, x, y, z))   // asm 0x40919D (:76 @0x00406E40 - UNHOOKED)
            return 1;                                               // asm 0x4091A6
        RGE_Action* action = 0;                                     // asm loc_409207 xor eax,eax
        if (obj)
        {
            RGE_Action_Move_To* move = (RGE_Action_Move_To*)operator_new_internal(sizeof(RGE_Action_Move_To));   // asm 0x4091B3 (0x44)
            if (move)
                action = (RGE_Action*)RGE_Action_Move_To__RGE_Action_Move_To2(move,   // asm 0x4091D3 (:101)
                    this_, obj, 0.0f, 0);
        }
        else
        {
            RGE_Action_Move_To* move = (RGE_Action_Move_To*)operator_new_internal(sizeof(RGE_Action_Move_To));   // asm 0x4091DA (0x44)
            if (move)
                action = (RGE_Action*)RGE_Action_Move_To__RGE_Action_Move_To(move,    // asm 0x409200 (:100)
                    this_, x, y, z, 0.0f, 0);
        }
        if (action)
        {
            __int16 action_type = action->vfptr->type(action);      // asm 0x40922D (slot +0x18; RESTRUCTURE note above)
            this_->vfptr->setUnitAIAction(this_, action_type, obj, x, y, z);   // asm 0x40923A (slot 234 = +0x3A8; the 4 pre-staged args)
            this_->vfptr->set_only_action(this_, action);           // asm 0x409245 (slot 231 = +0x39C)
        }
        ++this_->command_flag;                                      // asm 0x40924B (byte @+0x141)
        return 0;                                                   // asm loc_409251
    }
    return 0;                                                       // asm 0x409251-0x409253
}

// Replacement for RGE_Action_Object::get_command_master. Original:
// @ 0x00409630 (idb_functions.h:138). Range [0x409630, 0x409710)
// (retn 10h = 4 stack args), RGE_Master_Static_Object* return. ABI:
// __thiscall. Virtual: vftable DATA XREF.
// Semantics: which master's task list governs this order - the unit's
// own master, unless the unit is task_by_group-tier and its own task
// list has no task for the order, in which case the first task_by_group
// sibling master whose list resolves one.
// Decode (asm 0x409630-0x40970F): obj == this -> own master; task_by_
// group gate (@+0x11C); the first get_target_task NAMED dispatch (slot
// +0x4, asm 0x409671) with (-1, 0); found -> return master_obj;
// else the sibling-master loop (the same triple gate; the candidate's
// tasks @+0x104, second dispatch asm 0x4096E8) -> found -> return the
// candidate (asm 0x40970A); exhausted -> own master. Join inventory:
// loc_409700 (4 inbound) and loc_409703 (2 inbound) - both are return
// paths covered by the written returns.
RGE_Master_Static_Object* __fastcall RGE_Action_Object__get_command_master_new(
    RGE_Action_Object* this_, DWORD dummy, RGE_Static_Object* obj,
    float x, float y, float z)
{
    if (obj != (RGE_Static_Object*)this_)                           // asm 0x40963A-0x40963C
    {
        if (this_->master_obj->task_by_group != 0)                  // asm 0x409645-0x40964D
        {
            RGE_Task* task = this_->master_obj->tasks->vfptr->get_target_task(   // asm 0x409671 (slot +0x4)
                this_->master_obj->tasks, this_, this_->master_obj,
                obj, x, y, z, -1, 0);
            if (task)
                return (RGE_Master_Static_Object*)this_->master_obj;   // asm 0x409703
            TRIBE_Player* owner = this_->owner;
            for (int i = 0; i < owner->master_object_num; ++i)      // asm 0x409692-0x4096FE
            {
                // base-pointer re-type: the .c's v12[1].los = +260 = tasks,
                // [esi+11Ch] = +284 = task_by_group
                RGE_Master_Action_Object* candidate = (RGE_Master_Action_Object*)owner->master_objects[i];
                if (!candidate)
                    continue;                                       // asm 0x4096A1
                if (candidate->object_group != this_->master_obj->object_group
                    || candidate->master_type != this_->master_obj->master_type
                    || candidate->task_by_group != this_->master_obj->task_by_group)
                    continue;                                       // asm 0x4096F3
                task = candidate->tasks->vfptr->get_target_task(    // asm 0x4096E8 (slot +0x4)
                    candidate->tasks, this_, this_->master_obj,
                    obj, x, y, z, -1, 0);
                if (task)
                    return (RGE_Master_Static_Object*)candidate;    // asm 0x40970A
            }
        }
    }
    return (RGE_Master_Static_Object*)this_->master_obj;            // asm loc_409700-0x409703
}

// Replacement for RGE_Action_Object::getTask. Original: @ 0x00409A60
// (idb_functions.h:152). Range [0x409A60, 0x409B3B) (retn 14h = 5 stack
// args), RGE_Task* return. ABI: __thiscall. Virtual: vftable DATA XREF.
// Semantics: resolve the TASK for an order - the own task list first
// (honoring the caller's certain_action filter), else a task_by_group
// sibling master's list (wildcard); a sibling hit TRANSFORMS the unit
// onto that sibling master first.
// Decode (asm 0x409A60-0x409B3A): the first get_target_task NAMED
// dispatch (slot +0x4, asm 0x409A8E) with the CALLER'S certain_action
// (word param @+0x14, pushed as a dword slot; the callee's slot is
// __int16 - only the low word is consumed) and alt_task = 0; found ->
// return it; the task_by_group sibling loop (the triple gate; the
// candidate's tasks @+0x104, second dispatch asm 0x409B07 with -1/0);
// a sibling hit -> the transform NAMED dispatch (runtime slot 51 =
// +0xCC, line-arith 11649-11598 = 51; asm 0x409B2F call [eax+0CCh],
// argument = the candidate master) -> return the task; exhausted ->
// return 0. NOTE: the asm reuses the certain_action param slot as the
// loop counter (asm 0x409AB1/0x409B18 - the param-slot-reuse class) -
// the written form uses a dedicated loop local. Join inventory: loc_
// 409B35 (4 inbound - all early-exit paths), loc_409B14 (3 inbound) -
// covered by the written returns/continue.
RGE_Task* __fastcall RGE_Action_Object__getTask_new(
    RGE_Action_Object* this_, DWORD dummy, RGE_Static_Object* target,
    float targetX, float targetY, float targetZ, __int16 certain_action)
{
    RGE_Task* task = this_->master_obj->tasks->vfptr->get_target_task(   // asm 0x409A8E (slot +0x4)
        this_->master_obj->tasks, this_, this_->master_obj,
        target, targetX, targetY, targetZ, certain_action, 0);
    if (task)
        return task;                                                // asm 0x409A95
    if (this_->master_obj->task_by_group == 0)                      // asm 0x409A9E-0x409AA6
        return 0;
    TRIBE_Player* owner = this_->owner;
    for (int i = 0; i < owner->master_object_num; ++i)              // asm 0x409AB5-0x409B1F
    {
        // base-pointer re-type: the .c's v10[1].los = +260 = tasks,
        // [edi+11Ch] = +284 = task_by_group
        RGE_Master_Action_Object* candidate = (RGE_Master_Action_Object*)owner->master_objects[i];
        if (!candidate)
            continue;                                               // asm 0x409AC4
        if (candidate->object_group != this_->master_obj->object_group
            || candidate->master_type != this_->master_obj->master_type
            || candidate->task_by_group != this_->master_obj->task_by_group)
            continue;                                               // asm 0x409B14
        task = candidate->tasks->vfptr->get_target_task(            // asm 0x409B07 (slot +0x4)
            candidate->tasks, this_, this_->master_obj,
            target, targetX, targetY, targetZ, -1, 0);
        if (task)
        {
            this_->vfptr->transform(this_,                          // asm 0x409B2F (slot +0xCC)
                (RGE_Master_Static_Object*)candidate);
            return task;
        }
    }
    return 0;                                                       // asm 0x409B23
}

// ============ RUNTIME METHODS (batch A6) ============

// Replacement for RGE_Action_Object::do_command. Original: @ 0x00408FD0
// (idb_functions.h:131). Range [0x408FD0, 0x409002) (retn 10h = 4 stack
// args), void. ABI: __thiscall. Virtual: vftable DATA XREF (Action/
// Combat/downstream vftables).
// Semantics: the plain order entry point - forward the order to work2
// with the neutral argument set (work_flag 0, gather_flag 0, certain_
// action -1) and count it.
// Decode (asm 0x408FD0-0x409001): obj == this guard (cmp/jz 0x408FD9);
// the work2 NAMED dispatch (runtime slot 227 = +0x38C, line-arith
// 11825-11598 = 227 post-IR#2; asm 0x408FF5 call [edx+38Ch]) with
// (obj, x, y, z, 0, 0, -1) - the asm's 7 pushes end push 0FFFFFFFFh
// (0x408FDF), matching the __int16 certain_action slot sign-extended;
// then ++command_flag (byte inc @+0x141, asm 0x4090FB). Join inventory:
// loc_409001 (1 inbound jz - the guard skip) - covered by the guard +
// implicit return.
void __fastcall RGE_Action_Object__do_command_new(
    RGE_Action_Object* this_, DWORD dummy, RGE_Static_Object* obj,
    float x, float y, float z)
{
    if (obj != (RGE_Static_Object*)this_)                           // asm 0x408FD7-0x408FD9
    {
        this_->vfptr->work2(this_, obj, x, y, z, 0, 0, -1);         // asm 0x408FF5 (slot +0x38C)
        ++this_->command_flag;                                      // asm 0x4090FB (byte @+0x141)
    }
}

// Replacement for RGE_Action_Object::work. Original: @ 0x00409270
// (idb_functions.h:134). Range [0x409270, 0x4092A5) (retn 14h = 5 stack
// args), void. ABI: __thiscall. Virtual: vftable DATA XREF.
// Semantics: the work-order entry point - forward to work2 with
// work_flag 1 and the caller's gather_flag (the width-verified raw-
// dword passthrough), certain_action -1; count the order.
// Decode (asm 0x409270-0x4092A4): obj == this guard; the work2 NAMED
// dispatch (slot +0x38C; asm 0x409298 call [edx+38Ch]) with (obj, x,
// y, z, 1, unk, -1) - unk forwarded RAW dword (asm 0x40927B/0x409281;
// the IR#2 width analysis: the 7th param is consumed as a 32-bit
// truthiness test, so the int passthrough is the bit-faithful form);
// ++command_flag (asm 0x40929E). Join inventory: loc_4092A4 (1 inbound
// jz) - covered by the guard + implicit return.
void __fastcall RGE_Action_Object__work_new(
    RGE_Action_Object* this_, DWORD dummy, RGE_Static_Object* obj,
    float x, float y, float z, int unk)
{
    if (obj != (RGE_Static_Object*)this_)                           // asm 0x409277-0x409279
    {
        this_->vfptr->work2(this_, obj, x, y, z, 1, unk, -1);       // asm 0x409298 (slot +0x38C)
        ++this_->command_flag;                                      // asm 0x40929E (byte @+0x141)
    }
}

// ============ RUNTIME METHODS (batch A7) ============

// Replacement for RGE_Action_Object::work2. Original: @ 0x004092B0
// (idb_functions.h:135). Range [0x4092B0, 0x409600) (retn 1Ch = 7 stack
// args), void. ABI: __thiscall. Virtual: vftable DATA XREF. Caller:
// TRIBE_Combat_Object__work2+311 (the runtime ladder) + the work/do_command
// dispatches (slots +0x38C, owned).
// Semantics: the order RESOLVER - find the master task the order maps to
// (own task list honoring certain_action, else a task_by_group sibling),
// transform onto a sibling master when the resolution switches masters,
// then either re-drive the current action, confirm a running attack
// (type 9), create the task's action fresh, or fall back to an
// AI-assisted move. work_flag is UNCONSUMED by this function (the
// callers' 0/1 distinction lives above; no read of the +0x14 slot in
// the entire asm range).
// Signature note: restarted from the regenerated files - the IR#2
// member/extern corrections (work_flag char, gather_flag int,
// certain_action __int16; the old `unk` 8th param is certain_action per
// the width analysis: forwarded into get_target_task's __int16 slot,
// gated (_WORD)==0xFFFF).
// Decode (asm 0x4092B0-0x4095FF):
//  obj == this -> return (0x4092BB).
//  The OWN get_target_task NAMED dispatch (slot +0x4; asm 0x4092E7)
//  with (certain_action, 0); ACCEPTED unless null or action_type == 7
//  (word @+6, cmp 7 asm 0x4092F0 - the Attack task type; the .c
//  v11+6 render).
//  Rejected/null -> the task_by_group sibling loop (the triple gate:
//  group word @+0x1E / master_type byte @+4 / task_by_group byte
//  @+0x11C; the candidate's tasks @+0x104; the second dispatch asm
//  0x409372 forwards certain_action - NOT the -1 wildcard); a hit
//  whose action_type DIFFERS from the own task's (or no own task)
//  -> the transform NAMED dispatch (slot 51 = +0xCC; asm 0x4093A5
//  call [edx+0CCh], argument = the candidate) and the resolved task
//  switches to it.
//  LABEL_18 equivalent: unresolved -> fall back to the own task
//  (asm 0x4093AF-0x4093B3: edi = var_4).
//  The certain_action GATE: no task AND certain_action != 0xFFFF ->
//  return (asm 0x4093BB cmp word ptr, 0FFFFh / jnz loc_4095FB; the
//  16-bit form - the parameter IS __int16).
//  The RE-DRIVE block (asm 0x4093C7-0x409452): RGE_Action_List__get_task
//  (:80 @0x00406E70 - UNHOOKED) -> current; if current == task AND
//  get_action (:78)->action_type == current->action_type AND
//  RGE_Action_List__action_work (:77; asm 0x409411 with obj/x/y/z):
//  get_action -> the type() NAMED dispatch (slot +0x18, member 6) ->
//  the setUnitAIAction NAMED dispatch (slot 234 = +0x3A8; asm 0x409448
//  call [eax+3A8h]) with the full 5-arg form; worked = 1.
//  The ATTACK-CONFIRM block (asm 0x40945A-0x4094FC): have_action == 1
//  (cmp al,1 - the exact == 1 guard) AND get_action()->get_target_obj
//  (slot +0x50) == obj AND get_action()->type == 9 (cmp ax,9) AND
//  get_action()->work (slot +0x34, member 13) with (obj,x/y/z):
//  type() -> setUnitAIAction -> unitAI(this)->autoAttackOrderFlag = 0
//  (RGE_Static_Object__unitAI :261; UnitAIModule::autoAttackOrderFlag
//  byte @+0x191 = 401 - hand-walk :4391-:4444 with ManagedArray_int_
//  = 16 bytes; asm 0x4094F3 mov byte ptr [eax+191h], 0) -> return.
//  The CREATE block (asm 0x4094FF-0x4095B8): worked -> return;
//  no task -> the SPEED FALLBACK; create_task_action (list vtbl member
//  2 = +0x8, IR#3-corrected 6-arg form; asm 0x40952D call [edx+8])
//  with (task, obj, x, y, z, gather_flag) - the 7th arg consumed as
//  the Gather gbg_unknown_1 flag; created ->
//    master object_group == 1 (Herd Animal, word @+0x1E asm 0x40953D)
//    AND hasActionOfType(this, 107) (slot 174 = +0x2B8, line-arith
//    11772-11598 = 174; asm 0x40954A push 6Bh / call [edx+2B8h]):
//    get_action -> RGE_Action__add_sub_action (:191; current_action,
//    action, 1) -> return (asm 0x40956E);
//    else: type() -> setUnitAIAction -> set_only_action (slot 231 =
//    +0x39C) -> unitAI->autoAttackOrderFlag = 0 -> return (asm
//    0x4095B8).
//  The SPEED FALLBACK (asm loc_4095BB): master speed @+0xD0 = 208
//  (fld / fcomp const_f_zero / fnstsw / test ah,41h / jnz -> return):
//  the taken set {>0 ordered} runs the move path - the literal C
//  `speed > 0.0` compiles unordered-false (NaN -> skip = return) =
//  the MATCH row of the polarity table (the maximumSpeed cap-gate
//  precedent; NaN cannot move). setUnitAIAction(this, 1, obj, x, y, z)
//  (the FULL 5-arg form, asm 0x4095E3) + the move_to NAMED dispatch
//  (slot 70 = +0x118, line-arith 11668-11598 = 70; asm 0x4095F5 call
//  [edx+118h]).
// Scheduling RESTRUCTURE (proven, the move_to/A6 precedent): the
// original pre-stages setUnitAIAction's args 2-5 before each type()
// call (the type callees are caller-clean bare-retn - RGE_Action__type
// @0x0040AC70 - and ignore the stack); the written named forms build
// each frame at its own call site. Identical callees, values, net
// stack. The type() member is the 0-arg __int16 form (the .c's 4-arg
// cast-renders are the pre-staging, not the callee contract).
// Exit paths: 6 asm return sites (0x4094FC attack-confirm; 0x40956E
// add_sub_action; 0x4095B8 create-else; 0x4095FF shared; plus the
// obj==this and certain_action-gate early returns mapping to the
// shared epilogue) -> the written returns map all.
void __fastcall RGE_Action_Object__work2_new(
    RGE_Action_Object* this_, DWORD dummy, RGE_Static_Object* obj,
    float x, float y, float z, char work_flag, int gather_flag,
    __int16 certain_action)
{
    if (obj == (RGE_Static_Object*)this_)                           // asm 0x4092BB-0x4092BD
        return;
    RGE_Task* task = this_->master_obj->tasks->vfptr->get_target_task(   // asm 0x4092E7 (slot +0x4)
        this_->master_obj->tasks, this_, this_->master_obj,
        obj, x, y, z, certain_action, 0);
    RGE_Task* own_task = 0;                                         // asm var_4 (the .c v29)
    if (!task || task->action_type == RGE_ACTION_TYPE_ATTACK)       // asm 0x4092EE-0x4092F5 (word @+6 cmp 7; ATTACK)
    {
        own_task = task;                                            // asm 0x4092FE
        if (this_->master_obj->task_by_group != 0)                  // asm 0x409302-0x40930A (@+0x11C)
        {
            TRIBE_Player* owner = this_->owner;
            for (int i = 0; i < owner->master_object_num; ++i)      // asm 0x409313-0x40939C
            {
                // base-pointer re-type: the .c's v10[1].los = +260 = tasks
                RGE_Master_Action_Object* candidate = (RGE_Master_Action_Object*)owner->master_objects[i];
                if (!candidate)
                    continue;                                       // asm 0x40932C
                if (candidate->object_group != this_->master_obj->object_group
                    || candidate->master_type != this_->master_obj->master_type
                    || candidate->task_by_group != this_->master_obj->task_by_group)
                    continue;                                       // asm 0x409391
                task = candidate->tasks->vfptr->get_target_task(    // asm 0x409372 (slot +0x4)
                    candidate->tasks, this_, this_->master_obj,
                    obj, x, y, z, certain_action, 0);
                if (task && (!own_task || own_task->action_type != task->action_type))
                {                                                   // asm 0x409379-0x40938B
                    this_->vfptr->transform(this_,                  // asm 0x4093A5 (slot +0xCC)
                        (RGE_Master_Static_Object*)candidate);
                    break;
                }
            }
        }
    }
    if (!task)
        task = own_task;                                            // asm 0x4093AF-0x4093B3 (LABEL_18)
    if (!task && certain_action != -1)                              // asm 0x4093B7-0x4093C1 (LABEL_20; cmp word, 0FFFFh)
        return;                                                     // asm jnz loc_4095FB
    RGE_Task* current = RGE_Action_List__get_task(this_->actions);  // asm 0x4093CD (:80)
    int worked = 0;                                                 // asm 0x4093D8/0x409452 (the reused slot)
    if (current && task == current
        && RGE_Action_List__get_action(this_->actions)->action_type == current->action_type
        && RGE_Action_List__action_work(this_->actions, obj, x, y, z))   // asm 0x409411 (:77)
    {
        RGE_Action* action = RGE_Action_List__get_action(this_->actions);   // asm 0x409420
        __int16 action_type = action->vfptr->type(action);          // asm 0x40943B (slot +0x18)
        this_->vfptr->setUnitAIAction(this_, action_type, obj, x, y, z);   // asm 0x409448 (slot +0x3A8)
        worked = 1;                                                 // asm 0x409452
    }
    if (RGE_Action_List__have_action(this_->actions) == 1           // asm 0x409460-0x409467 (cmp al,1)
        && RGE_Action_List__get_action(this_->actions)->vfptr->get_target_obj(
               RGE_Action_List__get_action(this_->actions)) == obj  // asm 0x40947C (slot +0x50)
        && RGE_Action_List__get_action(this_->actions)->vfptr->type(
               RGE_Action_List__get_action(this_->actions)) == RGE_ACTION_TYPE_ATTACK2   // asm 0x409492 (slot +0x18; cmp ax,9)
        && RGE_Action_List__get_action(this_->actions)->vfptr->work(
               RGE_Action_List__get_action(this_->actions), obj, x, y, z))   // asm 0x4094B6 (slot +0x34)
    {
        RGE_Action* action = RGE_Action_List__get_action(this_->actions);   // asm 0x4094C3
        __int16 action_type = action->vfptr->type(action);          // asm 0x4094DA (slot +0x18)
        this_->vfptr->setUnitAIAction(this_, action_type, obj, x, y, z);   // asm 0x4094E3 (slot +0x3A8)
        RGE_Static_Object__unitAI((RGE_Static_Object*)this_)->autoAttackOrderFlag = 0;   // asm 0x4094EB/0x4094F3 (:261; byte @+0x191)
        return;                                                     // asm 0x4094FC
    }
    if (!worked)                                                    // asm 0x4094FF-0x409505
    {
        if (task)                                                   // asm 0x40950B-0x40950D
        {
            RGE_Action* action = this_->actions->vfptr->create_task_action(   // asm 0x40952D (slot +0x8; IR#3 form)
                this_->actions, task, obj, x, y, z, gather_flag);
            if (action)
            {
                if (this_->master_obj->object_group == 1            // asm 0x40953D (word @+0x1E; Herd Animal)
                    && this_->vfptr->hasActionOfType(this_, RGE_ACTION_TYPE_ARTIFACT))   // asm 0x40954A (slot +0x2B8; push 6Bh)
                {
                    RGE_Action* current_action = RGE_Action_List__get_action(this_->actions);   // asm 0x40955D
                    RGE_Action__add_sub_action(current_action, action, 1);   // asm 0x409564 (:191)
                    return;                                         // asm 0x40956E
                }
                __int16 action_type = action->vfptr->type(action);  // asm 0x409587 (slot +0x18)
                this_->vfptr->setUnitAIAction(this_, action_type, obj, x, y, z);   // asm 0x409594 (slot +0x3A8)
                this_->vfptr->set_only_action(this_, action);       // asm 0x40959F (slot +0x39C)
                RGE_Static_Object__unitAI((RGE_Static_Object*)this_)->autoAttackOrderFlag = 0;   // asm 0x4095A7/0x4095AF
                return;                                             // asm 0x4095B8
            }
        }
        if (this_->master_obj->speed > 0.0)                         // asm loc_4095BB (test ah,41h / jnz = ordered-> runs; the polarity MATCH row - literal kept)
        {
            this_->vfptr->setUnitAIAction(this_, RGE_ACTION_TYPE_MOVE_TO, obj, x, y, z);   // asm 0x4095E3 (slot +0x3A8; push 1 = Move to; the FULL 5-arg form)
            this_->vfptr->move_to(this_, obj, x, y, z);             // asm 0x4095F5 (slot +0x118)
        }
    }
}

// ============ RUNTIME METHODS (batch A8 - the final Action export) ============

// Replacement for RGE_Action_Object::setUnitAIAction. Original:
// @ 0x00409C20 (idb_functions.h:159). Range [0x409C20, 0x409E00) (retn
// 14h = 5 stack args), void. ABI: __thiscall. Virtual: vftable DATA XREF
// (Action/Combat/downstream vftables; slot 234 = +0x3A8). Callers: the
// owned work2/move_to/A6 dispatches + TRIBE_Combat_Object__
// setUnitAIAction+193 (the runtime ladder).
// Semantics: mirror an order into the unit's AI module - map the action
// type to an (order, action) pair, set the order/action/target on the
// module, and record the order in its history.
// DECODE BASIS (owner-directed 2026-11-22): ASM ONLY - the .c render of
// the addOrderToHistory argument block is unreliable (overlapping
// by-value-BVector locals v10-v16, the (int)v16 artifact).
// Decode (asm 0x409C20-0x409DFF): module = unitAIValue @+0x7C = 124
// (idb_types.h:7478); null -> return (asm 0x409C2E). The switch (asm
// 0x409C3E-0x409C50: dec/cmp 6Eh = 111 cases; the two-level table
// off_409E04 + byte_409E38, both lst-verified) - 12 pairs:
//  1->{710,610} 3->{717,617} 4->{705,605} 5->{709,609} 9->{700,600}
//  12->{714,614} 101->{702,602} 104->{704,604} 105->{703,603}
//  106->{718,618} 110->{713,613} 111->{715,615}; default keeps -1/-1.
//  (asm cites: 0x409C96, 0x409C72, 0x409C7E, 0x409C8A, 0x409C57,
//  0x409CAE, 0x409C66, 0x409CC6, 0x409CD2, 0x409CDE, 0x409CA2,
//  0x409CBA.)
//  UnitAIModule__setCurrentOrder(module, order, 1) (:449; asm 0x409CED)
//  + setCurrentAction(module, action) (:450; asm 0x409CF6).
//  OBJ PATH (asm 0x409CFB-0x409D73): setCurrentTarget2(module, obj->id
//  (@+0x10), movsx group (@+0x1E via master_obj @+0x14), x, y, z)
//  (:503; asm 0x409D21); position(obj, &bufA) (slot 171 = +0x2AC, the
//  M12-verified copy-out getter; asm 0x409D30) -> bufA = the TARGET's
//  position {x, y, z} (the 12-byte temp copy asm 0x409D36-0x409D4A);
//  targetID = obj->id staged by the LINGERING PUSH asm 0x409D4D/
//  0x409D54 (push id before the out-buffer - the position callees pop
//  only the 4-byte buffer, the id push remains as the next call's
//  targetID slot; the .c's (int)v16 was this slot); position(this,
//  &bufB) (asm 0x409D58) -> the unit's OWN position.
//  NO-OBJ PATH (asm 0x409D75-0x409DD9): setCurrentTarget2(module, -1,
//  -1, x, y, z) (asm 0x409D8B); targetPosition = {y, y, y} - the
//  setCurrentTarget2 arg-push leftovers {x@E-52, y@E-48, z@E-44}
//  overwritten by mov [ecx],edx (y, asm 0x409DBD) and mov [ecx+8],edx
//  (y, asm 0x409DC2), the middle slot the untouched y push (mechanical
//  triple-confirmation; the .c's {x, y, z} v13 render is WRONG);
//  targetID = -1 (asm 0x409DBB); position(this, &bufD) (asm 0x409DCC)
//  -> the OWN position {x, y, z}.
//  SHARED TAIL (asm loc_409DE7): UnitAIModule__addOrderToHistory
//  (:534; module, order, action, own_position, target_id,
//  target_position - the by-value BVectors the compiler stages) ->
//  UnitAIModule__setTaskedByPlayer(module) (:508; asm 0x409DF4) ->
//  return. Exit paths: the shared epilogue (loc_409DF9) + the
//  unitAI-null early return - both map to the written return/implicit
//  return.
// Position NOTE: the copy-out getter discipline - each fetch gets its
// own named buffer (the BPath__waypoint rule); the .c's 1-arg
// cast-render at :52477 hid the out-buffer the asm pushes.
void __fastcall RGE_Action_Object__setUnitAIAction_new(
    RGE_Action_Object* this_, DWORD dummy, int actionType,
    RGE_Static_Object* obj, float x, float y, float z)
{
    UnitAIModule* module = this_->unitAIValue;                      // asm 0x409C28 (@+0x7C)
    if (!module)                                                    // asm 0x409C2C-0x409C2E
        return;
    int order = -1;                                                 // asm 0x409C38 (edi)
    int action = -1;                                                // asm 0x409C3B (esi)
    switch (actionType)                                             // asm 0x409C3E-0x409C50 (two-level table)
    {
        case RGE_ACTION_TYPE_MOVE_TO:   action = RGE_UNITAI_ACTION_MOVE;     order = RGE_UNITAI_ORDER_MOVE;     break;   // asm 0x409C96/0x409C9B
        case RGE_ACTION_TYPE_ENTER:     action = RGE_UNITAI_ACTION_ENTER;    order = RGE_UNITAI_ORDER_ENTER;    break;   // asm 0x409C72/0x409C77
        case RGE_ACTION_TYPE_EXPLORE:   action = RGE_UNITAI_ACTION_EXPLORE;  order = RGE_UNITAI_ORDER_EXPLORE;  break;   // asm 0x409C7E/0x409C83
        case RGE_ACTION_TYPE_GATHER:    action = RGE_UNITAI_ACTION_GATHER;   order = RGE_UNITAI_ORDER_GATHER;   break;   // asm 0x409C8A/0x409C8F
        case RGE_ACTION_TYPE_ATTACK2:   action = RGE_UNITAI_ACTION_ATTACK;   order = RGE_UNITAI_ORDER_ATTACK;   break;   // asm 0x409C57/0x409C5C
        case RGE_ACTION_TYPE_TRANSPORT: action = RGE_UNITAI_ACTION_TRANSPORT;order = RGE_UNITAI_ORDER_TRANSPORT;break;   // asm 0x409CAE/0x409CB3
        case RGE_ACTION_TYPE_BUILD:     action = RGE_UNITAI_ACTION_BUILD;    order = RGE_UNITAI_ORDER_BUILD;    break;   // asm 0x409C66/0x409C6B
        case RGE_ACTION_TYPE_CONVERT:   action = RGE_UNITAI_ACTION_CONVERT;  order = RGE_UNITAI_ORDER_CONVERT;  break;   // asm 0x409CC6/0x409CCB
        case RGE_ACTION_TYPE_HEAL:      action = RGE_UNITAI_ACTION_HEAL;     order = RGE_UNITAI_ORDER_HEAL;     break;   // asm 0x409CD2/0x409CD7
        case RGE_ACTION_TYPE_REPAIR:    action = RGE_UNITAI_ACTION_REPAIR;   order = RGE_UNITAI_ORDER_REPAIR;   break;   // asm 0x409CDE/0x409CE3
        case RGE_ACTION_TYPE_HUNT:      action = RGE_UNITAI_ACTION_HUNT;     order = RGE_UNITAI_ORDER_HUNT;     break;   // asm 0x409CA2/0x409CA7
        case RGE_ACTION_TYPE_TRADE:     action = RGE_UNITAI_ACTION_TRADE;    order = RGE_UNITAI_ORDER_TRADE;    break;   // asm 0x409CBA/0x409CBF
        default: break;                                             // asm loc_409CE8
    }
    UnitAIModule__setCurrentOrder(module, order, 1);                // asm 0x409CED (:449)
    UnitAIModule__setCurrentAction(module, action);                 // asm 0x409CF6 (:450)
    BVector own_position;
    BVector target_position;
    int target_id;
    if (obj)
    {
        UnitAIModule__setCurrentTarget2(module, obj->id,            // asm 0x409D21 (:503)
            obj->master_obj->object_group, x, y, z);                // asm 0x409D18 (movsx @+0x1E)
        obj->vfptr->position(obj, &target_position);                // asm 0x409D30 (slot +0x2AC) = tempA
        target_id = obj->id;                                        // asm 0x409D4D (the lingering push)
        this_->vfptr->position(this_, &own_position);               // asm 0x409D58 (slot +0x2AC) = bufB
    }
    else
    {
        UnitAIModule__setCurrentTarget2(module, -1, -1, x, y, z);   // asm 0x409D8B (:503)
        target_position.x = y;                                      // asm 0x409DBD
        target_position.y = y;                                      // asm [E-48] stale y push
        target_position.z = y;                                      // asm 0x409DC2
        target_id = -1;                                             // asm 0x409DBB
        this_->vfptr->position(this_, &own_position);               // asm 0x409DCC (slot +0x2AC) = bufD
    }
    UnitAIModule__addOrderToHistory(module, order, action,          // asm 0x409DEC (:534)
        own_position, target_id, target_position);
    UnitAIModule__setTaskedByPlayer(module);                        // asm 0x409DF4 (:508)
}

void install_RGE_Action_Object()
{
    setHook((void*)0x00408980, RGE_Action_Object__RGE_Action_Object_new);
    setHook((void*)0x00408C50, RGE_Action_Object__vector_deleting_destructor_new);
    setHook((void*)0x00408C70, RGE_Action_Object__RGE_Action_Object_read_new);
    setHook((void*)0x00408D20, RGE_Action_Object__destructor_new);
    setHook((void*)0x00408D90, RGE_Action_Object__recycle_out_of_game_new);
    setHook((void*)0x00408DB0, RGE_Action_Object__recycle_in_to_game_new);
    setHook((void*)0x00408C30, RGE_Action_Object__get_Selected_Groups_new);
    setHook((void*)0x00408C40, RGE_Static_Object__set_Selected_Groups_new);
    setHook((void*)0x00408F10, RGE_Action_Object__rehook_new);
    setHook((void*)0x00408F80, RGE_Action_Object__transform_new);
    setHook((void*)0x00408FA0, RGE_Action_Object__destroy_obj_new);
    setHook((void*)0x004097E0, RGE_Action_Object__set_action_new);
    setHook((void*)0x00409830, RGE_Action_Object__set_end_action_new);
    setHook((void*)0x00409970, RGE_Action_Object__have_action_new);
    setHook((void*)0x00409990, RGE_Action_Object__gameActionType_new);
    setHook((void*)0x00409A40, RGE_Action_Object__get_target_obj_new);
    setHook((void*)0x00409B70, RGE_Action_Object__searchRadius_new);
    setHook((void*)0x00409BD0, RGE_Action_Object__hasActionOfType_new);
    setHook((void*)0x00409800, RGE_Action_Object__set_only_action_new);
    setHook((void*)0x00409850, RGE_Action_Object__stop_new);
    setHook((void*)0x00409910, RGE_Action_Object__change_ownership_new);
    setHook((void*)0x00409940, RGE_Action_Object__copy_obj_new);
    setHook((void*)0x00409B40, RGE_Action_Object__actionState_new);
    setHook((void*)0x00409B80, RGE_Action_Object__get_action_checksum_new);
    setHook((void*)0x00409BB0, RGE_Action_Object__actionType_new);
    setHook((void*)0x00409BF0, RGE_Action_Object__hasSubActionOfType_new);
    setHook((void*)0x00409EB0, RGE_Action_Object__get_next_selected_group_new);
    setHook((void*)0x00409EE0, RGE_Action_Object__get_num_Selected_groups_new);
    setHook((void*)0x00409610, RGE_Action_Object__play_command_sound_new);
    setHook((void*)0x00409620, RGE_Action_Object__play_move_sound_new);
    setHook((void*)0x00408DE0, RGE_Action_Object__setup_new);
    setHook((void*)0x00408EB0, RGE_Action_Object__create_action_list_new);
    setHook((void*)0x00408F30, RGE_Action_Object__save_new);
    setHook((void*)0x00408E40, RGE_Action_Object__setup_read_new);
    setHook((void*)0x00409870, RGE_Action_Object__update_new);
    setHook((void*)0x004099B0, RGE_Action_Object__is_idle_new);
    setHook((void*)0x00409010, RGE_Action_Object__actionTypeIfOrdered_new);
    setHook((void*)0x00409160, RGE_Action_Object__move_to_new);
    setHook((void*)0x00409630, RGE_Action_Object__get_command_master_new);
    setHook((void*)0x00409A60, RGE_Action_Object__getTask_new);
    setHook((void*)0x00409720, RGE_Action_Object__set_task_new);
    setHook((void*)0x00409780, RGE_Action_Object__setTaskByTaskID_new);
    setHook((void*)0x00408FD0, RGE_Action_Object__do_command_new);
    setHook((void*)0x00409270, RGE_Action_Object__work_new);
    setHook((void*)0x004092B0, RGE_Action_Object__work2_new);
    setHook((void*)0x00409C20, RGE_Action_Object__setUnitAIAction_new);
}
