#pragma once
#include "overlay.h"
#include <windows.h>

enum SpectatorLayout { SP_FULL = 0, SP_COMPACT = 1 };

// Container batches Lock/Unlock and GetDc/ReleaseDc across all cells, so
// views must NOT acquire them; do only SLP work on SP_PASS_SLP, only GDI
// work (via da->DrawDc) on SP_PASS_GDI.
enum SpectatorPass { SP_PASS_SLP = 0, SP_PASS_GDI = 1 };

struct SpectatorViewDef {
    const char* label;
    int         full_h;
    int         compact_h;

    int (*render)(TDrawArea* da, HRGN clip,
                   TRIBE_Player* player,
                   int x, int y, int w,
                   SpectatorLayout layout, SpectatorPass pass,
                   void* user_data);

    bool (*need_redraw)(void* user_data);

    // Optional: pre-collect per-player data once per frame, reused by both passes.
    void (*begin_frame)(void* user_data);

    void* (*create)();
    void (*destroy)(void* user_data);
};

struct SpectatorViewInstance {
    SpectatorViewDef def;
    void* user_data;

    SpectatorViewInstance(const SpectatorViewDef& def_t, void* user_data_t) :
        def(def_t), user_data(user_data_t) {
    };
};

// Call once from DllMain before any register_spectator_view().
void register_spectator_overlay();
void register_spectator_view(const SpectatorViewDef& def);

// Nearest-palette-index lookup against the active draw_system palette, cached.
// Use for TDrawArea__FillRect calls inside the Lock pass.
unsigned __int8 pal_index(COLORREF rgb);

int sp_font_h();
