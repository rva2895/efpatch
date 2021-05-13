#pragma once
#include "stdafx.h"
#include "mg1file.h"
#include <string>
#include <vector>

struct REC_DATA
{
    bool exists;
    bool valid;
    DWORD version;
    std::string file;
    HBITMAP map;
    std::string team_1[8];
    std::string team_2[8];
    int n_team1;
    int n_team2;
    int duration;
    int start_time;
    char team_1_colors[8];
    char team_1_civs[8];
    char team_2_colors[8];
    char team_2_civs[8];

    float team_1_cc_x[8];
    float team_1_cc_y[8];
    float team_2_cc_x[8];
    float team_2_cc_y[8];

    int map_x;
};

#define CACHE_SIZE 100

void setRecBrowseHooks();

class REC_CACHE
{
private:
    bool state_valid;
    void* wnd;
    int x;
    int y;
    DWORD tid;
    CRITICAL_SECTION cs;
    REC_DATA* cache[CACHE_SIZE];
    //std::vector<std::pair<std::string, int>> queue;
public:
    REC_CACHE(void*,int,int);
    ~REC_CACHE();
    REC_DATA get_rec_data(std::string f, int priority);
    void add_rec_data(REC_DATA rd);
    int get_rec_cache_index(std::string f);
    //void ack_queue(std::string f);
    void update(std::string);
    void invalidate();

    int getX() { return x; };
    int getY() { return y; };
};

HBITMAP make_bitmap(int x, int y, int view_x, int view_y, TILE* map);
