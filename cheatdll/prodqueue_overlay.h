#pragma once
#include <vector>
#include "overlay.h"

extern TShape** iconsUnitPtr;   // defined in icons.cpp; iconsUnitPtr[civ] -> unit-icon SLP

struct PQEntry
{
    __int16 master_id;
    int     total_count;
    __int16 max_progress;   // 0-100; highest active-training progress across all buildings
};

// Populate |entries| from the local player's buildings.  Returns false when nothing is queued.
bool collect_prodqueue(std::vector<PQEntry>& entries);

// Draw the production-queue strip into |da| (the overlay image buffer).
// |panel_w| is render_rect->right.  Caller must NOT have a DC or lock already acquired.
void draw_prodqueue_overlay(TDrawArea* da, int panel_w, const std::vector<PQEntry>& entries, HRGN clip_region);

// Register the prodqueue overlay with the overlay system.  Call once at startup.
void register_prodqueue_overlay();
