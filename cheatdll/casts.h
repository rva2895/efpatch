#pragma once

struct UNIT_EXTRA
{
    bool    speedReductionEnabled;
    bool    stealthOffEnabled;
    bool    reloadTimeEnabled;
    bool    hpDrainEnabled;
    bool    hpDrainPercentEnabled;
    bool    countersUsed;

    bool    hasBeenPurged;
    bool    keepUnitExtra;

    unsigned int animalTimer;
    unsigned int kills;
    int     solidifyCounter;

    float   speedReductionModifier;
    float   speedReductionTime;

    float   stealthOffTime;

    float   reloadTimeModifier;
    float   reloadTimeTime;

    float   hpDrainPerSecond;
    float   hpDrainTime;
    float   hpDrainLeftover;

    float   hpDrainPercentPerSecond;
    float   hpDrainPercentTime;
    float   hpDrainPercentLeftover;

    float   miscCounter1;
    float   miscCounter2;
    float   miscCounter3;
    float   miscCounter4;
    float   miscCounter5;
};

struct UNIT_EXTRA_OLD //old definition provided for compatibility
{
    bool    speedReductionEnabled;
    float   speedReductionTime;
    float   speedReductionModifier;

    bool    stealthOffEnabled;
    float   stealthOffTime;

    bool    reloadTimeEnabled;
    float   reloadTimeModifier;
    float   reloadTimeTime;

    bool    hpDrainEnabled;
    float   hpDrainPerSecond;
    float   hpDrainTime;
    float   hpDrainLeftover;

    bool    hpDrainPercentEnabled;
    float   hpDrainPercentPerSecond;
    float   hpDrainPercentTime;
    float   hpDrainPercentLeftover;

    bool    countersUsed;
    float   miscCounter1;
    float   miscCounter2;
    float   miscCounter3;
    float   miscCounter4;
    float   miscCounter5;
};

void setCastHooks();

UNIT_EXTRA* __fastcall getUnitExtra(RGE_Static_Object* unit);
UNIT_EXTRA* __fastcall createUnitExtra(RGE_Static_Object* unit);
void __fastcall addUnitExtra(RGE_Static_Object* unit, UNIT_EXTRA* ud);
void __fastcall removeUnitExtra(RGE_Static_Object* unit);

void __stdcall specialDamage(RGE_Static_Object* unit, short type, int damage, int armor);
