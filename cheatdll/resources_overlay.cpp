#include "stdafx.h"
#include "resources_overlay.h"
#include "spectator_overlay.h"

// ---------------------------------------------------------------------------
// Attribute indices
// ---------------------------------------------------------------------------
static const int ATTR_FOOD     = 0;
static const int ATTR_CARBON   = 1;
static const int ATTR_ORE      = 2;
static const int ATTR_NOVA     = 3;
static const int ATTR_POP_ROOM = 4;   // free pop slots (cap - current)
static const int ATTR_CIV_POP  = 11;  // civilian unit pop count
static const int ATTR_MIL_POP  = 12;  // military unit pop count

// ---------------------------------------------------------------------------
// Card layout – mirrors unit-icon card style
// ---------------------------------------------------------------------------
static const int CARD_W      = 40;   // card width (wide enough for "999/200")
static const int CARD_ICN_H  = 24;   // coloured icon area height
static const int CARD_VAL_H  = 10;   // value text strip below icon
static const int CARD_TOTAL  = CARD_ICN_H + CARD_VAL_H;   // 34 px
static const int N_CARDS     = 5;    // Food, Carbon, Ore, Nova, Pop

// Keep compact_h in sync with the queue view (37 px) so switching views
// doesn't cause a panel resize.
static const int ROW_H_RES   = 37;

// ---------------------------------------------------------------------------
// Per-resource descriptors
// ---------------------------------------------------------------------------
struct ResInfo { const char* letter; COLORREF color; int attr; };

static const ResInfo RES[4] = {
    { "F", RGB( 60, 200,  70), ATTR_FOOD   },
    { "C", RGB(200, 140,  45), ATTR_CARBON },
    { "O", RGB(180, 180, 190), ATTR_ORE    },
    { "N", RGB( 90, 150, 230), ATTR_NOVA   },
};
static const COLORREF POP_COLOR = RGB(255, 215, 80);

// ---------------------------------------------------------------------------
// Cached brushes
// ---------------------------------------------------------------------------
static HBRUSH s_br_icon_bg[N_CARDS] = {};  // dark tinted bg per resource
static HBRUSH s_br_card_dark        = NULL;
static HBRUSH s_br_val_bg           = NULL;

static void ensure_res_brushes()
{
    if (s_br_card_dark) return;
    s_br_card_dark = CreateSolidBrush(RGB(15, 15, 15));
    s_br_val_bg    = CreateSolidBrush(RGB( 0,  0,  0));
    for (int i = 0; i < 4; i++)
    {
        COLORREF c = RES[i].color;
        // Dim tint of the resource colour for the card interior
        s_br_icon_bg[i] = CreateSolidBrush(
            RGB(GetRValue(c) / 6, GetGValue(c) / 6, GetBValue(c) / 6));
    }
    s_br_icon_bg[4] = CreateSolidBrush(
        RGB(GetRValue(POP_COLOR) / 6, GetGValue(POP_COLOR) / 6, GetBValue(POP_COLOR) / 6));
}

// ---------------------------------------------------------------------------
// Draw one card at (cx, cy)
// ---------------------------------------------------------------------------
static void draw_res_card(HDC hdc, int cx, int cy,
                          HBRUSH tint_brush, COLORREF text_color,
                          const char* letter, const char* value)
{
    // Dark outer border / background
    RECT rcard = { cx, cy, cx + CARD_W, cy + CARD_ICN_H };
    FillRect(hdc, &rcard, s_br_card_dark);

    // Coloured tint fill (1px inset)
    RECT rinner = { cx + 1, cy + 1, cx + CARD_W - 1, cy + CARD_ICN_H - 1 };
    FillRect(hdc, &rinner, tint_brush);

    // Letter centred in icon area
    SetTextColor(hdc, text_color);
    DrawTextA(hdc, letter, -1, &rcard, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Value strip below
    RECT rval = { cx, cy + CARD_ICN_H, cx + CARD_W, cy + CARD_ICN_H + CARD_VAL_H };
    FillRect(hdc, &rval, s_br_val_bg);
    SetTextColor(hdc, text_color);
    DrawTextA(hdc, value, -1, &rval, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

// ---------------------------------------------------------------------------
// View callbacks
// ---------------------------------------------------------------------------
static bool res_need_redraw()
{
    return true;  // resources change every tick; container gates on isRec()
}

static void res_render(TDrawArea* da, HRGN /*clip*/,
                       TRIBE_Player* player, int /*player_idx*/,
                       int x, int y, int w, SpectatorLayout /*layout*/)
{
    if (!player || !player->attributes) return;
    if (!TDrawArea__GetDc(da, "sp_res")) return;

    ensure_res_brushes();

    HDC hdc = da->DrawDc;
    SetBkMode(hdc, TRANSPARENT);
    HGDIOBJ old_font = SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));

    // Right-align all N_CARDS cards within [x, x+w]
    int strip_w = N_CARDS * CARD_W;
    int cx      = x + w - strip_w;
    if (cx < x) cx = x;

    // 4 resource cards
    for (int i = 0; i < 4; i++)
    {
        int val = (int)player->attributes[RES[i].attr];
        char buf[12];
        if (val >= 10000)
            snprintf(buf, sizeof(buf), "%.1fk", val / 1000.0f);
        else
            snprintf(buf, sizeof(buf), "%d", val);

        draw_res_card(hdc, cx + i * CARD_W, y,
                      s_br_icon_bg[i], RES[i].color,
                      RES[i].letter, buf);
    }

    // Population card
    {
        int pop_cur = (int)(player->attributes[ATTR_CIV_POP] + player->attributes[ATTR_MIL_POP]);
        int pop_cap = (int)(pop_cur + player->attributes[ATTR_POP_ROOM]);
        char buf[16];
        snprintf(buf, sizeof(buf), "%d/%d", pop_cur, pop_cap);
        draw_res_card(hdc, cx + 4 * CARD_W, y,
                      s_br_icon_bg[4], POP_COLOR,
                      "P", buf);
    }

    SelectObject(hdc, old_font);
    TDrawArea__ReleaseDc(da, "sp_res");
}

// ---------------------------------------------------------------------------
// View registration
// ---------------------------------------------------------------------------
static const SpectatorViewDef s_res_view_def = {
    "Resources",
    ROW_H_RES,   // full_h
    ROW_H_RES,   // compact_h – matches queue view so no resize on tab switch
    res_render,
    res_need_redraw
};

void register_resources_spectator_view()
{
    register_spectator_view(s_res_view_def);
}
