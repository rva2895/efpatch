#include "stdafx.h"

std::vector<std::pair<int, std::string>> goal_names;
std::vector<std::pair<int, std::string>> sn_names;

int(__thiscall* get_strategic_number_exe)(void *this_, int sn) =
    (int(__thiscall*)(void*, int))0x005A5710;

int(__thiscall* get_goal_exe)(void *this_, int goal) =
    (int(__thiscall*)(void*, int))0x0058AA40;

int __stdcall get_sn(void* player, int sn)
{
    void* ai = *(void**)((DWORD)player + 0x1DD0);
    if (ai)
    {
        ai = (void*)((DWORD)ai + 0x4CF4);
        return get_strategic_number_exe(ai, sn);
    }
    else
        return -1;
}

int __stdcall get_goal(void* player, int sn)
{
    void* ai = *(void**)((DWORD)player + 0x1DD0);
    if (ai)
    {
        ai = (void*)((DWORD)ai + 0x165C);
        return get_goal_exe(ai, sn-1);
    }
    else
        return -1;
}

bool __stdcall player_is_ai(void* player)
{
    return *(void**)((DWORD)player + 0x1DD0);
}

void load_goal_sn_txt()
{
    log("Loading goal list");
    FILE* f = fopen("data\\goal.txt", "rt");
    if (f)
    {
        char name[0x100];
        int id;
        while (fscanf(f, "%d %s", &id, name) > 0)
        {
            for (char* p = name; *p; ++p) *p = tolower(*p);
            goal_names.push_back(std::pair<int, std::string>(id, name));
        }

        fclose(f);
    }
    else
        log("Warning: goal.txt not found, you cannot use aliases with /goal command");

    log("Loading sn list");
    f = fopen("data\\sn.txt", "rt");
    if (f)
    {
        char name[0x100];
        int id;
        while (fscanf(f, "%d %s", &id, name) > 0)
        {
            for (char* p = name; *p; ++p) *p = tolower(*p);
            sn_names.push_back(std::pair<int, std::string>(id, name));
        }

        fclose(f);
    }
    else
        log("Warning: sn.txt not found, you cannot use aliases with /sn command");
}

void get_sn_with_alias(void* player, char* name_)
{
    int sn = -1;
    for (char* p = name_; *p; ++p) *p = tolower(*p);
    std::string name = name_;
    for (auto it = sn_names.begin(); it != sn_names.end(); ++it)
        if (it->second == name)
            sn = it->first;
    if (sn == -1)
        if (sscanf(name_, "%d", &sn) == 0)
        {
            chat("Unknown strategic number: \"%s\"", name_);
            return;
        }
    if (player_is_ai(player))
        chat("Strategic number %s = %d", name_, get_sn(player, sn));
    else
        chat("Current player does not have an active AI");
}

void get_goal_with_alias(void* player, char* name_)
{
    int goal = -1;
    for (char* p = name_; *p; ++p) *p = tolower(*p);
    std::string name = name_;
    for (auto it = goal_names.begin(); it != goal_names.end(); ++it)
        if (it->second == name)
            goal = it->first;
    if (goal == -1)
        if (sscanf(name_, "%d", &goal) == 0)
        {
            chat("Unknown goal: \"%s\"", name_);
            return;
        }
    if (player_is_ai(player))
        chat("Goal %s = %d", name_, get_goal(player, goal));
    else
        chat("Current player does not have an active AI");
}
