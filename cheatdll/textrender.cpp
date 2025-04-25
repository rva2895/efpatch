#include "stdafx.h"
#include "textrender.h"
#include "registry.h"

#include <dwrite.h>

float screen_scale_factor = 1.0f;

extern CONFIG_DATA cd;

HRESULT (__stdcall* DWriteCreateFactory_ptr)(
    __in DWRITE_FACTORY_TYPE factoryType,
    __in REFIID iid,
    __out IUnknown** factory
);

HRESULT DWriteDrawText(int x, int y, bool right_align, COLORREF color);
HRESULT DWritePrepareText(HDC hdc, const wchar_t* text, HFONT hfont, int x, int y, int cx, int cy, UINT format);

// DirectWrite global variables.
IDWriteFactory* g_pDWriteFactory = NULL;
IDWriteTextFormat* g_pTextFormat = NULL;
IDWriteGdiInterop* g_pGdiInterop = NULL;
IDWriteTextLayout* g_pTextLayout = NULL;
IDWriteBitmapRenderTarget* g_pBitmapRenderTarget = NULL;
IDWriteRenderingParams* g_pRenderingParams = NULL;

// For our custom renderer class.
IDWriteTextRenderer* g_pGdiTextRenderer = NULL;

GdiTextRenderer::GdiTextRenderer(
    IDWriteBitmapRenderTarget* bitmapRenderTarget,
    IDWriteRenderingParams* renderingParams
)
    :
    cRefCount_(1),
    pRenderTarget_(bitmapRenderTarget),
    pRenderingParams_(renderingParams),
    color_(RGB(0,0,0))
{
    pRenderTarget_->AddRef();
    pRenderingParams_->AddRef();
}

/******************************************************************
*                                                                 *
*  GdiTextRenderer::GdiTextRenderer                               *
*                                                                 *
*  Destructor releases the interfaces passed when the class was   *
*  created.                                                       *
*                                                                 *
******************************************************************/

GdiTextRenderer::~GdiTextRenderer()
{
    SafeRelease(&pRenderTarget_);
    SafeRelease(&pRenderingParams_);
}

void GdiTextRenderer::setColor(
    COLORREF color
)
{
    color_ = color;
}

/******************************************************************
*                                                                 *
*  GdiTextRenderer::DrawGlyphRun                                  *
*                                                                 *
*  Gets GlyphRun outlines via IDWriteFontFace::GetGlyphRunOutline *
*  and then draws and fills them using Direct2D path geometries   *
*                                                                 *
******************************************************************/
STDMETHODIMP GdiTextRenderer::DrawGlyphRun(
    __maybenull void* clientDrawingContext,
    FLOAT baselineOriginX,
    FLOAT baselineOriginY,
    DWRITE_MEASURING_MODE measuringMode,
    __in DWRITE_GLYPH_RUN const* glyphRun,
    __in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
    IUnknown* clientDrawingEffect
)
{
    HRESULT hr = S_OK;

    // Pass on the drawing call to the render target to do the real work.
    RECT dirtyRect = { 0 };

    hr = pRenderTarget_->DrawGlyphRun(
        baselineOriginX,
        baselineOriginY,
        measuringMode,
        glyphRun,
        pRenderingParams_,
        color_,
        &dirtyRect
    );

    return hr;
}

/******************************************************************
*                                                                 *
*  GdiTextRenderer::DrawUnderline                                 *
*                                                                 *
*  This function is not implemented for the purposes of this      *
*  sample.                                                        *
*                                                                 *
******************************************************************/

STDMETHODIMP GdiTextRenderer::DrawUnderline(
    __maybenull void* clientDrawingContext,
    FLOAT baselineOriginX,
    FLOAT baselineOriginY,
    __in DWRITE_UNDERLINE const* underline,
    IUnknown* clientDrawingEffect
)
{
    // Not implemented
    return E_NOTIMPL;
}

HDC draw_dc = NULL;

/******************************************************************
*                                                                 *
*  GdiTextRenderer::DrawStrikethrough                             *
*                                                                 *
*  This function is not implemented for the purposes of this      *
*  sample.                                                        *
*                                                                 *
******************************************************************/

STDMETHODIMP GdiTextRenderer::DrawStrikethrough(
    __maybenull void* clientDrawingContext,
    FLOAT baselineOriginX,
    FLOAT baselineOriginY,
    __in DWRITE_STRIKETHROUGH const* strikethrough,
    IUnknown* clientDrawingEffect
)
{
    RECT r;
    r.left = floor(baselineOriginX);
    r.top = floor(baselineOriginY + strikethrough->offset);
    r.right = ceil(baselineOriginX + strikethrough->width);
    r.bottom = ceil(baselineOriginY + strikethrough->offset + strikethrough->thickness);

    HBRUSH br = CreateSolidBrush(color_);
    FillRect(pRenderTarget_->GetMemoryDC(), &r, br);
    DeleteObject(br);

    return S_OK;
}

/******************************************************************
*                                                                 *
*  GdiTextRenderer::DrawInlineObject                              *
*                                                                 *
*  This function is not implemented for the purposes of this      *
*  sample.                                                        *
*                                                                 *
******************************************************************/

STDMETHODIMP GdiTextRenderer::DrawInlineObject(
    __maybenull void* clientDrawingContext,
    FLOAT originX,
    FLOAT originY,
    IDWriteInlineObject* inlineObject,
    BOOL isSideways,
    BOOL isRightToLeft,
    IUnknown* clientDrawingEffect
)
{
    // Not implemented
    return E_NOTIMPL;
}

/******************************************************************
*                                                                 *
*  GdiTextRenderer::AddRef                                        *
*                                                                 *
*  Increments the ref count                                       *
*                                                                 *
******************************************************************/

STDMETHODIMP_(unsigned long) GdiTextRenderer::AddRef()
{
    return InterlockedIncrement(&cRefCount_);
}

/******************************************************************
*                                                                 *
*  GdiTextRenderer::Release                                       *
*                                                                 *
*  Decrements the ref count and deletes the instance if the ref   *
*  count becomes 0                                                *
*                                                                 *
******************************************************************/

STDMETHODIMP_(unsigned long) GdiTextRenderer::Release()
{
    long newCount = InterlockedDecrement(&cRefCount_);

    if (newCount == 0)
    {
        delete this;
        return 0;
    }
    return newCount;
}

/******************************************************************
*                                                                 *
*  GdiTextRenderer::IsPixelSnappingDisabled                       *
*                                                                 *
*  Determines whether pixel snapping is disabled. The recommended *
*  default is FALSE, unless doing animation that requires         *
*  subpixel vertical placement.                                   *
*                                                                 *
******************************************************************/

STDMETHODIMP GdiTextRenderer::IsPixelSnappingDisabled(
    __maybenull void* clientDrawingContext,
    __out BOOL* isDisabled
)
{
    *isDisabled = FALSE;
    return S_OK;
}

/******************************************************************
*                                                                 *
*  GdiTextRenderer::GetCurrentTransform                           *
*                                                                 *
*  Returns the current transform applied to the render target..   *
*                                                                 *
******************************************************************/

STDMETHODIMP GdiTextRenderer::GetCurrentTransform(
    __maybenull void* clientDrawingContext,
    __out DWRITE_MATRIX* transform
)
{
    //forward the render target's transform
    pRenderTarget_->GetCurrentTransform(transform);
    return S_OK;
}

/******************************************************************
*                                                                 *
*  GdiTextRenderer::GetPixelsPerDip                               *
*                                                                 *
*  This returns the number of pixels per DIP.                     *
*                                                                 *
******************************************************************/

STDMETHODIMP GdiTextRenderer::GetPixelsPerDip(
    __maybenull void* clientDrawingContext,
    __out FLOAT* pixelsPerDip
)
{
    *pixelsPerDip = pRenderTarget_->GetPixelsPerDip();
    return S_OK;
}

/******************************************************************
*                                                                 *
*  GdiTextRenderer::QueryInterface                                *
*                                                                 *
*  Query interface implementation                                 *
*                                                                 *
******************************************************************/

STDMETHODIMP GdiTextRenderer::QueryInterface(
    IID const& riid,
    void** ppvObject
)
{
    if (__uuidof(IDWriteTextRenderer) == riid)
    {
        *ppvObject = this;
    }
    else if (__uuidof(IDWritePixelSnapping) == riid)
    {
        *ppvObject = this;
    }
    else if (__uuidof(IUnknown) == riid)
    {
        *ppvObject = this;
    }
    else
    {
        *ppvObject = NULL;
        return E_FAIL;
    }

    return S_OK;
}

void DWriteProcessText(bool do_paint, HDC hdc, int x, int y, int cx, int cy, const wchar_t* str, int c, COLORREF color, UINT format, LPSIZE psizl)
{
    std::wstring text(str);
    if (c > 0)
        text.resize(c);

    HRESULT hr = S_OK;

    HFONT hfont = NULL;
    HDC memoryHdc = NULL;

    hfont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);

    bool right_align = format & DT_RIGHT;

    // The DirectWrite objects need only be created once and can be reused each time the window paints
    hr = DWritePrepareText(hdc, text.c_str(), hfont, x, y, cx, cy, format);

    DWRITE_TEXT_METRICS tm;
    g_pTextLayout->GetMetrics(&tm);
    tm.width *= screen_scale_factor;
    tm.height *= screen_scale_factor;

    if (psizl)
    {
        psizl->cx = ceil(tm.width);
        psizl->cy = ceil(tm.height);
    }

    if (!do_paint)
        return;

    // Get the memory device context, the drawing is done offscreen.
    if (SUCCEEDED(hr))
    {
        memoryHdc = g_pBitmapRenderTarget->GetMemoryDC();
        SetBoundsRect(memoryHdc, NULL, DCB_ENABLE | DCB_RESET);
    }

    int blt_x;
    int blt_y;
    int blt_cx;
    int blt_cy;

    bool right_align_fix = false;

    if (cx == -1 || cy == -1)
    {
        blt_x = right_align ? x - ceil(tm.width) : x;
        blt_y = y;
        blt_cx = ceil(tm.width);
        blt_cy = ceil(tm.height);
        right_align_fix = right_align;
    }
    else
    {
        blt_x = x;
        blt_y = y;
        blt_cx = cx;
        blt_cy = cy;
    }

    BitBlt(
        memoryHdc,
        blt_x, blt_y,
        blt_cx, blt_cy,
        hdc,
        blt_x, blt_y,
        SRCCOPY
    );

    if (SUCCEEDED(hr))
    {
        hr = DWriteDrawText(x, y, right_align_fix, color);
    }

    // Transfer from DWrite's rendering target to the window.
    BitBlt(
        hdc,
        blt_x, blt_y,
        blt_cx, blt_cy,
        memoryHdc,
        blt_x, blt_y,
        SRCCOPY
    );
}

/******************************************************************
*                                                                 *
*  DWriteOnPaint                                                  *
*                                                                 *
*  Clears the memory DC of the bitmap render target, renders to   *
*  it using a custom text renderer, and then copies it to the     *
*  window device context using the BitBlt function.               *
*                                                                 *
******************************************************************/

HRESULT DWriteDrawText(int x, int y, bool right_align, COLORREF color)
{
    HRESULT hr = S_OK;

    ((GdiTextRenderer*)g_pGdiTextRenderer)->setColor(color);
    hr = g_pTextLayout->Draw(
        NULL,
        g_pGdiTextRenderer,
        right_align ? 0.0f : (float)x / screen_scale_factor,
        (float)y / screen_scale_factor);

    return hr;
}

/******************************************************************
*                                                                 *
*  DWriteCreateResources                                          *
*                                                                 *
*  Creates the global DirectWrite objects needed for rendering,   *
*  including the IDWriteGdiInterop and IDWriteBitmapRenderTarget  *
*  interfaces.                                                    *
*                                                                 *
*  It also converts the GDI LOGFONT structure to a IDWriteFont    *
*  object and creates a IDWriteTextLayout object using this       *
*  information.                                                   *
*                                                                 *
******************************************************************/

HRESULT DWriteInit()
{
    HRESULT hr = S_OK;

    if (!g_pDWriteFactory)
    {
        // Create the DirectWrite factory.
        hr = DWriteCreateFactory_ptr(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&g_pDWriteFactory)
        );
    }

    // Create a GDI interop interface.
    if (SUCCEEDED(hr))
    {
        if (!g_pGdiInterop)
            hr = g_pDWriteFactory->GetGdiInterop(&g_pGdiInterop);
    }

    // Create a bitmap render target for our custom renderer.
    if (SUCCEEDED(hr))
    {
        //SafeRelease(&g_pBitmapRenderTarget);
        if (!g_pBitmapRenderTarget)
        {
            RECT r;
            r.left = 0;
            r.right = (*base_game)->prog_info->game_wid;
            r.top = 0;
            r.bottom = (*base_game)->prog_info->game_hgt;

            hr = g_pGdiInterop->CreateBitmapRenderTarget(NULL, r.right, r.bottom, &g_pBitmapRenderTarget);
        }
    }

    // Create default rendering params for our custom renderer.
    if (SUCCEEDED(hr))
    {
        //SafeRelease(&g_pRenderingParams);
        if (!g_pRenderingParams)
            hr = g_pDWriteFactory->CreateRenderingParams(&g_pRenderingParams);
    }

    if (SUCCEEDED(hr))
    {
        // Initialize the custom renderer class.
        if (!g_pGdiTextRenderer)
            g_pGdiTextRenderer = new GdiTextRenderer(g_pBitmapRenderTarget, g_pRenderingParams);
    }

    return hr;
}

HRESULT DWritePrepareText(HDC hdc, const wchar_t* text, HFONT hfont, int x, int y, int cx, int cy, UINT format)
{
    DWriteInit();

    HRESULT hr = S_OK;

    // DirectWrite variables.
    IDWriteFontFamily* pFontFamily = NULL;
    IDWriteFont* pFont = NULL;
    IDWriteLocalizedStrings* pFamilyNames = NULL;

    // Logical (GDI) font.
    LOGFONTW lf = {};

    UINT32 length = 0;
    UINT32 index = 0;
    float fontSize = 0;

    // length of the string
    UINT32 textLength = 0;

    wchar_t* name = NULL;

    // Calculate the string length.
    textLength = UINT32(wcslen(text));

    if (SUCCEEDED(hr))
    {
        // Get a logical font from the font handle.
        GetObjectW(hfont, sizeof(LOGFONTW), &lf);
    }

    // Convert to a DirectWrite font.
    if (SUCCEEDED(hr))
    {
        hr = g_pGdiInterop->CreateFontFromLOGFONT(&lf, &pFont);
    }

    if (!SUCCEEDED(hr))
    {
        wcscpy(lf.lfFaceName, L"Arial");
        hr = g_pGdiInterop->CreateFontFromLOGFONT(&lf, &pFont);
    }

    // Get the font family.
    if (SUCCEEDED(hr))
    {
        hr = pFont->GetFontFamily(&pFontFamily);
    }

    // Get a list of localized family names.
    if (SUCCEEDED(hr))
    {
        hr = pFontFamily->GetFamilyNames(&pFamilyNames);
    }

    // Select the first locale.  This is OK, because we are not displaying the family name.
    index = 0;

    // Get the length of the family name.
    if (SUCCEEDED(hr))
    {
        hr = pFamilyNames->GetStringLength(index, &length);
    }

    if (SUCCEEDED(hr))
    {
        // Allocate a new string.
        name = new wchar_t[length + 1];
        if (name == NULL)
        {
            hr = E_OUTOFMEMORY;
        }
    }

    // Get the actual family name.
    if (SUCCEEDED(hr))
    {
        hr = pFamilyNames->GetString(index, name, length + 1);
    }

    if (SUCCEEDED(hr))
    {
        // Calculate the font size.
        fontSize = abs((float)MulDiv(lf.lfHeight, 96, GetDeviceCaps(hdc, LOGPIXELSY)));
    }

    if (cd.largeText)
    {
        if (fontSize >= 19 && fontSize < 20)
            fontSize *= 1.0;
        else if (fontSize >= 13 && fontSize < 19)
            fontSize *= 1.125;
        else if (fontSize >= 12 && fontSize < 13)
            fontSize *= 1.25;
        else if (fontSize < 12)
            fontSize *= 1.25;

        fontSize = floor(fontSize);
    }

    // Create a text format using the converted font information.
    if (SUCCEEDED(hr))
    {
        SafeRelease(&g_pTextFormat);
        hr = g_pDWriteFactory->CreateTextFormat(
            name,                // Font family name.
            NULL,
            pFont->GetWeight(),
            pFont->GetStyle(),
            pFont->GetStretch(),
            fontSize,
            L"en-us",
            &g_pTextFormat
        );
        if (format & DT_RIGHT)
            g_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
        if (format & DT_CENTER)
            g_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        if (format & DT_BOTTOM)
            g_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
    }

    // Create a text layout.
    if (SUCCEEDED(hr))
    {
        SafeRelease(&g_pTextLayout);
        hr = g_pDWriteFactory->CreateTextLayout(
            text,
            textLength,
            g_pTextFormat,
            1024.0f,
            480.0f,
            &g_pTextLayout
        );
        
        if ((format & DT_RIGHT) && cx == -1 && cy == -1)
            g_pTextLayout->SetMaxWidth(x / screen_scale_factor);
        if (cx != -1)
            g_pTextLayout->SetMaxWidth(cx / screen_scale_factor);
        if (cy != -1)
            g_pTextLayout->SetMaxHeight(cy / screen_scale_factor);
    }

    // Underline and strikethrough are part of a LOGFONT structure, but are not
    // part of a DWrite font object so we must set them using the text layout.
    if (lf.lfUnderline)
    {
        DWRITE_TEXT_RANGE textRange = { 0, textLength };
        g_pTextLayout->SetUnderline(true, textRange);
    }

    if (lf.lfStrikeOut)
    {
        DWRITE_TEXT_RANGE textRange = { 0, textLength };
        g_pTextLayout->SetStrikethrough(true, textRange);
    }

    // Clean up local interfaces.
    SafeRelease(&pFontFamily);
    SafeRelease(&pFont);
    SafeRelease(&pFamilyNames);

    delete[] name;

    return hr;
}

COLORREF txt_color = 0;
UINT text_align = 0;

std::wstring AnsiToWide(const char* lpString, int c)
{
    int count = MultiByteToWideChar(CP_ACP, 0, lpString, c, NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_ACP, 0, lpString, c, &wstr[0], count);
    return wstr;
}

BOOL __stdcall GetTextExtentPoint32W_new(HDC hdc, LPCWSTR lpString, int c, LPSIZE psizl)
{
    DWriteProcessText(false, hdc, 0, 0, -1, -1, lpString, c, txt_color, text_align, psizl);

    return TRUE;
}

BOOL __stdcall GetTextExtentPoint32A_new(HDC hdc, LPCSTR lpString, int c, LPSIZE psizl)
{
    return GetTextExtentPoint32W_new(hdc, AnsiToWide(lpString, c).c_str(), c, psizl);
}

/*
COLORREF __stdcall SetTextColor_new(HDC hdc, COLORREF color)
{
    COLORREF prev = txt_color;
    txt_color = color;
    return prev;
}
*/

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

BOOL __stdcall TextOutW_new(HDC hdc, int x, int y, LPCWSTR lpString, int c)
{
    DWriteProcessText(true, hdc, x, y, -1, -1, lpString, c, txt_color, text_align & TA_RIGHT ? DT_RIGHT : 0, NULL);

    return TRUE;
}

BOOL __stdcall TextOutA_new(HDC hdc, int x, int y, LPCSTR lpString, int c)
{
    return TextOutW_new(hdc, x, y, AnsiToWide(lpString, c).c_str(), c);
}

int __stdcall DrawTextW_new(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format)
{
    DWriteProcessText(true, hdc, lprc->left, lprc->top, lprc->right - lprc->left, lprc->bottom - lprc->top, lpchText, cchText, txt_color, format, NULL);

    return 0;
}

int __stdcall DrawTextA_new(HDC hdc, LPCSTR lpchText, int cchText, LPRECT lprc, UINT format)
{
    return DrawTextW_new(hdc, AnsiToWide(lpchText, cchText).c_str(), cchText, lprc, format);
}

UINT __stdcall SetTextAlign_new(HDC hdc, UINT align)
{
    UINT prev = text_align;
    text_align = align;
    return prev;
}

bool check_dwrite_available()
{
    HMODULE h = LoadLibrary("dwrite.dll");
    if (!h)
        return false;

    DWriteCreateFactory_ptr = (HRESULT(__stdcall*)(DWRITE_FACTORY_TYPE, const IID&, IUnknown**))GetProcAddress(h, "DWriteCreateFactory");
    if (!DWriteCreateFactory_ptr)
        return false;

    return true;
}

#pragma optimize( "s", on )
void setTextRenderHooks(int version)
{
    setHook(GetProcAddress(GetModuleHandle("gdi32.dll"), "SetTextAlign"), SetTextAlign_new);
    setHook(GetProcAddress(GetModuleHandle("gdi32.dll"), "TextOutA"), TextOutA_new);
    setHook(GetProcAddress(GetModuleHandle("gdi32.dll"), "TextOutW"), TextOutW_new);
    setHook(GetProcAddress(GetModuleHandle("gdi32.dll"), "GetTextExtentPoint32A"), GetTextExtentPoint32A_new);
    setHook(GetProcAddress(GetModuleHandle("gdi32.dll"), "GetTextExtentPoint32W"), GetTextExtentPoint32W_new);
    //setHook(GetProcAddress(GetModuleHandle("gdi32.dll"), "SetTextColor"), SetTextColor_new);

    make_set_text_color_hook();

    setHook(GetProcAddress(GetModuleHandle("user32.dll"), "DrawTextA"), DrawTextA_new);
    setHook(GetProcAddress(GetModuleHandle("user32.dll"), "DrawTextW"), DrawTextW_new);

    //popup help first letter
    writeByte(0x004D31CF, 0x90);

    //slower tech tree mouse scroll refresh rate
    writeByte(0x0046BDF8, 33);

    if (version == VER_EF)
    {
        //transparent color index
        writeByte(0x005E002A, 177);
        writeByte(0x005DFA85, 177);
        writeByte(0x005DFABA, 177);

        writeByte(0x004CDB92, 177);
        writeByte(0x004CD427, 177);
        writeByte(0x004CD45C, 177);
    }
}
#pragma optimize( "", on )
