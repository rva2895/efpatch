#include "stdafx.h"
#include "spectator_overlay.h"
#include "rec.h"
#include "registry.h"
#include "log.h"
#include <vector>

static const int SP_FONT_ID = 11;

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

static int label_h() { return cd.largeText ? 22 : 18; }

// SWGB lobby colour order, slots 1..8. Slot 0 = gaia / fallback.
static const COLORREF PLAYER_COLORS[] =
{
    RGB(  0,   0,   0),
    RGB(110, 166, 235),
    RGB(255, 100, 100),
    RGB(  0, 255,   0),
    RGB(255, 255,   0),
    RGB(  0, 255, 255),
    RGB(241, 108, 232),
    RGB(223, 223, 223),
    RGB(255, 180,  21),
};

static const COLORREF TAB_VIEW_BG[] = {
    RGB( 30,  55, 110),
    RGB( 20,  85,  35),
    RGB( 90,  35,  90),
    RGB(100,  60,  10),
};

static COLORREF get_player_color(int idx)
{
    if (idx < 0 || idx > 8) idx = 0;
    return PLAYER_COLORS[idx];
}

struct PalCacheEntry { COLORREF rgb; unsigned __int8 idx; };
static std::vector<PalCacheEntry> s_pal_cache;

/*
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
*/

unsigned __int8 pal_index(COLORREF rgb)
{
    for (size_t i = 0; i < s_pal_cache.size(); i++)
        if (s_pal_cache[i].rgb == rgb) return s_pal_cache[i].idx;
    PalCacheEntry e; e.rgb = rgb; e.idx = GetNearestPaletteIndex((*base_game)->draw_system->Pal, rgb); //pal_lookup_uncached(rgb);
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
//static int s_active = 0;
//static unsigned int s_view_generation = 0;

// Wall-clock-gated throttle (need_redraw callback rate is unspecified).
// 100 ms matches the main object UI update cadence.
static const DWORD SP_REDRAW_MIN_MS = 100;

struct SpectatorUserData
{
    bool had_content;
    bool view_changed;
    DWORD last_draw_ms;
    TDrawArea* player_name_draw_area[9];
    std::vector<SpectatorViewInstance> views;
    int s_active;
};

void register_spectator_view(const SpectatorViewDef& def)
{
    s_views.push_back(def);
}

void spectator_next_view(SpectatorUserData* sp_user_data)
{
    if ((int)sp_user_data->views.size() <= 1) return;
    sp_user_data->s_active = (sp_user_data->s_active + 1) % (int)sp_user_data->views.size();
    sp_user_data->view_changed = true;
}

static int active_player_count()
{
    if (!*base_game || !(*base_game)->world) return 8;
    return max(1, (int)(*base_game)->world->player_num - 1);
}

static int current_cell_h(SpectatorUserData* sp_user_data)
{
    if (sp_user_data->views.empty()) return label_h();
    return label_h() + sp_user_data->views[sp_user_data->s_active].def.compact_h;
}

static int current_tab_h() { return TAB_H; }

static void draw_tab_bg(TDrawArea* da, int x, int w, int y, int h, SpectatorUserData* sp_user_data)
{
    if (sp_user_data->views.empty() || !da) return;
    COLORREF bg_col = (sp_user_data->s_active < (int)(sizeof(TAB_VIEW_BG) / sizeof(TAB_VIEW_BG[0])))
                      ? TAB_VIEW_BG[sp_user_data->s_active] : RGB(40, 40, 60);
    TDrawArea__FillRect(da, x, y, x + w, y + h, pal_index(bg_col));
}

static void draw_tab_text(HDC hdc, int x, int w, int y, int h, SpectatorUserData* sp_user_data)
{
    if (sp_user_data->views.empty() || !hdc) return;
    const char* label = sp_user_data->views[sp_user_data->s_active].def.label;
    int len = (int)strlen(label);

    SetTextColor(hdc, RGB(255, 255, 255));

    SIZE sz;
    GetTextExtentPoint32A(hdc, label, len, &sz);
    int font_h = sp_font_h();
    int tx = x + (w - sz.cx) / 2;
    int ty = y + (h - font_h) / 2;
    if (ty < y) ty = y;
    //RECT clip = { x, y, x + w, y + h };
    //ExtTextOutA(hdc, tx, ty, ETO_CLIPPED, &clip, label, len, NULL);
    TextOutA(hdc, tx, ty, label, len);
}

void TextOut_outline(HDC hdc, int x, int y, LPCSTR lpString, int c, COLORREF color)
{
    SetTextColor(hdc, RGB(0, 0, 0));
    TextOutA(hdc, x,     y    , lpString, c);
    TextOutA(hdc, x + 2, y    , lpString, c);
    TextOutA(hdc, x    , y + 2, lpString, c);
    TextOutA(hdc, x + 2, y + 2, lpString, c);
    SetTextColor(hdc, color);
    TextOutA(hdc, x + 1, y + 1, lpString, c);
}

static void render_player_name(TDrawArea* render_area, int id, const char* name)
{
    TRIBE_Player* player = (*base_game)->world->players[id];
    int color_id = player->color_table->vfptr->get_id(player->color_table) + 1;

    if (TDrawArea__GetDc(render_area, "sp_player_name"))
    {
        SetBkMode(render_area->DrawDc, TRANSPARENT);
        HGDIOBJ old_font = SelectObject(render_area->DrawDc, sp_font());
        UINT prev_align = SetTextAlign(render_area->DrawDc, TA_RIGHT);
        TextOut_outline(render_area->DrawDc, render_area->Width-8, 2, name, strlen(name), get_player_color(color_id));
        SetTextAlign(render_area->DrawDc, prev_align);
        SelectObject(render_area->DrawDc, old_font);

        TDrawArea__ReleaseDc(render_area, "sp_player_name");
    }
}

static int get_player_name_and_calc_width(int id, char* name_buf, size_t name_buf_size, TDrawArea* render_area)
{
    TRIBE_Player* player = (*base_game)->world->players[id];
    const char* name = (player->name && player->name[0]) ? player->name : NULL;
    int name_len;
    if (!name)
        snprintf(name_buf, name_buf_size, "Player %d", player->id);
    else
        strlcpy(name_buf, name, name_buf_size);

    if (TDrawArea__GetDc(render_area, "sp_player_name_parent"))
    {
        SIZE sz;
        GetTextExtentPoint32A(render_area->DrawDc, name_buf, strlen(name_buf), &sz);

        TDrawArea__ReleaseDc(render_area, "sp_player_name_parent");

        return sz.cx;
    }

    return 0;
}

static TDrawArea* create_player_name_draw_area(int id, TRIBE_Panel_Screen_Overlay* parent_panel)
{
    TDrawArea* render_area = (TDrawArea*)operator_new_internal(sizeof(TDrawArea));

    char b[0x40];
    snprintf(b, _countof(b), "Overlay Player Name %d", id);
    TDrawArea__TDrawArea(render_area, b, 0);
    TDrawArea__Init(render_area, parent_panel->render_area->DrawSystem, get_player_name_and_calc_width(id, b, _countof(b), parent_panel->render_area), cd.largeText ? 22 : 18, 0, 0);
    TDrawArea__PtrClear(render_area, NULL, 1);
    TDrawArea__SetTrans(render_area, 1, 1);
    render_player_name(render_area, id, b);
    return render_area;
}

static void draw_player_name(TDrawArea* dst, TDrawArea* src, int x, int cx, int y)
{
    RECT r = { src->Width - cx, 0, src->Width, src->Height };
    TDrawArea__Copy(src, dst, x, y, &r, 1);
}

static int create_player_name_draw_areas(TRIBE_Panel_Screen_Overlay* panel, SpectatorUserData* sp_user_data)
{
    int max_width = 0;
    for (int i = 1; i < (*base_game)->world->player_num; i++)
    {
        if (!sp_user_data->player_name_draw_area[i])
        {
            if ((*base_game)->world->players[i])
                sp_user_data->player_name_draw_area[i] = create_player_name_draw_area(i, panel);
            else
                sp_user_data->player_name_draw_area[i] = NULL;
        }
        if (sp_user_data->player_name_draw_area[i] && sp_user_data->player_name_draw_area[i]->Width > max_width)
            max_width = sp_user_data->player_name_draw_area[i]->Width;
    }
    return max_width;
}

static void delete_player_name_draw_areas(SpectatorUserData* sp_user_data)
{
    for (int i = 1; i < (*base_game)->world->player_num; i++)
    {
        if (sp_user_data->player_name_draw_area[i])
        {
            TDrawArea__destructor(sp_user_data->player_name_draw_area[i]);
            operator_delete_internal(sp_user_data->player_name_draw_area[i]);
            sp_user_data->player_name_draw_area[i] = NULL;
        }
    }
}

static void* sp_create(TRIBE_Panel_Screen_Overlay* panel, const void* /*user_init*/)
{
    s_pal_cache.clear();  // palette may differ between games
    SpectatorUserData* d = new SpectatorUserData;
    d->had_content = false;
    d->view_changed = true;
    d->s_active = 0;
    d->last_draw_ms = 0;
    memset(d->player_name_draw_area, 0, sizeof(d->player_name_draw_area));

    for (auto it = s_views.begin(); it != s_views.end(); ++it)
    {
        d->views.emplace_back(*it, it->create ? it->create() : NULL);
    }

    return d;
}

static void sp_destroy(TRIBE_Panel_Screen_Overlay* /*panel*/, void* user_data)
{
    SpectatorUserData* sp_user_data = (SpectatorUserData*)user_data;
    delete_player_name_draw_areas(sp_user_data);

    for (auto it = sp_user_data->views.begin(); it != sp_user_data->views.end(); ++it)
    {
        if (it->def.destroy) it->def.destroy(it->user_data);
    }

    delete (SpectatorUserData*)user_data;
}

static bool sp_need_redraw(TRIBE_Panel_Screen_Overlay* /*panel*/, void* user_data)
{
    SpectatorUserData* d = (SpectatorUserData*)user_data;

    if (!isRec() || d->views.empty()) return false;
    DWORD now = timeGetTime();
    if (d->view_changed)
    {
        d->view_changed = false;
        d->had_content = true;
        d->last_draw_ms = now;
        return true;
    }
    if (now - d->last_draw_ms < SP_REDRAW_MIN_MS) return false;

    bool any   = d->views[d->s_active].def.need_redraw(d->views[d->s_active].user_data);
    bool needs = any || d->had_content;
    d->had_content = any;
    if (needs) d->last_draw_ms = now;
    return needs;
}

static panel_size sp_handle_size(TRIBE_Panel_Screen_Overlay* panel, void* user_data)
{
    SpectatorUserData* sp_user_data = (SpectatorUserData*)user_data;

    delete_player_name_draw_areas(sp_user_data);

    int active = active_player_count();
    int h      = current_tab_h() + active * (current_cell_h(sp_user_data) + SP_CELL_GAP);

    panel_size s;
    s.left_border_in = s.right_border_in = s.bottom_border_in = 0;
    s.top_border_in  = 2;
    s.min_wid_in     = 200;
    s.max_wid_in     = 10000;
    s.min_hgt_in     = s.max_hgt_in = h;
    return s;
}

static void draw_player_stripe(TDrawArea* da, int col_x, int col_w, int cell_y, int cell_h,
                                TRIBE_Player* player)
{
    int color_id = player->color_table->vfptr->get_id(player->color_table) + 1;
    TDrawArea__FillRect(da,
        col_x + col_w - STRIPE_W, cell_y,
        col_x + col_w,            cell_y + cell_h,
        pal_player(color_id));
}

/*
static void draw_player_name_old(HDC hdc, int col_x, int col_w, int cell_y, int font_h,
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
    //ExtTextOutA(hdc, text_x, text_y, ETO_OPAQUE | ETO_CLIPPED, &bg, name, name_len, NULL);
    TextOutA(hdc, text_x, text_y, name, name_len);
}
*/

static RECT sp_render_to_image_buffer(TRIBE_Panel_Screen_Overlay* panel, void* user_data,
                                       TDrawArea* render_area, RECT* render_rect, HRGN clip_region)
{
    SpectatorUserData* sp_user_data = (SpectatorUserData*)user_data;

    RECT used_render_rect = {};

    if (!isRec() || sp_user_data->views.empty()) return used_render_rect;
    if (!*base_game || !(*base_game)->world)  return used_render_rect;

    int panel_w = render_rect->right;
    int pnum    = (*base_game)->world->player_num;
    int active  = pnum - 1;
    if (active <= 0) return used_render_rect;

    const SpectatorViewInstance& view = sp_user_data->views[sp_user_data->s_active];
    int tab_h     = current_tab_h();
    int lh        = label_h();
    int cell_h    = lh + view.def.compact_h;
    int col_w     = sp_col_w(panel_w);
    int col_x     = panel_w - col_w;
    int cell_step = cell_h + SP_CELL_GAP;
    int content_x = col_x;
    int content_w = col_w - STRIPE_W;

    if (view.def.begin_frame) view.def.begin_frame(view.user_data);

    int tab_w = (col_w < SP_TAB_W) ? col_w : SP_TAB_W;
    int tab_x = col_x + col_w - tab_w;

    int render_w = max(SP_TAB_W, create_player_name_draw_areas(panel, sp_user_data));

    // TDrawArea::Copy can only be used on an unlocked surface
    for (int i = 1; i < pnum; i++)
    {
        int cell_y = tab_h + (i - 1) * cell_step;
        draw_player_name(render_area, sp_user_data->player_name_draw_area[i], col_x, col_w, cell_y);
    }

    if (TDrawArea__Lock(render_area, "sp_slp", 1))
    {
        TDrawArea__SetClipRect(render_area, NULL);
        draw_tab_bg(render_area, tab_x, tab_w, 0, tab_h, sp_user_data);

        for (int i = 1; i < pnum; i++)
        {
            TRIBE_Player* player = (*base_game)->world->players[i];
            if (!player) continue;
            int cell_y = tab_h + (i - 1) * cell_step;

            // Stripe spans label + content rows; view content clip is set after.
            RECT full_cell = { col_x, cell_y, col_x + col_w, cell_y + cell_h };
            TDrawArea__SetClipRect(render_area, &full_cell);
            draw_player_stripe(render_area, col_x, col_w, cell_y, cell_h, player);

            RECT view_cell = { content_x, cell_y + lh,
                               content_x + content_w, cell_y + cell_h };
            TDrawArea__SetClipRect(render_area, &view_cell);
            render_w = max(render_w,
                view.def.render(render_area, clip_region, player,
                        content_x, cell_y + lh, content_w,
                        SP_COMPACT, SP_PASS_SLP, view.user_data));
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

        draw_tab_text(hdc, tab_x, tab_w, 0, tab_h, sp_user_data);

        int font_h = sp_font_h();
        for (int i = 1; i < pnum; i++)
        {
            TRIBE_Player* player = (*base_game)->world->players[i];
            if (!player) continue;
            int cell_y = tab_h + (i - 1) * cell_step;

            render_w = max(render_w,
                view.def.render(render_area, clip_region, player,
                        content_x, cell_y + lh, content_w,
                        SP_COMPACT, SP_PASS_GDI, view.user_data));
        }

        SelectObject(hdc, old_font);
        SelectClipRgn(hdc, 0);
        TDrawArea__ReleaseDc(render_area, "sp_gdi");
    }

    used_render_rect.left = render_rect->right - (render_w + panel_w - (content_x + content_w));
    used_render_rect.top = render_rect->top;
    used_render_rect.right = render_rect->right;
    used_render_rect.bottom = render_rect->bottom;

    return used_render_rect;
}

static void sp_handle_hotkey(TRIBE_Panel_Screen_Overlay* panel, void* user_data, int hotkey)
{
    SpectatorUserData* sp_user_data = (SpectatorUserData*)user_data;
    switch (hotkey)
    {
    case 0x63:  // F8 – toggle spectator overlay on/off
        panel->vfptr->set_active((TPanel*)panel, panel->active ? 0 : 1);
        break;
    case 0x64:  // Alt+Q – cycle to next view
        spectator_next_view(sp_user_data);
        break;
    default:
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
