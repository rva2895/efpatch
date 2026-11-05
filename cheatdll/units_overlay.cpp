#include "stdafx.h"
#include "units_overlay.h"
#include "spectator_overlay.h"
#include "prodqueue_overlay.h"

// Match the queue view's compact_h so view-switch doesn't resize the panel.
static const int ROW_H_RES = 58;

bool collect_units_for_player(TRIBE_Player* player, std::vector<UEntry>& entries)
{
    entries.clear();
    if (!player || !player->objects) return false;

    for (int i = 0; i < player->objects->Number_of_objects; i++)
    {
        RGE_Static_Object* obj = player->objects->List[i];
        if (!obj || !obj->master_obj) continue;
        if (obj->master_obj->master_type != 70) continue;
        if (obj->master_obj->object_group == 1) continue;

        TRIBE_Combat_Object* combat_obj = (TRIBE_Combat_Object*)obj;

        short converted_master_id = combat_obj->master_obj->object_group == 58 ? 83 : combat_obj->master_obj->id;

        int idx = -1;
        for (int k = 0; k < (int)entries.size(); k++)
            if (entries[k].master_id == converted_master_id) { idx = k; break; }

        if (idx < 0)
        {
            UEntry e;
            e.master_id = converted_master_id;
            e.total_count = 1;
            entries.push_back(e);
        }
        else
        {
            UEntry& e = entries[idx];
            e.total_count++;
        }
    }
    return !entries.empty();
}

bool collect_units(std::vector<UEntry>& entries)
{
    entries.clear();
    if (!*base_game || !(*base_game)->world) return false;
    return collect_units_for_player((TRIBE_Player*)RGE_Base_Game__get_player(*base_game), entries);
}

extern inline int pq_count_h(int icon_size);
extern inline int pq_bar_h(int icon_size);
// Cell step ? native SLP width + 8 px gutter so icons don't bleed into neighbours.
extern inline int pq_cell_step(int icon_size);
extern TShape* resolve_unit_slp(TRIBE_Player* player);

void draw_units_overlay_pass(TDrawArea* da, int cell_x, int cell_w,
    const std::vector<UEntry>& entries,
    HRGN /* clip_region */, TRIBE_Player* player, int row_y,
    int icon_size, bool right_align,
    SpectatorPass pass)
{
    if (!player) return;

    int count_h_i = pq_count_h(icon_size);
    int cell_w_i = pq_cell_step(icon_size);

    int n = (int)entries.size();
    int origin_x = right_align
        ? max(cell_x, cell_x + cell_w - n * cell_w_i - 8)
        : cell_x;

    int strip_w = max(icon_size, SLP_NATIVE_H);

    if (pass == SP_PASS_SLP)
    {
        // Palette idx 0 is true black on this surface; other colours go through
        // nearest-match (close enough for bars that don't need text contrast).
        const unsigned __int8 pi_black = 0;
        unsigned __int8 pi_icon = pal_index(RGB(60, 60, 80));
        TShape* unit_slp = resolve_unit_slp(player);

        int icon_h = max(icon_size, SLP_NATIVE_H);

        for (int i = 0; i < n; i++)
        {
            int cx = origin_x + i * cell_w_i;

            if (!unit_slp)
                TDrawArea__FillRect(da, cx, row_y, cx + icon_size, row_y + icon_h, pi_icon);

            int cy = row_y + icon_h;
            TDrawArea__FillRect(da, cx, cy, cx + strip_w, cy + count_h_i, pi_black);

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
        const UEntry& e = entries[i];
        int cx = origin_x + i * cell_w_i;
        int cy = row_y + icon_h;
        char buf[12];
        int  len = snprintf(buf, sizeof(buf), "%d", e.total_count);

        SIZE sz;
        GetTextExtentPoint32A(hdc, buf, len, &sz);
        int tx = cx + (strip_w - sz.cx) / 2;
        int ty = cy + (count_h_i - font_h) / 2;
        if (ty < cy) ty = cy;
        //RECT clip = { cx, cy, cx + strip_w, cy + count_h_i };
        //ExtTextOutA(hdc, tx, ty, ETO_CLIPPED, &clip, buf, len, NULL);
        TextOutA(hdc, tx, ty, buf, len);
    }
}

struct UnitsCacheEntry {
    std::vector<UEntry> units;
};

struct UnitsOverlayUserData {
    UnitsCacheEntry s_queue_cache[9]; // index 1..8; 0 unused (gaia)
};

static bool units_need_redraw(void* /* user_data */)
{
    return true;  // resources change every tick; container gates on isRec()
}

static void* units_create()
{
    UnitsOverlayUserData* units_user_data = new UnitsOverlayUserData;

    return units_user_data;
}

static void units_destroy(void* user_data)
{
    UnitsOverlayUserData* units_user_data = (UnitsOverlayUserData*)user_data;

    delete units_user_data;
}

static void units_begin_frame(void* user_data)
{
    UnitsOverlayUserData* units_user_data = (UnitsOverlayUserData*)user_data;

    if (!*base_game || !(*base_game)->world) return;
    int pnum = (*base_game)->world->player_num;
    if (pnum > 9) pnum = 9;
    for (int i = 1; i < pnum; i++)
    {
        TRIBE_Player* p = (*base_game)->world->players[i];
        units_user_data->s_queue_cache[i].units.clear();
        if (!p) continue;
        collect_units_for_player(p, units_user_data->s_queue_cache[i].units);
    }
}

static int units_render(TDrawArea* da, HRGN clip,
    TRIBE_Player* player,
    int x, int y, int w,
    SpectatorLayout layout, SpectatorPass pass,
    void* user_data)
{
    UnitsOverlayUserData* units_user_data = (UnitsOverlayUserData*)user_data;

    if (player->id < 1 || player->id > 8) return 0;
    const std::vector<UEntry>& units = units_user_data->s_queue_cache[player->id].units;

    if (layout == SP_FULL)
    {
        int cell_w_c = max(ICON_SIZE_C, 36) + 4;
        if (!units.empty())
            draw_units_overlay_pass(da, x, w, units, clip, player, y, ICON_SIZE, false, pass);

        return units.size() * (cell_w_c + MARGIN_R / 2) + MARGIN_R * 2;
    }
    else // SP_COMPACT: both right-aligned; techs at far right, units to their left
    {
        int cell_w_c = max(ICON_SIZE_C, 36) + 4;
        if (!units.empty())
            draw_units_overlay_pass(da, x, w, units, clip, player, y, ICON_SIZE_C, true, pass);

        int unit_strip_w = units.empty() ? 0 : ((int)units.size() * (cell_w_c + MARGIN_R / 2));
        return unit_strip_w + MARGIN_R * 2;
    }
}

static const SpectatorViewDef s_units_view_def = {
    "Units",
    114,
    ROW_H_RES,
    units_render,
    units_need_redraw,
    units_begin_frame,
    units_create,
    units_destroy,
};

void register_units_spectator_view()
{
    register_spectator_view(s_units_view_def);
}
