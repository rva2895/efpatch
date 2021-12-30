#pragma once

struct UNIT_LOS_DATA
{
    /*int x1;
    int y1;
    int x2;
    int y2;
    int radius;
    bool square;
    bool sq_type;
    UNIT_LOS_DATA* next;
    */
    int x;
    int y;
    int radius;
};

void setLOSHooks();
