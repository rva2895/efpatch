#pragma once
#include <vector>
#include "overlay.h"

extern TShape** iconsUnitPtr;   // defined in icons.cpp; iconsUnitPtr[civ] -> unit-icon SLP
extern TShape** iconsTechPtr;   // defined in icons.cpp; iconsTechPtr[civ] -> tech-icon SLP

struct PQEntry
{
    __int16 master_id;
    int     total_count;
    __int16 max_progress;   // 0-100; highest active-training progress across all buildings
};

struct TechEntry
{
    __int16 tech_id;
    __int16 progress;
    __int16 icon;       // frame index into iconsTechPtr SLP
};

bool collect_prodqueue_for_player(TRIBE_Player* player, std::vector<PQEntry>& entries);
bool collect_techqueue_for_player(TRIBE_Player* player, std::vector<TechEntry>& entries);

// Convenience wrappers that use the local/observed player.
bool collect_prodqueue(std::vector<PQEntry>& entries);
bool collect_techqueue(std::vector<TechEntry>& entries);

// Icons are right-aligned within the cell. icon_size: 36 = normal, 24 = compact-small.
void draw_prodqueue_overlay(TDrawArea* da, int cell_x, int cell_w, const std::vector<PQEntry>& entries, HRGN clip_region, TRIBE_Player* player, int row_y = 0, int icon_size = 36);
void draw_techqueue_overlay(TDrawArea* da, int cell_x, int cell_w, const std::vector<TechEntry>& entries, HRGN clip_region, TRIBE_Player* player, int row_y = 0, int icon_size = 36);

// Active during live games only.
void register_prodqueue_overlay();

// Active during replays. Shows all players stacked; layout adapts to screen height.
void register_spectator_overlay();
