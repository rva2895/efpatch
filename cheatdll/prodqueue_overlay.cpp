#include "stdafx.h"
#include "prodqueue_overlay.h"
#include "consts.h"

// --- Layout constants ---
static const int ICON_SIZE = 36;
static const int COUNT_H   = 14;
static const int BAR_H     = 5;
static const int CELL_W    = ICON_SIZE + 2;
static const int MARGIN_R  = 8;
static const int ROW_H     = ICON_SIZE + COUNT_H + BAR_H;   // 55 px per row
static const int ROW_GAP   = 2;                              // gap between rows
static const int TECH_ROW_Y = ROW_H + ROW_GAP;              // tech row starts at y=57
static const int PANEL_H   = TECH_ROW_Y + ROW_H + 2;        // total panel height = 114

static HBRUSH s_br_bg     = NULL;
static HBRUSH s_br_bar_bg = NULL;
static HBRUSH s_br_bar_fg = NULL;
static HBRUSH s_br_bar_tech = NULL;
static HBRUSH s_br_icon   = NULL;

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
// collect_prodqueue
// ---------------------------------------------------------------------------
bool collect_prodqueue(std::vector<PQEntry>& entries)
{
    entries.clear();

    if (!*base_game || !(*base_game)->world) return false;

    TRIBE_Player* player = (TRIBE_Player*)RGE_Base_Game__get_player(*base_game);
    if (!player || !player->objects) return false;

    std::map<__int16, int> id_to_idx;

    for (int i = 0; i < player->objects->Number_of_objects; i++)
    {
        RGE_Static_Object* obj = player->objects->List[i];
        if (!obj || !obj->master_obj) continue;
        if (obj->master_obj->master_type != 80) continue; // buildings only

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
                if (slot_prg > e.max_progress)
                    e.max_progress = slot_prg;
            }
        }
    }

    return !entries.empty();
}

// ---------------------------------------------------------------------------
// collect_techqueue
// ---------------------------------------------------------------------------
bool collect_techqueue(std::vector<TechEntry>& entries)
{
    entries.clear();

    if (!*base_game || !(*base_game)->world) return false;

    TRIBE_Player* player = (TRIBE_Player*)RGE_Base_Game__get_player(*base_game);
    if (!player || !player->tech_tree) return false;

    TRIBE_Player_Tech* pt  = player->tech_tree;
    if (!pt->tech_player_tree || !pt->base_tech) return false;

    __int16 num      = pt->tech_player_tree_num;
    __int16 tech_num = pt->base_tech->tech_tree_num;
    __int16 limit    = (num < tech_num) ? num : tech_num;

    for (__int16 i = 0; i < limit; i++)
    {
        Tech_Player_Tree& ptt = pt->tech_player_tree[i];

        // state == 2 is RESEARCHING in the AoE2/SWGB engine.
        // Also require research_done > 0 as a safety check.
        if (ptt.state != 2 || ptt.research_done <= 0.0f) continue;

        float total = (float)pt->base_tech->tech_tree[i].research;
        int pct = 0;
        if (total > 0.0f)
            pct = (int)((ptt.research_done / total) * 100.0f);
        if (pct < 0)   pct = 0;
        if (pct > 100) pct = 100;

        TechEntry e;
        e.tech_id  = i;
        e.progress = (__int16)pct;
        e.icon     = pt->base_tech->tech_tree[i].icon;
        entries.push_back(e);
    }

    return !entries.empty();
}

// ---------------------------------------------------------------------------
// draw_prodqueue_overlay  (unit row)
// ---------------------------------------------------------------------------
void draw_prodqueue_overlay(TDrawArea* da, int panel_w, const std::vector<PQEntry>& entries, HRGN clip_region)
{
    ensure_brushes();

    TRIBE_Player* player = (TRIBE_Player*)RGE_Base_Game__get_player(*base_game);
    if (!player) return;

    // Resolve the unit-icon SLP for this civ (fall back to any loaded civ).
    int civ = player->culture;
    TShape* unit_slp = (iconsUnitPtr && civ >= 1 && civ <= CIV_COUNT) ? iconsUnitPtr[civ] : NULL;
    if (!unit_slp || !unit_slp->Is_Loaded || unit_slp->Num_Shapes <= 0)
    {
        if (iconsUnitPtr)
        {
            for (int k = 1; k <= CIV_COUNT; k++)
            {
                TShape* s = iconsUnitPtr[k];
                if (s && s->Is_Loaded && s->Num_Shapes > 0) { unit_slp = s; break; }
            }
        }
    }
    bool has_slp = unit_slp && unit_slp->Is_Loaded && unit_slp->Num_Shapes > 0;

    int n        = (int)entries.size();
    int origin_x = panel_w - n * CELL_W - MARGIN_R;
    if (origin_x < 0) origin_x = 0;

    // --- GDI pass: count text + progress bar ---
    if (TDrawArea__GetDc(da, "pq_unit"))
    {
        HDC hdc = da->DrawDc;
        RECT clip_rect = { 0, 0, panel_w, ROW_H };
        TDrawArea__SetClipRect(da, &clip_rect);
        SelectClipRgn(hdc, clip_region);

        SetBkMode(hdc, TRANSPARENT);
        HGDIOBJ old_font = SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));

        for (int i = 0; i < n; i++)
        {
            const PQEntry& e = entries[i];
            int cx = origin_x + i * CELL_W;

            if (!has_slp)
            {
                RECT ricon = { cx, 0, cx + ICON_SIZE, ICON_SIZE };
                FillRect(hdc, &ricon, s_br_icon);
            }

            int count_y = ICON_SIZE;
            RECT rcount = { cx, count_y, cx + ICON_SIZE, count_y + COUNT_H };
            FillRect(hdc, &rcount, s_br_bg);
            SetTextColor(hdc, RGB(255, 220, 50));
            char buf[12];
            snprintf(buf, sizeof(buf), "x%d", e.total_count);
            DrawTextA(hdc, buf, -1, &rcount, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            int bar_y = count_y + COUNT_H;
            RECT rbar = { cx, bar_y, cx + ICON_SIZE, bar_y + BAR_H };
            FillRect(hdc, &rbar, s_br_bar_bg);
            if (e.max_progress > 0 && e.max_progress <= 100)
            {
                int fill_w = (ICON_SIZE * e.max_progress) / 100;
                if (fill_w > 0)
                {
                    RECT rfill = { cx, bar_y, cx + fill_w, bar_y + BAR_H };
                    FillRect(hdc, &rfill, s_br_bar_fg);
                }
            }
        }

        SelectObject(hdc, old_font);
        SelectClipRgn(hdc, 0);
        TDrawArea__ReleaseDc(da, "pq_unit");
    }

    // --- TShape icon pass ---
    if (has_slp && TDrawArea__Lock(da, "pq_unit", 1))
    {
        for (int i = 0; i < n; i++)
        {
            RGE_Master_Static_Object* mo = player->master_objects[entries[i].master_id];
            if (!mo) continue;
            __int16 frame = mo->button_pict;
            if (frame < 0 || frame >= unit_slp->Num_Shapes) continue;
            Shape_Info* si = &unit_slp->shape_info[frame];
            int dx = origin_x + i * CELL_W + si->Hotspot_X;
            int dy = si->Hotspot_Y;
            TShape__shape_draw(unit_slp, da, dx, dy, frame, NULL);
        }
        TDrawArea__Unlock(da, "pq_unit");
    }
}

// ---------------------------------------------------------------------------
// draw_techqueue_overlay  (tech row)
// ---------------------------------------------------------------------------
void draw_techqueue_overlay(TDrawArea* da, int panel_w, const std::vector<TechEntry>& entries, HRGN clip_region, int row_y)
{
    ensure_brushes();

    TRIBE_Player* player = (TRIBE_Player*)RGE_Base_Game__get_player(*base_game);
    if (!player) return;

    // Resolve the tech-icon SLP for this civ (fall back to any loaded civ).
    int civ = player->culture;
    TShape* tech_slp = (iconsTechPtr && civ >= 1 && civ <= CIV_COUNT) ? iconsTechPtr[civ] : NULL;
    if (!tech_slp || !tech_slp->Is_Loaded || tech_slp->Num_Shapes <= 0)
    {
        if (iconsTechPtr)
        {
            for (int k = 1; k <= CIV_COUNT; k++)
            {
                TShape* s = iconsTechPtr[k];
                if (s && s->Is_Loaded && s->Num_Shapes > 0) { tech_slp = s; break; }
            }
        }
    }
    bool has_slp = tech_slp && tech_slp->Is_Loaded && tech_slp->Num_Shapes > 0;

    int n        = (int)entries.size();
    int origin_x = panel_w - n * CELL_W - MARGIN_R;
    if (origin_x < 0) origin_x = 0;

    // --- GDI pass: progress % label + progress bar ---
    if (TDrawArea__GetDc(da, "pq_tech"))
    {
        HDC hdc = da->DrawDc;
        RECT clip_rect = { 0, row_y, panel_w, row_y + ROW_H };
        TDrawArea__SetClipRect(da, &clip_rect);
        SelectClipRgn(hdc, clip_region);

        SetBkMode(hdc, TRANSPARENT);
        HGDIOBJ old_font = SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));

        for (int i = 0; i < n; i++)
        {
            const TechEntry& e = entries[i];
            int cx = origin_x + i * CELL_W;

            if (!has_slp)
            {
                RECT ricon = { cx, row_y, cx + ICON_SIZE, row_y + ICON_SIZE };
                FillRect(hdc, &ricon, s_br_icon);
            }

            int label_y = row_y + ICON_SIZE;
            RECT rlabel = { cx, label_y, cx + ICON_SIZE, label_y + COUNT_H };
            FillRect(hdc, &rlabel, s_br_bg);
            SetTextColor(hdc, RGB(140, 200, 255));   // light blue for techs
            char buf[8];
            snprintf(buf, sizeof(buf), "%d%%", (int)e.progress);
            DrawTextA(hdc, buf, -1, &rlabel, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            int bar_y = label_y + COUNT_H;
            RECT rbar = { cx, bar_y, cx + ICON_SIZE, bar_y + BAR_H };
            FillRect(hdc, &rbar, s_br_bar_bg);
            if (e.progress > 0)
            {
                int fill_w = (ICON_SIZE * e.progress) / 100;
                if (fill_w > 0)
                {
                    RECT rfill = { cx, bar_y, cx + fill_w, bar_y + BAR_H };
                    FillRect(hdc, &rfill, s_br_bar_tech);
                }
            }
        }

        SelectObject(hdc, old_font);
        SelectClipRgn(hdc, 0);
        TDrawArea__ReleaseDc(da, "pq_tech");
    }

    // --- TShape icon pass ---
    if (has_slp && TDrawArea__Lock(da, "pq_tech", 1))
    {
        for (int i = 0; i < n; i++)
        {
            __int16 frame = entries[i].icon;
            if (frame < 0 || frame >= tech_slp->Num_Shapes) continue;
            Shape_Info* si = &tech_slp->shape_info[frame];
            int dx = origin_x + i * CELL_W + si->Hotspot_X;
            int dy = row_y + si->Hotspot_Y;
            TShape__shape_draw(tech_slp, da, dx, dy, frame, NULL);
        }
        TDrawArea__Unlock(da, "pq_tech");
    }
}

// ---------------------------------------------------------------------------
// Overlay API callbacks
// ---------------------------------------------------------------------------

struct PQUserData
{
    bool had_queue;
};

static void* pq_create(const void* user_init)
{
    PQUserData* d = new PQUserData;
    d->had_queue = false;
    return d;
}

static void pq_destroy(void* user_data)
{
    delete (PQUserData*)user_data;
}

static bool pq_need_redraw(void* user_data)
{
    PQUserData* d = (PQUserData*)user_data;
    std::vector<PQEntry> units;
    bool has_units = collect_prodqueue(units);
    std::vector<TechEntry> techs;
    bool has_techs = collect_techqueue(techs);
    bool has_anything = has_units || has_techs;
    bool needs_redraw = has_anything || d->had_queue;
    d->had_queue = has_anything;
    return needs_redraw;
}

static panel_size pq_handle_size(void* user_data)
{
    // Span full parent width at top of game view.
    // Mode 7 (L+T+R anchored): left/right borders = 0 (flush to parent edges).
    panel_size s;
    s.left_border_in   = 0;
    s.top_border_in    = 2;
    s.right_border_in  = 0;
    s.bottom_border_in = 0;
    s.min_wid_in       = 100;
    s.max_wid_in       = 10000;
    s.min_hgt_in       = PANEL_H;
    s.max_hgt_in       = PANEL_H;
    return s;
}

static RECT pq_render_to_image_buffer(void* user_data, TDrawArea* render_area, RECT* render_rect, HRGN clip_region)
{
    int panel_w = render_rect->right;

    std::vector<PQEntry> units;
    collect_prodqueue(units);
    if (!units.empty())
        draw_prodqueue_overlay(render_area, panel_w, units, clip_region);

    std::vector<TechEntry> techs;
    collect_techqueue(techs);
    if (!techs.empty())
        draw_techqueue_overlay(render_area, panel_w, techs, clip_region, TECH_ROW_Y);

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
