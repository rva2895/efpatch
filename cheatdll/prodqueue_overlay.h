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

// Populate |entries| from the local player's production queues.  Returns false when nothing queued.
bool collect_prodqueue(std::vector<PQEntry>& entries);

// Populate |entries| from the local player's active tech research.  Returns false when nothing.
bool collect_techqueue(std::vector<TechEntry>& entries);

// Draw the production-queue unit strip into |da| (the overlay image buffer).
// |panel_w| is render_rect->right.  Caller must NOT have a DC or lock already acquired.
void draw_prodqueue_overlay(TDrawArea* da, int panel_w, const std::vector<PQEntry>& entries, HRGN clip_region);

// Draw the tech-research strip into |da| at the given y offset.
void draw_techqueue_overlay(TDrawArea* da, int panel_w, const std::vector<TechEntry>& entries, HRGN clip_region, int row_y);

// Register the prodqueue overlay with the overlay system.  Call once at startup.
void register_prodqueue_overlay();
