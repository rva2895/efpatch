#pragma once
#include <memory>
#include "mg1file.h"
#include "sc1file.h"

class HBITMAP_HOLDER
{
private:
    HBITMAP map;

public:
    HBITMAP_HOLDER(HBITMAP map_)
    {
        map = map_;
#ifdef _DEBUG
        log("Created bitmap 0x%X", map);
#endif
    };

    ~HBITMAP_HOLDER()
    {
        DeleteObject(map);
#ifdef _DEBUG
        log("Deleted bitmap 0x%X", map);
#endif
    };

    HBITMAP get_map()
    {
        return map;
    };
    friend class std::shared_ptr<HBITMAP_HOLDER>;
};

struct REC_DATA
{
    bool exists;
    bool valid;
    //DWORD version;
    std::string version;
    std::string file;
    //HBITMAP map;
    std::shared_ptr<HBITMAP_HOLDER> map;
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

struct object_info
{
    int x;
    int y;
};

struct SCEN_DATA
{
    bool exists;
    bool valid;
    std::string file;
    //HBITMAP map;
    std::shared_ptr<HBITMAP_HOLDER> map;
    //int n_players;
    //std::string player_names[8];
    //char player_colors[8];
    bool active[16];
    bool human[16];
    unsigned int timestamp;
    int trigger_count;
    float version;
    //int n_objects;
    //object_info* objects[8];
    std::string instructions;
    std::string orig_filename;
};

#define CACHE_SIZE 100

template <class T>
class ITEM_CACHE
{
private:
    bool state_valid;
    TScreenPanel* wnd;
    int x;
    int y;
    unsigned int tid;
    HANDLE worker_thread_event;
    CRITICAL_SECTION cs;
    std::string current_file;
    T* cache[CACHE_SIZE];
public:
    ITEM_CACHE()
    {
        //init critical secion
        InitializeCriticalSection(&cs);
        EnterCriticalSection(&cs);
        state_valid = false;
        wnd = NULL;
        x = 0;
        y = 0;
        tid = 0;
        current_file = "";
        memset(cache, 0, sizeof(cache));
        /*for (int i = 0; i < CACHE_SIZE; i++)
        {
            cache[i] = new T;
            cache[i]->exists = false;
        }*/
        LeaveCriticalSection(&cs);
    };

    ~ITEM_CACHE()
    {
        EnterCriticalSection(&cs);
        for (int i = 0; i < CACHE_SIZE; i++)
        {
            //if (cache[i]->map)
                //DeleteObject(cache[i]->map);
            delete cache[i];
        }
        LeaveCriticalSection(&cs);
        DeleteCriticalSection(&cs);
    };

    void init(TScreenPanel* panel, int x_, int y_, unsigned int (__stdcall* cache_thread)(ITEM_CACHE<T>* cache))
    {
        if (state_valid)
            invalidate();

        EnterCriticalSection(&cs);
        wnd = panel;
        x = x_;
        y = y_;
        for (int i = 0; i < CACHE_SIZE; i++)
        {
            cache[i] = new T;
            cache[i]->exists = false;
        }
        //init cache thread
        worker_thread_event = CreateEvent(NULL, FALSE, FALSE, 0);
        _beginthreadex(NULL, 0, (_beginthreadex_proc_type)cache_thread, this, 0, &tid);
        WaitForSingleObject(worker_thread_event, INFINITE);
        CloseHandle(worker_thread_event);
        state_valid = true;
        LeaveCriticalSection(&cs);
    };

    T get_item(const std::string& f, int priority)
    {
        EnterCriticalSection(&cs);
        T id;
        if (state_valid)
        {
            int index = get_item_index(f);
            if (index != -1)
            {
                T* pid = cache[index];
                for (int i = index; i > 0; i--)
                    cache[i] = cache[i - 1];
                cache[0] = pid;
                id = *pid;
            }
            else
            {
                char* s = make_str_copy(f.c_str());
                PostThreadMessage(tid, WM_APP_ITEMCACHE_FILE, (WPARAM)s, priority);
                id = { 0 };
            }
        }
        else
            id = { 0 };
        LeaveCriticalSection(&cs);
        return id;
    };

    void add_item(const T& id)
    {
        EnterCriticalSection(&cs);
        if (state_valid && get_item_index(id.file) == -1)
        {
            //if (cache[CACHE_SIZE - 1]->map)
                //DeleteObject(cache[CACHE_SIZE - 1]->map);
            delete cache[CACHE_SIZE - 1];
            for (int i = (CACHE_SIZE - 1); i > 0; i--)
                cache[i] = cache[i - 1];
            cache[0] = new T;
            *cache[0] = id;
        }
        LeaveCriticalSection(&cs);
    };

    int get_item_index(const std::string& f)
    {
        if (state_valid)
            for (int i = 0; i < CACHE_SIZE; i++)
                if ((cache[i]->exists) && (cache[i]->file == f))
                    return i;
        return -1;
    };

    void update(const std::string& file)
    {
        if (state_valid && (file == current_file))
            PostMessage(hWnd_main, WM_APP_ITEMCACHE_UPDATE_BK, (WPARAM)wnd, 0);
    };

    bool isValid()
    {
        return state_valid;
    };

    void invalidate()
    {
        EnterCriticalSection(&cs);
        if (state_valid)
        {
            state_valid = false;
            PostThreadMessage(tid, WM_APP_ITEMCACHE_STOP, 0, 0);
            for (int i = 0; i < CACHE_SIZE; i++)
            {
                //if (cache[i]->map)
                    //DeleteObject(cache[i]->map);
                delete cache[i];
                cache[i] = NULL;
            }
            wnd = NULL;
            x = 0;
            y = 0;
            tid = 0;
            current_file = "";
        }
        LeaveCriticalSection(&cs);
    };

    void set_current_file(const std::string& f)
    {
        current_file = f;
    };

    HANDLE get_worker_thread_event()
    {
        return worker_thread_event;
    };

    int getX() { return x; };
    int getY() { return y; };

    void report_size()
    {
        int size = 0;
        EnterCriticalSection(&cs);
        if (state_valid)
        {
            for (int i = 0; i < CACHE_SIZE; i++)
                if (cache[i]->exists)
                    size++;
        }
        LeaveCriticalSection(&cs);
#ifdef _DEBUG
        log("Cache size = %d", size);
#endif
    };
};

HBITMAP make_bitmap(int x, int y, int view_x, int view_y, TILE* map);
HBITMAP make_bitmap_for_scen(int view_x, int view_y, SCEN& scen);

extern HWND hWnd_main;

template<class T>
void init_item(T& d, const std::string& filename)
{
    d.exists = true;
    d.file = filename;
}

void insert_item_data(REC_DATA& rd, ITEM_CACHE<REC_DATA>& cache)
{
    MG1 mg1(rd.file.c_str());

    if (mg1.loaded)
    {
        rd.valid = true;
        rd.duration = mg1.d.duration;
        rd.start_time = mg1.d.start_time;

        rd.map = std::shared_ptr<HBITMAP_HOLDER>(new HBITMAP_HOLDER(make_bitmap(mg1.d.map_x, mg1.d.map_y, cache.getX(), cache.getY(), mg1.d.map)));

        TEAM team = mg1.getTeam1();
        rd.n_team1 = team.n;
        for (int i = 0; i < team.n; i++)
        {
            rd.team_1[i] = (std::string)team.names[i];
            rd.team_1_colors[i] = team.colors[i];
            rd.team_1_civs[i] = team.civs[i];
            rd.team_1_cc_x[i] = team.cc_x[i];
            rd.team_1_cc_y[i] = team.cc_y[i];
        }

        team = mg1.getTeam2();
        rd.n_team2 = team.n;
        for (int i = 0; i < team.n; i++)
        {
            rd.team_2[i] = (std::string)team.names[i];
            rd.team_2_colors[i] = team.colors[i];
            rd.team_2_civs[i] = team.civs[i];
            rd.team_2_cc_x[i] = team.cc_x[i];
            rd.team_2_cc_y[i] = team.cc_y[i];
        }
        rd.map_x = mg1.d.map_x;
        rd.version = mg1.version;
    }
    else
        rd.valid = false;

    cache.add_item(rd);
    cache.update(rd.file);

    //log(cache.)
    cache.report_size();
}

void insert_item_data(SCEN_DATA& sd, ITEM_CACHE<SCEN_DATA>& cache)
{
    SCEN* sc1 = new SCEN(sd.file.c_str());

    if (sc1->loaded)
    {
        sd.valid = true;
        sd.map = std::shared_ptr<HBITMAP_HOLDER>(new HBITMAP_HOLDER(make_bitmap_for_scen(cache.getX(), cache.getY(), *sc1)));
        for (int i = 0; i < 16; i++)
        {
            sd.active[i] = sc1->header2.player_data1[i].active;
            sd.human[i] = sc1->header2.player_data1[i].human;
        }
        sd.timestamp = sc1->header.timestamp;
        sd.trigger_count = sc1->triggers.number_of_triggers;
        sd.instructions = sc1->header.instructions.str ? sc1->header.instructions.str : "";
        sd.version = sc1->header2.version2;
        sd.orig_filename = sc1->header2.filename.str ? sc1->header2.filename.str : "";
    }
    else
        sd.valid = false;

    delete sc1;

    cache.add_item(sd);
    cache.update(sd.file);
}

template<class T>
unsigned int __stdcall item_cache_thread(ITEM_CACHE<T>* item_cache)
{
    //initialize worker thread and set event
    MSG msg;
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(item_cache->get_worker_thread_event());

    std::vector<std::pair<std::string, int>> priority_queue;
    while (true)
    {
        //check message queue
        while (PeekMessage(&msg, nullptr, WM_APP_ITEMCACHE_STOP, WM_APP_ITEMCACHE_STOP, PM_REMOVE))
        {
            switch (msg.message)
            {
            case WM_APP_ITEMCACHE_STOP: //end
                priority_queue.clear();
                return 0;
                break;
            default:
                break;
            }
        }

        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            //process messages if available
            switch (msg.message)
            {
            case WM_APP_ITEMCACHE_FILE: //file
            {
                int i = 0;
                std::string file((char*)msg.wParam);
                //check if already exists
                for (i = 0; i < priority_queue.size(); i++)
                    if (priority_queue[i].first == file)
                        break;
                //remove old if found
                if (i < priority_queue.size())
                    priority_queue.erase(priority_queue.begin() + i);

                priority_queue.emplace_back(std::pair<std::string, int>(file, (int)msg.lParam));
                free((void*)msg.wParam);
                break;
            }
            case WM_APP_ITEMCACHE_STOP: //end
                priority_queue.clear();
                return 0;
                break;
            default:
                break;
            }
        }

        if (priority_queue.size() > 0)
        {
            //find last priority message
            int found_at = -1;
            for (int i = priority_queue.size() - 1; i >= 0; i--)
                if (priority_queue[i].second == 2)
                {
                    if (found_at == -1)
                        found_at = i;
                    else  //older high priority selections are no longer focused -> low priority
                        priority_queue[i].second = 0;
                }
            //find medium priority
            if (found_at == -1)
                for (int i = priority_queue.size() - 1; i >= 0; i--)
                    if (priority_queue[i].second == 1)
                    {
                        found_at = i;
                        break;
                    }
            //find low priority
            if (found_at == -1)
                for (int i = priority_queue.size() - 1; i >= 0; i--)
                    if (priority_queue[i].second == 0)
                    {
                        found_at = i;
                        break;
                    }
            //if found anything
            if (found_at != -1)
            {
                auto it = priority_queue.begin() + found_at;

                T id;
                init_item(id, it->first);
                priority_queue.erase(it);

                insert_item_data(id, *item_cache);
            }
            else
                Sleep(10);
        }
        else
            Sleep(10);

        //truncate the queue
        if (priority_queue.size() > CACHE_SIZE)
        {
            //remove priority 0 first
            std::vector<int> indices;
            for (int i = 0; i < priority_queue.size(); i++)
                if ((priority_queue.size() - indices.size()) > (CACHE_SIZE - 10))
                {
                    if (priority_queue[i].second == 0)
                        indices.push_back(i);
                }
                else
                {
                    break;
                }
            for (int i = (indices.size() - 1); i >= 0; i--)
                priority_queue.erase(priority_queue.begin() + i);
        }
    }
    return 0;
}
