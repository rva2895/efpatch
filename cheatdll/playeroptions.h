#pragma once

#define NUM_SAVED_PLAYER_LOC 8

struct saved_view_loc
{
    bool set;
    float x;
    float y;
};

struct PLAYER_EXTRA
{
    saved_view_loc view_loc[NUM_SAVED_PLAYER_LOC];
};

void setPlayerOptionsHooks();

PLAYER_EXTRA* __fastcall get_player_extra(RGE_Player* player);
