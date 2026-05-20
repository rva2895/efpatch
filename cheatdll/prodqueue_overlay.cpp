#include "stdafx.h"
#include "prodqueue_overlay.h"
#include "spectator_overlay.h"
#include "consts.h"
#include "rec.h"

static const int ICON_SIZE   = 36;
static const int COUNT_H     = 14;
static const int BAR_H       = 5;
static const int MARGIN_R    = 8;
static const int ROW_H       = ICON_SIZE + COUNT_H + BAR_H;
static const int ROW_GAP     = 2;
static const int TECH_ROW_Y  = ROW_H + ROW_GAP;
static const int PANEL_H     = TECH_ROW_Y + ROW_H + 2;

// Compact (spectator) layout.
static const int ICON_SIZE_C     = 24;
static const int COUNT_H_C       = 14;
static const int COUNT_H_C_LARGE = 18;       // cd.largeText path
static const int BAR_H_C         = 4;
// SLPs render at their native ~36 px from (cx, row_y) regardless of icon_size,
// so the strip-below must sit at row_y + 36, not row_y + icon_size.
static const int SLP_NATIVE_H    = 36;
// Allocate the largeText height so the bar never clips on toggle.
static const int ROW_H_COMPACT   = SLP_NATIVE_H + COUNT_H_C_LARGE + BAR_H_C;

static HBRUSH s_br_bg       = NULL;
static HBRUSH s_br_bar_bg   = NULL;
static HBRUSH s_br_bar_fg   = NULL;
static HBRUSH s_br_bar_tech = NULL;
static HBRUSH s_br_icon     = NULL;

static void ensure_brushes()
{
    if (s_br_bg) return;
    s_br_bg       = CreateSolidBrush(RGB(  0,   0,   0));
    s_br_bar_bg   = CreateSolidBrush(RGB( 40,  40,  40));
    s_br_bar_fg   = CreateSolidBrush(RGB( 30, 200,  70));
    s_br_bar_tech = CreateSolidBrush(RGB( 80, 150, 240));
    s_br_icon     = CreateSolidBrush(RGB( 60,  60,  80));
}

static TShape* resolve_unit_slp(TRIBE_Player* player)
{
    if (!iconsUnitPtr) return NULL;
    int civ = player ? player->culture : 0;
    TShape* slp = (civ >= 1 && civ <= CIV_COUNT) ? iconsUnitPtr[civ] : NULL;
    if (!slp || !slp->Is_Loaded || slp->Num_Shapes <= 0)
        for (int k = 1; k <= CIV_COUNT; k++)
        {
            TShape* s = iconsUnitPtr[k];
            if (s && s->Is_Loaded && s->Num_Shapes > 0) { slp = s; break; }
        }
    return (slp && slp->Is_Loaded && slp->Num_Shapes > 0) ? slp : NULL;
}

static TShape* resolve_tech_slp(TRIBE_Player* player)
{
    if (!iconsTechPtr) return NULL;
    int civ = player ? player->culture : 0;
    TShape* slp = (civ >= 1 && civ <= CIV_COUNT) ? iconsTechPtr[civ] : NULL;
    if (!slp || !slp->Is_Loaded || slp->Num_Shapes <= 0)
        for (int k = 1; k <= CIV_COUNT; k++)
        {
            TShape* s = iconsTechPtr[k];
            if (s && s->Is_Loaded && s->Num_Shapes > 0) { slp = s; break; }
        }
    return (slp && slp->Is_Loaded && slp->Num_Shapes > 0) ? slp : NULL;
}

bool collect_prodqueue_for_player(TRIBE_Player* player, std::vector<PQEntry>& entries)
{
    entries.clear();
    if (!player || !player->objects) return false;

    for (int i = 0; i < player->objects->Number_of_objects; i++)
    {
        RGE_Static_Object* obj = player->objects->List[i];
        if (!obj || !obj->master_obj) continue;
        if (obj->master_obj->master_type != 80) continue;

        TRIBE_Building_Object* bld = (TRIBE_Building_Object*)obj;
        if (!bld->production_queue || bld->production_queue_count <= 0) continue;

        __int16 active_id  = 0;
        __int16 active_prg = 0;
        bool training = TRIBE_Building_Object__production_queue_status(
                            bld, &active_id, &active_prg) != 0;

        for (int j = 0; j < bld->production_queue_count; j++)
        {
            const Production_Queue_Record& rec = bld->production_queue[j];
            if (rec.master_id < 0 || rec.unit_count <= 0) continue;
            if (rec.master_id >= player->master_object_num) continue;
            if (!player->master_objects[rec.master_id])    continue;

            __int16 slot_prg = (training && j == 0 && rec.master_id == active_id)
                               ? active_prg : 0;

            // Linear scan beats std::map: N is small (<~20 distinct master_ids).
            int idx = -1;
            for (int k = 0; k < (int)entries.size(); k++)
                if (entries[k].master_id == rec.master_id) { idx = k; break; }

            if (idx < 0)
            {
                PQEntry e;
                e.master_id    = rec.master_id;
                e.total_count  = rec.unit_count;
                e.max_progress = slot_prg;
                entries.push_back(e);
            }
            else
            {
                PQEntry& e = entries[idx];
                e.total_count += rec.unit_count;
                if (slot_prg > e.max_progress) e.max_progress = slot_prg;
            }
        }
    }
    return !entries.empty();
}

bool collect_techqueue_for_player(TRIBE_Player* player, std::vector<TechEntry>& entries)
{
    entries.clear();
    if (!player || !player->tech_tree) return false;

    TRIBE_Player_Tech* pt = player->tech_tree;
    if (!pt->tech_player_tree || !pt->base_tech) return false;

    __int16 limit = min(pt->tech_player_tree_num, pt->base_tech->tech_tree_num);

    for (__int16 i = 0; i < limit; i++)
    {
        Tech_Player_Tree& ptt = pt->tech_player_tree[i];
        if (ptt.state != 2 || ptt.research_done <= 0.0f) continue;

        float total = (float)pt->base_tech->tech_tree[i].research;
        int pct = (total > 0.0f) ? (int)((ptt.research_done / total) * 100.0f) : 0;
        pct = max(0, min(100, pct));

        TechEntry e;
        e.tech_id  = i;
        e.progress = (__int16)pct;
        e.icon     = pt->base_tech->tech_tree[i].icon;
        entries.push_back(e);
    }
    return !entries.empty();
}

bool collect_prodqueue(std::vector<PQEntry>& entries)
{
    entries.clear();
    if (!*base_game || !(*base_game)->world) return false;
    return collect_prodqueue_for_player((TRIBE_Player*)RGE_Base_Game__get_player(*base_game), entries);
}

bool collect_techqueue(std::vector<TechEntry>& entries)
{
    entries.clear();
    if (!*base_game || !(*base_game)->world) return false;
    return collect_techqueue_for_player((TRIBE_Player*)RGE_Base_Game__get_player(*base_game), entries);
}

static inline int pq_count_h(int icon_size)
{
    return (icon_size <= 24) ? (cd.largeText ? COUNT_H_C_LARGE : COUNT_H_C) : COUNT_H;
}
static inline int pq_bar_h(int icon_size) { return (icon_size <= 24) ? BAR_H_C : BAR_H; }
// Cell step ≥ native SLP width + 8 px gutter so icons don't bleed into neighbours.
static inline int pq_cell_step(int icon_size) { return max(icon_size, 36) + 8; }

void draw_prodqueue_overlay_pass(TDrawArea* da, int cell_x, int cell_w,
                                  const std::vector<PQEntry>& entries,
                                  HRGN clip_region, TRIBE_Player* player, int row_y,
                                  int icon_size, bool right_align,
                                  SpectatorPass pass)
{
    if (!player) return;

    int count_h_i = pq_count_h(icon_size);
    int bar_h_i   = pq_bar_h(icon_size);
    int cell_w_i  = pq_cell_step(icon_size);

    int n        = (int)entries.size();
    int origin_x = right_align
                   ? max(cell_x, cell_x + cell_w - n * cell_w_i - MARGIN_R)
                   : cell_x;

    int strip_w = max(icon_size, SLP_NATIVE_H);

    if (pass == SP_PASS_SLP)
    {
        // Palette idx 0 is true black on this surface; other colours go through
        // nearest-match (close enough for bars that don't need text contrast).
        const unsigned __int8 pi_black  = 0;
        unsigned __int8 pi_bar_bg = pal_index(RGB( 40,  40,  40));
        unsigned __int8 pi_bar_fg = pal_index(RGB( 30, 200,  70));
        unsigned __int8 pi_icon   = pal_index(RGB( 60,  60,  80));
        TShape* unit_slp = resolve_unit_slp(player);

        int icon_h = max(icon_size, SLP_NATIVE_H);

        for (int i = 0; i < n; i++)
        {
            const PQEntry& e = entries[i];
            int cx = origin_x + i * cell_w_i;

            if (!unit_slp)
                TDrawArea__FillRect(da, cx, row_y, cx + icon_size, row_y + icon_h, pi_icon);

            int cy = row_y + icon_h;
            TDrawArea__FillRect(da, cx, cy, cx + strip_w, cy + count_h_i, pi_black);
            int by = cy + count_h_i;
            TDrawArea__FillRect(da, cx, by, cx + strip_w, by + bar_h_i, pi_bar_bg);
            if (e.max_progress > 0 && e.max_progress <= 100)
            {
                int fw = (strip_w * e.max_progress) / 100;
                if (fw > 0) TDrawArea__FillRect(da, cx, by, cx + fw, by + bar_h_i, pi_bar_fg);
            }

            if (unit_slp)
            {
                RGE_Master_Static_Object* mo = player->master_objects[entries[i].master_id];
                if (!mo) continue;
                __int16 frame = mo->button_pict;
                if (frame < 0 || frame >= unit_slp->Num_Shapes) continue;
                TShape__shape_draw(unit_slp, da, cx, row_y, frame, NULL);
            }
        }
        return;
    }

    HDC hdc = da->DrawDc;
    if (!hdc) return;
    SetTextColor(hdc, RGB(255, 230, 80));
    int icon_h = max(icon_size, SLP_NATIVE_H);
    int font_h = sp_font_h();

    for (int i = 0; i < n; i++)
    {
        const PQEntry& e = entries[i];
        int cx = origin_x + i * cell_w_i;
        int cy = row_y + icon_h;
        char buf[12];
        int  len = snprintf(buf, sizeof(buf), "%d", e.total_count);

        SIZE sz;
        GetTextExtentPoint32A(hdc, buf, len, &sz);
        int tx = cx + (strip_w - sz.cx) / 2;
        int ty = cy + (count_h_i - font_h) / 2;
        if (ty < cy) ty = cy;
        RECT clip = { cx, cy, cx + strip_w, cy + count_h_i };
        ExtTextOutA(hdc, tx, ty, ETO_CLIPPED, &clip, buf, len, NULL);
    }
}

void draw_techqueue_overlay_pass(TDrawArea* da, int cell_x, int cell_w,
                                  const std::vector<TechEntry>& entries,
                                  HRGN clip_region, TRIBE_Player* player, int row_y,
                                  int icon_size, bool right_align,
                                  SpectatorPass pass)
{
    if (!player) return;

    int count_h_i = pq_count_h(icon_size);
    int bar_h_i   = pq_bar_h(icon_size);
    int cell_w_i  = pq_cell_step(icon_size);

    int n        = (int)entries.size();
    int origin_x = right_align
                   ? max(cell_x, cell_x + cell_w - n * cell_w_i - MARGIN_R)
                   : cell_x;

    int strip_w = max(icon_size, SLP_NATIVE_H);

    if (pass == SP_PASS_SLP)
    {
        const unsigned __int8 pi_black  = 0;
        unsigned __int8 pi_bar_bg = pal_index(RGB( 40,  40,  40));
        unsigned __int8 pi_bar_fg = pal_index(RGB( 80, 150, 240));
        unsigned __int8 pi_icon   = pal_index(RGB( 60,  60,  80));
        TShape* tech_slp = resolve_tech_slp(player);

        int icon_h = max(icon_size, SLP_NATIVE_H);

        for (int i = 0; i < n; i++)
        {
            const TechEntry& e = entries[i];
            int cx = origin_x + i * cell_w_i;

            if (!tech_slp)
                TDrawArea__FillRect(da, cx, row_y, cx + icon_size, row_y + icon_h, pi_icon);

            int ly = row_y + icon_h;
            TDrawArea__FillRect(da, cx, ly, cx + strip_w, ly + count_h_i, pi_black);
            int by = ly + count_h_i;
            TDrawArea__FillRect(da, cx, by, cx + strip_w, by + bar_h_i, pi_bar_bg);
            if (e.progress > 0)
            {
                int fw = (strip_w * e.progress) / 100;
                if (fw > 0) TDrawArea__FillRect(da, cx, by, cx + fw, by + bar_h_i, pi_bar_fg);
            }

            if (tech_slp)
            {
                __int16 frame = e.icon;
                if (frame < 0 || frame >= tech_slp->Num_Shapes) continue;
                TShape__shape_draw(tech_slp, da, cx, row_y, frame, NULL);
            }
        }
        return;
    }

    HDC hdc = da->DrawDc;
    if (!hdc) return;
    SetTextColor(hdc, RGB(160, 210, 255));
    int icon_h = max(icon_size, SLP_NATIVE_H);
    int font_h = sp_font_h();

    for (int i = 0; i < n; i++)
    {
        const TechEntry& e = entries[i];
        int cx = origin_x + i * cell_w_i;
        int ly = row_y + icon_h;
        char buf[8];
        // "100%" doesn't fit; the only 3-digit % value is 100, so drop the %.
        int  len = snprintf(buf, sizeof(buf), (e.progress >= 100) ? "%d" : "%d%%", (int)e.progress);

        SIZE sz;
        GetTextExtentPoint32A(hdc, buf, len, &sz);
        int tx = cx + (strip_w - sz.cx) / 2;
        int ty = ly + (count_h_i - font_h) / 2;
        if (ty < ly) ty = ly;
        RECT clip = { cx, ly, cx + strip_w, ly + count_h_i };
        ExtTextOutA(hdc, tx, ty, ETO_CLIPPED, &clip, buf, len, NULL);
    }
}

// Single-cell wrappers for the live overlay (one cell, one player). Spectator
// path calls draw_*_pass directly inside its batched Lock/GetDc passes.
void draw_prodqueue_overlay(TDrawArea* da, int cell_x, int cell_w,
                             const std::vector<PQEntry>& entries,
                             HRGN clip_region, TRIBE_Player* player, int row_y,
                             int icon_size, bool right_align)
{
    if (!player) return;

    int count_h_i = pq_count_h(icon_size);
    int bar_h_i   = pq_bar_h(icon_size);

    RECT clip_rect = { cell_x, row_y, cell_x + cell_w, row_y + max(icon_size, SLP_NATIVE_H) + count_h_i + bar_h_i };
    TDrawArea__SetClipRect(da, &clip_rect);

    if (TDrawArea__Lock(da, "pq_unit", 1))
    {
        draw_prodqueue_overlay_pass(da, cell_x, cell_w, entries, clip_region,
                                    player, row_y, icon_size, right_align, SP_PASS_SLP);
        TDrawArea__Unlock(da, "pq_unit");
    }

    if (TDrawArea__GetDc(da, "pq_unit"))
    {
        HDC hdc = da->DrawDc;
        SetBkMode(hdc, TRANSPARENT);
        SelectClipRgn(hdc, clip_region);
        RGE_Font* gf = RGE_Base_Game__get_font(*base_game, 26);
        HGDIOBJ old_font = SelectObject(hdc, gf ? gf->font : GetStockObject(DEFAULT_GUI_FONT));
        draw_prodqueue_overlay_pass(da, cell_x, cell_w, entries, clip_region,
                                    player, row_y, icon_size, right_align, SP_PASS_GDI);
        SelectObject(hdc, old_font);
        SelectClipRgn(hdc, 0);
        TDrawArea__ReleaseDc(da, "pq_unit");
    }

    TDrawArea__SetClipRect(da, NULL);
}

void draw_techqueue_overlay(TDrawArea* da, int cell_x, int cell_w,
                             const std::vector<TechEntry>& entries,
                             HRGN clip_region, TRIBE_Player* player, int row_y,
                             int icon_size, bool right_align)
{
    if (!player) return;

    int count_h_i = pq_count_h(icon_size);
    int bar_h_i   = pq_bar_h(icon_size);

    RECT clip_rect = { cell_x, row_y, cell_x + cell_w, row_y + max(icon_size, SLP_NATIVE_H) + count_h_i + bar_h_i };
    TDrawArea__SetClipRect(da, &clip_rect);

    if (TDrawArea__Lock(da, "pq_tech", 1))
    {
        draw_techqueue_overlay_pass(da, cell_x, cell_w, entries, clip_region,
                                    player, row_y, icon_size, right_align, SP_PASS_SLP);
        TDrawArea__Unlock(da, "pq_tech");
    }

    if (TDrawArea__GetDc(da, "pq_tech"))
    {
        HDC hdc = da->DrawDc;
        SetBkMode(hdc, TRANSPARENT);
        SelectClipRgn(hdc, clip_region);
        RGE_Font* gf = RGE_Base_Game__get_font(*base_game, 26);
        HGDIOBJ old_font = SelectObject(hdc, gf ? gf->font : GetStockObject(DEFAULT_GUI_FONT));
        draw_techqueue_overlay_pass(da, cell_x, cell_w, entries, clip_region,
                                    player, row_y, icon_size, right_align, SP_PASS_GDI);
        SelectObject(hdc, old_font);
        SelectClipRgn(hdc, 0);
        TDrawArea__ReleaseDc(da, "pq_tech");
    }

    TDrawArea__SetClipRect(da, NULL);
}

// Live overlay: local player's queue, non-spectator games only.

struct PQUserData { bool had_queue; };

static void* pq_create(TRIBE_Panel_Screen_Overlay* panel, const void* user_init)
{
    PQUserData* d = new PQUserData;
    d->had_queue = false;
    return d;
}
static void pq_destroy(TRIBE_Panel_Screen_Overlay* panel, void* user_data)
{
    delete (PQUserData*)user_data;
}

static bool pq_need_redraw(TRIBE_Panel_Screen_Overlay* panel, void* user_data)
{
    PQUserData* d = (PQUserData*)user_data;
    if (isRec())
    {
        bool was = d->had_queue;
        d->had_queue = false;
        return was;
    }
    std::vector<PQEntry>  u; bool hu = collect_prodqueue(u);
    std::vector<TechEntry> t; bool ht = collect_techqueue(t);
    bool any = hu || ht;
    bool needs = any || d->had_queue;
    d->had_queue = any;
    return needs;
}

static panel_size pq_handle_size(TRIBE_Panel_Screen_Overlay* panel, void* user_data)
{
    panel_size s;
    s.left_border_in = s.right_border_in = s.bottom_border_in = 0;
    s.top_border_in  = 2;
    s.min_wid_in     = 100;
    s.max_wid_in     = 10000;
    s.min_hgt_in     = s.max_hgt_in = PANEL_H;
    return s;
}

static RECT pq_render_to_image_buffer(TRIBE_Panel_Screen_Overlay* panel, void* user_data, TDrawArea* render_area, RECT* render_rect, HRGN clip_region)
{
    if (isRec()) return *render_rect;

    int panel_w = render_rect->right;
    TRIBE_Player* player = (TRIBE_Player*)RGE_Base_Game__get_player(*base_game);
    if (!player) return *render_rect;

    std::vector<PQEntry> units;
    collect_prodqueue_for_player(player, units);
    if (!units.empty())
        draw_prodqueue_overlay(render_area, 0, panel_w, units, clip_region, player, 0);

    std::vector<TechEntry> techs;
    collect_techqueue_for_player(player, techs);
    if (!techs.empty())
        draw_techqueue_overlay(render_area, 0, panel_w, techs, clip_region, player, TECH_ROW_Y);

    return *render_rect;
}

static void pq_handle_hotkey(TRIBE_Panel_Screen_Overlay* panel, void* /*user_data*/, int hotkey)
{
    if (hotkey == 0x63)  // F8: toggle live overlay
        panel->vfptr->set_active((TPanel*)panel, panel->active ? 0 : 1);
}

void register_prodqueue_overlay()
{
    TRIBE_Panel_Screen_Overlay_User_Callbacks cb = {};
    cb.render_to_image_buffer = pq_render_to_image_buffer;
    cb.need_redraw            = pq_need_redraw;
    cb.handle_size            = pq_handle_size;
    cb.handle_hotkey          = pq_handle_hotkey;
    cb.create                 = pq_create;
    cb.destroy                = pq_destroy;
    register_screen_overlay(cb, NULL);
}

// Queue spectator view: container handles player grid + name + stripe; this
// view fills the per-player content area with the unit and tech rows.

static bool queue_need_redraw()
{
    return *base_game && (*base_game)->world;
}

struct QueueCacheEntry {
    std::vector<PQEntry>   units;
    std::vector<TechEntry> techs;
};
static QueueCacheEntry s_queue_cache[9];  // index 1..8; 0 unused (gaia)

static void queue_begin_frame()
{
    if (!*base_game || !(*base_game)->world) return;
    int pnum = (*base_game)->world->player_num;
    if (pnum > 9) pnum = 9;
    for (int i = 1; i < pnum; i++)
    {
        TRIBE_Player* p = (*base_game)->world->players[i];
        s_queue_cache[i].units.clear();
        s_queue_cache[i].techs.clear();
        if (!p) continue;
        collect_prodqueue_for_player(p, s_queue_cache[i].units);
        collect_techqueue_for_player(p, s_queue_cache[i].techs);
    }
}

static void queue_render(TDrawArea* da, HRGN clip,
                          TRIBE_Player* player, int player_idx,
                          int x, int y, int w,
                          SpectatorLayout layout, SpectatorPass pass)
{
    if (player_idx < 1 || player_idx > 8) return;
    const std::vector<PQEntry>&   units = s_queue_cache[player_idx].units;
    const std::vector<TechEntry>& techs = s_queue_cache[player_idx].techs;

    if (layout == SP_FULL)
    {
        if (!units.empty())
            draw_prodqueue_overlay_pass(da, x, w, units, clip, player, y, ICON_SIZE, false, pass);
        if (!techs.empty())
            draw_techqueue_overlay_pass(da, x, w, techs, clip, player, y + TECH_ROW_Y, ICON_SIZE, false, pass);
    }
    else // SP_COMPACT: both right-aligned; techs at far right, units to their left
    {
        int cell_w_c     = max(ICON_SIZE_C, 36) + 4;
        int tech_strip_w = techs.empty() ? 0 : ((int)techs.size() * cell_w_c + MARGIN_R);
        if (!techs.empty())
            draw_techqueue_overlay_pass(da, x, w,                techs, clip, player, y, ICON_SIZE_C, true, pass);
        if (!units.empty())
            draw_prodqueue_overlay_pass(da, x, w - tech_strip_w, units, clip, player, y, ICON_SIZE_C, true, pass);
    }
}

static const SpectatorViewDef s_queue_view_def = {
    "Production",
    PANEL_H,
    ROW_H_COMPACT,
    queue_render,
    queue_need_redraw,
    queue_begin_frame
};

void register_queue_spectator_view()
{
    register_spectator_view(s_queue_view_def);
}
