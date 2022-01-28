#include "stdafx.h"
#include "gdip.h"

#include <map>

#include <GdiPlus.h>
using namespace Gdiplus;
#pragma comment (lib, "gdiplus.lib")


ULONG_PTR           gdiplusToken;
//FontFamily  fontFamily2(L"Times New Roman");
//Font        font(&fontFamily, 24, FontStyleRegular, UnitPixel);
//PointF      pointF(30.0f, 10.0f);
//SolidBrush  solidBrush(Color(255, 0, 0, 255));

//FontFamily* fontFamily;
//Font* font2;
//SolidBrush* solidBrush;

/*
void init_stuff()
{
    fontFamily = new FontFamily(L"Arial");
    //font2 = new Font(fontFamily, 14, FontStyleRegular, UnitPixel);
    //solidBrush = new SolidBrush(0xFFFFFFFF);
}


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

BOOL __stdcall GetTextExtentPoint32A_new(HDC hdc, LPCSTR lpString, int c, LPSIZE psizl)
{
    //psizl->cx = 0;
    //psizl->cy = 0;
    Graphics* g = get_graphics(hdc);
    wchar_t str[0x200];
    size_t conv;
    mbstowcs_s(&conv, str, _countof(str), lpString, c >= _countof(str) ? (c - 1) : c);

    RectF boundRect;
    Font font(hdc);
    //Font* font = get_font(hdc);
    PointF pointF(0.0f, 0.0f);
    StringFormat sf = StringFormat::GenericTypographic();
    graphics->MeasureString(str, c, &font, pointF, &sf, &boundRect);
    //graphics->MeasureString(str, cchText, &font, layoutRect, &boundRect);
    psizl->cx = ceil(boundRect.Width);
    psizl->cy = ceil(boundRect.Height);
    return TRUE;
}

COLORREF txt_color = 0;

COLORREF __stdcall SetTextColor_new(HDC hdc, COLORREF color)
{
    COLORREF prev = txt_color;
    txt_color = color;
    return prev;
}

UINT text_align = 0;

BOOL __stdcall TextOutA_new(HDC hdc, int x, int y, LPCSTR lpString, int c)
{
    //FontFamily  fontFamily2(L"Times New Roman");
    //Font        font2(&fontFamily2, 16, FontStyleRegular, UnitPixel);
    //PointF      pointF(30.0f, 10.0f);
    SolidBrush  solidBrush(Color(255, GetRValue(txt_color), GetGValue(txt_color), GetBValue(txt_color)));
    //RectF        rectF(x, y, x+200, y+50);
    PointF      pointF(x, y);
    Graphics* g = get_graphics(hdc);
    //wchar_t str[0x200];
    size_t conv;
    //mbstowcs_s(&conv, str, _countof(str), lpString, c >= _countof(str) ? (c - 1) : c);
    int buf_size = MultiByteToWideChar(CP_ACP, 0, lpString, c, NULL, 0);
    wchar_t* str;
    int result;
    if (buf_size > 0)
    {
        str = (wchar_t*)malloc(buf_size * sizeof(wchar_t));
        result = MultiByteToWideChar(CP_ACP, 0, lpString, c, str, buf_size);
        if (result <= 0)
            str[0] = '\0';
    }
    else
    {
        str = (wchar_t*)malloc(1 * sizeof(wchar_t));
        str[0] = '\0';
    }
    StringFormat sf = StringFormat::GenericTypographic();
    if (text_align & TA_RIGHT)
        sf.SetAlignment(StringAlignmentFar);

    //graphics.DrawString(str, -1, font, rectF, NULL, solidBrush);
    Font font(hdc);
    //Font* font = get_font(hdc);
    //if (strstr(lpString, "r(v"))
    g->DrawString(str, c, &font, pointF, &sf, &solidBrush);

    free(str);

    return TRUE;
}

int __stdcall DrawTextA_new(HDC hdc, LPCSTR lpchText, int cchText, LPRECT lprc, UINT format)
{
    SolidBrush  solidBrush(Color(255, GetRValue(txt_color), GetGValue(txt_color), GetBValue(txt_color)));
    wchar_t str[0x200];
    size_t conv;
    mbstowcs_s(&conv, str, _countof(str), lpchText, cchText >= _countof(str) ? (cchText - 1) : cchText);
    LONG offset_x = 0;
    LONG offset_y = 0;
    StringFormat sf = StringFormat::GenericTypographic();
    Font font(hdc);

    Graphics* g = get_graphics(hdc);

    if (format & DT_RIGHT || format & DT_CENTER || format & DT_VCENTER || format & DT_BOTTOM)
    {
        RectF boundRect;
        RectF layoutRect(lprc->left, lprc->top, lprc->right - lprc->left, lprc->bottom - lprc->top);
        g->MeasureString(str, cchText, &font, layoutRect, &sf, &boundRect);

        if (format & DT_RIGHT)
            offset_x = lprc->right - lprc->left - boundRect.Width;
        if (format & DT_CENTER)
            offset_x = (lprc->right - lprc->left - boundRect.Width) / 2;
        if (format & DT_VCENTER)
            offset_y = (lprc->bottom - lprc->top - boundRect.Height) / 2;
        if (format & DT_BOTTOM)
            offset_y = lprc->bottom - lprc->top - boundRect.Height;
    }

    RectF        rectF(lprc->left + offset_x, lprc->top + offset_y,
        lprc->right - (lprc->left + offset_x), lprc->bottom - (lprc->top + offset_y));


    //Font* font = get_font(hdc);
    g->DrawString(str, cchText, &font, rectF, &sf, &solidBrush);

    return 0;
}

UINT __stdcall SetTextAlign_new(HDC hdc, UINT align)
{
    UINT prev = text_align;
    text_align = align;
    return prev;
}


void setGDIPlusHooks()
{
    //setHook((void*)0x00472B9C, on_DrawArea_GetDC);
    //setHook((void*)0x00472BD4, on_DrawArea_ReleaseDC);

    setHook(GetProcAddress(GetModuleHandle("gdi32.dll"), "SetTextAlign"), SetTextAlign_new);
    setHook(GetProcAddress(GetModuleHandle("gdi32.dll"), "TextOutA"), TextOutA_new);
    setHook(GetProcAddress(GetModuleHandle("gdi32.dll"), "GetTextExtentPoint32A"), GetTextExtentPoint32A_new);
    setHook(GetProcAddress(GetModuleHandle("user32.dll"), "DrawTextA"), DrawTextA_new);

    setHook(GetProcAddress(GetModuleHandle("gdi32.dll"), "SetTextColor"), SetTextColor_new);

    //GdiplusStartup
    GdiplusStartupInput gdiplusStartupInput;

    Status st = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    //init_stuff();
}
