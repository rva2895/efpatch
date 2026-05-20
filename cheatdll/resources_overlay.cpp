#include "stdafx.h"
#include "resources_overlay.h"
#include "spectator_overlay.h"

static const int ATTR_FOOD     = 0;
static const int ATTR_CARBON   = 1;
static const int ATTR_ORE      = 2;
static const int ATTR_NOVA     = 3;
static const int ATTR_POP_ROOM = 4;   // free pop slots (cap - current)
static const int ATTR_CIV_POP  = 11;  // civilian pop count
static const int ATTR_MIL_POP  = 12;  // military pop count

// Inline layout: [icon][value] × 5, left-to-right. Pop is wider for "cur/cap".
static const int ICON_W      = 24;
static const int ICON_H      = 24;
// SLPs draw their content near the top of the bounding box; nudge down to
// align the icon's centre with the text centre.
static const int ICON_DROP_Y = 4;
static const int VAL_W       = 34;   // resources: fits "9999" / "10.0k"
static const int VAL_W_POP   = 48;   // pop: fits "199/200"
static const int ICON_GAP    = 0;
static const int ITEM_GAP    = 3;
static const int ITEM_W      = ICON_W + ICON_GAP + VAL_W;
static const int ITEM_W_POP  = ICON_W + ICON_GAP + VAL_W_POP;
static const int N_CARDS     = 5;
static const int RES_STRIP_W = 4 * ITEM_W + ITEM_W_POP + 4 * ITEM_GAP;

// Match the queue view's compact_h so view-switch doesn't resize the panel.
static const int ROW_H_RES = 58;

// SLP 50731 in interfac.drs holds the resource icons
struct ResInfo { const char* letter; COLORREF color; int attr; int frame; };

static const int RES_SLP_ID    = 50731;
static const int RES_POP_FRAME = 5;   // objpaneldraw treats 5 as "other".

static const ResInfo RES[4] = {
    { "F", RGB( 60, 200,  70), ATTR_FOOD,   2 },
    { "C", RGB(200, 140,  45), ATTR_CARBON, 0 },
    { "O", RGB(180, 180, 190), ATTR_ORE,    1 },
    { "N", RGB( 90, 150, 230), ATTR_NOVA,   3 },
};
static const COLORREF POP_COLOR = RGB(255, 215, 80);

static TShape s_res_shp        = {};
static bool   s_res_shp_tried  = false;
static bool   s_res_shp_loaded = false;

static TShape* res_icons_slp()
{
    if (!s_res_shp_tried)
    {
        s_res_shp_tried = true;
        char fname[] = "interfac.drs";
        TShape__TShape(&s_res_shp, fname, RES_SLP_ID);
        s_res_shp_loaded = s_res_shp.Is_Loaded && s_res_shp.Num_Shapes > 0;
    }
    return s_res_shp_loaded ? &s_res_shp : NULL;
}

static void draw_res_value(HDC hdc, int item_x, int y, int val_w, int font_h,
                            COLORREF text_color,
                            const char* value, int value_len)
{
    SetTextColor(hdc, text_color);
    int tx = item_x + ICON_W + ICON_GAP;
    int ty = y + (ICON_H - font_h) / 2;
    if (ty < y) ty = y;
    RECT clip = { tx, y, tx + val_w, y + ICON_H };
    ExtTextOutA(hdc, tx, ty, ETO_CLIPPED, &clip, value, value_len, NULL);
}

static bool res_need_redraw()
{
    return true;  // resources change every tick; container gates on isRec()
}

static void res_render(TDrawArea* da, HRGN /*clip*/,
                       TRIBE_Player* player, int /*player_idx*/,
                       int x, int y, int w,
                       SpectatorLayout /*layout*/, SpectatorPass pass)
{
    if (!player || !player->attributes) return;

    int cx_base = x + w - RES_STRIP_W;
    if (cx_base < x) cx_base = x;

    int item_x[N_CARDS];
    for (int i = 0; i < 4; i++) item_x[i] = cx_base + i * (ITEM_W + ITEM_GAP);
    item_x[4] = cx_base + 4 * (ITEM_W + ITEM_GAP);

    if (pass == SP_PASS_SLP)
    {
        // Black backing per item so icon + text read against the panel bg.
        const unsigned __int8 pi_black = 0;
        for (int i = 0; i < 4; i++)
            TDrawArea__FillRect(da, item_x[i], y, item_x[i] + ITEM_W, y + ICON_H, pi_black);
        TDrawArea__FillRect(da, item_x[4], y, item_x[4] + ITEM_W_POP, y + ICON_H, pi_black);

        TShape* shp = res_icons_slp();
        if (!shp) return;  // GDI pass draws the letter fallback
        int iy = y + ICON_DROP_Y;
        for (int i = 0; i < 4; i++)
        {
            int f = RES[i].frame;
            if (f >= 0 && f < shp->Num_Shapes)
                TShape__shape_draw(shp, da, item_x[i], iy, f, NULL);
        }
        if (RES_POP_FRAME >= 0 && RES_POP_FRAME < shp->Num_Shapes)
            TShape__shape_draw(shp, da, item_x[4], iy, RES_POP_FRAME, NULL);
        return;
    }

    HDC hdc = da->DrawDc;
    if (!hdc) return;
    int font_h = sp_font_h();
    bool has_icons = res_icons_slp() != NULL;

    for (int i = 0; i < 4; i++)
    {
        int val = (int)player->attributes[RES[i].attr];
        char buf[12];
        int len;
        if (val >= 10000)
            len = snprintf(buf, sizeof(buf), "%.1fk", val / 1000.0f);
        else
            len = snprintf(buf, sizeof(buf), "%d", val);

        if (!has_icons)
        {
            SetTextColor(hdc, RES[i].color);
            RECT rletter = { item_x[i], y, item_x[i] + ICON_W, y + ICON_H };
            ExtTextOutA(hdc, item_x[i] + ICON_W / 2 - 4, y + (ICON_H - font_h) / 2,
                        ETO_CLIPPED, &rletter, RES[i].letter, 1, NULL);
        }
        draw_res_value(hdc, item_x[i], y, VAL_W, font_h, RES[i].color, buf, len);
    }

    int pop_cur = (int)(player->attributes[ATTR_CIV_POP] + player->attributes[ATTR_MIL_POP]);
    int pop_cap = (int)(pop_cur + player->attributes[ATTR_POP_ROOM]);
    char buf[16];
    int len = snprintf(buf, sizeof(buf), "%d/%d", pop_cur, pop_cap);
    if (!has_icons)
    {
        SetTextColor(hdc, POP_COLOR);
        RECT rletter = { item_x[4], y, item_x[4] + ICON_W, y + ICON_H };
        ExtTextOutA(hdc, item_x[4] + ICON_W / 2 - 4, y + (ICON_H - font_h) / 2,
                    ETO_CLIPPED, &rletter, "P", 1, NULL);
    }
    draw_res_value(hdc, item_x[4], y, VAL_W_POP, font_h, POP_COLOR, buf, len);
}

static const SpectatorViewDef s_res_view_def = {
    "Resources",
    ROW_H_RES,
    ROW_H_RES,
    res_render,
    res_need_redraw,
    NULL
};

void register_resources_spectator_view()
{
    register_spectator_view(s_res_view_def);
}
