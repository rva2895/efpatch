#pragma once

struct MAP_INFO
{
    int string_id;
    int rollover_string_id;
    char filename[21];
    int drs_id;
};

void setMapListHooks();
