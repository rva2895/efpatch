#include "stdafx.h"

// Replacement for RGE_Visible_Map::get_visible
// Original: RGE_Visible_Map__get_visible @ 0x00615430 (idb_functions.h:5942)
// ABI: __thiscall — this_ in ECX, map_col/map_row stack args, retn 8, int in EAX.
// Grounding: unified_map_offsets = project constant (test.cpp:1386, .data
// :007A1CBC in the lst — declared extern here); player @+8 (RGE_Visible_Map
// @idb_types.h:1318); mutualVisibleMask/mutualExploredMask @+0x130/+0x12C
// (RGE_Player, asm-verified). Semantics: mutualVisibleMask bit set → 15
// (fully visible); explored bit set → 128; else 0.
extern int** const unified_map_offsets;

int __fastcall RGE_Visible_Map__get_visible_new(RGE_Visible_Map* this_, DWORD dummy, int map_col, int map_row)
{
    const int visibility = unified_map_offsets[map_row][map_col];  // MAP_SIZE
    if (this_->player->mutualVisibleMask & visibility)
        return 15;
    return (visibility & this_->player->mutualExploredMask) != 0 ? 128 : 0;
}

void install_RGE_Visible_Map()
{
    setHook((void*)0x00615430, RGE_Visible_Map__get_visible_new);
}
