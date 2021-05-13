#include "stdafx.h"

struct timeline_colors
{
    DWORD offset1;
    DWORD offset2;
    COLORREF color1;
    COLORREF color2;
};

const timeline_colors colors[8] =
{
    { 0x005E1F44, 0x005E1F49, 0xCC9B82, 0x7F000A },
    { 0x005E1F50, 0x005E1F55, 0x0000FF, 0x000080 },
    { 0x005E1F5C, 0x005E1F61, 0x00FF00, 0x008000 },
    { 0x005E1F68, 0x005E1F6D, 0x00FFFF, 0x008080 },
    { 0x005E1F74, 0x005E1F79, 0xFFFF00, 0x808000 },
    { 0x005E1F80, 0x005E1F85, 0xFF00FF, 0x79107F },
    { 0x005E1F8C, 0x005E1F91, 0xD2D1D1, 0x808080 },
    { 0x005E1F98, 0x005E1F9D, 0x2E4EF4, 0x0019AD }
};

#pragma optimize( "s", on )
void setTimelineHooks()
{
    for (int i = 0; i < 8; i++)
    {
        writeDword(colors[i].offset1, colors[i].color1);
        writeDword(colors[i].offset2, colors[i].color2);
    }
}
#pragma optimize( "", on )
