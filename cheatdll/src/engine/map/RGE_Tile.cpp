#include "stdafx.h"

// Replacement for RGE_Tile::has_enemy_detector
// Original: RGE_Tile__has_enemy_detector @ 0x0048F810 (idb_functions.h:1933)
// ABI: __thiscall — this_ in ECX, player_id first stack argument, retn 4, u8 in AL.
// place_holder3 @ idb_types.h:4187; mask via get_player_by_id->gbg_unknown_mask
// (idb_functions.h:690, RGE_Base_Game::gbg_unknown_mask @idb_types.h:1699);
// ally scan via numberPlayers (:752) + isAlly vtable slot (idb_types.h:7128).
// no null-check on the first get_player_by_id result — faithful to the original.
unsigned __int8 __fastcall RGE_Tile__has_enemy_detector_new(RGE_Tile* this_, DWORD dummy, unsigned __int8 player_id)
{
    if (this_->place_holder3 && player_id)
    {
        unsigned __int8 mask = RGE_Base_Game__get_player_by_id(*base_game, player_id)->gbg_unknown_mask;
        if (mask)
        {
            if (((unsigned __int8)~mask & this_->place_holder3) != 0)
                return 1;
        }
        else
        {
            for (int i = 1; i <= RGE_Base_Game__numberPlayers(*base_game); ++i)
            {
                RGE_Player* p = RGE_Base_Game__get_player_by_id(*base_game, i);
                if (p)
                {
                    if (p->vfptr->isAlly(p, player_id))
                        mask |= 1 << (i - 1);
                }
            }
            if (((unsigned __int8)~mask & this_->place_holder3) != 0)
                return 1;
        }
    }
    return 0;
}

// Replacement for RGE_Tile::is_detector — same structure, ally-inclusive:
// returns (mask & place_holder3) != 0.
unsigned __int8 __fastcall RGE_Tile__is_detector_new(RGE_Tile* this_, DWORD dummy, unsigned __int8 player_id)
{
    if (this_->place_holder3 && player_id)
    {
        unsigned __int8 mask = RGE_Base_Game__get_player_by_id(*base_game, player_id)->gbg_unknown_mask;
        if (mask)
        {
            if ((mask & this_->place_holder3) != 0)
                return 1;
        }
        else
        {
            for (int i = 1; i <= RGE_Base_Game__numberPlayers(*base_game); ++i)
            {
                RGE_Player* p = RGE_Base_Game__get_player_by_id(*base_game, i);
                if (p)
                {
                    if (p->vfptr->isAlly(p, player_id))
                        mask |= 1 << (i - 1);
                }
            }
            if ((mask & this_->place_holder3) != 0)
                return 1;
        }
    }
    return 0;
}

// Replacement for RGE_Tile::is_powered
// Original: RGE_Tile__is_powered @ 0x0048F970 (idb_functions.h:1935)
// ABI: __thiscall — this_ in ECX, player_id/flag stack args, retn 8, u8 in AL.
// self bit-test (1 << (player_id-1)), ally fallback gated by flag.
unsigned __int8 __fastcall RGE_Tile__is_powered_new(RGE_Tile* this_, DWORD dummy, unsigned __int8 player_id, unsigned __int8 flag)
{
    unsigned __int8 mask = this_->place_holder1;
    if (mask && player_id)
    {
        if (((unsigned __int8)(1 << (player_id - 1)) & mask) != 0)
            return 1;
        if (flag)
        {
            mask = RGE_Base_Game__get_player_by_id(*base_game, player_id)->gbg_unknown_mask;
            if (mask)
            {
                if ((mask & this_->place_holder1) != 0)
                    return 1;
            }
            else
            {
                for (int i = 1; i <= RGE_Base_Game__numberPlayers(*base_game); ++i)
                {
                    RGE_Player* p = RGE_Base_Game__get_player_by_id(*base_game, i);
                    if (p)
                    {
                        if (p->vfptr->isAlly(p, player_id))
                            mask |= 1 << (i - 1);
                    }
                }
                if ((mask & this_->place_holder1) != 0)
                    return 1;
            }
        }
    }
    return 0;
}

// Replacement for RGE_Tile::is_shielded — is_detector shape with place_holder2.
unsigned __int8 __fastcall RGE_Tile__is_shielded_new(RGE_Tile* this_, DWORD dummy, unsigned __int8 player_id)
{
    if (this_->place_holder2 && player_id)
    {
        unsigned __int8 mask = RGE_Base_Game__get_player_by_id(*base_game, player_id)->gbg_unknown_mask;
        if (mask)
        {
            if ((mask & this_->place_holder2) != 0)
                return 1;
        }
        else
        {
            for (int i = 1; i <= RGE_Base_Game__numberPlayers(*base_game); ++i)
            {
                RGE_Player* p = RGE_Base_Game__get_player_by_id(*base_game, i);
                if (p)
                {
                    if (p->vfptr->isAlly(p, player_id))
                        mask |= 1 << (i - 1);
                }
            }
            if ((mask & this_->place_holder2) != 0)
                return 1;
        }
    }
    return 0;
}

void install_RGE_Tile()
{
    setHook((void*)0x0048F810, RGE_Tile__has_enemy_detector_new);
    setHook((void*)0x0048F8C0, RGE_Tile__is_detector_new);
    setHook((void*)0x0048F970, RGE_Tile__is_powered_new);
    setHook((void*)0x0048FA40, RGE_Tile__is_shielded_new);
}
