#define WINDOWS_IGNORE_PACKING_MISMATCH
#include "d3d11_window.h"
#include <windowsx.h>

namespace {
HWND g_window = nullptr;
WNDPROC g_windowProc = nullptr;
DWORD g_logicalWidth = 0;
DWORD g_logicalHeight = 0;
bool g_mapMouse = false;
constexpr UINT kApplyFullscreenMenu = WM_APP + 0x47;

// Sizes the fullscreen window to the nearest monitor.
bool applyFullscreenMenu(HWND window) {
    const HMONITOR monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info{sizeof(info)};
    if (!GetMonitorInfoA(monitor, &info)) return false;
    const RECT& bounds = info.rcMonitor;
    return SetWindowPos(window, nullptr, bounds.left, bounds.top,
                        bounds.right - bounds.left, bounds.bottom - bounds.top,
                        SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_SHOWWINDOW) != FALSE;
}

bool isClientMouseMessage(UINT message) {
    switch (message) {
        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
        case WM_XBUTTONDBLCLK:
            return true;
        default:
            return false;
    }
}

// Applies fullscreen sizing and maps mouse coordinates.
LRESULT CALLBACK d3D11WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == kApplyFullscreenMenu) {
        const bool applied = g_mapMouse && window == g_window && applyFullscreenMenu(window);
        if (!applied) OutputDebugStringA("[GB D3D11] Failed to apply deferred fullscreen menu geometry.\r\n");
        return applied;
    }
    if (g_mapMouse && window == g_window && isClientMouseMessage(message)) {
        RECT client{};
        if (GetClientRect(window, &client) && client.right > 0 && client.bottom > 0) {
            LONG viewportWidth = client.right;
            LONG viewportHeight = MulDiv(viewportWidth, static_cast<int>(g_logicalHeight), static_cast<int>(g_logicalWidth));
            if (viewportHeight > client.bottom) {
                viewportHeight = client.bottom;
                viewportWidth = MulDiv(viewportHeight, static_cast<int>(g_logicalWidth), static_cast<int>(g_logicalHeight));
            }
            const LONG left = (client.right - viewportWidth) / 2;
            const LONG top = (client.bottom - viewportHeight) / 2;
            LONG x = MulDiv(GET_X_LPARAM(lParam) - left, static_cast<int>(g_logicalWidth), viewportWidth);
            LONG y = MulDiv(GET_Y_LPARAM(lParam) - top, static_cast<int>(g_logicalHeight), viewportHeight);
            x = x < 0 ? 0 : (x >= static_cast<LONG>(g_logicalWidth) ? static_cast<LONG>(g_logicalWidth) - 1 : x);
            y = y < 0 ? 0 : (y >= static_cast<LONG>(g_logicalHeight) ? static_cast<LONG>(g_logicalHeight) - 1 : y);
            lParam = MAKELPARAM(static_cast<short>(x), static_cast<short>(y));
        }
    }
    return CallWindowProcA(g_windowProc, window, message, wParam, lParam);
}
}

// Creates a centered bordered window at the requested size.
bool configureD3D11WindowedMode(HWND window, DWORD width, DWORD height) {
    constexpr DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
    constexpr DWORD extendedStyle = WS_EX_APPWINDOW;
    SetLastError(ERROR_SUCCESS);
    if (!SetWindowLongPtrA(window, GWL_STYLE, style) && GetLastError() != ERROR_SUCCESS) return false;
    SetLastError(ERROR_SUCCESS);
    if (!SetWindowLongPtrA(window, GWL_EXSTYLE, extendedStyle) && GetLastError() != ERROR_SUCCESS) return false;

    RECT frame{0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
    if (!AdjustWindowRectEx(&frame, style, FALSE, extendedStyle)) return false;
    const LONG frameWidth = frame.right - frame.left;
    const LONG frameHeight = frame.bottom - frame.top;
    MONITORINFO info{sizeof(info)};
    if (!GetMonitorInfoA(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &info)) return false;
    const LONG x = info.rcWork.left + (info.rcWork.right - info.rcWork.left - frameWidth) / 2;
    const LONG y = info.rcWork.top + (info.rcWork.bottom - info.rcWork.top - frameHeight) / 2;
    return SetWindowPos(window, nullptr, x, y, frameWidth, frameHeight,
                        SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_SHOWWINDOW) != FALSE;
}

bool setD3D11WindowMouseMapping(HWND window, DWORD width, DWORD height, bool enabled) {
    if (window != g_window) {
        if (!shutdownD3D11Window()) return false;
        if (enabled) {
            g_windowProc = reinterpret_cast<WNDPROC>(
                SetWindowLongPtrA(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&d3D11WindowProc)));
            if (!g_windowProc) return false;
            g_window = window;
        }
    }
    g_logicalWidth = width;
    g_logicalHeight = height;
    g_mapMouse = enabled;
    if (enabled && g_window && !PostMessageA(g_window, kApplyFullscreenMenu, 0, 0)) return false;
    return true;
}

// Restores the original window procedure.
bool shutdownD3D11Window() {
    if (g_window && g_windowProc && IsWindow(g_window)) {
        SetLastError(ERROR_SUCCESS);
        if (!SetWindowLongPtrA(g_window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(g_windowProc)) &&
            GetLastError() != ERROR_SUCCESS) return false;
    }
    g_window = nullptr;
    g_windowProc = nullptr;
    g_logicalWidth = 0;
    g_logicalHeight = 0;
    g_mapMouse = false;
    return true;
}
