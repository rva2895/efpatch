#include "stdafx.h"
#include "aitimer.h"

extern int current_loaded_version;

AITimerManager_new* __fastcall AITimerManager_new__AITimerManager_new(AITimerManager_new* tm, DWORD dummy, TRIBE_World* worldToMeasure)
{
    UNREFERENCED_PARAMETER(dummy);

    tm->world = worldToMeasure;
    tm->timer_count = AI_TIMER_COUNT;
    for (int i = 0; i < 8; i++)
    {
        tm->triggerTime[i] = (unsigned int*)malloc(tm->timer_count * sizeof(unsigned int));
        for (int j = 0; j < tm->timer_count; j++)
            tm->triggerTime[i][j] = UINT_MAX;
    }

    return tm;
}

AITimerManager_new* __fastcall AITimerManager_new__AITimerManager_new2(AITimerManager_new* tm, DWORD dummy, int infile, TRIBE_World* worldToMeasure)
{
    UNREFERENCED_PARAMETER(dummy);

    tm->world = worldToMeasure;

    if (current_loaded_version < 9) //<1.5.5
    {
        tm->timer_count = 21;
        for (int i = 0; i < 8; i++)
        {
            tm->triggerTime[i] = (unsigned int*)malloc(tm->timer_count * sizeof(unsigned int));
            tm->triggerTime[i][0] = UINT_MAX;
            for (int j = 1; j < tm->timer_count; j++)
                rge_read(infile, &tm->triggerTime[i][j], sizeof(unsigned int));
        }
    }
    else
    {
        rge_read(infile, &tm->timer_count, sizeof(tm->timer_count));
        for (int i = 0; i < 8; i++)
        {
            tm->triggerTime[i] = (unsigned int*)malloc(tm->timer_count * sizeof(unsigned int));
            for (int j = 0; j < tm->timer_count; j++)
                rge_read(infile, &tm->triggerTime[i][j], sizeof(unsigned int));
        }
    }

    return tm;
}

void __fastcall AITimerManager_new__destructor(AITimerManager_new* tm)
{
    for (int i = 0; i < 8; i++)
        free(tm->triggerTime[i]);
}

int __fastcall AITimerManager_new__save(AITimerManager_new* tm, DWORD dummy, int outfile)
{
    UNREFERENCED_PARAMETER(dummy);

    rge_write(outfile, &tm->timer_count, sizeof(tm->timer_count));
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < tm->timer_count; j++)
            rge_write(outfile, &tm->triggerTime[i][j], sizeof(unsigned int));

    return 1;
}

void __fastcall AITimerManager_new__disable(AITimerManager_new* tm, DWORD dummy, int playerId, int timerId)
{
    UNREFERENCED_PARAMETER(dummy);

    if (timerId >= 0 && timerId < tm->timer_count)
        tm->triggerTime[playerId - 1][timerId] = UINT_MAX;
}

void __fastcall AITimerManager_new__enable(AITimerManager_new* tm, DWORD dummy, int playerId, int timerId, unsigned int interval)
{
    UNREFERENCED_PARAMETER(dummy);

    if (timerId >= 0 && timerId < tm->timer_count)
        tm->triggerTime[playerId - 1][timerId] = tm->world->world_time + 1000 * interval;
}

int __fastcall AITimerManager_new__triggered(AITimerManager_new* tm, DWORD dummy, int playerId, int timerId)
{
    UNREFERENCED_PARAMETER(dummy);

    if (timerId >= 0 && timerId < tm->timer_count && tm->triggerTime[playerId - 1][timerId] != UINT_MAX)
        return tm->world->world_time >= tm->triggerTime[playerId - 1][timerId];
    else
        return 0;
}

#pragma optimize( "s", on )
void setAITimerHooks()
{
    //new size
    writeDword(0x0058150E, sizeof(AITimerManager_new));
    writeDword(0x0058167E, sizeof(AITimerManager_new));

    setHook((void*)0x00418120, AITimerManager_new__AITimerManager_new);
    setHook((void*)0x00418140, AITimerManager_new__AITimerManager_new2);
    setHook((void*)0x00418180, AITimerManager_new__destructor);
    setHook((void*)0x00418190, AITimerManager_new__save);
    setHook((void*)0x004181D0, AITimerManager_new__disable);
    setHook((void*)0x004181F0, AITimerManager_new__enable);
    setHook((void*)0x00418230, AITimerManager_new__triggered);
}
#pragma optimize( "", on )
