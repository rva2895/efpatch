#pragma once

struct UNIT_EXTRA
{
    bool    speedReductionEnabled;
    bool    stealthOffEnabled;
    bool    reloadTimeEnabled;
    bool    hpDrainEnabled;
    bool    hpDrainPercentEnabled;
    bool    countersUsed;

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

UNIT_EXTRA* __fastcall getUnitExtra(UNIT* unit);
void __fastcall addUnitExtra(UNIT* unit, UNIT_EXTRA* ud);
void __fastcall removeUnitExtra(UNIT* unit);

void __stdcall specialDamage(UNIT* unit, short type, int damage, int armor);
