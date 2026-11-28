#include "stdafx.h"
#include "RGE_Master_Static_Object.h"
#include "RGE_Action_Types.h"

// .rdata vftable of RGE_Combat_Object (lst :987552; slot 0 =
// scalar_deleting_destructor, lst :117479 XREF)
static RGE_Combat_ObjectVtbl* const rge_combat_object_vftable =
    (RGE_Combat_ObjectVtbl*)0x00657544;

// .data:007A1CB8 - the game's Visible_Unit_Manager* slot (lst :1106232;
// .c:44329). MAP_SIZE: mapsize.cpp:1787 re-points the destructor's read
// of this slot (disp32 0x443B7A) at the patch's malloc'd mapptr_2 base
// when setMapSizeHooks runs; see the // MAP_SIZE marker in the destructor.
static Visible_Unit_Manager** const game_visible_unit_manager =
    (Visible_Unit_Manager**)0x007A1CB8;

// Replacement for RGE_Combat_Object::RGE_Combat_Object (runtime ctor).
// Original: @ 0x00443990 (idb_functions.h:1069). Range [0x443990,
// 0x443A74) (lst :117375/:117469, retn 18h = 6 stack args), returns this.
// ABI: __thiscall - this_ in ECX. Callers: RGE_Master_Combat_Object__
// make_new_obj +9D (owned, hook-routed) and RGE_Missile_Object__
// RGE_Missile_Object +3D (the runtime ladder; lst :117376/:117377).
// Decode (asm 0x443990-0x443A74): base runtime Action ctor with
// do_setup = 0 (hooked -> routes), the Combat vfptr store, then IF
// do_setup: the extension init (asm order - VUR_Ptrs[9] @+0x144 cleared
// rep-stosd x9 (asm 0x4439E9-0x4439F7), Multi_Unified_Points @+0x16C,
// Unified_Map_Value @+0x168, Large_Object_Radius @+0x16D, formation_id/
// row/col @+0x179/17A/17B, attack_count @+0x170 - ALL BEFORE the setup
// call), the runtime Action setup (idb constant :124), type @+0x5E = 50
// (0x32, asm 0x443A3A), attack_timer @+0x174 = 0, capture_flag @+0x178
// = 0.
// SEH fold: chunk 0x641490 = mov ecx, [ebp-10h] / jmp
// RGE_Action_Object__destructor (0x408D20 - HOOKED -> routes; the
// IMMEDIATE base level - runtime ladder evidence).
RGE_Combat_Object* __fastcall RGE_Combat_Object__RGE_Combat_Object_new(
    RGE_Combat_Object* this_, DWORD dummy,
    RGE_Master_Combat_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z, int do_setup)
{
    RGE_Combat_Object* result = this_;
    __try
    {
        RGE_Action_Object__RGE_Action_Object(
            (RGE_Action_Object*)this_, (RGE_Master_Action_Object*)tobj, obj_owner, x, y, z, 0);
        this_->vfptr = rge_combat_object_vftable;                    // asm 0x4439DC
        if (do_setup != 0)
        {
            for (size_t i = 0; i < _countof(this_->VUR_Ptrs); ++i)   // asm 0x4439E9-0x4439F7 (rep stosd x9)
                this_->VUR_Ptrs[i] = 0;
            this_->Multi_Unified_Points = 0;                         // asm 0x443A0B (@0x16C)
            this_->Unified_Map_Value = 0;                            // asm 0x443A11 (@0x168)
            this_->Large_Object_Radius = 0;                          // asm 0x443A17 (@0x16D)
            this_->formation_id = 0;                                 // asm 0x443A1D (@0x179)
            this_->formation_row = 0;                                // asm 0x443A23 (@0x17A)
            this_->formation_col = 0;                                // asm 0x443A29 (@0x17B)
            this_->attack_count = 0;                                 // asm 0x443A2F (@0x170)
            RGE_Action_Object__setup(
                (RGE_Action_Object*)this_, (RGE_Master_Action_Object*)tobj, obj_owner, x, y, z);
            this_->type = RGE_MASTER_TYPE_COMBAT_BASE;                                        // asm 0x443A3A - runtime tier mirror
            this_->attack_timer = 0.0f;                              // asm 0x443A3E (@0x174)
            this_->capture_flag = 0;                                 // asm 0x443A44 (@0x178)
        }
        result = this_;
    }
    __except (RGE_Action_Object__destructor((RGE_Action_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x641490
    {
    }
    return result;
}

// Replacement for RGE_Combat_Object::scalar_deleting_destructor.
// Original: @ 0x00443A80 (idb_functions.h:1070). Range [0x443A80,
// 0x443A9B) (lst :117478/:117496, retn 4), void* result. ABI: __thiscall.
// Virtual: vftable slot 0 (lst :117479 - the Combat runtime vftable uses
// the SCALAR variant; there is no vector_deleting export). Body: the
// Combat destructor (IN BATCH - the direct call routes into
// RGE_Combat_Object__destructor_new per the hook-routing ruling), then
// flags & 1 -> operator_delete, return this. No SEH.
void* __fastcall RGE_Combat_Object__scalar_deleting_destructor_new(
    RGE_Combat_Object* this_, DWORD dummy, unsigned int flags)
{
    RGE_Combat_Object__destructor(this_);
    if (flags & 1)
        operator_delete_internal(this_);
    return this_;
}

// Replacement for RGE_Combat_Object::RGE_Combat_Object_read (runtime
// ctor_read). Original: @ 0x00443AA0 (idb_functions.h:1071). Range
// [0x443AA0, 0x443B02) (lst :117505/:117556, retn 0Ch = 3 stack args),
// returns this. ABI: __thiscall - this_ in ECX. Callers: RGE_Missile_
// Object__RGE_Missile_Object_read +2B (the runtime ladder) and
// RGE_Object_List__load +1FE (lst :117506/:117507).
// Decode (asm 0x443AA0-0x443B02): base runtime Action ctor_read with
// do_setup = 0 (hooked -> routes), the Combat vfptr store, then IF
// do_setup the DERIVED Combat setup_read (:1075, 0x443CA0 - NOT hooked,
// executes the original; it owns the extension init + the stream block).
// SEH fold: chunk 0x6414B0 -> RGE_Action_Object__destructor (HOOKED,
// routes).
RGE_Combat_Object* __fastcall RGE_Combat_Object__RGE_Combat_Object_read_new(
    RGE_Combat_Object* this_, DWORD dummy, int infile, RGE_Game_World* gworld, int do_setup)
{
    RGE_Combat_Object* result = this_;
    __try
    {
        RGE_Action_Object__RGE_Action_Object_read((RGE_Action_Object*)this_, infile, gworld, 0);
        this_->vfptr = rge_combat_object_vftable;                    // asm 0x443ADE
        if (do_setup != 0)
            RGE_Combat_Object__setup_read(this_, infile, gworld);    // asm 0x443AEA (:1075)
        result = this_;
    }
    __except (RGE_Action_Object__destructor((RGE_Action_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x6414B0
    {
    }
    return result;
}

// Replacement for RGE_Combat_Object::destructor. Original: @ 0x00443B10
// (idb_functions.h:1072). Range [0x443B10, 0x443BAB) (lst :117565/:117625),
// void, no stack args. ABI: __thiscall. Callers: the scalar deleting
// destructor above and RGE_Missile_Object__destructor +63 (the runtime
// ladder, hook-routed; lst :117565/:117566).
// Decode (asm 0x443B10-0x443BAB): vfptr store, THEN the visible-unit
// deregistration: if ((int)this->id @+0x10 >= 0) (signed test/jl,
// asm 0x443B3F) -> Visible_Unit_Manager__Update_Unit_Info (:5928,
// 0x614840) with (manager, id, owner->id @0xA0, ftol(world_x @0x48),
// ftol(world_y @0x4C), Large_Object_Radius @+0x16D (byte),
// master_obj->object_group (movsx word @0x1E), Unified_Map_Value
// @+0x168, 0, &VUR_Ptrs[0] @+0x144) - push order verified asm
// 0x443B52-0x443B87; manager read from the game slot 0x007A1CB8. Then
// the runtime Action destructor (HOOKED -> routes).
// SEH fold: chunk 0x6414D0 -> RGE_Action_Object__destructor (HOOKED,
// routes) - the throw-during-destruction unwind runs the FULL base
// destructor, immediate base level.
void __fastcall RGE_Combat_Object__destructor_new(RGE_Combat_Object* this_, DWORD dummy)
{
    __try
    {
        this_->vfptr = rge_combat_object_vftable;                    // asm 0x443B2E
        if (this_->id >= 0)                                          // asm 0x443B3F-0x443B41 (signed)
        {
            // MAP_SIZE: mapsize.cpp:1787 re-points the manager-slot read
            // inside this call (disp32 0x443B7A) at the patch's malloc'd
            // mapptr_2 base when setMapSizeHooks runs. NOT baked
            // (owner-directed MAP_SIZE rule 2026-11-15): the replacement
            // reads the original game slot 0x007A1CB8.
            Visible_Unit_Manager__Update_Unit_Info(
                *game_visible_unit_manager,                          // asm 0x443B78
                this_->id,                                           // asm push edi (Object_Id)
                this_->owner->id,                                    // asm 0x443B7F (id @0xA0)
                (int)this_->world_x,                                 // asm 0x443B6C-0x443B70 (ftol)
                (int)this_->world_y,                                 // asm 0x443B52-0x443B67 (ftol)
                this_->Large_Object_Radius,                          // asm 0x443B5D-0x443B66 (@0x16D)
                this_->master_obj->object_group,                     // asm 0x443B58 (movsx word @0x1E)
                this_->Unified_Map_Value,                            // asm 0x443B46 (@0x168)
                0,                                                   // asm 0x443B56 (NewUnifiedMap = 0)
                this_->VUR_Ptrs);                                    // asm 0x443B4C (lea eax, [esi+144h])
        }
        RGE_Action_Object__destructor((RGE_Action_Object*)this_);    // asm 0x443B96 - HOOKED, routes
    }
    __except (RGE_Action_Object__destructor((RGE_Action_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x6414D0
    {
    }
}

// Replacement for RGE_Combat_Object::recycle_in_to_game. Original:
// @ 0x00443BB0 (idb_functions.h:1073). Range [0x443BB0, 0x443C13)
// (lst :117634/:117678, retn 14h = 5 stack args), void. ABI: __thiscall.
// Virtual: vftable DATA XREF - the Combat AND Missile runtime vftables
// both slot this function (lst :117635/:117636), so the hook covers
// Missile's dispatch too. Caller: the Action recycle above (ladder).
// Decode (asm 0x443BB0-0x443C13): the extension init WITHOUT attack_count
// (VUR_Ptrs cleared rep-stosd x9, Multi_Unified_Points, Unified_Map_Value,
// Large_Object_Radius, formation_id/row/col = 0 - asm 0x443BB9-0x443BFF),
// then the runtime Action recycle_in_to_game (:123, HOOKED -> routes),
// then attack_timer @+0x174 = 0 (asm 0x443C0A). NO type store. No SEH.
void __fastcall RGE_Combat_Object__recycle_in_to_game_new(
    RGE_Combat_Object* this_, DWORD dummy,
    RGE_Master_Static_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z)
{
    for (size_t i = 0; i < _countof(this_->VUR_Ptrs); ++i)           // asm 0x443BB9-0x443BC6 (rep stosd x9)
        this_->VUR_Ptrs[i] = 0;
    this_->Multi_Unified_Points = 0;                                 // asm 0x443BE1 (@0x16C)
    this_->Unified_Map_Value = 0;                                    // asm 0x443BE7 (@0x168)
    this_->Large_Object_Radius = 0;                                  // asm 0x443BED (@0x16D)
    this_->formation_id = 0;                                         // asm 0x443BF3 (@0x179)
    this_->formation_row = 0;                                        // asm 0x443BF9 (@0x17A)
    this_->formation_col = 0;                                        // asm 0x443BFF (@0x17B)
    RGE_Action_Object__recycle_in_to_game(
        (RGE_Action_Object*)this_, tobj, obj_owner, x, y, z);        // asm 0x443C05 - HOOKED, routes
    this_->attack_timer = 0.0f;                                      // asm 0x443C0A (@0x174)
}

// ============ RUNTIME METHODS (batch C1) ============

// Replacement for RGE_Combat_Object::stop. Original: @ 0x00443DF0
// (idb_functions.h:1077). Range [0x443DF0, 0x443E0F) (retn = no stack
// args), void. ABI: __thiscall. Virtual: vftable DATA XREF. Caller:
// TRIBE_Building_Object__stop+1C (the runtime ladder).
// Semantics: stop the unit AND its AI module - the base stop (hooked ->
// routes) clears the action machinery; when the module exists it also
// stops the module's object tracking and clears the current target.
// Decode (asm 0x443DF0-0x443E0E): base RGE_Action_Object::stop (HOOKED
// -> routes; asm 0x443DF3); unitAIValue @+0x7C gate -> the stopObject
// NAMED dispatch (UnitAIModuleVtbl member 21 = +0x54, line-arith
// 7588-7567 = 21; asm 0x443E03 call [eax+54h], argument 0) +
// UnitAIModule__removeCurrentTarget (:501 - UNHOOKED; asm 0x443E09).
// Join inventory: loc_443E0E (1 inbound jz) - covered by the guard +
// implicit return.
void __fastcall RGE_Combat_Object__stop_new(RGE_Combat_Object* this_, DWORD dummy)
{
    RGE_Action_Object__stop((RGE_Action_Object*)this_);             // asm 0x443DF3 - HOOKED, routes
    if (this_->unitAIValue)                                         // asm 0x443DFB-0x443DFD (@+0x7C)
    {
        this_->unitAIValue->vfptr->stopObject(this_->unitAIValue, 0);   // asm 0x443E03 (slot +0x54)
        UnitAIModule__removeCurrentTarget(this_->unitAIValue);      // asm 0x443E09 (:501)
    }
}

// Replacement for RGE_Combat_Object::calc_attack_modifier. Original:
// @ 0x00443E10 (idb_functions.h:1078). Range [0x443E10, 0x443E16)
// (retn 4 = 1 stack arg), double return. ABI: __thiscall. Virtual:
// vftable DATA XREF (Combat + Missile vftables - the base
// implementation shared by the Missile tier).
// Semantics: the attack-modifier hook - the base implementation is the
// neutral 1.0 (the derived/patch variants may override).
// Decode (asm 0x443E10-0x443E15): fld ds:const_f_one; retn 4 - the
// literal 1.0, exact (the double return; the target param unused).
double __fastcall RGE_Combat_Object__calc_attack_modifier_new(
    RGE_Combat_Object* this_, DWORD dummy, RGE_Static_Object* target)
{
    return 1.0;                                                     // asm 0x443E10 (fld const_f_one)
}

// Replacement for RGE_Combat_Object::can_attack. Original: @ 0x00444AE0
// (idb_functions.h:1086). Range [0x444AE0, 0x444AF8) (retn = no stack
// args), unsigned __int8 return. ABI: __thiscall. Virtual: vftable DATA
// XREF.
// Semantics: may the unit attack now - true when the attack timer has
// expired.
// Decode (asm 0x444AE0-0x444AF7): attack_timer @+0x174 = 372 (the
// Combat runtime extension, batch 1b-4 map) vs const_f_zero: fld /
// fcomp / fnstsw / test ah,41h / jnz -> true. THE X87 SITE: the taken
// set {<= 0 ordered, NaN} -> the X87_LE_NAN macro (the literal
// `<= 0.0` would compile unordered-false - NaN -> false - DIVERGES).
unsigned __int8 __fastcall RGE_Combat_Object__can_attack_new(RGE_Combat_Object* this_, DWORD dummy)
{
    return X87_LE_NAN(this_->attack_timer, 0.0f);                   // asm 0x444AE0-0x444AF1
}

// Replacement for RGE_Combat_Object::reset_attack_timer. Original:
// @ 0x00444B00 (idb_functions.h:1087). Range [0x444B00, 0x444B0F)
// (retn = no stack args), void. ABI: __thiscall. Callers:
// RGE_Action_Attack__set_state/update (the attack cycle - unowned
// callees call UP into this).
// Semantics: reload the attack timer from the master's speed of attack.
// Decode (asm 0x444B00-0x444B0E): attack_timer @+0x174 = master_obj
// (@+0x14) -> speed_of_attack @+0x150 = 336 (the master walk) - the
// x87 fld/fstp float=float pair, float-safe per rule (d).
void __fastcall RGE_Combat_Object__reset_attack_timer_new(RGE_Combat_Object* this_, DWORD dummy)
{
    this_->attack_timer = this_->master_obj->speed_of_attack;       // asm 0x444B00-0x444B0E
}

// Replacement for RGE_Combat_Object::get_weapon_range. Original:
// @ 0x00444C80 (idb_functions.h:1090). Range [0x444C80, 0x444C9E)
// (retn 8 = 2 stack args), void. ABI: __thiscall. Virtual: vftable
// DATA XREF.
// Semantics: copy the current and original weapon ranges out of the
// master (the modify-era cur/orig pair).
// Decode (asm 0x444C80-0x444C9D): *cur = master_obj->weapon_range
// @+0x144 = 324; *orig = orig_weapon_range @+0x170 = 368 (the master
// walk). Straight-line, no jcc.
void __fastcall RGE_Combat_Object__get_weapon_range_new(
    RGE_Combat_Object* this_, DWORD dummy, float* cur_weapon_range_in,
    float* orig_weapon_range_in)
{
    *cur_weapon_range_in = this_->master_obj->weapon_range;         // asm 0x444C80-0x444C8D (@+0x144)
    *orig_weapon_range_in = this_->master_obj->orig_weapon_range;   // asm 0x444C8F-0x444C9C (@+0x170)
}

// Replacement for RGE_Combat_Object::get_speed_of_attack. Original:
// @ 0x00444CB0 (idb_functions.h:1091). Range [0x444CB0, 0x444CCE)
// (retn 8 = 2 stack args), void. ABI: __thiscall. Virtual: vftable
// DATA XREF.
// Semantics: the cur/orig speed-of-attack pair - the reset_attack_timer
// source and its original.
// Decode (asm 0x444CB0-0x444CCD): *cur = speed_of_attack @+0x150 = 336;
// *orig = orig_speed_of_attack @+0x174 = 372. Straight-line.
void __fastcall RGE_Combat_Object__get_speed_of_attack_new(
    RGE_Combat_Object* this_, DWORD dummy, float* cur_speed_of_attack_in,
    float* orig_speed_of_attack_in)
{
    *cur_speed_of_attack_in = this_->master_obj->speed_of_attack;         // asm 0x444CB0-0x444CBD (@+0x150)
    *orig_speed_of_attack_in = this_->master_obj->orig_speed_of_attack;   // asm 0x444CBF-0x444CCC (@+0x174)
}

// Replacement for RGE_Combat_Object::moveTo3. Original: @ 0x00445510
// (idb_functions.h:1099). Range [0x445510, 0x445527) (retn 8 = 2 stack
// args), int return. ABI: __thiscall. Virtual: vftable DATA XREF.
// Caller: TRIBE_Combat_Object__moveTo2+CB (the runtime ladder).
// Semantics: the target-id move entry - forwards to moveTo2 with the
// default minimum range 1.0.
// Decode (asm 0x445510-0x445526): the moveTo2 NAMED dispatch (runtime
// slot 124 = +0x1F0, line-arith 11476-11352 = 124, re-verified 2026-11-24
// against the current lines; asm 0x445521 call
// [eax+1F0h]) with (targetID, 1.0f, unconditional) - the .c's
// 1065353216 dint = the float 1.0 (the dd rule). Straight-line; the
// return value passes through.
int __fastcall RGE_Combat_Object__moveTo3_new(
    RGE_Combat_Object* this_, DWORD dummy, int targetID, int unconditional)
{
    return this_->vfptr->moveTo2(this_, targetID, 1.0f, unconditional);   // asm 0x445521 (slot +0x1F0)
}

// Replacement for RGE_Combat_Object::pause. Original: @ 0x00445F30
// (idb_functions.h:1108). Range [0x445F30, 0x445F4F) (retn = no stack
// args), int return (1). ABI: __thiscall. Virtual: vftable DATA XREF.
// Semantics: re-apply the master's current sprite to the unit (the
// pause visual reset).
// Decode (asm 0x445F30-0x445F4E): the master get_sprite NAMED dispatch
// (master vtbl slot 1 = +0x4; asm 0x445F3C call [eax+4]) -> the
// new_sprite NAMED dispatch (runtime slot 44 = +0xB0, the batch 1b-1
// precedent; asm 0x445F42 call [edi+0B0h]); return 1. Straight-line.
int __fastcall RGE_Combat_Object__pause_new(RGE_Combat_Object* this_, DWORD dummy)
{
    RGE_Sprite* sprite = this_->master_obj->vfptr->get_sprite(      // asm 0x445F3C (master slot +0x4)
        this_->master_obj, (RGE_Static_Object_idb*)this_);
    this_->vfptr->new_sprite(this_, sprite);                        // asm 0x445F42 (slot +0xB0)
    return 1;                                                       // asm 0x445F49
}

// Replacement for RGE_Combat_Object::rateOfFire. Original: @ 0x00445F50
// (idb_functions.h:1109). Range [0x445F50, 0x445F59) (retn = no stack
// args), double return. ABI: __thiscall. Virtual: vftable DATA XREF.
// Semantics: the master's speed of attack (the reset_attack_timer
// source).
// Decode (asm 0x445F50-0x445F58): master_obj (@+0x14) -> speed_of_attack
// @+0x150 = 336, fld m32/retn - the st0 passthrough (rule (h), exact).
double __fastcall RGE_Combat_Object__rateOfFire_new(RGE_Combat_Object* this_, DWORD dummy)
{
    return this_->master_obj->speed_of_attack;                      // asm 0x445F50-0x445F58 (fld [eax+150h])
}

// Replacement for RGE_Combat_Object::weaponRange. Original: @ 0x00446090
// (idb_functions.h:1112). Range [0x446090, 0x446099) (retn = no stack
// args), double return. ABI: __thiscall. Virtual: vftable DATA XREF.
// Semantics: the master's weapon range.
// Decode (asm 0x446090-0x446098): weapon_range @+0x144 = 324, fld/retn.
double __fastcall RGE_Combat_Object__weaponRange_new(RGE_Combat_Object* this_, DWORD dummy)
{
    return this_->master_obj->weapon_range;                         // asm 0x446090-0x446098 (fld [eax+144h])
}

// Replacement for RGE_Combat_Object::minimumWeaponRange. Original:
// @ 0x004460A0 (idb_functions.h:1113). Range [0x4460A0, 0x4460A9)
// (retn = no stack args), double return. ABI: __thiscall. Virtual:
// vftable DATA XREF.
// Semantics: the master's minimum weapon range (inside which the unit
// cannot attack).
// Decode (asm 0x4460A0-0x4460A8): minimum_weapon_range @+0x168 = 360,
// fld/retn.
double __fastcall RGE_Combat_Object__minimumWeaponRange_new(RGE_Combat_Object* this_, DWORD dummy)
{
    return this_->master_obj->minimum_weapon_range;                 // asm 0x4460A0-0x4460A8 (fld [eax+168h])
}

// Replacement for RGE_Combat_Object::currentTargetX. Original:
// @ 0x00446150 (idb_functions.h:1116). Range [0x446150, 0x44617D)
// (retn = no stack args), double return. ABI: __thiscall. Virtual:
// vftable DATA XREF.
// Semantics: the current action's target x, -1.0 when no action.
// Decode (asm 0x446150-0x44617C): have_action (:72) with the EXACT
// == 1 guard (cmp al,1); get_action (:78) -> RGE_Action__targetX
// (:186, the double-return st0 passthrough). The -1.0 arm:
// fld ds:flt_6545C8 (= -1.0, the A4-verified constant). Exit paths:
// 2 asm returns. (targetY @0x446180 / targetZ @0x4461B0 are the same
// shape with :187/:188.)
double __fastcall RGE_Combat_Object__currentTargetX_new(RGE_Combat_Object* this_, DWORD dummy)
{
    if (RGE_Action_List__have_action(this_->actions) != 1)          // asm 0x446159-0x446160 (:72)
        return -1.0;                                                // asm fld flt_6545C8
    RGE_Action* action = RGE_Action_List__get_action(this_->actions);   // asm 0x446168 (:78)
    return RGE_Action__targetX(action);                             // asm 0x44616F (:186)
}

// Replacement for RGE_Combat_Object::currentTargetY. Original:
// @ 0x00446180 (idb_functions.h:1117). Range [0x446180, 0x4461AD)
// (retn = no stack args), double return. ABI: __thiscall. Virtual:
// vftable DATA XREF. The targetX twin with RGE_Action__targetY (:187).
double __fastcall RGE_Combat_Object__currentTargetY_new(RGE_Combat_Object* this_, DWORD dummy)
{
    if (RGE_Action_List__have_action(this_->actions) != 1)          // asm 0x446189-0x446190
        return -1.0;                                                // asm 0x4461A6 (fld flt_6545C8)
    RGE_Action* action = RGE_Action_List__get_action(this_->actions);   // asm 0x446198
    return RGE_Action__targetY(action);                             // asm 0x44619F (:187)
}

// Replacement for RGE_Combat_Object::currentTargetZ. Original:
// @ 0x004461B0 (idb_functions.h:1118). Range [0x4461B0, 0x4461DD)
// (retn = no stack args), double return. ABI: __thiscall. Virtual:
// vftable DATA XREF. The targetX twin with RGE_Action__targetZ (:188).
double __fastcall RGE_Combat_Object__currentTargetZ_new(RGE_Combat_Object* this_, DWORD dummy)
{
    if (RGE_Action_List__have_action(this_->actions) != 1)          // asm 0x4461B9-0x4461C0
        return -1.0;
    RGE_Action* action = RGE_Action_List__get_action(this_->actions);
    return RGE_Action__targetZ(action);
}

// ============ RUNTIME METHODS (batch C2) ============

// Replacement for RGE_Combat_Object::release_being_worked_on. Original:
// @ 0x00444CE0 (idb_functions.h:1092). Range [0x444CE0, 0x444D17) (retn
// 4 = 1 stack arg), void. ABI: __thiscall. Virtual: vftable DATA XREF.
// Semantics: the work-release notify - after the base release, tell the
// notification pipeline the worker was released (mType 699 = the
// RGE_UNITAI_ACTION_RELEASED value, the registry's first owned
// consumer).
// Decode (asm 0x444CE0-0x444D16): base RGE_Animated_Object::
// release_being_worked_on (header-driven; asm 0x444CE9); caller->id
// @+0x10 >= 0 gate -> the notify NAMED dispatch (slot 120 = +0x1E0,
// line-arith 11718-11598 = 120; asm 0x444D0F call [edx+1E0h]) with
// (caller->id, this->id @+0x10, 699, movsx caller->master_obj->group
// (@+0x1E via master_obj @+0x14), 0, 0). Join inventory: loc_444D15
// (1 inbound jl) - covered by the guard + implicit return.
void __fastcall RGE_Combat_Object__release_being_worked_on_new(
    RGE_Combat_Object* this_, DWORD dummy, RGE_Static_Object* caller)
{
    RGE_Animated_Object__release_being_worked_on(
        (RGE_Animated_Object*)this_, caller);                    // asm 0x444CE9
    if (caller->id >= 0)                                         // asm 0x40944EE-0x40944F1 (@+0x10)
        this_->vfptr->notify(this_, caller->id, this_->id,       // asm 0x444D0F (slot +0x1E0)
            RGE_UNITAI_ACTION_RELEASED,
            caller->master_obj->object_group, 0, 0);
}

// Replacement for RGE_Combat_Object::copy_obj. Original: @ 0x004450C0
// (idb_functions.h:1093). Range [0x4450C0, 0x445103) (retn 4 = 1 stack
// arg), void. ABI: __thiscall. Virtual: vftable DATA XREF. Caller:
// TRIBE_Combat_Object__copy_obj+A6 (the runtime ladder).
// Semantics: when the unit currently shows its fight sprite, swap to
// the source master's fight sprite (the visual copy), then run the base
// Action copy.
// Decode (asm 0x4450C0-0x4450FE): the get_fight_sprite NAMED dispatch
// on the unit's OWN master (master_obj @+0x14; master vtbl slot 33 =
// +0x84 - the CONCRETE master Combat vftable @0x0065AFF8 slot 33 =
// RGE_Master_Combat_Object__get_fight_sprite, .lst-walked; the idb
// member 33 = :11249 agrees; asm 0x4450CA call [eax+84h]) vs the
// unit's sprite @+0x1C; equal -> source2's get_fight_sprite (the .c's
// `vfptr[1].get_death_sprite` name is the vfptr[1] render artifact -
// BOTH asm calls hit slot +0x84, .lst 989852) -> new_sprite (slot 44 =
// +0xB0; asm 0x4450EF call [edx+0B0h]); then base RGE_Action_Object__
// copy_obj (HOOKED -> routes; asm 0x4450F8). Join inventory: loc_4450F5
// (2 inbound) - covered by the guard cascade + the unconditional tail.
void __fastcall RGE_Combat_Object__copy_obj_new(
    RGE_Combat_Object* this_, DWORD dummy, RGE_Master_Static_Object* source2)
{
    RGE_Master_Combat_Object* combat_source =
        (RGE_Master_Combat_Object*)source2;                     // the Combat-tier master (the copy_obj slot only exists on the Combat-descended vftables)
    if (this_->sprite == this_->master_obj->vfptr->get_fight_sprite(
            this_->master_obj, (RGE_Static_Object_idb*)this_))      // asm 0x4450C5-0x4450CA (slot +0x84)
    {
        RGE_Sprite* sprite = combat_source->vfptr->get_fight_sprite(
            combat_source, (RGE_Static_Object_idb*)this_);          // asm 0x4450E0 (slot +0x84)
        if (sprite)
            this_->vfptr->new_sprite(this_, sprite);            // asm 0x4450EF (slot +0xB0)
    }
    RGE_Action_Object__copy_obj((RGE_Action_Object*)this_, source2);   // asm 0x4450F8 - HOOKED, routes
}

// Replacement for RGE_Combat_Object::currentTargetID. Original:
// @ 0x004460B0 (idb_functions.h:1114). Range [0x4460B0, 0x4460F3)
// (retn = no stack args), int return. ABI: __thiscall. Virtual: vftable
// DATA XREF.
// Semantics: the current action's target id, -1 when no action or no
// target object.
// Decode (asm 0x4460B0-0x4460F2): have_action (:72) with the EXACT
// == 1 guard (cmp al,1); get_action (:78) -> get_target_obj (slot
// +0x50) null-checked -> ->id @+0x10 (the second get_action fetch -
// the asm 0x4460C8/0x4460DE pair, faithful). Exit paths: 2 asm
// returns.
int __fastcall RGE_Combat_Object__currentTargetID_new(RGE_Combat_Object* this_, DWORD dummy)
{
    if (RGE_Action_List__have_action(this_->actions) != 1)          // asm 0x4460B9-0x4460C0 (:72)
        return -1;                                                  // asm 0x4460EF
    RGE_Action* action = RGE_Action_List__get_action(this_->actions);   // asm 0x4460C8 (:78)
    if (!action->vfptr->get_target_obj(action))                     // asm 0x409460D1 (slot +0x50)
        return -1;
    RGE_Action* current = RGE_Action_List__get_action(this_->actions);   // asm 0x4460DE (:78)
    return current->vfptr->get_target_obj(current)->id;             // asm 0x4460E7/0x4460EA (@+0x10)
}

// Replacement for RGE_Combat_Object::currentTargetID2. Original:
// @ 0x00446100 (idb_functions.h:1115). Range [0x446100, 0x446143)
// (retn = no stack args), int return. ABI: __thiscall. Virtual: vftable
// DATA XREF. The currentTargetID twin with get_target_obj2 (the
// RGE_ActionVtbl member 21 = +0x54, line-arith 15983-15962 = 21; asm
// 0x446258-0x44625B region /Fa [eax+84] check owed).
int __fastcall RGE_Combat_Object__currentTargetID2_new(RGE_Combat_Object* this_, DWORD dummy)
{
    if (RGE_Action_List__have_action(this_->actions) != 1)          // asm 0x446120-0x446127
        return -1;                                                  // asm 0x446143
    RGE_Action* action = RGE_Action_List__get_action(this_->actions);   // asm 0x446128
    if (!action->vfptr->get_target_obj2(action))                    // asm 0x44612D-0x446130 (slot +0x54)
        return -1;
    RGE_Action* current = RGE_Action_List__get_action(this_->actions);   // asm 0x44613A
    return current->vfptr->get_target_obj2(current)->id;            // asm 0x446140-0x446141
}

// Replacement for RGE_Combat_Object::get_visibility. Original:
// @ 0x00446260 (idb_functions.h:1119). Range [0x446260, 0x4462B6)
// (retn 4 = 1 stack arg), unsigned __int8 return. ABI: __thiscall.
// Virtual: vftable DATA XREF.
// Semantics: the extended visibility probe - the base tile visibility,
// UPGRADED when the unit has a Unified Map record and the viewing
// player's mutual mask covers it: 15 (visible) or 0x80 (explored).
// Decode (asm 0x446260-0x4462B5): ftol(world_y @+0x4C) -> map_row;
// ftol(world_x @+0x48) -> map_col (the push order verified against the
// owned get_visible_new signature (map_col, map_row) - the .c's push
// comments were the artifact); player->visible @+0x88;
// RGE_Visible_Map__get_visible (:5942 @0x615430 - HOOKED -> routes
// into the owned replacement); result != 15 && Multi_Unified_Points
// @+0x16C = 364 -> Unified_Map_Value @+0x168 = 360 vs player->
// mutualVisibleMask @+0x130 = 304 (the Static-round offsets) -> 15;
// else vs mutualExploredMask @+0x12C = 300 -> 0x80.
unsigned __int8 __fastcall RGE_Combat_Object__get_visibility_new(
    RGE_Combat_Object* this_, DWORD dummy, RGE_Player* player)
{
    unsigned __int8 result = RGE_Visible_Map__get_visible(
        player->visible, (int)this_->world_x, (int)this_->world_y);   // asm 0x446264-0x446283: map_col = ftol(world_x), map_row = ftol(world_y) - HOOKED, routes
    if (result != 15 && this_->Multi_Unified_Points)                  // asm 0x446285-0x446291 (@+0x16C)
    {
        if (player->mutualVisibleMask & this_->Unified_Map_Value)     // asm 0x446299-0x4462A1 (@+0x130 / @+0x168)
            result = 15;                                              // asm 0x4462A4
        else if (this_->Unified_Map_Value & player->mutualExploredMask)   // asm 0x4462AA-0x4462B0 (@+0x12C)
            result = 0x80;                                            // asm 0x4462B2
    }
    return result;                                                  // asm 0x4462B4-0x4462B6
}

// Replacement for RGE_Combat_Object::inAttackRange. Original:
// @ 0x00446020 (idb_functions.h:1120). Range [0x446020, 0x44608B)
// (retn 8 = 2 stack args), int return. ABI: __thiscall. Virtual:
// vftable DATA XREF.
// Semantics: is the target within the weapon range (return 1) and if
// not, is it inside the minimum range (the insideMinimum out-flag).
// Decode (asm 0x446020-0x44608A): *insideMinimum = 0; distance =
// RGE_Static_Object__distance_to_object (:3871, DOUBLE return -> the
// asm fstp m32 = the rule-(e) store-round; written as a float
// capture); minimum = minimumWeaponRange NAMED dispatch (slot 102 =
// +0x198, the fstp m32 store-round; /Fa check: [edx+408]); weapon =
// weaponRange NAMED dispatch (slot 101 = +0x194; the st0 stays
// UNROUNDED - rule (h) double capture). distance < minimum (test
// ah,1 - C0-only {<, NaN} -> the negated-opposite form
// X87_LT_NAN(distance, minimum)) -> *insideMinimum = 1; return
// X87_LE_NAN(distance, weapon) && *insideMinimum == 0 (test ah,41h
// / jz - the {<=, NaN} proceed set; NaN proceeds to the flag check -
// byte-faithful). Exit paths: 2 asm returns.
int __fastcall RGE_Combat_Object__inAttackRange_new(
    RGE_Combat_Object* this_, DWORD dummy, RGE_Static_Object* target,
    int* insideMinimum)
{
    *insideMinimum = 0;                                             // asm 0x4094602D
    float distance = (float)RGE_Static_Object__distance_to_object(  // asm 0x40946033 (:3871; the fstp m32 store-round)
        (RGE_Static_Object*)this_, target);
    float minimum = (float)this_->vfptr->minimumWeaponRange(this_);   // asm 0x40946040 (slot +0x198; the fstp m32 store-round)
    double weapon = this_->vfptr->weaponRange(this_);               // asm 0x4094604E (slot +0x194; the st0 consumed unrounded)
    if (X87_LT_NAN(distance, minimum))                              // asm 0x4094605E (test ah,1)
        *insideMinimum = 1;                                         // asm 0x40946063
    if (X87_LE_NAN(distance, weapon) && *insideMinimum == 0)        // asm 0x40946071-0x4094607B
        return 1;                                                   // asm 0x4094607E
    return 0;                                                       // asm 0x40946088
}

// Replacement for RGE_Combat_Object::teleport. Original: @ 0x004461E0
// (idb_functions.h:1121). Range [0x4461E0, 0x446252) (retn 0Ch = 3
// stack args), double return. ABI: __thiscall. Virtual: vftable DATA
// XREF. Caller: TRIBE_Building_Object__teleport+77 (the runtime
// ladder).
// Semantics: teleport and refresh the visibility-manager records - the
// Moving teleport first (the owned M12 replacement), then when the
// unit's tile changed, patch all 9 visibility records' position bytes.
// Decode (asm 0x4461E0-0x446251): old_x = ftol(world_x @+0x48);
// old_y = ftol(world_y @+0x4C) (asm 0x4461E6-0x4461F5 - the __ftol
// conversion, the (int) form per the template rule); base
// RGE_Moving_Object__teleport (:2133 - HOOKED -> routes into the M12
// replacement; asm 0x44620B) - the DOUBLE return captured unrounded
// (rule (h)); id >= 0 gate; new_x/new_y = ftol(world_x/world_y); the
// changed check (cmp ebx,edi / cmp eax,ebp); the VUR_Ptrs[9] loop:
// each non-null record gets pos_x @+6 = (byte)ftol(new_x), pos_y @+7
// = (byte)ftol(new_y) (VISIBLE_UNIT_REC :10338-10345 - pos_x/pos_y
// ARE bytes; the .c's int renders were IDA artifacts). Exit paths: 2
// asm returns (the jl skip + the shared tail).
double __fastcall RGE_Combat_Object__teleport_new(
    RGE_Combat_Object* this_, DWORD dummy, float x, float y, float z)
{
    int old_x = (int)this_->world_x;                                // asm 0x4461E6-0x4461F1 (__ftol)
    int old_y = (int)this_->world_y;                                // asm 0x4461EE-0x40946F3
    double result = RGE_Moving_Object__teleport(                    // asm 0x44620B (:2133) - HOOKED, routes
        (RGE_Moving_Object*)this_, x, y, z);
    if (this_->id >= 0)                                             // asm 0x446210-0x446215
    {
        int new_x = (int)this_->world_x;                            // asm 0x446217-0x44621F
        int new_y = (int)this_->world_y;                            // asm 0x44621F-0x446224
        if (new_x != old_x || new_y != old_y)                       // asm 0x446229-0x44622F
        {
            for (size_t i = 0; i < _countof(this_->VUR_Ptrs); ++i)  // asm 0x446231-0x44624C (the 9-iteration loop)
            {
                VISIBLE_UNIT_REC* record = this_->VUR_Ptrs[i];
                if (record)                                         // asm 0x44623E
                {
                    record->pos_x = (unsigned char)new_x;           // asm 0x446242 (mov [ecx+6], bl)
                    record->pos_y = (unsigned char)new_y;           // asm 0x446245 (mov [ecx+7], al)
                }
            }
        }
    }
    return result;                                                  // asm 0x44624E-0x446251
}

// Replacement for RGE_Combat_Object::notify. Original: @ 0x00445110
// (idb_functions.h:1094). Range [0x445110, 0x445184) (retn 18h = 6
// stack args), void. ABI: __thiscall. Virtual: vftable DATA XREF.
// Semantics: the notification FILTER - state 2 (active) passes any
// notify; state 0 (None) passes only mType 500; other states block.
// The module gets first refusal; a 699 (released) notify stops there.
// Decode (asm 0x445110-0x445183): object_state @+0x58: == 2 -> pass;
// != 0 -> return; == 0 && mType @+0xC != 500 (0x1F4) -> return. The
// module notify NAMED dispatch (UnitAIModuleVtbl member 5 = +0x14,
// line-arith 7572-7567 = 5; asm 0x445155 call [edx+14h]) - truthy ->
// return; mType == 699 (0x2BB = RGE_UNITAI_ACTION_RELEASED) -> return
// (asm 0x445160 cmp eax, 2BBh); else the owner notify NAMED dispatch
// (TRIBE_PlayerVtbl member 75 = +0x12C, line-arith 1168-1093 = 75;
// asm 0x44517A call [edx+12Ch]) on owner @+0x18. Exit paths: the
// shared epilogue loc_445180 (all paths funnel; the module-truthy
// return included). mType 500 UNKNOWN (the registry candidate).
void __fastcall RGE_Combat_Object__notify_new(
    RGE_Combat_Object* this_, DWORD dummy, int caller, int recipient,
    int mType, int p1, int p2, int p3)
{
    unsigned __int8 state = this_->object_state;                    // asm 0x445116 (@+0x58)
    if (state != 2)                                                 // asm 0x445119-0x44511B
    {
        if (state != 0)                                             // asm 0x44511D-0x44511F
            return;                                                 // asm jnz loc_445180
        if (mType != 500)                                           // asm 0x445125 (cmp 1F4h)
            return;                                                 // asm jnz loc_445180
    }
    if (this_->unitAIValue)                                         // asm 0x445141-0x445143
    {
        if (this_->unitAIValue->vfptr->notify(this_->unitAIValue,   // asm 0x445155 (slot +0x14)
                caller, recipient, mType, p1, p2, p3))
            return;                                                 // asm 0x44515A
    }
    if (mType != RGE_UNITAI_ACTION_RELEASED)                        // asm 0x445160 (cmp 2BBh; 699)
        this_->owner->vfptr->notify(this_->owner, caller,           // asm 0x44517A (slot +0x12C)
            recipient, mType, p1, p2, p3);
}

// Replacement for RGE_Combat_Object::enter_obj. Original: @ 0x00444D20
// (idb_functions.h:1095). Range [0x444D20, 0x444D98) (retn 8 = 2 stack
// args), void. ABI: __thiscall. Virtual: vftable DATA XREF. Caller:
// TRIBE_Combat_Object__enter_obj+13 (the runtime ladder).
// Semantics: enter a transport - after the base enter, when inside an
// object and the id is valid, update the visibility record (the unit
// is now INSIDE - the record zeroes the Unified Map entry).
// Decode (asm 0x444D20-0x444D97): base RGE_Static_Object::enter_obj
// (header-driven - UNHOOKED; asm 0x445430->0x444D30); inside_obj
// @+0x2C non-null AND id @+0x10 >= 0 -> the Update_Unit_Info call
// (the game slot 0x007A1CB8 = game_visible_unit_manager, the batch
// 1b-4 binding; args: id, owner->id, ftol(world_x), ftol(world_y),
// LOR @+0x16D, movsx group @+0x1E, UMV @+0x168, 0, VUR_Ptrs @+0x144)
// then Unified_Map_Value @+0x168 = 0 (asm 0x445646->0x444D46 region).
// Join inventory: loc_444D96 (2 inbound) - covered by the guard
// cascade + the shared tail.
void __fastcall RGE_Combat_Object__enter_obj_new(
    RGE_Combat_Object* this_, DWORD dummy, RGE_Static_Object* target,
    unsigned __int8 force_entry)
{
    RGE_Static_Object__enter_obj((RGE_Static_Object*)this_,         // asm 0x444D30
        target, force_entry);
    if (this_->inside_obj && this_->id >= 0)                        // asm 0x4094435-0x4094441 (@+0x2C / @+0x10)
    {
        Visible_Unit_Manager__Update_Unit_Info(                     // asm 0x444D67+ (:5928)
            // MAP_SIZE: mapsize.cpp re-points this read's disp32 (0x444D81,
            // the mov ecx at 0x444D7F) to the big-map mirror when
            // setMapSizeHooks runs (owner-directed MAP_SIZE rule
            // 2026-11-15): the replacement reads the original game slot
            // 0x007A1CB8.
            *game_visible_unit_manager,                             // asm 0x444D7F
            this_->id,                                              // asm 0x444D4D (mov ecx, [esi+10h] staged)
            this_->owner->id,
            (int)this_->world_x,                                    // asm 0x444D6C-0x444D71 (ftol @0x48)
            (int)this_->world_y,                                    // asm 0x444D52-0x445D57 (ftol @0x4C)
            this_->Large_Object_Radius,                             // asm 0x444D5F (@0x16D)
            this_->master_obj->object_group,                        // asm 0x444D58 (movsx @0x1E)
            this_->Unified_Map_Value,                               // asm 0x444D46 (@0x168)
            0,                                                      // asm 0x444D56 (NewUnifiedMap = 0)
            this_->VUR_Ptrs);                                       // asm 0x444D4C (lea @0x144)
        this_->Unified_Map_Value = 0;                               // asm 0x445646->0x444D46 region (@0x168 = 0)
    }
}

// Replacement for RGE_Combat_Object::setup. Original: @ 0x00443C20
// (idb_functions.h:1096). Range [0x443C20, 0x443C98) (retn 14h = 5
// stack args), returns 1. ABI: __thiscall. Callers:
// RGE_Missile_Object__RGE_Missile_Object+78 / RGE_Missile_Object__
// setup+2F (the runtime ladder calls UP into this).
// Semantics: the Combat-tier template setup - zero the extension
// (the VUR pointer array, the unified-map trio, the formation triple,
// attack_count), run the Action setup, then the tier id/timer/capture
// reset.
// Decode (asm 0x443C20-0x443C97): VUR_Ptrs[9] @+0x144 = 0 (rep stosd
// x9, asm 0x4439C9-0x443936 = the memset); MUP @+0x16C (byte), UMV
// @+0x168 (dword), LOR @+0x16D (byte), formation @+0x179/17A/17B
// (bytes), attack_count @+0x170 (dword) - ALL BEFORE the base call
// (asm 0x443C7B); base RGE_Action_Object::setup (:124 - HOOKED ->
// routes; asm 0x443C7B); type @+0x5E = 50 (0x32); attack_timer
// @+0x174 = 0; capture_flag @+0x178 = 0; return 1.
int __fastcall RGE_Combat_Object__setup_new(
    RGE_Combat_Object* this_, DWORD dummy,
    RGE_Master_Combat_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z)
{
    for (size_t i = 0; i < _countof(this_->VUR_Ptrs); ++i)          // asm 0x4439C9-0x443936 (rep stosd x9)
        this_->VUR_Ptrs[i] = 0;
    this_->Multi_Unified_Points = 0;                                // asm 0x443A51->0x443C51 (@0x16C)
    this_->Unified_Map_Value = 0;                                   // asm 0x443C57 (@0x168)
    this_->Large_Object_Radius = 0;                                 // asm 0x443C5D (@0x16D)
    this_->formation_id = 0;                                        // asm 0x443C63 (@0x179)
    this_->formation_row = 0;                                       // asm 0x443C69 (@0x17A)
    this_->formation_col = 0;                                       // asm 0x40943C6F (@0x17B)
    this_->attack_count = 0;                                        // asm 0x443C75 (@0x170)
    RGE_Action_Object__setup((RGE_Action_Object*)this_,             // asm 0x443C7B (:124) - HOOKED, routes
        (RGE_Master_Action_Object*)tobj, obj_owner, x, y, z);
    this_->type = RGE_MASTER_TYPE_COMBAT_BASE;                      // asm 0x443C80 (byte @+0x5E = 32h)
    this_->attack_timer = 0.0f;                                     // asm 0x443C84 (@0x174)
    this_->capture_flag = 0;                                        // asm 0x443C8A (@0x178)
    return 1;                                                       // asm 0x443C92
}

// ============ RUNTIME METHODS (batch C3) ============

// Replacement for RGE_Combat_Object::setup_read. Original: @ 0x00443CA0
// (idb_functions.h:1097). Range [0x443CA0, 0x443D5A) (retn 8 = 2 stack
// args), returns 1. ABI: __thiscall. Callers: RGE_Combat_Object__
// RGE_Combat_Object_read+4A / RGE_Missile_Object__setup_read+2B (the
// runtime ladder calls UP into this).
// Semantics: the Combat-tier load - zero the extension (the VUR pointer
// array, the unified-map trio), run the Action setup_read, then the
// tier id + 8 extension reads.
// Decode (asm 0x443CA0-0x443D59): VUR_Ptrs[9] @+0x144 = 0 (rep stosd
// x9, asm 0x40943CB9-0x40943CBF = the memset); MUP @+0x16C (byte),
// UMV @+0x168 (dword), LOR @+0x16D (byte) - all before the base call
// (asm 0x40943CD8); base RGE_Action_Object::setup_read (:125 - HOOKED
// -> routes; asm 0x40943CD8); type @+0x5E = 50 (0x32); 8 extension
// reads sizeof-exact (formation_row @+0x179 ×1, formation_col
// @+0x17A ×1, formation_id @+0x17B ×1, attack_timer @+0x174 ×4,
// capture_flag @+0x178 ×1, MUP @+0x16C ×1, LOR @+0x16D ×1,
// attack_count @+0x170 ×4). Straight-line; return 1.
int __fastcall RGE_Combat_Object__setup_read_new(
    RGE_Combat_Object* this_, DWORD dummy, int infile,
    RGE_Game_World* gworld)
{
    for (size_t i = 0; i < _countof(this_->VUR_Ptrs); ++i)          // asm 0x443CB3-0x443CBF (rep stosd x9)
        this_->VUR_Ptrs[i] = 0;
    this_->Multi_Unified_Points = 0;                                // asm 0x443CC5 (@0x16C)
    this_->Unified_Map_Value = 0;                                   // asm 0x443CC7 (@0x168)
    this_->Large_Object_Radius = 0;                                 // asm 0x443CCD (@0x16D)
    RGE_Action_Object__setup_read((RGE_Action_Object*)this_,        // asm 0x443CD8 (:125) - HOOKED, routes
        infile, gworld);
    this_->type = RGE_MASTER_TYPE_COMBAT_BASE;                      // asm 0x443CE7 (byte @+0x5E = 32h)
    rge_read(infile, &this_->formation_row, sizeof(this_->formation_row));    // asm 0x443CEB (@0x179, 1)
    rge_read(infile, &this_->formation_col, sizeof(this_->formation_col));    // asm 0x443CFA (@0x17A, 1)
    rge_read(infile, &this_->formation_id, sizeof(this_->formation_id));      // asm 0x443D09 (@0x17B, 1)
    rge_read(infile, &this_->attack_timer, sizeof(this_->attack_timer));      // asm 0x443D18 (@0x174, 4)
    rge_read(infile, &this_->capture_flag, sizeof(this_->capture_flag));      // asm 0x443D27 (@0x178, 1)
    rge_read(infile, &this_->Multi_Unified_Points, sizeof(this_->Multi_Unified_Points));   // asm 0x443D32 (@0x16C, 1)
    rge_read(infile, &this_->Large_Object_Radius, sizeof(this_->Large_Object_Radius));     // asm 0x443D3D (@0x16D, 1)
    rge_read(infile, &this_->attack_count, sizeof(this_->attack_count));      // asm 0x443D4C (@0x170, 4)
    return 1;                                                       // asm 0x443D54
}

// Replacement for RGE_Combat_Object::save. Original: @ 0x00443D60
// (idb_functions.h:1098). Range [0x443D60, 0x443DE8) (retn 4 = 1 stack
// arg), void. ABI: __thiscall. Callers: RGE_Missile_Object__save+A /
// TRIBE_Combat_Object__save+B (the runtime ladder calls UP into this).
// Semantics: the Combat-tier save - the base Action save + 8 extension
// writes (the same 8 fields as setup_read, sizeof-exact).
// Decode (asm 0x443D60-0x443DE7): base RGE_Action_Object::save (HOOKED
// -> routes; asm 0x443D69); 8 extension writes sizeof-exact
// (formation_row @+0x179 ×1, formation_col @+0x17A ×1, formation_id
// @+0x17B ×1, attack_timer @+0x174 ×4, capture_flag @+0x178 ×1,
// MUP @+0x16C ×1, LOR @+0x16D ×1, attack_count @+0x170 ×4).
// Straight-line, no jcc.
void __fastcall RGE_Combat_Object__save_new(
    RGE_Combat_Object* this_, DWORD dummy, int outfile)
{
    RGE_Action_Object__save((RGE_Action_Object*)this_, outfile);    // asm 0x443D69 - HOOKED, routes
    rge_write(outfile, &this_->formation_row, sizeof(this_->formation_row));    // asm 0x443D78 (@0x179, 1)
    rge_write(outfile, &this_->formation_col, sizeof(this_->formation_col));    // asm 0x443D87 (@0x17A, 1)
    rge_write(outfile, &this_->formation_id, sizeof(this_->formation_id));      // asm 0x443D96 (@0x17B, 1)
    rge_write(outfile, &this_->attack_timer, sizeof(this_->attack_timer));      // asm 0x443DA5 (@0x174, 4)
    rge_write(outfile, &this_->capture_flag, sizeof(this_->capture_flag));      // asm 0x443DB4 (@0x178, 1)
    rge_write(outfile, &this_->Multi_Unified_Points, sizeof(this_->Multi_Unified_Points));   // asm 0x443DC3 (@0x16C, 1)
    rge_write(outfile, &this_->Large_Object_Radius, sizeof(this_->Large_Object_Radius));     // asm 0x443DD2 (@0x16D, 1)
    rge_write(outfile, &this_->attack_count, sizeof(this_->attack_count));      // asm 0x443DE1 (@0x170, 4)
}

// Replacement for RGE_Combat_Object::get_armor. Original: @ 0x00444B10
// (idb_functions.h:1088). Range [0x444B10, 0x444BBE) (retn 8 = 2 stack
// args), void. ABI: __thiscall. Virtual: vftable DATA XREF.
// Semantics: the two-phase armor search - phase 1 scans the master's
// armor array for type-4 armors (the max value); phase 2 (if phase 1
// found nothing) falls back to base_armor + the all-values max; then
// copies the master's orig_armor.
// Decode (asm 0x444B10-0x444BBD): *cur = -1 (word @esi init 0FFFFh);
// the armor ptr non-null gate ([master+0x134] = armor @308); the
// armor_num @+0x132 = 306 loop; the type == 4 word cmp at element +0;
// the value word at element +2; the max-keep; the fallback: base_armor
// @+0x130 = 304 + the all-values scan (no type filter); the
// orig_armor copy from master @+0x16C = 364 (the Combat master
// extension @300-376, the master-side walk). The element stride =
// 4 bytes (RGE_Armor_Weapon_Info = {i16 type, i16 value}).
// Field access: `armor` ptr @+0x134 = 308, `armor_num` @+0x132 = 306,
// `base_armor` @+0x130 = 304, `orig_armor` @+0x16C = 364.
// The loops use `armor_num` (the runtime count field, not a declared
// array dimension). Exit paths: the shared tail loc_444BAB.
void __fastcall RGE_Combat_Object__get_armor_new(
    RGE_Combat_Object* this_, DWORD dummy, __int16* cur_armor_in,
    __int16* orig_armor_in)
{
    *cur_armor_in = -1;                                             // asm 0x444B16 (word 0FFFFh)
    if (this_->master_obj->armor)                                   // asm 0x444B1E-0x444B26 (armor @+0x134)
    {
        for (int i = 0; i < this_->master_obj->armor_num; ++i)      // asm 0x444B2A-0x444B5C (armor_num @+0x132)
        {
            if (this_->master_obj->armor[i].type == 4)              // asm 0x444B39 (word cmp 4)
            {
                if (this_->master_obj->armor[i].value > *cur_armor_in)   // asm 0x40944B43-0x40944B4A (value @+2)
                    *cur_armor_in = this_->master_obj->armor[i].value;   // asm 0x444B4C
            }
        }
    }
    if (*cur_armor_in == -1)                                        // asm 0x444B5E-0x444B62
    {
        *cur_armor_in = this_->master_obj->base_armor;              // asm 0x444B67 (@+0x130 = 304)
        if (this_->master_obj->armor)                               // asm 0x444B74-0x40944B7C
        {
            for (int j = 0; j < this_->master_obj->armor_num; ++j)  // asm 0x444B89-0x40944BA9
            {
                if (this_->master_obj->armor[j].value > *cur_armor_in)   // asm 0x40944B8F-0x40944B94 (no type filter)
                    *cur_armor_in = this_->master_obj->armor[j].value;   // asm 0x40944B99
            }
        }
    }
    *orig_armor_in = this_->master_obj->orig_armor;                 // asm 0x444BB2-0x40944BBB (@+0x16C = 364)
}

// Replacement for RGE_Combat_Object::transform. Original: @ 0x00443E20
// (idb_functions.h:1089). Range [0x443E20, 0x443ED2) (retn 4 = 1 stack
// arg), void. ABI: __thiscall. Virtual: vftable DATA XREF. Caller:
// TRIBE_Combat_Object__transform+39 (the runtime ladder).
// Semantics: the master transform with the visibility bracket - update
// the visibility records (clearing the old UMV), run the base Action
// transform, then update again (setting the new UMV).
// Decode (asm 0x443E20-0x443ED1): id @+0x10 >= 0 gate -> the PRE
// Update_Unit_Info (the game slot 0x007A1CB8 = game_visible_unit_
// manager, the 1b-4 binding; PriorUnifiedMap = UMV @+0x168 = 360,
// NewUnifiedMap = 0, the standard arg set); the base RGE_Action_
// Object::transform (:126 - HOOKED -> routes; asm 0x443E7B); the
// id >= 0 gate -> the POST Update_Unit_Info (PriorUnifiedMap = 0,
// NewUnifiedMap = UMV @+0x168). Join inventory: loc_443E74 (1 inbound
// jl), loc_443ED0 (1 inbound jl) - covered by the guard + the shared
// tail. Exit paths: 1 asm return (the shared epilogue).
void __fastcall RGE_Combat_Object__transform_new(
    RGE_Combat_Object* this_, DWORD dummy, RGE_Master_Static_Object* obj)
{
    if (this_->id >= 0)                                             // asm 0x443E27-0x40943E29
    {
        Visible_Unit_Manager__Update_Unit_Info(                     // asm 0x443E6F (the PRE call)
            // MAP_SIZE: mapsize.cpp re-points this read's disp32 (0x443E62,
            // the mov ecx at 0x443E60) to the big-map mirror when
            // setMapSizeHooks runs (owner-directed MAP_SIZE rule
            // 2026-11-15): the replacement reads the original game slot
            // 0x007A1CB8.
            *game_visible_unit_manager,                             // asm 0x443E60
            this_->id,                                              // asm push edi (Object_Id)
            this_->owner->id,                                       // asm 0x443E67 (id @0xA0)
            (int)this_->world_x,                                    // asm 0x40943E54-0x40943E58 (ftol @0x48)
            (int)this_->world_y,                                    // asm 0x40943E3A-0x40943E4F (ftol @0x4C)
            this_->Large_Object_Radius,                             // asm 0x40943E47 (@0x16D)
            this_->master_obj->object_group,                        // asm 0x40943E40 (movsx @0x1E)
            this_->Unified_Map_Value,                               // asm 0x40943E2E (@0x168; PriorUnifiedMap)
            0,                                                      // asm 0x40943E3E (NewUnifiedMap = 0)
            this_->VUR_Ptrs);                                       // asm 0x40943E34 (lea @0x144)
    }
    RGE_Action_Object__transform((RGE_Action_Object*)this_, obj);   // asm 0x40943E7B (:126) - HOOKED, routes
    if (this_->id >= 0)                                             // asm 0x443E80-0x40943E85
    {
        Visible_Unit_Manager__Update_Unit_Info(                     // asm 0x443ECB (the POST call)
            // MAP_SIZE: mapsize.cpp re-points this read's disp32 (0x443EC5,
            // the mov ecx at 0x443EC3) to the big-map mirror when
            // setMapSizeHooks runs (owner-directed MAP_SIZE rule
            // 2026-11-15): the replacement reads the original game slot
            // 0x007A1CB8.
            *game_visible_unit_manager,                             // asm 0x443EC3
            this_->id,                                              // asm push edi
            this_->owner->id,                                       // asm 0x443EBC (@0xA0)
            (int)this_->world_x,                                    // asm 0x40943EB0-0x40943EB4 (ftol @0x48)
            (int)this_->world_y,                                    // asm 0x40943E90-0x40943E9F (ftol @0x4C)
            this_->Large_Object_Radius,                             // asm 0x40943EA3 (@0x16D)
            this_->master_obj->object_group,                        // asm 0x40943E9B (movsx @0x1E)
            0,                                                      // asm 0x40943EA1 (PriorUnifiedMap = 0)
            this_->Unified_Map_Value,                               // asm 0x40943E9A (@0x168; NewUnifiedMap)
            this_->VUR_Ptrs);                                       // asm 0x40943E93 (lea @0x144)
    }
}

// Replacement for RGE_Combat_Object::damageCapability. Original:
// @ 0x00445F60 (idb_functions.h:1122). Range [0x445F60, 0x00446014)
// (retn 4 = 1 stack arg), double return. ABI: __thiscall. Virtual:
// vftable DATA XREF (Combat + Missile vftables).
// Semantics: the dual-path damage calculator - with a module and a
// valid attacker, the AI can-attack check gates the calc_attack_
// modifier + calculateDamage dispatch chain; without, the weapon-sum
// fallback scans the master's weapon array for positive values.
// Decode (asm 0x445F60-0x00446013):
//  PATH A (module): attackingUnit non-null + unitAI (:261) non-null
//   + canAttackUnit(module, attackingUnit) (UnitAIModuleVtbl slot 19
//   = +0x4C, the 1-arg form; asm 0x40945F81 call [edx+4Ch]) ->
//   the calc_attack_modifier NAMED dispatch (slot 95 = +0x17C,
//   line-arith 11781-11686?? - the /Fa verifies the emitted offset;
//   asm 0x40945F8D call [eax+17Ch]; the double return captured into
//   a float local = the rule-(e) store-round, asm fstp m32
//   0x40945F99) -> the calculateDamage dispatch on the ATTACKER's
//   vtbl (slot 59 = +0xEC; asm 0x40945FB7 call [edx+0ECh]) with
//   (weapon_num @+0x138 movsx, weapon @+0x13C, modifier, owner,
//   this) - the return DISCARDED (void in the original).
//  PATH B (no module or no attacker): the weapon-sum fallback -
//   result = 0.0; the weapon array (@+0x13C) scanned for positive
//   __int16 values: `fild` (the i16-to-double conversion), `fcom
//   const_f_zero`, `test ah,41h` - jnz → discard (the {<=0, NaN}
//   discard set), else `faddp` (add) - byte-faithful; the weapon
//   element stepped by 4 bytes (the RGE_Armor_Weapon_Info stride,
//   the .c's `v9 += 2` __int16 render = the 8-byte pair stride /
//   2 = the element stride; the asm `add ecx, 4` ✓).
//  The x87: PATH B's fcom + test ah,41h / jnz → discard is the
//   {<=, NaN} DISCARD set → the written `val > 0.0` compiles
//   unordered-false → NaN → no add ✓ byte-faithful.
// Exit paths: 2 asm returns (the shared epilogue + the fallback).
double __fastcall RGE_Combat_Object__damageCapability_new(
    RGE_Combat_Object* this_, DWORD dummy, RGE_Static_Object* attackingUnit)
{
    double result = 0.0;                                            // asm 0x445FD0 (fld const_f_zero)
    if (attackingUnit && RGE_Static_Object__unitAI(                 // asm 0x40945F68-0x40945F73 (:261)
            (RGE_Static_Object*)this_))
    {
        UnitAIModule* module = RGE_Static_Object__unitAI(           // asm 0x40945F77 (:261)
            (RGE_Static_Object*)this_);
        if (module->vfptr->canAttackUnit(module, (RGE_Static_Object_idb*)attackingUnit))    // asm 0x40945F81 (slot 19 = +0x4C)
        {
            double modifier = this_->vfptr->calc_attack_modifier(   // asm 0x40945F8D (slot 95 = +0x17C)
                this_, (RGE_Static_Object_idb*)attackingUnit);
            float modifier_f = (float)modifier;                     // asm 0x40945F99 (the fstp m32 store-round)
            // The original transparently passes calculateDamage's st0
            // through as damageCapability's return (asm: no x87
            // management between the call and retn). The .c's void
            // cast-form render was the artifact.
            return attackingUnit->calculateDamage(// asm 0x40945FB7 (slot 59 = +0xEC)
                this_->master_obj->weapon_num,
                this_->master_obj->weapon,
                modifier_f, (RGE_Player*)this_->owner,
                (RGE_Static_Object*)this_);
        }
        // the canAttackUnit false case: result stays 0.0 → return 0.0
    }
    else
    {
        // the weapon-sum fallback (asm 0x445FCD-0x0044600D)
        RGE_Master_Combat_Object* master = this_->master_obj;
        if (master->weapon)                                         // asm 0x445FDC-0x445FDE (weapon @+0x13C)
        {
            int count = master->weapon_num;                         // asm 0x445FE0 (movsx @+0x138)
            if (count > 0)
            {
                for (int i = 0; i < count; ++i)                     // asm 0x445FEE-0x44600D (the element stride 4)
                {
                    double val = (double)master->weapon[i].value;   // asm 0x445FEE (fild the i16→double)
                    if (val > 0.0)                                  // asm 0x445FF9-0x40946004 (test ah,41h / jnz → discard)
                        result += val;                              // asm 0x40946006 (faddp)
                }
            }
        }
    }
    return result;                                                  // asm 0x40946083/0x40946084
}

// Replacement for RGE_Combat_Object::transport. Original: @ 0x00445D00
// (idb_functions.h:1105). Range [0x445D00, 0x445DE3) (lst :121321/:121430,
// retn 10h = 4 stack args + this), returns int. ABI: __thiscall. Virtual:
// vftable DATA XREF on BOTH RGE_Combat_Object__vftable and RGE_Missile_
// Object__vftable (asm :121321-:121322) - the entry hook covers both
// dispatches. SEH: MSVC frame (push -1 / transport_SEH @0x445D02, fs:0
// swap 0x445D07-0x445D0E; the MSVC epilogue restores fs:0 and pops,
// 0x445DD1-0x445DE0); unwind funclet chunk 0x6415D0-0x6415DA = mov
// eax,[ebp-10h] / call operator_delete on the RAW allocation (scope
// stru_67FDC0) - folded per the make_new_obj precedent.
// Semantics: the transport MISSION BUILDER. If the current action already
// IS the Transport action (type() == 12, RGE_ACTION_TYPE registry), re-
// drive it with work(0, tX, tY, tZ) - obj_in = null, the existing action
// keeps its own target. Otherwise build a fresh RGE_Action_Transport
// (ctor obj_in = this, task_in = 0, position = the PARAMETERS), replace
// the whole action list (delete_list + set_only_action), and start the
// new action with work(this, tX, tY, tZ). Returns 1, or 0 when the
// allocation/construction failed (the action list is left untouched on
// that path). `unconditional` is read by NEITHER the asm nor the
// pseudocode (kept for ABI; retn 10h pops it).
int __fastcall RGE_Combat_Object__transport_new(
    RGE_Combat_Object* this_, DWORD dummy, float tX, float tY, float tZ,
    int unconditional)
{
    if (RGE_Action_List__have_action(this_->actions) == 1)              // asm 0x445D1C-0x445D27 (cmp al,1; actions @+0x13C)
    {
        RGE_Action* current = RGE_Action_List__get_action(this_->actions);        // asm 0x445D2B-0x445D31
        if (current->vfptr->type(current) == RGE_ACTION_TYPE_TRANSPORT)           // asm 0x445D36-0x445D3D (slot 6 @+0x18; cmp ax,0Ch)
        {
            RGE_Action* existing = RGE_Action_List__get_action(this_->actions);   // asm 0x445D43-0x445D49 (second fetch - faithful)
            existing->vfptr->work(existing, 0, tX, tY, tZ);               // asm 0x445DC9 (slot 13 @+0x34; pushes z,y,x,0 @0x445D4E-0x445D5F)
            return 1;                                                     // asm 0x445DCC
        }
    }
    RGE_Action_Transport* allocated =
        (RGE_Action_Transport*)operator_new_internal(sizeof(RGE_Action_Transport));  // asm 0x445D65-0x445D6F (push 40h; sizeof walked 64 = 0x40)
    RGE_Action* fresh = 0;                                                // asm 0x445D7B (edi = 0); EH state 0 @0x445D7F
    __try
    {
        if (allocated != 0)                                               // asm 0x445D7D-0x445D83 (cmp eax,edi / jz)
            fresh = (RGE_Action*)RGE_Action_Transport__RGE_Action_Transport(
                allocated, (RGE_Action_Object*)this_, 0, tX, tY, tZ);     // asm 0x445D85-0x445D90 (pushes z,y,x,task_in=0,obj_in=this_)
    }
    __except (operator_delete_internal(allocated), EXCEPTION_CONTINUE_SEARCH)  // SEH chunk 0x6415D0-0x6415DA (operator_delete, raw allocation)
    {
    }
    if (fresh == 0)                                                       // asm 0x445D97-0x445DA1 (EH state -> -1 @0x445D99)
        return 0;                                                         // asm 0x445DA3-0x445DA5
    RGE_Action_List__delete_list(this_->actions);                         // asm 0x445DA7-0x445DAD
    this_->vfptr->set_only_action(this_, fresh);                          // asm 0x445DB2-0x445DB7 (slot 231 @+0x39C)
    fresh->vfptr->work(fresh, (RGE_Static_Object_idb*)this_, tX, tY, tZ);     // asm 0x445DC9 (pushes z,y,x,this_ @0x445DBD-0x445DC6)
    return 1;                                                             // asm 0x445DCC
}

// Replacement for RGE_Combat_Object::unload. Original: @ 0x00445DF0
// (idb_functions.h:1106). Range [0x445DF0, 0x445EEB) (lst :121439/:121553,
// retn 10h), returns int. ABI: __thiscall. Virtual: vftable DATA XREF on
// both Combat and Missile vftables (asm :121439-:121440). SEH: MSVC frame;
// unwind funclet chunk 0x6415F0-0x6415FA = operator_delete on the raw
// allocation (scope stru_67FDE8) - transport's twin chunk, same fold.
// Semantics: the transport TWIN - the UNLOAD order. Same Transport-action
// gate; the THREE deltas vs transport: (a) the existing-action work call
// and the ctor use the OBJECT'S CURRENT POSITION (world_x/world_y/world_z
// @+0x48/+0x4C/+0x50, idb_types.h:1462-1464) instead of the parameters;
// (b) the existing-action work call passes obj_in = this (transport passes
// 0), asm 0x445E4A push esi; (c) the return-0 path is a fully inline
// epilogue (asm 0x445E94-0x445EA6). The FINAL work call on the fresh
// action uses the caller's x/y/z parameters (asm 0x445EBF-0x445ED0).
// `unconditional` is read by NEITHER the asm nor the pseudocode (kept for
// ABI; retn 10h pops it).
int __fastcall RGE_Combat_Object__unload_new(
    RGE_Combat_Object* this_, DWORD dummy, int unconditional, float x,
    float y, float z)
{
    if (RGE_Action_List__have_action(this_->actions) == 1)              // asm 0x445E0A-0x445E15 (cmp al,1)
    {
        RGE_Action* current = RGE_Action_List__get_action(this_->actions);        // asm 0x445E19-0x445E1F
        if (current->vfptr->type(current) == RGE_ACTION_TYPE_TRANSPORT)           // asm 0x445E24-0x445E2B (slot 6 @+0x18; cmp ax,0Ch)
        {
            RGE_Action* existing = RGE_Action_List__get_action(this_->actions);   // asm 0x445E31-0x445E37 (second fetch - faithful)
            existing->vfptr->work(existing, (RGE_Static_Object_idb*)this_,    // asm 0x445ED3 (slot 13 join; pushes world_z,world_y,world_x,this_ @0x445E3C-0x445E4A)
                this_->world_x, this_->world_y, this_->world_z);
            return 1;                                                     // asm 0x445EDB-0x445EE8
        }
    }
    RGE_Action_Transport* allocated =
        (RGE_Action_Transport*)operator_new_internal(sizeof(RGE_Action_Transport));  // asm 0x445E52-0x445E5C (push 40h)
    RGE_Action* fresh = 0;                                                // asm 0x445E86 (xor edi,edi); EH state 0 @0x445E62
    __try
    {
        if (allocated != 0)                                               // asm 0x445E60-0x445E6A (test eax,eax / jz)
            fresh = (RGE_Action*)RGE_Action_Transport__RGE_Action_Transport(
                allocated, (RGE_Action_Object*)this_, 0, this_->world_x,  // asm 0x445E6C-0x445E7D (pushes the object's world_z,y,x, task_in=0, obj_in=this_)
                this_->world_y, this_->world_z);
    }
    __except (operator_delete_internal(allocated), EXCEPTION_CONTINUE_SEARCH)  // SEH chunk 0x6415F0-0x6415FA (operator_delete, raw allocation)
    {
    }
    if (fresh == 0)                                                       // asm 0x445E88-0x445E92 (EH state -> -1 @0x445E8A)
        return 0;                                                         // asm 0x445E94-0x445EA6 (inline epilogue)
    RGE_Action_List__delete_list(this_->actions);                         // asm 0x445EA9-0x445EAF
    this_->vfptr->set_only_action(this_, fresh);                          // asm 0x445EB4-0x445EB9 (slot 231 @+0x39C)
    fresh->vfptr->work(fresh, (RGE_Static_Object_idb*)this_, x, y, z);        // asm 0x445ED3 (pushes z,y,x,this_ @0x445EBF-0x445ED0 - the PARAMETERS)
    return 1;                                                             // asm 0x445EDB-0x445EE8
}

// Replacement for RGE_Combat_Object::stopAction. Original: @ 0x00445EF0
// (idb_functions.h:1107). Range [0x445EF0, 0x445F27) (lst :121562/:121589,
// plain retn = 0 stack args + this), returns int (always 1). ABI:
// __thiscall. Virtual: vftable DATA XREF on both Combat and Missile
// vftables (asm :121562-:121563).
// Semantics: the STOP ORDER - tear the whole action list down
// (delete_list), and when the unit is in the ACTIVE state (object_state
// @+0x58 == 2 - the registry's "== 2 = moving/active" case, the same
// gate as the Building own-unit check), re-home the sprite: the
// MASTER-SIDE polymorphic getter get_sprite(master, this) (master vtbl
// slot 1 @+4) then the runtime new_sprite (slot 44 @+0xB0).
int __fastcall RGE_Combat_Object__stopAction_new(
    RGE_Combat_Object* this_, DWORD dummy)
{
    RGE_Action_List__delete_list(this_->actions);                   // asm 0x445EF3-0x445EF7 (actions @+0x13C)
    if (this_->object_state == 2)                                   // asm 0x445EF8-0x445F02 (byte @+0x58, cmp 2)
    {
        RGE_Sprite* sprite = this_->master_obj->vfptr->get_sprite(  // asm 0x445F0B-0x445F0D (slot 1 @+4; pushes (RGE_Static_Object*)this_)
            this_->master_obj, (RGE_Static_Object_idb*)this_);
        this_->vfptr->new_sprite(this_, sprite);                    // asm 0x445F10-0x445F13 (slot 44 @+0xB0; pushes the sprite)
    }
    return 1;                                                       // asm 0x445F1A+0x445F20 / 0x445F21-0x445F27 (both exit sites)
}

// Replacement for RGE_Combat_Object::is_invisible. Original: @ 0x00443950
// (idb_functions.h:1068). Range [0x443950, 0x443989) (lst :117330/:117364,
// retn 4 = 1 stack arg), returns unsigned __int8. ABI: __thiscall.
// Virtual: vftable DATA XREF on both Combat and Missile vftables.
// Semantics: the visibility override for ACTIVE special actions: while
// the current action is an Attack2 (9) or a Convert (0x68 = 104, the
// RGE_ACTION_TYPE registry), the unit reports NOT invisible (0) to
// player_id; every other case forwards to the base
// RGE_Static_Object__is_invisible (:3796, 0x0054BAB0 - NOT hooked; the
// asm calls it directly, 0x443983 - the faithful form per the
// hook-routing ruling). The type tests read the DIRECT action_type
// field word @+4 (idb_types.h:15022), NOT the type() virtual (asm
// 0x443966 mov ax,[eax+4]).
unsigned __int8 __fastcall RGE_Combat_Object__is_invisible_new(
    RGE_Combat_Object* this_, DWORD dummy, int player_id)
{
    if (this_->actions)                                             // asm 0x443953-0x44395B (actions @+0x13C, test/jz)
    {
        RGE_Action* current = RGE_Action_List__get_action(this_->actions);   // asm 0x44395D
        if (current != 0)                                           // asm 0x443962-0x443964 (test/jz)
        {
            if (current->action_type == RGE_ACTION_TYPE_ATTACK2     // asm 0x443966-0x44396E (word @+4; cmp ax,9)
                || current->action_type == RGE_ACTION_TYPE_CONVERT) // asm 0x443970-0x443974 (cmp ax,68h)
                return 0;                                           // asm 0x443976-0x443979 (xor al,al)
        }
    }
    return RGE_Static_Object__is_invisible(                         // asm 0x44397C-0x443983 (the direct base call; player_id push 0x443980)
        (RGE_Static_Object*)this_, player_id);
}

// Replacement for RGE_Combat_Object::get_weapon. Original: @ 0x00444BD0
// (idb_functions.h:1089). Range [0x444BD0, 0x444C7A) (lst :119421/:119497,
// retn 8 = 2 stack args), void. ABI: __thiscall. Virtual: vftable DATA
// XREF on both Combat and Missile vftables.
// Semantics: the CURRENT weapon value: the max value among the master's
// weapon types (0-initialized accumulator), with the all-values max as
// the fallback when the filtered max is 0; then the unconditional
// orig_weapon copy @+0x16E = 366. Element stride 4 (RGE_Armor_Weapon_
// Info = {i16 type, i16 value}); the 3/4 namespace is the PARTIAL
// registry entry (DECOMP_GAME_FACTS.md).
// BAKED (owner-directed VERBATIM ADOPTION 2026-11-23): the LIVE
// object-panel hook objpanel.cpp:400 (setHook 0x00444BD0 via dllmain
// :318 VER_EF) was itself a FULL replacement of this function - its
// handler (objpanel.cpp:319-365) is taken VERBATIM as this body (the
// AA-unit delta: object_group 9/16/33/40/55 filter type == 0; the
// registry object_group values; the handler's `obj` parameter name and
// statement order kept verbatim). Path-exact vs the original for
// non-AA units: the one-pass filtered/total selection == the asm's
// two-pass form (the 0x444C27 gate == !(max_filtered > 0), the
// fallback loop == max_total, both accumulators max-from-0; the
// incoming *cur is never observed - 0-init precedes every read). The
// per-iteration master/weapon re-reads match the asm's own reloads.
// The objpanel call site retires; the handler definition is removed
// (the code lives here now - tombstone at objpanel.cpp:319).
// ASM-ORIGINAL decode (the citation source): *cur = 0 (0x444BD6); the
// weapon ptr gate ([master+0x13C] = weapon @316, 0x444BDB-0x444BE6);
// weapon_num @+0x138 = 312 loops (movsx signed); the type word at
// element +0 (cmp 4 @0x444C00 / cmp 3 @0x444C06 - the verbatim body's
// 3||4 order is commutative); the value word at element +2
// (0x444C0C-0x444C13, signed); the max-keep (0x444C15); the fallback
// gate (0x444C27-0x444C2B) + the all-values scan (0x444C45-0x444C65);
// the orig_weapon copy (0x444C67-0x444C77).
void __fastcall RGE_Combat_Object__get_weapon_new(RGE_Combat_Object* obj, DWORD dummy, __int16* cur_weapon_in, __int16* orig_weapon_in)
{
    UNREFERENCED_PARAMETER(dummy);

    *cur_weapon_in = 0;                                             // asm 0x444BD6 (word store)
    if (obj->master_obj->weapon)                                    // asm 0x444BDB-0x444BE6 (master_obj @+0x14; weapon @+0x13C)
    {
        bool is_aa_unit;                                            // baked: objpanel.cpp:326 (no asm counterpart - the AA delta)
        switch (obj->master_obj->object_group)                      // the i16 @+30 (the registry field)
        {
        case 9:     //aa turret
        case 16:    //aa destroyer 1
        case 33:    //aa mobile
        case 40:    //aa destroyer 2
        case 55:    //aa trooper
            is_aa_unit = true;
            break;
        default:
            is_aa_unit = false;
            break;
        }
        __int16 max_total = 0;                                      // baked: objpanel.cpp:340 (== the asm fallback accumulator 0x444C3A)
        __int16 max_filtered = 0;                                   // the asm's [edi] accumulator (0-init 0x444BD6)
        for (int i = 0; i < obj->master_obj->weapon_num; i++)       // asm 0x444BF3-0x444C25 (weapon_num @+0x138, movsx signed)
        {
            if (is_aa_unit)                                         // baked: objpanel.cpp:344 (the asm's filter: type 4/3)
            {
                if (obj->master_obj->weapon[i].type == 0)           // baked: objpanel.cpp:346 (AA units: type 0)
                    if (obj->master_obj->weapon[i].value > max_filtered)     // asm 0x444C0C-0x444C13 (value @element+2, signed cmp)
                        max_filtered = obj->master_obj->weapon[i].value; // asm 0x444C15 (the max-keep store)
            }
            else
            {
                if (obj->master_obj->weapon[i].type == 3 || obj->master_obj->weapon[i].type == 4)   // asm 0x444C00-0x444C0A (cmp 4 / cmp 3; the verbatim 3||4 order - commutative)
                    if (obj->master_obj->weapon[i].value > max_filtered)
                        max_filtered = obj->master_obj->weapon[i].value;
            }
            if (obj->master_obj->weapon[i].value > max_total)       // baked: objpanel.cpp:356 (== the asm fallback loop 0x444C45-0x444C65)
                max_total = obj->master_obj->weapon[i].value;
        }
        if (max_filtered > 0)                                       // baked: objpanel.cpp:359 (== the asm gate 0x444C27-0x444C2B)
            *cur_weapon_in = max_filtered;                          // asm 0x444C55 (the [edi] max-keep store)
        else
            *cur_weapon_in = max_total;                             // asm 0x444C45-0x444C65 (the fallback all-values max)
    }
    *orig_weapon_in = obj->master_obj->orig_weapon;                 // asm 0x444C67-0x444C77 (orig_weapon @+0x16E = 366)
}

// Replacement for RGE_Combat_Object::do_attack. Original: @ 0x00444720
// (idb_functions.h:1083). Range [0x444720, 0x44814) (lst :118855/:118965,
// retn 18h = 6 stack args), returns unsigned __int8. ABI: __thiscall.
// Virtual: vftable DATA XREF on both Combat and Missile vftables + CODE
// XREF TRIBE_Combat_Object__do_attack+167 (the TRIBE tier forwards into
// this implementation, asm :118855 - the entry hook covers that call).
// Semantics: the DAMAGE APPLIER. target == 0 -> return 0. When the
// master's area_effect_range (@+0x148 = 328) is > 0, the AREA branch:
// this->vfptr->area_attack(this, x, y, z, attacker, 0, target) (slot
// 237 @+0x3B4, RGE_Combat_ObjectVtbl :11571) - the u8 result is
// DISCARDED, return 1. When it is <= 0 (or NaN - the fcomp/test ah,41h
// jnz taken-set, written X87_LE_NAN per the mask-partition rule), the
// p1 = 600 = RGE_UNITAI_ACTION_ATTACK (the UnitAI action enum,
// owner-directed), p2 = target->id, p3 = 0) (slot 120 @+0x1E0, :11454;
// mType 512 is the 5xx notify namespace - UNKNOWN, the DECOMP_GAME_FACTS
// registry); then the hit roll roll = (rand_internal() *
// 100) / RAND_MAX_INTERNAL - the 0x80010003 imul + sar 0Eh sequence IS
// the signed /RAND_MAX (= consts.h RAND_MAX_INTERNAL) magic, scripted
// exact 32768/32768, roll 0..100; rand_internal (common.h:154,
// common.cpp:367, 0x00632BDD) is the GAME'S binary rand - the game's
// own CRT state, never the DLL CRT. Damage lands iff base_hit_chance
// (@+0x156 = 342) > 0 AND (roll <= base_hit_chance OR missile_id
// (@+0x154 = 340) >= 0 - ranged units always connect, the missile-less
// (id < 0) need the roll). On hit: modifier = this->vfptr->
// calc_attack_modifier(this, target) (slot 95 @+0x17C, :11429 - HOOKED,
// routes), damage = (float)(modifier * damage_mod) (the x87 fmul
// consumes the st0 return at 53-bit; the fstp m32 is the single
// store-round, asm 0x447E3-0x447F0), then target->vfptr->damage(target,
// weapon_num, weapon, damage, this->owner, attacker, first_call = 1)
// (slot 58 @+0xE8, :11392 - dispatched ON THE TARGET, ecx = edi
// 0x44808). Return 1 on both non-null paths.
unsigned __int8 __fastcall RGE_Combat_Object__do_attack_new(
    RGE_Combat_Object* this_, DWORD dummy, RGE_Static_Object* target,
    RGE_Combat_Object* attacker, float x, float y, float z, float damage_mod)
{
    if (target == 0)                                                // asm 0x44728-0x4472A (test/jnz)
        return 0;                                                   // asm 0x4472C-0x44730 (xor al,al)
    if (X87_LE_NAN(this_->master_obj->area_effect_range, 0.0f))     // asm 0x44736-0x44747 (fcomp const_f_zero; test ah,41h; jnz -> single-target: the taken set {<=, NaN})
    {
        this_->vfptr->notify(this_, this_->id, this_->id, 512,      // asm 0x44771-0x4478A (slot 120 @+0x1E0; pushes 0,target->id,600,512,id,id @0x44779-0x44787)
            RGE_UNITAI_ACTION_ATTACK, target->id, 0);               // p1 600 = RGE_UNITAI_ACTION_ATTACK (the UnitAI action enum, owner-directed); mType 512: the 5xx notify namespace UNKNOWN (DECOMP_GAME_FACTS)
        int n = rand_internal() * 100;                              // asm 0x44790-0x447A0 (lea/lea/shl = *100; rand_internal = common.h:154, the game's binary rand @0x00632BDD - the game's CRT state, common.cpp:367)
        int roll = n / RAND_MAX_INTERNAL;                           // asm 0x4479B-0x447B2 (imul 80010003h / sar 0Eh = the signed /RAND_MAX magic; scripted exact, roll 0..100; RAND_MAX_INTERNAL = consts.h)
        if (this_->master_obj->base_hit_chance > 0)                 // asm 0x447B4-0x447BE (word @+0x156 = 342; test ax/jle -> the shared return)
        {
            if (roll <= this_->master_obj->base_hit_chance          // asm 0x447C0-0x447C5 (movsx + cmp/jle -> the hit join loc_4447D1, 2 inbound)
                || this_->master_obj->missile_id >= 0)              // asm 0x447C7-0x447CF (word @+0x154 = 340; jl -> the shared return)
            {
                double modifier = this_->vfptr->calc_attack_modifier(this_, (RGE_Static_Object_idb*)target);   // asm 0x447D1-0x447D6 (slot 95 @+0x17C - HOOKED, routes)
                float damage = (float)(modifier * damage_mod);      // asm 0x447E3-0x447F0 (fmul [damage_mod] at 53-bit; fstp m32 - the single store-round)
                target->damage(// asm 0x4480A (slot 58 @+0xE8 - dispatched ON THE TARGET, ecx = edi @0x44808)
                    this_->master_obj->weapon_num,                  // asm 0x447F9-0x44807 (movsx @+0x138 = 312)
                    this_->master_obj->weapon,                      // asm 0x447F2-0x447F9 ([eax+13Ch] = 316)
                    damage,                                         // asm 0x447F0-0x447F8 (the fstp'd m32 bits pushed)
                    (RGE_Player*)this_->owner,                      // asm 0x447EA-0x447EF (@+0x18; the TRIBE_Player*/RGE_Player* dual-naming cast)
                    (RGE_Static_Object*)attacker,                   // asm 0x447E7-0x447E9 (push ecx = the attacker param)
                    1);                                             // asm 0x447E7 (push 1 = first_call)
            }
        }
        return 1;                                                   // asm 0x44810-0x44814 (the shared tail join loc_444810, 3 inbound)
    }
    this_->vfptr->area_attack(this_, x, y, z, (RGE_Combat_Object *)attacker, 0, (RGE_Static_Object_idb*)target); // asm 0x44749-0x44764 (slot 237 @+0x3B4; pushes target,0,attacker,z,y,x @0x44753-0x44761)
    return 1;                                                       // asm 0x4476B-0x4476E (the area_attack u8 result discarded)
}

// Replacement for RGE_Combat_Object::calculateDamage. Original: @ 0x004449C0
// (idb_functions.h:1085). Range [0x4449C0, 0x444ADB) (lst :119169/:119292,
// retn 14h = 5 stack args), returns double (the st0-return corollary -
// the caller damage chains fsub off st0 with no intermediate store).
// ABI: __thiscall. Virtual: vftable DATA XREF on both Combat and Missile
// vftables.
// Semantics: how much damage THIS unit takes from one weapon hit - the
// per-element net = (weapon value - armor value) x terrain x modifier,
// summed over the weapon_num elements, positive terms only, then
// clamped to >= 1.0.
// Terrain multiplier: 1.0; when this->tile (@+0x28 = 40) != 0 AND the
// master's defense_terrain_bonus (@+0x140 = 320) >= 0, the chain
// owner->world (@+0x8C, TRIBE_Player :727) -> terrains (@+0x60 = 96,
// TRIBE_World :970, float**) -> [defense_terrain_bonus] (the float*
// per-terrain row) -> [tile->terrain_type (@+5)] - the float
// multiplier; < 1.0 (or NaN) clamps to 1.0 (the defensive terrain never
// reduces below no-bonus; asm 0x444A0C-0x444A1D).
// The per-element armor: armor_value = base_armor (@+0x130 = 304); when
// armor (@+0x134 = 308) != 0 AND armor_num (@+0x132 = 306) > 0, the
// type scan (armor[j].type == weapon.type) OVERWRITES armor_value per
// match - the scan runs to completion, the LAST match wins (asm
// 0x444A69-0x444A7A, dec ecx/jnz, no early exit).
// The term: the fild pair (the weapon value @element+2 signed, the
// armor value) -> fsubp = weapon - armor at 53-bit; x terrain_mult;
// x attack_modifier (both m32 floats - exact x87 widening = the written
// double promotes, rule (j)); accumulated ONLY when term > 0.0 (fcom
// const_d_zero .rdata:006546B0, test ah,41h jnz -> the skip; the
// literal > matches the polarity table - NaN skips); the accumulation
// rides st1/st0 at 53-bit. The final clamp: result < 1.0 (or NaN) ->
// 1.0 (fcom const_f_one, test ah,1; written X87_LT_NAN). RESTRUCTURE:
// the asm re-tests weapon_num before the clamp (0x444AB7-0x444ABD) -
// provably redundant (the loop entry guard 0x444A3B already required
// weapon_num > 0 and nothing modifies the argument) - collapsed.
// BAKED: the casts.cpp special-weapon term-skip (onGetDamage @0x0044A7C,
// retired with the specialDamage move to damage_new): elements with
// type >= 41 contribute NO term (the asm jmp 0x444AA9 past the
// fild/fsubp/fmul/fcom chain - the effects apply via damage_new's
// special pass instead). The skip reads damage[i].type directly (the
// asm's dx was the scan's loaded copy; on the no-scan path it was a
// stale-register leftover - owner-approved deterministic form).
double __fastcall RGE_Combat_Object__calculateDamage_new(
    RGE_Combat_Object* this_, DWORD dummy, int weapon_num,
    RGE_Armor_Weapon_Info* damage, float attack_modifier,
    RGE_Player* attacking_player, RGE_Static_Object* attackingObject)
{
    RGE_Master_Combat_Object* master = this_->master_obj;           // asm 0x4449C3 (mov edx,[ecx+14h])
    RGE_Tile* tile = this_->tile;                                   // asm 0x4449C6 (mov eax,[ecx+28h]; tile @+0x28 = 40)
    int armor_num = master->armor_num;                              // asm 0x4449CB (movsx word @+0x132 = 306)
    RGE_Armor_Weapon_Info* armor = master->armor;                   // asm 0x4449D4 (@+0x134 = 308)
    int base_armor = master->base_armor;                            // asm 0x444A43-0x444A4A (movsx word @+0x130 = 304; the per-iteration reset source)
    float terrain_mult = 1.0f;                                      // asm 0x4449DA (var_10 = 1.0)
    if (tile != 0                                                   // asm 0x4449E2-0x4449E4 (test/jz)
        && master->defense_terrain_bonus >= 0)                      // asm 0x4449E6-0x4449F0 (word @+0x140 = 320; test si/jl - signed)
    {
        float m = this_->owner->world->terrains[                    // asm 0x4449F2-0x4449A6 (owner @+0x18; world @+0x8C TRIBE_Player :727; terrains @+0x60 TRIBE_World :970; the [bonus] row)
            master->defense_terrain_bonus][tile->terrain_type];     // asm 0x4449A9 (fld [eax+ebx*4]; terrain_type u8 @+5)
        if (X87_LT_NAN(m, 1.0f))                                    // asm 0x444A10-0x444A1B (fcom const_f_one; test ah,1; jz -> keep; the overwrite set {<1, NaN})
            m = 1.0f;                                               // asm 0x444A1D
        terrain_mult = m;                                           // asm 0x444A0C (the fst var_10 store - the dedicated-local form; the intermediate store is never observed)
    }
    double result = 0.0;                                            // asm 0x444A29 (fld const_f_zero)
    if (damage == 0 || weapon_num <= 0)                             // asm 0x444A2F-0x444A3D (test esi/jz; test eax/jle)
        return result;                                              // asm -> loc_444AD4
    for (int i = 0; i < weapon_num; ++i)                            // asm 0x444A52-0x444AB5 (var_C count; esi += 4 - the element stride 4)
    {
        const short wtype = damage[i].type;                         // asm 0x444A63 (mov dx,[esi] - the type word @element+0)
        int armor_value = base_armor;                               // asm 0x444A52-0x444A58 ([esp+damage] = var_8)
        if (armor != 0 && armor_num > 0)                            // asm 0x444A56-0x444A60 (test edi/jz; test ebp/jle)
        {
            for (int j = 0; j < armor_num; ++j)                     // asm 0x444A69-0x444A7A (dec ecx/jnz; NO early exit - the LAST match wins)
            {
                if (armor[j].type == wtype)                         // asm 0x444A69 (cmp [eax],dx)
                    armor_value = armor[j].value;                   // asm 0x444A6E-0x444A72 (movsx the value word @element+2)
            }
        }
        if (wtype >= 41)                                            // baked: casts.cpp:253 (cmp dx,41/jl - the special-weapon term-skip, signed)
            continue;                                               // baked: the asm jmp 0x444AA9 - the term is SKIPPED (the effects apply via damage_new)
        double weapon_value = damage[i].value;                      // asm 0x444A7C-0x444A84 (movsx @element+2; fild var_4)
        double term = (weapon_value - (double)armor_value)          // asm 0x444A88-0x444A8C (fild/fsubp - the exact-int 53-bit subtract)
            * terrain_mult                                          // asm 0x444A8E (fmul var_10 - the float promotes, rule (j))
            * attack_modifier;                                      // asm 0x444A92 (fmul [attack_modifier])
        if (term > 0.0)                                             // asm 0x444A96-0x444AA1 (fcom const_d_zero .rdata:006546B0; test ah,41h; jnz -> the skip; the literal > - NaN skips)
            result += term;                                         // asm 0x444AA3 (faddp st(1) - the 53-bit accumulation)
    }
    if (X87_LT_NAN(result, 1.0f))                                   // asm 0x444ABF-0x444AD2 (fcom const_f_one; test ah,1; jz -> keep; the clamp set {<1, NaN})
        result = 1.0f;                                              // asm 0x444ACC-0x444ACE (fstp st; fld const_f_one)
    return result;                                                  // asm 0x444AD4-0x444ADB (the shared epilogue; st0 unrounded)
}

void __stdcall specialDamage(RGE_Static_Object* unit, short type, int damage, int armor);   // casts.cpp:173 - the UNIT_EXTRA effects (the baked special-weapon pass calls it by reference)

// Replacement for RGE_Combat_Object::damage. Original: @ 0x00444820
// (idb_functions.h:1084). Range [0x444820, 0x4449B1) (lst :118974/:119160,
// retn 18h = 6 stack args), void. ABI: __thiscall. Virtual: vftable DATA
// XREF on both Combat and Missile vftables + CODE XREFs
// TRIBE_Building_Object__damage+95 / TRIBE_Combat_Object__damage+1F6
// (the TRIBE tiers forward into this implementation - the entry hook
// covers those calls). first_call is read by NEITHER the asm nor the
// pseudocode (kept for ABI).
// Semantics: apply a hit to THIS unit. The total comes from
// calculateDamage (slot 59 @+0xEC, RGE_Combat_ObjectVtbl :11393 - all
// six args forwarded, asm 0x444831-0x444841; the return rides st0),
// then the SHIELD-FIRST split: sp @+0x40 = 64 absorbs first (sp <= 0
// or total <= 0 or NaN -> hp @+0x3C = 60 takes it all; total <= sp ->
// sp -= total, hp untouched; total > sp -> hp -= total - sp, sp = 0),
// and the shield-hit flash timer shield_hit_timer (@+0xC, :7329 - float,
// the 2026-11-23 IDB rename) = 1.5f (flt_657540, .rdata:00657540;
// the plain-hp branch writes NO marker).
// The under-attack notifies: mType 500 to the unit (caller = the
// attacker id - for missiles fired_from_id @+0x184 = 388,
// RGE_Missile_Object :21058, guarded by the attacker's master_type
// == 60; else attackingObject->id @+0x10; recipient = id; p1 = the
// attacker id; p2 = ftol(hp); p3 = object_group @+0x32 = 50; asm
// 0x4448D4-0x4448EB), mType 530 to the victim's owner when the
// attacker's owner differs (caller = id, recipient/p1 = the attacker
// id, p2 = ftol(hp), p3 = group; asm 0x448FB-0x44946) - or the -1
// form when attackingObject == 0 (asm 0x44921-0x44946) - via the
// owner vtbl slot 75 @+0x12C (notify, RGE_PlayerVtbl :7202), and the
// owner's notifyUnitDeath (slot 76 @+0x130, :7203) when hp < 1.0 (or
// NaN; hp = 0 first; asm 0x44494C-0x44496D). Finally the damage
// sound: RGE_Static_Object__get_3d_sound_info (:3893, 0x00552A50 -
// NOT hooked, the idb constant = the original) and RGE_Sound__play
// (:3662, 0x00544400) on master->damage_sound (@+0x54 = 84).
// BAKED (the casts.cpp special-weapon patch, STRUCTURALLY MOVED here
// per the owner's combined-batch ruling): the special-weapon pass runs
// right after the calculateDamage call - the patch's onGetDamage hook
// fired it mid-loop in calculateDamage; with damage owned, the applier
// applies the effects (the patch's caller-sniffing gate is gone -
// everything damage does IS the real hit). Per element with type >= 41
// (the UNIT_EXTRA effect namespace), the same armor scan feeds
// specialDamage (casts.cpp:173, called by reference). The term-skip
// itself is baked in calculateDamage_new (caller-independent).
void __fastcall RGE_Combat_Object__damage_new(
    RGE_Combat_Object* this_, DWORD dummy, int weapon_num,
    RGE_Armor_Weapon_Info* damage, float attack_modifier,
    RGE_Player* attacking_player, RGE_Static_Object* attackingObject,
    int first_call)
{
    double total = this_->vfptr->calculateDamage(this_, weapon_num, // asm 0x444831-0x444841 (slot 59 @+0xEC; the six args forwarded: weapon_num,damage,attack_modifier,attacking_player,attackingObject)
        damage, attack_modifier, attacking_player, (RGE_Static_Object_idb*)attackingObject);
    {                                                               // baked: casts.cpp:877/878 - the special-weapon effects (the STRUCTURAL gate: the applier applies; the caller-sniffing is gone)
        RGE_Master_Combat_Object* master = this_->master_obj;
        for (int i = 0; i < weapon_num; ++i)
        {
            const short wtype = damage[i].type;
            int armor_value = master->base_armor;                   // @+0x130 = 304
            if (master->armor != 0 && master->armor_num > 0)        // @+0x134 = 308 / @+0x132 = 306
                for (int j = 0; j < master->armor_num; ++j)
                    if (master->armor[j].type == wtype)
                        armor_value = master->armor[j].value;
            if (wtype >= 41)                                        // the UNIT_EXTRA effect namespace (specialDamage's switch)
                specialDamage((RGE_Static_Object*)this_, wtype,     // casts.cpp:173 (called by reference - the bake precedent)
                    damage[i].value, armor_value);
        }
    }
    if (X87_LE_NAN(this_->sp, 0.0f)                                 // asm 0x444847-0x444855 (fld [esi+40h] = sp; fcomp const_f_zero; test ah,41h; jnz -> the hp branch)
        || X87_LE_NAN(total, 0.0f))                                 // asm 0x444857-0x444862 (fcom const_f_zero - the total; the skip set {<=, NaN})
    {
        this_->hp = (float)(this_->hp - total);                     // asm 0x4448A1-0x4448A9 (fld [esi+3Ch] = hp; fsub st,st(1); fstp - the single store-round)
    }
    else if (total > this_->sp)                                     // asm 0x444864-0x44486C (fcom [esi+40h]; test ah,41h; jz -> the pierce; NaN falls to the absorb)
    {
        this_->hp = (float)(this_->hp - (total - this_->sp));       // asm 0x444882-0x444891 (the fsub total-sp / fld hp / fsub st,st(1) / fstp sequence - the OLD sp read)
        this_->sp = 0.0f;                                           // asm 0x444888 (mov dword [esi+40h], 0)
        this_->shield_hit_timer = 1.5f;                             // asm 0x444894-0x44489C (flt_657540 = 1.5f, .rdata:00657540)
    }
    else
    {
        this_->sp = (float)(this_->sp - total);                     // asm 0x44486E-0x444873 (the shield absorbs; hp untouched)
        this_->shield_hit_timer = 1.5f;                             // asm 0x444876-0x44487D (the shield-hit marker)
    }
    int attacker_id = -1;                                           // asm 0x4448AC (or ebx,0FFFFFFFFh)
    if (attackingObject != 0)                                       // asm 0x4448AF-0x4448B2 (test edi/jz)
    {
        if (attackingObject->master_obj->master_type == 60)         // asm 0x4448B4-0x4448BB (master_obj @+0x14; master_type byte @+4, cmp 3Ch - the runtime-type guard)
            attacker_id = ((RGE_Missile_Object*)attackingObject)->fired_from_id;   // asm 0x4448BD (@+0x184 = 388, RGE_Missile_Object :21058)
        else
            attacker_id = attackingObject->id;                      // asm 0x4448C5 (@+0x10)
        this_->vfptr->notify(this_, attacker_id, this_->id, 500,    // asm 0x4448D4-0x4448EB (slot 120 @+0x1E0, :11454; mType 500 = UNDER-ATTACK; the pushes: group,ftol(hp),attacker_id,500,id,attacker_id)
            attackingObject->id, (int)this_->hp,                    // asm 0x4448D5 __ftol(hp) - the p2 int form
            this_->master_obj->object_group);                       // asm 0x4448D0 (movsx word @+0x32 = 50)
        if (attackingObject->owner != this_->owner)                 // asm 0x4448F1-0x4448F9 ([edi+18h] vs [esi+18h] - the pointer compare)
        {
            this_->owner->vfptr->notify(this_->owner, this_->id,    // asm 0x448FB-0x44946 (the owner vtbl slot 75 @+0x12C, :7202; mType 530 = the owner's unit-attacked notify)
                attackingObject->id, 530, attackingObject->id,      // caller = id; recipient/p1 = the attacker id
                (int)this_->hp, this_->master_obj->object_group);   // p2 = ftol(hp); p3 = the group
        }
    }
    else
    {
        this_->owner->vfptr->notify(this_->owner, this_->id, -1,    // asm 0x44921-0x44946 (the -1 form: the attacker unknown)
            530, -1, (int)this_->hp, this_->master_obj->object_group);
    }
    if (X87_LT_NAN(this_->hp, 1.0))                                 // asm 0x44494C-0x44495A (fld [esi+3Ch]; fcom const_d_one; test ah,1; jz -> keep; the set {<1, NaN})
    {
        this_->hp = 0.0f;                                           // asm 0x44962 (mov dword [esi+3Ch], 0)
        this_->owner->vfptr->notifyUnitDeath(this_->owner,          // asm 0x44969-0x4496D (slot 76 @+0x130, :7203)
            this_->id, attacker_id);
    }
    if (this_->master_obj->damage_sound != 0)                       // asm 0x44973-0x4497D (@+0x54 = 84)
    {
        int pan = 0;
        int volume = 0;
        if (RGE_Static_Object__get_3d_sound_info(                   // asm 0x4497F-0x44992 (:3893, 0x00552A50 - NOT hooked; the idb constant = the original)
                (RGE_Static_Object*)this_, &pan, &volume) != 0)
        {
            RGE_Sound__play(this_->master_obj->damage_sound, 1,     // asm 0x4499C-0x449AA (:3662, 0x00544400; playType 1, player_id/culture -1)
                pan, volume, -1, -1);
        }
    }
}

// Replacement for RGE_Combat_Object::explore. Original: @ 0x00445980
// (idb_functions.h:1103). Range [0x445980, 0x445AA7) (lst :120940/:121066,
// retn 0Ch = 3 stack args), returns int. ABI: __thiscall. Virtual:
// vftable DATA XREF on both Combat and Missile vftables. SEH: MSVC
// frame (push -1 / explore_SEH @0x445988); the unwind funclet chunk
// 0x641590-0x64159A = mov eax,[ebp+0Ch] / call operator_delete on the
// RAW allocation (scope stru_67FD70) - the transport/unload twin shape,
// same fold.
// Semantics: the EXPLORE MISSION BUILDER (the action_type 4 =
// RGE_ACTION_TYPE_EXPLORE gate). Three branches: (a) unconditional == 0
// AND the current action already IS the Explore action -> return 1
// (nothing to do); (b) the current action is the Explore action (with
// unconditional, or the non-already fall-through) -> re-TARGET it with
// work(0, (float)tX, (float)tY, world_z @+0x50) - obj_in = 0 (the
// transport twin's form); (c) otherwise build a fresh
// RGE_Action_Explore (operator_new 0x40 = sizeof, walked
// idb_types.h:28656-28676; ctor obj_in = this, task_in = 0, x = tX,
// y = tY, z = world_z), replace the action list (delete_list +
// set_only_action), and return 1 (0 on the allocation/ctor failure).
// The tX/tY INT params convert to float at both call forms (the asm's
// fild/fstp into the reserved slots - the pseudocode's x/y locals).
int __fastcall RGE_Combat_Object__explore_new(
    RGE_Combat_Object* this_, DWORD dummy, int tX, int tY, int unconditional)
{
    if (unconditional == 0                                          // asm 0x44599B-0x44599F (test/jnz skips the already-check)
        && RGE_Action_List__have_action(this_->actions) == 1)       // asm 0x4459A1-0x4459AC (cmp al,1; actions @+0x13C)
    {
        RGE_Action* current = RGE_Action_List__get_action(this_->actions);   // asm 0x4459B0-0x4459B6
        if (current->vfptr->type(current) == RGE_ACTION_TYPE_EXPLORE)        // asm 0x4459BF-0x4459C6 (slot 6 @+0x18; cmp ax,4)
            return 1;                                               // asm loc_445A92 (the already-exploring return)
    }
    if (RGE_Action_List__have_action(this_->actions) == 1)          // asm 0x4459CC-0x4459D9 (the join loc_4459CC, 2 inbound)
    {
        RGE_Action* current = RGE_Action_List__get_action(this_->actions);   // asm 0x4459DB-0x4459E1
        if (current->vfptr->type(current) == RGE_ACTION_TYPE_EXPLORE)        // asm 0x4459EA-0x4459F1 (slot 6; cmp ax,4)
        {
            RGE_Action* existing = RGE_Action_List__get_action(this_->actions);   // asm 0x4459F0-0x4459F9 (the third fetch - faithful)
            existing->vfptr->work(existing, 0, (float)tX, (float)tY,          // asm 0x445A03-0x445A18 (slot 13 @+0x34; the fild/fstp float conversions; obj_in = 0)
                this_->world_z);                                    // asm 0x4459FE (@+0x50)
            return 1;                                               // asm 0x445A1B (jmp loc_445A92)
        }
    }
    RGE_Action_Explore* allocated =
        (RGE_Action_Explore*)operator_new_internal(sizeof(RGE_Action_Explore));   // asm 0x445A1D-0x445A27 (push 40h; sizeof walked 64 = 0x40)
    RGE_Action* fresh = 0;                                          // asm 0x445A33 (xor edi,edi); EH state 0 @0x445A2D
    __try
    {
        if (allocated != 0)                                         // asm 0x445A2B-0x445A35
            fresh = (RGE_Action*)RGE_Action_Explore__RGE_Action_Explore(
                allocated, (RGE_Action_Object*)this_, 0, (float)tX, // asm 0x445A50 (obj_in = this_, task_in = 0; the (float)tX/tY @0x445A3A/0x445A43)
                (float)tY, this_->world_z);                         // asm 0x445A37 (push edx = z)
    }
    __except (operator_delete_internal(allocated), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x641590-0x64159A (operator_delete, raw allocation)
    {
    }
    if (fresh == 0)                                                 // asm 0x445A5B-0x445A65 (EH state -> -1 @0x445A5D)
        return 0;                                                   // asm 0x445A67-0x445A79 (the inline epilogue)
    RGE_Action_List__delete_list(this_->actions);                   // asm 0x445A7C-0x445A82
    this_->vfptr->set_only_action(this_, fresh);                    // asm 0x445A87-0x445A8C (slot 231 @+0x39C)
    return 1;                                                       // asm 0x445A92-0x445AA4
}

// Replacement for RGE_Combat_Object::moveTo2. Original: @ 0x00445530
// (idb_functions.h:1100). Range [0x445530, 0x44565D) (lst :120442/:120578,
// retn 0Ch = 3 stack args), returns int. ABI: __thiscall. Virtual:
// vftable DATA XREF on both Combat and Missile vftables. SEH: MSVC
// frame; the unwind funclet chunk 0x641530-0x64153A = mov eax,[ebp+4] /
// call operator_delete on the RAW allocation (scope stru_67FCF8) - the
// family's fourth twin, same fold.
// Semantics: the TARGET-RESOLVING MOVE BUILDER. oID resolves through
// RGE_Game_World__object (idb_functions.h:6086, 0x00623DB0 - __thiscall
// with the WORLD as this: owner->world @+0x8C, TRIBE_Player :727; the
// (RGE_Game_World*) dual-naming cast) - a null target returns 0. The
// already-check (skipped when unconditional != 0): the current action's
// get_target_obj (slot 20 @+0x50, RGE_ActionVtbl :15982) == the target
// AND its type == 1 (RGE_ACTION_TYPE_MOVE_TO) -> return 1 (already
// moving at it). The sprite: get_run_sprite (master vtbl slot 31
// @+0x7C, RGE_Master_Combat_ObjectVtbl :11247) - and the ORIGINAL
// calls it TWICE when non-null (the f() ? f() : g() source form -
// faithfully replicated); null -> get_move_sprite (slot 30 @+0x78,
// :11246). Then the Move_To builder: operator_new 0x44 = sizeof
// (RGE_Action_Move_To), walked idb_types.h:20755-20776 (the base 64 +
// float range @64); ctor RGE_Action_Move_To2 (idb_functions.h:101,
// 0x004082C0; obj_in = this, target_obj_in = the resolved target,
// range_in = minRange, sprite_in = the resolved sprite), replace the
// action list, return 1 (0 on the allocation/ctor failure).
int __fastcall RGE_Combat_Object__moveTo2_new(
    RGE_Combat_Object* this_, DWORD dummy, int oID, float range_in,
    int unconditional)
{
    RGE_Static_Object* target = RGE_Game_World__object(             // asm 0x44554E-0x44555D (owner @+0x18; world @+0x8C, TRIBE_Player :727; the resolver :6086)
        (RGE_Game_World*)this_->owner->world, oID);
    if (target == 0)                                                // asm 0x44555F-0x445561 (test/jnz)
        return 0;                                                   // asm 0x445563-0x445577 (the inline epilogue)
    if (unconditional == 0                                          // asm 0x44557B-0x44557D (test/jnz skips the already-check)
        && RGE_Action_List__have_action(this_->actions) == 1)       // asm 0x44557F-0x44558C (cmp al,1)
    {
        RGE_Action* current = RGE_Action_List__get_action(this_->actions);   // asm 0x44558E-0x445594
        if (current->vfptr->get_target_obj(current) == (RGE_Static_Object_idb *)target       // asm 0x44559D-0x4455A2 (slot 20 @+0x50, RGE_ActionVtbl :15982)
            && current->vfptr->type(current) == RGE_ACTION_TYPE_MOVE_TO)     // asm 0x4455B3-0x4455BA (slot 6 @+0x18; cmp ax,1)
            return 1;                                               // asm loc_445647 (the already-moving return)
    }
    RGE_Sprite* move_sprite = this_->master_obj->vfptr->get_run_sprite(   // asm 0x4455C0-0x4455C6 (slot 31 @+0x7C, :11247 - the FIRST call)
        this_->master_obj, (RGE_Static_Object_idb*)this_);
    if (move_sprite != 0)                                           // asm 0x4455CC-0x4455CE (test/jz)
        move_sprite = this_->master_obj->vfptr->get_run_sprite(     // asm 0x4455D0-0x4455D3 (the SECOND get_run_sprite - the source's f() ? f() : g() form, faithful)
            this_->master_obj, (RGE_Static_Object_idb*)this_);
    else
        move_sprite = this_->master_obj->vfptr->get_move_sprite(    // asm 0x4455D8-0x4455DB (slot 30 @+0x78, :11246)
            this_->master_obj, (RGE_Static_Object_idb*)this_);
    RGE_Action_Move_To* allocated =
        (RGE_Action_Move_To*)operator_new_internal(sizeof(RGE_Action_Move_To));   // asm 0x4455DE-0x4455E2 (push 44h; sizeof walked 68 = 0x44 - the base 64 + float range @64)
    RGE_Action* fresh = 0;                                          // asm 0x44560D (xor edi,edi on the null path); EH state 0 @0x4455F0
    __try
    {
        if (allocated != 0)                                         // asm 0x4455EE-0x4455F8
            fresh = (RGE_Action*)RGE_Action_Move_To__RGE_Action_Move_To2(
                allocated, (RGE_Action_Object*)this_, target,       // asm 0x4455FE-0x445604 (obj_in = this_, target_obj_in = the resolved target)
                range_in, move_sprite);                             // asm 0x4455FF-0x445600 (range_in, sprite_in)
    }
    __except (operator_delete_internal(allocated), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x641530-0x64153A (operator_delete, raw allocation)
    {
    }
    if (fresh == 0)                                                 // asm 0x44560F-0x445619 (EH state -> -1 @0x445611)
        return 0;                                                   // asm 0x44561B-0x44562E (the inline epilogue)
    RGE_Action_List__delete_list(this_->actions);                   // asm 0x445631-0x445637
    this_->vfptr->set_only_action(this_, fresh);                    // asm 0x44563C-0x445641 (slot 231 @+0x39C)
    return 1;                                                       // asm 0x445647-0x44565A
}

// Replacement for RGE_Combat_Object::attack. Original: @ 0x00445380
// (idb_functions.h:1098). Range [0x445380, 0x445501) (lst :120262/:120410,
// retn 10h = 4 stack args), returns int. ABI: __thiscall. Virtual:
// vftable DATA XREF on both Combat and Missile vftables. SEH: MSVC
// frame; the unwind funclet chunk 0x641510-0x64151A = the operator_delete
// raw-allocation fold (the family's sixth twin).
// Semantics: the COORDINATE-FORM ATTACK BUILDER. The already-check
// (skipped when unconditional != 0): the current action's target
// coordinate vs the requested (x, y, z) and its type being Attack2 (9)
// or Attack (7) -> return 1 (already attacking). PRESERVED ORIGINAL
// QUIRK: the coordinate compares call RGE_Action__targetX (:186,
// 0x0040B040) THREE TIMES - the same getter vs x, then y, then z
// (targetY/targetZ exist at :187/:188 but are never called here) - so
// the skip can only pass when the requested x == y == z; the compares'
// fall-through set is {==, NaN} (fcomp + test ah,40h + jz -> the skip)
// - written X87_EQ_NAN (the getter hoisted to a local per the macro's
// call-arg rule). Otherwise the builder: operator_new 0x68 = sizeof
// (RGE_Action_Attack), walked idb_types.h:20891-20924; ctor
// RGE_Action_Attack2 (idb_functions.h:8, 0x004012E0; obj_in = this,
// x, y, z, the three sprites = 0, range = master->weapon_range
// @+0x168 = 360, min_range = minimum_weapon_range @+0x144 = 324,
// missile_id @+0x154 = 340, fire_missile_at_frame @+0x15A = 346);
// null -> return 0. Then: delete_list; type(fresh) (slot 6 @+0x18 -
// with the setUnitAIAction args STAGED on the stack around it, asm
// 0x4454BF-0x4454CC); setUnitAIAction (slot 234 @+0x3A8, :11568 -
// this->vfptr->setUnitAIAction(this, the issued type word, 0, x, y, z)
// - asm 0x4444CF-0x4444DF: the edx = the spilled unit vtbl (0x4444BD/
// 0x4444C8, the dead x-slot reuse), the [esp+1Ch+arg_10] read = the
// reload of that spill (the IDA arg_10 label = the decompositional
// artifact across the SEH chunks; the dispatch = the unit's OWN
// setUnitAIAction - the UnitAI learns the new attack action); then
// set_only_action (slot 231 @+0x39C). Return 1.
unsigned __int8 __fastcall RGE_Combat_Object__attack_new(
    RGE_Combat_Object* this_, DWORD dummy, float x, float y, float z,
    int unconditional)
{
    RGE_Action* fresh = 0;                                          // asm 0x44539D (xor edi,edi)
    if (unconditional == 0                                          // asm 0x4453A1-0x4453A3 (cmp edi/jnz skips the already-check)
        && RGE_Action_List__have_action(this_->actions) == 1)       // asm 0x4453A9-0x4453B6 (cmp al,1)
    {
        RGE_Action* current = RGE_Action_List__get_action(this_->actions);   // asm 0x4453BC-0x4453C2
        double tx = RGE_Action__targetX(current);                   // asm 0x4453C9 (:186, 0x0040B040 - the double/st0 return)
        if (X87_EQ_NAN(tx, x)                                       // asm 0x4453CE-0x4453D7 (fcomp; test ah,40h; jz -> the skip; the {==, NaN} fall-through)
            && X87_EQ_NAN(tx, y)                                    // asm 0x4453EB-0x4453F4 (the SAME getter vs y - the preserved quirk)
            && X87_EQ_NAN(tx, z)                                    // asm 0x445408-0x445411 (vs z)
            && (current->vfptr->type(current) == RGE_ACTION_TYPE_ATTACK2     // asm 0x445422-0x445429 (slot 6 @+0x18; cmp ax,9)
                || current->vfptr->type(current) == RGE_ACTION_TYPE_ATTACK)) // asm 0x44543E-0x445445 (cmp ax,7)
            return 1;                                               // asm loc_4454EA (the already-attacking return)
    }
    RGE_Action_Attack* allocated =
        (RGE_Action_Attack*)operator_new_internal(sizeof(RGE_Action_Attack));   // asm 0x44544B-0x4444D (push 68h; sizeof walked 104 = 0x68); EH state 0 @0x445463
    __try
    {
        if (allocated != 0)                                         // asm 0x445461-0x445467 (cmp eax,edi/jz)
            fresh = (RGE_Action*)RGE_Action_Attack__RGE_Action_Attack2(          // asm 0x445497 (:8, 0x004012E0)
                allocated, (RGE_Action_Object*)this_, x, y, z,      // asm 0x44548D-0x445494 (obj_in = this_; x, y, z)
                0, 0, 0,                                            // asm 0x44548E-0x445490 (move/fight/wait sprites = 0, edi)
                this_->master_obj->weapon_range,                    // asm 0x44547C-0x445488 (@+0x168 = 360; min_range @+0x144 = 324 pushed first)
                this_->master_obj->minimum_weapon_range,
                this_->master_obj->missile_id,                      // asm 0x445474 (@+0x154 = 340, the word)
                this_->master_obj->fire_missile_at_frame);          // asm 0x44546C (@+0x15A = 346, the word)
    }
    __except (operator_delete_internal(allocated), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x641510-0x64151A (operator_delete, raw allocation)
    {
    }
    if (fresh == 0)                                                 // asm 0x44549E-0x4454AC
        return 0;                                                   // asm 0x4454AA-0x4454AC
    RGE_Action_List__delete_list(this_->actions);                   // asm 0x4454AE-0x4454B4
    int action_type = fresh->vfptr->type(fresh);                    // asm 0x4454C2-0x4454CC (slot 6 @+0x18; the setUnitAIAction args STAGED on the stack around this call: 0x4454BF-0x4454C4)
    this_->vfptr->setUnitAIAction(this_, action_type, 0, x, y, z);  // asm 0x4444CF-0x4444DF (slot 234 @+0x3A8, :11568; edx = the spilled unit vtbl 0x4444BD/0x4444C8; [esp+1Ch+arg_10] = that spill reloaded - the IDA arg_10 label is the decompositional artifact)
    this_->vfptr->set_only_action(this_, fresh);                    // asm 0x4454DF-0x4454E4 (slot 231 @+0x39C)
    return 1;                                                       // asm 0x4454EA-0x4454FE
}

// Replacement for RGE_Combat_Object::moveTo. Original: @ 0x00445660
// (idb_functions.h:1101). Range [0x445660, 0x4457E5) (lst :120585/:120756,
// retn 18h = 6 stack args), returns int. ABI: __thiscall. Virtual:
// vftable DATA XREF on both Combat and Missile vftables + CODE XREF
// TRIBE_Combat_Object__moveTo+DF (the TRIBE tier forwards into this
// implementation - the entry hook covers that call). SEH: MSVC frame;
// the unwind funclet chunk 0x641550-0x64155A = mov eax,[ebp+0Ch] /
// call operator_delete on the RAW allocation (scope stru_67FD20) - the
// family twin, same fold.
// Semantics: the COORDINATE-FORM MOVE BUILDER. The already-check
// (skipped when unconditional != 0): the current action's target
// coordinate vs the requested (x, y, z) via the CORRECT three getters
// (targetX/targetY/targetZ :186-188 - the attack twin's triple-targetX
// quirk does NOT appear here) and its type == 1
// (RGE_ACTION_TYPE_MOVE_TO) -> return 1 (already moving). The
// re-target: move_to (slot 12 @+0x30, RGE_ActionVtbl :15974) with
// obj_in = THIS (the explore/transport twins pass 0) -> return 1 when
// move_to answers 1. Otherwise the builder: the walk flag picks the
// sprite (walk != 0 -> get_move_sprite slot 30 @+0x78 directly; walk
// == 0 -> get_run_sprite slot 31 @+0x7C with the f() ? f() :
// get_move_sprite double-call form), operator_new 0x44 = sizeof
// (RGE_Action_Move_To), walked idb_types.h:20755-20776; ctor
// RGE_Action_Move_To (idb_functions.h:100, 0x00408220; obj_in = this,
// x, y, z, range_in = minRange, sprite_in), replace the action list,
// return 1 (0 on the allocation/ctor failure).
int __fastcall RGE_Combat_Object__moveTo_new(
    RGE_Combat_Object* this_, DWORD dummy, float x, float y, float z,
    float minRange, int unconditional, int walk)
{
    if (unconditional == 0                                          // asm 0x44567D-0x445681 (test/jnz skips the already-check)
        && RGE_Action_List__have_action(this_->actions) == 1)       // asm 0x445687-0x445694 (cmp al,1)
    {
        RGE_Action* current = RGE_Action_List__get_action(this_->actions);   // asm 0x445696-0x44569C
        double tx = RGE_Action__targetX(current);                   // asm 0x4456A3 (:186)
        double ty = RGE_Action__targetY(current);                   // asm 0x4456C0 (:187)
        double tz = RGE_Action__targetZ(current);                   // asm 0x4456DD (:188)
        if (X87_EQ_NAN(tx, x) && X87_EQ_NAN(ty, y)                  // asm 0x4456A8-0x4456EB (fcomp + test ah,40h; jz -> the skip; the {==, NaN} fall-through x3)
            && X87_EQ_NAN(tz, z)
            && current->vfptr->type(current) == RGE_ACTION_TYPE_MOVE_TO)     // asm 0x4456FC-0x445703 (slot 6 @+0x18; cmp ax,1)
            return 1;                                               // asm loc_4457CE (the already-moving return)
    }
    if (RGE_Action_List__have_action(this_->actions) == 1)          // asm 0x445709-0x44571E
    {
        RGE_Action* current = RGE_Action_List__get_action(this_->actions);   // asm 0x445720-0x445726
        if (current->vfptr->move_to(current,                        // asm 0x445737-0x44573D (slot 12 @+0x30, RGE_ActionVtbl :15974)
                (RGE_Static_Object_idb*)this_, x, y, z) == 1)           // asm 0x44572B-0x445733 (the pushes: z,y,x,this - obj_in = THIS)
            return 1;                                               // asm 0x44573A-0x44573D (cmp eax,1 -> loc_4457CE)
    }
    RGE_Sprite* move_sprite;                                        // asm 0x445743-0x44576E (the walk-flag sprite resolution)
    if (walk != 0)                                                  // asm 0x44574A-0x44574C (test/jz)
        move_sprite = this_->master_obj->vfptr->get_move_sprite(    // asm 0x44574E-0x445751 (slot 30 @+0x78, :11246)
            this_->master_obj, (RGE_Static_Object_idb*)this_);
    else
    {
        move_sprite = this_->master_obj->vfptr->get_run_sprite(     // asm 0x445759-0x445766 (slot 31 @+0x7C, :11247 - the FIRST call)
            this_->master_obj, (RGE_Static_Object_idb*)this_);
        if (move_sprite != 0)                                       // asm 0x44575F-0x445761
            move_sprite = this_->master_obj->vfptr->get_run_sprite( // asm 0x445765-0x445766 (the SECOND call - the f() ? f() : g() form, faithful)
                this_->master_obj, (RGE_Static_Object_idb*)this_);
        else
            move_sprite = this_->master_obj->vfptr->get_move_sprite(// asm 0x44576D (slot 30 @+0x78)
                this_->master_obj, (RGE_Static_Object_idb*)this_);
    }
    RGE_Action_Move_To* allocated =
        (RGE_Action_Move_To*)operator_new_internal(sizeof(RGE_Action_Move_To));   // asm 0x445771-0x44577D (push 44h; sizeof walked 68 = 0x44)
    RGE_Action* fresh = 0;                                          // EH state 0 @0x445783
    __try
    {
        if (allocated != 0)                                         // asm 0x445781-0x44578B
            fresh = (RGE_Action*)RGE_Action_Move_To__RGE_Action_Move_To(
                allocated, (RGE_Action_Object*)this_, x, y, z,      // asm 0x445799-0x44579D (obj_in = this_; x, y, z)
                minRange, move_sprite);                             // asm 0x44578D-0x445796 (range_in, sprite_in)
    }
    __except (operator_delete_internal(allocated), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x641550-0x64155A (operator_delete, raw allocation)
    {
    }
    if (fresh == 0)                                                 // asm 0x4457A8-0x4457B2
        return 0;                                                   // asm 0x4457B4-0x4457B6 (jmp loc_4457D3)
    RGE_Action_List__delete_list(this_->actions);                   // asm 0x4457B8-0x4457BE
    this_->vfptr->set_only_action(this_, fresh);                    // asm 0x4457C3-0x4457C8 (slot 231 @+0x39C)
    return 1;                                                       // asm 0x4457CE-0x4457D3
}

// Replacement for RGE_Combat_Object::gather. Original: @ 0x004457F0
// (idb_functions.h:1102). Range [0x4457F0, 0x445976) (lst :120765/:120931,
// retn 8 = 2 stack args), returns int. ABI: __thiscall. Virtual:
// vftable DATA XREF on both Combat and Missile vftables. SEH: MSVC
// frame; the unwind funclet chunk 0x641570 = the operator_delete
// raw-allocation fold (the family's seventh twin).
// Semantics: the TASK-BASED GATHER BUILDER (the action_type 5 =
// RGE_ACTION_TYPE_GATHER gate) - the family's variant that goes through
// the TASK layer. oID resolves through RGE_Game_World__object
// (:6086, 0x00623DB0; the (RGE_Game_World*) dual-naming cast) - a null
// target returns 0 (the eax = the resolver's null, the implicit return).
// The already-check (skipped when unconditional != 0): the current
// action's get_target_obj (slot 20 @+0x50) == the target AND its type
// == 5 -> return 1 (already gathering). The re-target: work (slot 13
// @+0x34) with obj_in = THE TARGET and the unit's OWN position
// (world_x/world_y/world_z @+0x48/4C/50) as the coordinates (the
// unload twin's arg form). Otherwise the TASK-DRIVEN builder:
// getTask (slot 233 @+0x3A4, RGE_Combat_ObjectVtbl :11567) builds the
// RGE_Task for (target, world_x, world_y, world_z, -1) - the final
// __int16 param = -1 (UNKNOWN - a registry row owed); the created task
// feeds RGE_Action_Gather__RGE_Action_Gather (:41, 0x00404160;
// obj_in = this, task_in = the task, target_obj_in = the target;
// operator_new 0x60 = sizeof(RGE_Action_Gather), walked
// idb_types.h:28713-28744), replace the action list, return 1 (0 on
// the allocation/ctor failure).
int __fastcall RGE_Combat_Object__gather_new(
    RGE_Combat_Object* this_, DWORD dummy, int oID, int unconditional)
{
    RGE_Static_Object* target = RGE_Game_World__object(             // asm 0x44580E-0x445818 (owner @+0x18; world @+0x8C, TRIBE_Player :727; the resolver :6086)
        (RGE_Game_World*)this_->owner->world, oID);
    if (target == 0)                                                // asm 0x44581F-0x445821 (test/jnz)
        return 0;                                                   // asm 0x445823-0x445834 (the eax = the resolver's null - the implicit return)
    if (unconditional == 0                                          // asm 0x44583B-0x44583D
        && RGE_Action_List__have_action(this_->actions) == 1)       // asm 0x44583F-0x44584C
    {
        RGE_Action* current = RGE_Action_List__get_action(this_->actions);   // asm 0x44584E-0x445854
        if (current->vfptr->get_target_obj(current) == (RGE_Static_Object_idb *)target       // asm 0x44585D-0x445862 (slot 20 @+0x50)
            && current->vfptr->type(current) == RGE_ACTION_TYPE_GATHER)      // asm 0x445873-0x44587A (slot 6 @+0x18; cmp ax,5)
            return 1;                                               // asm loc_445960 (the already-gathering return)
    }
    if (RGE_Action_List__have_action(this_->actions) == 1)          // asm 0x445880-0x44588D
    {
        RGE_Action* current = RGE_Action_List__get_action(this_->actions);   // asm 0x44588F-0x445895
        if (current->vfptr->type(current) == RGE_ACTION_TYPE_GATHER)         // asm 0x44589E-0x4458A5 (slot 6; cmp ax,5)
        {
            current->vfptr->work(current, (RGE_Static_Object_idb*)target,                   // asm 0x4458B7-0x4458C3 (slot 13 @+0x34; the pushes: world_z,world_y,world_x,target - obj_in = THE TARGET)
                this_->world_x, this_->world_y, this_->world_z);    // asm 0x4458B2-0x4458BC (@+0x48/4C/50)
            return 1;                                               // asm 0x4458C6 (jmp loc_445960)
        }
    }
    RGE_Task* task = this_->vfptr->getTask(this_, (RGE_Static_Object_idb*)target,           // asm 0x4458CB-0x4458DE (slot 233 @+0x3A4, RGE_Combat_ObjectVtbl :11567; the pushes: -1,z,y,x,target)
        this_->world_x, this_->world_y, this_->world_z, -1);        // the final __int16 = -1: UNKNOWN - a registry row owed
    if (task == 0)                                                  // asm 0x4458E6-0x4458E8 (test ebx/jnz)
        return 0;                                                   // asm 0x4458EA-0x4458FB (xor eax,eax; the inline epilogue)
    RGE_Action_Gather* allocated =
        (RGE_Action_Gather*)operator_new_internal(sizeof(RGE_Action_Gather));   // asm 0x4458FE-0x445905 (push 60h; sizeof walked 96 = 0x60); EH state 0 @0x44590E
    RGE_Action* fresh = 0;                                          // asm 0x445926 (xor esi,esi)
    __try
    {
        if (allocated != 0)                                         // asm 0x44590C-0x445916
            fresh = (RGE_Action*)RGE_Action_Gather__RGE_Action_Gather(
                allocated, (RGE_Action_Object*)this_, task, target);// asm 0x44591D (obj_in = this_, task_in = the task, target_obj_in = the target)
    }
    __except (operator_delete_internal(allocated), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x641570 (operator_delete, raw allocation)
    {
    }
    if (fresh == 0)                                                 // asm 0x445928-0x445932 (EH state -> -1 @0x44592A)
        return 0;                                                   // asm 0x445934-0x445947 (xor eax,eax; the inline epilogue)
    RGE_Action_List__delete_list(this_->actions);                   // asm 0x44594A-0x445950
    this_->vfptr->set_only_action(this_, fresh);                    // asm 0x445955-0x44595A (slot 231 @+0x39C)
    return 1;                                                       // asm 0x445960-0x445973
}

// Replacement for RGE_Combat_Object::attack2. Original: @ 0x00445190
// (idb_functions.h:1097). Range [0x445190, 0x445377) (lst :120062/:120253,
// retn 8 = 2 stack args), returns int. ABI: __thiscall. Virtual:
// vftable DATA XREF on both Combat and Missile vftables + CODE XREF
// TRIBE_Combat_Object__attack+1A7 (the TRIBE tier forwards into this
// implementation - the entry hook covers that call). SEH: MSVC frame;
// the unwind funclet chunk 0x6414F0-0x6414FA = mov eax,[ebp+4] / call
// operator_delete on the RAW allocation (scope stru_67FCA8) - the
// family's seventh twin, same fold.
// Semantics: the ID-FORM ATTACK BUILDER. oID resolves through
// RGE_Game_World__object (:6086, 0x00623DB0; the (RGE_Game_World*)
// dual-naming cast) - a null target returns 0 (the resolver's null,
// the implicit return). The already-check (skipped when unconditional
// != 0): the current action's get_target_obj (slot 20 @+0x50) == the
// target AND its type being Attack2 (9) or Attack (7) -> return 1
// (already attacking it). The re-target: when the current action
// already targets it AND is an Attack2 (9): work (slot 13 @+0x34) with
// obj_in = THE TARGET and the TARGET's world_x/world_y/world_z
// (@+0x48/4C/50) as the coordinates -> return 1. THE WORKER TRANSFORM
// (the special block, asm 0x44529B-0x4452D5): when the master's
// object_group (@+0x1E = 30) == 58 (WORKER - the registry), the unit
// TRANSFORMS into the armed variant before the action is built:
// task_by_group (@+0x11C = 284) == 1 -> master_objects[83], else
// master_objects[293] (the transform-target master ids - registry
// candidates), and the transform goes through the unit vtbl slot 51
// @+0xCC (transform, RGE_Combat_ObjectVtbl :11385 - OWNED, routes to
// transform_new). Then the builder: operator_new 0x68 = sizeof
// (RGE_Action_Attack), walked idb_types.h:20891-20924; ctor
// RGE_Action_Attack (idb_functions.h:7, 0x00401150; obj_in = this,
// target_obj_in = the target, the three sprites = 0, the four master
// weapon fields), replace the action list, return 1 (0 on the
// allocation/ctor failure).
int __fastcall RGE_Combat_Object__attack2_new(
    RGE_Combat_Object* this_, DWORD dummy, int oID, int unconditional)
{
    RGE_Static_Object* target = RGE_Game_World__object(             // asm 0x4451AD-0x4451BC (owner @+0x18; world @+0x8C, TRIBE_Player :727; the resolver :6086)
        (RGE_Game_World*)this_->owner->world, oID);
    if (target == 0)                                                // asm 0x4451BE-0x4451C0 (test/jnz)
        return 0;                                                   // asm 0x4451C2-0x4451D2 (the eax = the resolver's null - the implicit return)
    if (unconditional == 0                                          // asm 0x4451D9-0x4451DB
        && RGE_Action_List__have_action(this_->actions) == 1)       // asm 0x4451DD-0x4451EA
    {
        RGE_Action* current = RGE_Action_List__get_action(this_->actions);   // asm 0x4451EC-0x4451F2
        if (current->vfptr->get_target_obj(current) == (RGE_Static_Object_idb *)target)      // asm 0x4451FB-0x445200 (slot 20 @+0x50)
        {
            RGE_Action* t9 = RGE_Action_List__get_action(this_->actions);    // asm 0x445202-0x445208 (the second fetch - faithful)
            if (t9->vfptr->type(t9) == RGE_ACTION_TYPE_ATTACK2)     // asm 0x445211-0x445218 (cmp ax,9)
                return 1;                                           // asm loc_445362
            RGE_Action* t7 = RGE_Action_List__get_action(this_->actions);    // asm 0x44521E-0x445224 (the third fetch - faithful)
            if (t7->vfptr->type(t7) == RGE_ACTION_TYPE_ATTACK)      // asm 0x44522D-0x445234 (cmp ax,7)
                return 1;                                           // asm loc_445362
        }
    }
    if (RGE_Action_List__have_action(this_->actions) == 1)          // asm 0x44523A-0x445247
    {
        RGE_Action* current = RGE_Action_List__get_action(this_->actions);   // asm 0x44524F-0x445254
        if (current->vfptr->get_target_obj(current) == (RGE_Static_Object_idb *)target)      // asm 0x44525B-0x44525D
        {
            RGE_Action* t9 = RGE_Action_List__get_action(this_->actions);    // asm 0x445265-0x44526A
            if (t9->vfptr->type(t9) == RGE_ACTION_TYPE_ATTACK2)     // asm 0x44526E-0x445275 (cmp ax,9)
            {
                RGE_Action* t11 = RGE_Action_List__get_action(this_->actions);   // asm 0x445277-0x44527D (the third fetch - faithful)
                t11->vfptr->work(t11, (RGE_Static_Object_idb*)target, target->world_x,      // asm 0x445287-0x445293 (slot 13 @+0x34; obj_in = THE TARGET; the coords = the target's @+0x48/4C/50)
                    target->world_y, target->world_z);
                return 1;                                           // asm 0x445296 (jmp loc_445362)
            }
        }
    }
    if (this_->master_obj->object_group == 58)                      // asm 0x44529E-0x4452A3 (word @+0x1E = 30; 58 = WORKER - the registry)
    {
        RGE_Master_Static_Object* transform_target = 0;             // asm 0x4452D0's eax (the master pick)
        if (this_->master_obj->task_by_group == 1)                  // asm 0x4452A5-0x4452AC (byte @+0x11C = 284, cmp 1)
            transform_target = this_->owner->master_objects[83];    // asm 0x4452B1-0x4452B4 ([eax+14Ch] = 83*4 - the transform-target master id: registry candidate)
        else
            transform_target = this_->owner->master_objects[293];   // asm 0x4452C3-0x4452C6 ([ecx+494h] = 293*4 - registry candidate)
        if (transform_target != 0)                                  // asm 0x4452BA-0x4452CE (the two test/jz joins)
            this_->vfptr->transform(this_, transform_target);       // asm 0x4452D0-0x4452D5 (slot 51 @+0xCC, :11385 - OWNED, routes to transform_new)
    }
    RGE_Action_Attack* allocated =
        (RGE_Action_Attack*)operator_new_internal(sizeof(RGE_Action_Attack));   // asm 0x4452DB-0x4452E2 (push 68h; sizeof walked 104 = 0x68); EH state 0 @0x4452EB
    RGE_Action* fresh = 0;                                          // asm 0x445329 (xor edi,edi)
    __try
    {
        if (allocated != 0)                                         // asm 0x4452E9-0x4452F3
            fresh = (RGE_Action*)RGE_Action_Attack__RGE_Action_Attack(       // asm 0x445320 (:7, 0x00401150)
                allocated, (RGE_Action_Object*)this_, target,       // asm 0x44531C-0x44531D (obj_in = this_, target_obj_in = the target)
                0, 0, 0,                                            // asm 0x445316-0x445318 (move/fight/wait sprites = 0)
                this_->master_obj->weapon_range,                    // asm 0x445308 (@+0x168 = 360; min_range @+0x144 = 324)
                this_->master_obj->minimum_weapon_range,
                this_->master_obj->missile_id,                      // asm 0x445300 (@+0x154 = 340)
                this_->master_obj->fire_missile_at_frame);          // asm 0x4452F8 (@+0x15A = 346)
    }
    __except (operator_delete_internal(allocated), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x6414F0-0x6414FA (operator_delete, raw allocation)
    {
    }
    if (fresh == 0)                                                 // asm 0x44532B-0x44532D
        return 0;                                                   // asm 0x445337-0x445349 (the inline epilogue)
    RGE_Action_List__delete_list(this_->actions);                   // asm 0x44534C-0x445352
    this_->vfptr->set_only_action(this_, fresh);                    // asm 0x445357-0x44535C (slot 231 @+0x39C)
    return 1;                                                       // asm 0x445362-0x445374
}

// Replacement for RGE_Combat_Object::attack4. Original: @ 0x00444170
// (idb_functions.h:1081). Range [0x444170, 0x4443AF) (the regenerated
// listing :118273/:118533), retn 8 = 2 stack args), returns unsigned
// __int8. ABI: __thiscall. Virtual: vftable DATA XREF on both Combat
// and Missile vftables. NO SEH frame (the plain sub esp,0Ch + the 4
// register pushes - no __unwind markers, no EH state dance).
// Semantics: THE MISSILE FIRER - the function that actually launches
// the projectile for a hit. target == 0 -> return 0. attack_count++
// (@+0x170 = 368). Then the split on master->missile_id (@+0x154 =
// 340): missile_id < 0 (no missile) -> the DIRECT do_attack dispatch
// (slot 240 @+0x3C0, RGE_Combat_ObjectVtbl :11574 - OWNED, routes) with
// (target, launcher, world_x, world_y, world_z, 1.0f) and return 1.
// missile_id >= 0 -> the MISSILE path: the muzzle offset from the
// unit's angle (@+0xC8 = 200, the Moving extension): muzzle_x =
// -sin(angle)*weapon_offset_x + cos(angle)*weapon_offset_y + world_x;
// muzzle_y = sin(angle)*weapon_offset_y + cos(angle)*weapon_offset_x
// + world_y; muzzle_z = weapon_offset_z (@+0x164 = 356) + world_z
// (the weapon_offset_x/y/z @+0x15C/160/164 = 348/352/356); in_range =
// sqrt(radius_x^2 + radius_y^2) (@+0x3C/40 = 60/64) + weapon_range
// + 2.0 (flt_6552F4). master_objects[missile_id] null -> return 0.
// The projectile-master selection (the three-way): when the LAUNCHER
// is a GE Assault Mech and the target is an Aircraft ->
// master_objects[992]; when the LAUNCHER is an Air Cruiser and the
// target is an Aircraft -> master_objects[836]; else
// master_objects[missile_id] (the AA counter-battery master ids 992 /
// 836 - registry candidates). make_new_obj (the master vtbl slot 12
// @+0x30, :11228) creates the missile: (master, (RGE_Player*)owner,
// muzzle_x, muzzle_y, muzzle_z) - null -> return 0. init_missile
// (:2069, 0x0049BDE0): when (the launcher is a GE Assault Mech OR an
// Air Cruiser) AND the target is an Aircraft -> the launcher arg = 0
// (the AA homing form: no launcher-owner); else the launcher arg =
// the launcher. Then the notify pair: notify (slot 120 @+0x1E0) with
// (caller = id, recipient = id, mType 512, p1 = 600, p2 = target->id,
// p3 = 0) - the do_attack twin's 512/600 form; then the weapon_range
// <= 1.0 (or NaN) gate (fcom const_f_one; test ah,41h; jnz -> the
// return - written X87_LE_NAN); then the owner's computerPlayer
// (RGE_PlayerVtbl slot 7 @+0x1C, :7134) == 1 (the AI owner) gates the
// TARGET's mType 527 notify: notify (the TARGET's slot 120) with
// (caller = target->id, recipient = this->id, mType 527, p1 = this->
// id, p2 = ftol(target->hp @+0x3C = 60), p3 = the target's master's
// object_group @+0x32 = 50). Return 1.
// PRECISION NOTE (flagged): the original's fsin/fcos compute at the
// x87 80-bit microcode precision and the products ride the stack at
// 53-bit+; our CRT sin/cos return 53-bit doubles - a <= 1 ulp
// divergence in the muzzle position is possible (the multiplayer is
// safe - all clients run our code; the vanilla-comparison traces may
// show it).
unsigned __int8 __fastcall RGE_Combat_Object__attack4_new(
    RGE_Combat_Object* this_, DWORD dummy, RGE_Static_Object* target,
    RGE_Combat_Object* launcher)
{
    if (target == 0)                                                // asm 0x44417D-0x44417F (test/jnz)
        return 0;                                                   // asm 0x444181-0x44418A (xor al,al)
    ++this_->attack_count;                                          // asm 0x44418D-0x44419D (@+0x170 = 368)
    int missile_id = this_->master_obj->missile_id;                 // asm 0x44419D-0x4441A4 (word @+0x154 = 340)
    if (missile_id < 0)                                             // asm 0x4441A4-0x4441A7 (test cx/jl)
    {
        this_->vfptr->do_attack(this_, (RGE_Static_Object_idb*)target, (RGE_Combat_Object *)launcher,            // asm 0x444385-0x4443A0 (slot 240 @+0x3C0, :11574 - OWNED, routes; the pushes: 1.0,z,y,x,launcher,target)
            this_->world_x, this_->world_y, this_->world_z, 1.0f);  // asm 0x444385-0x44439D (the coords @+0x48/4C/50; the 1.0f damage_mod)
        return 1;                                                   // asm loc_4443A6 (al = 1)
    }
    RGE_Master_Combat_Object* master = this_->master_obj;
    float muzzle_x = (float)(-sin(this_->angle)                     // asm 0x4441AD-0x4441D4 (fcos/fsin/fchs/fmul/fadd; the .c's v14 form)
        * master->weapon_offset_x
        + cos(this_->angle) * master->weapon_offset_y
        + this_->world_x);
    float muzzle_y = (float)(sin(this_->angle)                      // asm 0x4441D8-0x4441EB (fmul/fxch/fmul/faddp/fadd; the .c's v13 form)
        * master->weapon_offset_y
        + cos(this_->angle) * master->weapon_offset_x
        + this_->world_y);
    float muzzle_z = (float)(master->weapon_offset_z                // asm 0x4441EF-0x4441F8 (@+0x164 = 356 + world_z)
        + this_->world_z);
    float in_range = (float)(sqrt(                                  // asm 0x4441FC-0x444412 (fld/fmul/fld/fmul/faddp/fsqrt)
        master->radius_x * master->radius_x
        + master->radius_y * master->radius_y)
        + master->weapon_range + 2.0f);                             // asm 0x444412-0x44441B (fadd weapon_range @+0x144 = 324; fadd flt_6552F4 = 2.0f)
    int missile_index = missile_id;                                 // asm 0x444418 (movsx)
    if (this_->owner->master_objects[missile_index] == 0)           // asm 0x444421-0x44442F (master_objects @+0x74; cmp 0/jnz)
        return 0;                                                   // asm 0x444431-0x44443A (xor al,al)
    RGE_Master_Static_Object* projectile_master = 0;                // the three-way selection
    bool target_is_aircraft = target->gbg_isAircraft();// asm 0x4452DD-form (the target's slot 5 @+0x14)
    if (launcher->vfptr->gbg_isGEAssaultMech(launcher)              // asm 0x4452C5-0x4452CF (slot 4 @+0x10)
        && target_is_aircraft)
        projectile_master = this_->owner->master_objects[992];      // asm 0x44425E-0x444262 ([ecx+0F80h] = 992*4 - registry candidate)
    else if (launcher->vfptr->gbg_isAirCruiser(launcher)            // asm 0x44527D-0x445283 (slot 6 @+0x18)
        && target_is_aircraft)
        projectile_master = this_->owner->master_objects[836];      // asm 0x444283-0x445289 ([ecx+0D10h] = 836*4 - registry candidate)
    else
        projectile_master = this_->owner->master_objects[missile_index];   // asm 0x445291-0x44529B ([ecx+edx*4])
    RGE_Missile_Object* missile = (RGE_Missile_Object*)             // asm 0x44429E-0x4442B3 (the master vtbl slot 12 @+0x30, :11228)
        projectile_master->vfptr->make_new_obj(projectile_master,
            (RGE_Player*)this_->owner, muzzle_x, muzzle_y, muzzle_z);   // asm 0x4442A4-0x4442AF (the owner cast: the RGE_Player* dual-naming)
    if (missile == 0)                                               // asm 0x4442B5-0x4442C2
        return 0;                                                   // asm 0x4442B9-0x4442C2 (xor al,al)
    if ((launcher->vfptr->gbg_isGEAssaultMech(launcher)             // asm 0x4452C5-0x4452CF (slot 4 @+0x10)
            || launcher->vfptr->gbg_isAirCruiser(launcher))         // asm 0x4452D0-0x4452D6 (slot 6 @+0x18)
        && target->gbg_isAircraft())                   // asm 0x4442DD-0x4442E6 (the target's slot 5)
        RGE_Missile_Object__init_missile(missile, 0, target,        // asm 0x4442E8-0x444307 (the AA homing form: the launcher = 0)
            in_range, this_->attack_count);
    else
        RGE_Missile_Object__init_missile(missile, launcher, target, // asm 0x4442F9-0x444307 (the normal form: the launcher)
            in_range, this_->attack_count);
    this_->vfptr->notify(this_, this_->id, this_->id, 512,          // asm 0x44430E-0x444327 (slot 120 @+0x1E0; the 512/600 pair - the do_attack twin)
        RGE_UNITAI_ACTION_ATTACK, target->id, 0);
    if (X87_LE_NAN(master->weapon_range, 1.0f))                     // asm 0x44432D-0x444341 (fcom const_f_one; test ah,41h; jnz -> the return; the set {<=1, NaN})
        return 1;                                                   // asm loc_4443A6
    if (target->owner->vfptr->computerPlayer(target->owner) == 1)   // asm 0x444343-0x44434E (the owner vtbl slot 7 @+0x1C, RGE_PlayerVtbl :7134; cmp eax,1)
    {
        RGE_Master_Static_Object* tmaster = target->master_obj;     // asm 0x444350 ([edi+14h] - the target is base-typed: RGE_Static_Object::master_obj)
        int target_hp = (int)target->hp;                            // asm 0x444356-0x444360 (fld [edi+3Ch] = 60; __ftol)
        int target_group = tmaster->object_group;                   // asm 0x444353 (movsx word @+0x32 = 50)
        target->notify(target->id, this_->id, 527,   // asm 0x44435D-0x444373 (slot 120 @+0x1E0; mType 527 - the 5xx registry; the pushes: group,ftol(hp),this_id,527,target_id,target_id)
            this_->id, target_hp, target_group);
    }
    return 1;                                                       // asm 0x444379-0x444382 (al = 1)
}

// Replacement for RGE_Combat_Object::area_attack. Original: @ 0x00443EE0
// (idb_functions.h:1080). Range [0x443EE0, 0x444165) (lst :117999/:118264,
// retn 18h = 6 stack args), returns unsigned __int8. ABI: __thiscall.
// Virtual: vftable DATA XREF on both Combat and Missile vftables. NO SEH
// frame (the plain sub esp,28h + the edi push).
// Semantics: THE AREA-EFFECT DAMAGE DEALER (the attack-ground splash).
// master->area_effect_range (@+0x148 = 328, float) <= 0 (or NaN) ->
// return 0. range_sq = range^2. The tile bounds: min_x = ftol(x-range)-2,
// max_x = ftol(x+range)+2, min_y = ftol(y-range)-2, max_y = ftol(y+range)+2
// (the __ftol truncations + the 2-tile margins); min_x clamped >= 0,
// max_x clamped <= map->map_width, min_y clamped >= 0, max_y clamped
// <= map->map_height. min_y >= max_y -> return 1 (the vacuous return).
// The scan: for each row in [min_y, max_y), for each tile in
// [min_x, max_x): tile = map->map_row_offset[row][min_x] (the 32-byte
// RGE_Tile stride); tile->TileObject_Count (u8 @+13, :4182) == 0 -> the
// next tile; for each of the TileObject_Count units in tile->Object_List
// (RGE_Static_Object** @+20, :4186): skip this (self); skip when the
// unit's master->area_effect_object_level (u8 @+0x9B = 155) < this->
// master_obj->area_effect_level (u8 @+0x14C = 332 - the level gate: the
// lower-tier objects take no splash); skip the ignore_object. The hit
// test: the unit's master radii (radius_x @+0x3C = 60, radius_y @+0x40
// = 64) absorb the distance first: excess_x = max(|unit->world_x - x| -
// radius_x, 0); excess_y = max(|unit->world_y - y| - radius_y, 0) - the
// {<= radius, NaN} clamp forms (fcom + test ah,41h; jnz -> 0); damage
// fires when range_sq > excess_x^2 + excess_y^2 (fcompp + test ah,41h;
// jnz -> the skip - the {range_sq <= sum, NaN} skip set) AND the roll:
// (rand_internal() * 100) / RAND_MAX_INTERNAL <= this->master_obj->
// base_hit_chance (@+0x156 = 342) (the asm's cmp dx/jg -> the miss). On
// hit: modifier = calc_attack_modifier (slot 95 @+0x17C, :11429 - OWNED,
// routes; the st0 -> the float spill = the single store-round); the unit
// takes the damage (the unit vtbl slot 58 @+0xE8, :11392): damage(unit,
// master->weapon_num (@+0x138 = 312), master->weapon (@+0x13C = 316),
// modifier, this->owner @+0x18, attacker, first_call = 1). Return 1.
unsigned __int8 __fastcall RGE_Combat_Object__area_attack_new(
    RGE_Combat_Object* this_, DWORD dummy, float x, float y, float z,
    RGE_Combat_Object* attacker, RGE_Static_Object* ignore_object,
    RGE_Static_Object* target)
{
    RGE_Map* map = this_->owner->world->map;                        // asm 0x443EE6-0x443EF1 (owner @+0x18; world @+0x8C; map @+0x34)
    float range = this_->master_obj->area_effect_range;             // asm 0x443EEF-0x443EF2 (@+0x148 = 328, float)
    if (X87_LE_NAN(range, 0.0f))                                    // asm 0x443EF8-0x443F0A (fcom const_f_zero; test ah,41h; jz -> the continue; the return set {<=0, NaN})
        return 0;                                                   // asm 0x443F0C-0x443F14 (xor al,al)
    float range_sq = range * range;                                 // asm 0x443F17-0x443F1E (fld st/fmul; fstp var_8)
    int min_x = (int)(x - range) - 2;                               // asm 0x443F22-0x443F39 (__ftol; the -2 margin)
    int max_x = (int)(range + x) + 2;                               // asm 0x443F2D-0x443F4B (__ftol; the +2)
    int min_y = (int)(y - range) - 2;                               // asm 0x443F52-0x443F5E
    int max_y = (int)(range + y) + 2;                               // asm 0x443F61-0x443F66
    if (min_x < 0)                                                  // asm 0x443F69-0x443F70 (test bp/jge -> keep; the clamp)
        min_x = 0;
    if (max_x > map->map_width)                                     // asm 0x443F7A-0x443F8F (cmp [edx+8]/jle -> keep)
        max_x = map->map_width;
    if (min_y < 0)                                                  // asm 0x443F93-0x443F96
        min_y = 0;
    if (max_y > map->map_height)                                    // asm 0x443F9A-0x443FA2 (cmp [edx+0Ch]/jle)
        max_y = map->map_height;
    if (min_y >= max_y)                                             // asm 0x443FA6-0x443FA9 (cmp si/jge -> loc_44415C)
        return 1;                                                   // asm 0x44415C (al = 1 - the vacuous return)
    for (int row = min_y; row < max_y; ++row)                       // asm loc_443FC6-0x444156 (var_1C += 4/var_C--)
    {
        RGE_Tile* tile_row = map->map_row_offset[row];              // asm 0x443FEA-0x443FF4 ([edx+0xBF18h] + var_1C)
        for (int col = min_x; col < max_x; ++col)                   // asm loc_443FE6-0x444134 (var_14 += 32/var_10--)
        {
            RGE_Tile* tile = &tile_row[col];                        // asm 0x443FF7 (the 32-byte stride; [ebx+ecx+0Dh])
            if (tile->TileObject_Count == 0)                        // asm 0x443FFD-0x444124 (test al/jz)
                continue;
            for (int k = 0; k < (int)tile->TileObject_Count; ++k)   // asm loc_44400F-0x444116 (mov cl,[ebx+0Dh]/jl - the count re-read per iteration, faithful)
            {
                RGE_Combat_Object* unit =                           // asm 0x445400F-12 ([ebx+14h] + ebp*4; the cast: the game's uniform 232-slot vtbl treatment)
                    (RGE_Combat_Object*)tile->Object_List[k];
                if (unit == this_)                                  // asm 0x44544015-17 (cmp esi,edi/jz)
                    continue;
                if (unit->master_obj->area_effect_object_level      // asm 0x4454401D-29 (byte @+0x9B = 155)
                    < this_->master_obj->area_effect_level)         // asm 0x44544029-2F (byte @+0x14C = 332; jb -> the skip; the lower-tier objects take no splash)
                    continue;
                if ((RGE_Static_Object*)unit == ignore_object)      // asm 0x44544035-39 (cmp/jz; the derived-to-base cast for the pointer compare)
                    continue;
                double dx = fabs(unit->world_x - x);                // asm 0x4454403F-50 (fld/fsub/fabs)
                double dy = fabs(unit->world_y - y);                // asm 0x44544062-70 (fld/fsub/fabs)
                float excess_x = X87_LE_NAN(dx,                     // asm 0x44544052-59 (fcom st(1); test ah,41h; jnz -> 0)
                    unit->master_obj->radius_x)
                    ? 0.0f                                          // asm 0x4454405F-63 (fstp/fstp/fld const_f_zero)
                    : (float)(dx - unit->master_obj->radius_x);     // asm 0x4454405B (fsubrp st(1),st - the REVERSE subtract)
                float excess_y = X87_LE_NAN(dy,                     // asm 0x44544072-83 (fcom var_18; test ah,41h; jnz -> 0)
                    unit->master_obj->radius_y)
                    ? 0.0f                                          // asm 0x44544083-85 (fstp/fld const_f_zero)
                    : (float)(dy - unit->master_obj->radius_y);     // asm 0x4454407D (fsub var_18)
                float sum_sq = excess_x * excess_x                  // asm 0x4454408B-93 (fld st/fmul st,st(1)/fld st(2)/fmul st,st(3)/faddp)
                    + excess_y * excess_y;
                if (X87_LE_NAN((double)range_sq, (double)sum_sq))   // asm 0x44544095-0x445440A4 (fld var_8/fcompp/fnstsw/test ah,41h/jnz -> the skip; the set {range_sq <= sum, NaN})
                    continue;                                       // asm 0x445440A4 (jnz loc_44410E)
                int roll = (rand_internal() * 100)                  // asm 0x445440A6-0x445440C8 (rand; the lea/lea/shl *100 + the 80010003h magic-imul / sar 0Eh)
                    / RAND_MAX_INTERNAL;
                if (roll > this_->master_obj->base_hit_chance)      // asm 0x445440CA-0x445440D1 (cmp dx,[ecx+156h] = 342; jg -> the miss)
                    continue;
                float modifier = (float)this_->vfptr->calc_attack_modifier(this_,   // asm 0x445440D3-0x445440DE (slot 95 @+0x17C, :11429 - OWNED, routes; fstp var_4 = the float spill)
                    (RGE_Static_Object_idb*)unit);
                unit->vfptr->damage(unit,                           // asm 0x445440F7-0x44544108 (slot 58 @+0xE8, :11392 - dispatched ON THE UNIT, ecx = esi)
                    this_->master_obj->weapon_num,                  // asm 0x445440FD (movsx word @+0x138 = 312)
                    this_->master_obj->weapon,                      // asm 0x445440F7 ([eax+13Ch] = 316)
                    modifier,                                       // asm 0x445440F2 (the var_4 spill reloaded - the single store-round)
                    (RGE_Player*)this_->owner,                      // asm 0x445440F1 (@+0x18 = 24; the TRIBE_Player*/RGE_Player* dual-naming cast)
                    (RGE_Static_Object_idb*)attacker,                   // asm 0x445440DE (push ecx = the attacker arg; the derived-to-base cast)
                    1);                                             // asm 0x445440E9 (push 1 = first_call)
            }
        }
    }
    return 1;                                                       // asm 0x44415C-0x444165 (al = 1)
}

// Replacement for RGE_Combat_Object::enter. Original: @ 0x00445AB0
// (idb_functions.h:1104). Range [0x445AB0, 0x445CFD) (lst :121075/:121314,
// retn 8 = 2 stack args), returns int. ABI: __thiscall. Virtual:
// vftable DATA XREF on both Combat and Missile vftables + CODE XREF
// TRIBE_Combat_Object__enter+26 (the TRIBE tier forwards into this
// implementation - the entry hook covers that call). SEH: MSVC frame;
// the unwind funclet chunk 0x6415B0-0x6415BA = mov eax,[ebp+0Ch] / call
// operator_delete on the RAW allocation (scope stru_67FD98) - the
// family's ninth twin, same fold.
// Semantics: THE GARRISON ENTER BUILDER (the action_type 3 =
// RGE_ACTION_TYPE_ENTER gate) - the richest mission-builder variant.
// (1) oID resolves through RGE_Game_World__object (:6086, 0x00623DB0;
// the (RGE_Game_World*) dual-naming cast) - a null target returns 0.
// (2) THE AI GARRISON-REQUEST REMOVAL: when the owner has a playerAI
// (the decision module @+0x1DD0 = 7632, TRIBE_Player), the embedded
// tacticalAI (TribeTacticalAIModule) keeps gbg_garrisonUnits - the
// std::vector<UnitPair> (the _First/_Last/_End trio :2034-38; the
// UnitPair = {int first_id; int second_id}, :4839-4843 = {the
// garrisoning unit's id, the garrison target's id}): the records
// matching (this->id, oID) are REMOVED (the shift-left + the _Last
// decrement) - the unit found its garrison; the AI's request is
// fulfilled. (3) The already-check (skipped when unconditional): the
// current action's get_target_obj (slot 20 @+0x50) == the target AND
// its type == 3 -> return 1. (4) The re-target: the current action's
// gbg_unknown1 (slot 14 @+0x38, RGE_ActionVtbl :15976 - the re-target
// query, (this, oID) args) == 1 -> return 1. (5) The TASK path:
// getTask (slot 233 @+0x3A4, :11567) with the SELECTOR 3 (= the ENTER
// action type - the getTask's final i16 = the requested action type;
// the gather twin used -1) on (target, the target's world_x/world_y/
// world_z @+0x48/4C/50) - null -> return 0. (6) operator_new 0x44 =
// sizeof(RGE_Action_Enter), walked idb_types.h:28598-28619 (the base
// 64 + mFirstTimeEnter @64); ctor RGE_Action_Enter (obj_in = this,
// task_in = the task, target_obj_in = the target) - null -> return 0.
// (7) THE WORK-INTERRUPT CASCADE: when (master->object_group (@+0x1E =
// 30) == 1 [Herd Animal - the registry] AND hasActionOfType (slot 174
// @+0x2B8, :11508) == 107 [Artifact]) OR hasActionOfType == 5 [Gather]
// OR == 110 [Hunt] OR == 101 [Build]: the SUB-ACTION path - the
// current work action keeps running and the fresh ENTER action becomes
// its SUB-ACTION (add_sub_action :191, 0x0040B0A0; deleteList = 1):
// the work resumes after the garrison completes. Otherwise:
// delete_list; the task's type word (slot 6 @+0x18 - with the
// setUnitAIAction args staged around the call, asm 0x445CAD-0x445CB3)
// feeds setUnitAIAction (slot 234 @+0x3A8; the args: the task's type,
// the target, the target's x/y/z); set_only_action. Return 1.
int __fastcall RGE_Combat_Object__enter_new(
    RGE_Combat_Object* this_, DWORD dummy, int oID, int unconditional)
{
    RGE_Static_Object* target = RGE_Game_World__object(             // asm 0x445AD2-0x445AE1 (owner @+0x18; world @+0x8C, TRIBE_Player :727; the resolver :6086)
        (RGE_Game_World*)this_->owner->world, oID);
    if (target == 0)                                                // asm 0x445AE3-0x445AE9 (test/jz)
        return 0;                                                   // asm 0x445AEB-0x445AF5 (the eax = the resolver's null - the implicit return)
    TribeMainDecisionAIModule* player_ai = this_->owner->playerAI;  // asm 0x445AEF-0x445AF2 (@+0x1DD0 = 7632, TRIBE_Player; the .c: v6)
    if (player_ai != 0)                                             // asm 0x445AF8-0x445AFA (test/jz)
    {
        UnitPair** first = player_ai->tacticalAI.gbg_garrisonUnits._First;   // asm 0x445AFF-0x445B11 ([ecx+6138h])
        for (int i = 0; first != 0                                          // asm 0x445B07-09 (test edi/jz)
            && i < (int)(player_ai->tacticalAI.gbg_garrisonUnits._Last - first); ++i)   // asm 0x445B0B-18 (sub/sar 2 - the element count)
        {
            UnitPair* record = first[i];                                // asm 0x445B20-23 ([edx+eax*4])
            if (record->first_id == this_->id                           // asm 0x445B23-26 ([ebx] == ebp; the record's first_id = the unit)
                && record->second_id == oID)                            // asm 0x445B29-30 ([ebx+4] == ebx; the record's second_id = the target)
            {
                for (UnitPair** j = &first[i + 1];                      // asm 0x445B3F-42 (lea edi,[edx+eax*4])
                    j != player_ai->tacticalAI.gbg_garrisonUnits._Last; ++j)   // asm 0x445B42-53 (the shift-left loop)
                    *(j - 1) = *j;                                      // asm 0x445B49-53
                --player_ai->tacticalAI.gbg_garrisonUnits._Last;        // asm 0x445B5D-6A (the _Last decrement - one record removed)
                break;                                                  // asm 0x445B32-37
            }
        }
    }
    if (unconditional == 0                                          // asm 0x445B7A-0x445B7D
        && RGE_Action_List__have_action(this_->actions) == 1)       // asm 0x445B7C-0x445B89
    {
        RGE_Action* current = RGE_Action_List__get_action(this_->actions);   // asm 0x445B8B-0x445B91
        if (current->vfptr->get_target_obj(current) == (RGE_Static_Object_idb *)target       // asm 0x445B9A-0x445B9F (slot 20 @+0x50)
            && current->vfptr->type(current) == RGE_ACTION_TYPE_ENTER)       // asm 0x445BB0-0x445BB7 (slot 6 @+0x18; cmp ax,3)
            return 1;                                               // asm loc_445CE6
    }
    if (RGE_Action_List__have_action(this_->actions) == 1)          // asm 0x445BBD-0x445BCA
    {
        RGE_Action* current = RGE_Action_List__get_action(this_->actions);   // asm 0x445BCC-0x445BD2
        if (current->vfptr->gbg_unknown1(current, (int)this_, oID) == 1)     // asm 0x445BDD-0x445BE1 (slot 14 @+0x38, RGE_ActionVtbl :15976; the re-target query)
            return 1;                                               // asm 0x445BE4-0x445BE7 (cmp eax,1 -> loc_445CE6)
    }
    RGE_Task* task = this_->vfptr->getTask(this_, (RGE_Static_Object_idb*)target,           // asm 0x445BED-0x445C00 (slot 233 @+0x3A4, :11567; the SELECTOR = 3 = ENTER - the getTask i16 semantics confirmed; the pushes: 3,world_z,world_y,world_x,target)
        target->world_x, target->world_y, target->world_z, 3);
    if (task == 0)                                                  // asm 0x445C08-0x445C0A (test edi/jz - the eax = the getTask's null)
        return 0;                                                   // asm 0x445C0B-0x445C0D (the implicit return via loc_445CEB)
    RGE_Action_Enter* allocated =
        (RGE_Action_Enter*)operator_new_internal(sizeof(RGE_Action_Enter));   // asm 0x445C10-0x445C1A (push 44h; sizeof walked 68 = 0x44 - the base 64 + mFirstTimeEnter @64); EH state 0 @0x445C20
    RGE_Action* fresh = 0;                                          // asm 0x445C38 (xor edi,edi)
    __try
    {
        if (allocated != 0)                                         // asm 0x445C1E-0x445C28
            fresh = (RGE_Action*)RGE_Action_Enter__RGE_Action_Enter(
                allocated, (RGE_Action_Object*)this_, task, target);// asm 0x445C2A-0x445C2F (obj_in = this_, task_in = the task, target_obj_in = the target)
    }
    __except (operator_delete_internal(allocated), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x6415B0-0x6415BA (operator_delete, raw allocation)
    {
    }
    if (fresh == 0)                                                 // asm 0x445C3A-0x445C3C
        return 0;                                                   // asm 0x445C46-0x445C48 (jmp loc_445CEB)
    if (this_->master_obj->object_group == 1                        // asm 0x445C4D-0x445C4F (word @+0x1E = 30; cmp 1 = Herd Animal - the registry)
        && this_->vfptr->hasActionOfType(this_, 107)                // asm 0x445C57-0x445C5D (slot 174 @+0x2B8, :11508; 107 = Artifact)
        || this_->vfptr->hasActionOfType(this_, 5)                  // asm 0x445C67-0x445C6D (5 = Gather)
        || this_->vfptr->hasActionOfType(this_, 110)                // asm 0x445C79-0x445C7D (110 = Hunt)
        || this_->vfptr->hasActionOfType(this_, 101))               // asm 0x445C89-0x445C8D (101 = Build)
    {
        RGE_Action* work = RGE_Action_List__get_action(this_->actions);   // asm 0x445C97-0x445C9D
        RGE_Action__add_sub_action(work, fresh, 1);                 // asm 0x445CA2-0x445CE1 (add_sub_action :191, 0x0040B0A0; deleteList = 1 - the ENTER rides the interrupted work)
    }
    else
    {
        RGE_Action_List__delete_list(this_->actions);               // asm 0x445C97-0x445C9D
        int action_type = fresh->vfptr->type(fresh);                // asm 0x445CB5-0x445CB8 (slot 6 @+0x18; the setUnitAIAction args staged around the call: 0x445CAD-0x445CB3)
        this_->vfptr->setUnitAIAction(this_, action_type, (RGE_Static_Object_idb*)target,   // asm 0x445CBB-0x445CC1 (slot 234 @+0x3A8; the staged: target,x,y,z - the garrison target)
            target->world_x, target->world_y, target->world_z);     // asm 0x445CA2-0x445CB3 (the target's @+0x48/4C/50)
        this_->vfptr->set_only_action(this_, fresh);                // asm 0x445CC6-0x445CC9 (slot 231 @+0x39C)
    }
    return 1;                                                       // asm 0x445CE6-0x445CEB
}

// ============ RUNTIME METHODS (batch C4) ============

// Replacement for RGE_Combat_Object::update. Original: @ 0x00444DA0
// (idb_functions.h:1094). Range [0x444DA0, 0x4450B7) (lst :119666/:119927,
// retn = no stack args), unsigned __int8 return. ABI: __thiscall. Virtual:
// vftable slot 37 = +0x94 (line-arith 11389-11352 = 37; DATA XREF lst
// :987672 the Combat vftable + :990198 the Missile vftable). Caller
// coverage: RGE_Missile_Object__update tail-jmps here (lst :119666 - the
// hook covers the Missile tier's update dispatch) and TRIBE_Combat_Object__
// update+146 calls it (lst :119667 - the derived base call, hook-routed).
// Semantics: the per-frame combat tick - decay the attack timer, run the
// base Action update, refresh the unit's Unified Map Value footprint
// (buildings through their LINKED OWNER BUILDING's position/point-set per
// the base-pointer rule: the .c's `this[1].CurrentObstructionData` render
// = [this+0x1DC] = TRIBE_Building_Object::linked_owner @476, and its
// world_x/world_y/Multi_Unified_Points feed the same footprint math the
// position branch computes from this - walker-verified with the
// desolid_flag@565 + sizeof-632 calibrations; the .c's ObsRecord* value
// typing was the render artifact, re-typed per the field-pun ladder), and
// when the footprint CHANGED update the Visible_Unit_Manager record; the
// capture scan then hands a capture_flag unit to the first HUMAN player
// whose visibility bit is set (change_ownership + the fanfare), and the
// UnitAI module ticks when it is the owner's turn. Returns the base
// update's byte unchanged.
unsigned __int8 __fastcall RGE_Combat_Object__update_new(
    RGE_Combat_Object* this_, DWORD dummy)
{
    unsigned __int8 result;
    int pos_x;
    int pos_y;
    unsigned __int8 point_index;
    unsigned int prior_unified_map;

    // COMBAT_OBJECT_UPDATE_V2 (the core.cpp:1431 interior hook at the
    // entry+3, BAKED per the step-3b default): the patched gate proceeds
    // on NON-ZERO BITS (the stub's test eax,eax/jz skip): NEGATIVE
    // timers decay too and clamp to zero below - the deliberate v2 fix
    // vs the vanilla skip. THE GATE FORM: X87_NE_ORD (rule 2 - the
    // ordered-nonzero disjunction; the /Fa on the literal != run proved
    // it compiles SKIP-ON-NaN (ucomiss/lahf/test ah,68h/jnp - the NaN
    // lahf byte 47h & 68h = 40h, odd parity, jnp taken) while the v2
    // bits test PROCEEDS on NaN - the macro is the faithful form; the
    // -0.0f corner (the bits test proceeds where the ordered != skips)
    // is unreachable - the zero-stores write +0.0 and the decay of a
    // positive timer cannot produce -0)
    if (X87_NE_ORD(this_->attack_timer, 0.0f))                       // asm 0x444DA6-0x444DB7: fld/fcomp const_f_zero/test ah,41h/jnz - the jnz targets the SKIP branch (the vanilla MATCH row); PATCHED by the v2 bits test (the macro expansion: comiss+ja || comiss+jbe, unfolded)
    {
        double decayed = (double)this_->attack_timer                 // asm 0x444DBC (fld) - the x87 fsub computes the 53-bit difference (widen-before-op)
            - (double)this_->owner->world->world_time_delta_seconds;   // asm 0x444DB9-0x444DC8 (owner @+0x18 -> world @+0x8C; the field @+0xA8)
        this_->attack_timer = (float)decayed;                        // asm 0x444DCE (fst m32 - the store-round happens BEFORE the compare)
        if (X87_LT_NAN(decayed, 0.0))                                // asm 0x444DD4-0x444DDF: fcomp consumes the UNROUNDED x87 value; test ah,1/jz = the C0-only taken set {<, NaN} - the macro form (a literal < would leave NaN unzeroed)
            this_->attack_timer = 0.0f;                              // asm 0x444DE1
    }
    result = RGE_Action_Object__update((RGE_Action_Object*)this_);   // asm 0x444DF0 (:145; the direct constant call, hook-routing ruling)
    if (this_->id >= 0 && this_->tile)                               // asm 0x444DF9-0x444E09 (test/jl @0x444DFE; test/jz @0x444E09)
    {
        if (this_->master_obj->master_type == RGE_MASTER_TYPE_BUILDING   // asm 0x444E0F-0x444E16 (cmp byte [master+4], 50h)
            && ((TRIBE_Building_Object*)this_)->linked_owner != 0)   // asm 0x444E1C-0x444E24 ([esi+1DCh] = the Building linked_owner @476 - the base-pointer rule resolution; the cast is layout-safe: gated on master_type 80 = a real Building instance, 632 bytes)
        {
            TRIBE_Building_Object* linked =
                ((TRIBE_Building_Object*)this_)->linked_owner;       // asm 0x444E1C (ebp; the LINKED OWNER BUILDING - the .c's ObsRecord* value typing was the this[1] render artifact, re-typed per the field-pun ladder)
            pos_x = (int)linked->world_x;                            // asm 0x444E2A-0x444E32 (__ftol [ebp+48h] = the linked owner's world_x @0x48)
            pos_y = (int)linked->world_y;                            // asm 0x444E32-0x444E3C (__ftol [ebp+4Ch] = world_y @0x4C)
            point_index = linked->Multi_Unified_Points;              // asm 0x444E56 ([ebp+16Ch] = Multi_Unified_Points @0x16C)
            prior_unified_map = this_->Unified_Map_Value;            // asm 0x444E3C-0x444E49 (from THIS - the linked owner's UMV is not read; the store below is this_'s too, asm 0x444E50)
            // MAP_SIZE - asm 0x444E42 disp32 0x444E45: mapsize.cpp:1730
            // re-points this table read at the big-map mapptr base when
            // setMapSizeHooks runs. NOT baked (owner-directed MAP_SIZE rule
            // 2026-11-15): the replacement reads the original
            // unified_map_offsets.
            this_->Unified_Map_Value =
                (unsigned int)unified_map_offsets[pos_y][pos_x];     // asm 0x444E42-0x444E50
            if (point_index)                                         // asm 0x444E5C-0x444E5E (test cl,cl; jbe - load-bearing: the index-1 fetch would underflow)
            {
                UMV_Offset_Point* points =
                    this_->owner->world->map->UMV_Data_Points[point_index - 1];   // asm 0x444E64-0x444E80 (RGE_Map+0xBF68, the map_row_offset@0xBF18 walk; index - 1 - the owned Doppleganger walk precedent)
                int count = points->Xoffset;                         // asm 0x444E80 (the count lives in the first point's Xoffset)
                // RESTRUCTURE (the owned Doppleganger walk form): the asm
                // count>=1 guard (cmp edx,1/jl @0x444E82-0x444E85) + the
                // do-while = exactly `count` iterations of points[1..count];
                // the indexed for reproduces the iteration set for every
                // count (0 iterations when count < 1).
                for (int k = 0; k < count; ++k)
                {
                    int row = pos_y + points[1 + k].Yoffset;         // asm 0x444E8E-0x444E95
                    int col = pos_x + points[1 + k].Xoffset;
                    // MAP_SIZE - asm 0x444E9A disp32 0x444E9D:
                    // mapsize.cpp:1731. NOT baked (owner-directed MAP_SIZE
                    // rule): the original table.
                    this_->Unified_Map_Value |=
                        (unsigned int)unified_map_offsets[row][col];   // asm 0x444E9A-0x444EAD (the read-modify-or through @+0x168)
                }
            }
        }
        else
        {
            pos_x = (int)this_->world_x;                             // asm 0x444EBA-0x444EC2 (__ftol)
            pos_y = (int)this_->world_y;                             // asm 0x444EC2-0x444ECC (__ftol)
            point_index = this_->Multi_Unified_Points;               // asm 0x444EE6 (@+0x16C)
            prior_unified_map = this_->Unified_Map_Value;            // asm 0x444ECC-0x444ED9
            // MAP_SIZE - asm 0x444ED2 disp32 0x444ED5: mapsize.cpp:1732.
            // NOT baked (owner-directed MAP_SIZE rule 2026-11-15).
            this_->Unified_Map_Value =
                (unsigned int)unified_map_offsets[pos_y][pos_x];     // asm 0x444ED2-0x444EE0
            if (point_index)                                         // asm 0x444EEC-0x444EEE (load-bearing)
            {
                UMV_Offset_Point* points =
                    this_->owner->world->map->UMV_Data_Points[point_index - 1];   // asm 0x444EF0-0x444F0C
                int count = points->Xoffset;                         // asm 0x444F0C
                // RESTRUCTURE: the mirror of the building-path walk (asm
                // 0x444F0E-0x444F3B; the count>=1 guard @0x444F0E + the
                // do-while -> the indexed for, same iteration set).
                for (int k = 0; k < count; ++k)
                {
                    int row = pos_y + points[1 + k].Yoffset;         // asm 0x444F16-0x444F1D
                    int col = pos_x + points[1 + k].Xoffset;
                    // MAP_SIZE - asm 0x444F22 disp32 0x444F25:
                    // mapsize.cpp:1733. NOT baked (owner-directed MAP_SIZE
                    // rule).
                    this_->Unified_Map_Value |=
                        (unsigned int)unified_map_offsets[row][col];   // asm 0x444F22-0x444F35
                }
            }
        }
        if (this_->Unified_Map_Value != prior_unified_map)           // asm 0x444F3D-0x444F49 (cmp; jz -> the tail)
        {
            // MAP_SIZE: mapsize.cpp:1821 re-points the manager-slot read
            // (disp32 0x444F7A, the mov ecx at 0x444F78) at the patch's
            // mapptr_2 when setMapSizeHooks runs. NOT baked (owner-directed
            // MAP_SIZE rule 2026-11-15): the replacement reads the original
            // game slot 0x007A1CB8.
            Visible_Unit_Manager__Update_Unit_Info(                  // asm 0x444F4F-0x444F84 (:5928; the push order verified)
                *game_visible_unit_manager,                          // asm 0x444F78
                this_->id,                                           // asm 0x444F74-0x444F7E (@+0x10)
                this_->owner->id,                                    // asm 0x444F6E-0x444F77 (@+0xA0)
                pos_x,                                               // asm push edi (the ftol'd x)
                pos_y,                                               // asm push eax (the ftol'd y)
                this_->Large_Object_Radius,                          // asm 0x444F61 (@+0x16D)
                this_->master_obj->object_group,                     // asm 0x444F58-0x444F5D (movsx word @+0x1E)
                prior_unified_map,
                this_->Unified_Map_Value,
                this_->VUR_Ptrs);                                    // asm 0x444F4F (lea @+0x144)
            if (this_->capture_flag)                                 // asm 0x444F84-0x444F8C (@+0x178)
            {
                TRIBE_Player* owner = this_->owner;                  // asm 0x444F92
                TRIBE_World* world = owner->world;                   // asm 0x444F9A (@+0x8C)
                int owner_id = owner->id;                            // asm 0x444FAC-0x444FB2 (@+0xA0; the asm hoists it to a stack slot - the read-once form)
                int player_num = world->player_num;                  // asm 0x444FA0 (movsx word @+0x48)
                // RESTRUCTURE (redundant-guard elimination x2): the asm gate
                // cmp edx,1/jle (0x444FA4-0x444FA6) is subsumed by the scan
                // condition (0 iterations when player_num <= 1 - the same
                // path set), and the asm re-test test edi,edi/jl
                // (0x444FD3-0x444FD5) is always false (the scan index runs
                // [1, player_num), never decremented) - both collapsed.
                for (int i = 1; i < player_num; ++i)                 // asm 0x444FB6-0x444FCC
                {
                    if (i == owner_id)                               // asm 0x444FB6-0x444FB8 (the owner-id slot cmp; jz -> the increment)
                        continue;
                    if (((1 << i) & this_->Unified_Map_Value) == 0)  // asm 0x444FBC-0x444FC7 (shl eax,cl / test ebp,eax; the ja-found form inverted)
                        continue;
                    TRIBE_Player* capturing = world->players[i];     // asm 0x444FDB-0x444FDE (fetch 1, @+0x4C)
                    if (!capturing->vfptr->computerPlayer(capturing))   // asm 0x444FE1-0x444FE8 (slot 7 = +0x1C, line-arith 1101-1094)
                    {
                        RGE_Player* new_owner =
                            (RGE_Player*)this_->owner->world->players[i];   // asm 0x444FEE-0x444FFE (fetch 2: the world + players RE-LOAD - the double-fetch kept faithful; the TRIBE_Player* -> RGE_Player* dual-naming cast, the Doppleganger :527/:541 precedent)
                        this_->vfptr->change_ownership(this_, new_owner);   // asm 0x445002 (slot 47 = +0xBC, line-arith 11399-11352)
                        if (new_owner == RGE_Base_Game__get_player((*base_game))   // asm 0x445008-0x445015 (:689)
                            && (this_->master_obj->master_type != RGE_MASTER_TYPE_BUILDING   // asm 0x445017-0x44501E (cmp byte @+4, 50h)
                                || ((TRIBE_Building_Object*)this_)->linked_owner == 0))   // asm 0x445020-0x445028 (the 0x1DC load - the linked_owner)
                        {
                            RGE_Base_Game__play_sound((*base_game), 38, 0, 0);   // asm 0x44502A-0x445036 (:804; sound_id 38 = the capture fanfare, the game-facts candidate)
                            RGE_View* view =
                                (RGE_View*)(*base_game)->vfptr->get_view_panel((*base_game));   // asm 0x44503B-0x445043 (slot 12 = +0x30, line-arith 16945-16933; the TPanel* -> RGE_View* dual-naming cast, the .c's own render)
                            if (view)
                                RGE_View__display_object_selection(view, this_->id, 1500, 2, 4);   // asm 0x44504A-0x445059 (:5857; 5DC = 1500)
                        }
                        if (this_->capture_flag == 1)                // asm 0x44505E-0x445065
                            this_->capture_flag = 0;                 // asm 0x445067
                    }
                    break;                                           // asm: the found path leaves the scan (0x444FD3 -> ... -> loc_44506E), computer or not - the exhausted path jmps to loc_44506E too
                }
            }
        }
    }
    if (this_->unitAIValue
        && RGE_Game_World__is_player_turn(                           // asm 0x44506E-0x44508F (:6054)
            (RGE_Game_World*)this_->owner->world,                    // the TRIBE_World* -> RGE_Game_World* dual-naming cast (the asm calls the RGE_Game_World function with the owner->world pointer; map @0x34 identical in both renderings)
            this_->owner->id))
    {
        unsigned int accum_delta = RGE_Game_World__get_accum_time_delta(   // asm 0x445091-0x4450A1 (:6055)
            (RGE_Game_World*)this_->owner->world, this_->owner->id);
        UnitAIModule__update(this_->unitAIValue, accum_delta);       // asm 0x4450A6-0x4450AA (:454; the asm re-reads @+0x7C for the call - kept)
    }
    return result;                                                   // asm 0x4450AF-0x4450B7 (al)
}

void install_RGE_Combat_Object()
{
    setHook((void*)0x00443990, RGE_Combat_Object__RGE_Combat_Object_new);
    setHook((void*)0x00443A80, RGE_Combat_Object__scalar_deleting_destructor_new);
    setHook((void*)0x00443AA0, RGE_Combat_Object__RGE_Combat_Object_read_new);
    setHook((void*)0x00443B10, RGE_Combat_Object__destructor_new);
    setHook((void*)0x00443BB0, RGE_Combat_Object__recycle_in_to_game_new);
    setHook((void*)0x00443DF0, RGE_Combat_Object__stop_new);
    setHook((void*)0x00443E10, RGE_Combat_Object__calc_attack_modifier_new);
    setHook((void*)0x00444AE0, RGE_Combat_Object__can_attack_new);
    setHook((void*)0x00444B00, RGE_Combat_Object__reset_attack_timer_new);
    setHook((void*)0x00444C80, RGE_Combat_Object__get_weapon_range_new);
    setHook((void*)0x00444CB0, RGE_Combat_Object__get_speed_of_attack_new);
    setHook((void*)0x00445510, RGE_Combat_Object__moveTo3_new);
    setHook((void*)0x00445F30, RGE_Combat_Object__pause_new);
    setHook((void*)0x00445F50, RGE_Combat_Object__rateOfFire_new);
    setHook((void*)0x00446090, RGE_Combat_Object__weaponRange_new);
    setHook((void*)0x004460A0, RGE_Combat_Object__minimumWeaponRange_new);
    setHook((void*)0x00446150, RGE_Combat_Object__currentTargetX_new);
    setHook((void*)0x00446180, RGE_Combat_Object__currentTargetY_new);
    setHook((void*)0x004461B0, RGE_Combat_Object__currentTargetZ_new);
    setHook((void*)0x00444CE0, RGE_Combat_Object__release_being_worked_on_new);
    setHook((void*)0x004450C0, RGE_Combat_Object__copy_obj_new);
    setHook((void*)0x004460B0, RGE_Combat_Object__currentTargetID_new);
    setHook((void*)0x00446100, RGE_Combat_Object__currentTargetID2_new);
    setHook((void*)0x00446260, RGE_Combat_Object__get_visibility_new);
    setHook((void*)0x00446020, RGE_Combat_Object__inAttackRange_new);
    setHook((void*)0x004461E0, RGE_Combat_Object__teleport_new);
    setHook((void*)0x00445110, RGE_Combat_Object__notify_new);
    setHook((void*)0x00444D20, RGE_Combat_Object__enter_obj_new);
    setHook((void*)0x00443C20, RGE_Combat_Object__setup_new);
    setHook((void*)0x00443CA0, RGE_Combat_Object__setup_read_new);
    setHook((void*)0x00443D60, RGE_Combat_Object__save_new);
    setHook((void*)0x00444B10, RGE_Combat_Object__get_armor_new);
    setHook((void*)0x00443E20, RGE_Combat_Object__transform_new);
    setHook((void*)0x00445F60, RGE_Combat_Object__damageCapability_new);
    setHook((void*)0x00445D00, RGE_Combat_Object__transport_new);
    setHook((void*)0x00445DF0, RGE_Combat_Object__unload_new);
    setHook((void*)0x00445EF0, RGE_Combat_Object__stopAction_new);
    setHook((void*)0x00443950, RGE_Combat_Object__is_invisible_new);
    setHook((void*)0x00444BD0, RGE_Combat_Object__get_weapon_new);
    setHook((void*)0x00444720, RGE_Combat_Object__do_attack_new);
    setHook((void*)0x00444820, RGE_Combat_Object__damage_new);
    setHook((void*)0x004449C0, RGE_Combat_Object__calculateDamage_new);
    setHook((void*)0x00445980, RGE_Combat_Object__explore_new);
    setHook((void*)0x00445530, RGE_Combat_Object__moveTo2_new);
    setHook((void*)0x00445380, RGE_Combat_Object__attack_new);
    setHook((void*)0x00445660, RGE_Combat_Object__moveTo_new);
    setHook((void*)0x004457F0, RGE_Combat_Object__gather_new);
    setHook((void*)0x00445190, RGE_Combat_Object__attack2_new);
    setHook((void*)0x00444170, RGE_Combat_Object__attack4_new);
    setHook((void*)0x00443EE0, RGE_Combat_Object__area_attack_new);
    setHook((void*)0x00445AB0, RGE_Combat_Object__enter_new);
    setHook((void*)0x00444DA0, RGE_Combat_Object__update_new);
}
