#define NOMINMAX
#define WINDOWS_IGNORE_PACKING_MISMATCH
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdio>

#include "d3d11_renderer.h"
#include "d3d11_palette_shader.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

namespace {

// The D3D11 device is shared by every display mode.
ID3D11Device* g_device = nullptr;
ID3D11DeviceContext* g_context = nullptr;
IDXGISwapChain* g_swapChain = nullptr;
ID3D11Texture2D* g_backBuffer = nullptr;
ID3D11RenderTargetView* g_renderTarget = nullptr;
ID3D11Texture2D* g_indexTexture = nullptr;
ID3D11ShaderResourceView* g_indexView = nullptr;
ID3D11Texture2D* g_paletteTexture = nullptr;
ID3D11ShaderResourceView* g_paletteView = nullptr;
ID3D11VertexShader* g_vertexShader = nullptr;
ID3D11PixelShader* g_pixelShader = nullptr;
HWND g_swapChainWindow = nullptr;
DWORD g_frameWidth = 0;
DWORD g_frameHeight = 0;
DWORD g_outputWidth = 0;
DWORD g_outputHeight = 0;
std::atomic<unsigned long> g_presentCount{0};
D3D11LogCallback g_logCallback = nullptr;

void logMessage(const char* message) {
    if (g_logCallback) g_logCallback(message);
    else OutputDebugStringA(message);
}

void logResult(const char* method, HRESULT result) {
    char message[160]{};
    std::snprintf(message, sizeof(message), "[GB D3D11] %s -> 0x%08lX.\r\n",
                  method, static_cast<unsigned long>(result));
    logMessage(message);
}

// Limits how often frame presentation is logged.
void logPresent() {
    const unsigned long count = ++g_presentCount;
    if (count <= 16 || (count & (count - 1)) == 0) {
        char message[128]{};
        std::snprintf(message, sizeof(message), "[GB D3D11] DXGI Present call #%lu.\r\n", count);
        logMessage(message);
    }
}

// Creates the shaders and textures.
HRESULT createPalettePipeline() {
    ID3DBlob* vertexBytecode = nullptr;
    ID3DBlob* pixelBytecode = nullptr;
    HRESULT result = D3DCompile(d3d11_palette_shader::vertexSource, sizeof(d3d11_palette_shader::vertexSource) - 1,
                                nullptr, nullptr, nullptr, "main", "vs_4_0", 0, 0, &vertexBytecode, nullptr);
    if (SUCCEEDED(result))
        result = D3DCompile(d3d11_palette_shader::pixelSource, sizeof(d3d11_palette_shader::pixelSource) - 1,
                            nullptr, nullptr, nullptr, "main", "ps_4_0", 0, 0, &pixelBytecode, nullptr);
    if (SUCCEEDED(result))
        result = g_device->CreateVertexShader(vertexBytecode->GetBufferPointer(), vertexBytecode->GetBufferSize(),
                                               nullptr, &g_vertexShader);
    if (SUCCEEDED(result))
        result = g_device->CreatePixelShader(pixelBytecode->GetBufferPointer(), pixelBytecode->GetBufferSize(),
                                              nullptr, &g_pixelShader);
    if (pixelBytecode) pixelBytecode->Release();
    if (vertexBytecode) vertexBytecode->Release();

    D3D11_TEXTURE2D_DESC texture{};
    texture.Width = g_frameWidth;
    texture.Height = g_frameHeight;
    texture.MipLevels = 1;
    texture.ArraySize = 1;
    texture.Format = DXGI_FORMAT_R8_UINT;
    texture.SampleDesc.Count = 1;
    texture.Usage = D3D11_USAGE_DEFAULT;
    texture.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    if (SUCCEEDED(result)) result = g_device->CreateTexture2D(&texture, nullptr, &g_indexTexture);
    if (SUCCEEDED(result)) result = g_device->CreateShaderResourceView(g_indexTexture, nullptr, &g_indexView);

    texture.Width = 256;
    texture.Height = 1;
    texture.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    if (SUCCEEDED(result)) result = g_device->CreateTexture2D(&texture, nullptr, &g_paletteTexture);
    if (SUCCEEDED(result)) result = g_device->CreateShaderResourceView(g_paletteTexture, nullptr, &g_paletteView);
    if (SUCCEEDED(result)) result = g_device->CreateRenderTargetView(g_backBuffer, nullptr, &g_renderTarget);
    return result;
}

// Recreates the swap chain when the window size changes.
HRESULT ensureSwapChain(HWND window, DWORD width, DWORD height) {
    if (!g_device || !window || !width || !height) return DDERR_NOCOOPERATIVELEVELSET;
    RECT client{};
    if (!GetClientRect(window, &client)) return DDERR_INVALIDOBJECT;
    if (client.right <= 0 || client.bottom <= 0) return DDERR_INVALIDMODE;
    const DWORD outputWidth = static_cast<DWORD>(client.right);
    const DWORD outputHeight = static_cast<DWORD>(client.bottom);
    if (g_swapChain && window == g_swapChainWindow && width == g_frameWidth && height == g_frameHeight &&
        outputWidth == g_outputWidth && outputHeight == g_outputHeight) return S_OK;
    resetD3D11Renderer();
    g_swapChainWindow = window;
    g_frameWidth = width;
    g_frameHeight = height;
    g_outputWidth = outputWidth;
    g_outputHeight = outputHeight;

    IDXGIDevice* dxgiDevice = nullptr;
    IDXGIAdapter* adapter = nullptr;
    IDXGIFactory* factory = nullptr;
    HRESULT result = g_device->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
    if (SUCCEEDED(result)) result = dxgiDevice->GetAdapter(&adapter);
    if (SUCCEEDED(result)) result = adapter->GetParent(IID_PPV_ARGS(&factory));

    DXGI_SWAP_CHAIN_DESC description{};
    description.BufferDesc.Width = outputWidth;
    description.BufferDesc.Height = outputHeight;
    description.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    description.SampleDesc.Count = 1;
    description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    description.BufferCount = 2;
    description.OutputWindow = window;
    description.Windowed = TRUE;
    description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    if (SUCCEEDED(result)) result = factory->CreateSwapChain(g_device, &description, &g_swapChain);
    if (SUCCEEDED(result)) result = g_swapChain->GetBuffer(0, IID_PPV_ARGS(&g_backBuffer));
    if (SUCCEEDED(result)) result = createPalettePipeline();

    if (factory) factory->Release();
    if (adapter) adapter->Release();
    if (dxgiDevice) dxgiDevice->Release();
    if (FAILED(result)) {
        resetD3D11Renderer();
        logResult("DXGI swap-chain creation", result);
    } else {
        logMessage("[GB D3D11] DXGI swap chain and palette shader created; native presentation active.\r\n");
    }
    return result;
}

}

void setD3D11LogCallback(D3D11LogCallback callback) {
    g_logCallback = callback;
}

static HRESULT ensureD3D11Device() {
    if (g_device) return S_OK;
    D3D_FEATURE_LEVEL level{};
    const D3D_FEATURE_LEVEL requested[] = {
        D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0
    };
    const HRESULT result = D3D11CreateDevice(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        requested, ARRAYSIZE(requested), D3D11_SDK_VERSION, &g_device, &level, &g_context);
    if (SUCCEEDED(result)) logMessage("[GB D3D11] D3D11 device created.\r\n");
    return result;
}

void resetD3D11Renderer() {
    if (g_pixelShader) { g_pixelShader->Release(); g_pixelShader = nullptr; }
    if (g_vertexShader) { g_vertexShader->Release(); g_vertexShader = nullptr; }
    if (g_paletteView) { g_paletteView->Release(); g_paletteView = nullptr; }
    if (g_paletteTexture) { g_paletteTexture->Release(); g_paletteTexture = nullptr; }
    if (g_indexView) { g_indexView->Release(); g_indexView = nullptr; }
    if (g_indexTexture) { g_indexTexture->Release(); g_indexTexture = nullptr; }
    if (g_renderTarget) { g_renderTarget->Release(); g_renderTarget = nullptr; }
    if (g_backBuffer) { g_backBuffer->Release(); g_backBuffer = nullptr; }
    if (g_swapChain) { g_swapChain->Release(); g_swapChain = nullptr; }
    g_swapChainWindow = nullptr;
    g_frameWidth = 0;
    g_frameHeight = 0;
    g_outputWidth = 0;
    g_outputHeight = 0;
}

// Uploads and presents one frame.
HRESULT presentD3D11Frame(HWND window, DWORD width, DWORD height,
                          const void* pixels, UINT pitch,
                          const PALETTEENTRY* palette, UINT syncInterval) {
    HRESULT result = ensureD3D11Device();
    if (FAILED(result)) return result;
    result = ensureSwapChain(window, width, height);
    if (FAILED(result)) return result;

    std::uint32_t colors[256]{};
    for (unsigned i = 0; i < 256; ++i) {
        colors[i] = static_cast<std::uint32_t>(palette[i].peRed) |
                    (static_cast<std::uint32_t>(palette[i].peGreen) << 8) |
                    (static_cast<std::uint32_t>(palette[i].peBlue) << 16) | 0xFF000000u;
    }
    g_context->UpdateSubresource(g_indexTexture, 0, nullptr, pixels, pitch, 0);
    g_context->UpdateSubresource(g_paletteTexture, 0, nullptr, colors, sizeof(colors), 0);

    const float scale = std::min(static_cast<float>(g_outputWidth) / width,
                                 static_cast<float>(g_outputHeight) / height);
    const float viewportWidth = width * scale;
    const float viewportHeight = height * scale;
    D3D11_VIEWPORT viewport{(g_outputWidth - viewportWidth) * 0.5f,
                            (g_outputHeight - viewportHeight) * 0.5f,
                            viewportWidth, viewportHeight, 0.0f, 1.0f};
    ID3D11ShaderResourceView* resources[] = {g_indexView, g_paletteView};
    const float black[4]{};
    g_context->OMSetRenderTargets(1, &g_renderTarget, nullptr);
    g_context->ClearRenderTargetView(g_renderTarget, black);
    g_context->RSSetViewports(1, &viewport);
    g_context->IASetInputLayout(nullptr);
    g_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g_context->VSSetShader(g_vertexShader, nullptr, 0);
    g_context->PSSetShader(g_pixelShader, nullptr, 0);
    g_context->PSSetShaderResources(0, ARRAYSIZE(resources), resources);
    g_context->Draw(3, 0);
    result = g_swapChain->Present(syncInterval, 0);
    if (SUCCEEDED(result)) logPresent(); else logResult("DXGI Present", result);
    return result;
}
