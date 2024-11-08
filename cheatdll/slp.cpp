#include "stdafx.h"
#include "slp.h"
#include "palfile.h"

#define cimg_verbosity 0
#define cimg_display 0

#include <Cimg.h>

SLP::SLP()
{
    memset(&meta, 0, sizeof(meta));
    frames = NULL;
    loaded = false;
}

SLP::~SLP()
{
    for (int i = 0; i < meta.num_frames; i++)
    {
        for (int j = 0; j < frames[i].height; j++)
            delete[] frames[i].scanlines[j].pixels;
        delete[] frames[i].scanlines;
    }
    delete[] frames;
}

int get_st(int id)
{
    switch (id)
    {
    case 51142:
        return 545;
    case 51147:
        return 535;
    case 51148:
        return 537;
    case 51149:
        return 526;
    case 51150:
        return 530;
    case 51141:
    case 51143:
    case 51144:
    case 51145:
    case 51146:
        return 526;
    case 51124:
        return 424;
    case 51126:
        return 423;
    case 51127:
        return 417;
    case 51128:
        return 429;
    case 51129:
        return 423;
    case 51130:
        return 435;
    case 51121:
    case 51122:
    case 51123:
    case 51125:
        return 416;
    //FO, RS 768
    case 51131:
        return 426;
    case 51132:
        return 416;
    //FO, RS 1024:
    case 51151:
        return 576;
    case 51152:
        return 526;
    //IR, NR, BS 1024:
    case 51153:
        return 576;
    case 51154:
        return 536;
    case 51155:
        return 526;
    //IR, NR, BS 768
    case 51133:
        return 426;
    case 51134:
        return 416;
    case 51135:
        return 416;
    default:
        return 0;
    }
}

//parser state machine states
enum states
{
    COPY,
    FILL,
    SET
};

int print_FILL(uint8_t*& ptr, uint8_t px_type, int len, SLP_scanline& scanline, int start)
{
    unsigned char* p = ptr;
    unsigned char cmd = 0;
    switch (px_type)
    {
    case T_PIXEL:
        if (len < 16)
        {
            cmd = len << 4;
            cmd |= 0x07;
            *p++ = cmd;
        }
        else if (len <= 0xFF)
        {
            cmd = 0x07;
            *p++ = cmd;
            *p++ = len & 0xFF;
        }
        else
        {
            print_FILL(p, px_type, len - 0xFF, scanline, start);
            cmd = 0x07;
            *p++ = cmd;
            *p++ = 0xFF;
        }
        break;
    case T_PLAYER_COLOR:
        if (len < 16)
        {
            cmd = len << 4;
            cmd |= 0x0A;
            *p++ = cmd;
        }
        else if (len <= 0xFF)
        {
            cmd = 0x0A;
            *p++ = cmd;
            *p++ = len & 0xFF;
        }
        else
            __debugbreak();
        break;
    default:
        __debugbreak();
        break;
    }
    *p++ = scanline.pixels[start].data;
    ptr = p;
    return start + len;
}

int print_COPY(uint8_t*& ptr, uint8_t px_type, int len, SLP_scanline& scanline, int start)
{
    unsigned char* p = ptr;
    unsigned char cmd = 0;
    switch (px_type)
    {
    case T_PIXEL:
        if (len < 64)
        {
            cmd = len << 2;
            *p++ = cmd;
        }
        else if (len <= 0xFFF)
        {
            cmd = len >> 4;
            cmd &= 0xF0;
            cmd |= 0x02;
            *p++ = cmd;
            *p++ = len & 0xFF;
        }
        else
            __debugbreak();
        break;
    case T_PLAYER_COLOR:
        if (len < 16)
        {
            cmd = len << 4;
            cmd |= 0x06;
            *p++ = cmd;
        }
        else if (len <= 0xFF)
        {
            cmd = 0x06;
            *p++ = cmd;
            *p++ = len & 0xFF;
        }
        else
            __debugbreak();
        break;
    default:
        __debugbreak();
        break;
    }
    for (int i = 0; i < len; i++)
    {
        *p = scanline.pixels[start++].data;
        p++;
    }

    ptr = p;
    return start;
}

int print_SET(uint8_t*& ptr, uint8_t px_type, int len, SLP_scanline& scanline, int start)
{
    unsigned char* p = ptr;
    unsigned char cmd = 0;
    switch (px_type)
    {
    case T_SHADOW:
        if (len < 16)
        {
            cmd = len << 4;
            cmd |= 0x0B;
            *p++ = cmd;
        }
        else if (len <= 0xFF)
        {
            cmd = 0x0B;
            *p++ = cmd;
            *p++ = len & 0xFF;
        }
        else
        {
            cmd = 0x0B;
            *p++ = cmd;
            *p++ = 0xFF;
            print_SET(p, px_type, len - 0xFF, scanline, start);
        }
        break;
    case T_TRANSPARENT:
        if (len < 64)
        {
            cmd = len << 2;
            cmd |= 0x01;
            *p++ = cmd;
        }
        else if (len <= 0xFFF)
        {
            cmd = len >> 4;
            cmd &= 0xF0;
            cmd |= 0x03;
            *p++ = cmd;
            *p++ = len & 0xFF;
        }
        else
            __debugbreak();
        break;
    case T_OUTLINE_COLOR:
        if (len > 1)
        {
            cmd = 0x5E;
            *p++ = cmd;
            *p++ = len & 0xFF;
        }
        else
        {
            cmd = 0x4E;
            *p++ = cmd;
        }
        break;
    case T_OUTLINE_SHIELD:
        if (len > 1)
        {
            cmd = 0x7E;
            *p++ = cmd;
            *p++ = len & 0xFF;
        }
        else
        {
            cmd = 0x6E;
            *p++ = cmd;
        }
        break;
    }
    ptr = p;
    return start + len;
}

template<class T>
T read(uint8_t*& p)
{
    T r = *(T*)p;
    p += sizeof(T);
    return r;
}

bool SLP::load(uint8_t* data)
{
    uint8_t* p = data;

    memcpy(&meta, p, sizeof(meta));
    p += sizeof(meta);

    frames = new SLP_frame[meta.num_frames];
    for (int i = 0; i < meta.num_frames; i++)
    {
        uint32_t* cmd_table = (uint32_t*)(data + read<uint32_t>(p)); //cmd table offset
        SLP_outline* outline = (SLP_outline*)(data + read<uint32_t>(p)); //outline table offset
        read<uint32_t>(p);                                  //palette offset
        frames[i].properties = read<uint32_t>(p);           //properties
        frames[i].width = read<int32_t>(p);                 //width
        frames[i].height = read<int32_t>(p);                //height
        frames[i].hotspot_x = read<int32_t>(p);             //hotspot_x
        frames[i].hotspot_y = read<int32_t>(p);             //hotspot_y

        frames[i].scanlines = new SLP_scanline[frames[i].height];

        for (int j = 0; j < frames[i].height; j++)
        {
            SLP_pixel px;
            px.type = T_TRANSPARENT;
            px.data = 0;
            frames[i].scanlines[j].pixels = new SLP_pixel[frames[i].width];

            if (outline[j].left == 0x8000 || outline[j].right == 0x8000)
                for (int k = 0; k < frames[i].width; k++)
                    frames[i].scanlines[j].pixels[k] = px;
            else
            {
                int k = 0;
                for (; k < outline[j].left; k++)
                    frames[i].scanlines[j].pixels[k] = px;

                uint8_t* ptr = data + cmd_table[j];
                uint8_t cmd;
                int count;
                while (k < frames[i].width)
                {
                    cmd = *ptr;
                    ptr++;

                    if ((cmd & 0x03) == 0x00)   //check 2 right bits (color list)
                    {
                        px.type = T_PIXEL;
                        count = cmd >> 2;
                        for (int t = 0; t < count; t++)
                        {
                            px.data = *ptr;
                            frames[i].scanlines[j].pixels[k] = px;
                            k++;
                            ptr++;
                        }
                    }
                    else if ((cmd & 0x03) == 0x01)        //skip
                    {
                        px.type = T_TRANSPARENT;
                        count = cmd >> 2;
                        if (!count)
                        {
                            count = *ptr;
                            ptr++;
                        }
                        for (int t = 0; t < count; t++)
                        {
                            frames[i].scanlines[j].pixels[k] = px;
                            k++;
                        }
                    }
                    else if ((cmd & 0x0F) == 0x02)        //big color list
                    {
                        px.type = T_PIXEL;
                        count = cmd >> 4;
                        count = count * 0x100 + *ptr;
                        ptr++;
                        for (int t = 0; t < count; t++)
                        {
                            px.data = *ptr;
                            frames[i].scanlines[j].pixels[k] = px;
                            k++;
                            ptr++;
                        }
                    }
                    else if ((cmd & 0x0F) == 0x03)        //big skip
                    {
                        px.type = T_TRANSPARENT;
                        count = cmd >> 4;
                        count = count * 0x100 + *ptr;
                        ptr++;
                        for (int t = 0; t < count; t++)
                        {
                            frames[i].scanlines[j].pixels[k] = px;
                            k++;
                        }
                    }
                    else if ((cmd & 0x0F) == 0x06)        //player color list
                    {
                        px.type = T_PLAYER_COLOR;
                        count = cmd >> 4;
                        if (!count)
                        {
                            count = *ptr;
                            ptr++;
                        }
                        for (int t = 0; t < count; t++)
                        {
                            px.data = *ptr;
                            frames[i].scanlines[j].pixels[k] = px;
                            k++;
                            ptr++;
                        }
                    }
                    else if ((cmd & 0x0F) == 0x07)        //color fill
                    {
                        px.type = T_PIXEL;
                        count = cmd >> 4;
                        if (!count)
                        {
                            count = *ptr;
                            ptr++;
                        }
                        px.data = *ptr;
                        for (int t = 0; t < count; t++)
                        {
                            frames[i].scanlines[j].pixels[k] = px;
                            k++;
                        }
                        ptr++;
                    }
                    else if ((cmd & 0x0F) == 0x0A)        //player color fill
                    {
                        px.type = T_PLAYER_COLOR;
                        count = cmd >> 4;
                        if (!count)
                        {
                            count = *ptr;
                            ptr++;
                        }
                        px.data = *ptr;
                        for (int t = 0; t < count; t++)
                        {
                            frames[i].scanlines[j].pixels[k] = px;
                            k++;
                        }
                        ptr++;
                    }
                    else if ((cmd & 0x0F) == 0x0B)        //shadow
                    {
                        px.type = T_SHADOW;
                        count = cmd >> 4;
                        if (!count)
                        {
                            count = *ptr;
                            ptr++;
                        }
                        for (int t = 0; t < count; t++)
                        {
                            frames[i].scanlines[j].pixels[k] = px;
                            k++;
                        }
                    }
                    else if (cmd == 0x7E)        //outline shield, run
                    {
                        px.type = T_OUTLINE_SHIELD;
                        count = *ptr;
                        ptr++;
                        for (int t = 0; t < count; t++)
                        {
                            frames[i].scanlines[j].pixels[k] = px;
                            k++;
                        }
                    }
                    else if (cmd == 0x5E)        //outline color, run
                    {
                        px.type = T_OUTLINE_COLOR;
                        count = *ptr;
                        ptr++;
                        for (int t = 0; t < count; t++)
                        {
                            frames[i].scanlines[j].pixels[k] = px;
                            k++;
                        }
                    }
                    else if (cmd == 0x6E)        //outline shield, single
                    {
                        px.type = T_OUTLINE_SHIELD;
                        frames[i].scanlines[j].pixels[k] = px;
                        k++;
                    }
                    else if (cmd == 0x4E)        //outline color, single
                    {
                        px.type = T_OUTLINE_COLOR;
                        frames[i].scanlines[j].pixels[k] = px;
                        k++;
                    }

                    else if (cmd == 0x0F)        //end of line
                    {
                        break;
                    }
                    else
                    {
                        __debugbreak();
                    }
                }
                //fill the rest with blank pixels;
                px.type = T_TRANSPARENT;
                px.data = 0;
                for (; k < frames[i].width; k++)
                    frames[i].scanlines[j].pixels[k] = px;
            }
        }
    }

    loaded = true;
    return true;
}

void SLP::stretch(int x, int y, int id)
{
    int stretch_at = get_st(id);
    //we only stretch frame 0;
    for (int j = 0; j < frames[0].height; j++)
    {
        SLP_pixel* new_px = new SLP_pixel[x];
        memcpy(new_px, frames[0].scanlines[j].pixels, frames[0].width * sizeof(SLP_pixel));
        delete[] frames[0].scanlines[j].pixels;
        frames[0].scanlines[j].pixels = new_px;
        //zann large fix
        if ((id == 51149) && (j > 50))
            stretch_at = 520;

        int delta = x - frames[0].width;
        memmove(frames[0].scanlines[j].pixels + stretch_at + delta, frames[0].scanlines[j].pixels + stretch_at, (frames[0].width - stretch_at) * sizeof(SLP_pixel));
        for (int k = stretch_at; k < stretch_at + delta; k++)
            frames[0].scanlines[j].pixels[k] = frames[0].scanlines[j].pixels[stretch_at];
    }
    frames[0].width = x;
    frames[0].hotspot_y = -y + frames[0].height;
}

void SLP::stretch_techtree(int x, int y)
{
    UNREFERENCED_PARAMETER(x);
    int i;
    int y_offset;
    if (y >= 1024) //stretch 1280 (frame 1)
    {
        i = 1;
        y_offset = (y - 1024) / 2;
    }
    else //stretch 1024 (frame 0)
    {
        i = 0;
        y_offset = (y - 768) / 2;
    }

    SLP_scanline* new_scanline = new SLP_scanline[y];
    memcpy(new_scanline + y_offset, frames[i].scanlines, frames[i].height * sizeof(SLP_scanline));
    delete[] frames[i].scanlines;
    frames[i].scanlines = new_scanline;
    for (int j = 0; j < y; j++)
        if (j < y_offset || j >= y_offset + frames[i].height)
        {
            frames[i].scanlines[j].pixels = new SLP_pixel[frames[i].width];
            for (int k = 0; k < frames[i].width; k++)
            {
                frames[i].scanlines[j].pixels[k].type = T_PIXEL;
                frames[i].scanlines[j].pixels[k].data = 0;
            }
        }
    frames[i].height = y;
    frames[i].hotspot_y += y_offset;
}

uint8_t* SLP::write(int* size, bool allow_fill)
{
    size_t data_len_max = 0x1000;
    for (int i = 0; i < meta.num_frames; i++)
        data_len_max += (frames[i].width + 0x10) * (frames[i].height + 0x10) + 0x20;
    data_len_max *= 2;
    uint8_t* data = (uint8_t*)malloc(data_len_max);

    strncpy(meta.comment, "EFPATCH SLP Writer 2.0\0", 24);

    uint8_t* ptr = data;
    //skip headers
    memcpy(ptr, &meta, sizeof(meta));
    ptr += sizeof(SLP_meta);
    
    SLP_file_frame* ptr_frame = (SLP_file_frame*)ptr;
    
    ptr += meta.num_frames * sizeof(SLP_file_frame);

    for (int i = 0; i < meta.num_frames; i++)
    {
        SLP_outline* ptr_outline = (SLP_outline*)ptr;
        ptr += frames[i].height * sizeof(SLP_outline);
        uint32_t* ptr_commands = (uint32_t*)ptr;
        ptr += frames[i].height * sizeof(uint32_t);

        ptr_frame[i].cmd_table_offset = (uint8_t*)ptr_commands - data;
        ptr_frame[i].outline_table_offset = (uint8_t*)ptr_outline - data;
        ptr_frame[i].palette_offset = 0;
        ptr_frame[i].properties = frames[i].properties;
        ptr_frame[i].width = frames[i].width;
        ptr_frame[i].height = frames[i].height;
        ptr_frame[i].hotspot_x = frames[i].hotspot_x;
        ptr_frame[i].hotspot_y = frames[i].hotspot_y;

        for (int j = 0; j < frames[i].height; j++)
        {
            ptr_commands[j] = ptr - data;

            uint16_t left;
            uint16_t right;
            int k = 0;
            while (k < frames[i].width && frames[i].scanlines[j].pixels[k].type == T_TRANSPARENT)
                k++;
            if (k >= frames[i].width)   //line is blank
            {
                left = 0x8000;
                right = 0x8000;
            }
            else
            {
                left = k;
                right = 0;
                k = frames[i].width - 1;
                while (frames[i].scanlines[j].pixels[k].type == T_TRANSPARENT && k > 0)
                {
                    k--;
                    right++;
                }

                k = left;
                int end = frames[i].width - right;

                int start = k;
                int state;
                int repeat = 1;
                int len = 1;
                SLP_pixel px = frames[i].scanlines[j].pixels[k];
                switch (px.type)
                {
                case T_PIXEL:
                case T_PLAYER_COLOR:
                    state = COPY;
                    break;
                case T_SHADOW:
                case T_TRANSPARENT:
                case T_OUTLINE_COLOR:
                case T_OUTLINE_SHIELD:
                    state = SET;
                    break;
                default:
                    __debugbreak();
                    break;
                }
                k++;

                //parse scanline data
                while (k < end)
                {
                    switch (state)
                    {
                    case COPY:
                        if (frames[i].scanlines[j].pixels[k].type == px.type)
                        {
                            if ((repeat < 3) || !allow_fill)
                            {
                                len++;
                                if (px.data == frames[i].scanlines[j].pixels[k].data)
                                    repeat++;
                                else
                                    repeat = 1;
                            }
                            else
                            {
                                len -= 3;
                                if (len != 0)
                                    start = print_COPY(ptr, px.type, len, frames[i].scanlines[j], start);
                                state = FILL;
                                len = 3;
                                k--;
                            }
                        }
                        else
                        {
                            start = print_COPY(ptr, px.type, len, frames[i].scanlines[j], start);
                            len = 1;
                            repeat = 1;
                            switch (frames[i].scanlines[j].pixels[k].type)
                            {
                            case T_PIXEL:
                            case T_PLAYER_COLOR:
                                break;
                            case T_SHADOW:
                            case T_TRANSPARENT:
                            case T_OUTLINE_COLOR:
                            case T_OUTLINE_SHIELD:
                                state = SET;
                                break;
                            default:
                                __debugbreak();
                                break;
                            }
                        }
                        break;
                    case FILL:
                        if (frames[i].scanlines[j].pixels[k] == px)
                            len++;
                        else
                        {
                            start = print_FILL(ptr, px.type, len, frames[i].scanlines[j], start);
                            len = 1;
                            repeat = 1;
                            switch (frames[i].scanlines[j].pixels[k].type)
                            {
                            case T_PIXEL:
                            case T_PLAYER_COLOR:
                                state = COPY;
                                break;
                            case T_SHADOW:
                            case T_TRANSPARENT:
                            case T_OUTLINE_COLOR:
                            case T_OUTLINE_SHIELD:
                                state = SET;
                                break;
                            default:
                                __debugbreak();
                                break;
                            }
                        }
                        break;
                    case SET:
                        if (frames[i].scanlines[j].pixels[k].type == px.type)
                            len++;
                        else
                        {
                            start = print_SET(ptr, px.type, len, frames[i].scanlines[j], start);
                            len = 1;
                            repeat = 1;
                            switch (frames[i].scanlines[j].pixels[k].type)
                            {
                            case T_PIXEL:
                            case T_PLAYER_COLOR:
                                state = COPY;
                                break;
                            case T_SHADOW:
                            case T_TRANSPARENT:
                            case T_OUTLINE_COLOR:
                            case T_OUTLINE_SHIELD:
                                break;
                            default:
                                __debugbreak();
                                break;
                            }
                        }
                        break;
                    default:
                        __debugbreak();
                        break;
                    }
                    px = frames[i].scanlines[j].pixels[k];
                    k++;
                }
                //print what's left
                switch (state)
                {
                case COPY:
                    start = print_COPY(ptr, px.type, len, frames[i].scanlines[j], start);
                    break;
                case FILL:
                    start = print_FILL(ptr, px.type, len, frames[i].scanlines[j], start);
                    break;
                case SET:
                    if (px.type != T_TRANSPARENT)
                        start = print_SET(ptr, px.type, len, frames[i].scanlines[j], start);
                    else
                        __debugbreak();        //!!!!!!
                    break;
                default:
                    __debugbreak();
                    break;
                }
            }
            *ptr = 0x0F;
            ptr++;
            ptr_outline[j].left = left;
            ptr_outline[j].right = right;
        }
    }

    *size = ptr - data;
    return data;
}

void SLP::resize(int x, int y, T_PALETTE& pal)
{
    for (int i = 0; i < meta.num_frames; i++)
        if (x != frames[i].width || y != frames[i].height)
        {
            uint8_t* pixels = (uint8_t*)malloc(frames[i].width * frames[i].height * 3);
            for (int j = 0; j < frames[i].height; j++)
                for (int k = 0; k < frames[i].width; k++)
                {
                    COLORREF c = pal.get_entry(frames[i].scanlines[j].pixels[k].data);
                    pixels[(j * frames[i].width + k)] = GetRValue(c);
                    pixels[(j * frames[i].width + k) + frames[i].width * frames[i].height] = GetGValue(c);
                    pixels[(j * frames[i].width + k) + frames[i].width * frames[i].height * 2] = GetBValue(c);
                }

            cimg_library::CImg<uint8_t> image(pixels, frames[i].width, frames[i].height, 1, 3);
            image.resize(x, y, 1, 3, 5);

            for (int j = 0; j < frames[i].height; j++)
                delete[] frames[i].scanlines[j].pixels;
            delete[] frames[i].scanlines;

            frames[i].width = x;
            frames[i].height = y;
            frames[i].scanlines = new SLP_scanline[frames[i].height];
            for (int j = 0; j < frames[i].height; j++)
            {
                frames[i].scanlines[j].pixels = new SLP_pixel[frames[i].width];
                for (int k = 0; k < frames[i].width; k++)
                {
                    frames[i].scanlines[j].pixels[k].type = T_PIXEL;
                    uint8_t r = image(k, j, 0, 0);
                    uint8_t g = image(k, j, 0, 1);
                    uint8_t b = image(k, j, 0, 2);
                    frames[i].scanlines[j].pixels[k].data = pal.get_index(RGB(r, g, b));
                }
            }
            free(pixels);
        }
}

void SLP::color_replace(const uint8_t* c, int count)
{
    for (int i = 0; i < meta.num_frames; i++)
        for (int j = 0; j < frames[i].height; j++)
            for (int k = 0; k < frames[i].width; k++)
                if (frames[i].scanlines[j].pixels[k].type == T_PIXEL)
                    for (int t = 0; t < count; t++)
                        if (frames[i].scanlines[j].pixels[k].data == c[t * 2])
                        {
                            frames[i].scanlines[j].pixels[k].data = c[t * 2 + 1];
                            break;
                        }
}
