#pragma once

namespace d3d11_palette_shader {

// Draws one triangle across the screen.
static const char vertexSource[] =
    "struct Output { float4 position : SV_Position; float2 uv : TEXCOORD0; };"
    "Output main(uint id : SV_VertexID) {"
    "float2 positions[3] = { float2(-1,-1), float2(-1,3), float2(3,-1) };"
    "float2 uvs[3] = { float2(0,1), float2(0,-1), float2(2,1) };"
    "Output output; output.position = float4(positions[id], 0, 1); output.uv = uvs[id]; return output; }";

// Looks up each 8-bit pixel in the palette.
static const char pixelSource[] =
    "Texture2D<uint> indices : register(t0);"
    "Texture2D<float4> palette : register(t1);"
    "float4 main(float4 position : SV_Position, float2 uv : TEXCOORD0) : SV_Target {"
    "uint width, height; indices.GetDimensions(width, height);"
    "int2 pixel = min(int2(uv * float2(width, height)), int2(width - 1, height - 1));"
    "uint index = indices.Load(int3(pixel, 0));"
    "return palette.Load(int3(index, 0, 0)); }";

}
