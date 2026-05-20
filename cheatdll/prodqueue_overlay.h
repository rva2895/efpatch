#pragma once
#include <vector>
#include "overlay.h"
#include "spectator_overlay.h"  // SpectatorPass

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

// Self-contained: acquires Lock and GetDc internally. One cell per frame only.
void draw_prodqueue_overlay(TDrawArea* da, int cell_x, int cell_w, const std::vector<PQEntry>& entries, HRGN clip_region, TRIBE_Player* player, int row_y = 0, int icon_size = 36, bool right_align = true);
void draw_techqueue_overlay(TDrawArea* da, int cell_x, int cell_w, const std::vector<TechEntry>& entries, HRGN clip_region, TRIBE_Player* player, int row_y = 0, int icon_size = 36, bool right_align = true);

// Pass-aware. Caller owns Lock (SLP) or GetDc + SetBkMode + default font (GDI),
// and (SLP only) sets the per-cell TDrawArea__SetClipRect before each cell.
void draw_prodqueue_overlay_pass(TDrawArea* da, int cell_x, int cell_w, const std::vector<PQEntry>& entries, HRGN clip_region, TRIBE_Player* player, int row_y, int icon_size, bool right_align, SpectatorPass pass);
void draw_techqueue_overlay_pass(TDrawArea* da, int cell_x, int cell_w, const std::vector<TechEntry>& entries, HRGN clip_region, TRIBE_Player* player, int row_y, int icon_size, bool right_align, SpectatorPass pass);

// Register the normal (single-player) overlay – active during live games.
void register_prodqueue_overlay();

// Register the Queue view into the spectator container.
// Call after register_spectator_overlay() in DllMain.
void register_queue_spectator_view();
