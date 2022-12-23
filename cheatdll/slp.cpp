#include "stdafx.h"
#include "slp.h"
#include "palfile.h"

#define cimg_verbosity 0
#define cimg_display 0

#include <Cimg.h>

SLP::SLP()
{
    memset(&hdr, 0, sizeof(hdr));
    frame_i = NULL;
    edge = NULL;
    rowoffset = NULL;
    pixels_total = 0;
    loaded = false;
}

SLP::~SLP()
{
    for (int i = 0; i < hdr.num_frames; i++)
    {
        for (int j = 0; j < frame_i[i].height; j++)
            delete[] frame_i[i].data[j];
        delete[] frame_i[i].data;
        delete[] rowoffset[i];
        delete[] edge[i];
    }
    delete[] edge;
    delete[] rowoffset;
    delete[] frame_i;
}

#pragma message("***slp.cpp: SLP::load(), parameter size - check!")
#pragma message("***slp.cpp: 50230 slp crashes!")

bool SLP::load(unsigned char* fbase, int size)
{
    UNREFERENCED_PARAMETER(size);
    unsigned char* fptr = fbase;

    //file_header hdr;
    memcpy(&hdr, fptr, sizeof(file_header));
    fptr += sizeof(file_header);

    frame_i = new frame_info[hdr.num_frames];
    for (int i = 0; i < hdr.num_frames; i++)
    {
        frame_i[i].cmd_table_offset = *(unsigned long*)fptr;        fptr += 4;
        frame_i[i].outline_table_offset = *(unsigned long*)fptr;    fptr += 4;
        frame_i[i].palette_offset = *(unsigned long*)fptr;            fptr += 4;
        frame_i[i].properties = *(unsigned long*)fptr;                fptr += 4;
        frame_i[i].width = *(long*)fptr;                            fptr += 4;
        frame_i[i].height = *(long*)fptr;                            fptr += 4;
        frame_i[i].hotspot_x = *(long*)fptr;                        fptr += 4;
        frame_i[i].hotspot_y = *(long*)fptr;                        fptr += 4;
        frame_i[i].data = NULL;
    }

    edge = new rowedge*[hdr.num_frames];
    rowoffset = new long*[hdr.num_frames];
    for (int i = 0; i < hdr.num_frames; i++)
    {
        fptr = fbase + frame_i[i].outline_table_offset;

        edge[i] = new rowedge[frame_i[i].height];
        memcpy(edge[i], fptr, sizeof(rowedge) * frame_i[i].height);
        fptr += sizeof(rowedge) * frame_i[i].height;

        rowoffset[i] = new long[frame_i[i].height];
        memcpy(rowoffset[i], fptr, sizeof(long) * frame_i[i].height);

        frame_i[i].data = new pixel*[frame_i[i].height];
        for (int j = 0; j < frame_i[i].height; j++)
        {
            pixel px;
            px.type = T_TRANSPARENT;
            px.data = 0;
            frame_i[i].data[j] = new pixel[frame_i[i].width];
            //parse line
            if ((edge[i][j].left == -32768) && (edge[i][j].right == -32768))    //entire line is blank
            {
                for (int k = 0; k < frame_i[i].width; k++)
                    frame_i[i].data[j][k] = px;
                ((int*)(fbase + frame_i[i].cmd_table_offset))[j] = 0;
                continue;
            }
            //left edge
            int k = 0;    //current position in line
            for (; k < edge[i][j].left; k++)
                frame_i[i].data[j][k] = px;
            //parse actual data
            unsigned char cmd;
            int count;
            unsigned char* ptr = fbase + ((int*)(fbase + frame_i[i].cmd_table_offset))[j];
            while (1)
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
                        frame_i[i].data[j][k] = px;
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
                        frame_i[i].data[j][k] = px;
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
                        frame_i[i].data[j][k] = px;
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
                        frame_i[i].data[j][k] = px;
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
                        frame_i[i].data[j][k] = px;
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
                        frame_i[i].data[j][k] = px;
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
                        frame_i[i].data[j][k] = px;
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
                        frame_i[i].data[j][k] = px;
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
                        frame_i[i].data[j][k] = px;
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
                        frame_i[i].data[j][k] = px;
                        k++;
                    }
                }
                else if (cmd == 0x6E)        //outline shield, single
                {
                    px.type = T_OUTLINE_SHIELD;
                    frame_i[i].data[j][k] = px;
                    k++;
                }
                else if (cmd == 0x4E)        //outline color, single
                {
                    px.type = T_OUTLINE_COLOR;
                    frame_i[i].data[j][k] = px;
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
            for (; k < frame_i[i].width; k++)
                frame_i[i].data[j][k] = px;

            /*for (int k = 0; k < frame_i[i].width; k++)
            if (frame_i[i].data[j][k].type != TRANSPARENT)
            continue;
            ((int*)(fbase + frame_i[i].cmd_table_offset))[j] = 0;*/
        }
    }

    loaded = true;
    return true;
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

void SLP::stretch(int x, int y, int id)
{
    int stretch_at = get_st(id);

    for (int j = 0; j < frame_i[0].height; j++)
    {
        pixel* new_px = new pixel[x];
        memcpy(new_px, frame_i[0].data[j], frame_i[0].width*sizeof(pixel));
        //free(frame_i[0].data[j]);
        delete[] frame_i[0].data[j];  //!!!
        frame_i[0].data[j] = new_px;
        //zann large fix
        if ((id == 51149) && (j > 50))
            stretch_at = 520;
        for (int k = x - 1; k >= (x - (frame_i[0].width - stretch_at)); k--)
            frame_i[0].data[j][k] = frame_i[0].data[j][k - (x - frame_i[0].width)];
        for (int k = stretch_at; k < stretch_at + (x - frame_i[0].width); k++)
            frame_i[0].data[j][k] = frame_i[0].data[j][stretch_at];
    }
    frame_i[0].width = x;
    frame_i[0].hotspot_y = -y + frame_i[0].height;
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

    rowedge* re = new rowedge[y];
    memcpy(re + y_offset, edge[i], frame_i[i].height * sizeof(rowedge));
    delete[] edge[i];
    edge[i] = re;
    for (int j = 0; j < y; j++)
    {
        if (j < y_offset || j >= (y_offset + frame_i[i].height))
        {
            edge[i][j].left = 0;
            edge[i][j].right = 0;
        }
    }

    pixel** new_px = new pixel*[y];
    memcpy(new_px + y_offset, frame_i[i].data, frame_i[i].height * sizeof(pixel*));
    delete[] frame_i[i].data;
    frame_i[i].data = new_px;
    for (int j = 0; j < y; j++)
    {
        if (j < y_offset || j >= (y_offset + frame_i[i].height))
        {
            frame_i[i].data[j] = new pixel[frame_i[i].width];
            for (int k = 0; k < frame_i[i].width; k++)
            {
                frame_i[i].data[j][k].type = T_PIXEL;
                frame_i[i].data[j][k].data = 0;
            }
        }
    }
    frame_i[i].height = y;
    frame_i[i].hotspot_y += y_offset;
}

//parser state machine states
enum states
{
    COPY,
    FILL,
    SET
};

////////////------------------------
int print_TEST(unsigned char** ptr, int type, int len, pixel* row, int start)
{
    UNREFERENCED_PARAMETER(row);
    UNREFERENCED_PARAMETER(type);
    unsigned char* p = *ptr;
    unsigned char cmd = 0;
    cmd = 0x0E;
    *p++ = cmd;
    *ptr = p;
    return start + len;
}
////////////------------------------

int print_FILL(unsigned char** ptr, int type, int len, pixel* row, int start)
{
    //////
    //start = print_TEST(ptr, type, len, row, start);
    //////
    unsigned char* p = *ptr;
    unsigned char cmd = 0;
    switch (type)
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
            print_FILL(&p, type, len - 0xFF, row, start);
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
    *p++ = row[start].data;
    *ptr = p;
    return start + len;
}

int print_COPY(unsigned char** ptr, int type, int len, pixel* row, int start)
{
    //////
    //start = print_TEST(ptr, type, len, row, start);
    //////
    unsigned char* p = *ptr;
    /*bool fill_flag = false;
    if (len >= 3)    //check run end
    {
    if ((row[start + len - 1] == row[start + len - 2]) && (row[start + len - 1] == row[start + len - 3]))
    {
    len -= 3;
    fill_flag = true;
    }
    }*/
    unsigned char cmd = 0;
    switch (type)
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
        *p = row[start++].data;
        p++;
    }

    //if (fill_flag)
    //    start = print_FILL(&p, type, 3, row, start);

    *ptr = p;
    return start;
}

int print_SET(unsigned char** ptr, int type, int len, pixel* row, int start)
{
    //////
    //start = print_TEST(ptr, type, len, row, start);
    //////
    unsigned char* p = *ptr;
    unsigned char cmd = 0;
    switch (type)
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
            //start = print_SET(&p, type, len - 0xFF, row, start += 0xFF);
            print_SET(&p, type, len - 0xFF, row, start);
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
    *ptr = p;
    return start + len;
}

bool compare_edges(rowedge** edge, int height, int frame)
{
    for (int i = 0; i < height; i++)
        if ((edge[frame][i].left != edge[frame - 1][i].left) || (edge[frame][i].right != edge[frame - 1][i].right))
            return false;
    return true;
}

unsigned char* SLP::optimize(int* size, bool allow_fill)
{
    void* new_slp = malloc(*size * 16);
    memset(new_slp, 0, *size * 16);

    unsigned char* ptr = (unsigned char*)new_slp;

    memcpy(ptr, &hdr, sizeof(file_header));
    ptr += sizeof(file_header);

    //print header later, now skip
    ptr += hdr.num_frames * 8 * sizeof(long);

    for (int i = 0; i < hdr.num_frames; i++)
    {
        //dirty hack: store array here temporarly
        if (frame_i[i].height > 0)
            frame_i[i].cmd_table_offset = (unsigned long)malloc(sizeof(unsigned long)*frame_i[i].height);
        else
            frame_i[i].cmd_table_offset = 0;

        for (int j = 0; j < frame_i[i].height; j++)
        {
            ((unsigned long*)frame_i[i].cmd_table_offset)[j] = ptr - (unsigned char*)new_slp;
            int k = 0;
            while ((frame_i[i].data[j][k].type == T_TRANSPARENT) && (k < (frame_i[i].width - edge[i][j].right)))
                k++;
            if (k >= frame_i[i].width)
            {
                edge[i][j].left = -32768; edge[i][j].right = -32768;
                continue;
            }
            else
            {
                edge[i][j].left = k;
                if (edge[i][j].right == -32768)
                    edge[i][j].right = 0;
            }

            int start = k;
            //main scan loop
            int state;
            int repeat = 1;
            int len = 1;
            int type = frame_i[i].data[j][k].type;
            int data = frame_i[i].data[j][k].data;
            switch (frame_i[i].data[j][k].type)
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

            //below is an implementation of a state machine, refer to the graph
            while (k < (frame_i[i].width - edge[i][j].right))
            {
                switch (state)
                {
                case COPY:
                    if (frame_i[i].data[j][k].type == type)
                    {
                        if ((repeat < 3) || !allow_fill)
                        {
                            len++;
                            if (data == frame_i[i].data[j][k].data)
                                repeat++;
                            else
                                repeat = 1;
                        }
                        else
                        {
                            len -= 3;
                            if (len != 0)
                                start = print_COPY(&ptr, type, len, frame_i[i].data[j], start);
                            state = FILL;
                            len = 3;
                            k--;
                        }
                    }
                    else
                    {
                        start = print_COPY(&ptr, type, len, frame_i[i].data[j], start);
                        len = 1;
                        repeat = 1;
                        switch (frame_i[i].data[j][k].type)
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
                    if ((frame_i[i].data[j][k].type == type) && (frame_i[i].data[j][k].data == data))
                        len++;
                    else
                    {
                        start = print_FILL(&ptr, type, len, frame_i[i].data[j], start);
                        len = 1;
                        repeat = 1;
                        switch (frame_i[i].data[j][k].type)
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
                    if (frame_i[i].data[j][k].type == type)
                        len++;
                    else
                    {
                        start = print_SET(&ptr, type, len, frame_i[i].data[j], start);
                        len = 1;
                        repeat = 1;
                        switch (frame_i[i].data[j][k].type)
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
                type = frame_i[i].data[j][k].type;
                data = frame_i[i].data[j][k].data;
                k++;
            }
            //print what's left
            switch (state)
            {
            case COPY:
                start = print_COPY(&ptr, type, len, frame_i[i].data[j], start);
                break;
            case FILL:
                start = print_FILL(&ptr, type, len, frame_i[i].data[j], start);
                break;
            case SET:
                if (type != T_TRANSPARENT)
                    start = print_SET(&ptr, type, len, frame_i[i].data[j], start);
                else
                    edge[i][j].right = len;        //might be off by one, check!!!!!!
                break;
            default:
                __debugbreak();
                break;
            }
            *ptr = 0x0F;    ptr++;
        }
    }
    //align
    if ((unsigned long)ptr % 4)
        ptr += 4 - (unsigned long)ptr % 4;

    unsigned char* prev_frame_start_ptr = 0;
    unsigned char* frame_start_ptr = 0;
    //print outline offsets
    for (int i = 0; i < hdr.num_frames; i++)
    {
        prev_frame_start_ptr = frame_start_ptr;
        if (frame_i[i].cmd_table_offset)
            frame_start_ptr = (unsigned char*)new_slp + ((unsigned long*)frame_i[i].cmd_table_offset)[0];
        else
        {
            frame_i[i].width = 0;
            frame_start_ptr = 0;
            continue;
        }

        frame_i[i].outline_table_offset = ptr - (unsigned char*)new_slp;

        //trim outline rows
        short min = 0x7FFF;
        for (int j = 0; j < frame_i[i].height; j++)
            if ((edge[i][j].left < min) && (edge[i][j].left != -32768))
                min = edge[i][j].left;
        if (min == 0x7FFF)    //frame is blank
        {
            frame_i[i].height = 0;
            frame_i[i].width = 0;
            frame_i[i].cmd_table_offset = 0;
            frame_i[i].outline_table_offset = 0;
            continue;
        }
        for (int j = 0; j < frame_i[i].height; j++)
        {
            if (edge[i][j].left != -32768)
                edge[i][j].left -= min;
            if (edge[i][j].right != -32768)
                edge[i][j].right += min;
        }
        if (min != 0x7FFF)
            frame_i[i].hotspot_x -= min;

        //now, remove redudant rows
        int first = 0;
        int last = frame_i[i].height;
        while ((edge[i][first].left == -32768) && (edge[i][first].right == -32768))
        {
            first++;
            frame_i[i].hotspot_y--;
            frame_i[i].height--;
        }
        do
        {
            last--;
            frame_i[i].height--;
        } while ((edge[i][last].left == -32768) && (edge[i][last].right == -32768));

        frame_i[i].height++;

        //compare with previous frame
        if (!prev_frame_start_ptr || (frame_start_ptr == prev_frame_start_ptr) ||
            memcmp(prev_frame_start_ptr, frame_start_ptr, frame_start_ptr - prev_frame_start_ptr) ||
            !((frame_i[i].height == frame_i[i - 1].height) && compare_edges(edge, frame_i[i].height, i)))
        {
            short* p = (short*)ptr;
            for (int j = first; j <= last; j++)
            {
                *p = edge[i][j].left; p++;
                *p = edge[i][j].right; p++;
            }
            ptr = (unsigned char*)p;

            //print cmd table offsets
            unsigned long cmd_table_offset_tmp = (unsigned long)(ptr - (unsigned char*)new_slp);
            unsigned long* q = (unsigned long*)ptr;
            for (int j = first; j <= last; j++)
            {
                *q = ((unsigned long*)frame_i[i].cmd_table_offset)[j];    q++;
            }
            ptr = (unsigned char*)q;
            free((void*)frame_i[i].cmd_table_offset);
            frame_i[i].cmd_table_offset = cmd_table_offset_tmp;
        }
        else    //frames are equal
        {
            memmove(frame_start_ptr, frame_start_ptr + (frame_start_ptr - prev_frame_start_ptr),
                ptr - (frame_start_ptr + (frame_start_ptr - prev_frame_start_ptr)));
            ptr -= frame_start_ptr - prev_frame_start_ptr;
            free((void*)frame_i[i].cmd_table_offset);

            for (int k = 0; k < i; k++)
                frame_i[k].cmd_table_offset -= frame_start_ptr - prev_frame_start_ptr;
            frame_i[i].cmd_table_offset = frame_i[i - 1].cmd_table_offset;
            frame_i[i].outline_table_offset = frame_i[i - 1].outline_table_offset;

            for (int k = 0; k < hdr.num_frames; k++)
            {
                if (k > i)
                    for (int s = 0; s < frame_i[k].height; s++)
                        ((unsigned long*)frame_i[k].cmd_table_offset)[s] -= frame_start_ptr - prev_frame_start_ptr;
                frame_i[k].outline_table_offset -= frame_start_ptr - prev_frame_start_ptr;
            }

            frame_start_ptr = prev_frame_start_ptr;
        }
    }
    *size = ptr - (unsigned char*)new_slp;
    ptr = (unsigned char*)new_slp + sizeof(file_header);
    for (int i = 0; i < hdr.num_frames; i++)
    {
        *(unsigned long*)ptr = frame_i[i].cmd_table_offset;        ptr += 4;
        *(unsigned long*)ptr = frame_i[i].outline_table_offset;    ptr += 4;
        *(unsigned long*)ptr = frame_i[i].palette_offset;        ptr += 4;
        *(unsigned long*)ptr = frame_i[i].properties;            ptr += 4;
        *(long*)ptr = frame_i[i].width;                            ptr += 4;
        *(long*)ptr = frame_i[i].height;                        ptr += 4;
        *(long*)ptr = frame_i[i].hotspot_x;                        ptr += 4;
        *(long*)ptr = frame_i[i].hotspot_y;                        ptr += 4;
    }

    return (unsigned char*)new_slp;
}

void SLP::color_replace(unsigned char* c, int count)
{
    for (int i = 0; i < hdr.num_frames; i++)
        for (int j = 0; j < frame_i[i].height; j++)
            for (int k = 0; k < frame_i[i].width; k++)
                if (frame_i[i].data[j][k].type == T_PIXEL)
                    for (int t = 0; t < count; t++)
                        if (frame_i[i].data[j][k].data == c[t * 2])
                        {
                            frame_i[i].data[j][k].data = c[t * 2 + 1];
                            break;
                        }
}

void SLP::resize(int new_x, int new_y, T_PALETTE& pal)
{
    if (new_x == frame_i[0].width && new_y == frame_i[0].height)
        return;

    unsigned char* pixels = (unsigned char*) malloc(frame_i[0].width * frame_i[0].height * 3);
    //T_PALETTE pal(NULL);
    for (int j = 0; j < frame_i[0].height; j++)
        for (int i = 0; i < frame_i[0].width; i++)
        {
            COLORREF c = pal.get_entry(frame_i[0].data[j][i].data);
            //COLORREF c = RGB(i/8, 0, 0);
            pixels[(j * frame_i[0].width + i)] = GetRValue(c);
            pixels[(j * frame_i[0].width + i) + frame_i[0].width * frame_i[0].height] = GetGValue(c);
            pixels[(j * frame_i[0].width + i) + frame_i[0].width * frame_i[0].height * 2] = GetBValue(c);
        }

    cimg_library::CImg<unsigned char> image(pixels, frame_i[0].width, frame_i[0].height, 1, 3);

    image.resize(new_x, new_y, 1, 3, 5);

    rowedge* re = new rowedge[new_y];
    delete[] edge[0];
    edge[0] = re;
    for (int j = 0; j < new_y; j++)
    {
        edge[0][j].left = 0;
        edge[0][j].right = 0;
    }

    pixel** new_px = new pixel * [new_y];
    for (int j = 0; j < frame_i[0].height; j++)
        delete[] frame_i[0].data[j];
    delete[] frame_i[0].data;
    frame_i[0].data = new_px;
    for (int j = 0; j < new_y; j++)
    {
        frame_i[0].data[j] = new pixel[new_x];
        for (int k = 0; k < new_x; k++)
        {
            frame_i[0].data[j][k].type = T_PIXEL;
            unsigned char r = image(k, j, 0, 0);
            unsigned char g = image(k, j, 0, 1);
            unsigned char b = image(k, j, 0, 2);
            frame_i[0].data[j][k].data = pal.get_index(RGB(r, g, b));
        }
    }
    frame_i[0].width = new_x;
    frame_i[0].height = new_y;

    free(pixels);
}

//TODO:
//INVESTIGATE SLPS IN RELEASE FOLDER
//FIX CLIFFS
//INTERFACE SLPS
//WORK ON REPEAT COMMAND
