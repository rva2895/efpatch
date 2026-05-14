#pragma once
#include "overlay.h"
#include <windows.h>

// Layout mode passed to each view's render callback.
enum SpectatorLayout { SP_FULL = 0, SP_COMPACT = 1 };

// A view that can be shown in the spectator panel.
// Register views with register_spectator_view(); the container handles
// the panel lifecycle, player layout, and name/stripe header.
struct SpectatorViewDef {
    const char* label;      // Short name shown in the tab strip (e.g. "Queue")
    int         full_h;     // Content cell height (px) for SP_FULL layout
    int         compact_h;  // Content cell height (px) for SP_COMPACT layout

    // Draw one player's content cell into |da|.
    // (x, y) is the top-left of the content area (below the name header).
    // |w| is the cell width minus the colour stripe.
    void (*render)(TDrawArea* da, HRGN clip,
                   TRIBE_Player* player, int player_idx,
                   int x, int y, int w, SpectatorLayout layout);

    // Return true when game state has changed and a redraw is needed.
    bool (*need_redraw)();
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
