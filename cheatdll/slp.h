#pragma once

#include "palfile.h"

enum types
{
    T_PIXEL,
    T_PLAYER_COLOR,
    T_TRANSPARENT,
    T_SHADOW,
    T_OUTLINE_SHIELD,
    T_OUTLINE_COLOR
};

//SLP structures

struct SLP_file_frame
{
    uint32_t   cmd_table_offset;
    uint32_t   outline_table_offset;
    uint32_t   palette_offset;
    uint32_t   properties;
    int32_t    width;
    int32_t    height;
    int32_t    hotspot_x;
    int32_t    hotspot_y;
};

struct SLP_meta
{
    char     version[4];
    int32_t  num_frames;
    char     comment[24];
};

struct SLP_pixel
{
    uint8_t type;
    uint8_t data;
    bool operator==(const SLP_pixel& p)
    {
        return ((p.type == type) && (p.data == data));
    }
};

struct SLP_outline
{
    uint16_t left;
    uint16_t right;
};

struct SLP_scanline
{
    SLP_pixel* pixels;
};

struct SLP_frame
{
    SLP_scanline* scanlines;
    uint32_t properties;
    int32_t  width;
    int32_t  height;
    int32_t  hotspot_x;
    int32_t  hotspot_y;
};

class SLP
{
private:
    SLP_meta meta;
    SLP_frame* frames;

    bool loaded;
public:
    SLP();
    ~SLP();
    bool load(uint8_t* data);
    void stretch(int x, int y, int id);
    void stretch_techtree(int x, int y);
    void resize(int x, int y, T_PALETTE& pal);
    void color_replace(const uint8_t* c, int count);
    uint8_t* write(int* size, bool allow_fill);
};
