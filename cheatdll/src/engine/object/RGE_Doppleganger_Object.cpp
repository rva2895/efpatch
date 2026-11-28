#include "stdafx.h"
#include "RGE_Master_Static_Object.h"

// .rdata vftable of RGE_Doppleganger_Object (lst :989403)
static RGE_Doppleganger_ObjectVtbl* const rge_doppleganger_object_vftable =
    (RGE_Doppleganger_ObjectVtbl*)0x0065A2C0;

// .data:0077FA90 - the game's sound-suppression flag (lst :1105298;
// battlegrounds_x2.c:44143 `char temp_disable_sound_playing`)
static char* const temp_disable_sound_playing = (char*)0x0077FA90;

// unified_map_offsets = project constant (common.h:119, common.cpp:24 =
// .data:007A1CBC); row-dword reads: unified_map_offsets[row] = table + row*4.

// Replacement for RGE_Doppleganger_Object::RGE_Doppleganger_Object (runtime
// ctor). Original: @ 0x0046FBF0 (idb_functions.h:1480). Range [0x46FBF0,
// 0x46FE66) (lst :188929/:189143, retn 1Ch = 7 stack args), returns this.
// ABI: __thiscall - this_ in ECX. Caller: RGE_Master_Doppleganger_Object__
// make_new_obj (owned, hook-routed; lst :188930).
// Decode (asm 0x46FBF0-0x46FE66): base runtime Animated ctor with
// do_setup = 0 (hooked -> routes), the Doppleganger vfptr store, then the
// UNCONDITIONAL dopple-extension zero block in asm order 0x46FC43-0x46FC89:
// B8/BC/C0/C4/C8/CC/D0 dwords, D4/D5 bytes, D8 = -1 dword, D6/D7 bytes.
// IF do_setup: the runtime Animated setup (idb constant :548), type @+0x5E
// = 0x19 (the DOPPELGANGER runtime tier mirror), LastMapValue/CantSeeBits
// zeroed again, then the tile computation. doppled_object = obj_to_dopple
// unconditionally (asm 0x46FCE3). IF obj_to_dopple: the source-binding
// copies, the construction-sprite gate (Building masters), the sound-
// suppressed sprite-list block, facet/animation-frame/draw-color copies,
// the gated Multi_Unified_Points copy and the source-died release; ELSE
// the no-source path releases the binding and self-destructs. validate()
// then return this.
// MAP_SIZE: mapsize.cpp:1739 re-points the tile-row table disp32 inside
// THIS function (0x46FCC4) at the big-map base (mapptr) when active; the
// ctor_read twin is mapsize.cpp:1740 @0x46FF43. NOT baked (owner-directed
// MAP_SIZE rule 2026-11-15): the replacement keeps the original's
// unified_map_offsets base, so under the big-map mod the dopple tile
// intentionally diverges from the patched original.
RGE_Doppleganger_Object* __fastcall RGE_Doppleganger_Object__RGE_Doppleganger_Object_new(
    RGE_Doppleganger_Object* this_, DWORD dummy,
    RGE_Master_Doppleganger_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z, int do_setup, RGE_Static_Object* obj_to_dopple)
{
    RGE_Doppleganger_Object* result = this_;
    __try
    {
        RGE_Animated_Object__RGE_Animated_Object(
            (RGE_Animated_Object*)this_, (RGE_Master_Animated_Object*)tobj, obj_owner, x, y, z, 0);
        this_->vfptr = rge_doppleganger_object_vftable;           // asm 0x46FC3D
        this_->doppled_object = 0;                                // asm 0x46FC43
        this_->draw_color_owner = 0;                              // asm 0x46FC49
        this_->doppled_master_obj = 0;                            // asm 0x46FC4F
        this_->doppled_player = 0;                                // asm 0x46FC55
        this_->MapAddress = 0;                                    // asm 0x46FC5B
        this_->LastMapValue = 0;                                  // asm 0x46FC61
        this_->CantSeeBits = 0;                                   // asm 0x46FC67
        this_->map_draw_level = 0;                                // asm 0x46FC6D
        this_->map_color = 0;                                     // asm 0x46FC73
        this_->doppledObjectID = -1;                              // asm 0x46FC79
        this_->Multi_Unified_Points = 0;                          // asm 0x46FC83
        this_->Destroy_Next_Turn = 0;                             // asm 0x46FC89
        if (do_setup != 0)
        {
            RGE_Animated_Object__setup(
                (RGE_Animated_Object*)this_, (RGE_Master_Animated_Object*)tobj, obj_owner, x, y, z);
            this_->type = RGE_MASTER_TYPE_DOPPELGANGER;                                     // asm 0x46FCAD - runtime tier mirror
            this_->LastMapValue = 0;                              // asm 0x46FCC8
            this_->CantSeeBits = 0;                               // asm 0x46FCCE
            this_->MapAddress = (unsigned int*)(                  // MAP_SIZE - asm 0x46FCC1-0x46FCD7:
                (int)unified_map_offsets[(int)y] + 4 * (int)x);   // ftol(y) row, + ftol(x)*4
        }
        this_->doppled_object = (RGE_Static_Object_idb*)obj_to_dopple;                    // asm 0x46FCE3 (unconditional)
        if (obj_to_dopple != 0)
        {
            this_->doppled_master_obj = obj_to_dopple->master_obj;              // asm 0x46FCEF-0x46FCF2
            this_->doppled_player = obj_to_dopple->owner->id;                   // asm 0x46FCF8-0x46FD01 (id @0xA0 = 160)
            this_->doppledObjectID = obj_to_dopple->id;                         // asm 0x46FD07-0x46FD0A
            this_->sprite = obj_to_dopple->sprite;                              // asm 0x46FD10-0x46FD13
            RGE_Master_Static_Object* src_master = obj_to_dopple->master_obj;
            int swap_flag = 0;                                                  // asm dl (xor dl, dl @0x46FD19)
            if (src_master->master_type == RGE_MASTER_TYPE_BUILDING &&                                // asm 0x46FD1B (byte @+4)
                src_master->object_group != 8 &&                                // asm 0x46FD21 (word @+0x1E)
                src_master->id != 335)                                          // asm 0x46FD28 (word @+0x18)
            {
                // Base-pointer rule: the pseudocode's v10[2].construction_radius_x
                // = 2*0xD0 + 0x6C = 0x20C = 524 -> the Building-tier construction_id
                // (TRIBE_Master_Building_Object, i16). Runtime-type evidence for the
                // base->derived cast: the master_type == RGE_MASTER_TYPE_BUILDING gate (factory identity).
                __int16 construction_id =
                    ((TRIBE_Master_Building_Object*)src_master)->construction_id;   // asm 0x46FD30
                if (construction_id > -1)                                           // asm 0x46FD37 (signed word cmp)
                {
                    RGE_Master_Static_Object* sprite_master =
                        obj_to_dopple->owner->master_objects[construction_id];      // asm 0x46FD3D-0x46FD47 (@0x74)
                    this_->sprite = sprite_master->vfptr->get_sprite(sprite_master, (RGE_Static_Object_idb*)obj_to_dopple);   // master vtbl slot +4 (asm 0x46FD4C)
                    swap_flag = 1;                                                  // asm 0x46FD52
                }
            }
            const char old_sound_gate = *temp_disable_sound_playing;            // asm 0x46FD54
            *temp_disable_sound_playing = 1;                                    // asm 0x46FD59
            RGE_Active_Sprite_List* src_list = this_->doppled_object->sprite_list;   // asm 0x46FD60-0x46FD6A (@+0x24)
            if (src_list == 0 || swap_flag != 0)                                // asm 0x46FD6D-0x46FD73
            {
                RGE_Active_Sprite_List__delete_list(this_->sprite_list);        // asm 0x46FD85-0x46FD88 (NO null guard - faithful)
                if (this_->sprite != 0)                                         // asm 0x46FD8D-0x46FD92
                    RGE_Active_Sprite_List__add_sprite(this_->sprite_list, this_->sprite, 0, 0, 0, 0);   // asm 0x46FD94-0x46FD9C
            }
            else
            {
                RGE_Active_Sprite_Node* temp_list = RGE_Active_Sprite_List__copy_sprite_list(src_list);   // asm 0x46FD75
                RGE_Active_Sprite_List__use_sprite_list(this_->sprite_list, temp_list);   // asm 0x46FD7A-0x46FD7E
            }
            *temp_disable_sound_playing = old_sound_gate;                       // asm 0x46FDA1-0x46FDA5
            this_->facet = this_->doppled_object->facet;                        // asm 0x46FDB1-0x46FDB4 (byte @0x45; DECODE-STATE
                                                                                // correction: facet, not "selected" - selected @0x46)
            __int16 frame = RGE_Static_Object__get_animation_frame((RGE_Static_Object *)this_->doppled_object);   // asm 0x46FDB7 (:3853)
            RGE_Static_Object__set_animation_frame((RGE_Static_Object*)this_, frame);        // asm 0x46FDBC-0x46FDBF (:3852)
            this_->draw_color_owner = (RGE_Player*)this_->doppled_object->owner;   // asm 0x46FDC4-0x46FDCD (owner @0x18;
                                                                                    // TRIBE_Player* -> RGE_Player* dual-naming cast)
            this_->map_draw_level = this_->doppled_object->master_obj->map_draw_level;   // asm 0x46FDD3-0x46FDDF (@0x9E = 158)
            if (this_->doppled_object->master_obj->map_draw_level == 3)         // asm 0x46FDDC
                this_->map_color = this_->doppled_object->master_obj->map_color;   // asm 0x46FDE7-0x46FDF0 (@0xA0 = 160)
            else
                this_->map_color = 0;                                           // asm 0x46FDF8
            if (this_->doppled_master_obj->master_type >= RGE_MASTER_TYPE_COMBAT_BASE)                   // asm 0x46FE04 (byte @+4, jb skips)
                // Base-pointer rule: the pseudocode's v20[2].gbg_civ_override
                // = 2*0xB4 + 0x4 = 0x16C = 364 -> RGE_Combat_Object::
                // Multi_Unified_Points (u8 @364; present in every Combat-tier
                // runtime struct prefix). Runtime-type evidence: the
                // master_type >= RGE_MASTER_TYPE_COMBAT_BASE gate (factory identity, Combat(70)
                // check_placement precedent).
                this_->Multi_Unified_Points =
                    ((RGE_Combat_Object*)this_->doppled_object)->Multi_Unified_Points;   // asm 0x46FE0A
            if (this_->doppled_object->object_state >= 7)                       // asm 0x46FE16 (byte @+0x58)
            {
                this_->doppled_object = 0;                                      // asm 0x46FE1C - the source-died release
                this_->doppledObjectID = -1;                                    // asm 0x46FE22
            }
        }
        else
        {
            this_->doppledObjectID = -1;                                        // asm 0x46FE30
            this_->doppled_master_obj = 0;                                      // asm 0x46FE3A
            this_->doppled_player = 0;                                          // asm 0x46FE40
            RGE_Static_Object__destroy_obj((RGE_Static_Object*)this_);          // asm 0x46FE46 (:3843 - SELF-DESTRUCT)
        }
        RGE_Doppleganger_Object__validate(this_);                               // asm 0x46FE4D (:1489)
        result = this_;
    }
    __except (RGE_Static_Object__destructor((RGE_Static_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x641CD0
    {
    }
    return result;
}

// Replacement for RGE_Doppleganger_Object::RGE_Doppleganger_Object_read
// (runtime ctor_read). Original: @ 0x0046FE90 (idb_functions.h:1482). Range
// [0x46FE90, 0x46FFF4) (lst :189180/:189291, retn 0Ch = 3 stack args),
// returns this. ABI: __thiscall - this_ in ECX. Callers: RGE_Object_List__
// load / RGE_Player_Object_List__Load (lst :189181/:189182).
// Decode (asm 0x46FE90-0x46FFF4): base runtime Animated ctor_read with
// do_setup = 0 (idb constant :546 - NOT hooked, executes the original), the
// Doppleganger vfptr store, the SAME zero block MINUS draw_color_owner (asm
// 0x46FED8-0x46FF15 has no BC store), then IF do_setup: the runtime Animated
// setup_read (:549), type = 25, LastMapValue/CantSeeBits zeroed again, and
// the tile computation from the LOADED world_y (@+0x4C) / world_x (@+0x48).
// THEN the unconditional rge_read stream (:3003, 0x004D5550): (4) ->
// doppled_object; doppledObjectID = -1 (asm 0x46FF82); (1) -> map_draw_
// level; (4) -> &map_color (DELIBERATE SPAN - see the call); (4) ->
// doppled_master_obj; (4) -> doppled_player; (4) -> CantSeeBits; (1) ->
// Multi_Unified_Points.
// MAP_SIZE: mapsize.cpp:1740 re-points the tile-row table disp32 inside
// THIS function (0x46FF43) at the big-map base (mapptr) when active. NOT
// baked (owner-directed MAP_SIZE rule 2026-11-15): the replacement keeps
// the original's unified_map_offsets base.
RGE_Doppleganger_Object* __fastcall RGE_Doppleganger_Object__RGE_Doppleganger_Object_read_new(
    RGE_Doppleganger_Object* this_, DWORD dummy, int infile, RGE_Game_World* gworld, int do_setup)
{
    RGE_Doppleganger_Object* result = this_;
    __try
    {
        int read_value;   // the asm's infile-arg-slot reuse as a 4-byte buffer
        RGE_Animated_Object__RGE_Animated_Object_read((RGE_Animated_Object*)this_, infile, gworld, 0);
        this_->vfptr = rge_doppleganger_object_vftable;           // asm 0x46FED2
        this_->doppled_object = 0;                                // asm 0x46FED8
        this_->doppled_master_obj = 0;                            // asm 0x46FEDE
        this_->doppled_player = 0;                                // asm 0x46FEE4
        this_->MapAddress = 0;                                    // asm 0x46FEEA
        this_->LastMapValue = 0;                                  // asm 0x46FEF0
        this_->CantSeeBits = 0;                                   // asm 0x46FEF6
        this_->map_draw_level = 0;                                // asm 0x46FEF9
        this_->map_color = 0;                                     // asm 0x46FEFF
        this_->doppledObjectID = -1;                              // asm 0x46FF05
        this_->Multi_Unified_Points = 0;                          // asm 0x46FF0F
        this_->Destroy_Next_Turn = 0;                             // asm 0x46FF15
        if (do_setup != 0)
        {
            RGE_Animated_Object__setup_read((RGE_Animated_Object*)this_, infile, gworld);
            this_->type = RGE_MASTER_TYPE_DOPPELGANGER;                                     // asm 0x46FF2D - runtime tier mirror
            this_->LastMapValue = 0;                              // asm 0x46FF47
            this_->CantSeeBits = 0;                               // asm 0x46FF51
            this_->MapAddress = (unsigned int*)(                  // MAP_SIZE - asm 0x46FF35-0x46FF5B:
                (int)unified_map_offsets[(int)this_->world_y]     // ftol(world_y @0x4C) row,
                + 4 * (int)this_->world_x);                       // + ftol(world_x @0x48)*4
        }
        rge_read(infile, &read_value, sizeof(read_value));                    // asm 0x46FF61-0x46FF69 (4)
        this_->doppled_object = (RGE_Static_Object_idb*)(RGE_Static_Object_idb*)read_value;               // asm 0x46FF74
        this_->doppledObjectID = -1;                                          // asm 0x46FF82
        rge_read(infile, &this_->map_draw_level, sizeof(this_->map_draw_level));   // asm 0x46FF8C (1)
        // DEVIATION (deliberate span, decode-record approved): the original
        // streams 4 bytes at &map_color covering map_color @0xD5 +
        // Multi_Unified_Points @0xD6 + Destroy_Next_Turn @0xD7 + the
        // doppledObjectID low byte @0xD8 - asm 0x46FF91-0x46FF9B (lea edx,
        // [esi+0D5h]; push 4). doppledObjectID reads 0xFFFFFFxx after this
        // call - faithful; sizeof(&map_color) would be 1.
        rge_read(infile, &this_->map_color, 4);
        rge_read(infile, &read_value, sizeof(read_value));                    // asm 0x46FFA0-0x46FFA8 (4)
        this_->doppled_master_obj = (RGE_Master_Static_Object*)read_value;    // asm 0x46FFB3
        rge_read(infile, &this_->doppled_player, sizeof(this_->doppled_player));   // asm 0x46FFC1 (4)
        rge_read(infile, &this_->CantSeeBits, sizeof(this_->CantSeeBits));         // asm 0x46FFCC (4)
        rge_read(infile, &this_->Multi_Unified_Points, sizeof(this_->Multi_Unified_Points));   // asm 0x46FFDB (1)
        result = this_;
    }
    __except (RGE_Static_Object__destructor((RGE_Static_Object*)this_), EXCEPTION_CONTINUE_SEARCH)   // SEH chunk 0x641CF0
    {
    }
    return result;
}

// ============ RUNTIME METHODS (batch M1) ============
// The temp_disable_sound_playing cpp-local binding (0x77FA90) from the
// ctor above is reused by recycle_in_to_game below.

// Replacement for RGE_Doppleganger_Object::setup. Original:
// @ 0x00470230 (idb_functions.h). Range [0x470230, 0x4702A0), returns
// int (1). Decode (.c:133745-133753): Animated setup + type=25 +
// the MAP_SIZE tile store + LastMapValue/CantSeeBits = 0.
int __fastcall RGE_Doppleganger_Object__setup_new(
    RGE_Doppleganger_Object* this_, DWORD dummy,
    RGE_Master_Doppleganger_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z)
{
    RGE_Animated_Object__setup((RGE_Animated_Object*)this_,
        (RGE_Master_Animated_Object*)tobj, obj_owner, x, y, z);   // .c:133747
    this_->type = RGE_MASTER_TYPE_DOPPELGANGER;                                // .c:133748
    // MAP_SIZE: mapsize.cpp:1748 re-points this disp32 (0x47026F) at
    // mapptr. NOT baked (owner-directed MAP_SIZE rule).
    this_->MapAddress = (unsigned int*)((int)unified_map_offsets[(int)y] + 4 * (int)x);   // .c:133749
    this_->LastMapValue = 0;                         // .c:133750
    this_->CantSeeBits = 0;                          // .c:133751
    return 1;
}

// Replacement for RGE_Doppleganger_Object::setup_read. Original:
// @ 0x004702A0 (idb_functions.h). Range [0x4702A0, 0x470300), returns
// int (1). Decode (.c:133756-133767): Animated setup_read + type=25 +
// the MAP_SIZE tile store from the LOADED world_y/world_x +
// LastMapValue/CantSeeBits = 0.
int __fastcall RGE_Doppleganger_Object__setup_read_new(
    RGE_Doppleganger_Object* this_, DWORD dummy, int infile, RGE_Game_World* gworld)
{
    RGE_Animated_Object__setup_read((RGE_Animated_Object*)this_, infile, gworld);   // .c:133760
    this_->type = RGE_MASTER_TYPE_DOPPELGANGER;                                // .c:133762
    // MAP_SIZE: mapsize.cpp:1749 (disp32 0x4702CE). NOT baked.
    this_->MapAddress = (unsigned int*)((int)unified_map_offsets[(int)this_->world_y]
        + 4 * (int)this_->world_x);                  // .c:133763
    this_->LastMapValue = 0;                         // .c:133764
    this_->CantSeeBits = 0;                          // .c:133765
    return 1;
}

// Replacement for RGE_Doppleganger_Object::recycle_in_to_game.
// Original: @ 0x00470000. Range [0x470000, 0x470230). Decode
// (.c:133634-133741): the zero block, the Animated recycle, type=25,
// doppled_object = obj_to_dopple, the source-binding block, the
// state>=7 validate-return path.
// MAP_SIZE: mapsize.cpp:1747 (disp32 0x4701A7). NOT baked.
void __fastcall RGE_Doppleganger_Object__recycle_in_to_game_new(
    RGE_Doppleganger_Object* this_, DWORD dummy,
    RGE_Master_Static_Object* tobj, RGE_Player* obj_owner,
    float x, float y, float z, RGE_Static_Object* obj_to_dopple)
{
    this_->doppled_object = 0;                       // .c:133654
    this_->doppled_master_obj = 0;                   // .c:133655
    this_->doppled_player = 0;                       // .c:133656
    this_->MapAddress = 0;                           // .c:133657
    this_->LastMapValue = 0;                         // .c:133658
    this_->CantSeeBits = 0;                          // .c:133659
    this_->map_draw_level = 0;                       // .c:133660
    this_->map_color = 0;                            // .c:133661
    this_->doppledObjectID = -1;                     // .c:133662
    this_->Multi_Unified_Points = 0;                 // .c:133663
    this_->Destroy_Next_Turn = 0;                    // .c:133664
    RGE_Animated_Object__recycle_in_to_game((RGE_Animated_Object*)this_, tobj, obj_owner, x, y, z);   // .c:133665
    this_->type = RGE_MASTER_TYPE_DOPPELGANGER;                                // .c:133666
    this_->doppled_object = (RGE_Static_Object_idb*)obj_to_dopple;           // .c:133667
    if (obj_to_dopple != 0)                          // .c:133668
    {
        this_->doppled_master_obj = obj_to_dopple->master_obj;        // .c:133670
        this_->doppled_player = obj_to_dopple->owner->id;             // .c:133671
        this_->doppledObjectID = obj_to_dopple->id;                   // .c:133672
        this_->sprite = obj_to_dopple->sprite;                        // .c:133673
        RGE_Master_Static_Object* master = obj_to_dopple->master_obj; // .c:133674
        int installed = 0;                                            // .c:133675 (v9)
        if (master->master_type == RGE_MASTER_TYPE_BUILDING && master->object_group != 8 && master->id != 335)   // .c:133676
        {
            __int16 construction_id = ((TRIBE_Master_Building_Object*)master)->construction_id;   // .c:133678 (v10; +0x20C render)
            if (construction_id > -1)                                 // .c:133679
            {
                RGE_Master_Static_Object* sprite_master =
                    obj_to_dopple->owner->master_objects[construction_id];   // .c:133681
                this_->sprite = sprite_master->vfptr->get_sprite(sprite_master, (RGE_Static_Object_idb*)obj_to_dopple);   // .c:133682
                installed = 1;                                        // .c:133683
            }
        }
        char old_sound = *temp_disable_sound_playing;             // .c:133686
        *temp_disable_sound_playing = 1;                          // .c:133687
        RGE_Active_Sprite_List* src_list = this_->doppled_object->sprite_list;   // .c:133689
        if (src_list == 0 || installed != 0)                      // .c:133690
        {
            RGE_Active_Sprite_List__delete_list(this_->sprite_list);   // .c:133692
            if (this_->sprite != 0)                               // .c:133693
                RGE_Active_Sprite_List__add_sprite(this_->sprite_list, this_->sprite, 0, 0, 0, 0);   // .c:133695
        }
        else
        {
            RGE_Active_Sprite_Node* temp_list = RGE_Active_Sprite_List__copy_sprite_list(src_list);   // .c:133699
            RGE_Active_Sprite_List__use_sprite_list(this_->sprite_list, temp_list);   // .c:133700
        }
        *temp_disable_sound_playing = old_sound;                  // .c:133702
        this_->facet = this_->doppled_object->facet;              // .c:133704
        __int16 frame = RGE_Static_Object__get_animation_frame((RGE_Static_Object *)this_->doppled_object);   // .c:133705
        RGE_Static_Object__set_animation_frame((RGE_Static_Object*)this_, frame);   // .c:133706
        this_->draw_color_owner = (RGE_Player*)this_->doppled_object->owner;   // .c:133708
        this_->map_draw_level = this_->doppled_object->master_obj->map_draw_level;   // .c:133709
        if (this_->doppled_object->master_obj->map_draw_level == 3)   // .c:133711
            this_->map_color = this_->doppled_object->master_obj->map_color;   // .c:133712
        else
            this_->map_color = 0;                                 // .c:133714
        // MAP_SIZE: mapsize.cpp:1747 (disp32 0x4701A7). NOT baked.
        this_->MapAddress = (unsigned int*)((int)unified_map_offsets[(int)y] + 4 * (int)x);   // .c:133715-133720
        this_->LastMapValue = 0;                                  // .c:133718
        this_->CantSeeBits = 0;                                   // .c:133719
        if (this_->doppled_master_obj->master_type >= RGE_MASTER_TYPE_COMBAT_BASE)         // .c:133721
            this_->Multi_Unified_Points =
                ((RGE_Combat_Object*)this_->doppled_object)->Multi_Unified_Points;   // .c:133722
        if (this_->doppled_object->object_state >= 7)             // .c:133723
        {
            this_->doppled_object = 0;                            // .c:133726
            this_->doppledObjectID = -1;                          // .c:133727
            this_->vfptr->validate(this_);                        // .c:133728
            return;                                               // .c:133729
        }
    }
    else
    {
        this_->doppledObjectID = -1;                              // .c:133735
        this_->doppled_master_obj = 0;                            // .c:133736
        this_->doppled_player = 0;                                // .c:133737
        this_->vfptr->destroy_obj(this_);                         // .c:133738
    }
    this_->vfptr->validate(this_);                                // .c:133740
}

// Replacement for RGE_Doppleganger_Object::update. Original:
// @ 0x004703B0 (idb_functions.h). Range [0x4703B0, 0x470530), u8
// return. Decode (.c:133815-133904): Animated update; the Destroy_
// Next_Turn machinery; the doppled_object state release (RESTRUCTURED
// per review: the original's goto done = the state-7 release falling
// into the shared ID=-1 store; written as the truth-table-equivalent
// disjunctive guard); the UMV visibility machinery (the MapAddress
// snapshot, the Multi_Unified_Points loop over map->UMV_Data_Points,
// LastMapValue compare, the mutualVisibleMask gates incl. the
// doppled->Unified_Map_Value base-pointer read @+0x168 (= v12[2].vfptr),
// the RemoveVisibleBits -> CantSeeBits accumulation).
unsigned __int8 __fastcall RGE_Doppleganger_Object__update_new(RGE_Doppleganger_Object* this_, DWORD dummy)
{
    unsigned __int8 result = RGE_Animated_Object__update((RGE_Animated_Object*)this_);   // .c:133833
    if (this_->Destroy_Next_Turn != 0 && this_->object_state < 3)   // .c:133835
    {
        this_->vfptr->destroy_obj(this_);            // .c:133837
        return result;                               // .c:133838
    }
    if (this_->doppled_object != 0 && this_->doppled_object->object_state >= 7)   // .c:133840-133844 (goto restructured)
    {
        this_->doppled_object = 0;                   // .c:133845
        this_->doppledObjectID = -1;                 // .c:133847
    }
    unsigned int map_value = *this_->MapAddress;     // .c:133849
    if (this_->Multi_Unified_Points != 0)            // .c:133850
    {
        UMV_Offset_Point* points =
            this_->owner->world->map->UMV_Data_Points[this_->Multi_Unified_Points - 1];   // .c:133853
        int xoff = points->Xoffset;                  // .c:133854 (the count lives in the first point's Xoffset)
        for (int k = 0; k < xoff; ++k)               // .c:133855-133868 (do-while; xoff >= 1 -> same iterations; indexed form)
        {
            int row = (int)this_->world_y + points[1 + k].Yoffset;   // .c:133861
            int col = (int)this_->world_x + points[1 + k].Xoffset;   // .c:133862
            map_value |= (unsigned int)unified_map_offsets[row][col];   // .c:133864 (2D indexed form)
        }
    }
    if (map_value != this_->LastMapValue)            // .c:133870
    {
        unsigned int mutual = this_->owner->mutualVisibleMask;   // .c:133872
        if ((mutual & map_value) != 0 && this_->object_state < 3)   // .c:133873
        {
            if (this_->Multi_Unified_Points != 0)    // .c:133875
            {
                if (this_->doppled_object != 0)      // .c:133877
                {
                    if ((mutual & ((RGE_Combat_Object*)this_->doppled_object)->Unified_Map_Value) == 0   // .c:133880 (v12[2].vfptr = +0x168 = Unified_Map_Value)
                        && (mutual & *this_->MapAddress) == 0)   // .c:133880
                        this_->Destroy_Next_Turn = 1;   // .c:133881
                }
            }
            if (this_->Destroy_Next_Turn == 0)       // .c:133884
                this_->vfptr->destroy_obj(this_);    // .c:133885
        }
        unsigned int non_mutual = map_value & this_->owner->nonMutualVisibleMask;   // .c:133888
        if (non_mutual != 0)                         // .c:133889
        {
            unsigned int* bits = this_->owner->RemoveVisibleBits;   // .c:133891
            do
            {
                if ((non_mutual & 1) == 1)           // .c:133894
                    this_->CantSeeBits |= *bits;     // .c:133895
                ++bits;                              // .c:133896
                non_mutual >>= 1;                    // .c:133897
            }
            while (non_mutual != 0);                 // .c:133898
        }
        this_->LastMapValue = map_value;             // .c:133901
    }
    return result;
}

// Replacement for RGE_Doppleganger_Object::save. Original:
// @ 0x00470530 (idb_functions.h). Range [0x470530, 0x4705F0). Decode
// (.c:133907-133932): Animated save + doppledObjectID-or-(−1 −
// doppled_player) w4 + map_draw_level b1 + map_color w4 (the
// deliberate 4-byte span - SAME span shape as the ctor_read; faithful)
// + doppled_master_obj id-or-(−1) w4 + doppled_player w4 + CantSeeBits
// w4 + Multi_Unified_Points b1.
void __fastcall RGE_Doppleganger_Object__save_new(
    RGE_Doppleganger_Object* this_, DWORD dummy, int outfile)
{
    RGE_Animated_Object__save((RGE_Animated_Object*)this_, outfile);   // .c:133914
    int id_out;
    if (this_->doppled_object != 0)                  // .c:133916
        id_out = this_->doppled_object->id;          // .c:133917
    else
        id_out = -1 - this_->doppled_player;         // .c:133919
    rge_write(outfile, &id_out, sizeof(id_out));     // .c:133920 (4)
    rge_write(outfile, &this_->map_draw_level, sizeof(this_->map_draw_level));   // .c:133921 (1)
    // DELIBERATE 4-BYTE SPAN over map_color @+0xD5 + Multi_Unified_Points
    // @+0xD6 + Destroy_Next_Turn @+0xD7 + the doppledObjectID low byte
    // @+0xD8 - same span shape as the ctor_read; faithful (.c:133922).
    rge_write(outfile, &this_->map_color, 4);
    if (this_->doppled_master_obj != 0)              // .c:133924
        id_out = this_->doppled_master_obj->id;      // .c:133925
    else
        id_out = -1;                                 // .c:133927
    rge_write(outfile, &id_out, sizeof(id_out));     // .c:133928 (4)
    rge_write(outfile, &this_->doppled_player, sizeof(this_->doppled_player));   // .c:133929 (4)
    rge_write(outfile, &this_->CantSeeBits, sizeof(this_->CantSeeBits));   // .c:133930 (4)
    rge_write(outfile, &this_->Multi_Unified_Points, sizeof(this_->Multi_Unified_Points));   // .c:133931 (1)
}

// Replacement for RGE_Doppleganger_Object::validate. Original:
// @ 0x004705F0 (idb_functions.h). Range [0x4705F0, 0x4706D0). Decode
// (.c:133935-133980): the tile-walk dedup - map_row_offset[wy][wx],
// TileObject_Count/Object_List walk; a found Doppleganger (type==25)
// with a different id, matching doppledObjectID (the v5[1].sprite_list
// render = doppledObjectID @216 via the base-pointer rule) and
// object_state == 2: same owner -> destroy_obj; different owner ->
// found->CantSeeBits |= 1 << this_owner_id (the v5[1].sprite->pict_name
// render = CantSeeBits @208 via the base-pointer rule) and this->
// CantSeeBits |= 1 << found_owner_id.
void __fastcall RGE_Doppleganger_Object__validate_new(RGE_Doppleganger_Object* this_, DWORD dummy)
{
    RGE_Tile* tile = &this_->owner->world->map->map_row_offset[(int)this_->world_y][(int)this_->world_x];   // .c:133945
    int count = tile->TileObject_Count;              // .c:133946-133948
    if (count != 0)
    {
        for (int i = 0; i < count; ++i)
        {
            RGE_Static_Object* found = (RGE_Static_Object *)tile->Object_List[i];   // .c:133955
            if (found != 0                               // .c:133956
                && found->type == RGE_MASTER_TYPE_DOPPELGANGER                     // .c:133957
                && found->id != this_->id                // .c:133958
                && ((RGE_Doppleganger_Object*)found)->doppledObjectID == this_->doppledObjectID   // .c:133959 (base-pointer)
                && found->object_state == 2)             // .c:133960
            {
                if (found->owner->id == this_->owner->id)   // .c:133962-133964
                    found->destroy_obj();   // .c:133966
                else
                {
                    ((RGE_Doppleganger_Object*)found)->CantSeeBits |= 1 << this_->owner->id;   // .c:133971 (base-pointer render; asm pass verifies the exact field)
                    this_->CantSeeBits |= 1 << found->owner->id;   // .c:133972
                }
            }
        }
    }
}

// Replacement for RGE_Doppleganger_Object::rehook. Original:
// @ 0x00470300 (idb_functions.h). Range [0x470300, 0x4703B0). Decode
// (.c:133770-133812): Static rehook (:3847-region... RGE_Static_Object__
// rehook 0x54F4B0); the doppled_object NEGATIVE-id resync (encoded <
// 0: doppled_object = 0, doppledObjectID = -1, draw_color_owner =
// world->players[-1-encoded]) OR the pointer resync (get_object_
// pointer -> doppled_object; non-null -> doppledObjectID = id +
// draw_color_owner = obj->owner); then the doppled_master_obj bounds
// restore (negative or >= players[doppled_player]->master_object_num
// -> zero + destroy_obj via vftable; else master_objects[doppled_
// master_obj]).
void __fastcall RGE_Doppleganger_Object__rehook_new(RGE_Doppleganger_Object* this_, DWORD dummy)
{
    RGE_Static_Object__rehook((RGE_Static_Object*)this_);   // .c:133781
    int encoded = (int)this_->doppled_object;        // .c:133782
    if (encoded < 0)                                 // .c:133783
    {
        this_->doppled_object = 0;                   // .c:133785
        int player_index = -1 - encoded;             // .c:133786
        this_->doppledObjectID = -1;                 // .c:133788
        this_->draw_color_owner =
            (RGE_Player*)this_->owner->world->players[player_index];   // .c:133789 (players @+0x3C; TRIBE_Player* -> RGE_Player* dual-naming cast)
    }
    else
    {
        RGE_Static_Object* obj = RGE_Static_Object__get_object_pointer(
            (RGE_Static_Object*)this_, encoded);     // .c:133792 (0x54F500)
        this_->doppled_object = (RGE_Static_Object_idb*)obj;                 // .c:133793
        if (obj != 0)                                // .c:133794
        {
            this_->doppledObjectID = obj->id;        // .c:133796
            this_->draw_color_owner = (RGE_Player*)obj->owner;   // .c:133797 (dual-naming cast)
        }
    }
    RGE_Master_Static_Object* old_master = this_->doppled_master_obj;   // .c:133801
    RGE_Player* player = (RGE_Player*)this_->owner->world->players[this_->doppled_player];   // .c:133802 (dual-naming cast)
    if ((int)old_master < 0 || (int)old_master >= player->master_object_num)   // .c:133802
    {
        this_->doppled_master_obj = 0;               // .c:133805
        this_->vfptr->destroy_obj(this_);            // .c:133806
    }
    else
    {
        this_->doppled_master_obj =
            player->master_objects[(int)old_master];   // .c:133810
    }
}

void install_RGE_Doppleganger_Object()
{
    setHook((void*)0x0046FBF0, RGE_Doppleganger_Object__RGE_Doppleganger_Object_new);
    setHook((void*)0x0046FE90, RGE_Doppleganger_Object__RGE_Doppleganger_Object_read_new);
    setHook((void*)0x00470230, RGE_Doppleganger_Object__setup_new);
    setHook((void*)0x004702A0, RGE_Doppleganger_Object__setup_read_new);
    setHook((void*)0x00470000, RGE_Doppleganger_Object__recycle_in_to_game_new);
    setHook((void*)0x004703B0, RGE_Doppleganger_Object__update_new);
    setHook((void*)0x00470530, RGE_Doppleganger_Object__save_new);
    setHook((void*)0x004705F0, RGE_Doppleganger_Object__validate_new);
    setHook((void*)0x00470300, RGE_Doppleganger_Object__rehook_new);
}
