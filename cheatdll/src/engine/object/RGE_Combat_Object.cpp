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
// slot 124 = +0x1F0, line-arith 11722-11598 = 124; asm 0x445521 call
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
        this_->master_obj, (RGE_Static_Object*)this_);
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
            this_->master_obj, (RGE_Static_Object*)this_))      // asm 0x4450C5-0x4450CA (slot +0x84)
    {
        RGE_Sprite* sprite = combat_source->vfptr->get_fight_sprite(
            combat_source, (RGE_Static_Object*)this_);          // asm 0x4450E0 (slot +0x84)
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
            *game_visible_unit_manager,
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
            *game_visible_unit_manager,
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
            *game_visible_unit_manager,
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
        if (module->vfptr->canAttackUnit(module, attackingUnit))    // asm 0x40945F81 (slot 19 = +0x4C)
        {
            double modifier = this_->vfptr->calc_attack_modifier(   // asm 0x40945F8D (slot 95 = +0x17C)
                this_, attackingUnit);
            float modifier_f = (float)modifier;                     // asm 0x40945F99 (the fstp m32 store-round)
            // The original transparently passes calculateDamage's st0
            // through as damageCapability's return (asm: no x87
            // management between the call and retn). The .c's void
            // cast-form render was the artifact.
            return attackingUnit->vfptr->calculateDamage(attackingUnit,   // asm 0x40945FB7 (slot 59 = +0xEC)
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
}
