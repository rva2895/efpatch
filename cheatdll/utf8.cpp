#include "stdafx.h"
#include "utf8.h"

#include <wchar.h>

std::wstring UTF8ToWide(const char* us, int c)
{
    int count = MultiByteToWideChar(CP_UTF8, 0, us, c, NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_UTF8, 0, us, c, &wstr[0], count);
    wstr.resize(wcslen(wstr.c_str()));
    return wstr;
}

std::wstring UTF8ToWide(const std::string& us)
{
    return UTF8ToWide(us.c_str());
}

std::string WideToUTF8(const wchar_t* ws, int c)
{
    int count = WideCharToMultiByte(CP_UTF8, 0, ws, c, NULL, 0, NULL, NULL);
    std::string str(count, 0);
    WideCharToMultiByte(CP_UTF8, 0, ws, c, &str[0], count, NULL, NULL);
    str.resize(strlen(str.c_str()));
    return str;
}

const char* WideToUTF8(const std::wstring& ws, char* dest, int dest_size)
{
    memset(dest, 0, dest_size);
    WideCharToMultiByte(CP_UTF8, 0, &ws[0], ws.length(), dest, dest_size - 1, NULL, NULL);
    return dest;
}

std::string WideToUTF8(const std::wstring& ws)
{
    return WideToUTF8(ws.c_str());
}

int get_u_char_len(const char* s)
{
    for (int i = 1; i <= 4; i++)
    {
        int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s, i, NULL, 0);
        if (n > 0)
            return i;
    }
    return -1;
}

wchar_t get_wchar_from_us(const char* s)
{
    int len = get_u_char_len(s);
    wchar_t wc = 0;
    if (len > 0)
    {
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s, len, &wc, 1);
    }
    return wc;
}

const char* __cdecl usinc(const char* s)
{
    while (*s)
    {
        int len = get_u_char_len(s);
        if (len >= 0)
        {
            s += len;
            break;
        }
        else
        {
            s++;
        }
    }
    return s;
}

const char* __cdecl uschr(const char* s, unsigned int c)
{
    do
    {
        if (get_wchar_from_us(s) == c)
            return s;
        s = usinc(s);
    } while (*s);
    return NULL;
}

int __cdecl uscmp(const char* s1, const char* s2)
{
    return wcscmp(UTF8ToWide_c_str(s1), UTF8ToWide_c_str(s2));
}

const char* __cdecl usdec(const char* start, const char* pos)
{
    const char* prev = start;
    const char* cur;
    while (true)
    {
        cur = usinc(prev);
        if (cur == prev)
            return cur;
        if (cur >= pos)
            return prev;
        else
            prev = cur;
    }
}

const char* __cdecl usninc(const char* s, size_t c)
{
    const char* pos = s;
    for (int i = 0; i < c; i++)
        pos = usinc(pos);

    return pos;
}

char* __cdecl usncpy(char* dst, const char* src, size_t count)
{
    const char* pos = usninc(src, count);
    memcpy(dst, src, pos - src);
    return dst;
}

size_t __cdecl uslen(const char* s)
{
    size_t len = 0;
    do
    {
        s = usinc(s);
        len++;
    } while (*s);
    return len;
}

char* __cdecl usncat(char* dst, const char* src, size_t count)
{
    while (*dst)
    {
        dst = (char*)usinc(dst);
    }
    const char* src_p = src;
    while (*src_p && count > 0)
    {
        src_p = usinc(src_p);
        count--;
    }
    memcpy(dst, src, src_p - src);
    dst[src_p - src] = 0;
    return dst;
}

/*
int __cdecl isucspace(unsigned int c)
{
    //return 0;
    return isspace(c);
    //return _ismbcspace(c);
}
*/

int __stdcall is_next_char_space(const char* s)
{
    return iswspace(get_wchar_from_us(s));
}

__declspec(naked) int __cdecl ismbcspace_esi()
{
    __asm
    {
        push    esi
        call    is_next_char_space
        ret
    }
}

__declspec(naked) int __cdecl ismbcspace_edi()
{
    __asm
    {
        push    edi
        call    is_next_char_space
        ret
    }
}

const char* __cdecl usstr(const char* str, const char* substr)
{
    size_t n = strlen(substr);
    const char* pos = str;
    size_t n_src = strlen(pos) - n;
    while (*pos && pos - str <= n_src)
    {
        if (!memcmp(pos, substr, n))
            return pos;
        pos = usinc(pos);
    }
    return NULL;
}

int __cdecl usicmp(const char* s1, const char* s2)
{
    return _wcsicmp(UTF8ToWide_c_str(s1), UTF8ToWide_c_str(s2));
}

char* __cdecl usupr(char* str)
{
    std::wstring ws(UTF8ToWide(str, strlen(str)));
    _wcsupr(&ws[0]);
    ws.resize(wcslen(ws.c_str()));
    strcpy(str, WideToUTF8_c_str(ws));
    return str;
}

int __stdcall MyLoadString_utf8(HINSTANCE stringTable, int stringID, char* strBuffer, int strBufferLen)
{
    std::wstring ws(MAX_LOADSTRING, 0);
    LoadStringW(stringTable, stringID, &ws[0], MAX_LOADSTRING);
    ws.resize(wcslen(ws.c_str()));
    WideToUTF8(ws, strBuffer, strBufferLen);
    return strlen(strBuffer);
}

char* compare_string_save_str = NULL;
size_t compare_string_save_str_len = 0;
std::wstring compare_string_save_str_w;

int __stdcall CompareStringU(LCID Locale, DWORD dwCmpFlags, PCNZCH lpString1, int cchCount1, PCNZCH lpString2, int cchCount2)
{
    int len = cchCount1 < 0 ? strlen(lpString1) : cchCount1;
    if (len > compare_string_save_str_len)
    {
        free(compare_string_save_str);
        compare_string_save_str = (char*)malloc(compare_string_save_str_len = len);
        compare_string_save_str[0] = '\0';
    }
    if (strncmp(lpString1, compare_string_save_str, len))
    {
        strncpy(compare_string_save_str, lpString1, len);
        compare_string_save_str_w = UTF8ToWide(lpString1, cchCount1 < 0 ? -1 : cchCount1);
    }
    
    //std::wstring ws1(UTF8ToWide(lpString1, cchCount1 < 0 ? -1 : cchCount1));
    std::wstring ws2(UTF8ToWide(lpString2, cchCount2 < 0 ? -1 : cchCount2));
    return CompareStringW(Locale, dwCmpFlags, compare_string_save_str_w.c_str(), compare_string_save_str_w.length(), ws2.c_str(), ws2.length());
}

void Convert_WIN32_FIND_DATAW_To_WIN32_FIND_DATAA(LPWIN32_FIND_DATAA fda, LPWIN32_FIND_DATAW fdw)
{
    fda->dwFileAttributes = fdw->dwFileAttributes;
    fda->ftCreationTime = fdw->ftCreationTime;
    fda->ftLastAccessTime = fdw->ftLastAccessTime;
    fda->ftLastWriteTime = fdw->ftLastWriteTime;
    fda->nFileSizeHigh = fdw->nFileSizeHigh;
    fda->nFileSizeLow = fdw->nFileSizeLow;
    fda->dwReserved0 = fdw->dwReserved0;
    fda->dwReserved1 = fdw->dwReserved1;
    WideToUTF8(fdw->cFileName, fda->cFileName, _countof(fda->cFileName));
    WideToUTF8(fdw->cAlternateFileName, fda->cAlternateFileName, _countof(fda->cAlternateFileName));
}

HANDLE __stdcall FindFirstFileU(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData)
{
    WIN32_FIND_DATAW fdw;
    HANDLE r = FindFirstFileW(UTF8ToWide_c_str(lpFileName), &fdw);
    Convert_WIN32_FIND_DATAW_To_WIN32_FIND_DATAA(lpFindFileData, &fdw);
    return r;
}

BOOL __stdcall FindNextFileU(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData)
{
    WIN32_FIND_DATAW fdw;
    BOOL r = FindNextFileW(hFindFile, &fdw);
    Convert_WIN32_FIND_DATAW_To_WIN32_FIND_DATAA(lpFindFileData, &fdw);
    return r;
}

UINT __stdcall GetTempFileNameU(LPCSTR lpPathName, LPCSTR lpPrefixString, UINT uUnique, LPSTR lpTempFileName)
{
    std::wstring ws(MAX_PATH, 0);
    UINT r = GetTempFileNameW(
        UTF8ToWide_c_str(lpPathName),
        UTF8ToWide_c_str(lpPrefixString),
        uUnique,
        &ws[0]
    );
    ws.resize(wcslen(ws.c_str()));
    WideToUTF8(ws, lpTempFileName, MAX_PATH);
    return r;
}

DWORD __stdcall GetTempPathU(DWORD nBufferLength, LPSTR lpBuffer)
{
    std::wstring ws(nBufferLength, 0);
    DWORD r = GetTempPathW(nBufferLength, &ws[0]);
    ws.resize(wcslen(ws.c_str()));
    WideToUTF8(ws, lpBuffer, nBufferLength);
    return r;
}

DWORD __stdcall GetFileAttributesU(LPCSTR lpFileName)
{
    return GetFileAttributesW(UTF8ToWide_c_str(lpFileName));
}

BOOL __stdcall CreateDirectoryU(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
    return CreateDirectoryW(UTF8ToWide_c_str(lpPathName), lpSecurityAttributes);
}

BOOL __stdcall DeleteFileU(LPCSTR lpFileName)
{
    return DeleteFileW(UTF8ToWide_c_str(lpFileName));
}

DWORD __stdcall GetCurrentDirectoryU(DWORD nBufferLength, LPSTR lpBuffer)
{
    std::wstring ws(nBufferLength, 0);
    DWORD r = GetCurrentDirectoryW(nBufferLength, &ws[0]);
    ws.resize(wcslen(ws.c_str()));
    WideToUTF8(ws, lpBuffer, nBufferLength);
    return r;
}

HANDLE __stdcall CreateFileMappingU(HANDLE hFile, LPSECURITY_ATTRIBUTES lpFileMappingAttributes, DWORD flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCSTR lpName)
{
    return CreateFileMappingW(
        hFile,
        lpFileMappingAttributes,
        flProtect,
        dwMaximumSizeHigh,
        dwMaximumSizeLow,
        UTF8ToWide_c_str_NULL(lpName)
    );
}

HANDLE __stdcall CreateFileU(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    return CreateFileW(
        UTF8ToWide_c_str(lpFileName),
        dwDesiredAccess,
        dwShareMode,
        lpSecurityAttributes,
        dwCreationDisposition,
        dwFlagsAndAttributes,
        hTemplateFile
    );
}

int __stdcall GetWindowTextU(HWND hWnd, LPSTR lpString, int nMaxCount)
{
    std::wstring ws(nMaxCount, 0);
    int r = GetWindowTextW(hWnd, &ws[0], nMaxCount);
    ws.resize(wcslen(ws.c_str()));
    WideToUTF8(ws, lpString, nMaxCount);
    return r;
}

BOOL __stdcall SetWindowTextU(HWND hWnd, LPCSTR lpString)
{
    return SetWindowTextW(hWnd, UTF8ToWide_c_str_NULL(lpString));
}

ATOM __stdcall RegisterClassU(const WNDCLASSA* lpWndClass)
{
    WNDCLASSW wcw;
    std::wstring w_menuName(UTF8ToWide(lpWndClass->lpszMenuName));
    std::wstring w_className(UTF8ToWide(lpWndClass->lpszClassName));
    wcw.style = lpWndClass->style;
    wcw.lpfnWndProc = lpWndClass->lpfnWndProc;
    wcw.cbClsExtra = lpWndClass->cbClsExtra;
    wcw.cbWndExtra = lpWndClass->cbWndExtra;
    wcw.hInstance = lpWndClass->hInstance;
    wcw.hIcon = lpWndClass->hIcon;
    wcw.hCursor = lpWndClass->hCursor;
    wcw.hbrBackground = lpWndClass->hbrBackground;
    wcw.lpszMenuName = w_menuName.c_str();
    wcw.lpszClassName = w_className.c_str();
    return RegisterClassW(&wcw);
}

HWND __stdcall CreateWindowExU(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    return CreateWindowExW(
        dwExStyle,
        UTF8ToWide_c_str_NULL(lpClassName),
        UTF8ToWide_c_str_NULL(lpWindowName),
        dwStyle,
        X,
        Y,
        nWidth,
        nHeight,
        hWndParent,
        hMenu,
        hInstance,
        lpParam
    );
}

int __stdcall DrawTextU(HDC hdc, LPCSTR lpchText, int cchText, LPRECT lprc, UINT format)
{
    std::wstring ws(UTF8ToWide(lpchText, cchText));
    return DrawTextW(hdc, ws.c_str(), ws.length(), lprc, format);
}

BOOL __stdcall TextOutU(HDC hdc, int x, int y, LPCSTR lpString, int c)
{
    std::wstring ws(UTF8ToWide(lpString, c));
    return TextOutW(hdc, x, y, ws.c_str(), ws.length());
}

BOOL __stdcall GetTextExtentPoint32U(HDC hdc, LPCSTR lpString, int c, LPSIZE psizl)
{
    std::wstring ws(UTF8ToWide(lpString, c));
    return GetTextExtentPoint32W(hdc, ws.c_str(), ws.length(), psizl);
}

void __stdcall verify_edit_panel_text_len(TEditPanel* edit_panel)
{
    int len = edit_panel->fixed_len + 1;
    std::wstring ws(len, 0);
    std::string s;
    int i = 0;
    do
    {
        GetWindowTextW(edit_panel->edit_wnd, &ws[0], len);
        ws.resize(wcslen(ws.c_str()));
        s = WideToUTF8(ws);
        i++;
        len--;
    } while (s.length() > edit_panel->fixed_len - 1);

    strlcpy(edit_panel->text, s.c_str(), edit_panel->fixed_len);
    edit_panel->text_len = s.length();

    if (i > 1)
    {
        SetWindowTextW(edit_panel->edit_wnd, ws.c_str());
        MessageBeep(0xFFFFFFFF);
    }

    WPARAM wParam = 0;
    LPARAM lParam = 0;
    SendMessageW(edit_panel->edit_wnd, EM_GETSEL, (WPARAM)&wParam, (LPARAM)&lParam);
    edit_panel->sel_len = lParam - wParam;
}

bool __stdcall try_add_char(TEditPanel* edit_panel, int key)
{
    int len = edit_panel->fixed_len + 1;
    std::wstring ws(len, 0);
    GetWindowTextW(edit_panel->edit_wnd, &ws[0], len);
    ws.resize(wcslen(ws.c_str()));
    std::string s = WideToUTF8(ws += key);
    return s.length() < edit_panel->fixed_len;
}

int __stdcall TEditPanel__verify_char_new(TEditPanel* edit_panel, int key)
{
    switch (key)
    {
    case '\t':
        return 0;
    case '\r':
        if (edit_panel->format != 7)
            return 0;
        break;
    case '\b':
        return 1;
    }

    if (edit_panel->edit_wnd)
    {
        verify_edit_panel_text_len(edit_panel);
    }

    if (edit_panel->sel_len || try_add_char(edit_panel, key))
    {
        switch (edit_panel->format)
        {
        case 1: //decimal
        case 2: //signed integer
        case 3: //unsigned integer
            if (key >= '0' && key <= '9')
                return 1;
            if (key == '-')
            {
                if (*edit_panel->text && edit_panel->sel_len != edit_panel->text_len)
                    break;
                if (edit_panel->format != 3)
                    return 1;
            }
            else
            {
                if (key != '.' || uschr(edit_panel->text, '.'))
                    break;
                if (edit_panel->format == 1)
                    return 1;
            }
            break;
        case 4: //filename (w/ ext.)
        case 5: //filename (w/out ext.)
            switch (key)
            {
            case '.':
                if (edit_panel->format != 4)
                    break;
                if (uschr(edit_panel->text, '.'))
                    break;
                return 1;
            case '\\':
            case ':':
            case '/':
            case '"':
            case '*':
            case '?':
            case '>':
            case '<':
            case '|':
                break;
            default:
                return 1;
            }
            break;
        case 6:
            switch (key)
            {
            case ':':
                if (edit_panel->text_len != 1)
                    break;
                return 1;
            case '/':
            case '"':
            case '*':
            case '?':
            case '>':
            case '<':
            case '|':
                break;
            default:
                return 1;
            }
            break;
        case 8:
            if (edit_panel->text_len >= 2)
            {
                if (edit_panel->text_len != 2 || key != '0')
                    break;
                if (uscmp("10", edit_panel->text))
                    break;
                return 1;
            }
            if (key >= '0' && key <= '9')
                return 1;
            if (edit_panel->text_len != 2 || key != '0')
                break;
            if (uscmp("10", edit_panel->text))
                break;
            return 1;
        default:
            return 1;
        }
    }
    MessageBeep(0xFFFFFFFF);
    return 0;
}

__declspec(naked) void on_edit_panel_set_focus() //004C8DFF
{
    __asm
    {
        push    esi
        call    verify_edit_panel_text_len
        mov     eax, 004C8E97h
        jmp     eax
    }
}

int* const MouseCursorInChildContol = (int* const)0x006A35E4;
int* const tab_key = (int* const)0x0077D4BC;

int __fastcall TEditPanel__sub_wnd_proc_new(TEditPanel* edit_panel, DWORD dummy, HWND hwnd, unsigned int msg, unsigned int wParam, int lParam)
{
    UNREFERENCED_PARAMETER(dummy);

    if (edit_panel->edit_wnd && edit_panel->active && edit_panel->visible && !edit_panel->hidden)
    {
        switch (msg)
        {
        case WM_CHAR:
            if (!TEditPanel__verify_char_new(edit_panel, wParam))
                return 0;
            break;
        case WM_SYSKEYDOWN:
            if (!edit_panel->imc || !edit_panel->enable_ime)
            {
                SetFocus(edit_panel->render_area->Wnd);
                PostMessageW(edit_panel->render_area->Wnd, WM_SYSKEYDOWN, wParam, lParam);
            }
            return 0;
        case WM_MOUSEMOVE:
            if (!(*base_game)->windows_mouse)
            {
                *MouseCursorInChildContol = 1;
                RGE_Base_Game__mouse_off((RGE_Base_Game*)base_game);
            }
            if (edit_panel->parent_panel)
            {
                POINT point;
                int ctrl_key;
                int shift_key;
                TPanel__get_mouse_info((TPanel*)edit_panel, wParam, lParam, &point, &ctrl_key, &shift_key);
                edit_panel->parent_panel->vfptr->handle_mouse_move(
                    edit_panel->parent_panel,
                    point.x + edit_panel->pnl_x,
                    point.y + edit_panel->pnl_y,
                    ctrl_key,
                    shift_key);
            }
            break;
        case WM_KEYUP:
            if (wParam == VK_TAB)
                return 0;
            break;
        case WM_PAINT:
            InvalidateRect(hwnd, NULL, 1);
            break;
        case WM_ERASEBKGND:
            if (edit_panel->brush)
            {
                RECT Rect;
                SelectObject((HDC)wParam, edit_panel->brush);
                GetClientRect(hwnd, &Rect);
                FillRect((HDC)wParam, &Rect, edit_panel->brush);
                return 1;
            }
            break;
        case WM_KEYDOWN:
            if (edit_panel->parent_panel)
            {
                if (edit_panel->parent_panel->vfptr->wnd_proc(edit_panel->parent_panel, hwnd, msg, wParam, lParam))
                    return 0;
                switch (wParam)
                {
                case VK_TAB:
                    *tab_key = 1;
                    edit_panel->vfptr->handle_key_down(
                        edit_panel,
                        VK_TAB,
                        lParam,
                        GetKeyState(VK_MENU) & 0x8000,
                        GetKeyState(VK_CONTROL) & 0x8000,
                        GetKeyState(VK_SHIFT) & 0x8000
                    );
                    *tab_key = 0;
                    return 0;
                case VK_RETURN:
                    edit_panel->parent_panel->vfptr->action(edit_panel->parent_panel, (TPanel*)edit_panel, 0, 0, 0);
                    return 0;
                case VK_ESCAPE:
                    if (!edit_panel->parent_panel->vfptr->action(edit_panel->parent_panel, (TPanel*)edit_panel, 1, 0, 0))
                        SendMessageW(edit_panel->edit_wnd, EM_UNDO, 0, 0);
                    return 0;
                }
            }
            else if (wParam == VK_ESCAPE)
            {
                SendMessageW(edit_panel->edit_wnd, EM_UNDO, 0, 0);
                return 0;
            }
            break;
        }
    }
    DWORD start_before = 0;
    DWORD end_before = 0;
    DWORD start_after = 0;
    DWORD end_after = 0;
    bool check_sel_redraw;
    if (!edit_panel->edit_wnd || msg == EM_GETSEL)
    {
        check_sel_redraw = false;
    }
    else
    {
        check_sel_redraw = true;
        SendMessageW(edit_panel->edit_wnd, EM_GETSEL, (WPARAM)&start_before, (LPARAM)&end_before);
    }
    LRESULT r = CallWindowProcW((WNDPROC)edit_panel->old_sub_wnd_proc, hwnd, msg, wParam, lParam);
    if (check_sel_redraw)
    {
        SendMessageW(edit_panel->edit_wnd, EM_GETSEL, (WPARAM)&start_after, (LPARAM)&end_after);
        if (start_before != start_after || end_before != end_after)
        {
            InvalidateRect(edit_panel->edit_wnd, NULL, 1);
            UpdateWindow(edit_panel->edit_wnd);
        }
    }
    if (msg == WM_PASTE)
    {
        if (edit_panel->edit_wnd)
        {
            verify_edit_panel_text_len(edit_panel);
        }
        if (!(*base_game)->windows_mouse && !MouseCursorInChildContol)
        {
            SetCursor(NULL);
            return r;
        }
    }
    else if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP || msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
    {
        TPanel* top_wnd = TPanelSystem__getTop(panel_system);
        r = top_wnd->vfptr->wnd_proc(
            top_wnd,
            edit_panel->render_area->Wnd,
            msg,
            wParam,
            MAKELPARAM(LOWORD(lParam) + edit_panel->pnl_x, HIWORD(lParam) + edit_panel->pnl_y)
        );
    }
    return r;
}

LONG __stdcall SetWindowLongU(HWND hWnd, int nIndex, LONG dwNewLong)
{
    return SetWindowLongW(hWnd, nIndex, dwNewLong);
}

__declspec(naked) void on_edi_set_window_long() //004C897A
{
    __asm
    {
        mov     edi, SetWindowLongW
        mov     ecx, 004C8980h
        jmp     ecx
    }
}

int __stdcall DrawTextExU(HDC hdc, LPSTR lpchText, int cchText, LPRECT lprc, UINT format, LPDRAWTEXTPARAMS lpdtp)
{
    std::wstring ws(UTF8ToWide(lpchText, cchText));
    ws.reserve(ws.length() + 4);
    return DrawTextExW(hdc, ws.data(), ws.length(), lprc, format, lpdtp);
}

std::string comm_service_sessions_name_buffer;

const char* __fastcall TCommunications_Services__GetServiceName_new(TCommunications_Services* comm_services, DWORD dummy, int id)
{
    UNREFERENCED_PARAMETER(dummy);

    if (id < 0 || id > comm_services->ServiceCount)
    {
        return NULL;
    }
    else
    {
        wchar_t b[80];
        MultiByteToWideChar(CP_ACP, 0, comm_services->Service[id].Name, _countof(comm_services->Service[id].Name), b, _countof(b));
        //b[_countof(b) - 1] = 0;
        comm_service_sessions_name_buffer = WideToUTF8(b, _countof(b));
        return comm_service_sessions_name_buffer.c_str();
    }
}

void __fastcall TRIBE_Game__setRandomMapScript_new(TRIBE_Game* game, DWORD dummy, char* filename)
{
    UNREFERENCED_PARAMETER(dummy);
    strlcpy(game->tribe_game_options.random_map_script, filename, _countof(game->tribe_game_options.random_map_script));
}

void __fastcall TRIBE_Game__show_status_message2_new(
    TRIBE_Game* this_,
    DWORD dummy,
    int string_id,
    char* info_file,
    int info_id,
    int show_settings,
    int use_logo_background)
{
    UNREFERENCED_PARAMETER(dummy);
    char messageIn[0x400];

    this_->vfptr->get_string3(this_, string_id, messageIn, _countof(messageIn));
    TRIBE_Game__show_status_message(this_, messageIn, info_file, info_id, show_settings, use_logo_background);
}

int __fastcall TEasy_Panel__create_text4_new(
    TEasy_Panel* this_,
    DWORD dummy,
    TPanel* parent,
    TTextPanel** text_panel,
    int text_id,
    int x,
    int y,
    int width,
    int height,
    int font_num,
    int horz_center,
    int vert_center,
    int word_wrap)
{
    UNREFERENCED_PARAMETER(dummy);
    char str[0x800];

    TPanel__get_string((TPanel*)this_, text_id, str, _countof(str));
    return this_->vfptr->create_text_1(
        this_,
        parent,
        text_panel,
        str,
        x,
        y,
        width,
        height,
        font_num,
        horz_center,
        vert_center,
        word_wrap);
}

#pragma optimize( "s", on )
void setUTF8Hooks()
{
    setHook((void*)0x006356FC, uschr);
    setHook((void*)0x0063576F, uscmp);
    setHook((void*)0x006357F6, usinc);
    setHook((void*)0x0063580D, usdec);
    setHook((void*)0x00635855, usncpy);
    setHook((void*)0x006358D5, uslen);
    setHook((void*)0x00633C57, usncat);
    //setHook((void*)0x00633CFF, isucspace);
    setHook((void*)0x00633D8E, usninc);
    setHook((void*)0x00633DAC, usstr);
    setHook((void*)0x00635B08, usicmp);
    setHook((void*)0x0063658F, usupr);

    setHookCall((void*)0x004262A4, ismbcspace_edi);
    setHookCall((void*)0x004262D1, ismbcspace_edi);
    setHookCall((void*)0x00426333, ismbcspace_edi);
    setHookCall((void*)0x00426360, ismbcspace_edi);
    setHookCall((void*)0x00426C11, ismbcspace_esi);
    setHookCall((void*)0x00426C3D, ismbcspace_esi);
    setHookCall((void*)0x00426C8B, ismbcspace_esi);
    setHookCall((void*)0x00426CB7, ismbcspace_esi);
    setHookCall((void*)0x004CA2D0, ismbcspace_edi);
    setHookCall((void*)0x004CA2E9, ismbcspace_edi);
    setHookCall((void*)0x004CA330, ismbcspace_esi);
    setHookCall((void*)0x004D10D0, ismbcspace_esi);
    setHookCall((void*)0x004D1107, ismbcspace_esi);

    setHook((void*)0x00487760, MyLoadString_utf8);
    setHook((void*)0x004C9360, TEditPanel__sub_wnd_proc_new);
    setHook((void*)0x004C8DFF, on_edit_panel_set_focus);
    setHook((void*)0x004C897A, on_edi_set_window_long);

    setHook((void*)0x005EEE10, TRIBE_Game__setRandomMapScript_new);

    //chat max_len
    //writeByte(0x0046E674, 120);
    //writeByte(0x00461D5E, 120);
    //writeByte(0x00460823, 120);

    setHook((void*)0x00446D70, TCommunications_Services__GetServiceName_new);

    setHook((void*)0x005E7F80, TRIBE_Game__show_status_message2_new);
    setHook((void*)0x004B9A80, TEasy_Panel__create_text4_new);

    setHook(GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "CompareStringA"), CompareStringU);
    setHook(GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "FindFirstFileA"), FindFirstFileU);
    setHook(GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "FindNextFileA"), FindNextFileU);
    setHook(GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "GetTempFileNameA"), GetTempFileNameU);
    setHook(GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "GetTempPathA"), GetTempPathU);
    setHook(GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "GetFileAttributesA"), GetFileAttributesU);
    setHook(GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "CreateDirectoryA"), CreateDirectoryU);
    setHook(GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "DeleteFileA"), DeleteFileU);
    setHook(GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "GetCurrentDirectoryA"), GetCurrentDirectoryU);
    setHook(GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "CreateFileMappingA"), CreateFileMappingU);
    setHook(GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "CreateFileA"), CreateFileU);
    setHook(GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetWindowTextA"), GetWindowTextU);
    setHook(GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowTextA"), SetWindowTextU);
    //setHook(GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowLongA"), SetWindowLongU);

    //setHook(GetProcAddress(GetModuleHandleW(L"user32.dll"), "RegisterClassA"), RegisterClassU);
    setHook(GetProcAddress(GetModuleHandleW(L"user32.dll"), "CreateWindowExA"), CreateWindowExU);
    setHook(GetProcAddress(GetModuleHandleW(L"user32.dll"), "DrawTextA"), DrawTextU);
    setHook(GetProcAddress(GetModuleHandleW(L"user32.dll"), "DrawTextExA"), DrawTextExU);
    setHook(GetProcAddress(GetModuleHandleW(L"gdi32.dll"), "TextOutA"), TextOutU);
    setHook(GetProcAddress(GetModuleHandleW(L"gdi32.dll"), "GetTextExtentPoint32A"), GetTextExtentPoint32U);

    writeByte(0x00536165, 2); //trigger quantity format
    writeByte(0x00533B37, 2); //player inven format
}
#pragma optimize( "", on )
