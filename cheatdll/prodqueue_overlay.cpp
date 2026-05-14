#include "stdafx.h"
#include "prodqueue_overlay.h"
#include "spectator_overlay.h"
#include "consts.h"
#include "rec.h"

// ---------------------------------------------------------------------------
// Layout constants
// ---------------------------------------------------------------------------
static const int ICON_SIZE     = 36;
static const int COUNT_H       = 14;
static const int BAR_H         = 5;
static const int CELL_W        = ICON_SIZE + 2;
static const int MARGIN_R      = 8;
static const int ROW_H         = ICON_SIZE + COUNT_H + BAR_H;  // 55 px
static const int ROW_GAP       = 2;
static const int TECH_ROW_Y    = ROW_H + ROW_GAP;              // 57 px
static const int PANEL_H       = TECH_ROW_Y + ROW_H + 2;       // 114 px

// Compact icon size (24px) row height: used by the spectator queue view
static const int ICON_SIZE_C   = 24;
static const int COUNT_H_C     = 10;
static const int BAR_H_C       = 3;
static const int ROW_H_COMPACT = ICON_SIZE_C + COUNT_H_C + BAR_H_C;  // 37 px

static HBRUSH s_br_bg       = NULL;
static HBRUSH s_br_bar_bg   = NULL;
static HBRUSH s_br_bar_fg   = NULL;
static HBRUSH s_br_bar_tech = NULL;
static HBRUSH s_br_icon     = NULL;

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

// ---------------------------------------------------------------------------
// SLP resolution helpers
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// collect_prodqueue_for_player
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// collect_techqueue_for_player
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// Convenience wrappers (local / currently-observed player)
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// draw_prodqueue_overlay
// Icons are right-aligned within [cell_x, cell_x + cell_w).
// row_y is the top of this row in the image buffer.
// ---------------------------------------------------------------------------
void draw_prodqueue_overlay(TDrawArea* da, int cell_x, int cell_w,
                             const std::vector<PQEntry>& entries,
                             HRGN clip_region, TRIBE_Player* player, int row_y,
                             int icon_size, bool right_align)
{
    ensure_brushes();
    if (!player) return;

    int count_h_i = (icon_size <= 24) ? 10 : COUNT_H;
    int bar_h_i   = (icon_size <= 24) ? 3  : BAR_H;
    // SLP sprites render at their native size (~36px) regardless of icon_size,
    // so the cell step must be wide enough to prevent sprite bleed-over.
    int cell_w_i  = max(icon_size, 36) + 4;

    TShape* unit_slp = resolve_unit_slp(player);
    bool has_slp = unit_slp != NULL;

    int n        = (int)entries.size();
    int origin_x = right_align
                   ? max(cell_x, cell_x + cell_w - n * cell_w_i - MARGIN_R)
                   : cell_x;

    if (TDrawArea__GetDc(da, "pq_unit"))
    {
        HDC hdc = da->DrawDc;
        RECT clip_rect = { cell_x, row_y, cell_x + cell_w, row_y + icon_size + count_h_i + bar_h_i };
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

// ---------------------------------------------------------------------------
// draw_techqueue_overlay
// ---------------------------------------------------------------------------
void draw_techqueue_overlay(TDrawArea* da, int cell_x, int cell_w,
                             const std::vector<TechEntry>& entries,
                             HRGN clip_region, TRIBE_Player* player, int row_y,
                             int icon_size, bool right_align)
{
    ensure_brushes();
    if (!player) return;

    int count_h_i = (icon_size <= 24) ? 10 : COUNT_H;
    int bar_h_i   = (icon_size <= 24) ? 3  : BAR_H;
    int cell_w_i  = max(icon_size, 36) + 4;

    TShape* tech_slp = resolve_tech_slp(player);
    bool has_slp = tech_slp != NULL;

    int n        = (int)entries.size();
    int origin_x = right_align
                   ? max(cell_x, cell_x + cell_w - n * cell_w_i - MARGIN_R)
                   : cell_x;

    if (TDrawArea__GetDc(da, "pq_tech"))
    {
        HDC hdc = da->DrawDc;
        RECT clip_rect = { cell_x, row_y, cell_x + cell_w, row_y + icon_size + count_h_i + bar_h_i };
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
// Normal (single-player) overlay
// Shows the local / currently-observed player's queue during live games only.
// ===========================================================================

struct PQUserData { bool had_queue; };

static void* pq_create(TRIBE_Panel_Screen_Overlay* /*panel*/, const void* /*user_init*/)
{
    PQUserData* d = new PQUserData;
    d->had_queue = false;
    return d;
}
static void pq_destroy(TRIBE_Panel_Screen_Overlay* /*panel*/, void* user_data) { delete (PQUserData*)user_data; }

static bool pq_need_redraw(TRIBE_Panel_Screen_Overlay* /*panel*/, void* user_data)
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

static panel_size pq_handle_size(TRIBE_Panel_Screen_Overlay* /*panel*/, void* /*user_data*/)
{
    panel_size s;
    s.left_border_in = s.right_border_in = s.bottom_border_in = 0;
    s.top_border_in  = 2;
    s.min_wid_in     = 100;
    s.max_wid_in     = 10000;
    s.min_hgt_in     = s.max_hgt_in = PANEL_H;
    return s;
}

static RECT pq_render_to_image_buffer(TRIBE_Panel_Screen_Overlay* /*panel*/, void* /*user_data*/,
                                       TDrawArea* render_area, RECT* render_rect, HRGN clip_region)
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
    if (hotkey == 0x63)  // F8 – toggle visibility
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

// ===========================================================================
// Queue spectator view
// Registered into the spectator container via register_queue_spectator_view().
// The container handles the player grid, name header, and colour stripe.
// This view renders the unit row and tech row into its allocated content area.
// ===========================================================================

static bool queue_need_redraw()
{
    if (!*base_game || !(*base_game)->world) return false;
    int pnum = (*base_game)->world->player_num;
    for (int i = 1; i < pnum; i++)
    {
        TRIBE_Player* p = (*base_game)->world->players[i];
        if (!p) continue;
        std::vector<PQEntry>   u;
        std::vector<TechEntry> t;
        if (collect_prodqueue_for_player(p, u))  return true;
        if (collect_techqueue_for_player(p, t)) return true;
    }
    return false;
}

static void queue_render(TDrawArea* da, HRGN clip,
                          TRIBE_Player* player, int /*player_idx*/,
                          int x, int y, int w, SpectatorLayout layout)
{
    std::vector<PQEntry>   units;
    std::vector<TechEntry> techs;
    collect_prodqueue_for_player(player, units);
    collect_techqueue_for_player(player, techs);

    if (layout == SP_FULL)
    {
        if (!units.empty())
            draw_prodqueue_overlay(da, x, w, units, clip, player, y, ICON_SIZE, false);
        if (!techs.empty())
            draw_techqueue_overlay(da, x, w, techs, clip, player, y + TECH_ROW_Y, ICON_SIZE, false);
    }
    else // SP_COMPACT: both right-aligned; techs at far right, units to their left
    {
        // cell_w_i must match what draw_*queue_overlay computes internally
        int cell_w_c     = max(ICON_SIZE_C, 36) + 4;
        // Reserve space for tech strip on the right; units fill what remains
        int tech_strip_w = techs.empty() ? 0 : ((int)techs.size() * cell_w_c + MARGIN_R);
        if (!techs.empty())
            draw_techqueue_overlay(da, x, w,                  techs, clip, player, y, ICON_SIZE_C, true);
        if (!units.empty())
            draw_prodqueue_overlay(da, x, w - tech_strip_w,   units, clip, player, y, ICON_SIZE_C, true);
    }
}

static const SpectatorViewDef s_queue_view_def = {
    "Production",
    PANEL_H,       // full_h:    unit row + tech row (114 px)
    ROW_H_COMPACT, // compact_h: single combined row with 24px icons (37 px)
    queue_render,
    queue_need_redraw
};

void register_queue_spectator_view()
{
    register_spectator_view(s_queue_view_def);
}
