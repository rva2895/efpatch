#include "stdafx.h"
#include "palfile.h"

T_PALETTE::T_PALETTE(const void* mem, size_t size)
{
    loaded = false;
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
        entries[i] = RGB(r, g, b);
    }
    pal = CreatePalette(lp);

    free(lp);
    free(str);

    loaded = true;
}

//int ccc = 0;

int T_PALETTE::get_index(COLORREF c)
{
    //c = c & 0xF8F8F8;
    int index;
    auto it = cache.find(c);
    if (it == cache.end())
    {
        index = GetNearestPaletteIndex(pal, c);
        //ccc++;
        //if (ccc % 100000 == 0)
        //    MessageBox(0, "100 000", "", 0);
        if (cache.size() < 0x100000)
            cache.emplace(std::pair<COLORREF, int>(c, index));
    }
    else
        index = it->second;
    return index;
}

COLORREF T_PALETTE::get_entry(int index)
{
    return entries[index];
}

T_PALETTE::~T_PALETTE()
{
    if (loaded)
        DeleteObject(pal);
}
