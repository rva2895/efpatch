#include "stdafx.h"
#include "spectator_overlay.h"
#include "rec.h"
#include <vector>

// ---------------------------------------------------------------------------
// Container-level layout constants
// ---------------------------------------------------------------------------
static const int STRIPE_W         = 4;    // player-colour stripe at right edge of cell
static const int LABEL_H          = 16;   // player name header height
static const int SP_CELL_GAP      = 3;    // vertical gap between player cells
static const int TAB_H            = 18;   // tab-strip height (always shown)
static const int SP_MAX_COL_W     = 280;  // max panel width – prevents spreading across full screen

// ---------------------------------------------------------------------------
// Standard SWGB player colours (index 0 = Gaia, 1-8 = players)
// ---------------------------------------------------------------------------
static const COLORREF PLAYER_COLORS[] =
{
    RGB(  0,   0,   0), // 0 gaia
    RGB( 36,  73, 255), // 1 blue
    RGB(255,   0,   0), // 2 red
    RGB(  0, 204,   0), // 3 green
    RGB(255, 255,   0), // 4 yellow
    RGB(  0, 220, 220), // 5 teal
    RGB(220, 120,   0), // 6 orange
    RGB(180,   0, 220), // 7 purple
    RGB(160, 160, 160), // 8 grey
};

static HBRUSH s_br_player[9] = {};

// Bold font – created once, matches DEFAULT_GUI_FONT size but FW_BOLD
static HFONT s_bold_font = NULL;
static HFONT get_bold_font()
{
    if (!s_bold_font)
        s_bold_font = CreateFontA(
            -11, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");
    return s_bold_font;
}

// One background colour per registered view – cycling changes which is shown
static const COLORREF TAB_VIEW_BG[] = {
    RGB( 30,  55, 110),  // Production – navy blue
    RGB( 20,  85,  35),  // Resources  – forest green
    RGB( 90,  35,  90),  // (future)   – purple
    RGB(100,  60,  10),  // (future)   – orange
};

COLORREF get_player_color(int idx)
{
    if (idx < 0 || idx > 8) idx = 0;
    return PLAYER_COLORS[idx];
}

HBRUSH get_player_brush(int idx)
{
    if (idx < 0 || idx > 8) idx = 0;
    if (!s_br_player[idx])
        s_br_player[idx] = CreateSolidBrush(PLAYER_COLORS[idx]);
    return s_br_player[idx];
}

// ---------------------------------------------------------------------------
// View registry
// ---------------------------------------------------------------------------
static std::vector<SpectatorViewDef> s_views;
static int s_active = 0;

// Declared in overlay.cpp; triggers panel resize for all registered overlays.
extern void __stdcall handle_overlay_size();

void register_spectator_view(const SpectatorViewDef& def)
{
    s_views.push_back(def);
}

void spectator_next_view()
{
    if ((int)s_views.size() <= 1) return;
    s_active = (s_active + 1) % (int)s_views.size();
    handle_overlay_size();
}

// ---------------------------------------------------------------------------
// Layout helpers
// ---------------------------------------------------------------------------
static int active_player_count()
{
    if (!*base_game || !(*base_game)->world) return 8;
    return max(1, (int)(*base_game)->world->player_num - 1);
}

static int current_cell_h()
{
    if (s_views.empty()) return LABEL_H;
    return LABEL_H + s_views[s_active].compact_h;  // always compact
}

static int current_tab_h() { return TAB_H; }  // always shown

// ---------------------------------------------------------------------------
// Tab strip
// ---------------------------------------------------------------------------
// x = left edge of the content column, w = column width
static void draw_tab_strip(TDrawArea* da, int x, int w, int y, int h)
{
    if (s_views.empty() || !TDrawArea__GetDc(da, "sp_tab")) return;

    HDC hdc = da->DrawDc;

    // Single tab: background colour is unique to the active view
    COLORREF bg_col = (s_active < (int)(sizeof(TAB_VIEW_BG) / sizeof(TAB_VIEW_BG[0])))
                      ? TAB_VIEW_BG[s_active] : RGB(40, 40, 60);
    HBRUSH bg = CreateSolidBrush(bg_col);
    RECT rtab = { x, y, x + w, y + h };
    FillRect(hdc, &rtab, bg);
    DeleteObject(bg);

    SetBkMode(hdc, TRANSPARENT);
    HGDIOBJ old_font = SelectObject(hdc, get_bold_font());
    SetTextColor(hdc, RGB(255, 255, 255));
    DrawTextA(hdc, s_views[s_active].label, -1, &rtab, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old_font);

    TDrawArea__ReleaseDc(da, "sp_tab");
}

// ---------------------------------------------------------------------------
// Overlay callbacks
// ---------------------------------------------------------------------------
struct SpectatorUserData { bool had_content; };

static void* sp_create(TRIBE_Panel_Screen_Overlay* /*panel*/, const void* /*user_init*/)
{
    SpectatorUserData* d = new SpectatorUserData;
    d->had_content = false;
    return d;
}

static void sp_destroy(TRIBE_Panel_Screen_Overlay* /*panel*/, void* user_data) { delete (SpectatorUserData*)user_data; }

static bool sp_need_redraw(TRIBE_Panel_Screen_Overlay* /*panel*/, void* user_data)
{
    if (!isRec() || s_views.empty()) return false;
    SpectatorUserData* d = (SpectatorUserData*)user_data;
    bool any   = s_views[s_active].need_redraw();
    bool needs = any || d->had_content;
    d->had_content = any;
    return needs;
}

static panel_size sp_handle_size(TRIBE_Panel_Screen_Overlay* /*panel*/, void* /*user_data*/)
{
    int active = active_player_count();
    int h      = current_tab_h() + active * (current_cell_h() + SP_CELL_GAP);

    panel_size s;
    s.left_border_in = s.right_border_in = s.bottom_border_in = 0;
    s.top_border_in  = 2;
    s.min_wid_in     = 200;
    s.max_wid_in     = 10000;
    s.min_hgt_in     = s.max_hgt_in = h;
    return s;
}

static RECT sp_render_to_image_buffer(TRIBE_Panel_Screen_Overlay* /*panel*/, void* /*user_data*/,
                                       TDrawArea* render_area, RECT* render_rect, HRGN clip_region)
{
    if (!isRec() || s_views.empty()) return *render_rect;
    if (!*base_game || !(*base_game)->world)  return *render_rect;

    int panel_w = render_rect->right;
    int pnum    = (*base_game)->world->player_num;
    int active  = pnum - 1;
    if (active <= 0) return *render_rect;

    const SpectatorViewDef& view = s_views[s_active];
    int tab_h     = current_tab_h();
    int cell_h    = LABEL_H + view.compact_h;
    int col_w     = min(panel_w, SP_MAX_COL_W);
    int col_x     = panel_w - col_w;  // right-align: push content to right edge
    int cell_step = cell_h + SP_CELL_GAP;

    draw_tab_strip(render_area, col_x, col_w, 0, tab_h);

    for (int i = 1; i < pnum; i++)
    {
        TRIBE_Player* player = (*base_game)->world->players[i];
        if (!player) continue;

        int cell_y = tab_h + (i - 1) * cell_step;

        // Stripe on RIGHT edge; content fills left portion
        int content_x = col_x;
        int content_w = col_w - STRIPE_W;

        // Header: player name label + colour stripe on right edge
        if (TDrawArea__GetDc(render_area, "sp_hdr"))
        {
            HDC hdc = render_area->DrawDc;

            RECT rstripe = { col_x + col_w - STRIPE_W, cell_y, col_x + col_w, cell_y + cell_h };
            FillRect(hdc, &rstripe, get_player_brush(i));

            const char* name = (player->name && player->name[0]) ? player->name : NULL;
            char name_buf[64];
            if (!name) { snprintf(name_buf, sizeof(name_buf), "Player %d", i); name = name_buf; }

            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, get_player_color(i));
            HGDIOBJ old_font = SelectObject(hdc, get_bold_font());
            RECT rname = { content_x + 3, cell_y, col_x + content_w - 2, cell_y + LABEL_H };
            DrawTextA(hdc, name, -1, &rname,
                      DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            SelectObject(hdc, old_font);

            TDrawArea__ReleaseDc(render_area, "sp_hdr");
        }

        // Content area handed to the active view
        view.render(render_area, clip_region, player, i,
                    content_x, cell_y + LABEL_H, content_w, SP_COMPACT);
    }

    return *render_rect;
}

static void sp_handle_hotkey(TRIBE_Panel_Screen_Overlay* panel, void* /*user_data*/, int hotkey)
{
    switch (hotkey)
    {
    case 0x63:  // F8 – toggle spectator overlay on/off
        panel->vfptr->set_active((TPanel*)panel, panel->active ? 0 : 1);
        break;
    case 0x64:  // Alt+Q – cycle to next view
        spectator_next_view();
        break;
    }
}

void register_spectator_overlay()
{
    TRIBE_Panel_Screen_Overlay_User_Callbacks cb = {};
    cb.render_to_image_buffer = sp_render_to_image_buffer;
    cb.need_redraw            = sp_need_redraw;
    cb.handle_size            = sp_handle_size;
    cb.handle_hotkey          = sp_handle_hotkey;
    cb.create                 = sp_create;
    cb.destroy                = sp_destroy;
    register_screen_overlay(cb, NULL);
}
