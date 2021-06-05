#include "stdafx.h"
#include "worlddump.h"

#include <time.h>
#include <MMSystem.h>
#include <zlib.h>

#include <process.h>

//int max_worldtime = 3256380; //checksums start to diverge, 01_1_rva_recorded game - 2021-05-07_23-53-08
//int max_worldtime = 1673840; //last working in both recs
int max_worldtime = INT_MAX;
//int max_worldtime = 1703840; //last, stuck after
//int max_worldtime = 1661420; //actual desync point

const char rec_stop_str[] = "Reached break point: replaying stopped - worldtime: %ld";

extern int __fastcall get_gametime2();

__declspec(naked) float* __fastcall player_getResources2(void* player)
{
    __asm
    {
        mov     eax, [ecx + 0ACh]
        ret
    }
}

unsigned int(__thiscall* player_checksumInternal)(void* player) =
    (unsigned int(__thiscall*) (void*))0x004C1680;

unsigned int player_checksumInternalWithRestore(void* player)
{
    char player_20 = *((char*)player + 0x20);
    int player_unk_14 = *(int*)((uint8_t*)player + 0x14);
    int player_unk_18 = *(int*)((uint8_t*)player + 0x18);
    int player_unk_1C = *(int*)((uint8_t*)player + 0x1C);

    unsigned int retval = player_checksumInternal(player);

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
        char s[0x100];
        va_list ap;
        va_start(ap, format);
        vfprintf(f, format, ap);
        va_end(ap);
    }
}

__declspec(naked) UNIT* __stdcall RGE_Action_Object__get_target_obj(void* unit)
{
    __asm
    {
        mov     ecx, [esp + 4]
        mov     eax, [ecx]
        call    dword ptr[eax + 148h]
        retn    4
    }
}

__declspec(naked) unsigned int __stdcall RGE_Action_Object__get_action_checksum(void* unit)
{
    __asm
    {
        mov     ecx, [esp + 4]
        mov     eax, [ecx]
        call    dword ptr[eax + 27Ch]
        retn    4
    }
}

__declspec(naked) unsigned int __stdcall RGE_Action_Object__get_waypoint_checksum(void* unit)
{
    __asm
    {
        mov     ecx, [esp + 4]
        mov     eax, [ecx]
        call    dword ptr[eax + 280h]
        retn    4
    }
}


__declspec(naked) char __stdcall get_n_players()
{
    __asm
    {
        mov     ecx, BaseGame_bg
        mov     ecx, [ecx]
        mov     ecx, [ecx + 17B4h]
        mov     ecx, [ecx + 126Ch]
        mov     al, [ecx + 48h]
        ret
    }
}

__declspec(naked) void** __stdcall get_players()
{
    __asm
    {
        mov     ecx, BaseGame_bg
        mov     ecx, [ecx]
        mov     ecx, [ecx + 17B4h]
        mov     ecx, [ecx + 126Ch]
        mov     eax, [ecx + 4Ch]
        ret
    }
}

__declspec(naked) int __fastcall get_player_units_count(void*)
{
    __asm
    {
        mov     ecx, [ecx + 78h]
        mov     eax, [ecx + 8]
        ret
    }
}

__declspec(naked) UNIT** __fastcall get_player_units(void*)
{
    __asm
    {
        mov     ecx, [ecx + 78h]
        mov     eax, [ecx + 4]
        ret
    }
}

int __cdecl compare_unit_ordinal(const void* u1, const void* u2)
{
    return ((*(UNIT**)u1)->ordinal - (*(UNIT**)u2)->ordinal);
}

unsigned int dump_objects(const char* filename)
{
    unsigned int retval;
    FILE* dump_file = NULL;
    if (filename)
        dump_file = fopen(filename, "wt");
    if ((dump_file || !filename) && get_gametime2() >= 0)
    {
        printf_if_exists(dump_file, "Out Of Sync world dump, worldtime=%d\n\n", get_gametime2());
        unsigned int game_checksum = 0;
        void** players = get_players();
        int n_players = get_n_players();
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

                float* resources = player_getResources2(players[p]);
                printf_if_exists(dump_file, "  Resources:\n");

                for (int i = 0; i <= MAX_RES; i++)
                {
                    printf_if_exists(dump_file, "    %d=%.2f\n", i, resources[i]);
                    player_checksum += resources[i] * 100;
                }
                printf_if_exists(dump_file, "  Objects:\n");

                int n = get_player_units_count(players[p]);
                if (n > 0)
                {
                    UNIT** units_unsorted = get_player_units(players[p]);
                    UNIT** units = new UNIT * [n];
                    memcpy(units, units_unsorted, sizeof(UNIT*) * n);
                    qsort(units, n, sizeof(UNIT*), compare_unit_ordinal);
                    for (int i = 0; i < n; i++)
                    {
                        unsigned int unit_checksum = 0;
                        printf_if_exists(dump_file, "    %d: x=%.4f, y=%.4f, hp=%.2f, sp=%.2f, r=%d, res=%.2f, unk50=%.2f, st=%d, id=%d\n",
                            units[i]->ordinal,
                            units[i]->x,
                            units[i]->y,
                            units[i]->hp,
                            units[i]->sp,
                            (int)units[i]->rotation,
                            units[i]->resources,
                            units[i]->unk50 * 100,
                            (int)units[i]->state,
                            (int)units[i]->prop_object->id1);
                        unit_checksum +=
                            units[i]->ordinal +
                            units[i]->x * 10000 +
                            units[i]->y * 10000 +
                            units[i]->hp * 100 +
                            units[i]->sp * 100 +
                            units[i]->rotation +
                            units[i]->resources * 100 +
                            units[i]->unk50 * 100 +
                            units[i]->state +
                            units[i]->prop_object->id1;

                        UNIT* target = RGE_Action_Object__get_target_obj(units[i]);
                        if (target)
                        {
                            /*if (target->prop_object->unit_class == 8 && target->player != units[i]->player)
                            {
                                chat("Unit %d target gate %d", units[i]->ordinal, target->ordinal);
                            }*/
                            printf_if_exists(dump_file, "      tar=%d\n", target->ordinal);
                            unit_checksum += target->ordinal;
                        }
                        unsigned int action_checksum = RGE_Action_Object__get_action_checksum(units[i]);
                        if (action_checksum)
                        {
                            printf_if_exists(dump_file, "      act_cs=%d\n", action_checksum);
                            unit_checksum += action_checksum;
                        }
                        unsigned int waypoint_checksum = RGE_Action_Object__get_waypoint_checksum(units[i]);
                        if (waypoint_checksum)
                        {
                            printf_if_exists(dump_file, "      wp_cs=%d\n", waypoint_checksum);
                            unit_checksum += waypoint_checksum;
                        }
                        //printf_if_exists(dump_file, "    Object cs=%d\n\n", p, player_checksum);
                        player_checksum += unit_checksum;
                    }
                    delete[] units;
                }
                printf_if_exists(dump_file, "Player %d cs=%d\n\n", p, player_checksum);
                game_checksum += player_checksum;
            }
        if (dump_file)
        {
            fprintf(dump_file, "World cs=%d\n", game_checksum);
            fclose(dump_file);
        }
        retval = game_checksum;
    }
    else
        retval = 0;
    return retval;
}

void __stdcall dump_checksums(unsigned int time)
{
    unsigned int checksum = dump_objects(NULL);
    //srand(timeGetTime());
    //unsigned int r = rand();
    //char name[MAX_PATH];
    //sprintf(name, "rge_checksum_dump_%08X.txt", r);
    FILE* f = fopen("rge_checksum_dump.txt", "at");
    if (f)
    {
        fprintf(f, "%d,%u\n", time, checksum);
        fclose(f);
    }
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

        mov     ecx, BaseGame_bg
        mov     ecx, [ecx]
        mov     eax, [ecx]
        call    dword ptr [eax + 18h]
        mov     ecx, 006ADBB8h
        mov     eax, 004B76A0h
        call    eax
        mov     esi, eax
        test    esi, esi
        jz      loc_61FD69
        mov     ecx, [ebx]
        push    ecx
        lea     eax, [ebp - 274h]
        push    offset rec_stop_str
        push    eax
        mov     eax, 006330C4h
        call    eax //_sprintf
        add     esp, 0Ch
        lea     ecx, [ebp - 274h]
        push    1
        push    64h
        push    1C2h
        push    0
        mov     eax, 0061FD61h
        jmp     eax

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

void __stdcall dump_expected_checksum(int* checksum)
{
    FILE* f = fopen("rge_expected_checksum_dump.txt", "at");
    if (f)
    {
        fprintf(f, "%d,%d\n", get_gametime2(), *checksum);
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

WORLD_DUMP_QUEUE wd_queue;

void __stdcall make_oos_dump()
{
    char name[MAX_PATH];

    unsigned int r = rand();
    char r_n[0x20];
    sprintf(r_n, "%08X", r);
    time_t rawtime;
    tm* tm_time;
    rawtime = time(NULL);
    tm_time = localtime(&rawtime);
    strftime(name, MAX_PATH, "world_dumps\\World_Dump-%Y-%m-%d_%H-%M-%S_", tm_time);
    strcpy(name + strlen(name), r_n);
    sprintf(r_n, "_%d.txt", get_gametime2());
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
        int worldtime = get_gametime2();
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

void setWorldDumpHooks()
{
    setHook((void*)0x0061E7F0, onWorldUpdate);
    setHook((void*)0x0061D020, onWorldDelGameInfo);
    setHook((void*)0x005EF8A6, onOutOfSyncDetected);
    //setHook((void*)0x005F0645, onOutOfSyncDetected2);
    //setHook((void*)0x0061EBB6, onCheckTime);
    //setHook((void*)0x0042FA49, onChecksumDump);
}

WORLD_DUMP::WORLD_DUMP()
{
    worldtime = get_gametime2();
    if (worldtime >= 0)
    {
        cs = 0;
        void** players_ptr = get_players();
        int n_players = get_n_players();
        players.reserve(n_players);
        for (int p = 0; p < n_players; p++)
            if (players_ptr[p])
            {
                player_info player;
                player.id = p;
                player.cs = 0;
                
                //Resources
                float* resources = player_getResources2(players_ptr[p]);
                player.resources.reserve(MAX_RES);
                for (int i = 0; i <= MAX_RES; i++)
                    player.resources.push_back(resources[i]);

                //Objects
                int n = get_player_units_count(players_ptr[p]);
                if (n > 0)
                {
                    UNIT** units = get_player_units(players_ptr[p]);
                    player.objects.reserve(n);
                    for (int i = 0; i < n; i++)
                    {
                        object_info object;
                        object.cs = 0;

                        object.id = units[i]->ordinal;
                        object.x = units[i]->x;
                        object.y = units[i]->y;
                        object.hp = units[i]->hp;
                        object.sp = units[i]->sp;
                        object.unk50 = units[i]->unk50;
                        object.state = units[i]->state;
                        object.rotation = units[i]->rotation;
                        object.resources = units[i]->resources;
                        object.unit_id = units[i]->prop_object->id1;

                        UNIT* target = RGE_Action_Object__get_target_obj(units[i]);
                        if (target)
                            object.target = target->ordinal;
                        else
                            object.target = 0;
                        object.act_cs = RGE_Action_Object__get_action_checksum(units[i]);
                        object.wp_cs = RGE_Action_Object__get_waypoint_checksum(units[i]);
                        
                        player.objects.push_back(object);
                    }
                }
                players.push_back(player);
            }
    }
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

void WORLD_DUMP::sort_objects()
{
    for (auto it = players.begin(), it_end = players.end(); it != it_end; ++it)
        std::sort(it->objects.begin(), it->objects.end());
}
/*
#define CHUNK 16384

int WORLD_DUMP::deflate_dump()
{
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
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];
    //allocate deflate state
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (ret != Z_OK)
        return ret;
    //compress until end of file
    
    do {
        strm.avail_in = fread(in, 1, CHUNK, source);
        if (ferror(source)) {
            (void)deflateEnd(&strm);
            return Z_ERRNO;
        }
        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;
        //run deflate() on input until output buffer not full, finish
        ///compression if all of source has been read in
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);    //no bad return value
            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)deflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);
        //done when last data in file processed
    } while (flush != Z_FINISH);
    //clean up and return
    (void)deflateEnd(&strm);
    return Z_OK;
}

void WORLD_DUMP::inflate_dump()
{

}*/

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
        fprintf(f, "  Player %d cs=%d\n", it->id, it->cs);
    }
    fprintf(f, "World cs=%d\n", cs);
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
