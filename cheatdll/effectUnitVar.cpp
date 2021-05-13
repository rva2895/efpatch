#include "stdafx.h"

#include "effects.h"
#include "casts.h"
#include "structs.h"
#include "objpanel.h"

void editVal (float* valPtr, float val, bool useMax, float max, int action)
{
    //float* hp = (float*)((int)unit+0x3C);
    switch (action)
    {
    case 0: //SET
        *valPtr = val;
        break;
    case 1: //ADD
        *valPtr += val;
        break;
    case 2: //MUL
        *valPtr *= val;
        break;
    default:
        break;
    }
    if (useMax)
        if (*valPtr > max)
            *valPtr = max;
}

void editHP (UNIT* unit, float val, int action)
{
    editVal (&unit->hp, val, false, 0, action);
}

void editHPPercent (UNIT* unit, float val, int action)
{
    float maxHP = unit->prop_object->hit_points;
    editVal (&unit->hp, val*maxHP/100, false, 0, action);
}

void editSP(UNIT* unit, float val, int action)
{
    editVal(&unit->sp, val, false, 0, action);
}

void editResources(UNIT* unit, float val, int action)
{
    editVal(&unit->resources, val, false, 0, action);
}

void editSPPercent (UNIT* unit, float val, int action)
{
    float maxHP = unit->prop_object->hit_points;
    editVal (&unit->sp, val*maxHP/100, false, 0, action);
}

void editReloadCooldown (UNIT* unit, float val, int action)
{
    editVal ((float*)((int)unit+0x174), val, false, 0, action);
}

void editReloadCooldownPercent (UNIT* unit, float val, int action)
{
    float reloadTime = unit->prop_object->reload_time_1;
    editVal ((float*)((int)unit+0x174), val*reloadTime/100, false, 0, action);
}

void editHPRegen (UNIT* unit, float val, float val2)
{
    specialDamage (unit, 47, -val, 0);
    specialDamage (unit, 48, val2, 0);
}

void editHPRegenPercent (UNIT* unit, float val, float val2)
{
    specialDamage (unit, 49, -val, 0);
    specialDamage (unit, 50, val2, 0);
}

void editCounter(UNIT* unit, float val, int action, int c)
{
    UNIT_EXTRA* ud = getUnitExtra(unit);
    if (!ud)
    {
        ud = new UNIT_EXTRA;
        memset(ud, 0, sizeof(UNIT_EXTRA));
        addUnitExtra(unit, ud);
    }
    ud->countersUsed = true;
    switch (c)
    {
    case 1:
        editVal(&ud->miscCounter1, val, false, 0, action);
        break;
    case 2:
        editVal(&ud->miscCounter2, val, false, 0, action);
        break;
    case 3:
        editVal(&ud->miscCounter3, val, false, 0, action);
        break;
    case 4:
        editVal(&ud->miscCounter4, val, false, 0, action);
        break;
    case 5:
        editVal(&ud->miscCounter5, val, false, 0, action);
        break;
    default:
        break;
    }
    //objPanel_invalidate();
}

void __stdcall effectUnitVarActual (UNIT* unit, char* str)
{
    int action;
    char var [50];
    float val;
    float val2;
    char* s = (char*) malloc (strlen(str)+1);
    strcpy (s, str);
    char* pch = strtok (s, " ");
    if (pch)
    {
        if (!strcmp (pch, "SET"))
            action = 0;
        else if (!strcmp (pch, "ADD"))
            action = 1;
        else if (!strcmp (pch, "MUL"))
            action = 2;
        else
        {
            free (s);
            return;
        }
    }
    else
    {
        free (s);
        return;
    }
    pch = strtok (NULL, " ");
    if (pch)
        strcpy (var, pch);

    pch = strtok (NULL, " ");
    if (pch)
        sscanf (pch, "%f", &val);

    if (!strcmp (var, "HP"))
    {
        editHP (unit, val, action);
    }
    else if (!strcmp (var, "HPPercent"))
    {
        editHPPercent (unit, val, action);
    }
    else if (!strcmp (var, "SP"))
    {
        editSP (unit, val, action);
    }
    else if (!strcmp (var, "SPPercent"))
    {
        editSPPercent (unit, val, action);
    }
    else if (!strcmp (var, "ReloadCooldown"))
    {
        editReloadCooldown (unit, val, action);
    }
    else if (!strcmp (var, "ReloadCooldownPercent"))
    {
        editReloadCooldownPercent (unit, val, action);
    }
    else if (!strcmp(var, "Resources"))
    {
        editResources(unit, val, action);
    }
    else if (!strcmp(var, "MiscCounter1"))
    {
        editCounter(unit, val, action, 1);
    }
    else if (!strcmp(var, "MiscCounter2"))
    {
        editCounter(unit, val, action, 2);
    }
    else if (!strcmp(var, "MiscCounter3"))
    {
        editCounter(unit, val, action, 3);
    }
    else if (!strcmp(var, "MiscCounter4"))
    {
        editCounter(unit, val, action, 4);
    }
    else if (!strcmp(var, "MiscCounter5"))
    {
        editCounter(unit, val, action, 5);
    }
    else if (!strcmp (var, "HPRegen"))
    {
        pch = strtok (NULL, " ");
        if (pch)
            sscanf (pch, "%f", &val2);
        editHPRegen (unit, val, 10*val2);
    }
    else if (!strcmp (var, "HPRegenPercent"))
    {
        pch = strtok (NULL, " ");
        if (pch)
            sscanf (pch, "%f", &val2);
        editHPRegenPercent (unit, val, 10*val2);
    }

    free (s);
}
