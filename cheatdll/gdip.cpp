#include "stdafx.h"
#include "gdip.h"

//#include <map>

#include <GdiPlus.h>
using namespace Gdiplus;
#pragma comment (lib, "gdiplus.lib")


ULONG_PTR   gdiplusToken;

/*
class DC_DATA
{
public:
    Graphics* graphics;
    //Font* font;
    explicit DC_DATA(Graphics* g)
    {
        graphics = g;
        //font = f;
    }
    explicit DC_DATA()
    {
        graphics = NULL;
        //font = NULL;
    }
    ~DC_DATA()
    {
        delete font;
        delete graphics;
    }
};

std::map<HDC, DC_DATA> dc_data;

void __stdcall add_dc(HDC hdc)
{
    DC_DATA dcd(new Graphics(hdc));
    auto it = dc_data.find(hdc);
    if (it != dc_data.end())
    {
        //delete it->second.font;
        delete it->second.graphics;
    }
    dc_data[hdc] = dcd;
}


Graphics* get_graphics2(HDC hdc)
{
    auto it = dc_data.find(hdc);
    if (it == dc_data.end())
    {
        add_dc(hdc);
        it = dc_data.find(hdc);
    }
    return it->second.graphics;
}

int new_g = 0;
int reused_g = 0;
*/

HDC prev_dc = NULL;
Graphics* graphics = NULL;

Font* default_font = NULL;

Graphics* get_graphics(HDC hdc)
{
    if (hdc != prev_dc || !graphics)
    {
        delete graphics;
        graphics = new Graphics(hdc);
        graphics->SetTextRenderingHint(TextRenderingHintSingleBitPerPixelGridFit);
        prev_dc = hdc;
    }
    return graphics;
}

/*
Font* get_font(HDC hdc)
{
    if (hdc != prev_dc || !graphics)
    {
        delete graphics;
        graphics = new Graphics(hdc);
        prev_dc = hdc;
    }
    return font;
    auto it = dc_data.find(hdc);
    return it->second.font;
}*/

/*
void __stdcall delete_dc(HDC hdc)
{
    auto it = dc_data.find(hdc);
    if (it != dc_data.end())
    {
        //delete it->second.font;
        delete it->second.graphics;
        dc_data.erase(it);
    }
}

__declspec(naked) void on_DrawArea_GetDC() //00472B9C
{
    __asm
    {
        call    dword ptr [eax + 44h]
        mov     eax, [esi]
        push    eax
        call    add_dc
        mov     eax, [esi]
        pop     esi
        ret     4
    }
}

__declspec(naked) void on_DrawArea_ReleaseDC() //00472BD4
{
    __asm
    {
        push    dword ptr [esi + 38h]
        call    delete_dc
        mov     dword ptr [esi + 38h], 0
        pop     esi
        ret     4
    }
}
*/

BOOL __stdcall GetTextExtentPoint32W_new(HDC hdc, LPCWSTR lpString, int c, LPSIZE psizl)
{
    RectF       boundRect;
    PointF      pointF(0.0f, 0.0f);
    StringFormat sf = StringFormat::GenericTypographic();
    Font        font(hdc);
    Graphics* g = get_graphics(hdc);
    g->MeasureString(lpString, c, &font, pointF, &sf, &boundRect);

    psizl->cx = ceil(boundRect.Width);
    psizl->cy = ceil(boundRect.Height);
    return TRUE;
}

BOOL __stdcall GetTextExtentPoint32A_new(HDC hdc, LPCSTR lpString, int c, LPSIZE psizl)
{
    int count = MultiByteToWideChar(CP_ACP, 0, lpString, c, NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_ACP, 0, lpString, c, &wstr[0], count);

    return GetTextExtentPoint32W_new(hdc, wstr.c_str(), c, psizl);
}

COLORREF txt_color = 0;

//int cntr = 0;

/*COLORREF __stdcall SetTextColor_new(HDC hdc, COLORREF color)
{
    cntr++;
    if (cntr % 1000 == 0)
        MessageBox(NULL, "OOOOO", "WWWWW", 0);

    COLORREF prev = txt_color;
    txt_color = color;
    return prev;
}*/

void* text_color_jump_address;

__declspec(naked) void __stdcall set_text_color_jmp()
{
    __asm
    {
        mov     eax, [esp + 8]
        mov     txt_color, eax
        mov     eax, text_color_jump_address
        jmp     eax
    }
}

void make_set_text_color_hook()
{
    void* address = GetProcAddress(GetModuleHandle("gdi32.dll"), "SetTextColor");
    writeWord((DWORD)address, 0xF9EB);
    writeByte((DWORD)address - 5, 0xE9);
    writeDword((DWORD)address - 4, (DWORD)set_text_color_jmp - (DWORD)address);
    text_color_jump_address = (void*)((DWORD)address + 2);
}

UINT text_align = 0;

BOOL __stdcall TextOutW_new(HDC hdc, int x, int y, LPCWSTR lpString, int c)
{
    StringFormat sf = StringFormat::GenericTypographic();
    if (text_align & TA_RIGHT)
        sf.SetAlignment(StringAlignmentFar);

    //COLORREF txt_color = RGB(255, 0, 255);
    SolidBrush  solidBrush(Color(255, GetRValue(txt_color), GetGValue(txt_color), GetBValue(txt_color)));
    PointF      pointF(x, y);
    Graphics*   g = get_graphics(hdc);
    Font        font(hdc);
    if (font.GetLastStatus() != Ok)
    {
        if (!default_font)
            default_font = new Font(L"System", 10, FontStyle::FontStyleBold);
        g->DrawString(lpString, c, default_font, pointF, &sf, &solidBrush);
    }
    else
        g->DrawString(lpString, c, &font, pointF, &sf, &solidBrush);

    return TRUE;
}

BOOL __stdcall TextOutA_new(HDC hdc, int x, int y, LPCSTR lpString, int c)
{
    int count = MultiByteToWideChar(CP_ACP, 0, lpString, c, NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_ACP, 0, lpString, c, &wstr[0], count);

    return TextOutW_new(hdc, x, y, wstr.c_str(), c);
}

int __stdcall DrawTextW_new(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format)
{
    LONG offset_x = 0;
    LONG offset_y = 0;

    Graphics*   g = get_graphics(hdc);
    StringFormat sf = StringFormat::GenericTypographic();
    Font        font(hdc);

    if (format & DT_RIGHT || format & DT_CENTER || format & DT_VCENTER || format & DT_BOTTOM)
    {
        RectF boundRect;
        RectF layoutRect(lprc->left, lprc->top, lprc->right - lprc->left, lprc->bottom - lprc->top);
        g->MeasureString(lpchText, cchText, &font, layoutRect, &sf, &boundRect);

        if (format & DT_RIGHT)
            offset_x = lprc->right - lprc->left - boundRect.Width;
        if (format & DT_CENTER)
            offset_x = (lprc->right - lprc->left - boundRect.Width) / 2;
        if (format & DT_VCENTER)
            offset_y = (lprc->bottom - lprc->top - boundRect.Height) / 2;
        if (format & DT_BOTTOM)
            offset_y = lprc->bottom - lprc->top - boundRect.Height;
    }

    RectF       rectF(lprc->left + offset_x, lprc->top + offset_y,
        lprc->right - (lprc->left + offset_x), lprc->bottom - (lprc->top + offset_y));

    //COLORREF txt_color = RGB(0, 255, 255);
    SolidBrush  solidBrush(Color(255, GetRValue(txt_color), GetGValue(txt_color), GetBValue(txt_color)));
    g->DrawString(lpchText, cchText, &font, rectF, &sf, &solidBrush);

    return 0;
}

int __stdcall DrawTextA_new(HDC hdc, LPCSTR lpchText, int cchText, LPRECT lprc, UINT format)
{
    int count = MultiByteToWideChar(CP_ACP, 0, lpchText, cchText, NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_ACP, 0, lpchText, cchText, &wstr[0], count);

    return DrawTextW_new(hdc, wstr.c_str(), cchText, lprc, format);
}

//NOT IMPLEMENTED
int __stdcall DrawTextExW_new(HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, UINT format, LPDRAWTEXTPARAMS lpdtp)
{
    return DrawTextW_new(hdc, lpchText, cchText, lprc, format);
}

//NOT IMPLEMENTED
int __stdcall DrawTextExA_new(HDC hdc, LPSTR lpchText, int cchText, LPRECT lprc, UINT format, LPDRAWTEXTPARAMS lpdtp)
{
    int count = MultiByteToWideChar(CP_ACP, 0, lpchText, cchText, NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_ACP, 0, lpchText, cchText, &wstr[0], count);

    return 0;
    //return DrawTextExW_new(hdc, wstr.c_str(), cchText, lprc, format, lpdtp);
}

UINT __stdcall SetTextAlign_new(HDC hdc, UINT align)
{
    UINT prev = text_align;
    text_align = align;
    return prev;
}

BOOL __stdcall ExtTextOutW_new(HDC hdc, int x, int y, UINT options, const RECT* lprect, LPCWSTR lpString, UINT c, const INT* lpDx)
{
    return TextOutW_new(hdc, x, y, lpString, c);
}

BOOL __stdcall ExtTextOutA_new(HDC hdc, int x, int y, UINT options, const RECT* lprect, LPCSTR lpString, UINT c, const INT* lpDx)
{
    int count = MultiByteToWideChar(CP_ACP, 0, lpString, c, NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_ACP, 0, lpString, c, &wstr[0], count);

    return ExtTextOutW_new(hdc, x, y, options, lprect, wstr.c_str(), c, lpDx);
}

DWORD __stdcall GetCharacterPlacementW_new(HDC hdc, LPCWSTR lpString, int nCount, int nMexExtent, LPGCP_RESULTSA lpResults, DWORD dwFlags)
{
    return 0;
}

DWORD __stdcall GetGlyphIndicesW_new(HDC hdc, LPCWSTR lpstr, int c, LPWORD pgi, DWORD fl)
{
    return 0;
}

#pragma optimize( "s", on )
void setGDIPlusHooks()
{
    //setHook((void*)0x00472B9C, on_DrawArea_GetDC);
    //setHook((void*)0x00472BD4, on_DrawArea_ReleaseDC);

    setHook(GetProcAddress(GetModuleHandle("gdi32.dll"), "SetTextAlign"), SetTextAlign_new);
    setHook(GetProcAddress(GetModuleHandle("gdi32.dll"), "TextOutA"), TextOutA_new);
    setHook(GetProcAddress(GetModuleHandle("gdi32.dll"), "TextOutW"), TextOutW_new);
    //setHook(GetProcAddress(GetModuleHandle("gdi32.dll"), "ExtTextOutA"), ExtTextOutA_new);
    //setHook(GetProcAddress(GetModuleHandle("gdi32.dll"), "ExtTextOutW"), ExtTextOutW_new);
    setHook(GetProcAddress(GetModuleHandle("gdi32.dll"), "GetTextExtentPoint32A"), GetTextExtentPoint32A_new);
    setHook(GetProcAddress(GetModuleHandle("gdi32.dll"), "GetTextExtentPoint32W"), GetTextExtentPoint32W_new);
    //setHook(GetProcAddress(GetModuleHandle("gdi32.dll"), "SetTextColor"), SetTextColor_new);

    make_set_text_color_hook();

    //setHook(GetProcAddress(GetModuleHandle("gdi32.dll"), "GetCharacterPlacementW"), GetCharacterPlacementW_new);
    //setHook(GetProcAddress(GetModuleHandle("gdi32.dll"), "GetGlyphIndicesA"), GetGlyphIndicesW_new);

    setHook(GetProcAddress(GetModuleHandle("user32.dll"), "DrawTextA"), DrawTextA_new);
    setHook(GetProcAddress(GetModuleHandle("user32.dll"), "DrawTextW"), DrawTextW_new);
    setHook(GetProcAddress(GetModuleHandle("user32.dll"), "DrawTextExA"), DrawTextExA_new);
    setHook(GetProcAddress(GetModuleHandle("user32.dll"), "DrawTextExW"), DrawTextExW_new);

    //GdiplusStartup
    GdiplusStartupInput gdiplusStartupInput;

    Status st = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}
#pragma optimize( "", on )
