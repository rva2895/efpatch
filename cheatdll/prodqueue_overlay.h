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
    __int16 progress;   // 0-100
    __int16 icon;       // frame index into iconsTechPtr SLP
};

// Populate |entries| from a specific player's production queues.
bool collect_prodqueue_for_player(TRIBE_Player* player, std::vector<PQEntry>& entries);

// Populate |entries| from a specific player's active tech research.
bool collect_techqueue_for_player(TRIBE_Player* player, std::vector<TechEntry>& entries);

// Convenience wrappers that use the local/observed player.
bool collect_prodqueue(std::vector<PQEntry>& entries);
bool collect_techqueue(std::vector<TechEntry>& entries);

// Draw the production-queue unit strip into |da|.
// cell_x/cell_w define the horizontal cell bounds.
// right_align=true  → icons flush-right (normal overlay style)
// right_align=false → icons flush-left  (spectator style)
void draw_prodqueue_overlay(TDrawArea* da, int cell_x, int cell_w, const std::vector<PQEntry>& entries, HRGN clip_region, TRIBE_Player* player, int row_y = 0, int icon_size = 36, bool right_align = true);

// Draw the tech-research strip into |da| within the same cell bounds.
void draw_techqueue_overlay(TDrawArea* da, int cell_x, int cell_w, const std::vector<TechEntry>& entries, HRGN clip_region, TRIBE_Player* player, int row_y = 0, int icon_size = 36, bool right_align = true);

// Register the normal (single-player) overlay – active during live games.
void register_prodqueue_overlay();

// Register the Queue view into the spectator container.
// Call after register_spectator_overlay() in DllMain.
void register_queue_spectator_view();
