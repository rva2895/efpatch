#pragma once

#define AI_TIMER_COUNT 256

struct AITimerManager_new
{
    TRIBE_World* world;
    int timer_count;
    unsigned int* triggerTime[8];
};

void setAITimerHooks();
