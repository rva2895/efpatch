#pragma once

void setTextRenderHooks();

#include <dwrite.h>
#include <intsafe.h>
#include <comdef.h>

class GdiTextRenderer : public IDWriteTextRenderer
{
public:
    GdiTextRenderer(
        IDWriteBitmapRenderTarget* bitmapRenderTarget,
        IDWriteRenderingParams* renderingParams
    );

    GdiTextRenderer::~GdiTextRenderer();

    IFACEMETHOD(IsPixelSnappingDisabled)(
        __maybenull void* clientDrawingContext,
        __out BOOL* isDisabled
        );

    IFACEMETHOD(GetCurrentTransform)(
        __maybenull void* clientDrawingContext,
        __out DWRITE_MATRIX* transform
        );

    IFACEMETHOD(GetPixelsPerDip)(
        __maybenull void* clientDrawingContext,
        __out FLOAT* pixelsPerDip
        );

    IFACEMETHOD(DrawGlyphRun)(
        __maybenull void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_MEASURING_MODE measuringMode,
        __in DWRITE_GLYPH_RUN const* glyphRun,
        __in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
        IUnknown* clientDrawingEffect
        );

    IFACEMETHOD(DrawUnderline)(
        __maybenull void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        __in DWRITE_UNDERLINE const* underline,
        IUnknown* clientDrawingEffect
        );

    IFACEMETHOD(DrawStrikethrough)(
        __maybenull void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        __in DWRITE_STRIKETHROUGH const* strikethrough,
        IUnknown* clientDrawingEffect
        );

    IFACEMETHOD(DrawInlineObject)(
        __maybenull void* clientDrawingContext,
        FLOAT originX,
        FLOAT originY,
        IDWriteInlineObject* inlineObject,
        BOOL isSideways,
        BOOL isRightToLeft,
        IUnknown* clientDrawingEffect
        );

public:
    IFACEMETHOD_(unsigned long, AddRef) ();
    IFACEMETHOD_(unsigned long, Release) ();
    IFACEMETHOD(QueryInterface)(
        IID const& riid,
        void** ppvObject
        );

    void setColor(COLORREF color);

private:
    unsigned long cRefCount_;
    IDWriteBitmapRenderTarget* pRenderTarget_;
    IDWriteRenderingParams* pRenderingParams_;
    COLORREF color_;
};

// SafeRelease inline function.
template <class T> inline void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}
