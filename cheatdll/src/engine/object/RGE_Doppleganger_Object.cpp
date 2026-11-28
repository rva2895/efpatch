#include "stdafx.h"
#include "RGE_Master_Static_Object.h"

// ============================================================================
// RGE_Doppleganger_Object.cpp - the converted runtime Doppleganger tier (the
// ladder conversion, 2026-11-28; derives the converted RGE_Animated_Object).
// The 9 exports owned: the ctor pair (the placement-new chain), the 4 slot
// overrides (37/39/40/167), and the 3 appended virtuals (220/221/222).
// THE CONVERSION NOTES:
// - the ctor/ctor_read SEH folds (chunks 0x641CD0/0x641CF0 -> the Static
//   destructor) are subsumed by the compiler-generated ctor-unwind of the
//   C++ chain (the throwing base ctor destroys the base subobject - the
//   same destructor, the same game-heap pairing through the base tripwire
//   bodies).
// - the explicit vfptr stores (0x0065A2C0) are the compiler's implicit
//   vftable stores now.
// - THE HIDE-AND-APPEND CALLS: the base-Animated setup/recycle_in calls in
//   these bodies are the QUALIFIED `RGE_Animated_Object::` form - MANDATORY
//   because the Dopple's own setup (slot 222) and recycle_in_to_game (slot
//   220) HIDE the base virtuals (the different parameter lists); the
//   unqualified forms would resolve to the Dopple members (the infinite
//   recursion through the trampolines) or fail to bind the extra args.
// - the MAP_SIZE stores keep the original's unified_map_offsets base (the
//   owner-directed not-baked rule; the mapsize.cpp sites retired).
// ============================================================================

// .data:0077FA90 - the game's sound-suppression flag (lst :1105298;
// battlegrounds_x2.c:44143 `char temp_disable_sound_playing`)
static char* const temp_disable_sound_playing = (char*)0x0077FA90;

// ----------------------------------------------------------------------------
// RGE_Doppleganger_Object::RGE_Doppleganger_Object - the runtime ctor.
// Original: @ 0x0046FBF0 (idb_functions.h:1480). Range [0x46FBF0, 0x46FE66)
// (retn 1Ch = 7 stack args). Decode (asm 0x46FBF0-0x46FE66): the base
// Animated ctor with do_setup = 0 ALWAYS, the Doppleganger vftable store,
// then the UNCONDITIONAL dopple-extension zero block in asm order
// 0x46FC43-0x46FC89 (B8/BC/C0/C4/C8/CC/D0 dwords, D4/D5 bytes, D8 = -1
// dword, D6/D7 bytes); IF do_setup: the runtime Animated setup + type
// @+0x5E = 0x19 (the tier mirror) + LastMapValue/CantSeeBits zeroed again
// + the tile computation; doppled_object = obj_to_dopple unconditionally
// (asm 0x46FCE3); IF obj_to_dopple: the source-binding copies, the
// construction-sprite gate (the Building masters), the sound-suppressed
// sprite-list block, facet/animation-frame/draw-color copies, the gated
// Multi_Unified_Points copy and the source-died release; ELSE the
// no-source path releases the binding and self-destructs; validate() then
// return this.
// MAP_SIZE: mapsize.cpp:1739 re-points the tile-row table disp32 inside
// THIS function (0x46FCC4) at the big-map base (mapptr) when active; the
// ctor_read twin is mapsize.cpp:1740 @0x46FF43. NOT baked (the
// owner-directed MAP_SIZE rule 2026-11-15): the replacement keeps the
// original's unified_map_offsets base, so under the big-map mod the dopple
// tile intentionally diverges from the patched original.
// ----------------------------------------------------------------------------
RGE_Doppleganger_Object::RGE_Doppleganger_Object(RGE_Master_Doppleganger_Object* tobj, RGE_Player* obj_owner,
                                                 float x, float y, float z, int do_setup, RGE_Static_Object* obj_to_dopple)
    : RGE_Animated_Object((RGE_Master_Animated_Object*)tobj, (TRIBE_Player*)obj_owner, x, y, z, 0)   // asm (do_setup = 0 ALWAYS)
{
    doppled_object = 0;                                // asm 0x46FC43
    draw_color_owner = 0;                              // asm 0x46FC49
    doppled_master_obj = 0;                            // asm 0x46FC4F
    doppled_player = 0;                                // asm 0x46FC55
    MapAddress = 0;                                    // asm 0x46FC5B
    LastMapValue = 0;                                  // asm 0x46FC61
    CantSeeBits = 0;                                   // asm 0x46FC67
    map_draw_level = 0;                                // asm 0x46FC6D
    map_color = 0;                                     // asm 0x46FC73
    doppledObjectID = -1;                              // asm 0x46FC79
    Multi_Unified_Points = 0;                          // asm 0x46FC83
    Destroy_Next_Turn = 0;                             // asm 0x46FC89
    if (do_setup != 0)
    {
        RGE_Animated_Object::setup(
            (RGE_Master_Animated_Object*)tobj, obj_owner, x, y, z);
        type = RGE_MASTER_TYPE_DOPPELGANGER;                                     // asm 0x46FCAD (the tier mirror)
        LastMapValue = 0;                                  // asm 0x46FCC8
        CantSeeBits = 0;                                   // asm 0x46FCCE
        MapAddress = (unsigned int*)((int)unified_map_offsets[(int)y] + 4 * (int)x);   // MAP_SIZE - asm 0x46FCC1-0x46FCD7: ftol(y) row, + ftol(x)*4
    }
    doppled_object = obj_to_dopple;                                  // asm 0x46FCE3 (unconditional)
    if (obj_to_dopple != 0)
    {
        doppled_master_obj = obj_to_dopple->master_obj;              // asm 0x46FCEF-0x46FCF2
        doppled_player = obj_to_dopple->owner->id;                   // asm 0x46FCF8-0x46FD01 (id @0xA0 = 160)
        doppledObjectID = obj_to_dopple->id;                         // asm 0x46FD07-0x46FD0A
        sprite = obj_to_dopple->sprite;                              // asm 0x46FD10-0x46FD13
        RGE_Master_Static_Object* src_master = obj_to_dopple->master_obj;
        int swap_flag = 0;                                           // asm dl (xor dl, dl @0x46FD19)
        if (src_master->master_type == RGE_MASTER_TYPE_BUILDING &&                        // asm 0x46FD1B (byte @+4)
            src_master->object_group != 8 &&                              // asm 0x46FD21 (word @+0x1E)
            src_master->id != 335)                                        // asm 0x46FD28 (word @+0x18)
        {
            // The base-pointer rule: the pseudocode's v10[2].construction_radius_x
            // = 2*0xD0 + 0x6C = 0x20C = 524 -> the Building-tier construction_id
            // (TRIBE_Master_Building_Object, i16). The runtime-type evidence for
            // the base->derived cast: the master_type == BUILDING gate (the factory identity).
            __int16 construction_id =
                ((TRIBE_Master_Building_Object*)src_master)->construction_id;   // asm 0x46FD30
            if (construction_id > -1)                                         // asm 0x46FD37 (the signed word cmp)
            {
                RGE_Master_Static_Object* sprite_master =
                    obj_to_dopple->owner->master_objects[construction_id];      // asm 0x46FD3D-0x46FD47 (@0x74)
                sprite = sprite_master->vfptr->get_sprite(sprite_master, (RGE_Static_Object_idb*)obj_to_dopple);   // the MASTER vtbl slot +4 (asm 0x46FD4C)
                swap_flag = 1;                                                  // asm 0x46FD52
            }
        }
        char const old_sound_gate = *temp_disable_sound_playing;            // asm 0x46FD54
        *temp_disable_sound_playing = 1;                                    // asm 0x46FD59
        RGE_Active_Sprite_List* src_list = doppled_object->sprite_list;     // asm 0x46FD60-0x46FD6A (@+0x24)
        if (src_list == 0 || swap_flag != 0)                                // asm 0x46FD6D-0x46FD73
        {
            RGE_Active_Sprite_List__delete_list(sprite_list);               // asm 0x46FD85-0x46FD88 (NO null guard - faithful)
            if (sprite != 0)                                                // asm 0x46FD8D-0x46FD92
                RGE_Active_Sprite_List__add_sprite(sprite_list, sprite, 0, 0, 0, 0);   // asm 0x46FD94-0x46FD9C
        }
        else
        {
            RGE_Active_Sprite_Node* temp_list = RGE_Active_Sprite_List__copy_sprite_list(src_list);   // asm 0x46FD75
            RGE_Active_Sprite_List__use_sprite_list(sprite_list, temp_list);   // asm 0x46FD7A-0x46FD7E
        }
        *temp_disable_sound_playing = old_sound_gate;                       // asm 0x46FDA1-0x46FDA5
        facet = doppled_object->facet;                                      // asm 0x46FDB1-0x46FDB4 (byte @0x45; the decode-state correction: facet, not "selected" - selected @0x46)
        __int16 frame = RGE_Static_Object__get_animation_frame(doppled_object);   // asm 0x46FDB7 (:3853)
        RGE_Static_Object::set_animation_frame(frame);                      // asm 0x46FDBC-0x46FDBF (:3852)
        draw_color_owner = (RGE_Player*)doppled_object->owner;              // asm 0x46FDC4-0x46FDCD (owner @0x18; the TRIBE_Player* -> RGE_Player* dual-naming cast)
        map_draw_level = doppled_object->master_obj->map_draw_level;        // asm 0x46FDD3-0x46FDDF (@0x9E = 158)
        if (doppled_object->master_obj->map_draw_level == 3)                // asm 0x46FDDC
            map_color = doppled_object->master_obj->map_color;              // asm 0x46FDE7-0x46FDF0 (@0xA0 = 160)
        else
            map_color = 0;                                                  // asm 0x46FDF8
        if (doppled_master_obj->master_type >= RGE_MASTER_TYPE_COMBAT_BASE)          // asm 0x46FE04 (byte @+4, jb skips)
            // The base-pointer rule: the pseudocode's v20[2].gbg_civ_override
            // = 2*0xB4 + 0x4 = 0x16C = 364 -> RGE_Combat_Object::
            // Multi_Unified_Points (u8 @364; present in every Combat-tier
            // runtime struct prefix). The runtime-type evidence: the
            // master_type >= COMBAT_BASE gate (the factory identity).
            Multi_Unified_Points =
                ((RGE_Combat_Object*)doppled_object)->Multi_Unified_Points;   // asm 0x46FE0A
        if (doppled_object->object_state >= 7)                              // asm 0x46FE16 (byte @+0x58)
        {
            doppled_object = 0;                                             // asm 0x46FE1C (the source-died release)
            doppledObjectID = -1;                                           // asm 0x46FE22
        }
    }
    else
    {
        doppledObjectID = -1;                                               // asm 0x46FE30
        doppled_master_obj = 0;                                             // asm 0x46FE3A
        doppled_player = 0;                                                 // asm 0x46FE40
        destroy_obj();                                                      // asm 0x46FE46 (:3843 - SELF-DESTRUCT; slot 56)
    }
    validate();                                                             // asm 0x46FE4D (:1489; slot 221)
}

// ----------------------------------------------------------------------------
// RGE_Doppleganger_Object::RGE_Doppleganger_Object - the stream ctor
// overload. Original: @ 0x0046FE90 (idb_functions.h:1482). Range
// [0x46FE90, 0x46FFF4) (retn 0Ch = 3 stack args). Decode (asm
// 0x46FE90-0x46FFF4): the base Animated ctor_read with do_setup = 0 (the
// idb constant :546 - NOT hooked, executes the original), then the SAME
// zero block MINUS draw_color_owner (asm 0x46FED8-0x46FF15 has no BC
// store), then IF do_setup: the Animated setup_read (:549), type = 25,
// LastMapValue/CantSeeBits zeroed again, and the tile computation from the
// LOADED world_y (@+0x4C) / world_x (@+0x48). THEN the unconditional
// rge_read stream (:3003, 0x004D5550): (4) -> doppled_object;
// doppledObjectID = -1 (asm 0x46FF82); (1) -> map_draw_level; (4) ->
// &map_color (the DELIBERATE SPAN - see the call); (4) ->
// doppled_master_obj; (4) -> doppled_player; (4) -> CantSeeBits; (1) ->
// Multi_Unified_Points.
// MAP_SIZE: mapsize.cpp:1740 re-points the tile-row table disp32 inside
// THIS function (0x46FF43) at the big-map base (mapptr) when active. NOT
// baked (the owner-directed MAP_SIZE rule 2026-11-15): the replacement
// keeps the original's unified_map_offsets base.
// ----------------------------------------------------------------------------
RGE_Doppleganger_Object::RGE_Doppleganger_Object(int infile, RGE_Game_World* gworld, int do_setup)
    : RGE_Animated_Object(infile, gworld, 0)                           // the base read (do_setup = 0 ALWAYS)
{
    int read_value;   // the asm's infile-arg-slot reuse as a 4-byte buffer
    doppled_object = 0;                                // asm 0x46FED8
    doppled_master_obj = 0;                            // asm 0x46FEDE
    doppled_player = 0;                                // asm 0x46FEE4
    MapAddress = 0;                                    // asm 0x46FEEA
    LastMapValue = 0;                                  // asm 0x46FEF0
    CantSeeBits = 0;                                   // asm 0x46FEF6
    map_draw_level = 0;                                // asm 0x46FEF9
    map_color = 0;                                     // asm 0x46FEFF
    doppledObjectID = -1;                              // asm 0x46FF05
    Multi_Unified_Points = 0;                          // asm 0x46FF0F
    Destroy_Next_Turn = 0;                             // asm 0x46FF15
    if (do_setup != 0)
    {
        RGE_Animated_Object::setup_read(infile, gworld);
        type = RGE_MASTER_TYPE_DOPPELGANGER;                                     // asm 0x46FF2D (the tier mirror)
        LastMapValue = 0;                                  // asm 0x46FF47
        CantSeeBits = 0;                                   // asm 0x46FF51
        MapAddress = (unsigned int*)((int)unified_map_offsets[(int)world_y]
            + 4 * (int)world_x);                              // MAP_SIZE - asm 0x46FF35-0x46FF5B: ftol(world_y @0x4C) row, + ftol(world_x @0x48)*4
    }
    rge_read(infile, &read_value, sizeof(read_value));                    // asm 0x46FF61-0x46FF69 (4)
    doppled_object = (RGE_Static_Object *)read_value;                     // asm 0x46FF74
    doppledObjectID = -1;                                                 // asm 0x46FF82
    rge_read(infile, &map_draw_level, sizeof(map_draw_level));            // asm 0x46FF8C (1)
    // DEVIATION (the deliberate span, decode-record approved): the original
    // streams 4 bytes at &map_color covering map_color @0xD5 +
    // Multi_Unified_Points @0xD6 + Destroy_Next_Turn @0xD7 + the
    // doppledObjectID low byte @0xD8 - asm 0x46FF91-0x46FF9B (lea edx,
    // [esi+0D5h]; push 4). doppledObjectID reads 0xFFFFFFxx after this
    // call - faithful; sizeof(&map_color) would be 1.
    rge_read(infile, &map_color, 4);
    rge_read(infile, &read_value, sizeof(read_value));                    // asm 0x46FFA0-0x46FFA8 (4)
    doppled_master_obj = (RGE_Master_Static_Object*)read_value;           // asm 0x46FFB3
    rge_read(infile, &doppled_player, sizeof(doppled_player));            // asm 0x46FFC1 (4)
    rge_read(infile, &CantSeeBits, sizeof(CantSeeBits));                  // asm 0x46FFCC (4)
    rge_read(infile, &Multi_Unified_Points, sizeof(Multi_Unified_Points));   // asm 0x46FFDB (1)
}

// ============ RUNTIME METHODS (batch M1) ============
// The temp_disable_sound_playing cpp-local binding (0x77FA90) from the
// ctor above is reused by recycle_in_to_game below.

// ----------------------------------------------------------------------------
// RGE_Doppleganger_Object::setup - the appended slot-222 virtual (hides the
// Animated's slot-219 setup; the tobj type differs). Original: @ 0x00470230
// (idb_functions.h). Range [0x470230, 0x4702A0), returns int (1). Decode
// (.c:133745-133753): the Animated setup + type = 25 + the MAP_SIZE tile
// store + LastMapValue/CantSeeBits = 0.
// ----------------------------------------------------------------------------
int RGE_Doppleganger_Object::setup(RGE_Master_Doppleganger_Object* tobj, RGE_Player* obj_owner,
                                   float x, float y, float z)
{
    RGE_Animated_Object::setup((RGE_Master_Animated_Object*)tobj, obj_owner, x, y, z);   // .c:133747 (the QUALIFIED base call - the base virtual is hidden)
    type = RGE_MASTER_TYPE_DOPPELGANGER;                                // .c:133748
    // MAP_SIZE: mapsize.cpp:1748 re-points this disp32 (0x47026F) at
    // mapptr. NOT baked (the owner-directed MAP_SIZE rule).
    MapAddress = (unsigned int*)((int)unified_map_offsets[(int)y] + 4 * (int)x);   // .c:133749
    LastMapValue = 0;                         // .c:133750
    CantSeeBits = 0;                          // .c:133751
    return 1;
}

// ----------------------------------------------------------------------------
// RGE_Doppleganger_Object::setup_read - the slot-167 override. Original:
// @ 0x004702A0 (idb_functions.h). Range [0x4702A0, 0x470300), returns
// int (1). Decode (.c:133756-133767): the Animated setup_read + type = 25 +
// the MAP_SIZE tile store from the LOADED world_y/world_x +
// LastMapValue/CantSeeBits = 0.
// ----------------------------------------------------------------------------
int RGE_Doppleganger_Object::setup_read(int infile, RGE_Game_World* gworld)
{
    RGE_Animated_Object::setup_read(infile, gworld);                   // .c:133760
    type = RGE_MASTER_TYPE_DOPPELGANGER;                               // .c:133762
    // MAP_SIZE: mapsize.cpp:1749 (the disp32 0x4702CE). NOT baked.
    MapAddress = (unsigned int*)((int)unified_map_offsets[(int)world_y]
        + 4 * (int)world_x);                                           // .c:133763
    LastMapValue = 0;                         // .c:133764
    CantSeeBits = 0;                          // .c:133765
    return 1;
}

// ----------------------------------------------------------------------------
// RGE_Doppleganger_Object::recycle_in_to_game - the appended slot-220
// virtual (the 7-arg form; hides the Animated's 5-arg slot-29 virtual).
// Original: @ 0x00470000. Range [0x470000, 0x470230). Decode
// (.c:133634-133741): the zero block, the Animated recycle, type = 25,
// doppled_object = obj_to_dopple, the source-binding block, the
// state>=7 validate-return path.
// MAP_SIZE: mapsize.cpp:1747 (the disp32 0x4701A7). NOT baked.
// ----------------------------------------------------------------------------
void RGE_Doppleganger_Object::recycle_in_to_game(RGE_Master_Static_Object* tobj, RGE_Player* obj_owner,
                                                 float x, float y, float z, RGE_Static_Object* obj_to_dopple)
{
    doppled_object = 0;                       // .c:133654
    doppled_master_obj = 0;                   // .c:133655
    doppled_player = 0;                       // .c:133656
    MapAddress = 0;                           // .c:133657
    LastMapValue = 0;                         // .c:133658
    CantSeeBits = 0;                          // .c:133659
    map_draw_level = 0;                       // .c:133660
    map_color = 0;                            // .c:133661
    doppledObjectID = -1;                     // .c:133662
    Multi_Unified_Points = 0;                 // .c:133663
    Destroy_Next_Turn = 0;                    // .c:133664
    RGE_Animated_Object::recycle_in_to_game(tobj, obj_owner, x, y, z);   // .c:133665 (the QUALIFIED base call - the 5-arg base virtual is hidden)
    type = RGE_MASTER_TYPE_DOPPELGANGER;      // .c:133666
    doppled_object = obj_to_dopple;           // .c:133667
    if (obj_to_dopple != 0)                   // .c:133668
    {
        doppled_master_obj = obj_to_dopple->master_obj;        // .c:133670
        doppled_player = obj_to_dopple->owner->id;             // .c:133671
        doppledObjectID = obj_to_dopple->id;                   // .c:133672
        sprite = obj_to_dopple->sprite;                        // .c:133673
        RGE_Master_Static_Object* master = obj_to_dopple->master_obj; // .c:133674
        int installed = 0;                                     // .c:133675 (v9)
        if (master->master_type == RGE_MASTER_TYPE_BUILDING && master->object_group != 8 && master->id != 335)   // .c:133676
        {
            __int16 construction_id = ((TRIBE_Master_Building_Object*)master)->construction_id;   // .c:133678 (+0x20C render)
            if (construction_id > -1)                          // .c:133679
            {
                RGE_Master_Static_Object* sprite_master =
                    obj_to_dopple->owner->master_objects[construction_id];   // .c:133681
                sprite = sprite_master->vfptr->get_sprite(sprite_master, (RGE_Static_Object_idb*)obj_to_dopple);   // .c:133682
                installed = 1;                                 // .c:133683
            }
        }
        char old_sound = *temp_disable_sound_playing;             // .c:133686
        *temp_disable_sound_playing = 1;                          // .c:133687
        RGE_Active_Sprite_List* src_list = doppled_object->sprite_list;   // .c:133689
        if (src_list == 0 || installed != 0)                      // .c:133690
        {
            RGE_Active_Sprite_List__delete_list(sprite_list);      // .c:133692
            if (sprite != 0)                                      // .c:133693
                RGE_Active_Sprite_List__add_sprite(sprite_list, sprite, 0, 0, 0, 0);   // .c:133695
        }
        else
        {
            RGE_Active_Sprite_Node* temp_list = RGE_Active_Sprite_List__copy_sprite_list(src_list);   // .c:133699
            RGE_Active_Sprite_List__use_sprite_list(sprite_list, temp_list);   // .c:133700
        }
        *temp_disable_sound_playing = old_sound;                  // .c:133702
        facet = doppled_object->facet;                            // .c:133704
        __int16 frame = RGE_Static_Object__get_animation_frame(doppled_object);   // .c:133705
        RGE_Static_Object::set_animation_frame(frame);            // .c:133706
        draw_color_owner = (RGE_Player*)doppled_object->owner;    // .c:133708 (the dual-naming cast)
        map_draw_level = doppled_object->master_obj->map_draw_level;   // .c:133709
        if (doppled_object->master_obj->map_draw_level == 3)      // .c:133711
            map_color = doppled_object->master_obj->map_color;    // .c:133712
        else
            map_color = 0;                                        // .c:133714
        // MAP_SIZE: mapsize.cpp:1747 (the disp32 0x4701A7). NOT baked.
        MapAddress = (unsigned int*)((int)unified_map_offsets[(int)y] + 4 * (int)x);   // .c:133715-133720
        LastMapValue = 0;                                  // .c:133718
        CantSeeBits = 0;                                   // .c:133719
        if (doppled_master_obj->master_type >= RGE_MASTER_TYPE_COMBAT_BASE)         // .c:133721
            Multi_Unified_Points =
                ((RGE_Combat_Object*)doppled_object)->Multi_Unified_Points;   // .c:133722
        if (doppled_object->object_state >= 7)             // .c:133723
        {
            doppled_object = 0;                            // .c:133726
            doppledObjectID = -1;                          // .c:133727
            validate();                                    // .c:133728 (slot 221)
            return;                                        // .c:133729
        }
    }
    else
    {
        doppledObjectID = -1;                              // .c:133735
        doppled_master_obj = 0;                            // .c:133736
        doppled_player = 0;                                // .c:133737
        destroy_obj();                                     // .c:133738 (slot 56)
    }
    validate();                                            // .c:133740 (slot 221)
}

// ----------------------------------------------------------------------------
// RGE_Doppleganger_Object::update - the slot-37 override. Original:
// @ 0x004703B0 (idb_functions.h). Range [0x4703B0, 0x470530), u8 return.
// Decode (.c:133815-133904): the Animated update; the Destroy_Next_Turn
// machinery; the doppled_object state release (the RESTRUCTURED
// truth-table-equivalent disjunctive guard - the original's goto done =
// the state-7 release falling into the shared ID=-1 store); the UMV
// visibility machinery (the MapAddress snapshot, the Multi_Unified_Points
// loop over map->UMV_Data_Points, the LastMapValue compare, the
// mutualVisibleMask gates incl. the doppled->Unified_Map_Value read
// @+0x168, the RemoveVisibleBits -> CantSeeBits accumulation).
// ----------------------------------------------------------------------------
unsigned __int8 RGE_Doppleganger_Object::update()
{
    unsigned __int8 result = RGE_Animated_Object::update();            // .c:133833
    if (Destroy_Next_Turn != 0 && object_state < 3)                    // .c:133835
    {
        destroy_obj();                                                 // .c:133837 (slot 56)
        return result;                                                 // .c:133838
    }
    if (doppled_object != 0 && doppled_object->object_state >= 7)      // .c:133840-133844 (the goto restructured)
    {
        doppled_object = 0;                                            // .c:133845
        doppledObjectID = -1;                                          // .c:133847
    }
    unsigned int map_value = *MapAddress;                              // .c:133849
    if (Multi_Unified_Points != 0)                                     // .c:133850
    {
        UMV_Offset_Point* points =
            owner->world->map->UMV_Data_Points[Multi_Unified_Points - 1];   // .c:133853
        int xoff = points->Xoffset;                                    // .c:133854 (the count lives in the first point's Xoffset)
        for (int k = 0; k < xoff; ++k)                                 // .c:133855-133868 (the indexed form)
        {
            int row = (int)world_y + points[1 + k].Yoffset;            // .c:133861
            int col = (int)world_x + points[1 + k].Xoffset;            // .c:133862
            map_value |= (unsigned int)unified_map_offsets[row][col];  // .c:133864 (the 2D indexed form)
        }
    }
    if (map_value != LastMapValue)                                     // .c:133870
    {
        unsigned int mutual = owner->mutualVisibleMask;                // .c:133872
        if ((mutual & map_value) != 0 && object_state < 3)             // .c:133873
        {
            if (Multi_Unified_Points != 0)                             // .c:133875
            {
                if (doppled_object != 0)                               // .c:133877
                {
                    if ((mutual & ((RGE_Combat_Object*)doppled_object)->Unified_Map_Value) == 0   // .c:133880 (+0x168 = Unified_Map_Value)
                        && (mutual & *MapAddress) == 0)                // .c:133880
                        Destroy_Next_Turn = 1;                         // .c:133881
                }
            }
            if (Destroy_Next_Turn == 0)                                // .c:133884
                destroy_obj();                                         // .c:133885 (slot 56)
        }
        unsigned int non_mutual = map_value & owner->nonMutualVisibleMask;   // .c:133888
        if (non_mutual != 0)                                           // .c:133889
        {
            unsigned int* bits = owner->RemoveVisibleBits;             // .c:133891
            do
            {
                if ((non_mutual & 1) == 1)                             // .c:133894
                    CantSeeBits |= *bits;                              // .c:133895
                ++bits;                                                // .c:133896
                non_mutual >>= 1;                                      // .c:133897
            }
            while (non_mutual != 0);                                   // .c:133898
        }
        LastMapValue = map_value;                                      // .c:133901
    }
    return result;
}

// ----------------------------------------------------------------------------
// RGE_Doppleganger_Object::save - the slot-40 override. Original:
// @ 0x00470530 (idb_functions.h). Range [0x470530, 0x4705F0). Decode
// (.c:133907-133932): the Animated save + doppledObjectID-or-(?-1 ?
// doppled_player) w4 + map_draw_level b1 + map_color w4 (the deliberate
// 4-byte span - the SAME span shape as the ctor_read; faithful) +
// doppled_master_obj id-or-(?-1) w4 + doppled_player w4 + CantSeeBits w4 +
// Multi_Unified_Points b1.
// ----------------------------------------------------------------------------
void RGE_Doppleganger_Object::save(int outfile)
{
    RGE_Animated_Object::save(outfile);                                // .c:133914
    int id_out;
    if (doppled_object != 0)                                           // .c:133916
        id_out = doppled_object->id;                                   // .c:133917
    else
        id_out = -1 - doppled_player;                                  // .c:133919
    rge_write(outfile, &id_out, sizeof(id_out));                       // .c:133920 (4)
    rge_write(outfile, &map_draw_level, sizeof(map_draw_level));       // .c:133921 (1)
    // THE DELIBERATE 4-BYTE SPAN over map_color @+0xD5 + Multi_Unified_Points
    // @+0xD6 + Destroy_Next_Turn @+0xD7 + the doppledObjectID low byte
    // @+0xD8 - the same span shape as the ctor_read; faithful (.c:133922).
    rge_write(outfile, &map_color, 4);
    if (doppled_master_obj != 0)                                       // .c:133924
        id_out = doppled_master_obj->id;                               // .c:133925
    else
        id_out = -1;                                                   // .c:133927
    rge_write(outfile, &id_out, sizeof(id_out));                       // .c:133928 (4)
    rge_write(outfile, &doppled_player, sizeof(doppled_player));       // .c:133929 (4)
    rge_write(outfile, &CantSeeBits, sizeof(CantSeeBits));             // .c:133930 (4)
    rge_write(outfile, &Multi_Unified_Points, sizeof(Multi_Unified_Points));   // .c:133931 (1)
}

// ----------------------------------------------------------------------------
// RGE_Doppleganger_Object::validate - the appended slot-221 virtual (the
// brand-new virtual; no base counterpart). Original: @ 0x004705F0
// (idb_functions.h). Range [0x4705F0, 0x4706D0). Decode
// (.c:133935-133980): the tile-walk dedup - map_row_offset[wy][wx], the
// TileObject_Count/Object_List walk; a found Doppleganger (type == 25)
// with a different id, matching doppledObjectID (the v5[1].sprite_list
// render = doppledObjectID @216 via the base-pointer rule) and
// object_state == 2: the same owner -> destroy_obj; a different owner ->
// found->CantSeeBits |= 1 << this_owner_id (the v5[1].sprite->pict_name
// render = CantSeeBits @208 via the base-pointer rule) and this->
// CantSeeBits |= 1 << found_owner_id.
// ----------------------------------------------------------------------------
void RGE_Doppleganger_Object::validate()
{
    RGE_Tile* tile = &owner->world->map->map_row_offset[(int)world_y][(int)world_x];   // .c:133945
    int count = tile->TileObject_Count;                                // .c:133946-133948
    if (count != 0)
    {
        for (int i = 0; i < count; ++i)
        {
            RGE_Static_Object* found = (RGE_Static_Object *)tile->Object_List[i];   // .c:133955
            if (found != 0                                             // .c:133956
                && found->type == RGE_MASTER_TYPE_DOPPELGANGER         // .c:133957
                && found->id != id                                     // .c:133958
                && ((RGE_Doppleganger_Object*)found)->doppledObjectID == doppledObjectID   // .c:133959 (the base-pointer rule)
                && found->object_state == 2)                           // .c:133960
            {
                if (found->owner->id == owner->id)                     // .c:133962-133964
                    found->destroy_obj();                              // .c:133966 (slot 56 - the named virtual)
                else
                {
                    ((RGE_Doppleganger_Object*)found)->CantSeeBits |= 1 << owner->id;   // .c:133971 (the base-pointer render; the asm pass verifies the exact field)
                    CantSeeBits |= 1 << found->owner->id;              // .c:133972
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------
// RGE_Doppleganger_Object::rehook - the slot-39 override. Original:
// @ 0x00470300 (idb_functions.h). Range [0x470300, 0x4703B0). Decode
// (.c:133770-133812): the Static rehook; the doppled_object NEGATIVE-id
// resync (the encoded < 0: doppled_object = 0, doppledObjectID = -1,
// draw_color_owner = world->players[-1-encoded]) OR the pointer resync
// (get_object_pointer -> doppled_object; non-null -> doppledObjectID = id
// + draw_color_owner = obj->owner); then the doppled_master_obj bounds
// restore (the negative or >= players[doppled_player]->master_object_num
// -> zero + destroy_obj via the vtable; else master_objects[doppled_
// master_obj]).
// ----------------------------------------------------------------------------
void RGE_Doppleganger_Object::rehook()
{
    RGE_Static_Object::rehook();                                       // .c:133781 (the base implementation)
    int encoded = (int)doppled_object;                                 // .c:133782
    if (encoded < 0)                                                   // .c:133783
    {
        doppled_object = 0;                                            // .c:133785
        int player_index = -1 - encoded;                               // .c:133786
        doppledObjectID = -1;                                          // .c:133788
        draw_color_owner = (RGE_Player*)owner->world->players[player_index];   // .c:133789 (players @+0x3C; the dual-naming cast)
    }
    else
    {
        RGE_Static_Object* obj = RGE_Static_Object__get_object_pointer(
            (RGE_Static_Object*)this, encoded);                        // .c:133792 (0x54F500)
        doppled_object = obj;                                          // .c:133793
        if (obj != 0)                                                  // .c:133794
        {
            doppledObjectID = obj->id;                                 // .c:133796
            draw_color_owner = (RGE_Player*)obj->owner;                // .c:133797 (the dual-naming cast)
        }
    }
    RGE_Master_Static_Object* old_master = doppled_master_obj;         // .c:133801
    RGE_Player* player = (RGE_Player*)owner->world->players[doppled_player];   // .c:133802 (the dual-naming cast)
    if ((int)old_master < 0 || (int)old_master >= player->master_object_num)   // .c:133802
    {
        doppled_master_obj = 0;                                        // .c:133805
        destroy_obj();                                                 // .c:133806 (slot 56)
    }
    else
    {
        doppled_master_obj =
            player->master_objects[(int)old_master];                   // .c:133810
    }
}

// ---- the placement-new trampolines (the ::new global form; see 15.3) ----
static RGE_Doppleganger_Object* __fastcall RGE_Doppleganger_Object_ctor_trampoline(
    RGE_Doppleganger_Object* this_, DWORD dummy, RGE_Master_Doppleganger_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z, int do_setup, RGE_Static_Object* obj_to_dopple)
{ return ::new (this_) RGE_Doppleganger_Object(tobj, obj_owner, x, y, z, do_setup, obj_to_dopple); }

static RGE_Doppleganger_Object* __fastcall RGE_Doppleganger_Object_ctor_read_trampoline(
    RGE_Doppleganger_Object* this_, DWORD dummy, int infile, RGE_Game_World* gworld, int do_setup)
{ return ::new (this_) RGE_Doppleganger_Object(infile, gworld, do_setup); }

// ---- the qualified-call trampolines ----
static unsigned __int8 __fastcall RGE_Doppleganger_Object_update_trampoline(
    RGE_Doppleganger_Object* this_, DWORD dummy)
{ return this_->RGE_Doppleganger_Object::update(); }

static void __fastcall RGE_Doppleganger_Object_rehook_trampoline(
    RGE_Doppleganger_Object* this_, DWORD dummy)
{ this_->RGE_Doppleganger_Object::rehook(); }

static void __fastcall RGE_Doppleganger_Object_save_trampoline(
    RGE_Doppleganger_Object* this_, DWORD dummy, int outfile)
{ this_->RGE_Doppleganger_Object::save(outfile); }

static void __fastcall RGE_Doppleganger_Object_validate_trampoline(
    RGE_Doppleganger_Object* this_, DWORD dummy)
{ this_->RGE_Doppleganger_Object::validate(); }

static void __fastcall RGE_Doppleganger_Object_recycle_in_to_game_trampoline(
    RGE_Doppleganger_Object* this_, DWORD dummy, RGE_Master_Static_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z, RGE_Static_Object* obj_to_dopple)
{ this_->RGE_Doppleganger_Object::recycle_in_to_game(tobj, obj_owner, x, y, z, obj_to_dopple); }

static int __fastcall RGE_Doppleganger_Object_setup_trampoline(
    RGE_Doppleganger_Object* this_, DWORD dummy, RGE_Master_Doppleganger_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z)
{ return this_->RGE_Doppleganger_Object::setup(tobj, obj_owner, x, y, z); }

static int __fastcall RGE_Doppleganger_Object_setup_read_trampoline(
    RGE_Doppleganger_Object* this_, DWORD dummy, int infile, RGE_Game_World* gworld)
{ return this_->RGE_Doppleganger_Object::setup_read(infile, gworld); }

// ============ THE INSTALLER (the same 9 addresses; the trampoline targets) ============
void install_RGE_Doppleganger_Object()
{
    setHook((void*)0x0046FBF0, RGE_Doppleganger_Object_ctor_trampoline);
    setHook((void*)0x0046FE90, RGE_Doppleganger_Object_ctor_read_trampoline);
    setHook((void*)0x00470230, RGE_Doppleganger_Object_setup_trampoline);
    setHook((void*)0x004702A0, RGE_Doppleganger_Object_setup_read_trampoline);
    setHook((void*)0x00470000, RGE_Doppleganger_Object_recycle_in_to_game_trampoline);
    setHook((void*)0x00470300, RGE_Doppleganger_Object_rehook_trampoline);
    setHook((void*)0x004703B0, RGE_Doppleganger_Object_update_trampoline);
    setHook((void*)0x00470530, RGE_Doppleganger_Object_save_trampoline);
    setHook((void*)0x004705F0, RGE_Doppleganger_Object_validate_trampoline);
}
