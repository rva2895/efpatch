#pragma once
#include "overlay.h"
#include <windows.h>

enum SpectatorLayout { SP_FULL = 0, SP_COMPACT = 1 };

// Container batches Lock/Unlock and GetDc/ReleaseDc across all cells, so
// views must NOT acquire them and must do only SLP work on SP_PASS_SLP,
// only GDI work (via da->DrawDc) on SP_PASS_GDI.
enum SpectatorPass { SP_PASS_SLP = 0, SP_PASS_GDI = 1 };

struct SpectatorViewDef {
    const char* label;
    int         full_h;
    int         compact_h;

    void (*render)(TDrawArea* da, HRGN clip,
                   TRIBE_Player* player, int player_idx,
                   int x, int y, int w,
                   SpectatorLayout layout, SpectatorPass pass);

    bool (*need_redraw)();

    // Optional: pre-collect per-player data once per frame, reused by both passes.
    void (*begin_frame)();
};

// Register the spectator container panel.
// Must be called once from DllMain, before any register_spectator_view() calls.
void register_spectator_overlay();

// Register a content view.  Safe to call any time after register_spectator_overlay().
void register_spectator_view(const SpectatorViewDef& def);

// Advance to the next registered view; triggers a panel resize.
// Wire this to whatever UI button / hotkey you add later.
void spectator_next_view();

// Player colour utilities shared by view modules.
COLORREF get_player_color(int player_idx);  // player_idx 1-8 (0 = gaia/black)
HBRUSH   get_player_brush(int player_idx);
