#include "stdafx.h"
#include "RGE_Master_Static_Object.h"

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

void install_RGE_Combat_Object()
{
    setHook((void*)0x00443990, RGE_Combat_Object__RGE_Combat_Object_new);
    setHook((void*)0x00443A80, RGE_Combat_Object__scalar_deleting_destructor_new);
    setHook((void*)0x00443AA0, RGE_Combat_Object__RGE_Combat_Object_read_new);
    setHook((void*)0x00443B10, RGE_Combat_Object__destructor_new);
    setHook((void*)0x00443BB0, RGE_Combat_Object__recycle_in_to_game_new);
}
