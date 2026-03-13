#include "stdafx.h"
#include "palfile.h"

T_PALETTE::T_PALETTE(const void* mem, size_t size, LPPALETTEENTRY lpe)
{
    int n;

    if (size == 0)
        return;

    char* str = (char*)malloc(size + 1);
    memcpy(str, mem, size);
    str[size] = '\0';

    char* token;
    char* temp;
    token = strtok_s(str, "\r\n", &temp);
    if (!token)
    {
        free(str);
        return;
    }
    if (strncmp(token, "JASC-PAL", 9))
    {
        free(str);
        return;
    }
    if (!strtok_s(NULL, "\r\n", &temp))
    {
        free(str);
        return;
    }
    token = strtok_s(NULL, "\r\n", &temp);
    if (token)
    {
        sscanf_s(token, "%d", &n);
        if (n != 256)
        {
            free(str);
            return;
        }
    }
    else
    {
        free(str);
        return;
    }

    LOGPALETTE* lp = (LOGPALETTE*)malloc(sizeof(PALETTEENTRY) * 256 + sizeof(LOGPALETTE));
    lp->palVersion = 0x300;
    lp->palNumEntries = 256;
    for (int i = 0; i < 256; i++)
    {
        int r, g, b;
        token = strtok_s(NULL, "\r\n", &temp);
        if (!token)
        {
            free(str);
            free(lp);
            return;
        }
        sscanf_s(token, "%d %d %d", &r, &g, &b);
        lp->palPalEntry[i].peRed = r;
        lp->palPalEntry[i].peGreen = g;
        lp->palPalEntry[i].peBlue = b;
        lp->palPalEntry[i].peFlags = 0;
        //entries[i] = RGB(r, g, b);
        palette_entries[i].peRed = r;
        palette_entries[i].peGreen = g;
        palette_entries[i].peBlue = b;
        palette_entries[i].peFlags = 0;

        lpe[i] = palette_entries[i];
    }
    pal = CreatePalette(lp);

    free(lp);
    free(str);
}

T_PALETTE::T_PALETTE(const LPPALETTEENTRY lpe)
{
    LOGPALETTE* lp = (LOGPALETTE*)malloc(sizeof(PALETTEENTRY) * 256 + sizeof(LOGPALETTE));
    lp->palVersion = 0x300;
    lp->palNumEntries = 256;
    for (int i = 0; i < 256; i++)
    {
        lp->palPalEntry[i] = lpe[i];
        palette_entries[i] = lpe[i];
    }
    pal = CreatePalette(lp);

    free(lp);
}

int T_PALETTE::get_index(COLORREF c) const
{
    return GetNearestPaletteIndex(pal, c);
}

COLORREF T_PALETTE::get_entry(int index) const
{
    const PALETTEENTRY& e = palette_entries[index];
    return RGB(e.peRed, e.peGreen, e.peBlue);
}

HPALETTE T_PALETTE::get_palette() const
{
    return pal;
}

T_PALETTE::~T_PALETTE()
{
    if (pal != NULL)
        DeleteObject(pal);
}

void COLOR_TABLE::init(const T_PALETTE& pal)
{
    for (int r = 0; r < COLOR_TABLE_ENTRY_SIZE; r++)
        for (int g = 0; g < COLOR_TABLE_ENTRY_SIZE; g++)
            for (int b = 0; b < COLOR_TABLE_ENTRY_SIZE; b++)
            {
                int rx = (r << COLOR_TABLE_SHIFT) + CLR_DELTA;
                int gx = (g << COLOR_TABLE_SHIFT) + CLR_DELTA;
                int bx = (b << COLOR_TABLE_SHIFT) + CLR_DELTA;
                color_table[r][g][b] = pal.get_index(RGB(rx, gx, bx));
            }
}

COLOR_TABLE::COLOR_TABLE(const void* mem, size_t size)
{
    T_PALETTE pal(mem, size, palette_entries);
    init(pal);
}

COLOR_TABLE::COLOR_TABLE(const LPPALETTEENTRY entries)
{
    T_PALETTE pal(entries);
    memcpy(palette_entries, entries, sizeof(palette_entries));
    init(pal);
}

unsigned char COLOR_TABLE::get_index(COLORREF c)
{
    return color_table[GetRValue(c) >> COLOR_TABLE_SHIFT][GetGValue(c) >> COLOR_TABLE_SHIFT][GetBValue(c) >> COLOR_TABLE_SHIFT];
}

unsigned char COLOR_TABLE::get_index_rgb(DWORD c)
{
    return color_table[GetBValue(c) >> COLOR_TABLE_SHIFT][GetGValue(c) >> COLOR_TABLE_SHIFT][GetRValue(c) >> COLOR_TABLE_SHIFT];
}

DWORD COLOR_TABLE::get_4index(DWORD* pixels)
{
    return (DWORD)color_table[GetBValue(pixels[0]) >> COLOR_TABLE_SHIFT][GetGValue(pixels[0]) >> COLOR_TABLE_SHIFT][GetRValue(pixels[0]) >> COLOR_TABLE_SHIFT]
        | (DWORD)(color_table[GetBValue(pixels[1]) >> COLOR_TABLE_SHIFT][GetGValue(pixels[1]) >> COLOR_TABLE_SHIFT][GetRValue(pixels[1]) >> COLOR_TABLE_SHIFT] << 8)
        | (DWORD)(color_table[GetBValue(pixels[2]) >> COLOR_TABLE_SHIFT][GetGValue(pixels[2]) >> COLOR_TABLE_SHIFT][GetRValue(pixels[2]) >> COLOR_TABLE_SHIFT] << 16)
        | (DWORD)(color_table[GetBValue(pixels[3]) >> COLOR_TABLE_SHIFT][GetGValue(pixels[3]) >> COLOR_TABLE_SHIFT][GetRValue(pixels[3]) >> COLOR_TABLE_SHIFT] << 24);
}

COLORREF COLOR_TABLE::get_entry(int index)
{
    PALETTEENTRY e = palette_entries[index];
    return RGB(e.peRed, e.peGreen, e.peBlue);
}
