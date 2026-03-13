#include "stdafx.h"
#include "textrender.h"
#include "registry.h"
#include "palfile.h"

#include <dwrite.h>

#ifdef EFPATCH_ENABLE_TIMING
#include <chrono>
#endif // EFPATCH_ENABLE_TIMING

float screen_scale_factor = 1.0f;

bool dwrite_checked_and_available = false;

HRESULT (__stdcall* DWriteCreateFactory_ptr)(
    __in DWRITE_FACTORY_TYPE factoryType,
    __in REFIID iid,
    __out IUnknown** factory
);

HRESULT DWriteDrawText(
    IDWriteTextLayout* pTextLayout,
    int x,
    int y,
    bool right_align,
    COLORREF color
);

HRESULT DWritePrepareText(
    HDC hdc,
    std::wstring_view text,
    HFONT hfont,
    int x,
    int y,
    int cx,
    int cy,
    UINT format,
    IDWriteTextLayout** ppTextLayout,
    DWRITE_TEXT_METRICS* ptm
);

// DirectWrite global variables.
IDWriteFactory* g_pDWriteFactory = NULL;
IDWriteGdiInterop* g_pGdiInterop = NULL;
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
    r.left = floor(baselineOriginX * screen_scale_factor);
    r.top = floor((baselineOriginY + strikethrough->offset) * screen_scale_factor);
    r.right = ceil((baselineOriginX + strikethrough->width) * screen_scale_factor);
    r.bottom = ceil((baselineOriginY + strikethrough->offset + strikethrough->thickness) * screen_scale_factor);

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
    if (__uuidof(IDWriteTextRenderer) == riid ||
        __uuidof(IDWritePixelSnapping) == riid ||
        __uuidof(IUnknown) == riid)
    {
        *ppvObject = this;
        AddRef();
        return S_OK;
    }
    else
    {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
}

/*
BOOL SaveHBITMAPToFile(HBITMAP hBitmap, LPCWSTR lpszFileName)
{
    HDC hDC;
    int iBits;
    WORD wBitCount;
    DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
    BITMAP Bitmap0;
    BITMAPFILEHEADER bmfHdr;
    BITMAPINFOHEADER bi;
    LPBITMAPINFOHEADER lpbi;
    HANDLE fh, hDib, hPal, hOldPal2 = NULL;
    hDC = CreateDCW(L"DISPLAY", NULL, NULL, NULL);
    iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
    DeleteDC(hDC);
    if (iBits <= 1)
        wBitCount = 1;
    else if (iBits <= 4)
        wBitCount = 4;
    else if (iBits <= 8)
        wBitCount = 8;
    else
        wBitCount = 24;
    GetObjectW(hBitmap, sizeof(Bitmap0), (LPWSTR)&Bitmap0);
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = Bitmap0.bmWidth;
    bi.biHeight = -Bitmap0.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = wBitCount;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrImportant = 0;
    bi.biClrUsed = 256;
    dwBmBitsSize = ((Bitmap0.bmWidth * wBitCount + 31) & ~31) / 8
        * Bitmap0.bmHeight;
    hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
    *lpbi = bi;

    hPal = GetStockObject(DEFAULT_PALETTE);
    if (hPal)
    {
        hDC = GetDC(NULL);
        hOldPal2 = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
        RealizePalette(hDC);
    }


    GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap0.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
        + dwPaletteSize, (BITMAPINFO*)lpbi, DIB_RGB_COLORS);

    if (hOldPal2)
    {
        SelectPalette(hDC, (HPALETTE)hOldPal2, TRUE);
        RealizePalette(hDC);
        ReleaseDC(NULL, hDC);
    }

    fh = CreateFileW(lpszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (fh == INVALID_HANDLE_VALUE)
        return FALSE;

    bmfHdr.bfType = 0x4D42; // "BM"
    dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
    bmfHdr.bfSize = dwDIBSize;
    bmfHdr.bfReserved1 = 0;
    bmfHdr.bfReserved2 = 0;
    bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

    WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

    WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
    GlobalUnlock(hDib);
    GlobalFree(hDib);
    CloseHandle(fh);
    return TRUE;
}
*/

struct DWriteProcessText_StaticVars
{
    HDC dc_indexed_target;
    BITMAPINFO* pbmi_indexed_target;
    HBITMAP bm_indexed_target;
    HBITMAP bm_indexed_target_old;
    BYTE* px_indexed_target;
    PALETTEENTRY palette_entries[256];
    COLOR_TABLE* color_table;
    DIBSECTION ds;
    bool force_reset;

    void set_memory_dc(HDC memory_dc)
    {
        dc_indexed_target = CreateCompatibleDC(memory_dc);
        GetObjectW(GetCurrentObject(memory_dc, OBJ_BITMAP), sizeof(ds), (void*)&ds);
        SetBoundsRect(memory_dc, NULL, DCB_ENABLE | DCB_RESET);
        force_reset = true;
    }

    void reset_memory_dc(HDC memory_dc)
    {
        DeleteDC(dc_indexed_target);
        set_memory_dc(memory_dc);
    }

    explicit DWriteProcessText_StaticVars(
        HDC memory_dc)
    {
        set_memory_dc(memory_dc);

        pbmi_indexed_target = (BITMAPINFO*)malloc(sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
        bm_indexed_target = NULL;
        px_indexed_target = NULL;
        memset(palette_entries, 0, sizeof(palette_entries));
        color_table = NULL;
    }

    ~DWriteProcessText_StaticVars()
    {
        free(pbmi_indexed_target);

        if (bm_indexed_target != NULL)
        {
            SelectObject(dc_indexed_target, bm_indexed_target_old);
            DeleteObject(bm_indexed_target);
        }

        DeleteDC(dc_indexed_target);

        if (color_table != NULL)
            delete color_table;
    }

    void reset_color_table()
    {
        memcpy(palette_entries, (*base_game)->draw_system->palette, sizeof(PALETTEENTRY) * 256);

        if (color_table != NULL)
            delete color_table;

        color_table = new COLOR_TABLE(palette_entries);
    }

    bool reset_indexed_bm()
    {
        memset(pbmi_indexed_target, 0, sizeof(BITMAPINFO));
        pbmi_indexed_target->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        pbmi_indexed_target->bmiHeader.biWidth = (*base_game)->prog_info->game_wid;
        pbmi_indexed_target->bmiHeader.biHeight = (*base_game)->prog_info->game_hgt;
        pbmi_indexed_target->bmiHeader.biPlanes = 1;
        pbmi_indexed_target->bmiHeader.biCompression = BI_RGB;
        pbmi_indexed_target->bmiHeader.biBitCount = 8;
        pbmi_indexed_target->bmiHeader.biClrUsed = 256;

        for (int i = 0; i < 256; i++)
        {
            pbmi_indexed_target->bmiColors[i].rgbRed = palette_entries[i].peRed; //GetRValue(e);
            pbmi_indexed_target->bmiColors[i].rgbGreen = palette_entries[i].peGreen; //GetGValue(e);
            pbmi_indexed_target->bmiColors[i].rgbBlue = palette_entries[i].peBlue; ////GetBValue(e);
            pbmi_indexed_target->bmiColors[i].rgbReserved = 0;
        }

        if (bm_indexed_target != NULL)
        {
            SelectObject(dc_indexed_target, bm_indexed_target_old);
            DeleteObject(bm_indexed_target);
        }

        bm_indexed_target = CreateDIBSection(
            dc_indexed_target,
            pbmi_indexed_target,
            DIB_RGB_COLORS,
            (void**)&px_indexed_target,
            NULL, 0
        );

        if (bm_indexed_target == NULL)
            return false;

        bm_indexed_target_old = (HBITMAP)SelectObject(dc_indexed_target, bm_indexed_target);

        return true;
    }
};

HRESULT DWriteProcessText(
    bool do_paint,
    HDC hdc,
    int x,
    int y,
    int cx,
    int cy,
    std::wstring_view text,
    COLORREF color,
    UINT format,
    LPRECT layout_rect
)
{
#ifdef EFPATCH_ENABLE_TIMING
    bool do_timing = false;
    auto start_time = std::chrono::high_resolution_clock::now();
    auto report_time = [&do_timing, &start_time](const char* info)
        {
            if (do_timing)
            {
                auto end_time = std::chrono::high_resolution_clock::now();
                auto time = end_time - start_time;
                auto us = time / std::chrono::microseconds(1);
                log("Timer: after %s: %lld us", info, us);
                start_time = std::chrono::high_resolution_clock::now();
            }
        };

    if (true)
    {
        log("*** BEGIN TIMING");
        log("%s [%d, %d, %d, %d]", WideToUTF8_c_str(text), x, y, cx, cy);
        do_timing = true;
    }

    report_time("timing start");

#endif // EFPATCH_ENABLE_TIMING

    HRESULT hr = S_OK;
    HFONT hfont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);

    bool right_align = format & DT_RIGHT;

    // The DirectWrite objects need only be created once and can be reused each time the window paints
    IDWriteTextLayout* pTextLayout;
    DWRITE_TEXT_METRICS tm;
    hr = DWritePrepareText(
        hdc,
        text,
        hfont,
        x, y,
        cx, cy,
        format,
        &pTextLayout,
        &tm
    );

#ifdef EFPATCH_ENABLE_TIMING
    report_time("DWritePrepareText()");
#endif // EFPATCH_ENABLE_TIMING

    if (FAILED(hr))
        return hr;

    tm.width = tm.widthIncludingTrailingWhitespace * screen_scale_factor;
    tm.height *= screen_scale_factor;
    tm.left *= screen_scale_factor;
    tm.top *= screen_scale_factor;

    if (layout_rect)
    {
        layout_rect->left = floor(tm.left);
        layout_rect->top = floor(tm.top);
        layout_rect->right = ceil(tm.left + tm.width);
        layout_rect->bottom = ceil(tm.top + tm.height);
    }

#ifdef EFPATCH_ENABLE_TIMING
    report_time("DWRITE_TEXT_METRICS adjust");
#endif // EFPATCH_ENABLE_TIMING

    if (!do_paint)
        return hr;

    HDC dc_dwrite_target = NULL;

    // Get the memory device context, the drawing is done offscreen.
    dc_dwrite_target = g_pBitmapRenderTarget->GetMemoryDC();

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

    static DWriteProcessText_StaticVars static_vars(dc_dwrite_target);

    if (static_vars.ds.dsBm.bmWidth != (*base_game)->prog_info->game_wid ||
        static_vars.ds.dsBm.bmHeight != (*base_game)->prog_info->game_hgt)
    {
        static_vars.reset_memory_dc(dc_dwrite_target);
        static_vars.force_reset = true;
    }

    if (blt_x + blt_cx > static_vars.ds.dsBm.bmWidth)
        blt_cx = static_vars.ds.dsBm.bmWidth - blt_x;

    if (blt_y + blt_cy > static_vars.ds.dsBm.bmHeight)
        blt_cy = static_vars.ds.dsBm.bmHeight - blt_y;

    BitBlt(
        dc_dwrite_target,
        blt_x, blt_y,
        blt_cx, blt_cy,
        hdc,
        blt_x, blt_y,
        SRCCOPY
    );

#ifdef EFPATCH_ENABLE_TIMING
    report_time("BitBlt dc_main -> dc_dwrite_target");
#endif // EFPATCH_ENABLE_TIMING

    hr = DWriteDrawText(pTextLayout, x, y, right_align_fix, color);

#ifdef EFPATCH_ENABLE_TIMING
    report_time("DWriteDrawText");
#endif // EFPATCH_ENABLE_TIMING

    if (FAILED(hr))
        return hr;

    if (static_vars.force_reset || memcmp(static_vars.palette_entries, (*base_game)->draw_system->palette, sizeof(PALETTEENTRY) * 256))
    {
#ifdef EFPATCH_ENABLE_TIMING
        start_time = std::chrono::high_resolution_clock::now();
#endif // EFPATCH_ENABLE_TIMING

        static_vars.reset_color_table();
        if (!static_vars.reset_indexed_bm())
            return E_FAIL;

        static_vars.force_reset = false;

#ifdef EFPATCH_ENABLE_TIMING
        bool do_timing_temp = do_timing;
        do_timing = true;
        report_time("Reset color table and bitmap");
        do_timing = do_timing_temp;
#endif // EFPATCH_ENABLE_TIMING
    }

    for (int y = blt_y; y < (blt_y + blt_cy); y++)
    {
        int y2 = static_vars.ds.dsBm.bmHeight - 1 - y;
        DWORD* row_start = (DWORD*)((BYTE*)static_vars.ds.dsBm.bmBits + (y * static_vars.ds.dsBm.bmWidth + blt_x) * 4);
        BYTE* row_dst = static_vars.px_indexed_target + (y2 * static_vars.pbmi_indexed_target->bmiHeader.biWidth + blt_x);
        int last_index = 0;
        DWORD last_rgb = 0;
        for (int x = 0; x < blt_cx; x++)
        {
            if (row_start[x] == last_rgb)
            {
                row_dst[x] = last_index;
            }
            else
            {
                last_rgb = row_start[x];
                last_index = static_vars.color_table->get_index_rgb(last_rgb);
                row_dst[x] = last_index;
            }
        }
    }

#ifdef EFPATCH_ENABLE_TIMING
    report_time("Write pixel buffer dc_rgb_target -> dc_indexed_target");
#endif // EFPATCH_ENABLE_TIMING

    // Transfer from DWrite's rendering target to the window.
    BitBlt(
        hdc,
        blt_x, blt_y,
        blt_cx, blt_cy,
        static_vars.dc_indexed_target,
        blt_x, blt_y,
        SRCCOPY
    );

#ifdef EFPATCH_ENABLE_TIMING
    report_time("BitBlt dc_indexed_target -> dc_main");
#endif // EFPATCH_ENABLE_TIMING

    return hr;
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

HRESULT DWriteDrawText(IDWriteTextLayout* pTextLayout, int x, int y, bool right_align, COLORREF color)
{
    HRESULT hr = S_OK;

    ((GdiTextRenderer*)g_pGdiTextRenderer)->setColor(color);
    hr = pTextLayout->Draw(
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

int dwrite_target_x = 0;
int dwrite_target_y = 0;

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
    if (SUCCEEDED(hr) && !g_pGdiInterop)
    {
        hr = g_pDWriteFactory->GetGdiInterop(&g_pGdiInterop);
    }

    // Create a bitmap render target for our custom renderer.
    if (SUCCEEDED(hr) && !g_pBitmapRenderTarget)
    {
        hr = g_pGdiInterop->CreateBitmapRenderTarget(
            NULL,
            dwrite_target_x = (*base_game)->prog_info->game_wid,
            dwrite_target_y = (*base_game)->prog_info->game_hgt,
            &g_pBitmapRenderTarget
        );
    }

    // Create default rendering params for our custom renderer.
    if (SUCCEEDED(hr) && !g_pRenderingParams)
    {
        hr = g_pDWriteFactory->CreateRenderingParams(&g_pRenderingParams);
    }

    // Initialize the custom renderer class.
    if (SUCCEEDED(hr) && !g_pGdiTextRenderer)
    {
        g_pGdiTextRenderer = new GdiTextRenderer(g_pBitmapRenderTarget, g_pRenderingParams);
    }

    return hr;
}

HRESULT DWriteDeInit()
{
    SafeRelease(&g_pGdiTextRenderer);
    SafeRelease(&g_pBitmapRenderTarget);

    return S_OK;
}

struct DWritePrepareText_StaticVars
{
    LOGFONTW lf;
    UINT format;
    int hdc_caps_logpixelsy;

    IDWriteTextFormat* pTextFormat = NULL;
    IDWriteTextLayout* pTextLayout = NULL;
    DWRITE_TEXT_METRICS tm;
    //int cx;
    //int cy;
    std::wstring text;

    bool force_reset;

    void reset(HDC hdc)
    {
        hdc_caps_logpixelsy = GetDeviceCaps(hdc, LOGPIXELSY);
        memset(&lf, 0, sizeof(LOGFONTW));
        format = 0;
        SafeRelease(&pTextLayout);
        SafeRelease(&pTextFormat);
    }

    explicit DWritePrepareText_StaticVars(HDC hdc)
    {
        reset(hdc);
        force_reset = true;
    }

    ~DWritePrepareText_StaticVars()
    {
        SafeRelease(&pTextLayout);
        SafeRelease(&pTextFormat);
    }

    HRESULT reset_font(const LPLOGFONTW plf, UINT format_)
    {
        memcpy(&lf, plf, sizeof(LOGFONTW));
        format = format_;

        HRESULT hr = S_OK;

        UINT32 length = 0;
        UINT32 index = 0;
        float fontSize = 0;

        IDWriteFont* pFont = NULL;
        IDWriteFontFamily* pFontFamily = NULL;
        IDWriteLocalizedStrings* pFamilyNames = NULL;

        // Convert to a DirectWrite font.
        if (SUCCEEDED(hr))
        {
            hr = g_pGdiInterop->CreateFontFromLOGFONT(&lf, &pFont);
        }

        if (FAILED(hr))
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
            std::wstring name(length + 1, L'\0');
            hr = pFamilyNames->GetString(index, &name[0], length + 1);

            if (SUCCEEDED(hr))
            {
                // Calculate the font size.
                fontSize = abs((float)MulDiv(lf.lfHeight, 96, hdc_caps_logpixelsy));

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
            }

            // Create a text format using the converted font information.
            if (SUCCEEDED(hr))
            {
                SafeRelease(&pTextFormat);
                hr = g_pDWriteFactory->CreateTextFormat(
                    name.c_str(),                // Font family name.
                    NULL,
                    pFont->GetWeight(),
                    pFont->GetStyle(),
                    pFont->GetStretch(),
                    fontSize,
                    L"en-us",
                    &pTextFormat
                );
            }

            if (SUCCEEDED(hr) && (format & DT_RIGHT))
            {
                hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
            }

            if (SUCCEEDED(hr) && (format & DT_CENTER))
            {
                hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            }

            if (SUCCEEDED(hr) && (format & DT_BOTTOM))
            {
                hr = pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
            }
        }

        // Clean up local interfaces.        
        SafeRelease(&pFamilyNames);
        SafeRelease(&pFontFamily);
        SafeRelease(&pFont);

        return hr;
    }
};

HRESULT DWritePrepareText(
    HDC hdc,
    std::wstring_view text,
    HFONT hfont,
    int x,
    int y,
    int cx,
    int cy,
    UINT format,
    IDWriteTextLayout** ppTextLayout,
    DWRITE_TEXT_METRICS* ptm)
{
#ifdef EFPATCH_ENABLE_TIMING
    bool do_timing = false;
    auto start_time = std::chrono::high_resolution_clock::now();
    auto report_time = [&do_timing, &start_time](const char* info)
        {
            if (do_timing)
            {
                auto end_time = std::chrono::high_resolution_clock::now();
                auto time = end_time - start_time;
                auto us = time / std::chrono::nanoseconds(1);
                log(" * DWritePrepareText: Timer: after %s: %lld ns", info, us);
                start_time = std::chrono::high_resolution_clock::now();
            }
        };
#endif // EFPATCH_ENABLE_TIMING

    HRESULT hr = S_OK;

    static DWritePrepareText_StaticVars static_vars(hdc);

    if (dwrite_target_x != (*base_game)->prog_info->game_wid ||
        dwrite_target_y != (*base_game)->prog_info->game_hgt)
    {
        hr = DWriteDeInit();
        static_vars.force_reset = true;
    }

    if (SUCCEEDED(hr))
    {
        hr = DWriteInit();
    }

#ifdef EFPATCH_ENABLE_TIMING
    report_time("DWriteInit");
#endif // EFPATCH_ENABLE_TIMING

    LOGFONTW lf;

    // Get a logical font from the font handle.
    memset(&lf, 0, sizeof(LOGFONTW));
    GetObjectW(hfont, sizeof(LOGFONTW), &lf);

#ifdef EFPATCH_ENABLE_TIMING
    report_time("GetObjectW");
#endif // EFPATCH_ENABLE_TIMING

    if (static_vars.force_reset || format != static_vars.format || memcmp(&lf, &static_vars.lf, sizeof(LOGFONTW)))
    {
        if (SUCCEEDED(hr))
        {
            hr = static_vars.reset_font(&lf, format);
        }

#ifdef EFPATCH_ENABLE_TIMING
        report_time("reset_font");
#endif // EFPATCH_ENABLE_TIMING

        static_vars.force_reset = true;
    }

    if (text != std::wstring_view(static_vars.text))
    {
        static_vars.force_reset = true;
        static_vars.text = std::wstring(text);
    }

    if (static_vars.force_reset || (format & DT_RIGHT))
    {
        // Create a text layout.
        if (SUCCEEDED(hr))
        {
            SafeRelease(&static_vars.pTextLayout);
            hr = g_pDWriteFactory->CreateTextLayout(
                text.data(),
                text.size(),
                static_vars.pTextFormat,
                1024.0f,
                480.0f,
                &static_vars.pTextLayout
            );
        }

        if (SUCCEEDED(hr) && (format & DT_RIGHT) && cx == -1 && cy == -1)
        {
            hr = static_vars.pTextLayout->SetMaxWidth(x / screen_scale_factor);
        }

        if (SUCCEEDED(hr) && cx != -1)
        {
            hr = static_vars.pTextLayout->SetMaxWidth(cx / screen_scale_factor);
        }

        if (SUCCEEDED(hr) && cy != -1)
        {
            hr = static_vars.pTextLayout->SetMaxHeight(cy / screen_scale_factor);
        }

        // Underline and strikethrough are part of a LOGFONT structure, but are not
        // part of a DWrite font object so we must set them using the text layout.
        if (SUCCEEDED(hr) && lf.lfUnderline)
        {
            DWRITE_TEXT_RANGE textRange = { 0, text.length() };
            hr = static_vars.pTextLayout->SetUnderline(true, textRange);
        }

        if (SUCCEEDED(hr) && lf.lfStrikeOut)
        {
            DWRITE_TEXT_RANGE textRange = { 0, text.length() };
            hr = static_vars.pTextLayout->SetStrikethrough(true, textRange);
        }

#ifdef EFPATCH_ENABLE_TIMING
        report_time("CreateTextLayout");
#endif // EFPATCH_ENABLE_TIMING

        if (SUCCEEDED(hr))
        {
            hr = static_vars.pTextLayout->GetMetrics(&static_vars.tm);
        }

#ifdef EFPATCH_ENABLE_TIMING
        report_time("GetMetrics");
#endif // EFPATCH_ENABLE_TIMING
    }

    static_vars.force_reset = false;

    *ppTextLayout = static_vars.pTextLayout;
    memcpy(ptm, &static_vars.tm, sizeof(DWRITE_TEXT_METRICS));

    return hr;
}

COLORREF txt_color = 0;
UINT text_align = 0;

bool GetTextExtentPoint32_str(HDC hdc, std::wstring_view text, LPSIZE psizl)
{
    RECT r;
    HRESULT hr = DWriteProcessText(
        false,
        hdc,
        0, 0,
        -1, -1,
        text,
        txt_color,
        text_align,
        &r
    );
    psizl->cx = r.right - r.left;
    psizl->cy = r.bottom - r.top;

    return SUCCEEDED(hr);
}

BOOL __stdcall GetTextExtentPoint32W_new(HDC hdc, LPCWSTR lpString, int c, LPSIZE psizl)
{
    return GetTextExtentPoint32_str(hdc, std::wstring_view(lpString, c), psizl);
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
    void* address = GetProcAddress(GetModuleHandleW(L"gdi32.dll"), "SetTextColor");
    writeWord((DWORD)address, 0xF9EB);
    writeByte((DWORD)address - 5, 0xE9);
    writeDword((DWORD)address - 4, (DWORD)set_text_color_jmp - (DWORD)address);
    text_color_jump_address = (void*)((DWORD)address + 2);
}

bool TextOut_str(HDC hdc, int x, int y, std::wstring_view text)
{
    HRESULT hr = DWriteProcessText(
        true,
        hdc,
        x, y,
        -1, -1,
        text,
        txt_color,
        text_align & TA_RIGHT ? DT_RIGHT : 0,
        NULL
    );

    return SUCCEEDED(hr);
}

BOOL __stdcall TextOutW_new(HDC hdc, int x, int y, LPCWSTR lpString, int c)
{
    return TextOut_str(hdc, x, y, std::wstring_view(lpString, c));
}

int DrawText_str(HDC hdc, std::wstring_view text, LPRECT lprc, UINT format)
{
    RECT r;
    HRESULT hr = DWriteProcessText(
        true,
        hdc,
        lprc->left, lprc->top,
        lprc->right - lprc->left, lprc->bottom - lprc->top,
        text,
        txt_color,
        format,
        &r
    );

    if (SUCCEEDED(hr))
        return r.bottom - lprc->top;
    else
        return 0;
}

int __stdcall DrawTextW_new(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format)
{
    return DrawText_str(hdc, std::wstring_view(lpchText, cchText != -1 ? cchText : wcslen(lpchText)), lprc, format);
}

UINT __stdcall SetTextAlign_new(HDC hdc, UINT align)
{
    UINT prev = text_align;
    text_align = align;
    return prev;
}

bool check_dwrite_available()
{
    if (dwrite_checked_and_available)
        return true;

    HMODULE h = LoadLibraryW(L"dwrite.dll");
    if (!h)
        return false;

    DWriteCreateFactory_ptr = (HRESULT(__stdcall*)(DWRITE_FACTORY_TYPE, const IID&, IUnknown**))GetProcAddress(h, "DWriteCreateFactory");
    if (!DWriteCreateFactory_ptr)
        return false;

    dwrite_checked_and_available = true;
    return true;
}

#pragma optimize( "s", on )
void setTextRenderHooks(int version)
{
    setHook(GetProcAddress(GetModuleHandleW(L"gdi32.dll"), "SetTextAlign"), SetTextAlign_new);
    setHook(GetProcAddress(GetModuleHandleW(L"gdi32.dll"), "TextOutW"), TextOutW_new);
    setHook(GetProcAddress(GetModuleHandleW(L"gdi32.dll"), "GetTextExtentPoint32W"), GetTextExtentPoint32W_new);
    //setHook(GetProcAddress(GetModuleHandle("gdi32.dll"), "SetTextColor"), SetTextColor_new);

    make_set_text_color_hook();

    setHook(GetProcAddress(GetModuleHandleW(L"user32.dll"), "DrawTextW"), DrawTextW_new);

    //popup help first letter
    writeByte(0x004D31CF, 0x90);

    //slower tech tree mouse scroll refresh rate
    writeByte(0x0046BDF8, 33);

    /*
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
    */
}
#pragma optimize( "", on )
