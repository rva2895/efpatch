#include "stdafx.h"
#include "resources_overlay.h"
#include "spectator_overlay.h"

static const int ATTR_FOOD     = 0;
static const int ATTR_CARBON   = 1;
static const int ATTR_ORE      = 2;
static const int ATTR_NOVA     = 3;
static const int ATTR_POP_ROOM = 4;   // population headroom
static const int ATTR_CUR_POP  = 11;  // current population
static const int ATTR_POP_CAP  = 32;  // population cap

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
    { "C", RGB(90, 150, 230), ATTR_CARBON, 0 },
    { "F", RGB(200, 140,  45), ATTR_FOOD,   2 },
    { "N", RGB(60, 200,  70), ATTR_NOVA,   3 },
    { "O", RGB(180, 180, 190), ATTR_ORE,    1 },
};
static const COLORREF POP_COLOR = RGB(255, 215, 80);

struct ResourcesOverlayUserData
{
    TShape* s_res_shp;
    bool s_res_shp_loaded;
};

static TShape* res_icons_slp(ResourcesOverlayUserData* res_user_data)
{
    return res_user_data->s_res_shp_loaded ? res_user_data->s_res_shp : NULL;
}

static void* res_create()
{
    ResourcesOverlayUserData* res_user_data = new ResourcesOverlayUserData;

    res_user_data->s_res_shp = (TShape*)operator_new_internal(sizeof(TShape));
    TShape__TShape(res_user_data->s_res_shp, "", RES_SLP_ID);
    res_user_data->s_res_shp_loaded =
        res_user_data->s_res_shp->Is_Loaded && res_user_data->s_res_shp->Num_Shapes > 0;

    return res_user_data;
}

static void res_destroy(void* user_data)
{
    ResourcesOverlayUserData* res_user_data = (ResourcesOverlayUserData*)user_data;

    TShape__destructor(res_user_data->s_res_shp);
    operator_delete_internal(res_user_data->s_res_shp);

    delete res_user_data;
}

static void draw_res_value(HDC hdc, int item_x, int y, int /* val_w */, int font_h,
                            COLORREF text_color,
                            const char* value, int value_len)
{
    SetTextColor(hdc, text_color);
    int tx = item_x + ICON_W + ICON_GAP;
    int ty = y + (ICON_H - font_h) / 2;
    if (ty < y) ty = y;
    //RECT clip = { tx, y, tx + val_w, y + ICON_H };
    //ExtTextOutA(hdc, tx, ty, ETO_CLIPPED, &clip, value, value_len, NULL);
    TextOutA(hdc, tx, ty, value, value_len);
}

static bool res_need_redraw(void* /* user_data */)
{
    return true;  // resources change every tick; container gates on isRec()
}

static int res_render(TDrawArea* da, HRGN /*clip*/,
                       TRIBE_Player* player,
                       int x, int y, int w,
                       SpectatorLayout /*layout*/, SpectatorPass pass,
                       void* user_data)
{
    ResourcesOverlayUserData* res_user_data = (ResourcesOverlayUserData*)user_data;

    if (!player || !player->attributes) return 0;

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

        TShape* shp = res_icons_slp(res_user_data);
        if (!shp) return 0;  // GDI pass draws the letter fallback
        int iy = y + ICON_DROP_Y;
        for (int i = 0; i < 4; i++)
        {
            int f = RES[i].frame;
            if (f >= 0 && f < shp->Num_Shapes)
                TShape__shape_draw(shp, da, item_x[i], iy, f, NULL);
        }
        if (RES_POP_FRAME >= 0 && RES_POP_FRAME < shp->Num_Shapes)
            TShape__shape_draw(shp, da, item_x[4], iy, RES_POP_FRAME, NULL);
        return 0;
    }

    HDC hdc = da->DrawDc;
    if (!hdc) return 0;
    int font_h = sp_font_h();
    bool has_icons = res_icons_slp(res_user_data) != NULL;

    UINT prev_align = SetTextAlign(hdc, TA_RIGHT);

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
            //RECT rletter = { item_x[i], y, item_x[i] + ICON_W, y + ICON_H };
            //ExtTextOutA(hdc, item_x[i] + ICON_W / 2 - 4, y + (ICON_H - font_h) / 2,
            //            ETO_CLIPPED, &rletter, RES[i].letter, 1, NULL);
            TextOutA(hdc, item_x[i] + ICON_W / 2 - 4, y + (ICON_H - font_h) / 2, RES[i].letter, 1);
        }
        draw_res_value(hdc, item_x[i] + VAL_W, y, VAL_W, font_h, RES[i].color, buf, len);
    }

    int pop_cur = (int)(player->attributes[ATTR_CUR_POP]);
    int pop_cap = (int)(min(pop_cur + player->attributes[ATTR_POP_ROOM], player->attributes[ATTR_POP_CAP]));
    char buf[16];
    int len = snprintf(buf, sizeof(buf), "%d/%d", pop_cur, pop_cap);
    if (!has_icons)
    {
        SetTextColor(hdc, POP_COLOR);
        //RECT rletter = { item_x[4], y, item_x[4] + ICON_W, y + ICON_H };
        //ExtTextOutA(hdc, item_x[4] + ICON_W / 2 - 4, y + (ICON_H - font_h) / 2,
        //            ETO_CLIPPED, &rletter, "P", 1, NULL);
        TextOutA(hdc, item_x[4] + ICON_W / 2 - 4, y + (ICON_H - font_h) / 2, "P", 1);
    }
    draw_res_value(hdc, item_x[4] + VAL_W_POP, y, VAL_W_POP, font_h, POP_COLOR, buf, len);

    SetTextAlign(hdc, prev_align);

    return 4 * (ITEM_W + ITEM_GAP) + (ITEM_W_POP + ITEM_GAP);
}

static const SpectatorViewDef s_res_view_def = {
    "Resources",
    ROW_H_RES,
    ROW_H_RES,
    res_render,
    res_need_redraw,
    NULL,
    res_create,
    res_destroy,
};

void register_resources_spectator_view()
{
    register_spectator_view(s_res_view_def);
}
