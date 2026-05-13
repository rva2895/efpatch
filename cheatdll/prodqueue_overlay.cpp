#include "stdafx.h"
#include "prodqueue_overlay.h"
#include "consts.h"
#include "rec.h"

static const int ICON_SIZE   = 36;
static const int COUNT_H     = 14;
static const int BAR_H       = 5;
static const int CELL_W      = ICON_SIZE + 2;
static const int MARGIN_R    = 8;
static const int ROW_H       = ICON_SIZE + COUNT_H + BAR_H;
static const int ROW_GAP     = 2;
static const int TECH_ROW_Y  = ROW_H + ROW_GAP;
static const int PANEL_H     = TECH_ROW_Y + ROW_H + 2;

static const int LABEL_H     = 16;
static const int SP_CELL_H   = LABEL_H + PANEL_H;   // 130 px per player (full layout)
static const int SP_CELL_GAP = 4;
static const int STRIPE_W    = 4;

// Compact layouts omit the name header; unit+tech share one row.
static const int COMPACT_CELL_H       = ROW_H + 2;   // 57 px, icon_size=36
static const int SMALL_ICON_SIZE      = 24;
static const int SMALL_COUNT_H        = 10;
static const int SMALL_BAR_H          = 3;
static const int SMALL_ROW_H          = SMALL_ICON_SIZE + SMALL_COUNT_H + SMALL_BAR_H;
static const int SMALL_COMPACT_CELL_H = SMALL_ROW_H + 2; // 39 px, icon_size=24

// Returns 0 (full, 130 px), 1 (compact-normal, 57 px), or 2 (compact-small, 39 px).
static int sp_pick_layout(int active, int avail_h)
{
    if (active <= 0) return 0;
    if (active * (SP_CELL_H      + SP_CELL_GAP) <= avail_h) return 0;
    if (active * (COMPACT_CELL_H + SP_CELL_GAP) <= avail_h) return 1;
    return 2;
}

static int sp_cell_h_for_layout(int layout)
{
    if (layout == 0) return SP_CELL_H;
    if (layout == 1) return COMPACT_CELL_H;
    return SMALL_COMPACT_CELL_H;
}

static int sp_icon_size_for_layout(int layout)
{
    return (layout == 2) ? SMALL_ICON_SIZE : ICON_SIZE;
}

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

static HBRUSH s_br_bg       = NULL;
static HBRUSH s_br_bar_bg   = NULL;
static HBRUSH s_br_bar_fg   = NULL;
static HBRUSH s_br_bar_tech = NULL;
static HBRUSH s_br_icon     = NULL;
static HBRUSH s_br_player[9] = {};

static void ensure_brushes()
{
    if (!s_br_bg)
    {
        s_br_bg       = CreateSolidBrush(RGB(  0,   0,   0));
        s_br_bar_bg   = CreateSolidBrush(RGB( 40,  40,  40));
        s_br_bar_fg   = CreateSolidBrush(RGB( 30, 160,  60));
        s_br_bar_tech = CreateSolidBrush(RGB( 60, 120, 220));
        s_br_icon     = CreateSolidBrush(RGB( 60,  60,  80));
    }
}

static HBRUSH get_player_brush(int idx)
{
    if (idx < 0 || idx > 8) idx = 0;
    if (!s_br_player[idx])
        s_br_player[idx] = CreateSolidBrush(PLAYER_COLORS[idx]);
    return s_br_player[idx];
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

    std::map<__int16, int> id_to_idx;

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

            auto it = id_to_idx.find(rec.master_id);
            if (it == id_to_idx.end())
            {
                PQEntry e;
                e.master_id    = rec.master_id;
                e.total_count  = rec.unit_count;
                e.max_progress = slot_prg;
                id_to_idx[rec.master_id] = (int)entries.size();
                entries.push_back(e);
            }
            else
            {
                PQEntry& e = entries[it->second];
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

void draw_prodqueue_overlay(TDrawArea* da, int cell_x, int cell_w,
                             const std::vector<PQEntry>& entries,
                             HRGN clip_region, TRIBE_Player* player, int row_y,
                             int icon_size)
{
    ensure_brushes();
    if (!player) return;

    int count_h_i = (icon_size <= 24) ? 10 : COUNT_H;
    int bar_h_i   = (icon_size <= 24) ? 3  : BAR_H;
    int row_h_i   = icon_size + count_h_i + bar_h_i;
    int cell_w_i  = icon_size + 2;

    TShape* unit_slp = resolve_unit_slp(player);
    bool has_slp = unit_slp != NULL;

    int n        = (int)entries.size();
    int origin_x = cell_x + cell_w - n * cell_w_i - MARGIN_R;
    if (origin_x < cell_x) origin_x = cell_x;

    if (TDrawArea__GetDc(da, "pq_unit"))
    {
        HDC hdc = da->DrawDc;
        RECT clip_rect = { cell_x, row_y, cell_x + cell_w, row_y + row_h_i };
        TDrawArea__SetClipRect(da, &clip_rect);
        SelectClipRgn(hdc, clip_region);
        SetBkMode(hdc, TRANSPARENT);
        HGDIOBJ old_font = SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));

        for (int i = 0; i < n; i++)
        {
            const PQEntry& e = entries[i];
            int cx = origin_x + i * cell_w_i;

            if (!has_slp)
            {
                RECT ricon = { cx, row_y, cx + icon_size, row_y + icon_size };
                FillRect(hdc, &ricon, s_br_icon);
            }

            int cy = row_y + icon_size;
            RECT rcount = { cx, cy, cx + icon_size, cy + count_h_i };
            FillRect(hdc, &rcount, s_br_bg);
            SetTextColor(hdc, RGB(255, 220, 50));
            char buf[12];
            snprintf(buf, sizeof(buf), "x%d", e.total_count);
            DrawTextA(hdc, buf, -1, &rcount, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            int by = cy + count_h_i;
            RECT rbar = { cx, by, cx + icon_size, by + bar_h_i };
            FillRect(hdc, &rbar, s_br_bar_bg);
            if (e.max_progress > 0 && e.max_progress <= 100)
            {
                int fw = (icon_size * e.max_progress) / 100;
                if (fw > 0) { RECT rf = { cx, by, cx + fw, by + bar_h_i }; FillRect(hdc, &rf, s_br_bar_fg); }
            }
        }

        SelectObject(hdc, old_font);
        SelectClipRgn(hdc, 0);
        TDrawArea__ReleaseDc(da, "pq_unit");
    }

    if (has_slp && TDrawArea__Lock(da, "pq_unit", 1))
    {
        for (int i = 0; i < n; i++)
        {
            RGE_Master_Static_Object* mo = player->master_objects[entries[i].master_id];
            if (!mo) continue;
            __int16 frame = mo->button_pict;
            if (frame < 0 || frame >= unit_slp->Num_Shapes) continue;
            Shape_Info* si = &unit_slp->shape_info[frame];
            TShape__shape_draw(unit_slp, da,
                origin_x + i * cell_w_i + si->Hotspot_X,
                row_y + si->Hotspot_Y, frame, NULL);
        }
        TDrawArea__Unlock(da, "pq_unit");
    }
}

void draw_techqueue_overlay(TDrawArea* da, int cell_x, int cell_w,
                             const std::vector<TechEntry>& entries,
                             HRGN clip_region, TRIBE_Player* player, int row_y,
                             int icon_size)
{
    ensure_brushes();
    if (!player) return;

    int count_h_i = (icon_size <= 24) ? 10 : COUNT_H;
    int bar_h_i   = (icon_size <= 24) ? 3  : BAR_H;
    int row_h_i   = icon_size + count_h_i + bar_h_i;
    int cell_w_i  = icon_size + 2;

    TShape* tech_slp = resolve_tech_slp(player);
    bool has_slp = tech_slp != NULL;

    int n        = (int)entries.size();
    int origin_x = cell_x + cell_w - n * cell_w_i - MARGIN_R;
    if (origin_x < cell_x) origin_x = cell_x;

    if (TDrawArea__GetDc(da, "pq_tech"))
    {
        HDC hdc = da->DrawDc;
        RECT clip_rect = { cell_x, row_y, cell_x + cell_w, row_y + row_h_i };
        TDrawArea__SetClipRect(da, &clip_rect);
        SelectClipRgn(hdc, clip_region);
        SetBkMode(hdc, TRANSPARENT);
        HGDIOBJ old_font = SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));

        for (int i = 0; i < n; i++)
        {
            const TechEntry& e = entries[i];
            int cx = origin_x + i * cell_w_i;

            if (!has_slp)
            {
                RECT ricon = { cx, row_y, cx + icon_size, row_y + icon_size };
                FillRect(hdc, &ricon, s_br_icon);
            }

            int ly = row_y + icon_size;
            RECT rl = { cx, ly, cx + icon_size, ly + count_h_i };
            FillRect(hdc, &rl, s_br_bg);
            SetTextColor(hdc, RGB(140, 200, 255));
            char buf[8];
            snprintf(buf, sizeof(buf), "%d%%", (int)e.progress);
            DrawTextA(hdc, buf, -1, &rl, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            int by = ly + count_h_i;
            RECT rbar = { cx, by, cx + icon_size, by + bar_h_i };
            FillRect(hdc, &rbar, s_br_bar_bg);
            if (e.progress > 0)
            {
                int fw = (icon_size * e.progress) / 100;
                if (fw > 0) { RECT rf = { cx, by, cx + fw, by + bar_h_i }; FillRect(hdc, &rf, s_br_bar_tech); }
            }
        }

        SelectObject(hdc, old_font);
        SelectClipRgn(hdc, 0);
        TDrawArea__ReleaseDc(da, "pq_tech");
    }

    if (has_slp && TDrawArea__Lock(da, "pq_tech", 1))
    {
        for (int i = 0; i < n; i++)
        {
            __int16 frame = entries[i].icon;
            if (frame < 0 || frame >= tech_slp->Num_Shapes) continue;
            Shape_Info* si = &tech_slp->shape_info[frame];
            TShape__shape_draw(tech_slp, da,
                origin_x + i * cell_w_i + si->Hotspot_X,
                row_y + si->Hotspot_Y, frame, NULL);
        }
        TDrawArea__Unlock(da, "pq_tech");
    }
}

// ===========================================================================
// Normal overlay – local player, live games only.
// ===========================================================================

struct PQUserData { bool had_queue; };

static void* pq_create(const void* user_init)
{
    UNREFERENCED_PARAMETER(user_init);
    PQUserData* d = new PQUserData;
    d->had_queue = false;
    return d;
}
static void pq_destroy(void* user_data) { delete (PQUserData*)user_data; }

static bool pq_need_redraw(void* user_data)
{
    PQUserData* d = (PQUserData*)user_data;
    if (isRec())
    {
        // One final redraw to clear the buffer when entering replay mode;
        // pq_render_to_image_buffer will draw nothing, leaving it transparent.
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

// NOTE: handle_size receives TRIBE_Panel_Screen_Overlay* (see overlay.cpp:handle_overlay_size).
static panel_size pq_handle_size(void* /*panel_ptr*/)
{
    panel_size s;
    s.left_border_in = s.right_border_in = s.bottom_border_in = 0;
    s.top_border_in  = 2;
    s.min_wid_in     = 100;
    s.max_wid_in     = 10000;
    s.min_hgt_in     = s.max_hgt_in = PANEL_H;
    return s;
}

static RECT pq_render_to_image_buffer(void* user_data, TDrawArea* render_area, RECT* render_rect, HRGN clip_region)
{
    UNREFERENCED_PARAMETER(user_data);
    // Spectator overlay takes over in replay mode; leave the buffer transparent.
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

void register_prodqueue_overlay()
{
    TRIBE_Panel_Screen_Overlay_User_Callbacks cb;
    cb.render_to_image_buffer = pq_render_to_image_buffer;
    cb.need_redraw            = pq_need_redraw;
    cb.handle_size            = pq_handle_size;
    cb.create                 = pq_create;
    cb.destroy                = pq_destroy;
    register_screen_overlay(cb, NULL);
}

// ===========================================================================
// Spectator overlay – all players, replay mode only.
// ===========================================================================

struct SpectatorUserData { bool had_queue; };

static void* sp_create(const void* user_init)
{
    UNREFERENCED_PARAMETER(user_init);
    SpectatorUserData* d = new SpectatorUserData;
    d->had_queue = false;
    return d;
}
static void sp_destroy(void* user_data) { delete (SpectatorUserData*)user_data; }

static bool sp_need_redraw(void* user_data)
{
    if (!isRec()) return false;
    if (!*base_game || !(*base_game)->world) return false;

    SpectatorUserData* d = (SpectatorUserData*)user_data;
    bool any  = false;
    int  pnum = (*base_game)->world->player_num;
    for (int i = 1; i < pnum && !any; i++)
    {
        TRIBE_Player* p = (*base_game)->world->players[i];
        if (!p) continue;
        std::vector<PQEntry>  u; if (collect_prodqueue_for_player(p, u))  { any = true; break; }
        std::vector<TechEntry> t; if (collect_techqueue_for_player(p, t)) { any = true; break; }
    }
    bool needs = any || d->had_queue;
    d->had_queue = any;
    return needs;
}

// handle_size receives TRIBE_Panel_Screen_Overlay*; reads parent panel height
// to pick the layout before the panel is sized.
static panel_size sp_handle_size(void* panel_ptr)
{
    int avail_h = 600; // safe default before layout is known
    if (panel_ptr)
    {
        TRIBE_Panel_Screen_Overlay* overlay = (TRIBE_Panel_Screen_Overlay*)panel_ptr;
        if (overlay->parent_panel && overlay->parent_panel->pnl_hgt > 0)
            avail_h = overlay->parent_panel->pnl_hgt;
    }

    int active = 8; // safe default before world is loaded
    if (*base_game && (*base_game)->world)
        active = max(1, (int)(*base_game)->world->player_num - 1);

    int layout = sp_pick_layout(active, avail_h);
    int cell_h = sp_cell_h_for_layout(layout);
    int h      = active * (cell_h + SP_CELL_GAP);

    panel_size s;
    s.left_border_in = s.right_border_in = s.bottom_border_in = 0;
    s.top_border_in  = 2;
    s.min_wid_in     = 200;
    s.max_wid_in     = 10000;
    s.min_hgt_in     = s.max_hgt_in = h;
    return s;
}

static RECT sp_render_to_image_buffer(void* user_data, TDrawArea* render_area, RECT* render_rect, HRGN clip_region)
{
    UNREFERENCED_PARAMETER(user_data);
    if (!isRec()) return *render_rect;
    if (!*base_game || !(*base_game)->world) return *render_rect;

    int panel_w = render_rect->right;
    int avail_h = render_rect->bottom;
    int pnum    = (*base_game)->world->player_num;
    int active  = pnum - 1;
    if (active <= 0) return *render_rect;

    int layout    = sp_pick_layout(active, avail_h);
    int cell_h    = sp_cell_h_for_layout(layout);
    int icon_size = sp_icon_size_for_layout(layout);
    int cell_step = cell_h + SP_CELL_GAP;

    // Icons occupy [0, panel_w - STRIPE_W); colour stripe sits on the right edge.
    int icon_x = 0;
    int icon_w = panel_w - STRIPE_W;

    int count_h_i       = (icon_size <= 24) ? SMALL_COUNT_H : COUNT_H;
    int bar_h_i         = (icon_size <= 24) ? SMALL_BAR_H   : BAR_H;
    int row_h_i         = icon_size + count_h_i + bar_h_i;
    int tech_row_offset = row_h_i + ROW_GAP;

    for (int i = 1; i < pnum; i++)
    {
        TRIBE_Player* player = (*base_game)->world->players[i];
        if (!player) continue;

        int cell_y  = (i - 1) * cell_step;
        int queue_y = cell_y;

        if (TDrawArea__GetDc(render_area, "sp_hdr"))
        {
            HDC hdc = render_area->DrawDc;

            RECT rstripe = { panel_w - STRIPE_W, cell_y, panel_w, cell_y + cell_h };
            FillRect(hdc, &rstripe, get_player_brush(i));

            if (layout == 0)
            {
                char name_buf[64];
                const char* name = (player->name && player->name[0]) ? player->name : NULL;
                if (!name) { snprintf(name_buf, sizeof(name_buf), "Player %d", i); name = name_buf; }

                COLORREF col = (i <= 8) ? PLAYER_COLORS[i] : RGB(255, 255, 255);
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, col);
                HGDIOBJ old_font = SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
                RECT rname = { 3, cell_y, panel_w - STRIPE_W - 2, cell_y + LABEL_H };
                DrawTextA(hdc, name, -1, &rname, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
                SelectObject(hdc, old_font);

                queue_y = cell_y + LABEL_H;
            }

            TDrawArea__ReleaseDc(render_area, "sp_hdr");
        }

        std::vector<PQEntry>  units;
        std::vector<TechEntry> techs;
        collect_prodqueue_for_player(player, units);
        collect_techqueue_for_player(player, techs);

        if (layout == 0)
        {
            if (!units.empty())
                draw_prodqueue_overlay(render_area, icon_x, icon_w, units, clip_region, player, queue_y, icon_size);
            if (!techs.empty())
                draw_techqueue_overlay(render_area, icon_x, icon_w, techs, clip_region, player, queue_y + tech_row_offset, icon_size);
        }
        else
        {
            // Compact: unit strip right-aligned; tech strip right-aligned left of units.
            int cell_w_i     = icon_size + 2;
            int unit_strip_w = (int)units.size() * cell_w_i + (units.empty() ? 0 : MARGIN_R);
            if (!techs.empty())
                draw_techqueue_overlay(render_area, icon_x, icon_w - unit_strip_w, techs, clip_region, player, queue_y, icon_size);
            if (!units.empty())
                draw_prodqueue_overlay(render_area, icon_x, icon_w, units, clip_region, player, queue_y, icon_size);
        }
    }

    return *render_rect;
}

void register_spectator_overlay()
{
    TRIBE_Panel_Screen_Overlay_User_Callbacks cb;
    cb.render_to_image_buffer = sp_render_to_image_buffer;
    cb.need_redraw            = sp_need_redraw;
    cb.handle_size            = sp_handle_size;
    cb.create                 = sp_create;
    cb.destroy                = sp_destroy;
    register_screen_overlay(cb, NULL);
}
