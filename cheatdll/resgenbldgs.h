#pragma once

void initBldgResProdList(const wchar_t* prefix, const wchar_t* filename);
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
