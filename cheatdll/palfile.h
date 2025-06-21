#pragma once

class T_PALETTE
{
private:
    COLORREF entries[256];
    HPALETTE pal;
    std::map<COLORREF, int> cache;
    bool loaded;
public:
    T_PALETTE(const void*, size_t);
    ~T_PALETTE();
    int get_index(COLORREF);
    COLORREF get_entry(int index);
};
