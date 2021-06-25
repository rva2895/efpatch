#pragma once

#include <queue>
#include <deque>

#define MAX_RES 210

#define WORLD_DUMP_QUEUE_MAX_TIME 120000

struct object_info
{
    int id;
    float x;
    float y;
    float hp;
    float sp;
    float resources;
    float unk50;
    char state;
    char rotation;
    short unit_id;
    unsigned int act_cs;
    unsigned int wp_cs;
    unsigned int cs;
    int target;
    bool operator<(const object_info& o2) const
    {
        return id < o2.id;
    }
};

struct player_info
{
    int id;
    std::vector<float> resources;
    std::vector<object_info> objects;
    unsigned int cs;
};

class WORLD_DUMP
{
private:
    bool deflated;
    int worldtime;
    std::vector<player_info> players;
    unsigned int cs;
    void* deflated_data;
    size_t deflated_data_size;

public:
    WORLD_DUMP();
    int get_worldtime() const;
    unsigned int get_cs() const;
    void update_cs();
    void sort_objects();
    int deflate_dump();
    int inflate_dump();
    void print(FILE* f) const;
};

class WORLD_DUMP_QUEUE
{
private:
    std::queue<WORLD_DUMP> world_dump_queue;
public:
    bool check_worldtime(int worldtime) const;
    void add_dump(const WORLD_DUMP& wd);
    void print_dump(FILE* f);
    void clear();
};

void setWorldDumpHooks();
