#include "stdafx.h"

// .rdata vftable of TRIBE_Building_Object (lst :994974)
static TRIBE_Building_ObjectVtbl* const rge_tribe_building_object_vftable =
    (TRIBE_Building_ObjectVtbl*)0x00663110;

// unified_map_offsets = project constant (common.h:119, common.cpp:24 =
// .data:007A1CBC); row-dword reads: unified_map_offsets[row] = table + row*4.

// Replacement for TRIBE_Building_Object::TRIBE_Building_Object (runtime
// ctor1, the uID form). Original: @ 0x005531A0 (idb_functions.h:3907).
// Range [0x5531A0, 0x5533C7) (lst :566272/:566433, retn 1Ch = 7 stack
// args incl. uID), returns this. ABI: __thiscall - this_ in ECX.
// Caller: TRIBE_Master_Building_Object__make_new_obj2 +54 (owned,
// hook-routed; lst :566273).
// Decode (asm 0x5531A0-0x5533C7): base runtime TRIBE_Combat ctor with
// do_setup = 0 (hooked -> routes), then the Building(80) extension init
// in asm order 0x5531E3-0x55335F (all fields census-cited idb_types.h
// :9857-9899; exact offsets from the asm stores), the vfptr store
// (asm 0x5532CE, AFTER the field block), the linked_child[4] clear
// (asm 0x5532D4-0x5532EA), the gather-point block (asm 0x5532F0-
// 0x553318), IF do_setup the derived Building setup (:3915, same-class
// call), the Monument default-task set (master id 276 = Monument,
// owner-supplied: word @+0x18 == 0x114 -> set_task(master->default_task
// @+0x108), idb :139), and the doppleganger-install tail.
// SEH fold: chunk 0x6448F0 = mov ecx, [ebp-10h] / jmp
// TRIBE_Combat_Object__destructor (0x555A040 - HOOKED -> routes; the
// IMMEDIATE base level - runtime ladder evidence).
TRIBE_Building_Object* __fastcall TRIBE_Building_Object__TRIBE_Building_Object_new(
    TRIBE_Building_Object* this_, DWORD dummy,
    TRIBE_Master_Building_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z, int uID, int do_setup)
{
    TRIBE_Building_Object* result = this_;
    __try
    {
        TRIBE_Combat_Object__TRIBE_Combat_Object(
            (TRIBE_Combat_Object*)this_, (TRIBE_Master_Combat_Object*)tobj, obj_owner, x, y, z, 0);
        this_->save_damage_percent = 0;                     // asm 0x5531E3 (byte @+0x1CC)
        this_->captured_unit_count = 0;                     // asm 0x5531E9 (byte @+0x1CD)
        this_->extra_actions = 0;                           // asm 0x5531EF (@+0x1D0)
        this_->research_actions = 0;                        // asm 0x5531F5 (@+0x1D4)
        this_->villager_count = 0;                          // asm 0x5531FB (@+0x1D8)
        this_->linked_owner = 0;                            // asm 0x553201 (@+0x1DC)
        this_->production_queue = 0;                        // asm 0x553207 (@+0x1F4)
        this_->production_queue_size = 0;                   // asm 0x55320D (WORD @+0x1F8)
        this_->production_queue_count = 0;                  // asm 0x55321C (WORD @+0x1FA)
        this_->production_queue_total_units = 0;            // asm 0x553223 (WORD @+0x1FC)
        this_->production_queue_enabled = 1;                // asm 0x553227-0x55322A (byte @+0x1FE)
        this_->production_queue_actions = 0;                // asm 0x553230 (@+0x200)
        this_->production_queue_change_flag = 0;            // asm 0x553236 (byte @+0x204)
        this_->production_queue_auto_paused = 0;            // asm 0x55323C (byte @+0x205)
        this_->production_queue_need_attr = -1;             // asm 0x553242 (WORD @+0x206)
        this_->relic_count = 0;                             // asm 0x553249 (@+0x208)
        this_->specific_relic_count = 0;                    // asm 0x55324F (@+0x20C)
        this_->raider_transform_count = 0;                  // asm 0x553255 (@+0x210)
        this_->wonder_node = 0;                             // asm 0x5532B5-region (@+0x22C)
        this_->default_frame = 0;                           // asm 0x553261 (byte @+0x234)
        this_->desolid_flag = 0;                            // asm 0x553267 (byte @+0x235)
        this_->pending_order = 0;                           // asm 0x55326D (@+0x238)
        this_->snow_flag = 0;                               // asm 0x553273 (byte @+0x23C)
        this_->mEndpoint1.x = 0.0f;                         // asm 0x553279 (@+0x240)
        this_->mEndpoint1.y = 0.0f;                         // asm 0x55327F (@+0x244)
        this_->mEndpoint1.z = 0.0f;                         // asm 0x553285 (@+0x248)
        this_->mEndpoint2.x = 0.0f;                         // asm 0x55328B (@+0x24C)
        this_->mEndpoint2.y = 0.0f;                         // asm 0x553291 (@+0x250)
        this_->mEndpoint2.z = 0.0f;                         // asm 0x553297 (@+0x254)
        this_->mGateLocked = 0;                             // asm 0x5532A9 (@+0x258)
        this_->mCloseTimer = 0.0f;                          // asm 0x5532AF (@+0x260)
        this_->building_terrain_type = 0xFF;                // asm 0x5532B5 (byte @+0x264; the -1 store)
        this_->semi_asleep = 0;                             // asm 0x5532BC (byte @+0x265)
        this_->unique_build_id = uID;                       // asm 0x5532C2 (@+0x268; ctor2 stores -1, ctor3 none)
        this_->DoppleInstalled = 0;                         // asm 0x5532C8-region (@+0x270)
        this_->vfptr = rge_tribe_building_object_vftable;   // asm 0x5532CE (after the field block)
        for (size_t i = 0; i < _countof(this_->linked_child); ++i)   // asm 0x5532D4-0x5532EA (@+0x1E0..0x1EC)
            this_->linked_child[i] = 0;
        this_->gather_point_target = -1;                    // asm 0x5532F0 (@+0x224)
        this_->gather_point_target_master = -1;             // asm 0x5532F6 (WORD @+0x228)
        this_->gather_point_exists = 0;                     // asm 0x553303 (@+0x214)
        this_->gather_point_x = 0;                          // asm 0x553318 (@+0x218)
        this_->gather_point_y = 0;                          // asm 0x55330F (@+0x21C)
        this_->gather_point_z = 0;                          // asm 0x553309 (@+0x220)
        if (do_setup != 0)
            TRIBE_Building_Object__setup(this_, tobj, obj_owner, x, y, z);   // asm 0x553330 (:3915)
        if (this_->master_obj->id == 276)                   // asm 0x553335-0x55333E (word @+0x18; 276 = Monument)
            RGE_Action_Object__set_task((RGE_Action_Object*)this_,
                this_->master_obj->default_task);           // asm 0x553338-0x55334A (:139; default_task @+0x108)
        TRIBE_Master_Building_Object* master = this_->master_obj;
        __int16 object_group = master->object_group;        // asm 0x55334F-0x553352 (word @+0x1E)
        if (object_group != 34 && object_group != 36        // asm 0x553356-0x553360 (0x22 Undeployed Cannon /
            && this_->desolid_flag == 0                     // 0x24 Cannon; asm jz skip both)
            && (master->object_flags & 4) == 0              // asm 0x55336A-0x553371 (byte @+0xAC, bit 4)
            && (this_->linked_owner == 0 || object_group == 8))   // asm 0x553373-0x55337F (@+0x1DC; 8 = Gate)
        {
            // MAP_SIZE: mapsize.cpp:1749 re-points the tile-table disp32
            // (0x553396) at mapptr when setMapSizeHooks runs. NOT baked
            // (owner-directed MAP_SIZE rule): the replacement keeps the
            // original's unified_map_offsets base - under the big-map mod
            // the dopple tile intentionally diverges from the patched
            // original.
            this_->DoppleInstalled = RGE_Doppleganger_Creator__add_doppleganger_check(
                this_->owner->doppleganger_creator,         // asm 0x55339D-0x5533A2 (@+0x90)
                (RGE_Static_Object*)this_,
                (unsigned int*)((int)unified_map_offsets[(int)this_->world_y]   // MAP_SIZE - asm 0x553381-0x55339A:
                    + 4 * (int)this_->world_x));            // ftol(world_y @0x4C) row, + ftol(world_x @0x48)*4
        }
        result = this_;
    }
    __except (TRIBE_Combat_Object__destructor((TRIBE_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x6448F0
    {
    }
    return result;
}

// Replacement for TRIBE_Building_Object::scalar_deleting_destructor.
// Original: @ 0x005533F0 (idb_functions.h:3910). Range [0x5533F0,
// 0x55340B) (lst :566469/:566487, retn 4), void* result. ABI:
// __thiscall. Virtual: vftable slot 0 (lst :566470). Body: the Building
// destructor (IN BATCH - the direct call routes into
// TRIBE_Building_Object__destructor_new per the hook-routing ruling),
// then flags & 1 -> operator_delete, return this. No SEH.
void* __fastcall TRIBE_Building_Object__scalar_deleting_destructor_new(
    TRIBE_Building_Object* this_, DWORD dummy, unsigned int flags)
{
    TRIBE_Building_Object__destructor(this_);
    if (flags & 1)
        operator_delete_internal(this_);
    return this_;
}

// Replacement for TRIBE_Building_Object::TRIBE_Building_Object2 (runtime
// ctor2, no uID). Original: @ 0x00553410 (idb_functions.h:3911). Range
// [0x553410, 0x553633) (lst :566496/:566655, retn 18h = 6 stack args),
// returns this. ABI: __thiscall - this_ in ECX. Caller: TRIBE_Master_
// Building_Object__make_new_obj +4F (owned, hook-routed; lst :566497).
// Decode (asm 0x553410-0x553633): IDENTICAL to ctor1 except
// unique_build_id @+0x268 = -1 (asm 0x55352E stores eax from the
// or eax, -1; ctor1 stores the uID param) and the SEH chunk
// (0x644910 -> TRIBE_Combat_Object__destructor, HOOKED -> routes).
TRIBE_Building_Object* __fastcall TRIBE_Building_Object__TRIBE_Building_Object2_new(
    TRIBE_Building_Object* this_, DWORD dummy,
    TRIBE_Master_Building_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z, int do_setup)
{
    TRIBE_Building_Object* result = this_;
    __try
    {
        TRIBE_Combat_Object__TRIBE_Combat_Object(
            (TRIBE_Combat_Object*)this_, (TRIBE_Master_Combat_Object*)tobj, obj_owner, x, y, z, 0);
        this_->save_damage_percent = 0;                     // asm 0x553453
        this_->captured_unit_count = 0;                     // asm 0x553459
        this_->extra_actions = 0;                           // asm 0x55345F
        this_->research_actions = 0;                        // asm 0x553465
        this_->villager_count = 0;                          // asm 0x55346B
        this_->linked_owner = 0;                            // asm 0x553471
        this_->production_queue = 0;                        // asm 0x553477
        this_->production_queue_size = 0;                   // asm 0x55347D (WORD)
        this_->production_queue_count = 0;                  // asm 0x55348C (WORD)
        this_->production_queue_total_units = 0;            // asm 0x55348C-0x553493 (WORD)
        this_->production_queue_enabled = 1;                // asm 0x55349A
        this_->production_queue_actions = 0;                // asm 0x5534A0
        this_->production_queue_change_flag = 0;            // asm 0x5534A6
        this_->production_queue_auto_paused = 0;            // asm 0x5534AC
        this_->production_queue_need_attr = -1;             // asm 0x5534B2 (WORD)
        this_->relic_count = 0;                             // asm 0x5534B9
        this_->specific_relic_count = 0;                    // asm 0x5534BF
        this_->raider_transform_count = 0;                  // asm 0x5534C5
        this_->wonder_node = 0;                             // asm 0x5534CB-region
        this_->default_frame = 0;                           // asm 0x5534D1
        this_->desolid_flag = 0;                            // asm 0x5534D7
        this_->pending_order = 0;                           // asm 0x5534DD
        this_->snow_flag = 0;                               // asm 0x5534E3
        this_->mEndpoint1.x = 0.0f;                         // asm 0x5534E9-0x5534F5
        this_->mEndpoint1.y = 0.0f;
        this_->mEndpoint1.z = 0.0f;
        this_->mEndpoint2.x = 0.0f;
        this_->mEndpoint2.y = 0.0f;
        this_->mEndpoint2.z = 0.0f;
        this_->mGateLocked = 0;                             // asm 0x553515
        this_->mCloseTimer = 0.0f;                          // asm 0x55351B
        this_->building_terrain_type = 0xFF;                // asm 0x553521
        this_->semi_asleep = 0;                             // asm 0x553528
        this_->unique_build_id = -1;                        // asm 0x55352E (eax from or eax, -1)
        this_->DoppleInstalled = 0;                         // asm 0x553534-region
        this_->vfptr = rge_tribe_building_object_vftable;   // asm 0x55353A
        for (size_t i = 0; i < _countof(this_->linked_child); ++i)   // asm 0x553540-0x553556
            this_->linked_child[i] = 0;
        this_->gather_point_target = -1;                    // asm 0x55355C
        this_->gather_point_target_master = -1;             // asm 0x553562 (WORD)
        this_->gather_point_exists = 0;                     // asm 0x553587-region (@+0x214)
        this_->gather_point_x = 0;                          // asm 0x553575-region (@+0x218)
        this_->gather_point_y = 0;                          // asm 0x55357B-region (@+0x21C)
        this_->gather_point_z = 0;                          // asm 0x553581-region (@+0x220)
        if (do_setup != 0)
            TRIBE_Building_Object__setup(this_, tobj, obj_owner, x, y, z);   // asm 0x55359C (:3915)
        if (this_->master_obj->id == 276)                   // asm 0x5535A1-0x5535AA (Monument)
            RGE_Action_Object__set_task((RGE_Action_Object*)this_,
                this_->master_obj->default_task);           // asm 0x5535AC-0x5535B6 (:139)
        TRIBE_Master_Building_Object* master = this_->master_obj;
        __int16 object_group = master->object_group;        // asm 0x5535BB-0x5535BE
        if (object_group != 34 && object_group != 36        // asm 0x5535C2-0x5535CC
            && this_->desolid_flag == 0                     // asm 0x5535CE-0x5535D4
            && (master->object_flags & 4) == 0              // asm 0x5535D6-0x5535DD
            && (this_->linked_owner == 0 || object_group == 8))   // asm 0x5535DF-0x5535EB
        {
            // MAP_SIZE: mapsize.cpp:1750 re-points the tile-table disp32
            // (0x553602) at mapptr when setMapSizeHooks runs. NOT baked
            // (owner-directed MAP_SIZE rule): the replacement keeps the
            // original's unified_map_offsets base.
            this_->DoppleInstalled = RGE_Doppleganger_Creator__add_doppleganger_check(
                this_->owner->doppleganger_creator,         // asm 0x553609-0x55360E (@+0x90)
                (RGE_Static_Object*)this_,
                (unsigned int*)((int)unified_map_offsets[(int)this_->world_y]   // MAP_SIZE - asm 0x5535ED-0x553606
                    + 4 * (int)this_->world_x));
        }
        result = this_;
    }
    __except (TRIBE_Combat_Object__destructor((TRIBE_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x644910
    {
    }
    return result;
}

// Replacement for TRIBE_Building_Object::TRIBE_Building_Object3 (runtime
// ctor_read; the IDA name is numbered, the signature is the ctor_read
// shape). Original: @ 0x00553640 (idb_functions.h:3912). Range
// [0x553640, 0x553823) (lst :566664/:566797, retn 0Ch = 3 stack args),
// returns this. ABI: __thiscall - this_ in ECX. Caller: TRIBE_Object_
// List__Load +6D (lst :566665).
// Decode (asm 0x553640-0x553823): base runtime TRIBE_Combat ctor_read
// (TRIBE_Combat_Object3, :3984 - HOOKED -> routes), the SAME extension
// init block (asm 0x553672-0x5537FD; NO unique_build_id store - the asm
// leaves @+0x268 to the derived setup_read), vfptr, linked_child[4],
// gather-point block, IF do_setup the derived Building setup_read
// (:3916, same-class call), then the doppleganger-install tail (NO
// set_task block in ctor3).
// SEH fold: chunk 0x644930 -> TRIBE_Combat_Object__destructor (HOOKED,
// routes).
TRIBE_Building_Object* __fastcall TRIBE_Building_Object__TRIBE_Building_Object3_new(
    TRIBE_Building_Object* this_, DWORD dummy, int infile, RGE_Game_World* gworld, int do_setup)
{
    TRIBE_Building_Object* result = this_;
    __try
    {
        TRIBE_Combat_Object__TRIBE_Combat_Object3((TRIBE_Combat_Object*)this_, infile, gworld, 0);
        this_->save_damage_percent = 0;                     // asm 0x553672
        this_->captured_unit_count = 0;                     // asm 0x553678
        this_->extra_actions = 0;                           // asm 0x55367E
        this_->research_actions = 0;                        // asm 0x553684
        this_->villager_count = 0;                          // asm 0x55368A
        this_->linked_owner = 0;                            // asm 0x553690
        this_->production_queue = 0;                        // asm 0x553696
        this_->production_queue_size = 0;                   // asm 0x5536A4 (WORD)
        this_->production_queue_count = 0;                  // asm 0x5536AB (WORD)
        this_->production_queue_total_units = 0;            // asm 0x5536B2 (WORD)
        this_->production_queue_enabled = 1;                // asm 0x5536B9
        this_->production_queue_actions = 0;                // asm 0x5536BF
        this_->production_queue_change_flag = 0;            // asm 0x5536C5
        this_->production_queue_auto_paused = 0;            // asm 0x5536CB
        this_->production_queue_need_attr = -1;             // asm 0x5536D1 (WORD)
        this_->relic_count = 0;                             // asm 0x5536D8
        this_->specific_relic_count = 0;                    // asm 0x5536DE
        this_->raider_transform_count = 0;                  // asm 0x5536E4
        this_->wonder_node = 0;                             // asm 0x5536EA-region
        this_->default_frame = 0;                           // asm 0x5536F0
        this_->desolid_flag = 0;                            // asm 0x5536F6
        this_->pending_order = 0;                           // asm 0x5536FC
        this_->snow_flag = 0;                               // asm 0x553702
        this_->mEndpoint1.x = 0.0f;                         // asm 0x553708-0x553726
        this_->mEndpoint1.y = 0.0f;
        this_->mEndpoint1.z = 0.0f;
        this_->mEndpoint2.x = 0.0f;
        this_->mEndpoint2.y = 0.0f;
        this_->mEndpoint2.z = 0.0f;
        this_->mGateLocked = 0;                             // asm 0x553734
        this_->mCloseTimer = 0.0f;                          // asm 0x55373A
        this_->building_terrain_type = 0xFF;                // asm 0x553740
        this_->semi_asleep = 0;                             // asm 0x553747
        this_->DoppleInstalled = 0;                         // asm 0x55374D-region
        this_->vfptr = rge_tribe_building_object_vftable;   // asm 0x553753
        for (size_t i = 0; i < _countof(this_->linked_child); ++i)   // asm 0x553759-0x55376F
            this_->linked_child[i] = 0;
        this_->gather_point_target = -1;                    // asm 0x553775
        this_->gather_point_target_master = -1;             // asm 0x55377B (WORD)
        this_->gather_point_exists = 0;                     // asm 0x553788 (@+0x214)
        this_->gather_point_x = 0;                          // asm 0x55379A (@+0x218)
        this_->gather_point_y = 0;                          // asm 0x553794 (@+0x21C)
        this_->gather_point_z = 0;                          // asm 0x55378E (@+0x220)
        if (do_setup != 0)
            TRIBE_Building_Object__setup_read(this_, infile, gworld);   // asm 0x5537A6 (:3916)
        TRIBE_Master_Building_Object* master = this_->master_obj;
        __int16 object_group = master->object_group;        // asm 0x5537AB-0x5537AE
        if (object_group != 34 && object_group != 36        // asm 0x5537B2-0x5537BC
            && this_->desolid_flag == 0                     // asm 0x5537BE-0x5537C4
            && (master->object_flags & 4) == 0              // asm 0x5537C6-0x5537CD
            && (this_->linked_owner == 0 || object_group == 8))   // asm 0x5537CF-0x5537DB
        {
            // MAP_SIZE: mapsize.cpp:1751 re-points the tile-table disp32
            // (0x5537F2) at mapptr when setMapSizeHooks runs. NOT baked
            // (owner-directed MAP_SIZE rule): the replacement keeps the
            // original's unified_map_offsets base.
            this_->DoppleInstalled = RGE_Doppleganger_Creator__add_doppleganger_check(
                this_->owner->doppleganger_creator,         // asm 0x5537F6-0x553804 (@+0x90)
                (RGE_Static_Object*)this_,
                (unsigned int*)((int)unified_map_offsets[(int)this_->world_y]   // MAP_SIZE - asm 0x5537DD-0x5537FF
                    + 4 * (int)this_->world_x));
        }
        result = this_;
    }
    __except (TRIBE_Combat_Object__destructor((TRIBE_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x644930
    {
    }
    return result;
}

// Replacement for TRIBE_Building_Object::destructor. Original:
// @ 0x00553830 (idb_functions.h:3913). Range [0x553830, 0x553A92)
// (lst :566806/:567042), void, no stack args. ABI: __thiscall. Caller:
// the scalar deleting destructor above (lst :566807).
// Decode (asm 0x553830-0x553A92): vfptr store; the doppleganger
// de-registration (remove_doppleganger_check :1493 on owner->
// doppleganger_creator @+0x90, asm 0x553857-0x553867); IF semi_asleep
// @+0x265: RemoveObject (:1125) on owner->visible_object_updator
// @+0x94 and semi_asleep = 0 (asm 0x55386C-0x553885); the linked_child
// sweep (each non-null child's linked_owner @+0x1DC = 0, asm
// 0x55388B-0x5538A6); the linked-owner back-link removal (search
// linked_owner->linked_child[0..3] for this_, null the slot, asm
// 0x5538A8-0x5538C9); the three action-list deleting destructors
// (extra_actions @+0x1D0, research_actions @+0x1D4,
// production_queue_actions @+0x200 - RGE_Action_List vtbl slot 0,
// each then zeroed, asm 0x5538D0-0x553913); empty_production_queue
// (:3957, unhooked); the production_queue free (asm 0x553924 - the
// game's own free; free_internal keeps the game-heap pairing, CRT
// rule) and production_queue = 0; the wonder-node removal (owner->
// world @+0x8C -> wonders @+0x2C0; find_by_id (:2288) == this ->
// remove_node (:2287); wonder_node = 0, asm 0x553932-0x553970); the
// Farm terrain-decay gate (see the FARM_DEAD bake-in below);
// the mROO/tile/linked-owner clear_map_area_reference block (asm
// 0x5539DB-0x553A4A); the snow-sprite removal (IF snow_flag @+0x23C
// == 1: remove_sprite (:564) of master->snow_sprite @+0x1D0 = 464,
// snow_flag = 0, asm 0x553A50-0x553A6B); the runtime Combat
// destructor (HOOKED -> routes).
// SEH fold: chunk 0x644950 -> TRIBE_Combat_Object__destructor (HOOKED,
// routes) - the throw-during-destruction unwind runs the FULL base
// destructor, immediate base level.
void __fastcall TRIBE_Building_Object__destructor_new(TRIBE_Building_Object* this_, DWORD dummy)
{
    __try
    {
        this_->vfptr = rge_tribe_building_object_vftable;    // asm 0x553851
        RGE_Doppleganger_Creator__remove_doppleganger_check(
            this_->owner->doppleganger_creator,              // asm 0x553857-0x553861 (@+0x90)
            (RGE_Static_Object*)this_);
        if (this_->semi_asleep != 0)                         // asm 0x55386C-0x553874 (byte @+0x265)
        {
            RGE_Visible_Object_Updator__RemoveObject(        // :1125
                this_->owner->visible_object_updator,        // asm 0x553876-0x55387A (@+0x94)
                (RGE_Combat_Object*)this_);
            this_->semi_asleep = 0;                          // asm 0x553885
        }
        for (size_t i = 0; i < _countof(this_->linked_child); ++i)   // asm 0x55388B-0x5538A6
        {
            if (this_->linked_child[i] != 0)                 // asm 0x553896-0x55389A
                this_->linked_child[i]->linked_owner = 0;    // asm 0x55389C (child @+0x1DC)
        }
        if (this_->linked_owner != 0)                        // asm 0x5538A8-0x5538B0 (@+0x1DC)
        {
            size_t index = 0;                                // asm 0x5538B2-0x5538C5
            while (index < _countof(this_->linked_owner->linked_child)
                && this_->linked_owner->linked_child[index] != this_)
                ++index;
            if (index < _countof(this_->linked_owner->linked_child))
                this_->linked_owner->linked_child[index] = 0;   // asm 0x5538C9
        }
        if (this_->extra_actions != 0)                       // asm 0x5538D0-0x5538D8 (@+0x1D0)
        {
            this_->extra_actions->vfptr->__vecDelDtor(this_->extra_actions, 1);   // asm 0x5538DA-0x5538DE (slot 0)
            this_->extra_actions = 0;                        // asm 0x5538E0
        }
        if (this_->research_actions != 0)                    // asm 0x5538E6-0x5538EE (@+0x1D4)
        {
            this_->research_actions->vfptr->__vecDelDtor(this_->research_actions, 1);   // asm 0x5538F0-0x5538F4
            this_->research_actions = 0;                     // asm 0x5538F6
        }
        TRIBE_Building_Object__empty_production_queue(this_);   // asm 0x5538FE (:3957, unhooked)
        if (this_->production_queue_actions != 0)            // asm 0x553903-0x55390B (@+0x200)
        {
            this_->production_queue_actions->vfptr->__vecDelDtor(this_->production_queue_actions, 1);   // asm 0x55390D-0x553911
            this_->production_queue_actions = 0;             // asm 0x553913
        }
        if (this_->production_queue != 0)                    // asm 0x553919-0x553921 (@+0x1F4)
        {
            free_internal(this_->production_queue);          // asm 0x553924 call _free (CRT rule: game-heap pairing)
            this_->production_queue = 0;                     // asm 0x55392C
        }
        if (this_->wonder_node != 0)                         // asm 0x553932-0x553938 (@+0x22C)
        {
            RGE_Object_List* wonders = this_->owner->world->wonders;   // asm 0x55393A-0x55393D (world @+0x8C, wonders @+0x2C0)
            if (wonders != 0
                && RGE_Object_List__find_by_id(wonders, this_->id)   // asm 0x55394D-0x553956 (:2288, id @+0x10)
                    == (RGE_Static_Object*)this_)                // asm 0x553956-0x553958
                RGE_Object_List__remove_node(wonders, (RGE_Static_Object*)this_, 0);   // asm 0x55395A-0x55396B (:2287)
            this_->wonder_node = 0;                          // asm 0x553970-0x553938
        }
        __int16 master_id = this_->master_obj->id;           // asm 0x553976-0x553979 (word @+0x18)
        TRIBE_World* world = this_->owner->world;            // asm @+0x8C (game_state @+0x29, map @+0x34)
        int do_change = 0;
        if (master_id == 357)                                // asm 0x55397D (0x165 = Depleted Farm)
            do_change = (world->game_state == 0);            // asm 0x553947-0x553991 (byte @+0x29)
        else if (master_id == 50 && world->game_state == 0)  // asm 0x553993-0x5539A7 (0x32 = Farm)
        {
            unsigned __int8 terrain = RGE_Map__get_terrain(  // asm 0x5539A9-0x5539BE (:1994; map @+0x34)
                world->map, (int)this_->world_x, (int)this_->world_y);   // ftol; push order mapcol=x, maprow=y
            do_change = (terrain != 8 && terrain != 31
                && terrain != 228);                          // FARM_DEAD: baked from terrain_gen.cpp:271
                                                             // (onFarmDead3 hooked @0x5539C8 - extends the
                                                             // skip set with terrain 228); patch site retired
        }
        if (do_change != 0)
            TRIBE_Building_Object__change_terrain(this_, 27);// asm 0x5539D2-0x5539D6 (:3961, 0x1B)
        if (this_->mROOAlreadyCalled == 0 && this_->tile != 0 && this_->linked_owner == 0)   // asm 0x5539DB-0x5539F0
        {
            TRIBE_Master_Building_Object* master = this_->master_obj;   // asm 0x5539F2
            RGE_Map__clear_map_area_reference(               // :1954
                this_->owner->world->map,                    // asm 0x553A3F-0x553A47 (world @+0x8C, map @+0x34)
                (int)(this_->world_x - master->radius_x),    // asm 0x5539F6-0x5539FC (radius_x @+0x3C)
                (int)(this_->world_y - master->radius_y),    // asm 0x553A01-0x553A09 (radius_y @+0x40)
                (int)(master->radius_x + this_->world_x - 0.01f),   // asm 0x553A0E-0x553A1C (flt_654978 = float32 0.01)
                (int)(master->radius_y + this_->world_y - 0.01f));  // asm 0x553A21-0x553A31
        }
        if (this_->snow_flag == 1)                           // asm 0x553A50-0x553A57 (byte @+0x23C)
        {
            RGE_Active_Sprite_List__remove_sprite(           // :564
                this_->sprite_list,                          // asm 0x553A62 (@+0x24)
                this_->master_obj->snow_sprite);             // asm 0x553A59-0x553A5C (master @+0x1D0 = 464)
            this_->snow_flag = 0;                            // asm 0x553A6B
        }
        RGE_Combat_Object__destructor((RGE_Combat_Object*)this_);   // asm 0x553A7C - HOOKED, routes
    }
    __except (TRIBE_Combat_Object__destructor((TRIBE_Combat_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x644950
    {
    }
}

// Replacement for TRIBE_Building_Object::recycle_out_of_game. Original:
// @ 0x00553AA0 (idb_functions.h:3914). Range [0x553AA0, 0x553C37)
// (lst :567051/:567182), void, no stack args. ABI: __thiscall. Virtual:
// vftable slot (lst :567052).
// Decode (asm 0x553AA0-0x553C37): the three action lists go through
// delete_list (:62) here - NOT the deleting destructor (vs the
// destructor's vecDelDtor form): extra_actions @+0x1D0, research_
// actions @+0x1D4, then empty_production_queue (:3957), then
// production_queue_actions @+0x200; the production_queue free + the
// queue-state reset block (size/count/total_units words = 0, enabled
// = 1, change_flag/auto_paused = 0, need_attr word = -1, asm
// 0x553AE4-0x553B1B); the wonder-node removal (same as the destructor,
// asm 0x553B24-0x553B62); the Farm terrain-decay gate (FARM_DEAD
// bake-in below, asm 0x553B68-0x553BC9); the endpoint/gate resets
// (mEndpoint1/2 = 0, mGateLocked = 0, mFirstUpdate = 1, mCloseTimer =
// 0, building_terrain_type = 0xFF - with the snow_flag read saved
// BEFORE the resets, asm 0x553BEE); the snow-sprite removal; then the
// runtime Action recycle (:122 - HOOKED -> routes; the main `actions`
// list cleanup comes from that hook). No SEH.
void __fastcall TRIBE_Building_Object__recycle_out_of_game_new(TRIBE_Building_Object* this_, DWORD dummy)
{
    if (this_->extra_actions != 0)                       // asm 0x553AA6-0x553AAE (@+0x1D0)
        RGE_Action_List__delete_list(this_->extra_actions);           // asm 0x553AB0 (:62)
    if (this_->research_actions != 0)                    // asm 0x553AB5-0x553ABD (@+0x1D4)
        RGE_Action_List__delete_list(this_->research_actions);        // asm 0x553ABF (:62)
    TRIBE_Building_Object__empty_production_queue(this_);             // asm 0x553AC6 (:3957, unhooked)
    if (this_->production_queue_actions != 0)            // asm 0x553ACB-0x553AD3 (@+0x200)
        RGE_Action_List__delete_list(this_->production_queue_actions);// asm 0x553AD5 (:62)
    if (this_->production_queue != 0)                    // asm 0x553ADA-0x553AE2 (@+0x1F4)
    {
        free_internal(this_->production_queue);          // asm 0x553AE5 (CRT rule: game-heap pairing)
        this_->production_queue = 0;                     // asm 0x553AED
        this_->production_queue_size = 0;                // asm 0x553AF3 (WORD @+0x1F8)
        this_->production_queue_count = 0;               // asm 0x553AFA (WORD @+0x1FA)
        this_->production_queue_total_units = 0;         // asm 0x553B01 (WORD @+0x1FC)
        this_->production_queue_enabled = 1;             // asm 0x553B08 (byte @+0x1FE)
        this_->production_queue_change_flag = 0;         // asm 0x553B0F (byte @+0x204)
        this_->production_queue_auto_paused = 0;         // asm 0x553B15 (byte @+0x205)
        this_->production_queue_need_attr = -1;          // asm 0x553B1B (WORD @+0x206)
    }
    if (this_->wonder_node != 0)                         // asm 0x553B24-0x553B2A (@+0x22C)
    {
        RGE_Object_List* wonders = this_->owner->world->wonders;   // asm 0x553B2C-0x553B2F
        if (wonders != 0
            && RGE_Object_List__find_by_id(wonders, this_->id)       // asm 0x553B3F-0x553B43 (:2288)
                == (RGE_Static_Object*)this_)                        // asm 0x553B48-0x553B4A
            RGE_Object_List__remove_node(wonders, (RGE_Static_Object*)this_, 0);   // asm 0x553B4C-0x553B5D (:2287)
        this_->wonder_node = 0;                          // asm 0x553B62-0x553B14
    }
    __int16 master_id = this_->master_obj->id;           // asm 0x553B68-0x553B6C (word @+0x18)
    TRIBE_World* world = this_->owner->world;            // asm @+0x8C
    int do_change = 0;
    if (master_id == 357)                                // asm 0x553B70 (Depleted Farm)
        do_change = (world->game_state == 0);            // asm 0x553B76-0x553B82 (byte @+0x29)
    else if (master_id == 50 && world->game_state == 0)  // asm 0x553B84-0x553B93 (Farm)
    {
        unsigned __int8 terrain = RGE_Map__get_terrain(  // asm 0x553B98-0x553BAD (:1994)
            world->map, (int)this_->world_x, (int)this_->world_y);
        do_change = (terrain != 8 && terrain != 31
            && terrain != 228);                          // FARM_DEAD: baked from terrain_gen.cpp:270
                                                         // (onFarmDead2 hooked @0x553BB7 - extends the
                                                         // skip set with terrain 228); patch site retired
    }
    if (do_change != 0)
        TRIBE_Building_Object__change_terrain(this_, 27);// asm 0x553BC1-0x553BC5 (:3961, 0x1B)
    this_->mEndpoint1.x = 0.0f;                          // asm 0x553BCA-0x553BE8 (@+0x240..0x254)
    this_->mEndpoint1.y = 0.0f;
    this_->mEndpoint1.z = 0.0f;
    this_->mEndpoint2.x = 0.0f;
    this_->mEndpoint2.y = 0.0f;
    this_->mEndpoint2.z = 0.0f;
    unsigned __int8 snow = this_->snow_flag;             // asm 0x553BEE (read BEFORE the resets, byte @+0x23C)
    this_->mGateLocked = 0;                              // asm 0x553BF4 (@+0x258)
    this_->mFirstUpdate = 1;                             // asm 0x553BFC (@+0x25C)
    this_->mCloseTimer = 0.0f;                           // asm 0x553C06 (@+0x260)
    this_->building_terrain_type = 0xFF;                 // asm 0x553C0C (@+0x264)
    if (snow == 1)                                       // asm 0x553BFA-0x553C14 (saved flags)
    {
        RGE_Active_Sprite_List__remove_sprite(           // :564
            this_->sprite_list,                          // asm 0x553C1F (@+0x24)
            this_->master_obj->snow_sprite);             // asm 0x553C16-0x553C19 (@+0x1D0)
        this_->snow_flag = 0;                            // asm 0x553C28
    }
    RGE_Action_Object__recycle_out_of_game((RGE_Action_Object*)this_);   // asm 0x553C30 (:122 - HOOKED, routes)
}

void install_TRIBE_Building_Object()
{
    setHook((void*)0x005531A0, TRIBE_Building_Object__TRIBE_Building_Object_new);
    setHook((void*)0x005533F0, TRIBE_Building_Object__scalar_deleting_destructor_new);
    setHook((void*)0x00553410, TRIBE_Building_Object__TRIBE_Building_Object2_new);
    setHook((void*)0x00553640, TRIBE_Building_Object__TRIBE_Building_Object3_new);
    setHook((void*)0x00553830, TRIBE_Building_Object__destructor_new);
    setHook((void*)0x00553AA0, TRIBE_Building_Object__recycle_out_of_game_new);
}
