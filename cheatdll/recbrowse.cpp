#include "stdafx.h"
#include "recbrowse.h"
#include "itemcache.h"
#include "mg1file.h"
#include "registry.h"
#include <ddraw.h>
#include <process.h>
#include <string>
#include <vector>
#include <time.h>

extern CONFIG_DATA cd;

ITEM_CACHE<REC_DATA>* rec_cache = NULL;
ITEM_CACHE<SCEN_DATA>* scen_cache = NULL;

int prev_rec_viewport = -1;
int prev_rec_position = -1;

int prev_scen_viewport = -1;
int prev_scen_position = -1;

extern const char* savegame_path;
extern const char* scenario_path;

std::string rec_extension;
std::string save_extension;

HFONT font_bk;

bool loaded_player_brushes = false;
HBRUSH hb[9];

COLORREF player_colors[] =
{
    RGB(0, 0, 255),
    RGB(255, 0, 0),
    RGB(0, 255, 0),
    RGB(255, 255, 0),
    RGB(0, 255, 255),
    RGB(255, 0, 255),
    RGB(48, 48, 48),
    RGB(128, 0, 0),
    RGB(255, 255, 255)
};

#define BGR(b,g,r) (RGB(r,g,b))

COLORREF get_bgr_player_color(COLORREF c)
{
    return BGR(GetRValue(c), GetGValue(c), GetBValue(c));
}

COLORREF get_color(unsigned char c)
{
    //all colors are BGR !!
    switch (c)
    {
    case 0:
        return BGR(0, 169, 0);
    case 1:
        return BGR(48, 93, 182);
    case 2:
        return BGR(248, 201, 138);
    case 3:
        return cd.gameVersion == VER_EF ? BGR(185, 185, 185) : BGR(191, 169, 115);
    case 4:
        return BGR(116, 115, 75);
    case 5:
        return cd.gameVersion == VER_EF ? BGR(0, 169, 0) : BGR(0, 169, 0);
    case 6:
        return BGR(243, 170, 92);
    case 7:
    case 8:
        return BGR(138, 139, 87);
    case 9:
        return BGR(0, 169, 0);
    case 10:
        return BGR(37, 116, 57);
    case 11:
        return BGR(243, 170, 92);
    case 12:
        return BGR(0, 169, 0);
    case 13:
        return BGR(37, 116, 57);
    case 14:
        return BGR(248, 201, 138);
    case 15:
        return BGR(255, 210, 255);
    case 16:
        return BGR(67, 67, 67);
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
        return BGR(37, 116, 57);
    case 22:
        return BGR(0, 74, 161);
    case 23:
        return BGR(0, 53, 135);
    case 24:
    case 25:
        return BGR(243, 170, 92);
    case 26:
        return cd.gameVersion == VER_EF ? BGR(232, 180, 120) : BGR(84, 115, 125);
    case 27:
        return BGR(191, 169, 115);
    case 28:
        return BGR(48, 93, 182);
    case 29:
    case 30:
    case 31:
        return BGR(138, 139, 87);
    case 32:
    case 33:
        return BGR(0, 169, 0);
    case 34:
        return cd.gameVersion == VER_EF ? BGR(185, 185, 185) : BGR(0, 169, 0);
    case 35:
        return cd.gameVersion == VER_EF ? BGR(216, 223, 255) : BGR(248, 201, 138);
    case 36:
        return cd.gameVersion == VER_EF ? BGR(191, 169, 115) : BGR(0, 169, 0);
    case 37:
        return BGR(152, 192, 240);
    case 38:
        return cd.gameVersion == VER_EF ? BGR(185, 185, 185) : BGR(243, 170, 92);
    case 39:
        return cd.gameVersion == VER_EF ? BGR(185, 185, 185) : BGR(191, 169, 115);
    case 40:
        return cd.gameVersion == VER_EF ? BGR(134, 126, 118) : BGR(243, 170, 92);
    case 41:
        return cd.gameVersion == VER_EF ? BGR(0, 169, 0) : BGR(0, 169, 0);
    case 42:
        return cd.gameVersion == VER_EF ? BGR(84, 115, 125) : BGR(84, 115, 125);
    case 43:
        return cd.gameVersion == VER_EF ? BGR(185, 185, 185) : BGR(191, 169, 115);
    case 44:
        return cd.gameVersion == VER_EF ? BGR(84, 115, 125) : BGR(84, 115, 125);
    case 45:
    case 46:
        return BGR(248, 201, 138);
    case 47:
        return cd.gameVersion == VER_EF ? BGR(243, 170, 92) : BGR(0, 169, 0);
    case 48:
        return BGR(138, 139, 87);
    case 49:
    case 50:
        return BGR(151, 206, 255);
    case 51:
        return cd.gameVersion == VER_EF ? BGR(181, 16, 14) : BGR(150, 36, 0);
    case 52:
        return cd.gameVersion == VER_EF ? BGR(84, 115, 125) : BGR(84, 115, 125);
    case 53:
        return BGR(116, 115, 75);
    //EF
    case 54:
        return BGR(248, 201, 138);
    case 55:
        return BGR(0, 169, 0);
    case 56:
        return BGR(28, 28, 28);
    case 57:
    case 58:
        return BGR(84, 115, 125);
    case 59:
        return BGR(116, 115, 75);
    case 60:
        return BGR(0, 169, 0);
    case 61:
    case 62:
        return BGR(248, 201, 138);
    case 63:
        return BGR(0, 169, 0);
    case 64:
        return BGR(243, 170, 92);
    case 65:
        return BGR(158, 56, 48);
    case 66:
        return BGR(0, 169, 0);
    case 67:
    case 68:
    case 69:
        return BGR(191, 169, 115);
    case 70:
        return BGR(0, 169, 0);
    case 71:
        return BGR(243, 170, 92);
    case 72:
        return BGR(84, 115, 125);
    case 73:
    case 74:
    case 75:
    case 76:
    case 77:
    case 78:
    case 79:
    case 80:
    case 81:
    case 82:
    case 83:
    case 84:
    case 85:
    case 86:
        return BGR(37, 116, 57);
    case 87:
    case 88:
    case 89:
    case 90:
        return BGR(151, 206, 255);
    case 91:
        return BGR(84, 115, 125);
    case 92:
        return BGR(151, 206, 255);
    case 93:
    case 94:
    case 95:
        return BGR(191, 169, 115);
    case 96:
    case 97:
    case 98:
    case 99:
        return BGR(185, 185, 185);
    case 100:
        return BGR(84, 115, 125);
    case 101:
        return BGR(156, 197, 217);
    case 102:
    case 103:
        return BGR(248, 201, 138);
    //104-128: FORBIDDEN
    case 129:
    case 130:
        return BGR(232, 180, 120);
    case 131:
        return BGR(243, 170, 92);
    case 132:
        return BGR(84, 115, 125);
    case 133:
        return BGR(248, 201, 138);
    case 134:
    case 135:
        return BGR(84, 115, 125);
    case 136:
        return BGR(185, 185, 185);
    case 137:
    case 138:
        return BGR(191, 169, 115);
    case 139:
        return BGR(48, 93, 182);
    case 140:
        return BGR(0, 74, 161);
    case 141:
        return BGR(0, 53, 135);
    case 142:
        return BGR(116, 115, 75);
    case 143:
        return BGR(0, 169, 0);
    case 144:
    case 145:
    case 146:
        return BGR(84, 115, 125);
    case 147:
        return BGR(232, 180, 120);
    case 148:
    case 149:
    case 150:
    case 151:
        return BGR(84, 115, 125);
    case 152:
    case 153:
    case 154:
    case 155:
        return BGR(0, 169, 0);
    case 156:
        return BGR(156, 197, 217);
    case 157:
    case 158:
    case 159:
    case 160:
        return BGR(216, 223, 255);
    case 161:
    case 162:
        return BGR(0, 169, 0);
    case 163:
    case 164:
        return BGR(84, 115, 125);
    case 165:
        return BGR(191, 169, 115);
    case 166:
        return BGR(232, 180, 120);
    case 167:
    case 168:
        return BGR(243, 170, 92);
    case 169:
    case 170:
        return BGR(248, 201, 138);
    case 171:
        return BGR(196, 226, 116);
    case 172:
        return BGR(243, 170, 92);
    case 173:
        return BGR(191, 169, 115);
    case 174:
        return BGR(0, 169, 0);
    case 175:
        return BGR(185, 185, 185);
    case 176:
        return BGR(151, 206, 255);
    case 177:
    case 178:
    case 179:
        return BGR(37, 116, 57);
    case 180:
        return BGR(243, 170, 92);
    case 181:
    case 182:
        return BGR(84, 115, 125);
    case 183:
    case 184:
        return BGR(134, 126, 118);
    case 185:
        return BGR(243, 170, 92);
    case 186:
        return BGR(84, 115, 125);
    case 187:
    case 188:
    case 189:
        return BGR(232, 180, 120);
    case 190:
    case 191:
        return BGR(84, 115, 125);
    case 192:
    case 193:
    case 194:
        return BGR(243, 170, 92);
    case 195:
        return BGR(0, 169, 0);
    case 196:
        return BGR(243, 170, 92);
    case 197:
    case 198:
        return BGR(84, 115, 125);
    case 199:
    case 200:
        return BGR(0, 169, 0);
    case 201:
        return BGR(255, 210, 255);
    case 202:
        return BGR(0, 53, 135);
    case 203:
        return BGR(0, 74, 161);
    case 204:
    case 205:
    case 206:
    case 207:
        return BGR(37, 116, 57);
    case 208:
        return BGR(216, 223, 255);
    case 209:
        return BGR(134, 126, 118);
    case 210:
    case 211:
        return BGR(255, 210, 255);
    case 212:
    case 213:
        return BGR(243, 170, 92);
    case 214:
        return BGR(84, 115, 125);
    case 215:
        return BGR(84, 115, 125);
    case 216:
    case 217:
        return BGR(0, 169, 0);
    case 218:
        return BGR(113, 153, 36);
    case 219:
        return BGR(84, 115, 125);
    case 220:
        return BGR(232, 180, 120);
    case 221:
        return BGR(156, 197, 217);
    case 222:
    case 223:
        return BGR(84, 115, 125);
    case 224:
    case 225:
        return BGR(185, 185, 185);
    case 226:
    case 227:
    case 228:
        return BGR(138, 139, 87);
    case 229:
        return BGR(37, 116, 57);
    case 230:
    case 231:
        return BGR(84, 115, 125);
    case 232:
        return BGR(151, 206, 255);
    case 233:
        return BGR(255, 210, 255);
    case 234:
        return BGR(243, 170, 92);
    case 235:
        return BGR(37, 116, 57);
    case 236:
        return BGR(158, 56, 48);
    case 237:
        return BGR(84, 115, 125);
    case 238:
        return BGR(216, 223, 255);
    case 239:
        return BGR(248, 201, 138);
    case 240:
    case 241:
    case 242:
    case 243:
    case 244:
        return BGR(37, 116, 57);
    case 245:
        return BGR(151, 206, 255);
    case 246:
        return BGR(116, 115, 75);
    case 247:
        return BGR(151, 206, 255);
    case 248:
    case 249:
        return BGR(134, 126, 118);
    case 250:
        return BGR(151, 206, 255);
    default:
        return BGR(255, 255, 255);
    }
}

int screenToMap_x(int x, int y, int view_x, int view_y, int m)
{
    UNREFERENCED_PARAMETER(view_y);
    return ((float)y - (float)view_x / 4) / ((float)view_x / 2) * m +
        (float)x * m / (float)view_x;
}

int screenToMap_y(int x, int y, int view_x, int view_y, int m)
{
    UNREFERENCED_PARAMETER(view_y);
    return -((float)y - (float)view_x / 4) / ((float)view_x / 2) * m +
        (float)x * m / (float)view_x;
}

int mapToScreen_x(int x, int y, int view_x, int view_y, int m)
{
    UNREFERENCED_PARAMETER(view_y);
    return (x + y) * view_x / (2 * m);
}

int mapToScreen_y(int x, int y, int view_x, int view_y, int m)
{
    UNREFERENCED_PARAMETER(view_y);
    return (x - y) * (view_x / 2) / (2 * m) + view_x / 4;
}

HBITMAP make_bitmap(int x, int y, int view_x, int view_y, TILE* map)
{
    UNREFERENCED_PARAMETER(y);
    void* data = malloc(view_x*view_y * 4);
    memset(data, 0, view_x*view_y * 4);

    if (map)
        for (int j = 0; j < view_y; j++)
            for (int i = 0; i < view_x; i++)
            {
                int map_x = screenToMap_x(i, j, view_x, view_y, x);
                int map_y = screenToMap_y(i, j, view_x, view_y, x);
                if ((map_x > 0) && (map_y > 0) &&
                    (map_x < x) && (map_y < x))
                    //((COLORREF*)data)[j*view_x+i] = RGB(255, 0, 0);
                {
                    ((COLORREF*)data)[(view_y - j)*view_x + i] = get_color(map[map_y*x + map_x].terrain);
                }
            }

    HBITMAP hBitmap = CreateBitmap(view_x, view_y, 1, 32, data);
    free(data);
    return hBitmap;
}

HBITMAP make_bitmap_for_scen(int view_x, int view_y, SCEN& scen)
{
    void* data = malloc(view_x * view_y * 4);
    memset(data, 0, view_x * view_y * 4);

    for (int j = 0; j < view_y; j++)
        for (int i = 0; i < view_x; i++)
        {
            int map_x = screenToMap_x(i, j, view_x, view_y, scen.map.x);
            int map_y = screenToMap_y(i, j, view_x, view_y, scen.map.y);
            if ((map_x > 0) && (map_y > 0) &&
                (map_x < scen.map.x) && (map_y < scen.map.y))
                //((COLORREF*)data)[j*view_x+i] = RGB(255, 0, 0);
            {
                ((COLORREF*)data)[(view_y - j) * view_x + i] = get_color(scen.map.terrain_data[map_y * scen.map.x + map_x].id);
            }
        }

    //draw objects
    for (int i = 0; i < (scen.player_data3.number_of_players); i++)
    {
        for (int j = 0; j < scen.units.player_units[i].unit_count; j++)
        {
            int scr_x = mapToScreen_x(
                scen.units.player_units[i].units[j].x,
                scen.units.player_units[i].units[j].y,
                view_x,
                view_y,
                scen.map.x);
            int scr_y = mapToScreen_y(
                scen.units.player_units[i].units[j].x,
                scen.units.player_units[i].units[j].y,
                view_x,
                view_y,
                scen.map.x);
            ((COLORREF*)data)[(view_y - scr_y) * view_x + scr_x] =
                get_bgr_player_color(player_colors[i > 0 ? scen.player_data3.player_data3_sub[i - 1].color : 8]);
        }
    }

    HBITMAP hBitmap = CreateBitmap(view_x, view_y, 1, 32, data);
    free(data);
    return hBitmap;
}

bool load_screen_font = false;

void setup_screen_font()
{
    load_screen_font = true;
    font_bk = CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"));
}

void __stdcall paint_save_game_screen_bk(TribeLoadSavedGameScreen* SaveGameScreen)
{
    if (!SaveGameScreen->list)
        return;

    IDirectDrawSurface* s = SaveGameScreen->render_area->DrawSurface;

    if (!load_screen_font)
        setup_screen_font();

    int x = SaveGameScreen->pnl_wid;
    int y = SaveGameScreen->pnl_hgt;

    TListPanel* SaveGameList = SaveGameScreen->list;

    RECT r;

    //element sizes based on resolution
    int map_edge_dist = y > 900 ? 10 : 8;
    int color_box_size = y > 900 ? 16 : 14;
    int color_box_dist = y > 900 ? 40 : 30;
    int team_dist = y > 900 ? 20 : 15;

    int cc_size = 0;
    //y > 900 ? 6 : 4;
    if (x <= 1024)
        cc_size = 4;
    else if (x <= 1280)
        cc_size = 5;
    else if (x <= 1600)
        cc_size = 6;
    else if (x <= 1920)
        cc_size = 7;
    else
        cc_size = 9;

    int text_offset = y > 900 ? 7 : 3;
    int color_box_offset = y > 900 ? 17 : 10;
    int player_name_offset = y > 900 ? 42 : 32;

    r.left = 515 * (x / 800.0); //was 535
    r.right = 766 * (x / 800.0);
    r.top = 110 * (y / 600.0);
    r.bottom = 529 * (y / 600.0);

    if (!rec_cache)
        rec_cache = new ITEM_CACHE<REC_DATA>;

    if (!rec_cache->isValid())
        rec_cache->init((TScreenPanel*)SaveGameScreen, r.right - r.left - 4, (r.right - r.left - 4) / 2, item_cache_thread<REC_DATA>);

    HDC hdc;
    s->GetDC(&hdc);

    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    MoveToEx(hdc, r.left, r.top, NULL);
    LineTo(hdc, r.left, r.bottom);
    LineTo(hdc, r.right, r.bottom);
    LineTo(hdc, r.right, r.top);
    LineTo(hdc, r.left, r.top);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    r.left++;
    r.right--;
    r.top++;
    r.bottom--;

    HBRUSH brush_bk = CreateSolidBrush(RGB(24, 24, 24));
    FillRect(hdc, &r, brush_bk);

    bool is_recorded_game_file = TTextPanel__get_id2((TTextPanel*)SaveGameList, SaveGameList->cur_line) == 1;

    char* file = SaveGameList->vfptr->get_text(SaveGameList, SaveGameList->cur_line);
    std::string filename = (std::string)savegame_path + (std::string)file + (is_recorded_game_file ? rec_extension : save_extension);
    rec_cache->set_current_file(filename);

    REC_DATA rd;
    if (is_recorded_game_file)
        rd = rec_cache->get_item(filename, 2);
    else
    {
        rd = { 0 };
        rd.exists = true;
        rd.valid = false;
    }

    //draw players
    if (!loaded_player_brushes)
    {
        for (int i = 0; i <= 8; i++)
            hb[i] = CreateSolidBrush(player_colors[i]);

        loaded_player_brushes = true;
    }

    char str[0x100];
    if (rd.exists)
        if (rd.valid)
        {
            rd.duration /= 1000;
            rd.start_time /= 1000;
            int total = rd.start_time + rd.duration;
            snprintf(str, _countof(str), "%d:%02d:%02d (%d:%02d:%02d - %d:%02d:%02d)",
                rd.duration / 3600, (rd.duration / 60) % 60, rd.duration % 60,
                rd.start_time / 3600, (rd.start_time / 60) % 60, rd.start_time % 60,
                total / 3600, (total / 60) % 60, total % 60);
            HDC hdcMem = CreateCompatibleDC(hdc);
            HGDIOBJ oldBitmap = SelectObject(hdcMem, rd.map.get()->get_map());
            BITMAP bitmap;

            GetObject(rd.map.get()->get_map(), sizeof(bitmap), &bitmap);
            BitBlt(hdc, r.left + 1, r.top + 1, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

            SelectObject(hdcMem, oldBitmap);
            DeleteDC(hdcMem);

            RECT r_cc;
            for (int i = 0; i < rd.n_team1; i++)
            {
                if ((rd.team_1_cc_x[i] >= 0) && (rd.team_1_cc_y[i] >= 0))
                {
                    r_cc.left = r.left + mapToScreen_x(rd.team_1_cc_x[i], rd.team_1_cc_y[i], r.right - r.left, r.bottom - r.top, rd.map_x) - cc_size;
                    r_cc.right = r.left + mapToScreen_x(rd.team_1_cc_x[i], rd.team_1_cc_y[i], r.right - r.left, r.bottom - r.top, rd.map_x) + cc_size;
                    r_cc.top = r.top + mapToScreen_y(rd.team_1_cc_x[i], rd.team_1_cc_y[i], r.right - r.left, r.bottom - r.top, rd.map_x) - cc_size;
                    r_cc.bottom = r.top + mapToScreen_y(rd.team_1_cc_x[i], rd.team_1_cc_y[i], r.right - r.left, r.bottom - r.top, rd.map_x) + cc_size;

                    HBRUSH hb_c;
                    if ((rd.team_1_colors[i] >= 0) && (rd.team_1_colors[i] < 8))
                        hb_c = hb[rd.team_1_colors[i]];
                    else
                        hb_c = hb[8];
                    FillRect(hdc, &r_cc, hb_c);
                }
            }
            for (int i = 0; i < rd.n_team2; i++)
            {
                if ((rd.team_2_cc_x[i] >= 0) && (rd.team_2_cc_y[i] >= 0))
                {
                    r_cc.left = r.left + mapToScreen_x(rd.team_2_cc_x[i], rd.team_2_cc_y[i], r.right - r.left, r.bottom - r.top, rd.map_x) - cc_size;
                    r_cc.right = r.left + mapToScreen_x(rd.team_2_cc_x[i], rd.team_2_cc_y[i], r.right - r.left, r.bottom - r.top, rd.map_x) + cc_size;
                    r_cc.top = r.top + mapToScreen_y(rd.team_2_cc_x[i], rd.team_2_cc_y[i], r.right - r.left, r.bottom - r.top, rd.map_x) - cc_size;
                    r_cc.bottom = r.top + mapToScreen_y(rd.team_2_cc_x[i], rd.team_2_cc_y[i], r.right - r.left, r.bottom - r.top, rd.map_x) + cc_size;

                    HBRUSH hb_c;
                    if ((rd.team_2_colors[i] >= 0) && (rd.team_2_colors[i] < 8))
                        hb_c = hb[rd.team_2_colors[i]];
                    else
                        hb_c = hb[8];
                    FillRect(hdc, &r_cc, hb_c);
                }
            }
        }
        else
            snprintf(str, _countof(str), "Preview not available");
    else
        snprintf(str, _countof(str), "Loading...");

    r.top += (r.right - r.left) / 2 + map_edge_dist;

    r.left += text_offset;

    HANDLE hOld = SelectObject(hdc, font_bk);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    DrawText(hdc, str, strlen(str), &r, DT_LEFT | DT_WORDBREAK);

    if (rd.exists && rd.valid)
    {
        r.left -= text_offset;

        RECT r_ver;
        r_ver.right = r.right;
        r_ver.bottom = r.bottom;
        r_ver.top = r_ver.bottom - 20;
        r_ver.left = r_ver.right - 60;

        FillRect(hdc, &r_ver, brush_bk);

        //snprintf(str, _countof(str), "VER %s", (char*)&rd.version);
        strcpy_safe(str, _countof(str), rd.version.c_str());
        DrawText(hdc, str, strlen(str), &r_ver, DT_RIGHT);

        r.top += (y > 900 ? 30 : 24);

        RECT color_box;
        color_box.left = r.left + color_box_offset;
        color_box.top = r.top + 10;
        color_box.right = color_box.left + color_box_size;
        color_box.bottom = color_box.top + color_box_size;
        RECT player_name;
        player_name.left = r.left + player_name_offset;
        player_name.top = r.top + (y > 900 ? 10 : 8);
        player_name.right = r.right - 10;
        player_name.bottom = player_name.top + 20;

        //draw player boxes
        hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
        hOldPen = (HPEN)SelectObject(hdc, hPen);

        for (int i = 0; i < rd.n_team1; i++)
        {
            HBRUSH hb_c;
            if ((rd.team_1_colors[i] >= 0) && (rd.team_1_colors[i] < 8))
                hb_c = hb[rd.team_1_colors[i]];
            else
                hb_c = hb[8];
            FillRect(hdc, &color_box, hb_c);
            MoveToEx(hdc, color_box.left, color_box.top, NULL);
            LineTo(hdc, color_box.left, color_box.bottom);
            LineTo(hdc, color_box.right, color_box.bottom);
            LineTo(hdc, color_box.right, color_box.top);
            LineTo(hdc, color_box.left, color_box.top);
            color_box.top += color_box_dist;
            color_box.bottom += color_box_dist;
            DrawText(hdc, rd.team_1[i].c_str(), rd.team_1[i].length(), &player_name, DT_LEFT);
            player_name.top += color_box_dist;
            player_name.bottom += color_box_dist;
        }
        color_box.top += team_dist;
        color_box.bottom += team_dist;
        player_name.top += team_dist;
        player_name.bottom += team_dist;
        for (int i = 0; i < rd.n_team2; i++)
        {
            HBRUSH hb_c;
            if ((rd.team_2_colors[i] >= 0) && (rd.team_2_colors[i] < 8))
                hb_c = hb[rd.team_2_colors[i]];
            else
                hb_c = hb[8];
            FillRect(hdc, &color_box, hb_c);
            MoveToEx(hdc, color_box.left, color_box.top, NULL);
            LineTo(hdc, color_box.left, color_box.bottom);
            LineTo(hdc, color_box.right, color_box.bottom);
            LineTo(hdc, color_box.right, color_box.top);
            LineTo(hdc, color_box.left, color_box.top);
            color_box.top += color_box_dist;
            color_box.bottom += color_box_dist;
            DrawText(hdc, rd.team_2[i].c_str(), rd.team_2[i].length(), &player_name, DT_LEFT);
            player_name.top += color_box_dist;
            player_name.bottom += color_box_dist;
        }

        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
    }
    //DrawText(hdc, players.c_str(), players.length(), &r, DT_LEFT | DT_WORDBREAK);

    DeleteObject(brush_bk);

    SelectObject(hdc, hOld);
    s->ReleaseDC(hdc);

    //prepare closest 5
    if (prev_rec_position != SaveGameList->cur_line)
    {
        int cache_min = (SaveGameList->cur_line - 5) > 0 ? (SaveGameList->cur_line - 5) : 0;
        int cache_max = (SaveGameList->cur_line + 5) < SaveGameList->num_lines ? (SaveGameList->cur_line + 5) : SaveGameList->num_lines;
        for (int i = cache_min; i < cache_max; i++)
        {
            if (TTextPanel__get_id2((TTextPanel*)SaveGameList, i) == 1)
            {
                file = SaveGameList->vfptr->get_text(SaveGameList, i);
                filename = (std::string)savegame_path + (std::string)file + rec_extension;
                rec_cache->get_item(filename, 1);
            }
        }
    }

    prev_rec_position = SaveGameList->cur_line;
    //prepare viewport
    if (prev_rec_viewport != SaveGameList->top_line)
    {
        int cache_top = (SaveGameList->top_line - 5) > 0 ? (SaveGameList->top_line - 5) : 0;
        int cache_bottom = (SaveGameList->bot_line + 5) < SaveGameList->num_lines ? (SaveGameList->bot_line + 5) : SaveGameList->num_lines;
        for (int i = cache_top; i < cache_bottom; i++)
        {
            if (TTextPanel__get_id2((TTextPanel*)SaveGameList, i) == 1)
            {
                file = SaveGameList->vfptr->get_text(SaveGameList, i);
                filename = (std::string)savegame_path + (std::string)file + rec_extension;
                rec_cache->get_item(filename, 0);
            }
        }
    }
    prev_rec_viewport = SaveGameList->top_line;
}

TListPanel* get_list(TScreenPanel* panel)
{
    switch (DWORD(panel->vfptr))
    {
    case 0x00661F3C:
        return ((TRIBE_Screen_Sed_Open*)panel)->list;
    case 0x00662128:
        return ((TribeSelectScenarioScreen*)panel)->scenarioList;
    default:
        return NULL;
    }
}

void __stdcall paint_load_scen_screen_bk(TScreenPanel* LoadScenScreen)
{
    TListPanel* ScenList = get_list(LoadScenScreen);

    if (!ScenList)
        return;

    IDirectDrawSurface* s = LoadScenScreen->render_area->DrawSurface;

    if (!load_screen_font)
        setup_screen_font();

    int x = LoadScenScreen->pnl_wid;
    int y = LoadScenScreen->pnl_hgt;

    RECT r;

    r.left = 515 * (x / 800.0); //was 535
    r.right = 760 * (x / 800.0);
    r.top = 87 * (y / 600.0);
    r.bottom = 521 * (y / 600.0);

    if (!scen_cache)
        scen_cache = new ITEM_CACHE<SCEN_DATA>;

    if (!scen_cache->isValid())
        scen_cache->init((TScreenPanel*)LoadScenScreen, r.right - r.left - 4, (r.right - r.left - 4) / 2, item_cache_thread<SCEN_DATA>);

    HDC hdc;
    s->GetDC(&hdc);

    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    MoveToEx(hdc, r.left, r.top, NULL);
    LineTo(hdc, r.left, r.bottom);
    LineTo(hdc, r.right, r.bottom);
    LineTo(hdc, r.right, r.top);
    LineTo(hdc, r.left, r.top);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    r.left++;
    r.right--;
    r.top++;
    r.bottom--;

    HBRUSH brush_bk = CreateSolidBrush(RGB(24, 24, 24));
    FillRect(hdc, &r, brush_bk);

    char* file = ScenList->vfptr->get_text(ScenList, ScenList->cur_line);

    auto convert_list_id_to_scen_extension = [](int id)
        {
            switch (id)
            {
            case 2:
                return ".sc2";
            case 1:
                return ".sc1";
            case 0:
                return ".scx";
            default:
                return "";
            }
        };

    std::string filename =
        (std::string)scenario_path +
        (std::string)file +
        convert_list_id_to_scen_extension(TTextPanel__get_id2((TTextPanel*)ScenList, ScenList->cur_line));

    scen_cache->set_current_file(filename);

    SCEN_DATA sd = scen_cache->get_item(filename, 2);

    char str[0x1000];
    if (sd.exists)
        if (sd.valid)
        {
            int n_humans = 0;
            int n_computers = 0;

            for (int i = 0; i < 16; i++)
                if (sd.active[i])
                    sd.human[i] ? n_humans++ : n_computers++;

            tm lt;
            time_t t = sd.timestamp;
            localtime_s(&lt, &t);

            char s_time[0x40];

            strftime(s_time, _countof(s_time), "%Y-%m-%d_%H:%M:%S", &lt);

            snprintf(str, _countof(str), "Players: %d human%s, %d computer%s\nTrigger count: %d\nTimestamp: %s\n\nInstructions:\n%s",
                //sd.orig_filename.c_str(),
                n_humans, n_humans == 1 ? "" : "s", n_computers, n_computers == 1 ? "" : "s",
                sd.trigger_count,
                s_time,
                sd.instructions.c_str());


            HDC hdcMem = CreateCompatibleDC(hdc);
            HGDIOBJ oldBitmap = SelectObject(hdcMem, sd.map.get()->get_map());
            BITMAP bitmap;

            GetObject(sd.map.get()->get_map(), sizeof(bitmap), &bitmap);
            BitBlt(hdc, r.left + 1, r.top + 1, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

            SelectObject(hdcMem, oldBitmap);
            DeleteDC(hdcMem);
        }
        else
            snprintf(str, _countof(str), "Preview not available");
    else
        snprintf(str, _countof(str), "Loading...");

    int map_edge_dist = y > 900 ? 10 : 8;
    int text_offset = y > 900 ? 7 : 3;

    r.top += (r.right - r.left) / 2 + map_edge_dist;

    r.left += text_offset;

    HANDLE hOld = SelectObject(hdc, font_bk);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    DrawText(hdc, str, strlen(str), &r, DT_LEFT | DT_WORDBREAK);

    if (sd.exists && sd.valid)
    {
        RECT r_ver;
        r_ver.right = r.right;
        r_ver.bottom = r.bottom;
        r_ver.top = r_ver.bottom - 20;
        r_ver.left = r_ver.right - 90;

        FillRect(hdc, &r_ver, brush_bk);

        //snprintf(str, _countof(str), "VER %s", (char*)&rd.version);
        //strcpy_safe(str, _countof(str), rd.version.c_str());
        snprintf(
            str,
            _countof(str),
            "%.02f (%s)",
            sd.version,
            convert_list_id_to_scen_extension(TTextPanel__get_id2((TTextPanel*)ScenList, ScenList->cur_line)));

        //SetBkMode(hdc, OPAQUE);
        //SetBkColor(hdc, RGB(24, 24, 24));
        DrawText(hdc, str, strlen(str), &r_ver, DT_RIGHT);
        //SetBkMode(hdc, TRANSPARENT);
    }

    DeleteObject(brush_bk);

    SelectObject(hdc, hOld);
    s->ReleaseDC(hdc);

    //prepare closest 5
    if (prev_scen_position != ScenList->cur_line)
    {
        int cache_min = (ScenList->cur_line - 5) > 0 ? (ScenList->cur_line - 5) : 0;
        int cache_max = (ScenList->cur_line + 5) < ScenList->num_lines ? (ScenList->cur_line + 5) : ScenList->num_lines;
        for (int i = cache_min; i < cache_max; i++)
        {
            file = ScenList->vfptr->get_text(ScenList, ScenList->cur_line + i);
            filename =
                (std::string)scenario_path +
                (std::string)file +
                convert_list_id_to_scen_extension(TTextPanel__get_id2((TTextPanel*)ScenList, i));
            scen_cache->get_item(filename, 1);
        }
    }
    prev_scen_position = ScenList->cur_line;
    //prepare viewport
    if (prev_scen_viewport != ScenList->top_line)
    {
        int cache_top = (ScenList->top_line - 5) > 0 ? (ScenList->top_line - 5) : 0;
        int cache_bottom = (ScenList->bot_line + 5) < ScenList->num_lines ? (ScenList->bot_line + 5) : ScenList->num_lines;
        for (int i = cache_top; i < cache_bottom; i++)
        {
            file = ScenList->vfptr->get_text(ScenList, i);
            filename =
                (std::string)scenario_path +
                (std::string)file +
                convert_list_id_to_scen_extension(TTextPanel__get_id2((TTextPanel*)ScenList, i));
            scen_cache->get_item(filename, 0);
        }
    }
    prev_scen_viewport = ScenList->top_line;

    return;
}

void __stdcall invalidate_rec_cache()
{
    prev_rec_viewport = -1;
    prev_rec_position = -1;
    if (rec_cache)
        rec_cache->invalidate();
}

void __stdcall invalidate_scen_cache()
{
    prev_scen_viewport = -1;
    prev_scen_position = -1;
    if (scen_cache)
        scen_cache->invalidate();
}

__declspec(naked) void on_delete_save_game_screen() //0050A7D0
{
    __asm
    {
        push    ecx
        call    invalidate_rec_cache
        pop     ecx
        push    0FFFFFFFFh
        push    00643E58h
        mov     eax, 0050A7D7h
        jmp     eax
    }
}

__declspec(naked) void on_delete_select_scen_screen1() //0053FBB0
{
    __asm
    {
        push    ecx
        call    invalidate_scen_cache
        pop     ecx
        push    0FFFFFFFFh
        push    00644698h
        mov     eax, 0053FBB7h
        jmp     eax
    }
}

__declspec(naked) void on_delete_select_scen_screen2() //00540670
{
    __asm
    {
        push    ecx
        call    invalidate_scen_cache
        pop     ecx
        push    0FFFFFFFFh
        push    006446F8h
        mov     eax, 00540677h
        jmp     eax
    }
}

void __stdcall draw_bk_dispatch(TPanel* panel)
{
    switch ((DWORD)panel->vfptr)
    {
    case 0x006607F4:
        paint_save_game_screen_bk((TribeLoadSavedGameScreen*)panel);
        break;
    case 0x00661F3C:
    case 0x00662128:
        paint_load_scen_screen_bk((TScreenPanel*)panel);
        break;
    default:
        break;
    }
}

__declspec(naked) void on_draw_bk() //004B899F
{
    __asm
    {
        push    esi
        call    draw_bk_dispatch
        mov     eax, [esi]
        mov     ecx, esi
        call    dword ptr [eax + 38h]
        mov     ebx, 004B89A6h
        jmp     ebx
    }
}

__declspec(naked) void on_set_select_scen_x() //00540372
{
    __asm
    {
        mov     dword ptr [esi + 7F4h], 30
        mov     ecx, 00540378h
        jmp     ecx
    }
}

#pragma optimize( "s", on )
void setRecBrowseHooks(int version)
{
    setHook((void*)0x0050A7D0, on_delete_save_game_screen);
    setHook((void*)0x0053FBB0, on_delete_select_scen_screen1);
    setHook((void*)0x00540670, on_delete_select_scen_screen2);
    setHook((void*)0x004B899F, on_draw_bk);

    //file list
    writeByte(0x0050A5E2, 35); //x
    writeDword(0x0050A5DB, 460); //width, was 415

    //sed open
    writeByte(0x0053FA0E, 30);
    writeByte(0x0053F9E2, 30);
    writeDword(0x0053FA07, 370);

    //select scen
    setHook((void*)0x00540372, on_set_select_scen_x);
    writeDword(0x0054036E, 370);

    switch (version)
    {
    case VER_CC:
        rec_extension = ".mg1";
        save_extension = ".ga1";
        break;
    case VER_EF:
        rec_extension = ".mg2";
        save_extension = ".ga2";
        break;
    default:
        rec_extension.clear();
        save_extension.clear();
        break;
    }
}
#pragma optimize( "", on )
