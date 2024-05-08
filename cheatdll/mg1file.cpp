#include "stdafx.h"
#include "mg1file.h"
#include "zlib.h"
#include <stdio.h>

#define SCEN_STEP 0x2000

#ifdef S_DEBUG
extern FILE* fptr;
#endif

char* find_player_info_end(char* d, char* p, int n_players, int total)
{
    bool diplo_bad = true;
    while (diplo_bad)
    {
        while (memcmp(p, "\x00\x0B\x01\x0B", 4) && memcmp(p, "\x00\x0B\x03\x0B", 4) &&
            ((p - d) < (total - (4 + 2 + n_players + 9 * 4))))
            p++;

        if ((p - d) >= (total - (2 + 4 + n_players + 9 * 4)))
            return 0;

        diplo_bad = false;

        char* p2 = p + 4;

        for (int i = 0; i < n_players; i++)
            switch (p2[i])
            {
            case 0:
            case 3:
                break;
            default:
                diplo_bad = true;
                break;
            }

        p2 += n_players;
        if (*p2)
            diplo_bad = true;
        p2 += 4;

        long* diplo_array = (long*)p2;
        
        for (int i = 0; i < 8; i++)
        {
            switch (diplo_array[i])
            {
            case 1:
            case 2:
            case 3:
            case 4:
            case -1:
                break;
            default:
                diplo_bad = true;
                break;
            }
        }
        if (diplo_bad)
            p++;
    }
    return p + 4;
}

MG1::MG1(const char* filename)
{
#ifdef _DEBUG
    log("MG1: %s", filename);
#endif

    loaded = false;
    version = NULL;
    d.map = NULL;
    d.players = NULL;
    d.number_of_players = 0;
    d.pop_limit = -1;
    d.game_type = 3;
    map_type = NULL;
    p = NULL;

    FILE* f = fopen(filename, "rb");

    if (!f)
        return;

    long end;
    fread(&end, sizeof(long), 1, f);
    long start = 8;
    if (end < start)
    {
        fclose(f);
        return;
    }
    fseek(f, start, SEEK_SET);

    z_stream_s st;
    memset(&st, 0, sizeof(st));

    int dst_n = SCEN_STEP;
    void* dst = malloc(SCEN_STEP);

    void* data = malloc(end - start);
    fread(data, end - start, 1, f);
    fclose(f);

    int r = inflateInit2(&st, -MAX_WBITS);

    st.avail_in = end - start;
    st.next_in = (Bytef*)data;
    st.avail_out = SCEN_STEP;
    st.next_out = (Bytef*)dst;

    while (r = inflate(&st, Z_NO_FLUSH), r != Z_STREAM_END)
    {
        switch (r)
        {
        case Z_OK:
            dst_n *= 2;
            dst = realloc(dst, dst_n);
            st.next_out = (Bytef*)((char*)dst + dst_n / 2);
            st.avail_out = dst_n / 2;
            break;
        case Z_BUF_ERROR:
            dst_n *= 2;
            dst = realloc(dst, dst_n);
            st.next_out = (Bytef*)dst;
            st.avail_out = dst_n;
            st.next_in = (Bytef*)data;
            st.avail_in = end - start;
            break;
        default:
            inflateEnd(&st);
            free(data);
            free(dst);
            return;
            break;
        }
    }

    int total = st.total_out;

    inflateEnd(&st);
    free(data);

    p = (char*)dst;

    //compressed data loaded

    DWORD ver1 = read4(); //0x00342E39
    DWORD ver2 = read4();
    if (ver1 != 0x20524556)
    {
        free(dst);
        return;
    }
    skip(4);
    DWORD sub_ver = 0;
    switch (ver2)
    {
    case 0x00342E39:
        //version = 0x00312E32;    //9.4 -> 2.1
        version = "1.1";
        break;
    case 0x00322E32:
        //version = 0x00322E32;    //2.2
        version = ">1.1";
        break;
    case 0x00382E39:    //EF 1.4.0
        //version = 0x00382E39;
        version = "1.4.0";
        break;
    case 0x00392E39:    //EF 1.4.1+
        //version = 0x00392E39;
        //sub version
        sub_ver = read4();
        switch (sub_ver)
        {
        case 0:
            version = "1.4.1-p";
            break;
        case 1:
            version = "1.4.1-rc1";
            break;
        case 2:
            version = "1.4.1";
            break;
        case 3:
            version = "1.4.2";
            break;
        case 4:
            version = "1.5.0";
            break;
        case 5:
            version = "1.5.1";
            break;
        case 6:
            version = "1.5.2";
            break;
        case 7:
            version = "1.5.3";
            break;
        case 8: //CURRENT_VERSION
            version = "1.5.4";
            break;
#if CURRENT_VERSION != 8
#error Must update for new CURRENT_VERSION
#endif
        default:
            version = ">1.5.4";
            break;
        }
        break;
    default:
        //version = 0;    //unsupported
        version = "Unknown";
        return;
        break;
    }
    if (read4()) //if AI data exists
    {
        skip(2);
        int num_string = read2();
        skip(4);
        for (int i = 0; i < num_string; i++)
            skip(read4());
        skip(6);
        for (int i = 0; i < 8; i++)
        {
            skip(10);
            int num_rule = read2();
            skip(4);
            for (int j = 0; j < num_rule; j++)
                skip(400+192);
        }
        skip(5544+320);
    }

    skip(4);
    d.game_speed = read4();
    d.start_time = read4();
    skip(33);
    d.my_player_number = read2();
    d.number_of_players = read1();
    skip(62);
    d.map_x = read4();
    d.map_y = read4();
    int n_unk_data = read4();
    for (int i = 0; i < n_unk_data; i++)
    {
        skip(1275);
        skip(d.map_x*d.map_y);
        int n_float = read4();
        skip(4 * n_float);
        skip(4);
    }
    skip(2);
    //map data
    d.map = (TILE*)malloc(2 * d.map_x*d.map_y);
    memcpy(d.map, p, 2 * d.map_x*d.map_y);
    p += 2 * d.map_x*d.map_y;

    skip(4 * d.map_x*d.map_y);

    while (memcmp(p, "\x98\x9E\x00\x00\x02\x0B", 6))
        p++;

    p += 6;

    //******** PLAYER INFO ************
    d.players = (PLAYERINFO*)malloc(d.number_of_players * sizeof(PLAYERINFO));
    for (int pl = 0; pl < d.number_of_players; pl++)
    {
        readN(&d.players[pl].diplomacy_from, d.number_of_players);
        readN(&d.players[pl].diplomacy_to, 9 * 4);
        skip(5);
        d.players[pl].name_len = read2();
        d.players[pl].name = (char*)malloc(d.players[pl].name_len + 1);
        readN(d.players[pl].name, d.players[pl].name_len);
        d.players[pl].name[d.players[pl].name_len] = 0;
        skip(1);
        int n_resources = read4();
        skip(1);
        skip(4 * n_resources);
        skip(18);
        d.players[pl].civ = read1();
        skip(3);
        d.players[pl].color = read1();

        d.players[pl].cc_x = -1;
        d.players[pl].cc_y = -1;

        char* p_test;

        if (pl == (d.number_of_players - 1))
            p_test = (char*)dst + total - 40;
        else
        {
            p_test = find_player_info_end((char*)dst, p, d.number_of_players, total);

            //overflow
            if (!p_test)
            {
                for (int i = 0; i <= pl; i++)
                {
                    free(d.players[0].name);
                    d.players[0].name = 0;
                }
                free(dst);
                dst = 0;
                free(d.players);
                d.players = 0;
                return;
            }
        }

        //find CC
        char* p_cc = p;
        for (; p_cc < p_test; p_cc++)
        {
            if (*p_cc == 80)
                if (*(short*)(p_cc + 2) == 109)
                {
                    float cc_y = *(float*)(p_cc + 31);
                    float cc_x = *(float*)(p_cc + 35);
                    if ((cc_x > 0) && (cc_x < d.map_x) && (cc_y > 0) && (cc_y < d.map_y))
                    {
                        d.players[pl].cc_x = cc_x;
                        d.players[pl].cc_y = cc_y;
                        break;
                    }
                }
        }

        p = p_test;
    }

    //******** MISC **********
    p = (char*)dst + dst_n - 64;
    while (memcmp(p, "Map Type:", 9) && p > dst)
        p--;
    if (p > dst)
    {
        p += 9;
        char* map_type_tmp = (char*)malloc(128);
        char* ptr = map_type_tmp;
        while ((*p != '\n') && ((ptr - map_type_tmp) < 127))
            *ptr++ = *p++;
        *ptr = 0;
        map_type = (char*)malloc(strlen(map_type_tmp) + 1);
        strcpy_safe(map_type, strlen(map_type_tmp) + 1, map_type_tmp);
        free(map_type_tmp);
    }
    else
    {
        map_type = (char*)malloc(10);
        strcpy_safe(map_type, 10, "<Unknown>");
    }

    p = (char*)dst + dst_n - 16;
    while ((memcmp(p, "\x9A\x99\x99\x99\x99\x99\xF9\x3F", 8)) && (p > dst))
        p--;

    p += 8;
    p += 1;
    long n_triggers = read4();

    //triggers
    for (int i = 0; i < n_triggers; i++)
    {
        skip(18);
        skip(read4());
        skip(read4());
        //effects
        long n_effects = read4();
        for (int j = 0; j < n_effects; j++)
        {
            skip(6 * 4);
            long num_selected = read4();
            skip(18 * 4);
            skip(read4());
            skip(read4());
            if (num_selected > 0)
                skip(num_selected * 4);
        }
        skip(n_effects * 4);
        //conditions
        long n_conditions = read4();
        skip(18 * 4 * n_conditions);
        skip(n_conditions * 4);
    }
    skip(n_triggers * 4);

    skip(8); //teams
    skip(13); //unknown
    d.pop_limit = read4();
    d.game_type = read1();
    skip(1); //teams locked

    free(dst);

    d.duration = 0;
    d.duration2 = 0;

    //************* READ COMMANDS *****************
    f = fopen(filename, "rb");
    if (!f)
    {
        log("Error: cannot open file %s", filename);
        return;
    }
    fread(&start, 4, 1, f);
    fread(&end, 4, 1, f);
    long next_chapter = end;
    if (!end)
    {
        fseek(f, 0, SEEK_END);
        end = ftell(f);
    }

    dst = malloc(end - start);
    fseek(f, start, SEEK_SET);
    fread(dst, end - start, 1, f);
    fclose(f);
    p = (char*)dst;

    //game start
    int test = read4();
    if (test != 3)
    {
        free(dst);
        return;
    }
    test = read4();
    if (test != 500)
    {
        free(dst);
        return;
    }
    skip(5 * 4);
    //read commands
    int type;
    int len;
    int command;

    bool first_chapter = true;

    do
    {
        if (!first_chapter)
        {
            f = fopen(filename, "rb");
            if (!f)
            {
                log("Error: cannot open file %s", filename);
                return;
            }
            fseek(f, next_chapter, SEEK_SET);
            fread(&start, 4, 1, f);
            fread(&end, 4, 1, f);
            next_chapter = end;
            if (!end)
            {
                fseek(f, 0, SEEK_END);
                end = ftell(f);
            }

            dst = malloc(end - start);
            fseek(f, start, SEEK_SET);
            fread(dst, end - start, 1, f);
            fclose(f);
            p = (char*)dst;
        }

        while (p < ((char*)dst + end - start))
        {
            type = read4();
            switch (type)
            {
            case 1: //command
                len = read4();
                command = read4();
                skip(len);
                break;
            case 2: //sync
                d.duration += read4();
                skip(4 * 4);
                break;
            case 0: //time sync
                skip(4);
                d.duration2 = read4();
                skip(4 * 4);
                break;
            case 4: //chat
                skip(4);
                len = read4();
#ifdef S_DEBUG
                fputs(p, fptr);
                fputs("\n", fptr);
#endif
                skip(len);    //chat string
                break;
            default: //error!
                free(dst);
                return;
            }
        }
        free(dst);
        first_chapter = false;
    } while (next_chapter);

    //unit counter
    /*if (cnt > 10)
    {
        char b[0x100];
        snprintf(b, _countof(b), "%s: %d aa troops", filename, cnt);
        MessageBox(0, b, "stats", 0);
    }*/

    loaded = true;

#ifdef _DEBUG
    log("MG1: %s loaded", filename);
#endif
}

MG1::~MG1()
{
    free(d.map);
    if (d.players)
    {
        for (int i = 0; i < d.number_of_players; i++)
            free(d.players[i].name);
        free(d.players);
    }
    free(map_type);
}

long MG1::read4()
{
    long r = *(long*)p;
    p += 4;
    return r;
}

short MG1::read2()
{
    short r = *(short*)p;
    p += 2;
    return r;
}

char MG1::read1()
{
    char r = *(char*)p;
    p += 1;
    return r;
}

void MG1::skip(int n)
{
    p += n;
}

void MG1::readN(void* dst, int n)
{
    memcpy(dst, p, n);
    p += n;
}

TEAM MG1::getTeam1()
{
    TEAM t;
    memset(&t, 0, sizeof(TEAM));
    int j = 0;
    for (int i = 1; i < d.number_of_players; i++)
        if (d.players[1].diplomacy_from[i] == 0)
        {
            t.n++;
            t.colors[j] = d.players[i].color;
            t.names[j] = d.players[i].name;
            t.civs[j] = d.players[i].civ;
            t.cc_x[j] = d.players[i].cc_x;
            t.cc_y[j] = d.players[i].cc_y;
            j++;
        }
    return t;
}

TEAM MG1::getTeam2()
{
    TEAM t;
    memset(&t, 0, sizeof(TEAM));
    int j = 0;
    for (int i = 1; i < d.number_of_players; i++)
        if (d.players[1].diplomacy_from[i] != 0)
        {
            t.n++;
            t.colors[j] = d.players[i].color;
            t.names[j] = d.players[i].name;
            t.civs[j] = d.players[i].civ;
            t.cc_x[j] = d.players[i].cc_x;
            t.cc_y[j] = d.players[i].cc_y;
            j++;
        }
    return t;
}

DWORD MG1::getTeams()
{
    char analyzed[8];
    char teams[8];

    memset(analyzed, 0, 8);
    memset(teams, 0, 8);

    int p2 = 0;
    int i = 0;
    while (i < d.number_of_players - 1)
    {
        if (!analyzed[i])
        {
            p2 = 0;
            while (p2 < d.number_of_players - 1)
            {
                if (!analyzed[p2])
                {
                    if (allied(i + 1, p2 + 1))
                    {
                        analyzed[p2] = 1;
                        teams[i]++;
                    }
                }
                p2++;
            }
        }
        i++;
    }
    DWORD t = 0;
    for (int j = 0; j < 8; j++)
    {
        if (teams[j] > 0)
        {
            t <<= 4;
            t |= teams[j];
        }
    }
    return t;
}

int MG1::getNAllies(int player)
{
    int n = 0;
    for (int i = 1; i < d.number_of_players; i++)
        if ((d.players[player].diplomacy_from[i] == 0) && (i != player))
            n++;
    return n;
}

bool MG1::allied(int p1, int p2)
{
    return d.players[p1].diplomacy_from[p2] == 0;
}

char* MG1::getMapType()
{
    return map_type;
}

const char* MG1::getGameType()
{
    switch (d.game_type)
    {
    case 0:
        return "Random Map";
    case 1:
        return "Terminate The Commander";
    case 2:
        return "Death Match";
    case 3:
        return "Scenario";
    case 4:
        return "Campaign";
    case 5:
        return "Commander of the Base";
    case 6:
        return "Monument Race";
    case 7:
        return "Defend the Monument";
    default:
        return "<Unknown>";
    }
}
