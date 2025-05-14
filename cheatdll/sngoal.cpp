#include "stdafx.h"

std::vector<std::pair<int, std::string>> goal_names;
std::vector<std::pair<int, std::string>> sn_names;

int __stdcall get_goal(TRIBE_Player* player, int goal)
{
    if (player->playerAI)
        return TribeInformationAIModule__getGoal(&player->playerAI->informationAI, goal);
    else
        return -1;
}

void load_goal_sn_txt()
{
    char name[0x100];
    int id;
    log("Loading goal list");
    FILE* f = fopen(DATA_FOLDER_PREFIX_FROM_ROOT"goal.txt", "rt");
    if (f)
    {
        while (fscanf_s(f, "%d %s", &id, name, _countof(name)) > 0)
        {
            for (char* p = name; *p; ++p) *p = tolower(*p);
            goal_names.emplace_back(std::pair<int, std::string>(id, name));
        }

        fclose(f);
    }
    else
        log("Warning: goal.txt not found, you cannot use aliases with /goal command");

    log("Loading sn list");
    f = fopen(DATA_FOLDER_PREFIX_FROM_ROOT"sn.txt", "rt");
    if (f)
    {
        while (fscanf_s(f, "%d %s", &id, name, _countof(name)) > 0)
        {
            for (char* p = name; *p; ++p) *p = tolower(*p);
            sn_names.emplace_back(std::pair<int, std::string>(id, name));
        }

        fclose(f);
    }
    else
        log("Warning: sn.txt not found, you cannot use aliases with /sn command");
}

void get_sn_with_alias(TRIBE_Player* player, char* name_)
{
    int sn = -1;
    for (char* p = name_; *p; ++p) *p = tolower(*p);
    std::string name = name_;
    for (auto it = sn_names.begin(); it != sn_names.end(); ++it)
        if (it->second == name)
            sn = it->first;
    if (sn == -1)
        if (sscanf_s(name_, "%d", &sn) == 0)
        {
            chat("Unknown strategic number: \"%s\"", name_);
            return;
        }
    if (player->playerAI)
        chat("Strategic number %s = %d", name_, TRIBE_Player__strategicNumber(player, sn));
    else
        chat("Current player does not have an active AI");
}

void get_goal_with_alias(TRIBE_Player* player, char* name_)
{
    int goal = -1;
    for (char* p = name_; *p; ++p) *p = tolower(*p);
    std::string name = name_;
    for (auto it = goal_names.begin(); it != goal_names.end(); ++it)
        if (it->second == name)
            goal = it->first;
    if (goal == -1)
        if (sscanf_s(name_, "%d", &goal) == 0)
        {
            chat("Unknown goal: \"%s\"", name_);
            return;
        }
    if (player->playerAI)
        chat("Goal %s = %d", name_, get_goal(player, goal - 1));
    else
        chat("Current player does not have an active AI");
}
