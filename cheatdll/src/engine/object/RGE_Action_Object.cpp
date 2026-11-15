#include "stdafx.h"

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

void install_RGE_Action_Object()
{
    setHook((void*)0x00408980, RGE_Action_Object__RGE_Action_Object_new);
    setHook((void*)0x00408C50, RGE_Action_Object__vector_deleting_destructor_new);
    setHook((void*)0x00408C70, RGE_Action_Object__RGE_Action_Object_read_new);
    setHook((void*)0x00408D20, RGE_Action_Object__destructor_new);
    setHook((void*)0x00408D90, RGE_Action_Object__recycle_out_of_game_new);
    setHook((void*)0x00408DB0, RGE_Action_Object__recycle_in_to_game_new);
}
