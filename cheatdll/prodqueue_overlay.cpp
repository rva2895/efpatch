#include "stdafx.h"
#include "prodqueue_overlay.h"
#include "consts.h"

static const int ICON_SIZE = 36;
static const int COUNT_H   = 14;
static const int BAR_H     = 5;
static const int CELL_W    = ICON_SIZE + 2;
static const int MARGIN_R  = 8;

static HBRUSH s_br_bg     = NULL;
static HBRUSH s_br_bar_bg = NULL;
static HBRUSH s_br_bar_fg = NULL;
static HBRUSH s_br_icon   = NULL;

static void ensure_brushes()
{
    if (!s_br_bg)
    {
        s_br_bg     = CreateSolidBrush(RGB(  0,   0,   0));
        s_br_bar_bg = CreateSolidBrush(RGB( 40,  40,  40));
        s_br_bar_fg = CreateSolidBrush(RGB( 30, 160,  60));
        s_br_icon   = CreateSolidBrush(RGB( 60,  60,  80));
    }
}

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

    // --- GDI pass: count text + progress bar (+ fallback icon placeholder) ---
    if (TDrawArea__GetDc(da, "prodqueue"))
    {
        HDC hdc = da->DrawDc;
        RECT full_rect = { 0, 0, panel_w, ICON_SIZE + COUNT_H + BAR_H };
        TDrawArea__SetClipRect(da, &full_rect);
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
        TDrawArea__ReleaseDc(da, "prodqueue");
    }

    // --- TShape icon pass ---
    if (has_slp && TDrawArea__Lock(da, "prodqueue", 1))
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
        TDrawArea__Unlock(da, "prodqueue");
    }
}

// --- Overlay API callbacks ---

struct PQUserData
{
    bool had_queue;
};

static void* pq_create()
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
    std::vector<PQEntry> pq;
    bool has_queue = collect_prodqueue(pq);
    bool needs_redraw = has_queue || d->had_queue;
    d->had_queue = has_queue;
    return needs_redraw;
}

static panel_size pq_handle_size(void* user_data)
{
    // Span full parent width at top of game view, fixed height of 57px.
    // Mode 7 (L+T+R anchored): left=0, top=2, right=0 from edges; stretch width; fixed height.
    panel_size s;
    s.left_border_in   = 0;
    s.top_border_in    = 2;
    s.right_border_in  = 0;
    s.bottom_border_in = 0;
    s.min_wid_in       = 100;
    s.max_wid_in       = 10000;
    s.min_hgt_in       = ICON_SIZE + COUNT_H + BAR_H + 2;
    s.max_hgt_in       = ICON_SIZE + COUNT_H + BAR_H + 2;
    return s;
}

static void pq_render_to_image_buffer(void* user_data, TDrawArea* render_area, RECT* render_rect, HRGN clip_region)
{
    std::vector<PQEntry> entries;
    if (!collect_prodqueue(entries)) return;
    draw_prodqueue_overlay(render_area, render_rect->right, entries, clip_region);
}

void register_prodqueue_overlay()
{
    TRIBE_Panel_Screen_Overlay_User_Callbacks cb;
    cb.render_to_image_buffer = pq_render_to_image_buffer;
    cb.need_redraw            = pq_need_redraw;
    cb.handle_size            = pq_handle_size;
    cb.create                 = pq_create;
    cb.destroy                = pq_destroy;
    register_screen_overlay(cb);
}
