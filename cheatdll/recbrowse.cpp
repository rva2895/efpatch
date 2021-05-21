#include "stdafx.h"
#include "recbrowse.h"
#include "mg1file.h"
#include <ddraw.h>
#include <process.h>
#include <string>
#include <vector>

extern HWND hWnd_main;

REC_CACHE* rec_cache = 0;

HANDLE worker_thread_event;

int prev_viewport = -1;
int prev_position = -1;

int draw_count = 0;

std::string rec_extension;

unsigned int __stdcall rec_cache_thread(void*)
{
    //initialize worker thread and set event
    MSG msg;
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(worker_thread_event);

    std::vector<std::pair<std::string, int>> priority_queue;
    while (true)
    {
        //check message queue
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            switch (msg.message)
            {
            case WM_APP + 1001:    //file
            {
                int i = 0;
                std::string file((char*)msg.wParam);
                //check if already exists
                for (i = 0; i < priority_queue.size(); i++)
                    if (priority_queue[i].first == file)
                        break;
                //remove old if found
                if (i < priority_queue.size())
                    priority_queue.erase(priority_queue.begin() + i);

                priority_queue.push_back(std::pair<std::string, int>(file, (int)msg.lParam));
                free((void*)msg.wParam);
                break;
            }
            case WM_APP + 1002:    //end
                delete rec_cache;
                rec_cache = 0;
                return 0;
                break;
            default:
                break;
            }
        }

        draw_count = priority_queue.size();

        //process messages if available
        //find last priority message
        int found_at = -1;
        for (int i = priority_queue.size() - 1; i >= 0; i--)
            if (priority_queue[i].second == 2)
            {
                if (found_at == -1)
                    found_at = i;
                else  //older high priority selections are no longer focused -> low priority
                    priority_queue[i].second = 0;
            }
        //find medium priority
        if (found_at == -1)
            for (int i = priority_queue.size() - 1; i >= 0; i--)
                if (priority_queue[i].second == 1)
                {
                    found_at = i;
                    break;
                }
        //find low priority
        if (found_at == -1)
            for (int i = priority_queue.size() - 1; i >= 0; i--)
                if (priority_queue[i].second == 0)
                {
                    found_at = i;
                    break;
                }
        //if found anything
        if (found_at != -1)
        {
            auto it = priority_queue.begin() + found_at;
            MG1 mg1(it->first.c_str());

            REC_DATA rd;
            rd.exists = true;
            rd.file = it->first;

            priority_queue.erase(it);

            if (mg1.loaded)
            {
                rd.valid = true;
                rd.duration = mg1.d.duration;
                rd.start_time = mg1.d.start_time;

                rd.map = make_bitmap(mg1.d.map_x, mg1.d.map_y, rec_cache->getX(), rec_cache->getY(), mg1.d.map);

                TEAM team = mg1.getTeam1();
                rd.n_team1 = team.n;
                for (int i = 0; i < team.n; i++)
                {
                    rd.team_1[i] = (std::string)team.names[i];
                    rd.team_1_colors[i] = team.colors[i];
                    rd.team_1_civs[i] = team.civs[i];
                    rd.team_1_cc_x[i] = team.cc_x[i];
                    rd.team_1_cc_y[i] = team.cc_y[i];
                }

                team = mg1.getTeam2();
                rd.n_team2 = team.n;
                for (int i = 0; i < team.n; i++)
                {
                    rd.team_2[i] = (std::string)team.names[i];
                    rd.team_2_colors[i] = team.colors[i];
                    rd.team_2_civs[i] = team.civs[i];
                    rd.team_2_cc_x[i] = team.cc_x[i];
                    rd.team_2_cc_y[i] = team.cc_y[i];
                }
                rd.map_x = mg1.d.map_x;
                rd.version = mg1.version;
            }
            else
                rd.valid = false;

            //rec_cache->ack_queue(rd.file);
            rec_cache->add_rec_data(rd);
            rec_cache->update(rd.file);
        }
        else
            Sleep(10);

        //truncate the queue
        if (priority_queue.size() > CACHE_SIZE)
        {
            //remove priority 0 first
            std::vector<int> indices;
            for (int i = 0; i < priority_queue.size(); i++)
                if ((priority_queue.size() - indices.size()) >(CACHE_SIZE - 10))
                {
                    if (priority_queue[i].second == 0)
                        indices.push_back(i);
                }
                else
                {
                    break;
                }
            for (int i = (indices.size() - 1); i >= 0; i--)
                priority_queue.erase(priority_queue.begin() + i);
        }
    }
    return 0;
}

std::string current_file = "";

void REC_CACHE::update(const std::string& file)
{
    if (state_valid && (file == current_file))
        SendMessage(hWnd_main, WM_APP + 1000, (WPARAM)wnd, 0);
}

bool rec_cache_invalid = true;

void REC_CACHE::invalidate()
{
    EnterCriticalSection(&cs);
    state_valid = false;
    rec_cache_invalid = true;
    PostThreadMessage(tid, WM_APP + 1002, 0, 0);
    LeaveCriticalSection(&cs);
}

REC_CACHE::REC_CACHE(void* wnd_, int x_, int y_)
{
    //clear cache
    rec_cache_invalid = false;
    state_valid = true;
    wnd = wnd_;
    x = x_;
    y = y_;
    memset(cache, 0, sizeof(cache));
    for (int i = 0; i < CACHE_SIZE; i++)
        cache[i] = new REC_DATA;
    //init critical secion
    InitializeCriticalSection(&cs);
    //init cache thread
    worker_thread_event = CreateEvent(NULL, FALSE, FALSE, 0);
    _beginthreadex(NULL, 0, rec_cache_thread, NULL, 0, &tid);
    WaitForSingleObject(worker_thread_event, INFINITE);
    CloseHandle(worker_thread_event);
}

REC_DATA REC_CACHE::get_rec_data(const std::string& f, int priority)
{
    EnterCriticalSection(&cs);
    REC_DATA rd;
    int index = get_rec_cache_index(f);
    if (index != -1)
    {
        REC_DATA* prd = cache[index];
        for (int i = index; i > 0; i--)
            cache[i] = cache[i - 1];
        cache[0] = prd;
        rd = *prd;
    }
    else
    {
        char* s = (char*)malloc(f.length() + 1);
        strcpy(s, f.c_str());
        //bool found = false;
        //bool higher_priority = false;
        //for (auto it = queue.begin(); it != queue.end(); ++it)
        //    if (it->first == f)
        //if (priority > it->second)
        //{
        //    it->second = priority;
        //    higher_priority = true;
        //}
        //else
        //{
        //            found = true;
        //}
        //if (!found)
        //{
        //if (!higher_priority)
        //        queue.push_back(std::pair<std::string, int>(f, priority));
        PostThreadMessage(tid, WM_APP + 1001, (WPARAM)s, priority);
        //}
        rd = { 0 };
    }
    LeaveCriticalSection(&cs);
    return rd;
}

REC_CACHE::~REC_CACHE()
{
    for (int i = 0; i < CACHE_SIZE; i++)
    {
        if (cache[i]->map)
            DeleteObject(cache[i]->map);
        delete cache[i];
    }
}

/*void REC_CACHE::ack_queue(std::string f)
{
EnterCriticalSection(&cs);
for (auto it = queue.begin(); it != queue.end(); ++it)
if (it->first == f)
{
queue.erase(it);
break;
}
LeaveCriticalSection(&cs);
}*/

void REC_CACHE::add_rec_data(const REC_DATA& rd)
{
    EnterCriticalSection(&cs);
    if (get_rec_cache_index(rd.file) == -1)
    {
        if (cache[CACHE_SIZE - 1]->map)
            DeleteObject(cache[CACHE_SIZE - 1]->map);
        delete cache[CACHE_SIZE - 1];
        for (int i = (CACHE_SIZE - 1); i > 0; i--)
            cache[i] = cache[i - 1];
        cache[0] = new REC_DATA;
        *cache[0] = rd;
    }
    LeaveCriticalSection(&cs);
}

int REC_CACHE::get_rec_cache_index(const std::string& f)
{
    for (int i = 0; i < CACHE_SIZE; i++)
        if ((cache[i]->exists) && (cache[i]->file == f))
            return i;
    return -1;
}

LPDIRECTDRAWSURFACE7 load_bk = NULL;
void* wnd_list;

HFONT font_bk;

bool loaded_player_brushes = false;
HBRUSH hb[9];
HWND hWndPlayers;

char colors[16];

#define BGR(b,g,r) (RGB(r,g,b))

COLORREF get_color(char c)
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
        return BGR(191, 169, 115);
    case 4:
        return BGR(116, 115, 75);
    case 5:
        return BGR(0, 169, 0);
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
        return BGR(84, 115, 125);
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
    case 34:
        return BGR(0, 169, 0);
    case 35:
        return BGR(248, 201, 138);
    case 36:
        return BGR(0, 169, 0);
    case 37:
        return BGR(152, 192, 240);
    case 38:
        return BGR(243, 170, 92);
    case 39:
        return BGR(191, 169, 115);
    case 40:
        return BGR(243, 170, 92);
    case 41:
        return BGR(0, 169, 0);
    case 42:
        return BGR(84, 115, 125);
    case 43:
        return BGR(191, 169, 115);
    case 44:
        return BGR(84, 115, 125);
    case 45:
    case 46:
        return BGR(248, 201, 138);
    case 47:
        return BGR(0, 169, 0);
    case 48:
        return BGR(138, 139, 87);
    case 49:
    case 50:
        return BGR(151, 206, 255);
    case 51:
        return BGR(150, 36, 0);
    case 52:
        return BGR(84, 115, 125);
    case 53:
        return BGR(116, 115, 75);
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

//HBRUSH brush_bk;

bool load_screen_font = false;

void init_listbk()
{
    //font_bk = CreateFont(15, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
    //    OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
    //    DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"));

    //brush_bk = CreateSolidBrush(RGB(16, 16, 16));
}

char* (__thiscall* window_getSelText)(void* this_, int sel) =
(char* (__thiscall*) (void*, int))0x004D3E20;

int get_index(void* wnd)
{
    if (wnd)
        return *(short*)((DWORD)wnd + 0x104);
    else
        return -1;
}

int get_scroll_position(void* wnd)
{
    if (wnd)
        return *(short*)((DWORD)wnd + 0x100);
    else
        return -1;
}

void* wnd_bk = 0;

void __stdcall paintOnScreen_loadbk(LPDIRECTDRAWSURFACE7 s, void* wnd)
{
    if (!wnd_list)
        return;

    if (*(int*)((DWORD)wnd + 0x204) != 0xC36E)
        return;

    if (!load_bk)
    {
        //if (rec_cache)
        //{
        //    delete rec_cache;
        //    rec_cache = 0;
        //}
        return;
    }

    if (!load_screen_font)
    {
        load_screen_font = true;
        font_bk = CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"));
    }

    int x = *(int*)((DWORD)wnd + 0x14);
    int y = *(int*)((DWORD)wnd + 0x18);

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
    r.right = 760 * (x / 800.0);
    r.top = 110 * (y / 600.0);
    r.bottom = 529 * (y / 600.0);

    if (!rec_cache)
        rec_cache = new REC_CACHE(wnd, r.right - r.left - 4, (r.right - r.left - 4) / 2);

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
    DeleteObject(brush_bk);

    char* file = window_getSelText(wnd_list, get_index(wnd_list));

    std::string filename = "savegame\\" + (std::string)file + rec_extension;
    current_file = filename;

    REC_DATA rd = rec_cache->get_rec_data(filename, 2);

    //draw playersplayers
    if (!loaded_player_brushes)
    {
        hb[0] = CreateSolidBrush(RGB(0, 0, 255));
        hb[1] = CreateSolidBrush(RGB(255, 0, 0));
        hb[2] = CreateSolidBrush(RGB(0, 255, 0));
        hb[3] = CreateSolidBrush(RGB(255, 255, 0));
        hb[4] = CreateSolidBrush(RGB(0, 255, 255));
        hb[5] = CreateSolidBrush(RGB(255, 0, 255));
        hb[6] = CreateSolidBrush(RGB(48, 48, 48));
        //hb[6] = CreateSolidBrush(RGB(127, 127, 127));
        //hb[7] = CreateSolidBrush(RGB(255, 127, 0));
        hb[7] = CreateSolidBrush(RGB(128, 0, 0));
        hb[8] = CreateSolidBrush(RGB(255, 255, 255));

        loaded_player_brushes = true;
    }

    char str[0x100];
    if (rd.exists)
        if (rd.valid)
        {
            rd.duration /= 1000;
            rd.start_time /= 1000;
            int total = rd.start_time + rd.duration;
            sprintf(str, "%d:%02d:%02d (%d:%02d:%02d - %d:%02d:%02d)",
                rd.duration / 3600, (rd.duration / 60) % 60, rd.duration % 60,
                rd.start_time / 3600, (rd.start_time / 60) % 60, rd.start_time % 60,
                total / 3600, (total / 60) % 60, total % 60);
            HDC hdcMem = CreateCompatibleDC(hdc);
            HGDIOBJ oldBitmap = SelectObject(hdcMem, rd.map);
            BITMAP bitmap;

            GetObject(rd.map, sizeof(bitmap), &bitmap);
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
            sprintf(str, "Invalid");
    else
        sprintf(str, "Loading...");

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
        r_ver.left = r_ver.right - 50;

        switch (rd.version)
        {
        case 0x00312E32:
            strcpy(str, "1.1");
            break;
        case 0x00322E32:
            strcpy(str, "2.2");
            break;
        case 0x00382E39:
            strcpy(str, "1.4");
            break;
        default:
            strcpy(str, "");
            break;
        }

        //sprintf(str, "VER %s", (char*)&rd.version);
        DrawText(hdc, str, strlen(str), &r_ver, DT_RIGHT);

        r.top += 30;

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

    SelectObject(hdc, hOld);

    s->ReleaseDC(hdc);

    //prepare closest 5
    if (prev_position != get_index(wnd_list))
        for (int i = -5; i <= 5; i++)
        {
            if (i != 0)
            {
                file = window_getSelText(wnd_list, get_index(wnd_list) + i);
                filename = "savegame\\" + (std::string)file + rec_extension;
                rec_cache->get_rec_data(filename, 1);
            }
        }
    prev_position = get_index(wnd_list);
    //prepare viewport
    if (prev_viewport != get_scroll_position(wnd_list))
        for (int i = get_scroll_position(wnd_list); i < (get_scroll_position(wnd_list) + 40); i++)
        {
            file = window_getSelText(wnd_list, i);
            filename = "savegame\\" + (std::string)file + rec_extension;
            rec_cache->get_rec_data(filename, 0);
        }
    prev_viewport = get_scroll_position(wnd_list);
}

__declspec(naked) void onCreateLoadBk() //004B7AC3
{
    __asm
    {
        push    edi
        push    eax
        cmp     dword ptr [ebp + 204h], 0C36Eh
        jnz     _no_create_bk
        mov     wnd_bk, ebp
        mov     eax, [ebp + 20h]
        mov     eax, [eax + 3Ch]
        mov     load_bk, eax
_no_create_bk:
        mov     edx, [ebp + 0]
        mov     ecx, ebp
        mov     eax, 004B7ACAh
        jmp     eax
    }
}

__declspec(naked) void hook_loadbk() //004B899F
{
    __asm
    {
        mov     load_bk, esi
        //push    ecx
        push    esi
        mov     ecx, [esi + 20h]
        mov     ecx, [ecx + 3Ch]
        push    ecx
        call    paintOnScreen_loadbk
        //pop     ecx
        mov     eax, [esi]
        mov     ecx, esi
        call    dword ptr [eax + 38h]
        mov     esi, 004B89A6h
        jmp     esi
    }
}

__declspec(naked) void onCreateList() //004BA84B
{
    __asm
    {
        mov     ecx, eax
        mov     wnd_list, eax
        mov     eax, 004CB890h
        call    eax
        mov     ecx, 004BA856h
        jmp     ecx
    }
}

void __stdcall invalidate_cache()
{
    prev_viewport = -1;
    prev_position = -1;
    if (rec_cache)
        rec_cache->invalidate();
}

__declspec(naked) void onDeleteList() //004CB928
{
    __asm
    {
        mov     eax, wnd_list
        cmp     eax, esi
        jnz     _not_list
        xor     eax, eax
        mov     wnd_list, eax
        call    invalidate_cache
_not_list:
        mov     eax, esi
        pop     esi
        retn    4
    }
}

__declspec(naked) void onDeleteBk() //0050A7C8
{
    __asm
    {
        mov     eax, wnd_bk
        cmp     eax, esi
        jnz     _not_loadbk
        xor     eax, eax
        mov     wnd_bk, eax
        call    invalidate_cache
_not_loadbk:
        mov     eax, esi
        pop     esi
        retn    4
    }
}

#pragma optimize( "s", on )
void setRecBrowseHooks(int version)
{
    void init_listbk();

    setHook((void*)0x004B7AC3, onCreateLoadBk);
    setHook((void*)0x004B899F, hook_loadbk);
    setHook((void*)0x004BA84B, onCreateList);
    setHook((void*)0x004CB928, onDeleteList);

    //file list
    writeByte(0x0050A5E2, 40); //x
    writeDword(0x0050A5DB, 435); //width, was 415

    switch (version)
    {
    case VER_CC:
        rec_extension = ".mg1";
        break;
    case VER_EF:
        rec_extension = ".mg2";
        break;
    default:
        rec_extension.clear();
        break;
    }
}
#pragma optimize( "", on )
