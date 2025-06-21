#include "stdafx.h"
#include "worlddump.h"

#include <time.h>
#include <MMSystem.h>
#include <zlib.h>

#include <process.h>
#include <algorithm>

//int max_worldtime = 3256380; //checksums start to diverge, 01_1_rva_recorded game - 2021-05-07_23-53-08
//int max_worldtime = 1673840; //last working in both recs
int max_worldtime = INT_MAX;
//int max_worldtime = 1703840; //last, stuck after
//int max_worldtime = 1661420; //actual desync point

const char rec_stop_str[] = "Reached break point: replaying stopped - worldtime: %ld";

unsigned int player_checksumInternalWithRestore(RGE_Player* player)
{
    char player_20 = *((char*)player + 0x20);
    int player_unk_14 = *(int*)((uint8_t*)player + 0x14);
    int player_unk_18 = *(int*)((uint8_t*)player + 0x18);
    int player_unk_1C = *(int*)((uint8_t*)player + 0x1C);

    unsigned int retval = RGE_Player__create_checksum(player);

    *((char*)player + 0x20) = player_20;
    *(int*)((uint8_t*)player + 0x14) = player_unk_14;
    *(int*)((uint8_t*)player + 0x18) = player_unk_18;
    *(int*)((uint8_t*)player + 0x1C) = player_unk_1C;

    return retval;
}

void __cdecl printf_if_exists(FILE* f, char* format, ...)
{
    if (f)
    {
        va_list ap;
        va_start(ap, format);
        vfprintf(f, format, ap);
        va_end(ap);
    }
}

int __cdecl compare_unit_ordinal(const void* u1, const void* u2)
{
    return ((*(RGE_Static_Object**)u1)->id - (*(RGE_Static_Object**)u2)->id);
}

unsigned int dump_objects(const char* filename)
{
    unsigned int retval;
    FILE* dump_file = NULL;
    if (filename)
        dump_file = fopen(filename, "wt");
    if ((dump_file || !filename) && get_worldtime() >= 0)
    {
        printf_if_exists(dump_file, "Out Of Sync world dump, worldtime=%u\n\n", get_worldtime());
        unsigned int game_checksum = 0;
        RGE_Player** players = (RGE_Player**)(*base_game)->world->players;
        int n_players = (*base_game)->world->player_num;
        for (int p = 0; p < n_players; p++)
            if (players[p])
            {
                printf_if_exists(dump_file, "********\n\nPLAYER %d\n", p);
                unsigned int player_checksum = 0;

                int player_unk_A0 = *(int*)((uint8_t*)players[p] + 0xA0);
                int player_unk_14 = *(int*)((uint8_t*)players[p] + 0x14);
                int player_unk_18 = *(int*)((uint8_t*)players[p] + 0x18);
                int player_unk_1C = *(int*)((uint8_t*)players[p] + 0x1C);

                printf_if_exists(dump_file, "  ID=%d, CS_14=%d, CS_18=%d, CS_1C=%d\n",
                    player_unk_A0, player_unk_14, player_unk_18, player_unk_1C);
                player_checksum += player_unk_A0 + player_unk_14 + player_unk_18 + player_unk_1C;

                float* resources = players[p]->attributes;
                printf_if_exists(dump_file, "  Resources:\n");

                for (int i = 0; i <= MAX_RES; i++)
                {
                    printf_if_exists(dump_file, "    %d=%.2f\n", i, resources[i]);
                    player_checksum += resources[i] * 100;
                }
                printf_if_exists(dump_file, "  Objects:\n");

                int n = players[p]->objects->Number_of_objects;
                if (n > 0)
                {
                    RGE_Static_Object** units_unsorted = players[p]->objects->List;
                    RGE_Static_Object** units = new RGE_Static_Object * [n];
                    memcpy(units, units_unsorted, sizeof(RGE_Static_Object*) * n);
                    qsort(units, n, sizeof(RGE_Static_Object*), compare_unit_ordinal);
                    for (int i = 0; i < n; i++)
                    {
                        unsigned int unit_checksum = 0;
                        printf_if_exists(dump_file, "    %d: x=%.4f, y=%.4f, hp=%.2f, sp=%.2f, r=%d, res=%.2f, unk50=%.2f, st=%d, id=%d\n",
                            units[i]->id,
                            units[i]->world_x,
                            units[i]->world_y,
                            units[i]->hp,
                            units[i]->sp,
                            (int)units[i]->facet,
                            units[i]->attribute_amount_held,
                            units[i]->world_z * 100,
                            (int)units[i]->object_state,
                            (int)units[i]->master_obj->id);
                        unit_checksum +=
                            units[i]->id +
                            units[i]->world_x * 10000 +
                            units[i]->world_y * 10000 +
                            units[i]->hp * 100 +
                            units[i]->sp * 100 +
                            units[i]->facet +
                            units[i]->attribute_amount_held * 100 +
                            units[i]->world_z * 100 +
                            units[i]->object_state +
                            units[i]->master_obj->id;

                        RGE_Static_Object* target = units[i]->vfptr->get_target_obj(units[i]);
                        if (target)
                        {
                            /*if (target->prop_object->unit_class == 8 && target->player != units[i]->player)
                            {
                                chat("Unit %d target gate %d", units[i]->ordinal, target->ordinal);
                            }*/
                            printf_if_exists(dump_file, "      tar=%d\n", target->id);
                            unit_checksum += target->id;
                        }
                        int action_checksum = units[i]->vfptr->get_action_checksum(units[i]);
                        if (action_checksum)
                        {
                            printf_if_exists(dump_file, "      act_cs=%d\n", action_checksum);
                            unit_checksum += action_checksum;
                        }
                        int waypoint_checksum = units[i]->vfptr->get_waypoint_checksum(units[i]);
                        if (waypoint_checksum)
                        {
                            printf_if_exists(dump_file, "      wp_cs=%d\n", waypoint_checksum);
                            unit_checksum += waypoint_checksum;
                        }
                        printf_if_exists(dump_file, "    Object cs=%d\n\n", unit_checksum);
                        player_checksum += unit_checksum;
                    }
                    delete[] units;
                }
                printf_if_exists(dump_file, "Player %d cs=%u\n\n", p, player_checksum);
                game_checksum += player_checksum;
            }
        if (dump_file)
        {
            fprintf(dump_file, "World cs=%u\n", game_checksum);
            fclose(dump_file);
        }
        retval = game_checksum;
    }
    else
        retval = 0;
    return retval;
}

void __stdcall dump_checksums(long time)
{
    unsigned int checksum = dump_objects(NULL);
    //srand(timeGetTime());
    //unsigned int r = rand();
    //char name[MAX_PATH];
    //snprintf(name, _countof(name), "rge_checksum_dump_%08X.txt", r);
    FILE* f = fopen("rge_checksum_dump.txt", "at");
    if (f)
    {
        fprintf(f, "%ld,%u\n", time, checksum);
        fclose(f);
    }
}

void __stdcall pause_game()
{
    TRIBE_Screen_Game* game_screen = ((TRIBE_Game*)(*base_game))->game_screen;
    if (game_screen)
        TRIBE_Screen_Game__command_pause(game_screen);
}

void __stdcall rec_breakpoint_reached()
{
    pause_game();
    chat("Reached breakpoint, worldtime=%u", get_worldtime());
}

__declspec(naked) void onCheckTime()
{
    __asm
    {
        mov     ecx, [ebx]
        push    ecx
        call    dump_checksums

        mov     eax, max_worldtime
        mov     ecx, [ebx]

        cmp     ecx, eax
        jl      no_break

        call    rec_breakpoint_reached
        //mov     ecx, base_game
        //mov     ecx, [ecx]
        //mov     eax, [ecx]
        //call    dword ptr [eax + 18h]
        //mov     ecx, 006ADBB8h
        //mov     eax, 004B76A0h
        //call    eax
        //mov     esi, eax
        //test    esi, esi
        //jz      loc_61FD69
        //mov     ecx, [ebx]
        //push    ecx
        //lea     eax, [ebp - 274h]
        //push    offset rec_stop_str
        //push    eax
        //mov     eax, 006330C4h
        //call    eax //_sprintf
        //add     esp, 0Ch
        //lea     ecx, [ebp - 274h]
        //push    1
        //push    64h
        //push    1C2h
        //push    0
        //mov     eax, 0061FD61h
        //jmp     eax

no_break:
        mov     ecx, [edi + 0Ch]
        mov     edx, [ecx + 1614h]
        mov     eax, 0061EBBCh
        jmp     eax

loc_61FD69:
        mov     eax, 0061FD69h
        jmp     eax
    }
}

void __stdcall dump_expected_checksum(long* checksum)
{
    FILE* f = fopen("rge_expected_checksum_dump.txt", "at");
    if (f)
    {
        fprintf(f, "%u,%ld\n", get_worldtime(), *checksum);
        fclose(f);
    }
}

__declspec(naked) void onChecksumDump() //0042FA49
{
    __asm
    {
        add     esp, 0Ch
        lea     ecx, [esp + 14h]
        push    eax
        push    ecx
        call    dump_expected_checksum
        pop     eax
        cmp     eax, edi
        mov     ecx, 0042FA4Eh
        jmp     ecx
    }
}

/*
WORLD_DUMP_QUEUE wd_queue;

void __stdcall make_oos_dump()
{
    char name[MAX_PATH];

    unsigned int r = rand();
    char r_n[0x20];
    snprintf(r_n, _countof(r_n), "%08X", r);
    time_t rawtime;
    tm* tm_time;
    rawtime = time(NULL);
    tm_time = localtime(&rawtime);
    strftime(name, MAX_PATH, "world_dumps\\World_Dump-%Y-%m-%d_%H-%M-%S_", tm_time);
    strcpy(name + strlen(name), r_n);
    snprintf(r_n, _countof(r_n), "_%u.txt", get_worldtime());
    strcpy(name + strlen(name), r_n);
    FILE* f = fopen(name, "wt");
    if (f)
    {
        wd_queue.print_dump(f);
        fclose(f);
        chat("Saved world dumps to %s", name);
    }
    else
    {
        chat("Error: cannot create %s", name);
    }
}

bool world_dump_enabled = true;

void __stdcall update_dump()
{
    if (world_dump_enabled)
    {
        int worldtime = get_worldtime();
        if ((worldtime >= 0) && (wd_queue.check_worldtime(worldtime)))
        {
            DWORD t1 = timeGetTime();
            WORLD_DUMP wd;
            wd_queue.add_dump(wd);
            DWORD t2 = timeGetTime() - t1;
            log("Added dump: worldtime=%d, dump_time=%d", worldtime, t2);
        }
    }
}

void __stdcall clear_queue()
{
    wd_queue.clear();
}

__declspec(naked) void onOutOfSyncDetected() //005EF8A6
{
    __asm
    {
        mov     eax, 004F9AF0h
        call    eax
        call    make_oos_dump
        mov     eax, 005EF8ABh
        jmp     eax
    }
}

__declspec(naked) void onOutOfSyncDetected2() //005F0645
{
    __asm
    {
        call    make_oos_dump
        mov     eax, 006A3668h
        mov     eax, [eax]
        mov     ecx, 005F064Ah
        jmp     ecx
    }
}

__declspec(naked) void onWorldUpdate() //0061E7F0
{
    __asm
    {
        push    ecx
        call    update_dump
        pop     ecx
        push    ebp
        mov     ebp, esp
        sub     esp, 274h
        mov     eax, 0061E7F9h
        jmp     eax
    }
}

__declspec(naked) void onWorldDelGameInfo() //0061D020
{
    __asm
    {
        push    ecx
        call    clear_queue
        pop     ecx
        push    ebx
        push    esi
        mov     esi, ecx
        xor     ebx, ebx
        mov     eax, 0061D026h
        jmp     eax
    }
}

bool print_stats = false;
unsigned int wp_cs = 0;

__declspec(naked) void __stdcall flush_stack()
{
    __asm
    {
        push    edi
        sub     esp, 2000h
        mov     edi, esp
        //xor eax, eax
        //sub     eax, 1
        mov     eax, 0BAADF00Dh
        mov     ecx, 800h
        rep stosd
        add     esp, 2000h
        pop     edi
        ret
    }
}

void __stdcall unit_update_log_before(RGE_Static_Object* unit)
{
    if ((get_worldtime() == 527900) && (unit->id == 4325))
    {
        wp_cs = unit->vfptr->get_waypoint_checksum(unit);
        print_stats = true;
    }
}

void __stdcall unit_update_log_after(RGE_Static_Object* unit)
{
    if (print_stats)
    {
        chat("Before update: wp_cs=%u, after update: wp_cs=%u", wp_cs, unit->vfptr->get_waypoint_checksum(unit));
        print_stats = false;
    }
}

__declspec(naked) void unit_update_test() //004AF39B
{
    __asm
    {
        push    ecx
        push    edx
        push    ecx
        call    unit_update_log_before
        call    flush_stack
        pop     edx
        pop     ecx
        push    ecx
        call    dword ptr [edx + 94h]
        pop     ecx
        push    eax
        push    ecx
        call    unit_update_log_after
        pop     eax
        mov     ecx, 004AF3A1h
        jmp     ecx
    }
}

DWORD unit_ptr;

__declspec(naked) void unit_update_test2() //00444DF0
{
    __asm
    {
        push    ecx
        push    esi
        mov     unit_ptr, esi
        call    unit_update_log_before
        call    flush_stack
        pop     ecx
        mov     eax, 00409870h
        call    eax
        push    eax
        mov     eax, unit_ptr
        push    eax
        call    unit_update_log_after
        pop     eax
        mov     ecx, 00444DF5h
        jmp     ecx
    }
}

__declspec(naked) void unit_update_test3() //004098EA
{
    __asm
    {
        push    ecx
        mov     eax, [ecx + 4]
        mov     unit_ptr, eax
        push    eax
        call    unit_update_log_before
        //call    flush_stack
        pop     ecx
        mov     edx, [ecx]
        call    dword ptr [edx + 10h]
        push    eax
        mov     eax, unit_ptr
        push    eax
        call    unit_update_log_after
        pop     eax
        mov     ecx, 004098EFh
        jmp     ecx
    }
}

void __stdcall check_action(RGE_Action* action)
{
    if (action->vfptr == (void*)0x00654D5C) //move to
    {
        RGE_Action_Object* unit = action->obj;
        if (unit && unit->id == 4325)
        {
            FILE* f = fopen("act_log.txt", "at");
            if (f)
            {
                //chat("Action found ")
                fprintf(f, "WT=%u, act=%p, x=%.4f, y=%.4f\n", get_worldtime(), action, *((float*)action + 8), *((float*)action + 9));
                fclose(f);
            }
        }
    }
}

void __stdcall check_create_action(RGE_Action_Object* unit, RGE_Action* action, unsigned int* stack)
{
    if (unit && unit->id == 4325 && get_gametime2() == 527900)
    {
        FILE* f = fopen("act_log.txt", "at");
        if (f)
        {
            //chat("Action found ")
            fprintf(f, "WT=%u, NEW act %X, stack: \n", get_worldtime(), action);
            for (int i = 0; i < 20; i++)
                fprintf(f, "  %08X\n", *stack++);
            fclose(f);
        }
        if (get_worldtime() == 527900 && unit->id == 4325)
            __debugbreak();
    }
}

__declspec(naked) void on_create_action() //0040A880
{
    __asm
    {
        push    ecx
        mov     eax, [esp + 8]
        lea     edx, [esp + 4]
        push    edx
        push    ecx
        push    eax
        call    check_create_action
        pop     ecx

        push    ebx
        push    esi
        mov     esi, ecx
        or      eax, 0FFFFFFFFh
        mov     ebx, 0040A887h
        jmp     ebx
    }
}

__declspec(naked) void on_move_to_update() //004084B0
{
    __asm
    {
        push    ecx
        push    esi
        mov     esi, ecx
        push    esi
        call    check_action
        mov     eax, [esi + 10h]
        mov     ecx, 004084B7h
        jmp     ecx
    }
}
*/

void setWorldDumpHooks()
{
    //setHook((void*)0x0040A880, on_create_action);
    //setHook((void*)0x004084B0, on_move_to_update);
    //setHook((void*)0x004098EA, unit_update_test3);

    //setHook((void*)0x0061E7F0, onWorldUpdate);
    //setHook((void*)0x0061D020, onWorldDelGameInfo);
    //setHook((void*)0x005EF8A6, onOutOfSyncDetected);
    //setHook((void*)0x005F0645, onOutOfSyncDetected2);
    setHook((void*)0x0061EBB6, onCheckTime);
    //setHook((void*)0x0042FA49, onChecksumDump);
}

WORLD_DUMP::WORLD_DUMP()
{
    worldtime = get_worldtime();
    if (worldtime >= 0)
    {
        cs = 0;
        RGE_Player** players_ptr = (RGE_Player**)(*base_game)->world->players;
        int n_players = (*base_game)->world->player_num;
        players.reserve(n_players);
        for (int p = 0; p < n_players; p++)
            if (players_ptr[p])
            {
                player_info player;
                player.id = p;
                player.cs = 0;
                
                //Resources
                float* resources = players_ptr[p]->attributes;
                player.resources.reserve(MAX_RES);
                for (int i = 0; i <= MAX_RES; i++)
                    player.resources.push_back(resources[i]);

                //Objects
                int n = players_ptr[p]->objects->Number_of_objects;
                if (n > 0)
                {
                    RGE_Static_Object** units = players_ptr[p]->objects->List;
                    player.objects.reserve(n);
                    for (int i = 0; i < n; i++)
                    {
                        object_info object;
                        object.cs = 0;

                        object.id = units[i]->id;
                        object.x = units[i]->world_x;
                        object.y = units[i]->world_y;
                        object.hp = units[i]->hp;
                        object.sp = units[i]->sp;
                        object.unk50 = units[i]->world_z;
                        object.state = units[i]->object_state;
                        object.rotation = units[i]->facet;
                        object.resources = units[i]->attribute_amount_held;
                        object.unit_id = units[i]->master_obj->id;

                        RGE_Static_Object* target = units[i]->vfptr->get_target_obj(units[i]);
                        if (target)
                            object.target = target->id;
                        else
                            object.target = 0;
                        object.act_cs = units[i]->vfptr->get_action_checksum(units[i]);
                        object.wp_cs = units[i]->vfptr->get_waypoint_checksum(units[i]);
                        
                        player.objects.push_back(object);
                    }
                }
                players.push_back(player);
            }
    }
    deflated = false;
    //cs = 0;
    deflated_data = NULL;
    deflated_data_size = 0;
}

int WORLD_DUMP::get_worldtime() const
{
    return worldtime;
}

void WORLD_DUMP::update_cs()
{
    for (auto it = players.begin(), it_end = players.end(); it != it_end; ++it)
    {
        for (auto it_res = it->resources.begin(), it_res_end = it->resources.end(); it_res != it_res_end; ++it_res)
            it->cs += *it_res * 100;

        for (auto it_obj = it->objects.begin(), it_obj_end = it->objects.end(); it_obj != it_obj_end; ++it_obj)
        {
            it_obj->cs =
                it_obj->id +
                it_obj->x * 10000 +
                it_obj->y * 10000 +
                it_obj->hp * 100 +
                it_obj->sp * 100 +
                it_obj->rotation +
                it_obj->resources * 100 +
                it_obj->unk50 * 100 +
                it_obj->state +
                it_obj->unit_id +
                it_obj->target +
                it_obj->act_cs +
                it_obj->wp_cs;

            it->cs += it_obj->cs;
        }
        cs += it->cs;
    }
}

unsigned int WORLD_DUMP::get_cs() const
{
    return cs;
}

void WORLD_DUMP::sort_objects()
{
    for (auto it = players.begin(), it_end = players.end(); it != it_end; ++it)
        std::sort(it->objects.begin(), it->objects.end());
}

#define CHUNK 16384

int WORLD_DUMP::deflate_dump()
{
    //check if already deflated
    if (deflated)
        return 0;

    //serialize dump
    size_t size = 0;
    size += sizeof(int); //worldtime
    size += sizeof(int); //n players

    for (auto it = players.begin(), it_end = players.end(); it != it_end; ++it)
    {
        size += sizeof(int); //id
        size += sizeof(int); //cs
        size += sizeof(int); //n resources
        size += it->resources.size() * sizeof(float); //resources
        size += sizeof(int); //n objects
        size += it->objects.size() * sizeof(object_info); //objects
    }

    void* memory_in = malloc(size);
    int* p = (int*)memory_in;
    *p++ = worldtime;
    *p++ = players.size();
    for (auto it = players.begin(), it_end = players.end(); it != it_end; ++it)
    {
        *p++ = it->id;
        *p++ = it->cs;
        *p++ = it->resources.size();
        for (auto it_res = it->resources.begin(), it_res_end = it->resources.end(); it_res != it_res_end; ++it_res)
            *(float*)p++ = *it_res;
        *p++ = it->objects.size();
        for (auto it_obj = it->objects.begin(), it_obj_end = it->objects.end(); it_obj != it_obj_end; ++it_obj)
            *(object_info*)p++ = *it_obj;
    }

    int ret, flush;
    unsigned have;
    z_stream strm;
    //unsigned char in[CHUNK];
    //unsigned char out[CHUNK];
    void* out = malloc(size);
    //allocate deflate state
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (ret != Z_OK)
    {
        free(out);
        return ret;
    }
    //compress until end of file
    
        strm.avail_in = size;

        //flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = (uint8_t*)memory_in;
        //run deflate() on input until output buffer not full, finish
        ///compression if all of source has been read in

            strm.avail_out = size;
            strm.next_out = (uint8_t*)out;
            ret = deflate(&strm, Z_FINISH);    //no bad return value
            have = size - strm.avail_out;

    //clean up and return
    (void)deflateEnd(&strm);

    deflated_data = malloc(have);
    memcpy(deflated_data, out, have);
    free(out);
    free(memory_in);
    deflated_data_size = have;

    players.clear();

    deflated = true;

    return 0;
}

int WORLD_DUMP::inflate_dump()
{
    /*//check if deflated
    if (!deflated)
        return 0;

    int ret;
    unsigned have;
    z_stream strm;
    //unsigned char in[CHUNK];
    //unsigned char out[CHUNK];
    //allocate inflate state
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;

    strm.avail_in = deflated_data_size;
    strm.next_in = (uint8_t*)deflated_data;

    void* memory_out = malloc(CHUNK);
    strm.avail_out = CHUNK;
    strm.next_out = (uint8_t*)memory_out;*/
    return 0;
}

void WORLD_DUMP::print(FILE* f) const
{
    fprintf(f, "World dump, worldtime=%d\n", worldtime);
    for (auto it = players.begin(), it_end = players.end(); it != it_end; ++it)
    {
        fprintf(f, "  Player %d\n", it->id);
        fprintf(f, "    Resources:\n");
        int res_id = 0;
        for (auto it_res = it->resources.begin(), it_res_end = it->resources.end(); it_res != it_res_end; ++it_res)
        {
            fprintf(f, "      %d=%.2f:\n", res_id, *it_res);
            res_id++;
        }
        fprintf(f, "    Objects:\n");
        for (auto it_obj = it->objects.begin(), it_obj_end = it->objects.end(); it_obj != it_obj_end; ++it_obj)
        {
            fprintf(f, "      %d: x=%.4f, y=%.4f, hp=%.2f, sp=%.2f, r=%d, res=%.2f, unk50=%.2f, st=%d, id=%d\n",
                it_obj->id,
                it_obj->x,
                it_obj->y,
                it_obj->hp,
                it_obj->sp,
                it_obj->rotation,
                it_obj->resources,
                it_obj->unk50,
                it_obj->state,
                it_obj->unit_id);
            if (it_obj->target)
                fprintf(f, "        tar=%d\n", it_obj->target);
            if (it_obj->act_cs)
                fprintf(f, "        act_cs=%d\n", it_obj->act_cs);
            if (it_obj->wp_cs)
                fprintf(f, "        wp_cs=%d\n", it_obj->wp_cs);
        }
        fprintf(f, "  Player %d cs=%u\n", it->id, it->cs);
    }
    fprintf(f, "World cs=%u\n", cs);
}

bool WORLD_DUMP_QUEUE::check_worldtime(int worldtime) const
{
    return world_dump_queue.empty() || (worldtime > world_dump_queue.back().get_worldtime());
}

void WORLD_DUMP_QUEUE::add_dump(const WORLD_DUMP& wd)
{
    int current_worldtime = wd.get_worldtime();
    world_dump_queue.push(wd);
    while (current_worldtime - world_dump_queue.front().get_worldtime() > WORLD_DUMP_QUEUE_MAX_TIME)
        world_dump_queue.pop();
}

void WORLD_DUMP_QUEUE::print_dump(FILE* f)
{
    fprintf(f, "Out of Sync World Dumps, last 30 seconds:\n\n");
    while (!world_dump_queue.empty())
    {
        world_dump_queue.front().update_cs();
        world_dump_queue.front().print(f);
        fprintf(f, "\n");
        world_dump_queue.pop();
    }
}

void WORLD_DUMP_QUEUE::clear()
{
    while (!world_dump_queue.empty())
        world_dump_queue.pop();
}
