#include "stdafx.h"
#include "prodqueue_overlay.h"
#include "consts.h"

extern TShape** iconsUnitPtr;   // iconsUnitPtr[civ] → TShape* for civ's unit icons

static const int ICON_SIZE    = 36;  // width & height of command-button icon
static const int COUNT_H      = 14;  // height of count text row
static const int BAR_H        = 5;   // height of progress bar
static const int CELL_W       = ICON_SIZE + 2; // horizontal pitch between cells
static const int OVERLAY_Y    = 44;  // Y position (pixels from top of panel)
static const int MARGIN_R     = 12;  // gap from right edge of panel
static const int TOTAL_H      = ICON_SIZE + COUNT_H + BAR_H;

struct PQEntry
{
    __int16 master_id;
    int     total_count;
    __int16 max_progress;
};

// GDI brushes
static HBRUSH s_br_bg     = NULL;
static HBRUSH s_br_bar_bg = NULL;
static HBRUSH s_br_bar_fg = NULL;

static void ensure_brushes()
{
    if (!s_br_bg)
    {
        s_br_bg     = CreateSolidBrush(RGB(  0,   0,   0));
        s_br_bar_bg = CreateSolidBrush(RGB( 40,  40,  40));
        s_br_bar_fg = CreateSolidBrush(RGB( 30, 160,  60));
    }
}

void __stdcall draw_prodqueue_overlay(TRIBE_Panel_Object* obj_panel)
{
    if (!obj_panel || !obj_panel->player)  return;
    if (!obj_panel->render_area)           return;
    if (!*base_game || !(*base_game)->world) return;

    TDrawArea* da = obj_panel->render_area;
    ensure_brushes();

    TRIBE_Player* player = (TRIBE_Player*)RGE_Base_Game__get_player(*base_game);
    if (!player || !player->objects) return;

    TRIBE_Screen_Game* gs = ((TRIBE_Game*)(*base_game))->game_screen;
    if (!gs) return;

    int civ = player->culture;
    TShape* unit_slp = NULL;
    if (iconsUnitPtr && civ >= 1 && civ <= CIV_COUNT)
        unit_slp = iconsUnitPtr[civ];
    if ((!unit_slp || !unit_slp->Is_Loaded || unit_slp->Num_Shapes <= 0) && iconsUnitPtr)
    {
        for (int k = 1; k <= CIV_COUNT; k++)
        {
            TShape* s = iconsUnitPtr[k];
            if (s && s->Is_Loaded && s->Num_Shapes > 0)
            {
                unit_slp = s;
                break;
            }
        }
    }
    bool has_unit_slp = unit_slp && unit_slp->Is_Loaded && unit_slp->Num_Shapes > 0;

    std::vector<PQEntry>   entries;
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

    if (entries.empty()) return;

    int n         = (int)entries.size();
    int overlay_x = da->Width - n * CELL_W - MARGIN_R;
    if (overlay_x < 0) overlay_x = 0;

    if (has_unit_slp && TDrawArea__Lock(da, "prodqueue", 1))
    {
        for (int i = 0; i < n; i++)
        {
            RGE_Master_Static_Object* mo = player->master_objects[entries[i].master_id];
            if (!mo) continue;
            __int16 frame = mo->button_pict;
            if (frame < 0 || frame >= unit_slp->Num_Shapes) continue;
            Shape_Info* si = &unit_slp->shape_info[frame];
            int dx = overlay_x + i * CELL_W + si->Hotspot_X;
            int dy = OVERLAY_Y + si->Hotspot_Y;
            TShape__shape_draw(unit_slp, da, dx, dy, frame, NULL);
        }
        TDrawArea__Unlock(da, "prodqueue");
    }
    else if (!has_unit_slp)
    {
        if (TDrawArea__GetDc(da, ""))
        {
            HDC hdc = da->DrawDc;
            for (int i = 0; i < n; i++)
            {
                int cx = overlay_x + i * CELL_W;
                RECT ricon = { cx, OVERLAY_Y, cx + ICON_SIZE, OVERLAY_Y + ICON_SIZE };
                FillRect(hdc, &ricon, s_br_bar_bg);
            }
            TDrawArea__ReleaseDc(da, "");
        }
    }

    if (!TDrawArea__GetDc(da, "")) return;
    HDC hdc = da->DrawDc;

    SetBkMode(hdc, TRANSPARENT);
    HGDIOBJ old_font = SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));

    for (int i = 0; i < n; i++)
    {
        const PQEntry& e = entries[i];
        int cx      = overlay_x + i * CELL_W;
        int count_y = OVERLAY_Y + ICON_SIZE;

        // Count text
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
    TDrawArea__ReleaseDc(da, "");
}
