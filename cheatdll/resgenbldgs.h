#pragma once

void initBldgResProdList(const char* prefix, const char* filename);
int resGenHook();
void setResGenHooks();

typedef struct resGen
{
    int unitID;
    int useControlRes;
    int controlResID;
    int constantResAmount;
    int resID;
} resGen;
