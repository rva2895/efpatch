#include "stdafx.h"

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
            this_->type = 25;                                     // asm 0x46FCAD - runtime tier mirror
            this_->LastMapValue = 0;                              // asm 0x46FCC8
            this_->CantSeeBits = 0;                               // asm 0x46FCCE
            this_->MapAddress = (unsigned int*)(                  // MAP_SIZE - asm 0x46FCC1-0x46FCD7:
                (int)unified_map_offsets[(int)y] + 4 * (int)x);   // ftol(y) row, + ftol(x)*4
        }
        this_->doppled_object = obj_to_dopple;                    // asm 0x46FCE3 (unconditional)
        if (obj_to_dopple != 0)
        {
            this_->doppled_master_obj = obj_to_dopple->master_obj;              // asm 0x46FCEF-0x46FCF2
            this_->doppled_player = obj_to_dopple->owner->id;                   // asm 0x46FCF8-0x46FD01 (id @0xA0 = 160)
            this_->doppledObjectID = obj_to_dopple->id;                         // asm 0x46FD07-0x46FD0A
            this_->sprite = obj_to_dopple->sprite;                              // asm 0x46FD10-0x46FD13
            RGE_Master_Static_Object* src_master = obj_to_dopple->master_obj;
            int swap_flag = 0;                                                  // asm dl (xor dl, dl @0x46FD19)
            if (src_master->master_type == 80 &&                                // asm 0x46FD1B (byte @+4)
                src_master->object_group != 8 &&                                // asm 0x46FD21 (word @+0x1E)
                src_master->id != 335)                                          // asm 0x46FD28 (word @+0x18)
            {
                // Base-pointer rule: the pseudocode's v10[2].construction_radius_x
                // = 2*0xD0 + 0x6C = 0x20C = 524 -> the Building-tier construction_id
                // (TRIBE_Master_Building_Object, i16). Runtime-type evidence for the
                // base->derived cast: the master_type == 80 gate (factory identity).
                __int16 construction_id =
                    ((TRIBE_Master_Building_Object*)src_master)->construction_id;   // asm 0x46FD30
                if (construction_id > -1)                                           // asm 0x46FD37 (signed word cmp)
                {
                    RGE_Master_Static_Object* sprite_master =
                        obj_to_dopple->owner->master_objects[construction_id];      // asm 0x46FD3D-0x46FD47 (@0x74)
                    this_->sprite = sprite_master->vfptr->get_sprite(sprite_master, obj_to_dopple);   // master vtbl slot +4 (asm 0x46FD4C)
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
            __int16 frame = RGE_Static_Object__get_animation_frame(this_->doppled_object);   // asm 0x46FDB7 (:3853)
            RGE_Static_Object__set_animation_frame((RGE_Static_Object*)this_, frame);        // asm 0x46FDBC-0x46FDBF (:3852)
            this_->draw_color_owner = (RGE_Player*)this_->doppled_object->owner;   // asm 0x46FDC4-0x46FDCD (owner @0x18;
                                                                                    // TRIBE_Player* -> RGE_Player* dual-naming cast)
            this_->map_draw_level = this_->doppled_object->master_obj->map_draw_level;   // asm 0x46FDD3-0x46FDDF (@0x9E = 158)
            if (this_->doppled_object->master_obj->map_draw_level == 3)         // asm 0x46FDDC
                this_->map_color = this_->doppled_object->master_obj->map_color;   // asm 0x46FDE7-0x46FDF0 (@0xA0 = 160)
            else
                this_->map_color = 0;                                           // asm 0x46FDF8
            if (this_->doppled_master_obj->master_type >= 50)                   // asm 0x46FE04 (byte @+4, jb skips)
                // Base-pointer rule: the pseudocode's v20[2].gbg_civ_override
                // = 2*0xB4 + 0x4 = 0x16C = 364 -> RGE_Combat_Object::
                // Multi_Unified_Points (u8 @364; present in every Combat-tier
                // runtime struct prefix). Runtime-type evidence: the
                // master_type >= 50 gate (factory identity, Combat(70)
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
            this_->type = 25;                                     // asm 0x46FF2D - runtime tier mirror
            this_->LastMapValue = 0;                              // asm 0x46FF47
            this_->CantSeeBits = 0;                               // asm 0x46FF51
            this_->MapAddress = (unsigned int*)(                  // MAP_SIZE - asm 0x46FF35-0x46FF5B:
                (int)unified_map_offsets[(int)this_->world_y]     // ftol(world_y @0x4C) row,
                + 4 * (int)this_->world_x);                       // + ftol(world_x @0x48)*4
        }
        rge_read(infile, &read_value, sizeof(read_value));                    // asm 0x46FF61-0x46FF69 (4)
        this_->doppled_object = (RGE_Static_Object*)read_value;               // asm 0x46FF74
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

void install_RGE_Doppleganger_Object()
{
    setHook((void*)0x0046FBF0, RGE_Doppleganger_Object__RGE_Doppleganger_Object_new);
    setHook((void*)0x0046FE90, RGE_Doppleganger_Object__RGE_Doppleganger_Object_read_new);
}
