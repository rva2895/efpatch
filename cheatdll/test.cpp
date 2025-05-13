#include "stdafx.h"

#include "test.h"
#include "log.h"
#include "advcheat.h"
#include "rundll.h"
#include "oos.h"
#include "functionlist.h"
#include "advtriggereffect.h"
#include "worlddump.h"
#include "rmslog.h"
#include "resfile.h"

#include <process.h>
#include <MMSystem.h>
#include <vector>
#include <string>
#include <map>
#include <time.h>
#include <algorithm>

#include "resfile.h"

//extern RESFILE resfile;

#ifdef _DEBUG

int effectsFired = 0;
int triggerBegun = 0;
char triggerText[256];

void __cdecl effectLog(int n, char* ef)
{
    if (triggerBegun)
    {
        log("%s", triggerText);
        triggerBegun = 0;
    }
    log("---> Effect %d (%d)", n, *(int*)(ef + 4));
}

__declspec(naked) int effectLogHook() //005F4A77
{
    __asm
    {
        mov     eax, [edx]
        mov     ecx, [ecx + edi * 4]
        push    eax
        push    ecx
        push    edi
        call    effectLog
        pop     edi
        pop     ecx
        push    005F4A7Dh
        ret
    }
}

void __cdecl triggerLog(int n, char* tr)
{
    triggerBegun = 1;

    char szName[] = "Untitled";
    //*szName = 0;
    char* name;
    if (*(char**)(tr + 0x14))
        name = *(char**)(tr + 0x14);
    else
        name = szName;

    if (*tr == 1)
        snprintf(triggerText, _countof(triggerText), "Trigger %d (name: %s; %d conditions, %d effects", n,
            name, *(int*)(tr + 0x20), *(int*)(tr + 0x30));
}

__declspec(naked) int triggerLogHook() //005F54D2
{
    __asm
    {
        mov     ecx, [esi + 10h]
        push    ebx
        mov     ecx, [ecx + edi * 4]
        push    ecx
        push    edi
        call    triggerLog
        pop     edi
        pop     ecx
        push    005F54D9h
        ret
    }
}
#endif

/*
int retSave;
int data;

const char title[] = "DAT";
const char text[] = "read";

__declspec(naked) int onReadDat()
{
    __asm
    {
        mov     ecx, data
        mov     ecx, [ecx]
        cmp     cx, 9852
        jnz     _end
        //int     3
        //push    eax
        //push    0
        //push    offset title
        //push    offset text
        //push    0
        //call    MessageBoxA
        //pop     eax
_end:
        mov     ecx, retSave
        jmp     ecx
    }
}

__declspec(naked) int readDatHook() //004D5550
{
    __asm
    {
        mov     eax, [esp]
        mov     retSave, eax
        mov     data, edx
        mov     eax, onReadDat
        mov     [esp], eax
        push    ebp
        mov     ebp, esp
        sub     esp, 0Ch
        push    004D5556h
        ret
    }
}
*/

char* log_int_s = NULL;

__declspec(noinline) void __cdecl log_int(int unk1, char* fmt, ...)
{
    UNREFERENCED_PARAMETER(unk1);
    va_list ap;

    if (!log_int_s)
        return;
    
    if (!(((unsigned long)fmt >= 0x00689000) && ((unsigned long)fmt < 0x7A3A2C)))
        return;

    size_t len = strlen(fmt);

    if ((len < 8) || (len > 0x180))
        return;

    if (!strncmp("Act_Art", fmt, 7))
        return;

    va_start(ap, fmt);
    vsnprintf(log_int_s, LOG_INT_BUFFER_SIZE, fmt, ap);
    log_internal("%s", log_int_s);

    do_rms_logging(log_int_s);

    va_end(ap);
}

void* log_int_retval;

__declspec(naked) void log_int_end_wr()
{
    __asm
    {
        mov     eax, log_int_retval
        sub     esp, 4
        jmp     eax
    }
}

__declspec(naked) void log_int_wr()
{
    __asm
    {
        mov     eax, [esp + 8]
        cmp     eax, 00689000h
        jb      fix_log_ret
        cmp     eax, 007A3A2Ch
        jnb     fix_log_ret
        jmp     log_int
fix_log_ret:
        pop     eax
        mov     log_int_retval, eax
        mov     eax, log_int_end_wr
        mov     [esp], eax
        jmp     log_int
        ret
    }
}

__declspec(naked) void nullsub_26_2()
{
    __asm
    {
        ret
    }
}

/*
int check_file(char* file, int player)
{
    char s[0x100];
    snprintf(s, _countof(s), "Checking %s...", file);
    sendChat(s, player);
    FILE* f = fopen(file, "rb");
    if (!f)
    {
        snprintf(s, _countof(s), "Failed to open %s!", file);
        sendChat(s, player);
        return 0;
    }
    char b;
    char sum = 0;
    while (fread(&b, 1, 1, f) > 0)
        sum += b;
    fclose(f);
    snprintf(s, _countof(s), "File %s: %d", file, sum);
    sendChat(s, player);
    return sum;
}

void thread_proc(void* p)
{
    int player = (int)p;
    int sum = 0;
    sum += check_file("battlegrounds_x1.exe", player);
    sum += check_file(DLL_NAME, player);
    sum += check_file("language.dll", player);
    sum += check_file("language_x1.dll", player);
    sum += check_file("language_x2.dll", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"genie_x2.dat", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"gamedata.drs", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"gamedata_x1.drs", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"gamedata_x2.drs", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"expl.txt", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"ground-to-air.txt", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"jedi-holo.txt", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"master.txt", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"padawan.txt", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"resgen.txt", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"unconv.txt", player);
    sum += check_file(DATA_FOLDER_PREFIX_FROM_ROOT"terrain.txt", player);
    char s[0x100];
    sprintf(s, "Integrity check complete, checksum = %d", sum);
    sendChat(s, player);
}
*/

//extern int memory_temp;

//DWORD performance_time = 0;

//extern void __stdcall make_oos_dump();

//extern std::vector<FUNCTION_HOOK*> function_hooks;

struct
{
    bool operator() (FUNCTION_HOOK* fh1, FUNCTION_HOOK* fh2) const
    {
        return !(*fh1 < *fh2);
    }
} compare_call_counts_object;

extern unsigned int dump_objects(const char* filename);
extern int max_worldtime;

void __stdcall RGE_Action_List__add_action2(RGE_Action_List* list, RGE_Action* action)
{
    RGE_Action_Node* node;

    if (list->obj->object_state <= 2)
    {
        node = (RGE_Action_Node*)calloc_internal(1, 8);
        node->action = action;
        node->next = list->list;
        list->list = node;
        //action->vfptr->first_in_stack(action, 1u);
    }
    else if (action)
    {
        action->vfptr->__vecDelDtor(action, 1u);
    }
}

int* rand_val = (int*)0x0069EC98;

int rand_no_change()
{
    int rand_save = *rand_val;
    int result = rand_internal();
    *rand_val = rand_save;
    return result;
}

struct object_save_info
{
    int id;
    float originX;
    float originY;
    float originZ;
};

class save_info
{
private:

    //void saveObject()

public:
    int save_pos;
    int save_sync_counter;
    int save_rand_val;
    unsigned int save_worldtime;
    std::vector<object_save_info> save_objects;
    std::string filename;

    void saveObjects(RGE_Game_World* world)
    {
        world->players[0]->objects->List;
        for (int i = 0; i < world->players[0]->objects->Number_of_objects; i++)
        {
            UnitAIModule* unit_ai = world->players[0]->objects->List[i]->unitAIValue;
            if (unit_ai && (unit_ai->vfptr == (UnitAIModuleVtbl*)0x00665E28 || unit_ai->vfptr == (UnitAIModuleVtbl*)0x00665D3C))
            {
                TribeHuntedAnimalUnitAIModule* unit_ai_origins = (TribeHuntedAnimalUnitAIModule*)unit_ai;
                object_save_info osi;
                osi.id = world->players[0]->objects->List[i]->id;
                osi.originX = unit_ai_origins->originX;
                osi.originY = unit_ai_origins->originY;
                osi.originZ = unit_ai_origins->originZ;
                save_objects.emplace_back(osi);
            }
        }
    }

    void loadObjects(RGE_Game_World* world)
    {
        for (auto it = save_objects.begin(); it != save_objects.end(); ++it)
        {
            object_save_info& osi = *it;
            RGE_Static_Object* obj = RGE_Game_World__object(world, osi.id);
            UnitAIModule* unit_ai = obj->unitAIValue;
            if (unit_ai && (unit_ai->vfptr == (UnitAIModuleVtbl*)0x00665E28 || unit_ai->vfptr == (UnitAIModuleVtbl*)0x00665D3C))
            {
                TribeHuntedAnimalUnitAIModule* unit_ai_origins = (TribeHuntedAnimalUnitAIModule*)unit_ai;
                unit_ai_origins->originX = osi.originX;
                unit_ai_origins->originY = osi.originY;
                unit_ai_origins->originZ = osi.originZ;
            }
        }
    }
};

std::map<int, save_info> saved_states;

void save_state(RGE_Game_World* world)
{
    save_info si;
    //si.save_rand_val = *rand_val;
    si.save_worldtime = world->world_time;

    char filename[MAX_PATH];
    snprintf(filename, _countof(filename), "tempsave_rec_%d.tmp", world->world_time);
    si.filename = filename;

    int file = rge_open2(filename, 0x8301, 0x180);
    RGE_Game_World__save_to_open_file((RGE_Game_World*)world, file);
    rge_close(file);

    si.save_rand_val = *rand_val;

    si.save_pos = tell_internal(world->com_handler->mCommandLog->mFileHandle);
    si.save_sync_counter = world->com_handler->mCommandLog->mSyncCounter;
    si.saveObjects(world);

    saved_states.emplace(world->world_time, si);
}

void load_state(RGE_Game_World* world)
{
    auto it = saved_states.begin();
    if (it == saved_states.end())
        return;

    save_info& si = (*it).second;

    int file = rge_open((char*)si.filename.c_str(), 0x8000);
    RGE_Game_World__load_from_open_file((RGE_Game_World*)world, file);
    rge_close(file);
    lseek_internal(world->com_handler->mCommandLog->mFileHandle, si.save_pos, 0);

    world->com_handler->mCommandLog->mSyncCounter = si.save_sync_counter;

    *rand_val = si.save_rand_val;

    si.loadObjects(world);
}

void del_states()
{
    saved_states.clear();
}

extern int path_stats[100];

struct path_stats_struct
{
    int object_group;
    int count;
    float fraction;
};

bool compare(const path_stats_struct& first, const path_stats_struct& second)
{
    return first.fraction > second.fraction;
}

void calc_path_stats()
{
    std::vector<path_stats_struct> path_stats_vector;
    int count_total = 0;
    for (int i = 1; i <= 64; i++)
    {
        path_stats_struct pt;
        pt.object_group = i;
        pt.count = path_stats[i];
        count_total += pt.count;
        path_stats_vector.push_back(pt);
    }
    for (int i = 0; i < path_stats_vector.size(); i++)
    {
        path_stats_vector[i].fraction = (float)path_stats_vector[i].count / count_total;
    }
    std::sort(path_stats_vector.begin(), path_stats_vector.end(), compare);

    chat("Top 5 classes: %d (%.02f%%), %d (%.02f%%), %d (%.02f%%), %d (%.02f%%), %d (%.02f%%)",
        path_stats_vector[0].object_group, path_stats_vector[0].fraction * 100,
        path_stats_vector[1].object_group, path_stats_vector[1].fraction * 100,
        path_stats_vector[2].object_group, path_stats_vector[2].fraction * 100,
        path_stats_vector[3].object_group, path_stats_vector[3].fraction * 100,
        path_stats_vector[4].object_group, path_stats_vector[4].fraction * 100);
}

void verify_unitlines()
{
    int p = 0;

    int x = offsetof(TRIBE_Game, timings[6].accum_time);
    x = offsetof(TRIBE_Game, timings[7].accum_time);
    x = offsetof(TRIBE_Game, timings[21].accum_time);
    x = offsetof(TRIBE_Game, timings[3].accum_time);
    x = offsetof(TRIBE_Game, timings[1].accum_time);
    x = offsetof(TRIBE_Game, timings[14].accum_time);
    x = offsetof(TRIBE_Game, timings[12].accum_time);
    x = offsetof(TRIBE_Game, timings[10].accum_time);
    x = offsetof(TRIBE_Game, timings[15].accum_time);
    x = offsetof(TRIBE_Game, timings[16].accum_time);
    x = offsetof(TRIBE_Game, timings[13].accum_time);
    x = offsetof(TRIBE_Game, timings[4].accum_time);
    x = offsetof(TRIBE_Game, timings[17].accum_time);

    TRIBE_World* world = (*base_game)->world;
    TRIBE_Player* player = (TRIBE_Player*)RGE_Base_Game__get_player_by_id((*base_game), 2); //(*base_game)->world->players[0];

    RGE_Master_Static_Object** player_master_objects = player->master_objects;
    int num_master_objects = player->master_object_num;

    RGE_Master_Static_Object** gaia_master_master_objects = world->master_players[0]->master_objects;

    //verify unitlines
    for (int i = 0; i < world->gbg_numberUnitlines; i++)
    {
        for (int j = 0; j < world->gbg_unitlines[i].numberUnits; j++)
        {
            int obj_id = world->gbg_unitlines[i].units[j];
            if (!RGE_Player__isMasterObjectAvailable((RGE_Player*)player, obj_id) ||
                player_master_objects[obj_id]->unit_line != -i)
            {
                ;// chat("Unitline %d: bad obj %d", i, obj_id);
            }

            if (gaia_master_master_objects[obj_id] &&
                gaia_master_master_objects[obj_id]->unit_line != i)
            {
                chat("Unitline %d: obj %d", i, obj_id);
            }
        }
    }

    /*for (int i = 0; i < world->player_num; i++)
        for (int j = 0; j < world->players[i]->master_object_num; j++)
        {
            if (RGE_Player__isMasterObjectAvailable((RGE_Player*)world->players[i], j) &&
                world->players[i]->master_objects[j]->unit_line != -12851)
            {
                chat("Unitline %d: obj %d", i, j);
            }
        }
    */
}

int __stdcall onChat_2(int player_id, char* targets, char* s)
{
    UNREFERENCED_PARAMETER(targets);
    UNREFERENCED_PARAMETER(player_id);
    
    if (!strcmp(s, "/version"))
    {
        sendChat(EFPATCH_VERSION, -1);
        return 1;
    }
    /*
    if (!strcmp(s, "/verify-unitlines"))
    {
        verify_unitlines();
        return 1;
    }
    */
    /*
    else if (!strcmp(s, "/obj-count"))
    {
        RGE_Player* player = RGE_Base_Game__get_player(*base_game);
        chat("Objects: %d, Sleeping: %d, Doppleganger: %d",
            player->objects->Number_of_objects,
            player->sleeping_objects->Number_of_objects,
            player->doppleganger_objects->Number_of_objects);
        return 1;
    }
    else if (!strcmp(s, "/pathing-stats"))
    {
        calc_path_stats();
        return 1;
    }
    */
    /*
    else if (!strcmp(s, "/power"))
    {
        FILE* f = fopen("power.txt", "wt");
        RGE_Master_Static_Object obj;
        for (int i = 0; i < 1000; i++)
        {
            obj.id = i;
            int x = RGE_Master_Static_Object__gbg_needs_power(&obj);
            if (x) fprintf(f, "%d - %d\n", i, x);
        }
        fclose(f);
        return 1;
    }
    */
    /*
    else if (!strcmp(s, "/resfile"))
    {
        resfile.calc();
        return 1;
    }
    else if (!strcmp(s, "/resfile-gc"))
    {
        resfile.collect_garbage();
        return 1;
    }
    */
    /*
    else if (!strcmp(s, "/del-save"))
    {
        del_states();

        return 1;
    }
    else if (!strcmp(s, "/save"))
    {
        TRIBE_World* world = (*base_game)->world;

        save_state((RGE_Game_World*)world);

        //int x = rand_internal();

        return 1;
    }
    else if (!strcmp(s, "/load"))
    {
        TRIBE_World* world = (*base_game)->world;
        TRIBE_Screen_Game* game_screen = ((TRIBE_Game*)(*base_game))->game_screen;
        RGE_Player* player = RGE_Base_Game__get_player(*base_game);
        int id = player->id;
        float view_x = player->view_x;
        float view_y = player->view_y;

        TRIBE_Screen_Game__unhook_world(game_screen);

        world->vfptr->del_game_info(world);

        load_state((RGE_Game_World*)world);

        TRIBE_Screen_Game__rehook_world(game_screen);

        (*base_game)->vfptr->set_player(*base_game, id);
        player = RGE_Base_Game__get_player(*base_game);
        RGE_Player__set_view_loc(player, view_x, view_y, 0);
        RGE_Player__set_map_loc(player, view_x, view_y);

        TPanel* panel = TPanelSystem__getTop(panel_system);
        panel->vfptr->set_redraw(panel, 1);

        //int x = rand_internal();

        return 1;
    }
    else if (strstr(s, "/update"))
    {
        char d[0x100];
        int t;
        sscanf(s, "%s %d", d, &t);

        int* const do_fixed_update = (int* const)0x006A35E8;
        int do_fixed_update_old = *do_fixed_update;
        *do_fixed_update = 1;
        unsigned int start_wt = (*base_game)->world->world_time;
        unsigned int current_wt = start_wt;

        TCommCommandLog* log = (*base_game)->world->com_handler->mCommandLog;

        int old_speed = log->mReplaySpeed;
        log->mReplaySpeed = 12;

        unsigned int last_update_time = timeGetTime();

        unsigned __int8 game_state = (*base_game)->world->game_state;
        while (game_state == 0 && current_wt - start_wt < t * 1000)
        {
            game_state = (*base_game)->world->vfptr->update((*base_game)->world);
            current_wt = (*base_game)->world->world_time;

            TMessagePanel* message_panel; // [esp+0h] [ebp-20h]
            struct tagMSG Msg; // [esp+4h] [ebp-1Ch] BYREF

            unsigned int current_update_time = timeGetTime();

            if (current_update_time - last_update_time > 200)
            {
                last_update_time = current_update_time;
                while (PeekMessageA(&Msg, 0, 0, 0, 1u))
                {
                    TranslateMessage(&Msg);
                    DispatchMessageA(&Msg);
                }
                TRIBE_Screen_Game* game_screen = ((TRIBE_Game*)(*base_game))->game_screen;
                message_panel = NULL;
                if (game_screen)
                {
                    TPanel* main_view = (TPanel*)game_screen->main_view;
                    //if (main_view->have_focus)
                    {
                        message_panel = game_screen->message_panel;
                        TPanel__set_redraw(main_view, 1);

                        std::string message = "Seeking...";

                        TMessagePanel__show_message(message_panel, 1, message.c_str(), 0xF3u, 0, 0, 0, 0, -1, 0, -1);
                    }
                }
            }

            TRIBE_Screen_Game* game_screen = ((TRIBE_Game*)(*base_game))->game_screen;
            message_panel = NULL;
            if (game_screen)
            {
                TPanel* main_view = (TPanel*)game_screen->main_view;
                if (main_view->have_focus)
                {
                    message_panel = game_screen->message_panel;
                    TPanel__set_redraw(main_view, 1);
                }
            }
        }

        //for (int i = 0; i < 10000; i++)
        //    game_state = (*base_game)->world->vfptr->update((*base_game)->world);

        log->mReplaySpeed = old_speed;

        //for (int i = 0; i < 1000; i++)
        //    game_state = (*base_game)->world->vfptr->update((*base_game)->world);

        *do_fixed_update = do_fixed_update_old;
        //chat("t = %d", t);
        return 1;
    }
    */
    /*else if (strstr(s, "/control"))
    {
        char d[0x100];
        int p;
        sscanf(s, "%s %d", d, &p);
        if ((p >= 0) && (p <= 8))
        {
            RGE_Player__unselect_object(RGE_Base_Game__get_player(*base_game));
            TRIBE_Game__set_player(*(TRIBE_Game**)base_game, p);
        }
        return 1;
    }*/
    /*else if (!strcmp(s, "/test"))
    {
        sendChat("Started integrity check", player_id);
        _beginthread(thread_proc, 0, (void*)player_id);
        return 1;
    }
    else if (!strcmp(s, "/memory"))
    {
        sendChat("Memory: set to 0x100000", player_id);
        memory_temp = 0x100000;
        return 1;
    }*/
    /*
    else if (!strcmp(s, "/dump-world"))
    {
        srand(timeGetTime());
        unsigned int r = rand();
        char name[MAX_PATH];
        sprintf(name, "rge_dump_%08X.txt", r);
        chat("Dumping world to %s ...", name);
        dump_objects(name);
        chat("Dump complete");
        return 1;
    }
    else if (!strcmp(s, "/worldtime"))
    {
        chat("Worldtime = %u", get_worldtime());
        return 1;
    }
    else if (strstr(s, "/set-max"))
    {
        char d[0x100];
        int t;
        sscanf(s, "%s %d", d, &t);
        max_worldtime = t;
        chat("Set max worldtime to %d", t);
        return 1;
    }
    */
    /*
    else if (strstr(s, "/del-sprite-list"))
    {
        TRIBE_World* world = (*base_game)->world;
        if (world)
        {
            RGE_Player* player = RGE_Base_Game__get_player(*base_game);
            if (player)
            {
                RGE_Static_Object* obj = player->selected_obj;
                if (obj)
                {
                    RGE_Active_Sprite_List__delete_list(obj->sprite_list);
                }
                else
                    chat("No objects selected");
            }
        }

        return 1;
    }
    */
    /*
    else if (!strcmp(s, "/make-oos"))
    {
        float* r = player_getResources2(get_player(0));
        r[0] += 100.0f;
        chat("Caused out of sync");
        return 1;
    }
    else if (!strcmp(s, "/dump-all"))
    {
        make_oos_dump();
        return 1;
    }
    */
    /*
    else if (strstr(s, "/obj") || strstr(s, "/object"))
    {
        char d[0x100];
        int id;
        sscanf(s, "%s %d", d, &id);
        TRIBE_World* world = (*base_game)->world;
        if (world)
        {
            RGE_Static_Object* unit = RGE_Game_World__object((RGE_Game_World*)world, id);
            if (unit)
            {
                RGE_Player* player = RGE_Base_Game__get_player(*base_game);
                RGE_Player__unselect_object(player);
                RGE_Player__select_object(player, unit, 1);
            }
            else
                chat("Invalid object id");
        }

        return true;
    }
    else if (strstr(s, "/sel-id"))
    {
        TRIBE_World* world = (*base_game)->world;
        if (world)
        {
            RGE_Player* player = RGE_Base_Game__get_player(*base_game);
            if (player)
            {
                RGE_Static_Object* obj = player->selected_obj;
                if (obj)
                    chat("Selected object %d, 0x%X", obj->id, (DWORD)obj);
                else
                    chat("No objects selected");
            }
        }

        return true;
    }
    else if (strstr(s, "/goto"))
    {
        char d[0x100];
        float x, y;
        sscanf(s, "%s %f %f", d, &x, &y);
        RGE_Player__set_view_loc(RGE_Base_Game__get_player(*base_game), x, y, 0);
        return true;
    }
    */
    /*else if (strstr(s, "/load-all"))
    {
        int ext_types[] = {0x736C7020, 0x77617620, 0x62696E61};
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 100000; j++)
            {
                int size;
                resfile.get_resource(ext_types[i], j, &size);
            }
        }
        chat("Loaded all DRS resources");
        return 1;
    }*/
    /*
    else if (strstr(s, "/line"))
    {
        void(__thiscall * RGE_Obstruction_Manager_Land__AddDebugLine)(RGE_Obstruction_Manager_Land * this_, float X1, float Y1, float X2, float Y2, int Color, int DebugLine) =
            (void(__thiscall*)(RGE_Obstruction_Manager_Land*, float, float, float, float, int, int))0x004B42C0;

        RGE_Obstruction_Manager_Land* ob = (RGE_Obstruction_Manager_Land*)0x006ACCDC;

        RGE_Obstruction_Manager_Land__AddDebugLine(ob, 1, 1, 3, 5, 57, 0);

        return 1;
    }
    */
    /*
    else if (strstr(s, "/cs"))
    {
        WORLD_DUMP wd;
        wd.update_cs();
        chat("CS=%u", wd.get_cs());
        return true;
    }
    /*else if (!strcmp(s, "/random-test"))
    {
        chat("Testing random generator...");
        do_random_test();
        return 1;
    }*/
    /*else if (!strcmp(s, "/set-zero-count"))
    {
        for (auto it = function_hooks.begin(), it_end = function_hooks.end(); it != it_end; ++it)
            (*it)->set_zero_call_count();
        chat("Set call counts to zero");
        return 1;
    }
    else if (!strcmp(s, "/print-call-count"))
    {
        srand(timeGetTime());
        unsigned int r = rand();
        char name[MAX_PATH];
        sprintf(name, "call_count_%08X.txt", r);
        //chat("Sorting vector ...");
        std::sort(function_hooks.begin(), function_hooks.end(), compare_call_counts_object);
        FILE* f = fopen(name, "wt");
        if (f)
        {
            for (auto it = function_hooks.begin(), it_end = function_hooks.end(); it != it_end; ++it)
                fprintf(f, "0x%X - %lld - %lld ns\n", (*it)->get_address(), (*it)->get_call_count(), (*it)->get_total_time());
            fclose(f);
            chat("Printed call counts to %s", name);
        }
        else
            chat("Error writing file!");
        return 1;
    }*/
    /*else if (!strcmp(s, "/time-start"))
    {
        performance_time = timeGetTime();
        chat("Set start time");
        return 1;
    }
    else if (!strcmp(s, "/time-stop"))
    {
        chat("Elapsed %d ms", timeGetTime() - performance_time);
        return 1;
    }
    else if (!strcmp(s, "/render-off"))
    {
        chat("Render OFF");
        unsigned char instr = 0xC3;
        SIZE_T w;
        WriteProcessMemory(GetCurrentProcess(), (void*)0x00651BC0, &instr, 1, &w);
        return 1;
    }
    else if (!strcmp(s, "/render-on"))
    {
        chat("Render ON");
        unsigned char instr = 0x55;
        SIZE_T w;
        WriteProcessMemory(GetCurrentProcess(), (void*)0x00651BC0, &instr, 1, &w);
        return 1;
    }*/
    /*else if (strstr(s, "/action"))
    {
        TRIBE_World* world = (*base_game)->world;
        if (world)
        {
            RGE_Player* player = RGE_Base_Game__get_player(*base_game);
            if (player)
            {
                RGE_Action_Object* obj = (RGE_Action_Object*)player->selected_obj;
                if (obj)
                    //chat("Selected object %d, 0x%X", obj->id, (DWORD)obj);
                {
                    RGE_Master_Action_Object* master = obj->master_obj;
                    RGE_Sprite* sprite = master->vfptr->gbg_get_run_sprite_civ_override(master, (RGE_Static_Object*)obj);
                    
                    if (!sprite)
                        sprite = master->vfptr->gbg_get_move_sprite_civ_override(master, (RGE_Static_Object*)obj);

                    RGE_Action_Move_To* act = (RGE_Action_Move_To*)calloc_internal(1, sizeof(RGE_Action_Move_To));
                    RGE_Action_Move_To__RGE_Action_Move_To(act, (RGE_Action_Object*)obj, 2, 2, 0, 0, sprite);
                    RGE_Action_List__add_action(obj->actions, (RGE_Action*)act);

                    RGE_Action_Move_To* act2 = (RGE_Action_Move_To*)calloc_internal(1, sizeof(RGE_Action_Move_To));
                    RGE_Action_Move_To__RGE_Action_Move_To(act2, (RGE_Action_Object*)obj, 4, 5, 0, 0, sprite);
                    RGE_Action_List__add_action(obj->actions, (RGE_Action*)act2);
                    //RGE_Action__setSubAction((RGE_Action*)act2, 1);
                    char name[0x100];
                    if (obj->actions->list)
                    {
                        obj->actions->list->action->vfptr->get_state_name(obj->actions->list->action, name);
                        chat("Act = %d, state = %s", obj->actions->list->action->action_type, name);
                    }
                    else
                        chat("No actions");
                }
                else
                    chat("No objects selected");
            }
        }

        return 1;
    }*/
    else
        return 0;
}

__declspec(naked) void onChat() //00438140
{
    __asm
    {
        push    ecx
        mov     eax, [esp + 10h]
        push    eax
        mov     eax, [esp + 10h]
        push    eax
        mov     eax, [esp + 10h]
        push    eax
        call    onChat_2
        pop     ecx
        test    eax, eax
        jnz     __skip_chat
        sub     esp, 118h
        mov     eax, 00438146h
        jmp     eax
__skip_chat:
        xor     eax, eax
        ret     0Ch
    }
}

void __stdcall rms_token_log(char char5, char char4, char char3, char char2, int id, char char1, char* token, void* caller)
{
    log("RMS: (%d, %d, %d, %d, %d) - @0x%p: %d = %s", (int)char1, (int)char2, (int)char3, (int)char4, (int)char5, caller, id, token);
}

__declspec(naked) void onRmsToken() //004E1D40
{
    __asm
    {
        push    ebp
        mov     ebp, esp
        push    ecx
        mov     eax, [ebp + 4] //caller
        mov     ecx, [ebp + 8] //token
        push    eax
        push    ecx
        mov     eax, [ebp + 0Ch] //char1
        mov     ecx, [ebp + 10h] //id
        push    eax
        push    ecx
        mov     eax, [ebp + 14h] //char2
        mov     eax, [ebp + 18h] //char3
        push    eax
        push    ecx
        mov     eax, [ebp + 1Ch] //char4
        mov     eax, [ebp + 20h] //char5
        push    eax
        push    ecx
        call    rms_token_log
        pop     ecx
        leave

        push    ebx
        push    ebp
        push    esi
        push    edi
        mov     edi, [esp + 14h]
        mov     eax, 004E1D48h
        jmp     eax
    }
}

void fix_function_call(DWORD addr, DWORD f)
{
    writeDword(addr, f - (addr + 4));
}

/*
std::vector<void*> addresses;

std::vector<void*> adr_test;

void __stdcall add_addr(void* a)
{
    for (auto it = addresses.begin(); it != addresses.end(); ++it)
        if (*it == a)
            return;
    addresses.push_back(a);
}

extern bool time_collect;
extern bool time_stage_find;

int left;
int right;

volatile bool answer_flag = false;
volatile bool answer = false;

void msg_box(const char* s, int formal)
{
    UNREFERENCED_PARAMETER(formal);
    MessageBox(0, s, "Message", 0);
}

bool check(int l, int r)
{
    left = l;
    right = r;
    answer_flag = false;
    char s[0x100];
    sprintf(s, "Checking %d - %d, condition met? [/yes] [/no]", l, r);
    msg_box(s, 0);
    while (!answer_flag)
        ;
    return answer;
}

// A iterative binary search function. It returns 
// location of x in given array arr[l..r] if present, 
// otherwise -1 
int search_function(const std::vector<void*>& arr)
{
    int l = 0;
    int r = arr.size() - 1;
    while (l <= r)
    {
        int m = l + (r - l) / 2;

        // Check if x is present at mid 
        
        if (check(m, m))
            return m;

        // If x greater, ignore left half 
        if (check(m, r))
            l = m + 1;

        // If x is smaller, ignore right half 
        else
            r = m - 1;
    }

    // if we reach here, then element was 
    // not present 
    return -1;
}

DWORD adr_time;
BYTE jump_time[6];
BYTE old_time[6];

DWORD adr_text;
BYTE jump_text[6];
BYTE old_text[6];

DWORD last_call = 0;

int __stdcall Intercept_DrawTextA(HDC hdc, LPCSTR s, int c, LPRECT r, UINT format)
{
    //DrawTextA()
    DWORD written;
    WriteProcessMemory(GetCurrentProcess(), (void*)adr_text,
        (void*)old_text, 6, &written);

    if (c == -1)
        c = strlen(s);

    int x = r->left;
    int y = r->top;
    if (format & DT_BOTTOM)
        y = r->bottom - 15;

    //if (strstr(s, "for the"))
    //    __debugbreak();

    int retval;
    if (format & DT_RIGHT)
        retval = DrawTextA(hdc, s, c, r, format);
    else
        retval = TextOutA(hdc, x, y, s, c);

    WriteProcessMemory(GetCurrentProcess(), (char*)adr_text,
        (void*)jump_text, 6, &written);

    return retval;
}

void interceptTextOut()
{
    DWORD written;
    HINSTANCE hinst = GetModuleHandle("user32.dll");
    adr_text = (DWORD)GetProcAddress(hinst, "DrawTextA");

    log("DrawTextA found at %X", adr_text);

    jump_text[0] = 0x68;
    *(DWORD*)(jump_text + 1) = (DWORD)&Intercept_DrawTextA;
    jump_text[5] = 0xC3;

    ReadProcessMemory(GetCurrentProcess(), (void*)adr_text,
        (void*)old_text, 6, &written);

    WriteProcessMemory(GetCurrentProcess(), (char*)adr_text,
        (void*)jump_text, 6, &written);
}
*/

/*
float old_val;
float new_val;

float glitched_res = 0;

void __stdcall check_res()
{
    if (old_val < new_val)
        glitched_res += (new_val - old_val);
}

__declspec(naked) void repair_test() //005D0D59
{
    __asm
    {
        fmul    dword ptr [esp+10h]
        fsubr   dword ptr [ecx]
        
        push    eax
        push    ecx
        push    edx
        mov     eax, [ecx]
        mov     old_val, eax

        fstp    dword ptr [ecx]
        
        mov     eax, [ecx]
        mov     new_val, eax

        call    check_res

        pop     edx
        pop     ecx
        pop     eax

        push    005D0D61h
        ret
    }
}
*/

/*struct function_data
{
    BYTE original_data[11];
};

//std::map<std::pair<DWORD, function_data>> functions;
std::map<DWORD, function_data> functions;

DWORD __stdcall record_function_call(DWORD caller, DWORD function)
{

}

DWORD __stdcall record_function_return()
{

}

__declspec(naked) void on_function_return()
{
    __asm
    {

    }
}

__declspec(naked) void on_function_call()
{
    __asm
    {
        push    ecx
        push    edx
        mov     eax, [esp + 0Ch] //function
        mov     ecx, [esp + 10h] //caller
        push    eax
        push    ecx
        call    record_function_call
        push    eax
        push    edi
        push    esi
        push    0C8763E65h
        sub     esp, 100h
        mov     [esp], on_function_return
        lea     esi, [esp + 120h]
        lea     edi, [esp + 4]
        mov     ecx, 60
        movsd
        mov     eax, [esp + 118h]
        jmp     eax
    }
}

void __stdcall add_function(DWORD addr)
{
    function_data fd;
    DWORD r;
    ReadProcessMemory(GetCurrentProcess(), (void*)addr, fd.original_data, 11, &r);
    writeByte(addr, 0xE8); //call
    writeDword(addr + 1, 0); //$+0
    writeByte(addr + 5, 0x68); //push
    writeDword(addr + 6, (DWORD)&on_function_call); //address
    writeByte(addr + 10, 0xC3); //ret
}*/

//int control_source;
//int control_target = 1;
//bool control_initiated = false;

//for takecontrol
/*__declspec(naked) void onSync() //00432DFF
{
    __asm
    {
        mov      al, control_initiated
        test     al, al
        jz       _no_control
        push     ecx
        push     control_source
        call     takeControl
        pop      ecx
        mov      eax, 004496A0h
        call     eax
        mov      eax, control_target
        push     eax
        call     takeControl
        mov      eax, 00432E04h
        jmp      eax
_no_control:
        mov      eax, 004496A0h
        call     eax
        mov      eax, 00432E04h
        jmp      eax
    }
}*/

__declspec(naked) void onStatusScreenCreate() //005E7EC0
{
    __asm
    {
        ret     14h
    }
}

__declspec(naked) void onBlankScreenCreate() //004B4960
{
    __asm
    {
        ret     8
    }
}

__declspec(naked) int new_game_dev_mode() //0042C360
{
    __asm
    {
        mov     eax, 1
        ret
    }
}

__declspec(naked) int new_check_multi_copies() //00428270
{
    __asm
    {
        mov     eax, 1
        ret
    }
}

//const char* savegame_path = "savegame\\test\\recs\\";
const char* savegame_path = "savegame\\";
const char* scenario_path = "scenario\\";

std::map<void*, std::pair<void*, size_t>> allocations;

void* __fastcall new_malloc(size_t size, void* from)
{
    void* memory = malloc(size);
    memset(memory, 0, size);
    allocations[memory] = std::make_pair(from, size);
    return memory;
}

void __fastcall new_free(void* memory)
{
    auto it = allocations.find(memory);
    if (it != allocations.end())
    {
        free(memory);
        allocations.erase(it);
    }
}

size_t total_mem = 0;

void memory_print()
{
    total_mem = 0;
    log("Not freed allocations:");
    for (auto it = allocations.begin(); it != allocations.end(); ++it)
    {
        log("0x%p, %u bytes", it->second.first, it->second.second);
        total_mem += it->second.second;
    }
    log("Total: %u bytes", total_mem);
}

__declspec(naked) void* __cdecl malloc_wr(size_t size) //0063328C
{
    __asm
    {
        mov     edx, [esp]
        mov     ecx, [esp + 4]
        call    new_malloc
        ret
    }
}

__declspec(naked) void* __cdecl calloc_wr(size_t number, size_t size) //00632D33
{
    __asm
    {
        mov     edx, [esp]
        mov     ecx, [esp + 4]
        imul    ecx, [esp + 8]
        call    new_malloc
        ret
    }
}

__declspec(naked) void* __cdecl new_wr(size_t size) //00632B9D
{
    __asm
    {
        mov     edx, [esp]
        mov     ecx, [esp + 4]
        call    new_malloc
        ret
    }
}

__declspec(naked) void* __cdecl free_wr(size_t size) //00632CCA
{
    __asm
    {
        mov     ecx, [esp + 4]
        call    new_free
        ret
    }
}

__declspec(naked) void* __cdecl delete_wr(size_t size) //00632B42
{
    __asm
    {
        mov     ecx, [esp + 4]
        call    new_free
        ret
    }
}

void __stdcall report_dynamic_tile_cache_size_sub(int disabled, int size)
{
    log("Dynamic tile cache: disabled = %d, size = %d", disabled, size);
}

__declspec(naked) void* report_dynamic_tile_cache_size() //00497192
{
    __asm
    {
        mov     eax, [esi + 0C0D0h]
        mov     ecx, [esi + 0C0C8h]
        push    eax
        push    ecx
        call    report_dynamic_tile_cache_size_sub

        mov     eax, [esi + 0C0C8h]
        mov     ecx, 00497198h
        jmp     ecx
    }
}

#define ID_TO_WATCH 5746

void __stdcall on_check_ownership_log(void* action, void* player)
{
    int id;
    if (player)
        id = *(int*)((DWORD)player + 0xA0);
    else
        id = -1;

    int wt = get_worldtime();
    int ms = wt % 1000;
    wt /= 1000;
    int s = wt % 60;
    wt /= 60;
    int m = wt % 60;
    int h = wt / 60;

    RGE_Static_Object* obj = *(RGE_Static_Object**)((DWORD)action + 8);
    if (obj && obj->id == ID_TO_WATCH)
        log(" [T = %01d:%02d:%02d.%03d] ==> check_ownership: player = %d", h, m, s, ms, id);
}

__declspec(naked) void on_check_ownership() //00565182
{
    __asm
    {
        mov     eax, 00565410h
        call    eax
        push    eax
        push    eax
        push    esi
        call    on_check_ownership_log
        pop     eax
        mov     ecx, 00565187h
        jmp     ecx
    }
}

void __stdcall on_update_log(void* action)
{
    int wt = get_worldtime();
    int ms = wt % 1000;
    wt /= 1000;
    int s = wt % 60;
    wt /= 60;
    int m = wt % 60;
    int h = wt / 60;

    RGE_Static_Object* obj = *(RGE_Static_Object**)((DWORD)action + 8);
    unsigned __int16 type = *(unsigned __int16*)((DWORD)action + 4);
    unsigned __int8 state = *(unsigned __int8*)((DWORD)action + 0xC);
    if (obj && obj->id == ID_TO_WATCH)
        log(" [T = %01d:%02d:%02d.%03d] => update (%d): curr_state = %d", h, m, s, ms, (int)type, (int)state);
}

__declspec(naked) void on_update() //005650F0
{
    __asm
    {
        push    esi
        mov     esi, ecx
        push    edi

        push    ecx
        call    on_update_log

        mov     ecx, [esi + 34h]
        mov     eax, 005650F7h
        jmp     eax
    }
}

struct node
{
    void* action;
    node* next;
};

void __stdcall pause_game2()
{
    TRIBE_Screen_Game* game_screen = ((TRIBE_Game*)(*base_game))->game_screen;
    if (game_screen)
        TRIBE_Screen_Game__command_pause(game_screen);
}

void __stdcall on_action_list_update_log(void* action_list)
{
    int wt = get_worldtime();
    int ms = wt % 1000;
    wt /= 1000;
    int s = wt % 60;
    wt /= 60;
    int m = wt % 60;
    int h = wt / 60;

    RGE_Static_Object* obj = *(RGE_Static_Object**)((DWORD)action_list + 4);
    if (obj && obj->id == ID_TO_WATCH)
    {
        char b[0x100];
        b[0] = '\0';
        node* nd = *(node**)((DWORD)action_list + 8);
        while (nd)
        {
            void* action = nd->action;
            unsigned __int16 type = *(unsigned __int16*)((DWORD)action + 4);
            unsigned __int8 state = *(unsigned __int8*)((DWORD)action + 0xC);
            sprintf(b + strlen(b), "%d (%d) ", (int)type, (int)state);
            nd = nd->next;
        }
        //log(" actions: %s", b);

        wt = get_worldtime();
        //if (wt >= 201170)
            //__debugbreak();
            //pause_game2();
        log(" [T = %01d:%02d:%02d.%03d] => update_action_list: %s", h, m, s, ms, b);
    }
}

__declspec(naked) void on_action_list_update() //00406B60
{
    __asm
    {
        push    ebx
        push    edi
        mov     edi, ecx
        push    edi
        call    on_action_list_update_log
        mov     eax, [edi + 8]

        mov     ecx, 00406B67h
        jmp     ecx
    }
}

void __stdcall on_action_list_update_call_log(void* action)
{

}

__declspec(naked) void on_action_list_update_call() //00406B70
{
    __asm
    {
        mov     ecx, [eax]
        push    ecx
        push    ecx
        call    on_action_list_update
        pop     ecx
        mov     eax, [ecx]
        call    dword ptr [eax + 28h]

        mov     ecx, 00406B77h
        jmp     ecx
    }
}

void __stdcall on_set_default_log(RGE_Static_Object* obj)
{
    int wt = get_worldtime();
    int ms = wt % 1000;
    wt /= 1000;
    int s = wt % 60;
    wt /= 60;
    int m = wt % 60;
    int h = wt / 60;

    if (obj && obj->id == ID_TO_WATCH)
    {
        log(" [T = %01d:%02d:%02d.%03d] => set_default", h, m, s, ms);
    }
}

__declspec(naked) void on_set_default() //004098C0
{
    __asm
    {
        push    esi
        call    on_set_default_log
        mov     edx, [esi]
        mov     ecx, esi
        call    dword ptr [edx + 390h]

        mov     ecx, 004098C6h
        jmp     ecx
    }
}


void __stdcall on_action_object_update_log(RGE_Static_Object* obj)
{
    int wt = get_worldtime();
    int ms = wt % 1000;
    wt /= 1000;
    int s = wt % 60;
    wt /= 60;
    int m = wt % 60;
    int h = wt / 60;

    if (obj && obj->id == ID_TO_WATCH)
    {
        log(" [T = %01d:%02d:%02d.%03d] => action_object_update, state = %d", h, m, s, ms, (int)obj->object_state);

        wt = get_worldtime();
        //if (wt >= 200728 /*201170*/)
            //__debugbreak();
            //pause_game2();
    }
}

__declspec(naked) void on_action_object_update() //00409874
{
    __asm
    {
        mov     esi, ecx
        push    esi
        call    on_action_object_update_log
        
        mov     ecx, esi
        mov     eax, 004A1A00h
        call    eax
        mov     ecx, 00409879h
        jmp     ecx
    }
}

bool __stdcall building_get_change_terrain_unconditional(TRIBE_Building_Object* obj)
{
    return true;
    if (obj->master_obj->id == 50)  //farm
    {
        return true;
        switch (obj->owner->world->map->map_row_offset[(int)obj->world_y][(int)obj->world_x].terrain_type)
        {
        case 4:
        case 59:
        case 142:
        case 235:
            return true;
        default:
            return false;
        }
    }
    else
        return false;
}

__declspec(naked) void on_building_change_terrain() //00557C31
{
    __asm
    {
        push    edi
        call    building_get_change_terrain_unconditional

        mov     ecx, [edi + 18h]
        test    al, al
        jnz     building_change_terrain_unconditional

        mov     bl, [esp + 10h]
        mov     eax, 00557C38h
        jmp     eax

building_change_terrain_unconditional:
        mov     eax, 00557CC9h
        jmp     eax
    }
}

void __stdcall state_log(RGE_Action_Gather* action, int state)
{
    chat("State: %d", state);
    //action->farm_x = -1;
    //action->farm_y = -1;
    //if (state == 243)
       // __debugbreak();
}

__declspec(naked) void on_state() //00404490
{
    __asm
    {
        mov    eax, [esp + 4]
        push   ecx
        push   eax
        push   ecx
        call   state_log
        pop    ecx
        _emit  0x64
        _emit  0xA1
        _emit  0x00
        _emit  0x00
        _emit  0x00
        _emit  0x00
        mov    edx, 00404496h
        jmp    edx
    }
}

void __stdcall log_player_update(RGE_Player* player)
{
    log("WT=%d, P%d, before update: rand=%d", player->world->world_time, player->id, rand_no_change());
    player->vfptr->update(player);
    log("WT=%d, P%d, after update: rand=%d", player->world->world_time, player->id, rand_no_change());
}

__declspec(naked) void log_on_player_update() //0061F85E
{
    __asm
    {
        push    ecx
        call    log_player_update
        mov     ecx, 0061F864h
        jmp     ecx
    }
}

void __stdcall log_object_list_update(RGE_Player_Object_List* ol)
{
    log("  OL: before update: rand=%d", rand_no_change());
    RGE_Player_Object_List__Update(ol);
    log("  OL: after update: rand=%d", rand_no_change());
}

__declspec(naked) void log_on_object_list_update() //004C1985
{
    __asm
    {
        push    ecx
        call    log_object_list_update
        mov     ecx, 004C198Ah
        jmp     ecx
    }
}

unsigned __int8 __stdcall log_object_update(RGE_Static_Object* obj)
{
    log("    obj=%d: before update: rand=%d", obj->id, rand_no_change());

    //if (obj->owner->world->world_time == 21990 && obj->id == 10730)
    //    __debugbreak();

    unsigned __int8 result = obj->vfptr->update(obj);
    log("    obj=%d: after update: rand=%d", obj->id, rand_no_change());
    return result;
}

__declspec(naked) void log_on_object_update() //004AF39B
{
    __asm
    {
        push    ecx
        call    log_object_update
        mov     ecx, 004AF3A1h
        jmp     ecx
    }
}

void __fastcall TRIBE_Game__calc_timing_text_new(TRIBE_Game* game)
{
    unsigned int wu; // ecx
    unsigned int v3; // ecx

    unsigned __int8* initialized_last_time = (unsigned __int8*)0x0078F6A4;
    unsigned int* last_time = (unsigned int*)0x0078F824;
    int* show_timing_max = (int*)0x0078F974;

    if ((*initialized_last_time & 1) == 0)
    {
        *initialized_last_time |= 1u;
        *last_time = timeGetTime();
    }
    *last_time = timeGetTime();
    wu = game->world_update_count;
    if (!wu)
        wu = 1;
    if (*show_timing_max)
        sprintf(
            game->timing_text2,
            "t%lu,f%lu,max,r(v%lu,m%lu,o%lu),s%lu,u%lu,c%lu,lg%lu,sp%lu,sm%lu,m%lu,p%lu,ds%lu,ol%lu",
            game->world_update_fps,
            game->view_update_fps,
            game->timings[6].last_max_time,     //render view
            game->timings[7].last_max_time,     //render map
            game->timings[21].last_max_time,    //render other
            game->timings[3].last_max_time,     //s \ paint
            game->timings[1].last_max_time,     //u \ world update
            game->timings[14].last_max_time,    //c \ comm cycle
            game->timings[12].last_max_time,    //lg \ draw
            game->timings[10].last_max_time,    //sp \ sound play
            game->timings[16].last_max_time,    //sm \ sound manager
            game->timings[13].last_max_time,    //m \ mouse
            game->timings[17].last_max_time,    //p \ pick
            game->timings[2].last_max_time,     //ds \ draw smth
            game->timings[29].last_max_time);   //ol \ object list
    else
        sprintf(
            game->timing_text2,
            "t%lu,fps:%lu,avg,r(v%lu,m%lu,o%lu),s%lu,u%lu,c%lu,lg%lu,sp%lu,ss%lu,sm%lu,m%lu,o%lu,p%lu",
            game->world_update_fps,
            game->view_update_fps,
            game->timings[6].last_time / wu,    //render view
            game->timings[7].last_time / wu,    //render map
            game->timings[21].last_time / wu,   //render other
            game->timings[3].last_time / wu,    //s \ paint
            game->timings[1].last_time / wu,    //u \ world update
            game->timings[14].last_time / wu,   //c \ comm cycle
            game->timings[12].last_time / wu,   //lg \ draw
            game->timings[10].last_time / wu,   //sp \ sound play
            game->timings[15].last_time / wu,   //ss \ nothing
            game->timings[16].last_time / wu,   //sm \ sound manager
            game->timings[13].last_time / wu,   //m \ mouse
            game->timings[4].last_time / wu,    //o \ nothing
            game->timings[17].last_time / wu);  //p \ pick
    v3 = game->view_update_count;
    if (!v3)
        v3 = 1;
    sprintf(game->timing_text, "avg view time=%lu, count=%lu", game->timings[6].last_time / v3, game->view_update_count);
}

void __stdcall do_update_players_start()
{

}

__declspec(naked) void update_players_start() //0061F741
{
    __asm
    {
        lea     edi, [ebp - 80h]
        rep stosd
        call    do_update_players_start

    }
}

/*
__declspec(naked) void test_rType() //00586F35
{
    __asm
    {
        cmp     edx, 4
        jb      good_rtype
        int     3

good_rtype:
        lea     eax, [eax + eax * 2]
        mov     ecx, [ecx + edx * 4 + 1F24h]
        mov     ebx, 00586F3Fh
        jmp     ebx
    }
}

void __stdcall TPanel__set_positioning_new(
    void* ret_addr,
    TPanel* this_,
    int position_mode_in,
    int left_border_in,
    int top_border_in,
    int right_border_in,
    int bottom_border_in,
    int min_wid_in,
    int max_wid_in,
    int min_hgt_in,
    int max_hgt_in,
    TPanel* left_panel_in,
    TPanel* top_panel_in,
    TPanel* right_panel_in,
    TPanel* bottom_panel_in)
{
    if (this_->parent_panel && this_->parent_panel->vfptr == (TPanelVtbl*)0x00661C50)
    {
        std::string panel_type;
        DWORD vtbl = (DWORD)this_->vfptr;
        switch (vtbl)
        {
        case 0x0065E170:
            panel_type = "TTextPanel";
            break;
        case 0x0065CD6C:
            panel_type = "TButtonPanel";
            break;
        case 0x0065D6B8:
            panel_type = "TListPanel";
            break;
        case 0x0065DB78:
            panel_type = "TScrollBarPanel";
            break;
        case 0x00668FFC:
            panel_type = "TRIBE_Diamond_Map_View";
            break;
        case 0x0065C668:
            panel_type = "TPanel";
            break;
        case 0x006691A4:
            panel_type = "TRIBE_Main_View";
            break;
        default:
            panel_type = std::to_string(vtbl);
            break;
        }

        log("Pos, r=0x%08X: type=%s, (%d,%d,%d,%d) (%d,%d)",
            ret_addr,
            panel_type.c_str(),
            left_border_in,
            top_border_in,
            right_border_in,
            bottom_border_in,
            max_wid_in,
            max_hgt_in);
    }

    TDrawArea* v15; // eax
    TPanelVtbl* v16; // edx

    this_->position_mode = position_mode_in;
    this_->right_border = right_border_in;
    this_->top_border = top_border_in;
    this_->max_wid = max_wid_in;
    this_->left_border = left_border_in;
    this_->min_wid = min_wid_in;
    this_->left_panel = left_panel_in;
    this_->bottom_border = bottom_border_in;
    this_->max_hgt = max_hgt_in;
    this_->bottom_panel = bottom_panel_in;
    v15 = this_->render_area;
    this_->min_hgt = min_hgt_in;
    this_->right_panel = right_panel_in;
    v16 = this_->vfptr;
    this_->top_panel = top_panel_in;
    if (v15)
        v16->handle_size(this_, v15->Width, v15->Height);
    else
        v16->handle_size(this_, 0, 0);
    if (this_->active)
        this_->vfptr->set_redraw(this_, 2);
}

__declspec(naked) void on_set_positioning()
{
    __asm
    {
        mov     eax, [esp]
        mov     [esp], ecx
        push    eax
        push    eax
        jmp     TPanel__set_positioning_new
    }
}
*/

#pragma optimize( "s", on )
void setTestHook()
{
    //Sed UI panel
    //writeDword(0x005368BA, 700);
    //writeDword(0x005368BF, 700);

    //diamond map dimensions
    //writeDword(0x00536893, 800);
    //writeDword(0x0053688E, 800);
    //writeDword(0x00536889, 450);
    //writeDword(0x00536884, 450);

    //diamond map bitmap sizer
    //writeDword(0x005298C6, 50415);

    //setHook((void*)0x004B5D80, on_set_positioning);

    //setHook((void*)0x00586F35, test_rType);

    //writeNops(0x00517611, 6);
    //writeByte(0x00517611, 0xE9);

    //writeByte(0x005EBB65, 0xEB);

    //setHook((void*)0x005E95E0, TRIBE_Game__calc_timing_text_new);
    //setHook((void*)0x004AF380, RGE_Player_Object_List__Update_new);

    //setHook((void*)0x0061F85E, log_on_player_update);
    //setHook((void*)0x004C1985, log_on_object_list_update);
    //setHook((void*)0x004AF39B, log_on_object_update);

    //writeNops(0x00404F59, 2);

    //setHook((void*)0x00404490, on_state);
    //setHook((void*)0x00557C31, on_building_change_terrain);

    /*
    unsigned __int8* color_black = (unsigned __int8*)0x007A1C60;
    //*color_black = 1;
    int* group_debug_display = (int*)0x006ACCA4;
    *group_debug_display = 1;
    */

    /*setHook((void*)0x005650F0, on_update);
    setHook((void*)0x00565182, on_check_ownership);
    setHook((void*)0x00406B60, on_action_list_update);
    setHook((void*)0x004098C0, on_set_default);
    setHook((void*)0x00409874, on_action_object_update);*/

    //validate IAT 00654000 - 00654440
    
/*#ifdef TARGET_VOOBLY
    void* IAT_data = malloc(0x440);
    ReadProcessMemory(GetCurrentProcess(), (void*)0x00654000, IAT_data, 0x440, NULL);
    writeData(0x00654000, IAT_data, 0x440);
#endif*/

    //malloc log
    /*setHook((void*)0x0063328C, malloc_wr);
    setHook((void*)0x00632D33, calloc_wr);
    setHook((void*)0x00632B9D, new_wr);
    setHook((void*)0x00632CCA, free_wr);
    setHook((void*)0x00632B42, delete_wr);*/

    //setHook((void*)0x0042C360, new_game_dev_mode);
    //setHook((void*)0x00428270, new_check_multi_copies);
    //writeDword(0x00659F80, 0x004B6120);
    //setHook((void*)0x0046D9B0, (void*)0x004B6120);
    //setHook((void*)0x005E7EC0, onStatusScreenCreate);
    //setHook((void*)0x004B4960, onBlankScreenCreate);
    //srand(timeGetTime());
    
    //setOOSHooks();
    //savegame path
    writeDword(0x0048F566, (DWORD)savegame_path);

    //setHook((void*)0x005D0D59, repair_test);

    //RMS token count
    //setHook((void*)0x004E1D40, onRmsToken);

    /* !! conflicts trigger or condition !!
#ifdef _DEBUG
    setHook((void*)0x005F54D2, triggerLogHook);
    setHook((void*)0x005F4A77, effectLogHook);
#endif
    */

    //fix internal log
    log_int_s = (char*)malloc(LOG_INT_BUFFER_SIZE);

    writeDword(0x0040F1A6, (DWORD)nullsub_26_2);
    writeDword(0x0040F842, (DWORD)nullsub_26_2);
    writeDword(0x0040F885, (DWORD)nullsub_26_2);
    writeDword(0x0040F75B, (DWORD)nullsub_26_2);
    writeDword(0x0040EFBB, (DWORD)nullsub_26_2);
    writeDword(0x0057796E, (DWORD)nullsub_26_2);
    writeDword(0x00577A63, (DWORD)nullsub_26_2);
    writeDword(0x00577A89, (DWORD)nullsub_26_2);
    writeDword(0x00577AAF, (DWORD)nullsub_26_2);

    fix_function_call(0x0059B18B, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B1C6, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B1EA, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B4BE, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B524, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B56D, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B5AC, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B5D0, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B63D, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B675, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B73C, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B827, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B870, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B8AF, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B8C7, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B947, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059B9D0, (DWORD)nullsub_26_2); //Entering task
    fix_function_call(0x0059BACB, (DWORD)nullsub_26_2); //Entering task

    fix_function_call(0x004336D8, (DWORD)nullsub_26_2); //Starting turn
    //

    writeNops(0x004428F6, 5);
    writeNops(0x0061B129, 5);
    writeNops(0x005B944D, 5);
    writeNops(0x005A05DD, 5);
    writeNops(0x0050D0F3, 5);
    writeNops(0x00410D38, 5);
    writeNops(0x0042520B, 5);
    writeNops(0x004776BE, 5);
    writeNops(0x005A0559, 5);
    writeNops(0x005A0581, 5);
    writeNops(0x005A05DD, 5);

    setHook((void*)0x0060F920, log_int_wr);

#ifndef TARGET_VOOBLY
    setHook((void*)0x00438140, onChat);
#endif

    //
#ifdef _DEBUG
    //setHook((void*)0x004D5550, readDatHook);
#endif

    setHook((void*)0x00497192, report_dynamic_tile_cache_size);

    //action list
    //writeNops(0x004098FD, 5);
}
#pragma optimize( "", on )
