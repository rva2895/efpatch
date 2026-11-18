#include "stdafx.h"

// .rdata vftable of RGE_Missile_Object (lst :990078)
static RGE_Missile_ObjectVtbl* const rge_missile_object_vftable =
    (RGE_Missile_ObjectVtbl*)0x0065B808;

// Replacement for RGE_Missile_Object::RGE_Missile_Object (runtime ctor).
// Original: @ 0x0049BA40 (idb_functions.h:2059). Range [0x49BA40,
// 0x49BAE6) (lst :265707/:265785, retn 18h = 6 stack args), returns
// this. ABI: __thiscall - this_ in ECX. Caller: RGE_Master_Missile_
// Object__make_new_obj +9D (owned, hook-routed; lst :265708).
// Decode (asm 0x49BA40-0x49BAE6): base runtime Combat ctor with
// do_setup = 0 (hooked -> routes), the Missile vfptr store, then IF
// do_setup: ghost_mode @+0x180 (byte) = 1 (asm 0x49BAA7), fired_from_id
// @+0x184 = -1 (asm 0x49BAAE), the runtime Combat setup (:1074,
// unhooked), own_master @+0x188 (byte) = 0 (asm 0x49BABF), type @+0x5E
// = 60 (0x3C - stored TWICE inside the branch, asm 0x49BAC6 + 0x49BAC9
// - the original's inner and outer source lines; the else path stores
// it once, asm 0x49BACE). max_range @+0x17C is NOT stored by the ctor
// (setup_read streams it). SEH fold: chunk 0x642550 -> RGE_Combat_
// Object__destructor (0x443B10 - HOOKED -> routes; the IMMEDIATE base
// level - runtime ladder evidence).
RGE_Missile_Object* __fastcall RGE_Missile_Object__RGE_Missile_Object_new(
    RGE_Missile_Object* this_, DWORD dummy,
    RGE_Master_Combat_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z, int do_setup)
{
    RGE_Missile_Object* result = this_;
    __try
    {
        RGE_Combat_Object__RGE_Combat_Object(
            (RGE_Combat_Object*)this_, tobj, obj_owner, x, y, z, 0);
        this_->vfptr = rge_missile_object_vftable;       // asm 0x49BA90
        if (do_setup != 0)
        {
            this_->ghost_mode = 1;                       // asm 0x49BAA7 (byte @+0x180)
            this_->fired_from_id = -1;                   // asm 0x49BAAE (@+0x184)
            RGE_Combat_Object__setup((RGE_Combat_Object*)this_, tobj, obj_owner, x, y, z);   // asm 0x49BAB8 (:1074)
            this_->own_master = 0;                       // asm 0x49BABF (byte @+0x188)
            this_->type = 60;                            // asm 0x49BAC6 (inner store)
        }
        this_->type = 60;                                // asm 0x49BAC9/0x49BACE (outer store, both paths)
        result = this_;
    }
    __except (RGE_Combat_Object__destructor((RGE_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x642550
    {
    }
    return result;
}

// Replacement for RGE_Missile_Object::scalar_deleting_destructor.
// Original: @ 0x0049BAF0 (idb_functions.h:2060). Range [0x49BAF0,
// 0x49BB0B) (lst :265794/:265812, retn 4), void* result. ABI:
// __thiscall. Virtual: vftable slot 0 (lst :265795). Body: the Missile
// destructor (IN BATCH - the direct call routes into
// RGE_Missile_Object__destructor_new per the hook-routing ruling),
// then flags & 1 -> operator_delete, return this. No SEH.
void* __fastcall RGE_Missile_Object__scalar_deleting_destructor_new(
    RGE_Missile_Object* this_, DWORD dummy, unsigned int flags)
{
    RGE_Missile_Object__destructor(this_);
    if (flags & 1)
        operator_delete_internal(this_);
    return this_;
}

// Replacement for RGE_Missile_Object::RGE_Missile_Object_read (runtime
// ctor_read). Original: @ 0x0049BB10 (idb_functions.h:2061). Range
// [0x49BB10, 0x49BB76) (lst :265821/:265873, retn 0Ch = 3 stack args),
// returns this. ABI: __thiscall - this_ in ECX. Callers: RGE_Object_
// List__load +17C and RGE_Player_Object_List__Load +17C (lst :265822/
// :265823).
// Decode (asm 0x49BB10-0x49BB76): base runtime Combat ctor_read with
// do_setup = 0 (hooked -> routes), the Missile vfptr store, IF do_setup
// the derived Missile setup_read (unhooked - it owns the max_range/
// fired_from_id/own_master stream block incl. the own-master
// construction), then type @+0x5E = 60 UNCONDITIONALLY after the guard
// (asm 0x49BB63). SEH fold: chunk 0x642570 -> RGE_Combat_Object__
// destructor (HOOKED, routes).
RGE_Missile_Object* __fastcall RGE_Missile_Object__RGE_Missile_Object_read_new(
    RGE_Missile_Object* this_, DWORD dummy, int infile, RGE_Game_World* gworld, int do_setup)
{
    RGE_Missile_Object* result = this_;
    __try
    {
        RGE_Combat_Object__RGE_Combat_Object_read((RGE_Combat_Object*)this_, infile, gworld, 0);
        this_->vfptr = rge_missile_object_vftable;       // asm 0x49BB4E
        if (do_setup != 0)
            RGE_Missile_Object__setup_read(this_, infile, gworld);   // asm 0x49BB5A
        this_->type = 60;                                // asm 0x49BB63 - unconditional
        result = this_;
    }
    __except (RGE_Combat_Object__destructor((RGE_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x642570
    {
    }
    return result;
}

// Replacement for RGE_Missile_Object::destructor. Original:
// @ 0x0049BB80 (idb_functions.h:2062). Range [0x49BB80, 0x49BBF7)
// (lst :265882/:265933), void, no stack args. ABI: __thiscall. Caller:
// the scalar deleting destructor above (lst :265883).
// Decode (asm 0x49BB80-0x49BBF7): vfptr store, then the OWN-MASTER
// teardown (same shape as TRIBE_Combat's): if own_master @+0x188 != 0:
// own = master_obj @+0x14; own_master = 0 (asm 0x49BBC6); master_obj =
// owner->master_objects[own->id] (asm 0x49BBBC-0x49BBCC); if (own != 0)
// own->vfptr->__vecDelDtor(own, 1) (RGE_Master_Missile vtbl slot 5 =
// +0x14, asm 0x49BBD2-0x49BBD6) - the own master is DELETED. Then the
// runtime Combat destructor (HOOKED -> routes). SEH fold: chunk
// 0x642590 -> RGE_Combat_Object__destructor (HOOKED, routes).
void __fastcall RGE_Missile_Object__destructor_new(RGE_Missile_Object* this_, DWORD dummy)
{
    __try
    {
        this_->vfptr = rge_missile_object_vftable;       // asm 0x49BB9D
        if (this_->own_master != 0)                      // asm 0x49BBA3-0x49BBB1 (byte @+0x188)
        {
            RGE_Master_Missile_Object* own = this_->master_obj;   // asm 0x49BBB3
            this_->own_master = 0;                       // asm 0x49BBC6
            this_->master_obj =
                (RGE_Master_Missile_Object*)this_->owner->master_objects[own->id];   // asm 0x49BBBC-0x49BBCC
            if (own != 0)                                // asm 0x49BBBA/0x49BBD0 (saved flags)
                own->vfptr->__vecDelDtor(own, 1);        // asm 0x49BBD2-0x49BBD6 (slot 5 = +0x14)
        }
        RGE_Combat_Object__destructor((RGE_Combat_Object*)this_);   // asm 0x49BBE3 - HOOKED, routes
    }
    __except (RGE_Combat_Object__destructor((RGE_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x642590
    {
    }
}

// Replacement for RGE_Missile_Object::recycle_out_of_game. Original:
// @ 0x0049BC00 (idb_functions.h:2063). Range [0x49BC00, 0x49BC3A)
// (lst :265942/:265968), void, no stack args. ABI: __thiscall. Virtual:
// vftable slot (lst :265943).
// Decode (asm 0x49BC00-0x49BC3A): the OWN-MASTER teardown with NO
// vfptr store (same reads: own_master @+0x188, master_obj @+0x14,
// owner @+0x18 -> master_objects @+0x74, own->id word @+0x18; the
// saved-flags jz 0x49BC29 skips only the delete), then the runtime
// Action recycle (:122 - HOOKED -> routes; skips the Combat level,
// which has no own recycle - consistent with the vftable slotting
// recorded at the Combat round). No SEH.
void __fastcall RGE_Missile_Object__recycle_out_of_game_new(RGE_Missile_Object* this_, DWORD dummy)
{
    if (this_->own_master != 0)                          // asm 0x49BC03-0x49BC0B (byte @+0x188)
    {
        RGE_Master_Missile_Object* own = this_->master_obj;   // asm 0x49BC0D
        this_->own_master = 0;                           // asm 0x49BC1F
        this_->master_obj =
            (RGE_Master_Missile_Object*)this_->owner->master_objects[own->id];   // asm 0x49BC10-0x49BC26
        if (own != 0)                                    // asm 0x49BC13/0x49BC29 (saved flags)
            own->vfptr->__vecDelDtor(own, 1);            // asm 0x49BC2B-0x49BC2F (slot 5 = +0x14)
    }
    RGE_Action_Object__recycle_out_of_game((RGE_Action_Object*)this_);   // asm 0x49BC34 (:122) - HOOKED, routes
}

// ============ RUNTIME METHODS (batch 1) ============
// All: __thiscall originals, written __fastcall with the DWORD dummy.

// Replacement for RGE_Missile_Object::setup. Original: @ 0x0049BC40
// (idb_functions.h:2064). Range [0x49BC40, 0x49BC90) (lst :265976/
// :266006, retn 14h = 5 stack args), returns int (always 1).
// Decode (asm 0x49BC40-0x49BC90): ghost_mode @+0x180 (byte) = 1 and
// fired_from_id @+0x184 = -1 BEFORE the base call (asm 0x49BC5E/
// 0x49BC65), the runtime Combat setup (:1074, unhooked), then type
// @+0x5E = 60 and own_master @+0x188 (byte) = 0. No SEH.
int __fastcall RGE_Missile_Object__setup_new(
    RGE_Missile_Object* this_, DWORD dummy,
    RGE_Master_Combat_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z)
{
    this_->ghost_mode = 1;                           // asm 0x49BC5E (byte @+0x180)
    this_->fired_from_id = -1;                       // asm 0x49BC65 (@+0x184)
    RGE_Combat_Object__setup((RGE_Combat_Object*)this_, tobj, obj_owner, x, y, z);   // asm 0x49BC6F (:1074)
    this_->type = 60;                                // asm 0x49BC74
    this_->own_master = 0;                           // asm 0x49BC78 (@+0x188)
    return 1;
}

// Replacement for RGE_Missile_Object::setup_read. Original:
// @ 0x0049BC90 (idb_functions.h:2065). Range [0x49BC90, 0x49BD5A)
// (lst :266015/:266098, retn 8 = 2 stack args), returns int (always 1).
// Decode (asm 0x49BC90-0x49BD5A): ghost_mode @+0x180 = 1 BEFORE the
// base call (asm 0x49BCB4), the runtime Combat setup_read (:1075,
// unhooked), type @+0x5E = 60 interleaved (asm 0x49BCCA), then the
// stream block: rge_read(:3003) (4) -> max_range @+0x17C, (4) ->
// fired_from_id @+0x184, (1) -> own_master @+0x188; IF own_master:
// (1) -> a local flag (the asm reuses the dead gworld arg slot), the
// own-master construction - operator_new(0x188) = sizeof(RGE_Master_
// Missile_Object) (392, master-family walk) - guarded RGE_Master_
// Missile_Object__RGE_Master_Missile_Object2 (:1857, HOOKED -> routes;
// args infile, world->sprites @+0x44 (:962), world->sounds @+0x3C
// (:960), do_setup = 1) - master_obj @+0x14 = result. Returns 1
// always (asm 0x49BD4A).
// SEH fold: chunk 0x6425B0 = the allocation-unwind fold - operator_
// delete of the new master (written operator_delete_internal per the
// wrapper rule).
int __fastcall RGE_Missile_Object__setup_read_new(
    RGE_Missile_Object* this_, DWORD dummy, int infile, RGE_Game_World* gworld)
{
    int result = 1;
    RGE_Master_Missile_Object* new_master = 0;
    unsigned __int8 own_flag = 0;
    __try
    {
        this_->ghost_mode = 1;                       // asm 0x49BCB4 (byte @+0x180)
        RGE_Combat_Object__setup_read((RGE_Combat_Object*)this_, infile, gworld);   // asm 0x49BCBB (:1075)
        this_->type = 60;                            // asm 0x49BCCA
        rge_read(infile, &this_->max_range, sizeof(this_->max_range));               // asm 0x49BCCE (4)
        rge_read(infile, &this_->fired_from_id, sizeof(this_->fired_from_id));       // asm 0x49BCE1 (4)
        rge_read(infile, &this_->own_master, sizeof(this_->own_master));             // asm 0x49BCEE (1)
        if (this_->own_master != 0)
        {
            rge_read(infile, &own_flag, sizeof(own_flag));   // asm 0x49BD00 (1; the flag byte)
            new_master = (RGE_Master_Missile_Object*)operator_new_internal(sizeof(RGE_Master_Missile_Object));   // asm 0x49BD0A (0x188)
            if (new_master != 0)
                RGE_Master_Missile_Object__RGE_Master_Missile_Object2(   // asm 0x49BD38 (:1857 - HOOKED, routes)
                    new_master, infile,
                    this_->owner->world->sprites,        // asm 0x49BD30 (world @+0x8C, sprites @+0x44)
                    this_->owner->world->sounds,         // asm 0x49BD2D (sounds @+0x3C)
                    1);                                  // asm 0x49BD25 (do_setup)
            this_->master_obj = new_master;              // asm 0x49BD41 (@+0x14)
        }
        result = 1;
    }
    __except (operator_delete_internal(new_master), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x6425B0
    {
    }
    (void)own_flag;
    return result;
}

// Replacement for RGE_Missile_Object::rehook. Original: @ 0x0049BD60
// (idb_functions.h:2066). Range [0x49BD60, 0x49BD70) (lst :266108/
// :266110). A PURE JMP THUNK -> RGE_Action_Object__rehook (:127,
// 0x408F10); written as the same forwarder through the idb constant.
void __fastcall RGE_Missile_Object__rehook_new(RGE_Missile_Object* this_, DWORD dummy)
{
    RGE_Action_Object__rehook((RGE_Action_Object*)this_);
}

// Replacement for RGE_Missile_Object::save. Original: @ 0x0049BD70
// (idb_functions.h:2067). Range [0x49BD70, 0x49BDBF) (lst :266119/
// :266155, retn 4 = 1 stack arg), void.
// Decode (asm 0x49BD70-0x49BDBF): the runtime Combat save (:1076,
// HOOKED -> routes), then rge_write(:3005) max_range @+0x17C (4),
// fired_from_id @+0x184 (4), own_master @+0x188 (1); IF own_master:
// the OWN MASTER's save vtbl call (RGE_Master_Missile vtbl slot 11 =
// +0x2C, asm 0x49BDB3-0x49BDB9 - the private master streams with the
// object). No SEH.
void __fastcall RGE_Missile_Object__save_new(
    RGE_Missile_Object* this_, DWORD dummy, int outfile)
{
    RGE_Combat_Object__save((RGE_Combat_Object*)this_, outfile);   // asm 0x49BD7A (:1076)
    rge_write(outfile, &this_->max_range, sizeof(this_->max_range));               // asm 0x49BD89 (4)
    rge_write(outfile, &this_->fired_from_id, sizeof(this_->fired_from_id));       // asm 0x49BD98 (4)
    rge_write(outfile, &this_->own_master, sizeof(this_->own_master));             // asm 0x49BDA9 (1)
    if (this_->own_master != 0)                          // asm 0x49BDAE-0x49BDB1
        this_->master_obj->vfptr->save(this_->master_obj, outfile);   // asm 0x49BDB9 (slot 11 = +0x2C)
}

// Replacement for RGE_Missile_Object::update. Original: @ 0x0049BDD0
// (lst :266165/:266167 - an Attributes:thunk jmp -> RGE_Combat_Object_
// update, vftable-slotted; NO idb export - the thunk address itself is
// the Missile dispatch). Range [0x49BDD0, 0x49BDD5). Written as the
// same forwarder through the Combat constant (:1094, HOOKED -> routes).
unsigned __int8 __fastcall RGE_Missile_Object__update_new(RGE_Missile_Object* this_, DWORD dummy)
{
    return RGE_Combat_Object__update((RGE_Combat_Object*)this_);
}

// Replacement for RGE_Missile_Object::init_missile. Original:
// @ 0x0049BDE0 (idb_functions.h:2069). Range [0x49BDE0, 0x49BE9B)
// (lst :266176/:266257, retn 10h = 4 stack args), void.
// Decode (asm 0x49BDE0-0x49BE9B): attack_count @+0x170 = attack_count_
// set (asm 0x49BDFF); operator_new(0x60) = sizeof(RGE_Action_Missile)
// (:26251-26279 walk: vfptr@0 ... old_bounds@92 + 4 = 96); the guarded
// RGE_Action_Missile ctor (:91; args (RGE_Action_Object*)this,
// (RGE_Static_Object*)launcher, target, target->world_x/y/z @+0x48/
// 0x4C/0x50); RGE_Action_List__add_action (:67) on actions @+0x13C if
// the action built; max_range @+0x17C = in_range; fired_from_id =
// launcher ? launcher->id @+0x10 : -1.
// SEH fold: chunk 0x6425D0 = the ALLOCATION-UNWIND fold - operator_
// delete of the fresh action (written operator_delete_internal).
void __fastcall RGE_Missile_Object__init_missile_new(
    RGE_Missile_Object* this_, DWORD dummy, RGE_Combat_Object* launcher,
    RGE_Static_Object* target, float in_range, int attack_count_set)
{
    RGE_Action_Missile* action = 0;
    __try
    {
        this_->attack_count = attack_count_set;          // asm 0x49BDFF (@+0x170)
        action = (RGE_Action_Missile*)operator_new_internal(sizeof(RGE_Action_Missile));   // asm 0x49BE05 (0x60)
        if (action != 0)
            action = RGE_Action_Missile__RGE_Action_Missile(     // asm 0x49BE36 (:91)
                action,
                (RGE_Action_Object*)this_,
                (RGE_Static_Object*)launcher,
                target,
                target->world_x,                         // asm 0x49BE2D (@+0x48)
                target->world_y,                         // asm 0x49BE29 (@+0x4C)
                target->world_z);                        // asm 0x49BE28 (@+0x50)
        if (action != 0)                                 // asm 0x49BE3F-0x49BE41
            RGE_Action_List__add_action(this_->actions, (RGE_Action*)action);   // asm 0x49BE52 (:67; actions @+0x13C)
        this_->max_range = in_range;                     // asm 0x49BE5D (@+0x17C)
        if (launcher != 0)                               // asm 0x49BE5B-0x49BE63
            this_->fired_from_id = launcher->id;         // asm 0x49BE68 (@+0x184)
        else
            this_->fired_from_id = -1;                   // asm 0x49BE85
    }
    __except (operator_delete_internal(action), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x6425D0
    {
    }
}

// Replacement for RGE_Missile_Object::init_missile2. Original:
// @ 0x0049BEA0 (idb_functions.h:2070). Range [0x49BEA0, 0x49BF70)
// (lst :266266/:266xxx, retn 1Ch = 7 stack args), void. Caller: the
// owned Combat attack3 +156 and TRIBE_Combat volley_attack +1B8
// (lst :266267/:266268 - by-address calls route through this hook).
// Decode (asm 0x49BEA0-0x49BF70): the SAME shape as init_missile with
// the explicit x/y/z params going to the Action_Missile ctor
// (asm 0x49BED8-0x49BEEA) and attack_count AFTER add_action
// (asm order: add_action -> attack_count -> max_range -> fired_from_id
// tail). SEH fold: chunk 0x6425F0 = the allocation-unwind fold.
void __fastcall RGE_Missile_Object__init_missile2_new(
    RGE_Missile_Object* this_, DWORD dummy, RGE_Combat_Object* launcher,
    float x, float y, float z, float in_range, int attack_count_set,
    RGE_Static_Object* target)
{
    RGE_Action_Missile* action = 0;
    __try
    {
        action = (RGE_Action_Missile*)operator_new_internal(sizeof(RGE_Action_Missile));   // asm 0x49BEBC (0x60)
        if (action != 0)
            action = RGE_Action_Missile__RGE_Action_Missile(     // asm 0x49BEA0-region (:91)
                action,
                (RGE_Action_Object*)this_,
                (RGE_Static_Object*)launcher,
                target,
                x,
                y,
                z);
        if (action != 0)
            RGE_Action_List__add_action(this_->actions, (RGE_Action*)action);
        this_->attack_count = attack_count_set;          // asm (after add_action)
        this_->max_range = in_range;
        if (launcher != 0)                               // asm 0x49BE5B-analog
            this_->fired_from_id = launcher->id;
        else
            this_->fired_from_id = -1;
    }
    __except (operator_delete_internal(action), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x6425F0
    {
    }
}

// Replacement for RGE_Missile_Object::missile_move. Original:
// @ 0x0049BF70. Range [0x49BF70, 0x49C023) (lst :266362/:266435, retn,
// no stack args), returns u8. Not in the first-pass idb inventory
// (multi-word-return regex gap).
// Decode (FULL ASM PASS 0x49BF70-0x49C023 vs .c:169718-169740): the
// velocity-sum position (nx = velocity_x @+0xBC + world_x @+0x48, ny =
// velocity_y @+0xC0 + world_y @+0x4C, nz = velocity_z @+0xC4 + world_z
// @+0x50 - single float adds, each float-stored, asm 0x49BF76-0x49BF99);
// OOB gates in order: nx < 0 (0x49BF9D-0x49BFAC) || map_width <= nx (fild
// int at map+8, 0x49BFBA-0x49BFC6) || ny < 0 (0x49BFC8-0x49BFD7) ||
// map_height <= ny (map+0xC, 0x49BFD9-0x49BFE5); world @owner+0x8C, map
// @world+0x34. OOB -> die_die_die (slot +0xE4, asm 0x49BFEB) AND FALLS
// THROUGH - no else (asm loc_49BFE7 -> loc_49BFF1); teleport(nx, ny, nz)
// (slot +0xA8, asm 0x49C004; push order = params (nx, ny, nz)) executes on
// ALL paths; return (teleport(...) == world_z @+0x50) (fcomp, test ah,40h
// = the C3-only equality idiom; SSE sete is result-identical incl. NaN).
// DESYNC FIX #5 (2026-11-16): the first write invented `return 0` on the
// OOB path - the original's die_die_die branch falls through to the
// teleport + comparison on every path. The invented return skipped the
// teleport (stale world_x/y/z and tile state -> simulation divergence)
// and hard-coded the result the caller (RGE_Action_Missile__update+39B)
// consumes. Root cause: the asm pass this header previously flagged as
// owed was never executed; the store-and-read diff cannot see
// control-flow-only divergence (0/0 stores either side).
unsigned __int8 __fastcall RGE_Missile_Object__missile_move_new(RGE_Missile_Object* this_, DWORD dummy)
{
    float nx = this_->velocity_x + this_->world_x;               // asm 0x49BF76-0x49BF7F
    float ny = this_->velocity_y + this_->world_y;               // asm 0x49BF83-0x49BF8C
    float nz = this_->velocity_z + this_->world_z;               // asm 0x49BF90-0x49BF99
    RGE_Map* map = this_->owner->world->map;                     // asm 0x49BFAE-0x49BFB7
    if (nx < 0.0f || (double)map->map_width <= (double)nx
        || ny < 0.0f || (double)map->map_height <= (double)ny)   // asm 0x49BF9D-0x49BFE5
    {
        this_->vfptr->die_die_die(this_);                        // asm 0x49BFE7-0x49BFEB (slot +0xE4) - no else, falls through
    }
    return this_->vfptr->teleport(this_, nx, ny, nz)             // asm 0x49BFF1-0x49C004 (slot +0xA8) - ALL paths
        == this_->world_z;                                       // asm 0x49C00A-0x49C013
}

// Replacement for RGE_Missile_Object::make_object_collision_list.
// Original: @ 0x0049C030 (idb_functions.h:2072 - the prototype
// owner-corrected 2026-11-16: the first parameter is the TARGET OBJECT
// pointer, the last three are the missile's PRE-MOVE position). Range
// [0x49C030, 0x49C1C0) (retn 10h), returns RGE_Check_List*.
// Decode (regenerated .c :169745-169807): master radius_x/radius_y
// <= 0 -> return 0; operator_new(4) = sizeof(RGE_Check_List)
// (:26316-26319, {list} only) + the Check_List ctor (0x552470, 9 bytes
// - owner-confirmed body: list = 0); p2 = the POST-move this->world_x/
// world_z/world_y (the caller's missile_move has already advanced the
// missile - .c:50555 before the call at :50585); p1 = the PRE-move
// position snapshot the caller passes (.c:50446-50451: world_x/
// world_y/world_z snapshotted before missile_move); OBSystem_Land/Air
// (common.h:109-110) keyed on the TARGET object's master
// movement_type == 1 (target_obj->master_obj @+0x14, +120 ->
// movement_type) - the collision domain of the TARGET;
// findObSystemCollisions (:2118) over the p1<->p2 segment = the
// distance the missile traveled this tick; the obSystemCollisions
// walk (common.h:112-114, +4 stride, NodeProperties+8 -> the object;
// the world_z delta within both master radius_z bands) -> add_node
// (:3880); empty -> Check_List destructor (:3879) + delete, return 0.
RGE_Check_List* __fastcall RGE_Missile_Object__make_object_collision_list_new(
    RGE_Missile_Object* this_, DWORD dummy,
    RGE_Static_Object* target_obj,
    float old_x, float old_y, float old_z)
{
    // the x87 radius gate is test ah,41h; jnz -> the empty return: the taken
    // set includes UNORDERED, so a NaN master radius returns the empty list
    // in the original; the literal C <= is unordered-false and would run the
    // whole scan - the faithful form is X87_LE_NAN (the {<=, NaN} set, per
    // the macro mandate: unordered-inclusive compares are macro-only)
    if (X87_LE_NAN(this_->master_obj->radius_x, 0.0f) || X87_LE_NAN(this_->master_obj->radius_y, 0.0f))
        return 0;                                         // asm 0x49C04E-0x49C070 (test ah,41h; jnz x2 -> loc_49C1A3)
    RGE_Check_List* list = (RGE_Check_List*)operator_new_internal(sizeof(RGE_Check_List));   // asm 0x49C0xx (4)
    if (list != 0)
        list->list = 0;                                   // Check_List ctor 0x552470 (OVERRIDE: owner-confirmed body)
    BVector p1;
    BVector p2;
    p2.x = this_->world_x;                                // asm 0x49C09D-0x49C0A8 (@+0x48; the post-move x)
    p2.z = this_->world_z;                                // asm 0x49C0AC-0x49C0C7 (offset4 = .z; the post-move height)
    p2.y = this_->world_y;                                // asm 0x5537CB-analog (offset8 = .y; the post-move row)
    p1.x = old_x;                                         // asm 0x49C0A0-0x49C0B3 (offset0 = .x; the pre-move x)
    p1.z = old_z;                                         // asm 0x49C0A4-0x49C0BB (offset4 = .z; the pre-move height)
    p1.y = old_y;                                         // asm 0x49C0DE (offset8 = .y; the pre-move row)
    void* ob_system = (void*)*OBSystem_Land;              // common.h:109 (asm 0x49C0BF loads the slot dword)
    if (target_obj != 0)                                  // asm 0x49C0C5-0x49C0E2
    {
        if (target_obj->master_obj->movement_type == 1)   // asm 0x49C0E4-0x49C0EB (master @+0x14, movement_type byte @+0x78)
            ob_system = (void*)*OBSystem_Air;             // common.h:110 (asm 0x49C0ED)
    }
    findObSystemCollisions(ob_system, (RGE_Moving_Object*)this_, &p1, &p2, 0, 0, 0, 1.0f);   // :2118 (asm 0x49C0F3-0x49C107)
    if (*numberObSystemCollisions > 0)                    // common.h:113 (asm 0x49C10C-0x49C112)
    {
        for (int i = 0; i < *numberObSystemCollisions; ++i)
        {
            ObsRecord* rec = ((ObsRecord**)obSystemCollisions)[i];   // common.h:112; asm 0x49C11A (entry = ObsRecord*, +4 stride)
            RGE_Static_Object* hit = rec->Object;         // asm 0x49C11D-0x49C120 (ObsRecord.Object @+8 - deref unconditional)
            if (hit != 0)                                 // asm 0x49C120-0x49C122
            {
                float dz = hit->world_z - this_->world_z;     // asm 0x49C124-0x49C132 (world_z @+0x50)
                if (dz >= -hit->master_obj->radius_z && dz <= this_->master_obj->radius_z)   // asm 0x49C13D-0x49C148
                    RGE_Check_List__add_node(list, hit, 0.0f, 0.0f, 0);   // asm 0x49C14A-0x49C153 (:3880)
            }
        }
    }
    if (list->list == 0)                                  // asm 169799
    {
        if (list != 0)
        {
            RGE_Check_List__destructor(list);             // :3879
            operator_delete_internal(list);
        }
        return 0;
    }
    return list;
}

// Replacement for RGE_Missile_Object::handle_teleport_z_correction.
// SEMANTIC: the teleport HEIGHT fixer - non-ghost missiles re-run the base correction against the ground; ghost-mode missiles flag themselves airborne (on_ground = 0) and report the -99.9 sentinel height.
// Original: @ 0x0049C1C0 (idb_functions.h:2073). Range [0x49C1C0,
// 0x49C1F0), returns double.
// Decode (.c 169813-169819): IF !ghost_mode: the base Moving
// handle_teleport_z_correction (:2139, unhooked); ELSE on_ground = 0
// and return -99.9f (the .c's -99.900002 = float32 -99.9).
// DOUBLE RETURN (FLOAT PRECISION corollary (h)): ghost path is
// fld ds:flt_65BBE4 (= float32 -99.9, .rdata:0065BBE4); retn 4 (asm
// 0x49C1D1-0x49C1D7) - the m32 constant loads exact, so return -99.9f
// (float literal kept deliberately: (double)(float)-99.9 is exact; a
// bare double literal -99.9 would be a different value). Non-ghost
// path is the tail call + retn 4 (asm 0x49C1DF-0x49C1E4) - st0
// passthrough of the unhooked original; the double-typed idb constant
// captures the 53-bit st0 (the st0-boundary fix).
double __fastcall RGE_Missile_Object__handle_teleport_z_correction_new(
    RGE_Missile_Object* this_, DWORD dummy, float ground_z)
{
    if (this_->ghost_mode == 0)
        return RGE_Moving_Object__handle_teleport_z_correction(
            (RGE_Moving_Object*)this_, ground_z);        // :2139
    this_->on_ground = 0;
    return -99.9f;                                       // asm fld ds:flt_65BBE4 (0x49C1D1) - float32 constant, exact as double
}

// Replacement for RGE_Missile_Object::create_own_master. Original:
// @ 0x0049C1F0 (idb_functions.h:2074). Range [0x49C1F0, 0x49C2E0),
// returns int. Decode (.c 169822-169856): IF own_master -> return 1;
// result = master_obj->vfptr->make_new_master(master_obj) (the Combat
// master vtbl make_new_master slot, IDA-resolved); on success:
// master_obj = result, own_master = 1; IF launcher: the launcher's
// master donates combat stats - weapon_num word copy @+0x138 (312),
// area_effect_range/area_effect_level/defense_terrain_bonus copies,
// the weapon table: free_internal(old) + calloc_internal(weapon_num,
// 4) + memcpy (the qmemcpy is the documented memcpy exception; the
// original's bare free/calloc pair goes through the wrappers per the
// CRT rule).
int __fastcall RGE_Missile_Object__create_own_master_new(
    RGE_Missile_Object* this_, DWORD dummy, RGE_Combat_Object* launcher)
{
    if (this_->own_master != 0)                          // asm (byte @+0x188)
        return 1;
    int result = (int)this_->master_obj->vfptr->make_new_master(this_->master_obj);
    if (result != 0)
    {
        this_->master_obj = (RGE_Master_Missile_Object*)result;   // asm @+0x14
        this_->own_master = 1;                           // asm @+0x188
        if (launcher != 0)
        {
            RGE_Master_Combat_Object* donor = launcher->master_obj;   // asm @+0x14
            this_->master_obj->weapon_num = donor->weapon_num;   // asm word store @+0x138 (312)
            this_->master_obj->area_effect_range = donor->area_effect_range;
            this_->master_obj->area_effect_level = donor->area_effect_level;
            this_->master_obj->defense_terrain_bonus = donor->defense_terrain_bonus;
            if (this_->master_obj->weapon != 0)          // asm (weapon @+0x138-region)
                free_internal(this_->master_obj->weapon);
            if (this_->master_obj->weapon_num > 0)
            {
                this_->master_obj->weapon = (RGE_Armor_Weapon_Info*)
                    calloc_internal(this_->master_obj->weapon_num, sizeof(RGE_Armor_Weapon_Info));
                memcpy(this_->master_obj->weapon, donor->weapon,
                    sizeof(RGE_Armor_Weapon_Info) * this_->master_obj->weapon_num);   // qmemcpy
            }
        }
        result = 1;
    }
    return result;
}

void install_RGE_Missile_Object()
{
    setHook((void*)0x0049BA40, RGE_Missile_Object__RGE_Missile_Object_new);
    setHook((void*)0x0049BAF0, RGE_Missile_Object__scalar_deleting_destructor_new);
    setHook((void*)0x0049BB10, RGE_Missile_Object__RGE_Missile_Object_read_new);
    setHook((void*)0x0049BB80, RGE_Missile_Object__destructor_new);
    setHook((void*)0x0049BC00, RGE_Missile_Object__recycle_out_of_game_new);
    setHook((void*)0x0049BC40, RGE_Missile_Object__setup_new);
    setHook((void*)0x0049BC90, RGE_Missile_Object__setup_read_new);
    setHook((void*)0x0049BD60, RGE_Missile_Object__rehook_new);
    setHook((void*)0x0049BD70, RGE_Missile_Object__save_new);
    setHook((void*)0x0049BDD0, RGE_Missile_Object__update_new);
    setHook((void*)0x0049BDE0, RGE_Missile_Object__init_missile_new);
    setHook((void*)0x0049BEA0, RGE_Missile_Object__init_missile2_new);
    setHook((void*)0x0049BF70, RGE_Missile_Object__missile_move_new);
    setHook((void*)0x0049C030, RGE_Missile_Object__make_object_collision_list_new);
    setHook((void*)0x0049C1C0, RGE_Missile_Object__handle_teleport_z_correction_new);
    setHook((void*)0x0049C1F0, RGE_Missile_Object__create_own_master_new);
}
