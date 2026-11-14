#define NOMINMAX
#define WINDOWS_IGNORE_PACKING_MISMATCH
#include <Windows.h>
#include <ddraw.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <new>
#include <vector>

#include "d3d11_directdraw.h"
#include "d3d11_renderer.h"
#include "d3d11_window.h"
#include "resolution.h"
#include "registry.h"

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")

extern CONFIG_DATA cd;

namespace {

HWND g_window = nullptr;
DWORD g_displayWidth = 800;
DWORD g_displayHeight = 600;
using DirectDrawCreateProc = HRESULT (WINAPI*)(GUID*, LPDIRECTDRAW*, IUnknown*);
// System DirectDraw handles window and display modes.
DirectDrawCreateProc g_directDrawCreate = nullptr;
std::atomic<unsigned long> g_paletteGeneration{1};
char g_logPath[MAX_PATH]{};
class NativeDirectDraw;
class NativeSurface;
class NativePalette;
class NativeClipper;

HRESULT WINAPI GbDirectDrawCreate(GUID*, LPDIRECTDRAW*, IUnknown*);

void Log(const char* message) {
    OutputDebugStringA(message);
    if (!g_logPath[0]) return;
    HANDLE file = CreateFileA(g_logPath, FILE_APPEND_DATA, FILE_SHARE_READ, nullptr,
                              OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return;
    DWORD written = 0;
    WriteFile(file, message, static_cast<DWORD>(std::strlen(message)), &written, nullptr);
    CloseHandle(file);
}

void LogResult(const char* method, HRESULT result) {
    char message[160]{};
    std::snprintf(message, sizeof(message), "[GB D3D11] %s -> 0x%08lX.\r\n",
                  method, static_cast<unsigned long>(result));
    Log(message);
}

void LogSampled(const char* method, std::atomic<unsigned long>& counter) {
    const unsigned long count = ++counter;
    if (count <= 16 || (count & (count - 1)) == 0) {
        char message[128]{};
        std::snprintf(message, sizeof(message), "[GB D3D11] %s call #%lu.\r\n", method, count);
        Log(message);
    }
}

void SetLogPath(HMODULE module) {
    const DWORD length = GetModuleFileNameA(module, g_logPath, MAX_PATH);
    if (!length || length >= MAX_PATH) { g_logPath[0] = '\0'; return; }
    char* name = g_logPath + length;
    while (name != g_logPath && name[-1] != '\\' && name[-1] != '/') --name;
    lstrcpyA(name, "gb-d3d11-bootstrap.log");
}

class NativePalette final : public IDirectDrawPalette {
public:
    NativePalette(DWORD caps, const PALETTEENTRY* entries) : caps_(caps) {
        if (entries) std::memcpy(entries_, entries, sizeof(entries_));
        LogicalPalette256 logical{};
        logical.version = 0x0300;
        logical.count = 256;
        std::memcpy(logical.entries, entries_, sizeof(entries_));
        handle_ = CreatePalette(reinterpret_cast<LOGPALETTE*>(&logical));
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override {
        if (!object) return E_POINTER;
        *object = nullptr;
        if (iid != IID_IUnknown && iid != IID_IDirectDrawPalette) return E_NOINTERFACE;
        *object = static_cast<IDirectDrawPalette*>(this); AddRef(); return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return ++references_; }
    ULONG STDMETHODCALLTYPE Release() override { const ULONG n = --references_; if (!n) delete this; return n; }
    HRESULT STDMETHODCALLTYPE GetCaps(LPDWORD caps) override { if (!caps) return DDERR_INVALIDPARAMS; *caps = caps_; return DD_OK; }
    HRESULT STDMETHODCALLTYPE GetEntries(DWORD, DWORD base, DWORD count, LPPALETTEENTRY entries) override {
        if (!entries || base > 256 || count > 256 - base) return DDERR_INVALIDPARAMS;
        std::memcpy(entries, entries_ + base, count * sizeof(PALETTEENTRY)); return DD_OK;
    }
    HRESULT STDMETHODCALLTYPE Initialize(LPDIRECTDRAW, DWORD, LPPALETTEENTRY) override { return DDERR_ALREADYINITIALIZED; }
    HRESULT STDMETHODCALLTYPE SetEntries(DWORD, DWORD base, DWORD count, LPPALETTEENTRY entries) override {
        if (!entries || base > 256 || count > 256 - base) return DDERR_INVALIDPARAMS;
        std::memcpy(entries_ + base, entries, count * sizeof(PALETTEENTRY));
        if (handle_) SetPaletteEntries(handle_, base, count, entries);
        ++g_paletteGeneration;
        return NotifySurfaces();
    }
    const PALETTEENTRY* Entries() const { return entries_; }
    HPALETTE Handle() const { return handle_; }
    void Attach(NativeSurface* surface) { surfaces_.push_back(surface); }
    void Detach(NativeSurface* surface) { surfaces_.erase(std::remove(surfaces_.begin(), surfaces_.end(), surface), surfaces_.end()); }
private:
    struct LogicalPalette256 { WORD version; WORD count; PALETTEENTRY entries[256]; };
    ~NativePalette() { if (handle_) DeleteObject(handle_); }
    HRESULT NotifySurfaces();
    std::atomic<ULONG> references_{1};
    DWORD caps_;
    PALETTEENTRY entries_[256]{};
    HPALETTE handle_ = nullptr;
    std::vector<NativeSurface*> surfaces_;
};

// Surfaces without a palette use the display palette.
NativePalette* g_displayPalette = nullptr;
std::vector<NativeSurface*> g_nativeSurfaces;
HRESULT SetDisplayPalette(NativePalette* palette);

class NativeClipper final : public IDirectDrawClipper {
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override {
        if (!object) return E_POINTER;
        *object = nullptr;
        if (iid != IID_IUnknown && iid != IID_IDirectDrawClipper) return E_NOINTERFACE;
        *object = static_cast<IDirectDrawClipper*>(this); AddRef(); return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return ++references_; }
    ULONG STDMETHODCALLTYPE Release() override { const ULONG n = --references_; if (!n) delete this; return n; }
    HRESULT STDMETHODCALLTYPE GetClipList(LPRECT rect, LPRGNDATA data, LPDWORD size) override {
        if (!size) return DDERR_INVALIDPARAMS;
        RECT clip{};
        if (rect) clip = *rect;
        else if (window_) GetClientRect(window_, &clip);
        else clip = clipRect_;
        const DWORD required = sizeof(RGNDATAHEADER) + sizeof(RECT);
        if (!data || *size < required) { *size = required; return data ? DDERR_REGIONTOOSMALL : DD_OK; }
        data->rdh.dwSize = sizeof(RGNDATAHEADER); data->rdh.iType = RDH_RECTANGLES;
        data->rdh.nCount = 1; data->rdh.nRgnSize = sizeof(RECT); data->rdh.rcBound = clip;
        std::memcpy(data->Buffer, &clip, sizeof(clip)); *size = required; return DD_OK;
    }
    HRESULT STDMETHODCALLTYPE GetHWnd(HWND* window) override { if (!window) return DDERR_INVALIDPARAMS; *window = window_; return DD_OK; }
    HRESULT STDMETHODCALLTYPE Initialize(LPDIRECTDRAW, DWORD) override { return DDERR_ALREADYINITIALIZED; }
    HRESULT STDMETHODCALLTYPE IsClipListChanged(BOOL* changed) override { if (!changed) return DDERR_INVALIDPARAMS; *changed = changed_; changed_ = FALSE; return DD_OK; }
    HRESULT STDMETHODCALLTYPE SetClipList(LPRGNDATA data, DWORD) override {
        if (!data || !data->rdh.nCount) return DDERR_INVALIDCLIPLIST;
        clipRect_ = data->rdh.rcBound; window_ = nullptr; changed_ = TRUE; return DD_OK;
    }
    HRESULT STDMETHODCALLTYPE SetHWnd(DWORD, HWND window) override { window_ = window; changed_ = TRUE; return DD_OK; }
private:
    std::atomic<ULONG> references_{1};
    HWND window_ = nullptr;
    RECT clipRect_{};
    BOOL changed_ = FALSE;
};

HRESULT PresentPrimarySurface(NativeSurface* surface, UINT syncInterval = 1);

class NativeSurface final : public IDirectDrawSurface {
public:
    // Shares pixel data between GDI and D3D11.
    NativeSurface(DWORD width, DWORD height, DWORD caps, bool primary)
        : width_(width), height_(height), pitch_((width + 3) & ~3u), caps_(caps), primary_(primary) {
        BITMAPINFO256 info{};
        info.header.biSize = sizeof(BITMAPINFOHEADER);
        info.header.biWidth = static_cast<LONG>(width_);
        info.header.biHeight = -static_cast<LONG>(height_);
        info.header.biPlanes = 1; info.header.biBitCount = 8; info.header.biCompression = BI_RGB;
        memoryDc_ = CreateCompatibleDC(nullptr);
        bitmap_ = CreateDIBSection(memoryDc_, reinterpret_cast<BITMAPINFO*>(&info), DIB_RGB_COLORS,
                                   reinterpret_cast<void**>(&pixels_), nullptr, 0);
        if (bitmap_) previousBitmap_ = SelectObject(memoryDc_, bitmap_);
        g_nativeSurfaces.push_back(this);
    }
    bool Valid() const { return memoryDc_ && bitmap_ && pixels_; }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override {
        if (!object) return E_POINTER;
        *object = nullptr;
        if (iid != IID_IUnknown && iid != IID_IDirectDrawSurface) return E_NOINTERFACE;
        *object = static_cast<IDirectDrawSurface*>(this); AddRef(); return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return ++references_; }
    ULONG STDMETHODCALLTYPE Release() override { const ULONG n = --references_; if (!n) delete this; return n; }
    HRESULT STDMETHODCALLTYPE AddAttachedSurface(LPDIRECTDRAWSURFACE) override { return DDERR_UNSUPPORTED; }
    HRESULT STDMETHODCALLTYPE AddOverlayDirtyRect(LPRECT) override { return DDERR_UNSUPPORTED; }
    // Presents the primary surface after a blit.
    HRESULT STDMETHODCALLTYPE Blt(LPRECT destinationRect, LPDIRECTDRAWSURFACE sourceSurface, LPRECT sourceRect, DWORD flags, LPDDBLTFX effects) override {
        LogSampled("IDirectDrawSurface::Blt", bltCount_);
        RECT destination = destinationRect ? *destinationRect : RECT{0, 0, static_cast<LONG>(width_), static_cast<LONG>(height_)};
        HRESULT result = DD_OK;
        if (flags & DDBLT_COLORFILL) {
            if (!effects) return DDERR_INVALIDPARAMS;
            ClipRect(destination, width_, height_);
            for (LONG y = destination.top; y < destination.bottom; ++y)
                std::memset(pixels_ + y * pitch_ + destination.left, static_cast<BYTE>(effects->dwFillColor), destination.right - destination.left);
        } else {
            auto* source = dynamic_cast<NativeSurface*>(sourceSurface);
            if (!source) return DDERR_INVALIDOBJECT;
            RECT sourceArea = sourceRect ? *sourceRect : RECT{0, 0, static_cast<LONG>(source->width_), static_cast<LONG>(source->height_)};
            result = CopyRect(destination, *source, sourceArea, flags, effects);
        }
        if (primary_ && SUCCEEDED(result)) result = PresentPrimarySurface(this);
        return result;
    }
    HRESULT STDMETHODCALLTYPE BltBatch(LPDDBLTBATCH, DWORD, DWORD) override { return DDERR_UNSUPPORTED; }
    HRESULT STDMETHODCALLTYPE BltFast(DWORD x, DWORD y, LPDIRECTDRAWSURFACE sourceSurface, LPRECT sourceRect, DWORD flags) override {
        LogSampled("IDirectDrawSurface::BltFast", bltFastCount_);
        auto* source = dynamic_cast<NativeSurface*>(sourceSurface);
        if (!source) return DDERR_INVALIDOBJECT;
        RECT sourceArea = sourceRect ? *sourceRect : RECT{0, 0, static_cast<LONG>(source->width_), static_cast<LONG>(source->height_)};
        RECT destination{static_cast<LONG>(x), static_cast<LONG>(y), static_cast<LONG>(x) + sourceArea.right - sourceArea.left,
                         static_cast<LONG>(y) + sourceArea.bottom - sourceArea.top};
        const DWORD bltFlags = (flags & DDBLTFAST_SRCCOLORKEY) ? DDBLT_KEYSRC : 0;
        HRESULT result = CopyRect(destination, *source, sourceArea, bltFlags, nullptr);
        if (primary_ && SUCCEEDED(result)) result = PresentPrimarySurface(this);
        return result;
    }
    HRESULT STDMETHODCALLTYPE DeleteAttachedSurface(DWORD, LPDIRECTDRAWSURFACE) override { return DDERR_UNSUPPORTED; }
    HRESULT STDMETHODCALLTYPE EnumAttachedSurfaces(LPVOID, LPDDENUMSURFACESCALLBACK) override { return DD_OK; }
    HRESULT STDMETHODCALLTYPE EnumOverlayZOrders(DWORD, LPVOID, LPDDENUMSURFACESCALLBACK) override { return DD_OK; }
    HRESULT STDMETHODCALLTYPE Flip(LPDIRECTDRAWSURFACE, DWORD) override { return DDERR_NOTFLIPPABLE; }
    HRESULT STDMETHODCALLTYPE GetAttachedSurface(LPDDSCAPS, LPDIRECTDRAWSURFACE*) override { return DDERR_NOTFOUND; }
    HRESULT STDMETHODCALLTYPE GetBltStatus(DWORD) override { return DD_OK; }
    HRESULT STDMETHODCALLTYPE GetCaps(LPDDSCAPS caps) override { if (!caps) return DDERR_INVALIDPARAMS; caps->dwCaps = caps_; return DD_OK; }
    HRESULT STDMETHODCALLTYPE GetClipper(LPDIRECTDRAWCLIPPER* clipper) override { if (!clipper) return DDERR_INVALIDPARAMS; *clipper = clipper_; if (clipper_) clipper_->AddRef(); return clipper_ ? DD_OK : DDERR_NOCLIPPERATTACHED; }
    HRESULT STDMETHODCALLTYPE GetColorKey(DWORD flags, LPDDCOLORKEY key) override { if (!key || !(flags & DDCKEY_SRCBLT) || !hasSourceKey_) return DDERR_NOCOLORKEY; *key = sourceKey_; return DD_OK; }
    HRESULT STDMETHODCALLTYPE GetDC(HDC* dc) override {
        LogSampled("IDirectDrawSurface::GetDC", getDcCount_);
        if (!dc || dcAcquired_) return DDERR_DCALREADYCREATED;
        ApplyPalette(); dcAcquired_ = true; *dc = memoryDc_; return DD_OK;
    }
    HRESULT STDMETHODCALLTYPE GetFlipStatus(DWORD) override { return DDERR_NOTFLIPPABLE; }
    HRESULT STDMETHODCALLTYPE GetOverlayPosition(LPLONG, LPLONG) override { return DDERR_NOTAOVERLAYSURFACE; }
    HRESULT STDMETHODCALLTYPE GetPalette(LPDIRECTDRAWPALETTE* palette) override { if (!palette) return DDERR_INVALIDPARAMS; *palette = palette_; if (palette_) palette_->AddRef(); return palette_ ? DD_OK : DDERR_NOPALETTEATTACHED; }
    HRESULT STDMETHODCALLTYPE GetPixelFormat(LPDDPIXELFORMAT format) override { if (!format) return DDERR_INVALIDPARAMS; FillPixelFormat(*format); return DD_OK; }
    HRESULT STDMETHODCALLTYPE GetSurfaceDesc(LPDDSURFACEDESC desc) override { if (!desc || desc->dwSize != sizeof(*desc)) return DDERR_INVALIDPARAMS; FillDescription(*desc, pixels_); return DD_OK; }
    HRESULT STDMETHODCALLTYPE Initialize(LPDIRECTDRAW, LPDDSURFACEDESC) override { return DDERR_ALREADYINITIALIZED; }
    HRESULT STDMETHODCALLTYPE IsLost() override { return DD_OK; }
    HRESULT STDMETHODCALLTYPE Lock(LPRECT rect, LPDDSURFACEDESC desc, DWORD, HANDLE) override {
        LogSampled("IDirectDrawSurface::Lock", lockCount_);
        if (!desc || desc->dwSize != sizeof(*desc)) return DDERR_INVALIDPARAMS;
        if (locked_) return DDERR_SURFACEBUSY;
        RECT area = rect ? *rect : RECT{0, 0, static_cast<LONG>(width_), static_cast<LONG>(height_)};
        if (!ValidRect(area, width_, height_)) return DDERR_INVALIDRECT;
        FillDescription(*desc, pixels_ + area.top * pitch_ + area.left);
        desc->dwWidth = area.right - area.left; desc->dwHeight = area.bottom - area.top;
        locked_ = true; return DD_OK;
    }
    HRESULT STDMETHODCALLTYPE ReleaseDC(HDC dc) override { LogSampled("IDirectDrawSurface::ReleaseDC", releaseDcCount_); if (!dcAcquired_ || dc != memoryDc_) return DDERR_NODC; dcAcquired_ = false; return DD_OK; }
    HRESULT STDMETHODCALLTYPE Restore() override { return DD_OK; }
    HRESULT STDMETHODCALLTYPE SetClipper(LPDIRECTDRAWCLIPPER clipper) override { if (clipper) clipper->AddRef(); if (clipper_) clipper_->Release(); clipper_ = clipper; return DD_OK; }
    HRESULT STDMETHODCALLTYPE SetColorKey(DWORD flags, LPDDCOLORKEY key) override { if (!(flags & DDCKEY_SRCBLT)) return DDERR_UNSUPPORTED; if (key) { sourceKey_ = *key; hasSourceKey_ = true; } else hasSourceKey_ = false; return DD_OK; }
    HRESULT STDMETHODCALLTYPE SetOverlayPosition(LONG, LONG) override { return DDERR_NOTAOVERLAYSURFACE; }
    HRESULT STDMETHODCALLTYPE SetPalette(LPDIRECTDRAWPALETTE palette) override {
        if (palette) palette->AddRef();
        RestoreLogicalPalette();
        if (appliedPalette_) { appliedPalette_->Detach(this); appliedPalette_ = nullptr; }
        if (palette_) palette_->Release();
        palette_ = palette;
        if (primary_) {
            const HRESULT result = SetDisplayPalette(dynamic_cast<NativePalette*>(palette_));
            if (FAILED(result)) return result;
        }
        ApplyPalette();
        return DD_OK;
    }
    HRESULT STDMETHODCALLTYPE Unlock(LPVOID) override { LogSampled("IDirectDrawSurface::Unlock", unlockCount_); if (!locked_) return DDERR_NOTLOCKED; locked_ = false; return DD_OK; }
    HRESULT STDMETHODCALLTYPE UpdateOverlay(LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDOVERLAYFX) override { return DDERR_NOTAOVERLAYSURFACE; }
    HRESULT STDMETHODCALLTYPE UpdateOverlayDisplay(DWORD) override { return DDERR_NOTAOVERLAYSURFACE; }
    HRESULT STDMETHODCALLTYPE UpdateOverlayZOrder(DWORD, LPDIRECTDRAWSURFACE) override { return DDERR_NOTAOVERLAYSURFACE; }
    const BYTE* Pixels() const { return pixels_; }
    DWORD Width() const { return width_; }
    DWORD Height() const { return height_; }
    LONG Pitch() const { return static_cast<LONG>(pitch_); }
    NativePalette* Palette() const { return dynamic_cast<NativePalette*>(palette_); }
    // Presents palette changes without waiting for vsync.
    HRESULT RefreshPalette() { ApplyPalette(); return primary_ ? PresentPrimarySurface(this, 0) : DD_OK; }
private:
    struct BITMAPINFO256 { BITMAPINFOHEADER header; RGBQUAD colors[256]; };
    ~NativeSurface() {
        g_nativeSurfaces.erase(std::remove(g_nativeSurfaces.begin(), g_nativeSurfaces.end(), this), g_nativeSurfaces.end());
        if (appliedPalette_) appliedPalette_->Detach(this);
        RestoreLogicalPalette();
        if (palette_) palette_->Release(); if (clipper_) clipper_->Release();
        if (memoryDc_ && previousBitmap_) SelectObject(memoryDc_, previousBitmap_);
        if (bitmap_) DeleteObject(bitmap_); if (memoryDc_) DeleteDC(memoryDc_);
    }
    static bool ValidRect(const RECT& rect, DWORD width, DWORD height) { return rect.left >= 0 && rect.top >= 0 && rect.right >= rect.left && rect.bottom >= rect.top && rect.right <= static_cast<LONG>(width) && rect.bottom <= static_cast<LONG>(height); }
    static void ClipRect(RECT& rect, DWORD width, DWORD height) { rect.left = std::max<LONG>(0, rect.left); rect.top = std::max<LONG>(0, rect.top); rect.right = std::min<LONG>(width, rect.right); rect.bottom = std::min<LONG>(height, rect.bottom); }
    void FillPixelFormat(DDPIXELFORMAT& format) const { std::memset(&format, 0, sizeof(format)); format.dwSize = sizeof(format); format.dwFlags = DDPF_RGB | DDPF_PALETTEINDEXED8; format.dwRGBBitCount = 8; }
    void FillDescription(DDSURFACEDESC& desc, void* address) const { std::memset(&desc, 0, sizeof(desc)); desc.dwSize = sizeof(desc); desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT | DDSD_LPSURFACE; desc.dwWidth = width_; desc.dwHeight = height_; desc.lPitch = pitch_; desc.lpSurface = address; desc.ddsCaps.dwCaps = caps_; FillPixelFormat(desc.ddpfPixelFormat); }
    // Updates the DIB and GDI palettes together.
    void ApplyPalette() { auto* native = palette_ ? dynamic_cast<NativePalette*>(palette_) : g_displayPalette; if (!native || !memoryDc_) return; if (native != appliedPalette_) { RestoreLogicalPalette(); if (appliedPalette_) appliedPalette_->Detach(this); appliedPalette_ = native; appliedPalette_->Attach(this); } RGBQUAD colors[256]{}; const PALETTEENTRY* entries = native->Entries(); for (unsigned i = 0; i < 256; ++i) { colors[i].rgbRed = entries[i].peRed; colors[i].rgbGreen = entries[i].peGreen; colors[i].rgbBlue = entries[i].peBlue; } SetDIBColorTable(memoryDc_, 0, 256, colors); if (native->Handle()) { if (!logicalPaletteSelected_) { previousPalette_ = SelectPalette(memoryDc_, native->Handle(), FALSE); logicalPaletteSelected_ = true; } RealizePalette(memoryDc_); } }
    void RestoreLogicalPalette() { if (memoryDc_ && logicalPaletteSelected_) { SelectPalette(memoryDc_, previousPalette_, FALSE); RealizePalette(memoryDc_); previousPalette_ = nullptr; logicalPaletteSelected_ = false; } }
    HRESULT CopyRect(RECT destination, NativeSurface& source, RECT sourceArea, DWORD flags, LPDDBLTFX effects) {
        if (!ValidRect(sourceArea, source.width_, source.height_)) return DDERR_INVALIDRECT;
        const LONG requestedWidth = destination.right - destination.left, requestedHeight = destination.bottom - destination.top;
        if (requestedWidth != sourceArea.right - sourceArea.left || requestedHeight != sourceArea.bottom - sourceArea.top) return DDERR_NOSTRETCHHW;
        RECT clipped = destination; ClipRect(clipped, width_, height_);
        if (clipped.right <= clipped.left || clipped.bottom <= clipped.top) return DD_OK;
        sourceArea.left += clipped.left - destination.left; sourceArea.top += clipped.top - destination.top;
        const bool keyed = (flags & DDBLT_KEYSRC) != 0;
        DDCOLORKEY key = source.sourceKey_;
        if (flags & DDBLT_KEYSRCOVERRIDE) { if (!effects) return DDERR_INVALIDPARAMS; key = effects->ddckSrcColorkey; }
        else if (keyed && !source.hasSourceKey_) return DDERR_NOCOLORKEY;
        const LONG width = clipped.right - clipped.left, height = clipped.bottom - clipped.top;
        for (LONG y = 0; y < height; ++y) {
            const BYTE* src = source.pixels_ + (sourceArea.top + y) * source.pitch_ + sourceArea.left;
            BYTE* dst = pixels_ + (clipped.top + y) * pitch_ + clipped.left;
            if (!keyed && !(flags & DDBLT_KEYSRCOVERRIDE)) std::memmove(dst, src, width);
            else for (LONG x = 0; x < width; ++x) if (src[x] < key.dwColorSpaceLowValue || src[x] > key.dwColorSpaceHighValue) dst[x] = src[x];
        }
        return DD_OK;
    }
    std::atomic<ULONG> references_{1};
    DWORD width_, height_, pitch_, caps_;
    bool primary_, locked_ = false, dcAcquired_ = false, hasSourceKey_ = false, logicalPaletteSelected_ = false;
    BYTE* pixels_ = nullptr; HDC memoryDc_ = nullptr; HBITMAP bitmap_ = nullptr; HGDIOBJ previousBitmap_ = nullptr;
    HPALETTE previousPalette_ = nullptr;
    NativePalette* appliedPalette_ = nullptr;
    LPDIRECTDRAWPALETTE palette_ = nullptr; LPDIRECTDRAWCLIPPER clipper_ = nullptr; DDCOLORKEY sourceKey_{};
    std::atomic<unsigned long> bltCount_{0}, bltFastCount_{0}, lockCount_{0}, unlockCount_{0}, getDcCount_{0}, releaseDcCount_{0};
};

HRESULT NativePalette::NotifySurfaces() {
    HRESULT result = DD_OK;
    for (NativeSurface* surface : surfaces_) {
        const HRESULT refreshResult = surface->RefreshPalette();
        if (FAILED(refreshResult)) result = refreshResult;
    }
    return result;
}

// Keeps the display palette available to other surfaces.
HRESULT SetDisplayPalette(NativePalette* palette) {
    if (palette == g_displayPalette) return DD_OK;
    if (palette) palette->AddRef();
    NativePalette* previous = g_displayPalette;
    g_displayPalette = palette;
    ++g_paletteGeneration;
    HRESULT result = DD_OK;
    for (NativeSurface* surface : g_nativeSurfaces) {
        const HRESULT refreshResult = surface->RefreshPalette();
        if (FAILED(refreshResult)) result = refreshResult;
    }
    if (previous) previous->Release();
    Log("[GB D3D11] Native display palette propagated to unpalettized CPU surfaces.\r\n");
    return result;
}

HRESULT PresentPrimarySurface(NativeSurface* surface, UINT syncInterval) {
    NativePalette* palette = surface->Palette();
    if (!palette) return DDERR_NOPALETTEATTACHED;
    if (surface->Width() != g_displayWidth || surface->Height() != g_displayHeight) return DDERR_INVALIDPARAMS;
    return presentD3D11Frame(g_window, g_displayWidth, g_displayHeight,
                             surface->Pixels(), surface->Pitch(), palette->Entries(), syncInterval);
}

class NativeDirectDraw final : public IDirectDraw {
public:
    explicit NativeDirectDraw(LPDIRECTDRAW windowController) : windowController_(windowController) {}
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override { if (!object) return E_POINTER; *object = nullptr; if (iid != IID_IUnknown && iid != IID_IDirectDraw) return E_NOINTERFACE; *object = static_cast<IDirectDraw*>(this); AddRef(); return S_OK; }
    ULONG STDMETHODCALLTYPE AddRef() override { return ++references_; }
    ULONG STDMETHODCALLTYPE Release() override { const ULONG n = --references_; if (!n) delete this; return n; }
    HRESULT STDMETHODCALLTYPE Compact() override { return DD_OK; }
    HRESULT STDMETHODCALLTYPE CreateClipper(DWORD, LPDIRECTDRAWCLIPPER* output, IUnknown* outer) override { if (!output) return DDERR_INVALIDPARAMS; *output = nullptr; if (outer) return CLASS_E_NOAGGREGATION; auto* clipper = new (std::nothrow) NativeClipper(); if (!clipper) return E_OUTOFMEMORY; *output = clipper; return DD_OK; }
    HRESULT STDMETHODCALLTYPE CreatePalette(DWORD caps, LPPALETTEENTRY entries, LPDIRECTDRAWPALETTE* output, IUnknown* outer) override { if (!output || !entries) return DDERR_INVALIDPARAMS; *output = nullptr; if (outer) return CLASS_E_NOAGGREGATION; auto* palette = new (std::nothrow) NativePalette(caps, entries); if (!palette) return E_OUTOFMEMORY; *output = palette; return DD_OK; }
    HRESULT STDMETHODCALLTYPE CreateSurface(LPDDSURFACEDESC desc, LPDIRECTDRAWSURFACE* output, IUnknown* outer) override {
        if (!desc || desc->dwSize != sizeof(*desc) || !output) return DDERR_INVALIDPARAMS; *output = nullptr; if (outer) return CLASS_E_NOAGGREGATION;
        const bool primary = (desc->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE) != 0;
        const DWORD width = primary && !(desc->dwFlags & DDSD_WIDTH) ? g_displayWidth : desc->dwWidth;
        const DWORD height = primary && !(desc->dwFlags & DDSD_HEIGHT) ? g_displayHeight : desc->dwHeight;
        if (!width || !height) return DDERR_INVALIDPARAMS;
        auto* surface = new (std::nothrow) NativeSurface(width, height, desc->ddsCaps.dwCaps, primary);
        if (!surface) return E_OUTOFMEMORY; if (!surface->Valid()) { surface->Release(); return DDERR_OUTOFMEMORY; }
        *output = surface; return DD_OK;
    }
    HRESULT STDMETHODCALLTYPE DuplicateSurface(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE*) override { return DDERR_UNSUPPORTED; }
    HRESULT STDMETHODCALLTYPE EnumDisplayModes(DWORD, LPDDSURFACEDESC filter, LPVOID context, LPDDENUMMODESCALLBACK callback) override { if (!callback) return DDERR_INVALIDPARAMS; const DWORD modes[][2] = {{640,480},{800,600},{1024,768},{1280,720},{1280,1024},{1920,1080}}; for (const auto& mode : modes) { if (filter && (filter->dwFlags & DDSD_WIDTH) && filter->dwWidth != mode[0]) continue; if (filter && (filter->dwFlags & DDSD_HEIGHT) && filter->dwHeight != mode[1]) continue; DDSURFACEDESC desc{}; desc.dwSize = sizeof(desc); desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT; desc.dwWidth = mode[0]; desc.dwHeight = mode[1]; desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT); desc.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_PALETTEINDEXED8; desc.ddpfPixelFormat.dwRGBBitCount = 8; if (callback(&desc, context) == DDENUMRET_CANCEL) break; } return DD_OK; }
    HRESULT STDMETHODCALLTYPE EnumSurfaces(DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMSURFACESCALLBACK) override { return DDERR_UNSUPPORTED; }
    HRESULT STDMETHODCALLTYPE FlipToGDISurface() override { return DDERR_UNSUPPORTED; }
    HRESULT STDMETHODCALLTYPE GetCaps(LPDDCAPS driver, LPDDCAPS hel) override { if (driver) { const DWORD size = driver->dwSize; std::memset(driver, 0, size); driver->dwSize = size; driver->dwCaps = DDCAPS_BLT | DDCAPS_BLTCOLORFILL | DDCAPS_COLORKEY; driver->dwCKeyCaps = DDCKEYCAPS_SRCBLT; driver->dwVidMemTotal = driver->dwVidMemFree = 64 * 1024 * 1024; } if (hel) { const DWORD size = hel->dwSize; std::memset(hel, 0, size); hel->dwSize = size; } return DD_OK; }
    HRESULT STDMETHODCALLTYPE GetDisplayMode(LPDDSURFACEDESC desc) override { if (!desc || desc->dwSize != sizeof(*desc)) return DDERR_INVALIDPARAMS; std::memset(desc, 0, sizeof(*desc)); desc->dwSize = sizeof(*desc); desc->dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT; desc->dwWidth = g_displayWidth; desc->dwHeight = g_displayHeight; desc->ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT); desc->ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_PALETTEINDEXED8; desc->ddpfPixelFormat.dwRGBBitCount = 8; return DD_OK; }
    HRESULT STDMETHODCALLTYPE GetFourCCCodes(LPDWORD count, LPDWORD) override { if (!count) return DDERR_INVALIDPARAMS; *count = 0; return DD_OK; }
    // Does not keep a pointer to the primary surface.
    HRESULT STDMETHODCALLTYPE GetGDISurface(LPDIRECTDRAWSURFACE* output) override { if (!output) return DDERR_INVALIDPARAMS; *output = nullptr; return DDERR_NOTFOUND; }
    HRESULT STDMETHODCALLTYPE GetMonitorFrequency(LPDWORD frequency) override { if (!frequency) return DDERR_INVALIDPARAMS; *frequency = 60; return DD_OK; }
    HRESULT STDMETHODCALLTYPE GetScanLine(LPDWORD) override { return DDERR_UNSUPPORTED; }
    HRESULT STDMETHODCALLTYPE GetVerticalBlankStatus(LPBOOL status) override { if (!status) return DDERR_INVALIDPARAMS; *status = FALSE; return DD_OK; }
    HRESULT STDMETHODCALLTYPE Initialize(GUID*) override { return DDERR_ALREADYINITIALIZED; }
    HRESULT STDMETHODCALLTYPE RestoreDisplayMode() override { return windowController_->RestoreDisplayMode(); }
    // EF window mode ignores fullscreen requests.
    HRESULT STDMETHODCALLTYPE SetCooperativeLevel(HWND window, DWORD flags) override { char message[128]{}; std::snprintf(message, sizeof(message), "[GB D3D11] IDirectDraw::SetCooperativeLevel flags=0x%08lX.\r\n", flags); Log(message); fullscreen_ = !cd.windowMode && (flags & DDSCL_FULLSCREEN) != 0; const HRESULT result = windowController_->SetCooperativeLevel(window, fullscreen_ ? flags : DDSCL_NORMAL); if (FAILED(result)) { LogResult("System IDirectDraw::SetCooperativeLevel", result); return result; } g_window = window; resetD3D11Renderer(); return DD_OK; }
    // The fullscreen menu renders at 800x600 in the selected display mode.
    HRESULT STDMETHODCALLTYPE SetDisplayMode(DWORD width, DWORD height, DWORD bpp) override { char message[128]{}; std::snprintf(message, sizeof(message), "[GB D3D11] IDirectDraw::SetDisplayMode %lux%lu %lu-bpp.\r\n", width, height, bpp); Log(message); DWORD physicalWidth = width; DWORD physicalHeight = height; int configuredWidth = 0; int configuredHeight = 0; const bool fullscreenMenu = fullscreen_ && width == 800 && height == 600; if (fullscreenMenu) { getConfiguredResolution(&configuredWidth, &configuredHeight); physicalWidth = static_cast<DWORD>(configuredWidth); physicalHeight = static_cast<DWORD>(configuredHeight); } if (fullscreen_) { const HRESULT result = windowController_->SetDisplayMode(physicalWidth, physicalHeight, 32); if (FAILED(result)) { LogResult("System IDirectDraw::SetDisplayMode", result); return result; } } else if (!configureD3D11WindowedMode(g_window, width, height)) { Log("[GB D3D11] Failed to configure decorated windowed mode.\r\n"); return DDERR_GENERIC; } g_displayWidth = width; g_displayHeight = height; if (!setD3D11WindowMouseMapping(g_window, width, height, fullscreenMenu)) { Log("[GB D3D11] Failed to install fullscreen menu window procedure.\r\n"); return DDERR_GENERIC; } resetD3D11Renderer(); return DD_OK; }
    HRESULT STDMETHODCALLTYPE WaitForVerticalBlank(DWORD, HANDLE) override { return DDERR_UNSUPPORTED; }
private:
    ~NativeDirectDraw() { windowController_->Release(); }
    std::atomic<ULONG> references_{1}; LPDIRECTDRAW windowController_ = nullptr; bool fullscreen_ = false;
};

bool ReplaceDirectDrawImport() {
    // Replaces the x2 DirectDrawCreate import.
    auto* entry = reinterpret_cast<DWORD*>(0x00654014);
    g_directDrawCreate = reinterpret_cast<DirectDrawCreateProc>(*entry);
    DWORD protection;
    if (!VirtualProtect(entry, sizeof(*entry), PAGE_READWRITE, &protection)) return false;
    *entry = reinterpret_cast<DWORD>(&GbDirectDrawCreate);
    DWORD ignored = 0;
    if (!VirtualProtect(entry, sizeof(*entry), protection, &ignored)) {
        *entry = reinterpret_cast<DWORD>(g_directDrawCreate);
        VirtualProtect(entry, sizeof(*entry), protection, &ignored);
        return false;
    }
    return true;
}

// Creates the renderer and keeps system window handling.
HRESULT WINAPI GbDirectDrawCreate(GUID* guid, LPDIRECTDRAW* directDraw, IUnknown* outer) {
    Log("[GB D3D11] DirectDrawCreate intercepted; system DirectDraw retained for exact window behavior.\r\n");
    if (!directDraw) return DDERR_INVALIDPARAMS; *directDraw = nullptr;
    if (outer) return CLASS_E_NOAGGREGATION;
    if (guid) return DDERR_INVALIDDIRECTDRAWGUID;
    LPDIRECTDRAW windowController = nullptr;
    const HRESULT windowResult = g_directDrawCreate(guid, &windowController, outer);
    if (FAILED(windowResult)) return windowResult;
    auto* native = new (std::nothrow) NativeDirectDraw(windowController); if (!native) { windowController->Release(); return E_OUTOFMEMORY; }
    *directDraw = native; Log("[GB D3D11] Native DirectDraw v1 object created; rendering remains native D3D11.\r\n"); return DD_OK;
}

}

extern "C" __declspec(dllexport) BOOL __cdecl GbGetDisplayPalette(
    std::uint32_t* colors, unsigned long* generation) {
    if (!colors || !generation || !g_displayPalette) return FALSE;
    const PALETTEENTRY* entries = g_displayPalette->Entries();
    for (unsigned i = 0; i < 256; ++i) {
        colors[i] = static_cast<std::uint32_t>(entries[i].peRed) |
                    (static_cast<std::uint32_t>(entries[i].peGreen) << 8) |
                    (static_cast<std::uint32_t>(entries[i].peBlue) << 16);
    }
    *generation = g_paletteGeneration.load();
    return TRUE;
}

namespace {
bool g_rendererInitialized = false;
}

bool initializeD3D11Renderer(HMODULE module) {
    if (g_rendererInitialized) return true;
    SetLogPath(module);
    setD3D11LogCallback(&Log);
    Log("[GB D3D11] Renderer loading from efpatch; GPU palette shader; system DirectDraw window control.\r\n");
    if (!ReplaceDirectDrawImport()) {
        Log("[GB D3D11] Failed to install DirectDrawCreate IAT replacement.\r\n");
        return false;
    }
    Log("[GB D3D11] DirectDrawCreate IAT replacement installed.\r\n");
    g_rendererInitialized = true;
    return true;
}
