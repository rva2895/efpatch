#include "stdafx.h"
#include "chatcommand.h"
#include "worlddump.h"
//#include "functionlist.h"

extern unsigned int fog_param_a1;
extern unsigned int fog_param_a3;
extern unsigned int fog_param_a2;
extern unsigned int fog_param_a4;

void verify_unitlines()
{
    chat("Started unitline verification..");

    TRIBE_World* world = (*base_game)->world;
    TRIBE_Player* player = (TRIBE_Player*)RGE_Base_Game__get_player_by_id((*base_game), 2); //(*base_game)->world->players[0];

    RGE_Master_Static_Object** player_master_objects = player->master_objects;
    int num_master_objects = player->master_object_num;

    RGE_Master_Static_Object** gaia_master_master_objects = world->master_players[0]->master_objects;

    auto is_obj_in_line = [](TRIBE_Unitline* line, __int16 id)
        {
            for (int i = 0; i < line->numberUnits; i++)
                if (line->units[i] == id)
                    return true;
            return false;
        };

    for (int p = 0; p < world->master_player_num; p++)
        for (int i = 0; i < world->master_players[p]->master_object_num; i++)
        {
            if (world->master_players[p]->master_objects[i])
            {
                __int16 line_id = world->master_players[p]->master_objects[i]->unit_line;
                if (line_id != -1)
                {
                    if (line_id < 0 || line_id >= world->gbg_numberUnitlines)
                    {
                        log("Master player %d, obj %d: bad unitline", p, i);
                        //return;
                    }
                    else if (!is_obj_in_line(&world->gbg_unitlines[line_id], i))
                    {
                        log("Master player %d, obj %d: not in specified unitline", p, i);
                        //return;
                    }
                }
            }
        }

    chat("Unitline verification complete");
}

void data_save()
{
    int handle = rge_open2("data_test.dat", 0x8301, 0x180);
    if (handle >= 0)
    {
        (*base_game)->world->vfptr->save((*base_game)->world, handle);
        rge_close(handle);
        chat("Data file saved");
    }
    else
        chat("Error");
}

/*
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
*/

/*
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

int* const rand_val = (int* const)0x0069EC98;

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
*/

/*
extern std::vector<FUNCTION_HOOK*> function_hooks;

struct
{
    bool operator() (FUNCTION_HOOK* fh1, FUNCTION_HOOK* fh2) const
    {
        return !(*fh1 < *fh2);
    }
} compare_call_counts_object;

*/

extern unsigned int dump_objects(const char* filename);
extern int max_worldtime;

//extern void __stdcall make_oos_dump();

void do_test()
{
    int i, p;
    int counter = 0;

    TRIBE_World* world = (*base_game)->world;

    auto test_obj = [](RGE_Master_Player* master_player, __int16 id)
        {
            return ((id == -1) || ((id >= 0) && (id < master_player->master_object_num) && master_player->master_objects[id]));
        };

    auto test_valid_missile = [&](RGE_Master_Player* master_player, __int16 id)
        {
            return test_obj(master_player, id) && ((id == -1) || (master_player->master_objects[id]->master_type == 60));
        };

    auto is_obj_in_line = [](TRIBE_Unitline* line, __int16 id)
        {
            for (int i = 0; i < line->numberUnits; i++)
                if (line->units[i] == id)
                    return true;
            return false;
        };

    auto test_unit_line = [&](RGE_Master_Player* master_player, __int16 unit_line)
        {
            return ((unit_line == -1) || ((unit_line >= 0) && (unit_line < world->gbg_numberUnitlines)));
        };

    auto test_in_unit_line = [&](__int16 line_id, __int16 id)
        {
            return is_obj_in_line(&world->gbg_unitlines[line_id], id);
        };

    auto test_los = [](float los)
        {
            return (los == -1.0f) || ((los >= 0.0f) && (los <= 20.0f));
        };

    auto test_attribute = [](RGE_Master_Player* master_player, __int16 type)
        {
            return (type == -1) || ((type >= 0) && (type < master_player->attribute_num));
        };

    auto test_tech = [&](__int16 id)
        {
            return (id == -1) || ((id >= 0) && (id < world->tech->tech_tree_num));
        };

    auto report_error_and_inc_counter = [&](const char* error_str)
        {
            ++counter;
            //if (++counter < 10000)
            //    log(format, p, i);
            log("Master player %d, obj %d: %s", p, i, error_str);
        };

    for (p = 0; p < (*base_game)->world->master_player_num; p++)
        for (i = 0; i < (*base_game)->world->master_players[p]->master_object_num; i++)
        {
            RGE_Master_Player* master_player = (*base_game)->world->master_players[p];

            RGE_Master_Static_Object* obj = master_player->master_objects[i];
            if (obj)
            {
                //master_type
                switch (obj->master_type)
                {
                case 10:
                case 15:
                case 20:
                case 25:
                case 30:
                case 40:
                case 50:
                case 60:
                case 70:
                case 80:
                    break;
                default:
                    report_error_and_inc_counter("master_type");
                    break;
                }

                //unit line
                if (!test_unit_line(master_player, obj->unit_line))
                    report_error_and_inc_counter("bad unitline");
                else if (obj->unit_line != -1 && !test_in_unit_line(obj->unit_line, obj->id))
                    report_error_and_inc_counter("not in specified unitline");

                //los
                if (!test_los(obj->los))
                    report_error_and_inc_counter("bad los");

                //death_spawn_obj_id
                if (!test_obj(master_player, obj->death_spawn_obj_id))
                    report_error_and_inc_counter("bad death_spawn_obj_id");

                //attribute_type_held
                for (int k = 0; k < 3; k++)
                    if (!test_attribute(master_player, obj->attribute_type_held[k]))
                        report_error_and_inc_counter("bad attribute_type_held[]");

                if (obj->master_type >= 30)
                {
                    RGE_Master_Moving_Object* mov_obj = (RGE_Master_Moving_Object*)obj;
                    if (!test_obj(master_player, mov_obj->obj_trail_id))
                        report_error_and_inc_counter("bad obj_trail_id");

                    if (obj->master_type >= 40)
                    {
                        RGE_Master_Action_Object* act_obj = (RGE_Master_Action_Object*)obj;
                        if (!test_obj(master_player, act_obj->backup_drop_site))
                            report_error_and_inc_counter("bad drop_site");

                        if (!test_obj(master_player, act_obj->backup_drop_site))
                            report_error_and_inc_counter("bad backup_drop_site");

                        if (obj->master_type >= 50)
                        {
                            RGE_Master_Combat_Object* combat_obj = (RGE_Master_Combat_Object*)obj;
                            if (!test_valid_missile(master_player, combat_obj->missile_id))
                                report_error_and_inc_counter("bad missile_id");

                            if (obj->master_type >= 70)
                            {
                                TRIBE_Master_Combat_Object* tcombat_obj = (TRIBE_Master_Combat_Object*)obj;

                                //build_inventory
                                for (int k = 0; k < 3; k++)
                                    if (!test_attribute(master_player, tcombat_obj->build_inventory[k].type))
                                        report_error_and_inc_counter("bad build_inventory[]");

                                if (!test_obj(master_player, tcombat_obj->id_of_building_obj))
                                    report_error_and_inc_counter("bad id_of_building_obj");

                                if (!test_valid_missile(master_player, tcombat_obj->volley_missile_id))
                                    report_error_and_inc_counter("bad volley_missile_id");

                                if (obj->master_type >= 80)
                                {
                                    TRIBE_Master_Building_Object* bld_obj = (TRIBE_Master_Building_Object*)obj;
                                    if (!test_obj(master_player, bld_obj->on_build_make_obj))
                                        report_error_and_inc_counter("bad on_build_make_obj");

                                    if (!test_tech(bld_obj->on_build_make_tech))
                                        report_error_and_inc_counter("bad on_build_make_tech");

                                    for (int k = 0; k < 4; k++)
                                        if (!test_obj(master_player, bld_obj->linked_id[k]))
                                            report_error_and_inc_counter("bad linked_id[]");

                                    if (!test_obj(master_player, bld_obj->construction_id))
                                        report_error_and_inc_counter("bad construction_id");

                                    if (!test_obj(master_player, bld_obj->open_close_id))
                                        report_error_and_inc_counter("bad open_close_id");

                                    if (!test_obj(master_player, bld_obj->salvage_id))
                                        report_error_and_inc_counter("bad salvage_id");
                                }
                            }
                        }
                    }
                }
            }
        }

    chat("Scan complete. Found %d errors", counter);
}

#define MAKE_UINT32(a,b,c,d) (((d) << 24) | ((c) << 16) | ((b) << 8) | (a))

int last_find_id = INT_MAX;

bool check_chat_command(const char* str)
{
    if (!strcmp(str, "/version"))
    {
        chat(EFPATCH_VERSION);
        return true;
    }
#ifdef EFPATCH_ENABLE_DEBUG_COMMANDS
    else if (!strcmp(str, "/test"))
    {
        do_test();
        return true;
    }
    else if (!strcmp(str, "/verify-unitlines"))
    {
        verify_unitlines();
        return true;
    }
    /*
    else if (!strcmp(str, "/data-save"))
    {
        data_save();
        return true;
    }
    */
    else if (!strcmp(str, "/obj-count"))
    {
        RGE_Player* player = RGE_Base_Game__get_player(*base_game);
        chat("Objects: %d, Sleeping: %d, Doppleganger: %d",
            player->objects->Number_of_objects,
            player->sleeping_objects->Number_of_objects,
            player->doppleganger_objects->Number_of_objects);
        return true;
    }
    else if (strstr(str, "/find-obj"))
    {
        char d[0x100];
        int id;
        sscanf(str, "%s %d", d, &id);
        RGE_Player* player = RGE_Base_Game__get_player(*base_game);
        RGE_Static_Object* unit = RGE_Player_Object_List__find_by_master_id(player->objects, id, -1.0f, -1.0f, 0, 0, NULL, -1);
        if (unit)
        {
            RGE_Player__unselect_object(player);
            RGE_Player__select_object(player, unit, 1);
        }
        else
            chat("Invalid object id");
        return true;
    }
    /*
    else if (!strcmp(str, "/pathing-stats"))
    {
        calc_path_stats();
        return true;
    }
    */
    /*
    else if (!strcmp(str, "/del-save"))
    {
        del_states();

        return true;
    }
    else if (!strcmp(str, "/save"))
    {
        TRIBE_World* world = (*base_game)->world;

        save_state((RGE_Game_World*)world);

        return true;
    }
    else if (!strcmp(str, "/load"))
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

        return true;
    }
    */
    else if (strstr(str, "/update"))
    {
        char d[0x100];
        int t;
        sscanf(str, "%s %d", d, &t);

        int* const do_fixed_update = (int* const)0x006A35E8;
        int do_fixed_update_old = *do_fixed_update;
        *do_fixed_update = 1;
        unsigned int start_wt = (*base_game)->world->world_time;
        unsigned int current_wt = start_wt;

        TCommCommandLog* log = (*base_game)->world->com_handler->mCommandLog;

        int old_speed = log->mReplaySpeed;
        log->mReplaySpeed = 12;

        unsigned int last_update_time = timeGetTime();

        unsigned int first_update_time = last_update_time;

        unsigned __int8 game_state = (*base_game)->world->game_state;
        while (game_state == 0 && current_wt - start_wt < t * 1000)
        {
            game_state = (*base_game)->world->vfptr->update((*base_game)->world);
            current_wt = (*base_game)->world->world_time;

            TMessagePanel* message_panel; // [esp+0h] [ebp-20h]
            struct tagMSG Msg; // [esp+4h] [ebp-1Ch] BYREF

            unsigned int current_update_time = timeGetTime();

            //if (false)
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

        chat("Jumped %d seconds ahead, time elapsed: %u", t, timeGetTime() - first_update_time);
        char buf[0x100];
        snprintf(buf, _countof(buf), "%d", timeGetTime() - first_update_time);
        //MessageBox(0, buf, "test", 0);
        //TEasy_Panel__popupOKDialog((TEasy_Panel*)TPanelSystem__getTop(panel_system), buf, "Test", 450, 100, 1);

        //for (int i = 0; i < 10000; i++)
        //    game_state = (*base_game)->world->vfptr->update((*base_game)->world);

        log->mReplaySpeed = old_speed;

        //for (int i = 0; i < 1000; i++)
        //    game_state = (*base_game)->world->vfptr->update((*base_game)->world);

        *do_fixed_update = do_fixed_update_old;
        //chat("t = %d", t);
        return true;
    }
    else if (strstr(str, "/control"))
    {
        char d[0x100];
        int p;
        sscanf(str, "%s %d", d, &p);
        if ((p >= 0) && (p <= 8))
        {
            RGE_Player__unselect_object(RGE_Base_Game__get_player(*base_game));
            TRIBE_Game__set_player(*(TRIBE_Game**)base_game, p);
        }
        return true;
    }
    else if (!strcmp(str, "/dump-world"))
    {
        srand(timeGetTime());
        unsigned int r = rand();
        char name[MAX_PATH];
        sprintf(name, "rge_dump_%08X.txt", r);
        chat("Dumping world to %s ...", name);
        dump_objects(name);
        chat("Dump complete");
        return true;
    }
    else if (!strcmp(str, "/worldtime"))
    {
        chat("Worldtime = %u", get_worldtime());
        return true;
    }
    else if (strstr(str, "/set-max"))
    {
        char d[0x100];
        int t;
        sscanf(str, "%s %d", d, &t);
        max_worldtime = t;
        chat("Set max worldtime to %d", t);
        return true;
    }
    /*
    else if (!strcmp(str, "/dump-all"))
    {
        make_oos_dump();
        return true;
    }
    */
    else if (strstr(str, "/obj"))
    {
        char d[0x100];
        int id;
        sscanf(str, "%s %d", d, &id);
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
    else if (strstr(str, "/sel-id"))
    {
        TRIBE_World* world = (*base_game)->world;
        if (world)
        {
            RGE_Player* player = RGE_Base_Game__get_player(*base_game);
            if (player)
            {
                RGE_Static_Object* obj = player->selected_obj;
                if (obj)
                    chat("Selected object %d, master %d, sleep = %d", obj->id, obj->master_obj->id, obj->sleep_flag);
                else
                    chat("No objects selected");
            }
        }

        return true;
    }
    else if (strstr(str, "/goto"))
    {
        char d[0x100];
        float x, y;
        sscanf(str, "%s %f %f", d, &x, &y);
        RGE_Player__set_view_loc(RGE_Base_Game__get_player(*base_game), x, y, 0);
        return true;
    }
    else if (strstr(str, "/cs"))
    {
        WORLD_DUMP wd;
        wd.update_cs();
        chat("CS=%u", wd.get_cs());
        return true;
    }
    /*
    else if (!strcmp(str, "/set-zero-count"))
    {
        for (auto it = function_hooks.begin(), it_end = function_hooks.end(); it != it_end; ++it)
            (*it)->set_zero_call_count();
        chat("Set call counts to zero");
        return true;
    }
    else if (!strcmp(str, "/print-call-count"))
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
        return true;
    }
    */
    else if (!strcmp(str, "/render-off"))
    {
        chat("Render OFF");
        unsigned char instr = 0xC3;
        SIZE_T w;
        WriteProcessMemory(GetCurrentProcess(), (void*)0x00651BC0, &instr, 1, &w);
        return true;
    }
    else if (!strcmp(str, "/render-on"))
    {
        chat("Render ON");
        unsigned char instr = 0x55;
        SIZE_T w;
        WriteProcessMemory(GetCurrentProcess(), (void*)0x00651BC0, &instr, 1, &w);
        return true;
    }
    else if (strstr(str, "/fog-color"))
    {
        char d[0x100];
        int c;
        sscanf(str, "%s %d", d, &c);
        if (sscanf(str, "%s %d", d, &c) == 2 && c >= 0 && c < 256)
        {
            fog_param_a2 = MAKE_UINT32(c, c, c, c);
            fog_param_a4 = fog_param_a2;
        }
        else
        {
            chat("Invalid color");
        }
        return true;
    }
    else if (strstr(str, "/fog"))
    {
        char d[0x100];
        char param[0x100];
        //uint32_t a1, a3;
        sscanf(str, "%s %s", d, param);
        unsigned __int8 a = 0;
        unsigned __int8 b = 0xFF;
        if (!strcmp(param, "dark"))
        {
            fog_param_a1 = MAKE_UINT32(b, a, b, a); //dark
            fog_param_a3 = MAKE_UINT32(a, a, a, a);
            chat("Set fog pattern: 'dark'");
        }
        else if (!strcmp(param, "light"))
        {
            fog_param_a1 = MAKE_UINT32(b, a, b, a); //light
            fog_param_a3 = MAKE_UINT32(b, b, b, b);
            chat("Set fog pattern: 'light'");
        }
        else if (!strcmp(param, "vert"))
        {
            fog_param_a1 = MAKE_UINT32(b, a, b, a); //vert
            fog_param_a3 = MAKE_UINT32(b, a, b, a);
            chat("Set fog pattern: 'vert'");
        }
        else if (!strcmp(param, "horz"))
        {
            fog_param_a1 = MAKE_UINT32(b, b, b, b); //horz
            fog_param_a3 = MAKE_UINT32(a, a, a, a);
            chat("Set fog pattern: 'horz'");
        }
        else if (!strcmp(param, "standard"))
        {
            fog_param_a1 = MAKE_UINT32(a, b, a, b); //standard
            fog_param_a3 = MAKE_UINT32(b, a, b, a);
            chat("Set fog pattern: 'standard'");
        }
        else
        {
            chat("Unknown fog pattern");
        }
        //fog_param_a1 = a1;
        //fog_param_a3 = a3;
        return true;
    }
    
    /*
    else if (strstr(str, "/action"))
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
                    RGE_Sprite* sprite = master->vfptr->get_run_sprite(master, (RGE_Static_Object*)obj);

                    if (!sprite)
                        sprite = master->vfptr->get_move_sprite(master, (RGE_Static_Object*)obj);

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

        return true;
    }
    */
#endif
    return false;
}
