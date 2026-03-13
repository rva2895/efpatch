#pragma once

class T_PALETTE
{
private:
    PALETTEENTRY palette_entries[256];
    HPALETTE pal = NULL;
public:
    explicit T_PALETTE(const void*, size_t, LPPALETTEENTRY);
    explicit T_PALETTE(const LPPALETTEENTRY);
    explicit T_PALETTE(const T_PALETTE&) = delete;
    ~T_PALETTE();
    int get_index(COLORREF) const;
    COLORREF get_entry(int) const;
    HPALETTE get_palette() const;
};

#define COLOR_TABLE_SHIFT 2
#define COLOR_TABLE_ENTRY_SIZE (256 >> COLOR_TABLE_SHIFT)
#define CLR_DELTA (256 / COLOR_TABLE_ENTRY_SIZE / 2)

class COLOR_TABLE
{
private:
    unsigned char color_table[COLOR_TABLE_ENTRY_SIZE][COLOR_TABLE_ENTRY_SIZE][COLOR_TABLE_ENTRY_SIZE];
    PALETTEENTRY palette_entries[256];
public:
    explicit COLOR_TABLE(const void*, size_t);
    explicit COLOR_TABLE(const LPPALETTEENTRY);
    explicit COLOR_TABLE(const COLOR_TABLE&) = delete;
    void init(const T_PALETTE&);
    unsigned char get_index(COLORREF);
    unsigned char get_index_rgb(DWORD);
    DWORD get_4index(DWORD* pixels);
    COLORREF get_entry(int);
};
