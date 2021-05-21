#pragma once

#include <string>

//additional structures
struct pixel
{
    unsigned char type;
    unsigned char data;
    bool operator==(const pixel& p)
    {
        return ((p.type == type) && (p.data == data));
    }
};

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

struct file_header
{
    char  version[4];
    long  num_frames;
    char  comment[24];
};

struct frame_info
{
    unsigned long    cmd_table_offset;
    unsigned long    outline_table_offset;
    unsigned long    palette_offset;
    unsigned long    properties;
    long    width;
    long    height;
    long    hotspot_x;
    long    hotspot_y;
    //
    pixel** data;
};

struct rowedge
{
    short left, right;
};

class SLP
{
private:
    std::string file;

    file_header hdr;
    frame_info* frame_i;
    rowedge** edge;
    long** rowoffset;

    bool loaded;

    long pixels_total;

public:
    SLP();
    ~SLP();

    bool load(unsigned char* fbase, int size);
    unsigned char* optimize(int* size, bool allow_fill);

    void stretch(int x, int y, int id);
    void stretch_techtree(int x, int y);

    void color_replace(unsigned char* c, int count);
};
