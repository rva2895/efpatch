#include "stdafx.h"

// Replacement for RGE_Map::set_map_visible
// Original: RGE_Map__set_map_visible @ 0x00490650 (idb_functions.h:1944)
// ABI: __thiscall — this_ in ECX, flag first stack argument, retn 4, void.
// map_visible_flag: unsigned __int8 (idb_types.h:2927).
void __fastcall RGE_Map__set_map_visible_new(RGE_Map* this_, DWORD dummy, unsigned __int8 flag)
{
    this_->map_visible_flag = flag;
}

// Replacement for RGE_Map::set_map_fog
// Original: RGE_Map__set_map_fog @ 0x00490660 (idb_functions.h:1945)
// ABI: __thiscall — this_ in ECX, flag first stack argument, retn 4, void.
// fog_flag: unsigned __int8 (idb_types.h:2928).
void __fastcall RGE_Map__set_map_fog_new(RGE_Map* this_, DWORD dummy, unsigned __int8 flag)
{
    this_->fog_flag = flag;
}

// Replacement for RGE_Map::init_tile_sizes
// Original: RGE_Map__init_tile_sizes @ 0x00490330 (idb_functions.h:1941)
// ABI: __thiscall — this_ in ECX, no stack arguments, void. No EDX dummy.
// Initializes tile geometry (tile_width=64, tile_height=32, halves 32/16,
// elev_height=16) + the tilesizes[19] elevation-variant table
// (RGE_Tile_Size = { i16 width; i16 height; i16 y_delta; } @idb_types.h
// :2775-2781, array @2900). Ends with set_tile_size calls for indices
// 17/18 (idb_functions.h:1992, 0x004957E0 — not replaced; forwards).
void __fastcall RGE_Map__init_tile_sizes_new(RGE_Map* this_, DWORD dummy)
{
    this_->tile_width = 64;
    this_->tile_height = 32;
    this_->tile_half_width = 32;
    this_->tile_half_height = 16;
    this_->elev_height = 16;
    this_->tilesizes[0].width = 64;
    this_->tilesizes[0].height = 32;
    this_->tilesizes[0].y_delta = 0;
    this_->tilesizes[1].width = this_->tile_width;
    this_->tilesizes[1].height = this_->elev_height;
    this_->tilesizes[1].y_delta = -8;
    this_->tilesizes[2].width = this_->tile_width;
    this_->tilesizes[2].height = this_->elev_height + this_->tile_height;
    this_->tilesizes[2].y_delta = 8;
    this_->tilesizes[3].width = this_->tile_width;
    this_->tilesizes[3].height = this_->tile_height;
    this_->tilesizes[3].y_delta = -8;
    this_->tilesizes[4].width = this_->tile_width;
    this_->tilesizes[4].height = this_->tile_height;
    this_->tilesizes[4].y_delta = -8;
    this_->tilesizes[5].width = this_->tile_width;
    this_->tilesizes[5].height = this_->elev_height;
    this_->tilesizes[5].y_delta = -8;
    this_->tilesizes[6].width = this_->tile_width;
    this_->tilesizes[6].height = this_->elev_height + this_->tile_height;
    this_->tilesizes[6].y_delta = 8;
    this_->tilesizes[7].width = this_->tile_width;
    this_->tilesizes[7].height = this_->elev_height;
    this_->tilesizes[7].y_delta = -8;
    this_->tilesizes[8].width = this_->tile_width;
    this_->tilesizes[8].height = this_->elev_height + this_->tile_height;
    this_->tilesizes[8].y_delta = 8;
    this_->tilesizes[9].width = this_->tile_width;
    this_->tilesizes[9].height = this_->elev_height;
    this_->tilesizes[9].y_delta = -8;
    this_->tilesizes[10].width = this_->tile_width;
    this_->tilesizes[10].height = this_->elev_height + this_->tile_height;
    this_->tilesizes[10].y_delta = 8;
    this_->tilesizes[11].width = this_->tile_width;
    this_->tilesizes[11].height = this_->tile_height;
    this_->tilesizes[11].y_delta = -8;
    this_->tilesizes[12].width = this_->tile_width;
    this_->tilesizes[12].height = this_->tile_height;
    this_->tilesizes[12].y_delta = -8;
    this_->tilesizes[13].width = this_->tile_width;
    this_->tilesizes[13].height = this_->elev_height;
    this_->tilesizes[13].y_delta = -8;
    this_->tilesizes[14].width = this_->tile_width;
    this_->tilesizes[14].height = this_->elev_height + this_->tile_height;
    this_->tilesizes[14].y_delta = 8;
    this_->tilesizes[15].width = this_->tile_width;
    this_->tilesizes[15].height = this_->tile_height;
    this_->tilesizes[15].y_delta = 8;
    this_->tilesizes[16].width = this_->tile_width;
    this_->tilesizes[16].height = this_->tile_height;
    this_->tilesizes[16].y_delta = 8;
    RGE_Map__set_tile_size(this_, 0x11, this_->tile_half_width, this_->tile_height, 0);
    RGE_Map__set_tile_size(this_, 0x12, this_->tile_half_width, this_->tile_height, 0);
}

// Replacement for RGE_Map::clear_map
// Original: RGE_Map__clear_map @ 0x00490670 (idb_functions.h:1946)
// ABI: __thiscall — this_ in ECX, 5 stack args, retn 14h, void.
// Forwarder composition: new_map (replaced — routes through this hook),
// set_terrain (idb_functions.h:1997), reset_object_count (:6003),
// reset_player_visible_maps (:6038). All called via idb constants.
void __fastcall RGE_Map__clear_map_new(RGE_Map* this_, DWORD dummy, RGE_Player* player, RGE_Game_World* world, unsigned __int8 new_terrain, int width, int height)
{
    RGE_Map__new_map(this_, width, height);
    if (world)
        RGE_Game_World__reset_object_count(world);
    RGE_Map__set_terrain(this_, player, world, 0, 0, width - 1, height - 1, new_terrain, 1u, 0);
    if (world)
        RGE_Game_World__reset_player_visible_maps(world);
}

// Replacement for RGE_Map::data_load_random_map
// Original: RGE_Map__data_load_random_map @ 0x00490580 (idb_functions.h:1942)
// ABI: __thiscall — this_ in ECX, infile first stack argument, retn 4.
// Allocation sizeof-verified: sizeof(RGE_RMM_Database_Controller) = 22700
// = 0x58AC (recursive struct walk incl. RGE_Land/Terrain/Object/Elevation/
// Map_Data embedded tables). Ctor via idb_functions.h:3057 (0x004D8300).
// random_map field @ idb_types.h:2935.
void __fastcall RGE_Map__data_load_random_map_new(RGE_Map* this_, DWORD dummy, int infile)
{
    RGE_RMM_Database_Controller* v3 = (RGE_RMM_Database_Controller*)operator_new_internal(sizeof(RGE_RMM_Database_Controller));
    RGE_RMM_Database_Controller* v4 = 0;
    if (v3)
        v4 = RGE_RMM_Database_Controller__RGE_RMM_Database_Controller2(v3, infile);
    this_->random_map = v4;
}

// Replacement for RGE_Map::load_random_map
// Original: RGE_Map__load_random_map @ 0x004905E0 (idb_functions.h:1943)
// ABI: __thiscall — this_ in ECX, 4 file-name stack args, retn 14h.
// Allocation sizeof-verified (22700 = 0x58AC, same walk). Ctor via
// idb_functions.h:3055 (0x004D7E40) — argument order per the constant's
// parameter list: land, terrain, object, map.
void __fastcall RGE_Map__load_random_map_new(RGE_Map* this_, DWORD dummy, char* rmm_map_file, char* rmm_land_file, char* rmm_terr_file, char* rmm_obj_file)
{
    RGE_RMM_Database_Controller* v6 = (RGE_RMM_Database_Controller*)operator_new_internal(sizeof(RGE_RMM_Database_Controller));
    RGE_RMM_Database_Controller* v7 = 0;
    if (v6)
        v7 = RGE_RMM_Database_Controller__RGE_RMM_Database_Controller(v6, rmm_land_file, rmm_terr_file, rmm_obj_file, rmm_map_file);
    this_->random_map = v7;
}

void install_RGE_Map()
{
    setHook((void*)0x00490330, RGE_Map__init_tile_sizes_new);
    setHook((void*)0x00490670, RGE_Map__clear_map_new);
    setHook((void*)0x00490580, RGE_Map__data_load_random_map_new);
    setHook((void*)0x004905E0, RGE_Map__load_random_map_new);
    setHook((void*)0x00490650, RGE_Map__set_map_visible_new);
    setHook((void*)0x00490660, RGE_Map__set_map_fog_new);
}
