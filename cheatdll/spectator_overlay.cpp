#include "stdafx.h"
#include "spectator_overlay.h"
#include "rec.h"
#include "registry.h"
#include "log.h"
#include <vector>

static const int SP_FONT_ID = 22;

static const int STRIPE_W    = 4;
static const int SP_CELL_GAP = 3;
static const int TAB_H       = 18;
static const int SP_TAB_W    = 120;

// Floor sized to fit the resources view strip (~316 px) + player stripe.
static const int SP_MIN_COL_W = 330;
static int sp_col_w(int panel_w) {
    int w = (panel_w * 30) / 100;
    if (w < SP_MIN_COL_W) w = SP_MIN_COL_W;
    if (w > panel_w)      w = panel_w;
    return w;
}

static int label_h() { return cd.largeText ? 22 : 16; }

// SWGB lobby colour order, slots 1..8. Slot 0 = gaia / fallback.
static const COLORREF PLAYER_COLORS[] =
{
    RGB(  0,   0,   0),
    RGB( 36,  73, 255),
    RGB(255,   0,   0),
    RGB(  0, 204,   0),
    RGB(255, 255,   0),
    RGB(  0, 220, 220),
    RGB(220,   0, 200),
    RGB(160, 160, 160),
    RGB(220, 120,   0),
};

static const COLORREF TAB_VIEW_BG[] = {
    RGB( 30,  55, 110),
    RGB( 20,  85,  35),
    RGB( 90,  35,  90),
    RGB(100,  60,  10),
};

// color_table->get_id() returns 0..7 (the actual shown colour even with
// shuffled lobby picks). PLAYER_COLORS is 1-based
static int player_color_id(TRIBE_Player* player, int fallback_idx)
{
    if (player && player->color_table && player->color_table->vfptr
        && player->color_table->vfptr->get_id)
    {
        int id = player->color_table->vfptr->get_id(player->color_table);
        if (id >= 0 && id <= 7) return id + 1;
    }
    return (fallback_idx >= 1 && fallback_idx <= 8) ? fallback_idx : 0;
}

static COLORREF get_player_color(int idx)
{
    if (idx < 0 || idx > 8) idx = 0;
    return PLAYER_COLORS[idx];
}

struct PalCacheEntry { COLORREF rgb; unsigned __int8 idx; };
static std::vector<PalCacheEntry> s_pal_cache;

static unsigned __int8 pal_lookup_uncached(COLORREF rgb)
{
    if (!*base_game || !(*base_game)->draw_system) return 0;
    PALETTEENTRY* pal = (*base_game)->draw_system->palette;
    BYTE r = GetRValue(rgb), g = GetGValue(rgb), b = GetBValue(rgb);
    int best_i = 0, best_d = 0x7fffffff;
    for (int i = 0; i < 256; i++) {
        int dr = (int)pal[i].peRed   - (int)r;
        int dg = (int)pal[i].peGreen - (int)g;
        int db = (int)pal[i].peBlue  - (int)b;
        int d  = dr*dr + dg*dg + db*db;
        if (d < best_d) { best_d = d; best_i = i; if (d == 0) break; }
    }
    return (unsigned __int8)best_i;
}

unsigned __int8 pal_index(COLORREF rgb)
{
    for (size_t i = 0; i < s_pal_cache.size(); i++)
        if (s_pal_cache[i].rgb == rgb) return s_pal_cache[i].idx;
    PalCacheEntry e; e.rgb = rgb; e.idx = pal_lookup_uncached(rgb);
    s_pal_cache.push_back(e);
    return e.idx;
}

static unsigned __int8 pal_player(int color_id)
{
    if (color_id < 0 || color_id > 8) color_id = 0;
    return pal_index(PLAYER_COLORS[color_id]);
}

// Non-AA clone of the engine font: GDI's AA glyph cache can stall for ms
// when evicted; bitmap glyphs are consistent.
static HFONT s_noaa_font = NULL;
static HFONT s_noaa_src  = NULL;
static int   s_noaa_h    = 0;

static HFONT sp_font()
{
    RGE_Font* gf = RGE_Base_Game__get_font(*base_game, SP_FONT_ID);
    HFONT src = gf ? gf->font : NULL;
    if (!src) return (HFONT)GetStockObject(DEFAULT_GUI_FONT);

    // Rebuild if engine swapped the underlying font (e.g. cd.largeText toggle).
    if (src != s_noaa_src && s_noaa_font)
    {
        DeleteObject(s_noaa_font);
        s_noaa_font = NULL;
    }
    if (!s_noaa_font)
    {
        LOGFONTA lf = {};
        if (GetObjectA(src, sizeof(lf), &lf) > 0)
        {
            lf.lfQuality = NONANTIALIASED_QUALITY;
            s_noaa_font = CreateFontIndirectA(&lf);
            s_noaa_h    = (gf && gf->font_hgt > 0) ? gf->font_hgt : abs(lf.lfHeight);
        }
        s_noaa_src = src;
    }
    return s_noaa_font ? s_noaa_font : src;
}

int sp_font_h()
{
    sp_font();
    if (s_noaa_h > 0) return s_noaa_h;
    RGE_Font* f = RGE_Base_Game__get_font(*base_game, SP_FONT_ID);
    return (f && f->font_hgt > 0) ? f->font_hgt : 12;
}

static std::vector<SpectatorViewDef> s_views;
static int s_active = 0;
static unsigned int s_view_generation = 0;

// Wall-clock-gated throttle (need_redraw callback rate is unspecified).
// 100 ms matches the main object UI update cadence.
static const DWORD SP_REDRAW_MIN_MS = 100;

extern void __stdcall handle_overlay_size();

void register_spectator_view(const SpectatorViewDef& def)
{
    s_views.push_back(def);
}

void spectator_next_view()
{
    if ((int)s_views.size() <= 1) return;
    s_active = (s_active + 1) % (int)s_views.size();
    ++s_view_generation;
    handle_overlay_size();
}

static int active_player_count()
{
    if (!*base_game || !(*base_game)->world) return 8;
    return max(1, (int)(*base_game)->world->player_num - 1);
}

static int current_cell_h()
{
    if (s_views.empty()) return label_h();
    return label_h() + s_views[s_active].compact_h;
}

static int current_tab_h() { return TAB_H; }

static void draw_tab_bg(TDrawArea* da, int x, int w, int y, int h)
{
    if (s_views.empty() || !da) return;
    COLORREF bg_col = (s_active < (int)(sizeof(TAB_VIEW_BG) / sizeof(TAB_VIEW_BG[0])))
                      ? TAB_VIEW_BG[s_active] : RGB(40, 40, 60);
    TDrawArea__FillRect(da, x, y, x + w, y + h, pal_index(bg_col));
}

static void draw_tab_text(HDC hdc, int x, int w, int y, int h)
{
    if (s_views.empty() || !hdc) return;
    const char* label = s_views[s_active].label;
    int len = (int)strlen(label);

    SetTextColor(hdc, RGB(255, 255, 255));

    SIZE sz;
    GetTextExtentPoint32A(hdc, label, len, &sz);
    int font_h = sp_font_h();
    int tx = x + (w - sz.cx) / 2;
    int ty = y + (h - font_h) / 2;
    if (ty < y) ty = y;
    RECT clip = { x, y, x + w, y + h };
    ExtTextOutA(hdc, tx, ty, ETO_CLIPPED, &clip, label, len, NULL);
}

struct SpectatorUserData
{
    bool had_content;
    unsigned int seen_view_generation;
    DWORD last_draw_ms;
};

static void* sp_create(TRIBE_Panel_Screen_Overlay* /*panel*/, const void* /*user_init*/)
{
    s_pal_cache.clear();  // palette may differ between games
    SpectatorUserData* d = new SpectatorUserData;
    d->had_content = false;
    d->seen_view_generation = s_view_generation;
    d->last_draw_ms = 0;
    return d;
}

static void sp_destroy(TRIBE_Panel_Screen_Overlay* /*panel*/, void* user_data) { delete (SpectatorUserData*)user_data; }

static bool sp_need_redraw(TRIBE_Panel_Screen_Overlay* /*panel*/, void* user_data)
{
    if (!isRec() || s_views.empty()) return false;
    SpectatorUserData* d = (SpectatorUserData*)user_data;
    DWORD now = timeGetTime();
    if (d->seen_view_generation != s_view_generation)
    {
        d->seen_view_generation = s_view_generation;
        d->had_content = true;
        d->last_draw_ms = now;
        return true;
    }
    if (now - d->last_draw_ms < SP_REDRAW_MIN_MS) return false;

    bool any   = s_views[s_active].need_redraw();
    bool needs = any || d->had_content;
    d->had_content = any;
    if (needs) d->last_draw_ms = now;
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

static void draw_player_stripe(TDrawArea* da, int col_x, int col_w, int cell_y, int cell_h,
                                TRIBE_Player* player, int player_idx)
{
    int color_id = player_color_id(player, player_idx);
    TDrawArea__FillRect(da,
        col_x + col_w - STRIPE_W, cell_y,
        col_x + col_w,            cell_y + cell_h,
        pal_player(color_id));
}

static void draw_player_name(HDC hdc, int col_x, int col_w, int cell_y, int font_h,
                              TRIBE_Player* player, int player_idx)
{
    int color_id = player_color_id(player, player_idx);
    const char* name = (player->name && player->name[0]) ? player->name : NULL;
    char name_buf[64];
    int name_len;
    if (!name)
    {
        name_len = snprintf(name_buf, sizeof(name_buf), "Player %d", player_idx);
        name = name_buf;
    }
    else
    {
        name_len = (int)strlen(name);
        if (name_len > 63) name_len = 63;
    }

    SetTextColor(hdc, get_player_color(color_id));
    int content_w = col_w - STRIPE_W;
    int lh        = label_h();
    int text_y    = cell_y + (lh - font_h) / 2;
    if (text_y < cell_y) text_y = cell_y;

    SIZE sz;
    GetTextExtentPoint32A(hdc, name, name_len, &sz);
    int right_x = col_x + content_w - 4;
    int text_x  = right_x - sz.cx;
    int left_x  = col_x + 3;
    if (text_x < left_x) text_x = left_x;

    // Black backing fill + text in one call via ETO_OPAQUE.
    RECT bg = { text_x - 2, cell_y, text_x + sz.cx + 2, cell_y + lh };
    if (bg.left  < left_x)  bg.left  = left_x;
    if (bg.right > right_x) bg.right = right_x;
    SetBkColor(hdc, RGB(0, 0, 0));
    ExtTextOutA(hdc, text_x, text_y, ETO_OPAQUE | ETO_CLIPPED, &bg, name, name_len, NULL);
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
    int lh        = label_h();
    int cell_h    = lh + view.compact_h;
    int col_w     = sp_col_w(panel_w);
    int col_x     = panel_w - col_w;
    int cell_step = cell_h + SP_CELL_GAP;
    int content_x = col_x;
    int content_w = col_w - STRIPE_W;

    if (view.begin_frame) view.begin_frame();

    int tab_w = (col_w < SP_TAB_W) ? col_w : SP_TAB_W;
    int tab_x = col_x + col_w - tab_w;

    if (TDrawArea__Lock(render_area, "sp_slp", 1))
    {
        TDrawArea__SetClipRect(render_area, NULL);
        draw_tab_bg(render_area, tab_x, tab_w, 0, tab_h);

        for (int i = 1; i < pnum; i++)
        {
            TRIBE_Player* player = (*base_game)->world->players[i];
            if (!player) continue;
            int cell_y = tab_h + (i - 1) * cell_step;

            // Stripe spans label + content rows; view content clip is set after.
            RECT full_cell = { col_x, cell_y, col_x + col_w, cell_y + cell_h };
            TDrawArea__SetClipRect(render_area, &full_cell);
            draw_player_stripe(render_area, col_x, col_w, cell_y, cell_h, player, i);

            RECT view_cell = { content_x, cell_y + lh,
                               content_x + content_w, cell_y + cell_h };
            TDrawArea__SetClipRect(render_area, &view_cell);
            view.render(render_area, clip_region, player, i,
                        content_x, cell_y + lh, content_w,
                        SP_COMPACT, SP_PASS_SLP);
        }
        TDrawArea__SetClipRect(render_area, NULL);
        TDrawArea__Unlock(render_area, "sp_slp");
    }

    if (TDrawArea__GetDc(render_area, "sp_gdi"))
    {
        HDC hdc = render_area->DrawDc;
        // Views/headers must leave clip rgn and font alone — they're set once here.
        SetBkMode(hdc, TRANSPARENT);
        SelectClipRgn(hdc, clip_region);
        HGDIOBJ old_font = SelectObject(hdc, sp_font());

        draw_tab_text(hdc, tab_x, tab_w, 0, tab_h);

        int font_h = sp_font_h();
        for (int i = 1; i < pnum; i++)
        {
            TRIBE_Player* player = (*base_game)->world->players[i];
            if (!player) continue;
            int cell_y = tab_h + (i - 1) * cell_step;

            draw_player_name(hdc, col_x, col_w, cell_y, font_h, player, i);

            view.render(render_area, clip_region, player, i,
                        content_x, cell_y + lh, content_w,
                        SP_COMPACT, SP_PASS_GDI);
        }

        SelectObject(hdc, old_font);
        SelectClipRgn(hdc, 0);
        TDrawArea__ReleaseDc(render_area, "sp_gdi");
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
