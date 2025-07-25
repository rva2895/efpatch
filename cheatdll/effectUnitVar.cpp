#include "stdafx.h"

#include "effects.h"
#include "casts.h"
#include "objpanel.h"

template <class T>
void editVal(T* valPtr, float val, bool useMax, float max, int action)
{
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

template <class T>
void editValRangeWrapAround(T* valPtr, float val, float min, float max, int action)
{
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
    if (*valPtr >= max)
        *valPtr = min;
    if (*valPtr < min)
        *valPtr = max;
}

void editHP(RGE_Static_Object* unit, float val, int action)
{
    editVal(&unit->hp, val, false, 0, action);
}

void editHPPercent(RGE_Static_Object* unit, float val, int action)
{
    editVal(&unit->hp, val * unit->master_obj->hp / 100.0f, false, 0, action);
}

void editSP(RGE_Static_Object* unit, float val, int action)
{
    editVal(&unit->sp, val, false, 0, action);
}

void editResources(RGE_Static_Object* unit, float val, int action)
{
    editVal(&unit->attribute_amount_held, val, false, 0, action);
}

void editSPPercent(RGE_Static_Object* unit, float val, int action)
{
    editVal(&unit->sp, val * unit->master_obj->hp / 100.0f, false, 0, action);
}

void editReloadCooldown(RGE_Static_Object* unit, float val, int action)
{
    if (unit->master_obj->master_type >= 50)
    {
        RGE_Combat_Object* combat_obj = (RGE_Combat_Object*)unit;
        editVal(&combat_obj->attack_timer, val, false, 0, action);
    }
}

void editReloadCooldownPercent(RGE_Static_Object* unit, float val, int action)
{
    if (unit->master_obj->master_type >= 50)
    {
        RGE_Combat_Object* combat_obj = (RGE_Combat_Object*)unit;
        editVal(&combat_obj->attack_timer, val * combat_obj->master_obj->speed_of_attack / 100.0f, false, 0, action);
    }
}

void editHPRegen(RGE_Static_Object* unit, float val, float val2)
{
    specialDamage(unit, 47, -val, 0);
    specialDamage(unit, 48, val2, 0);
}

void editHPRegenPercent(RGE_Static_Object* unit, float val, float val2)
{
    specialDamage(unit, 49, -val, 0);
    specialDamage(unit, 50, val2, 0);
}

void editCounter(RGE_Static_Object* unit, float val, int action, int c)
{
    UNIT_EXTRA* ud = createUnitExtra(unit);
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

void editKills(RGE_Static_Object* unit, float val, int action)
{
    UNIT_EXTRA* ud = createUnitExtra(unit);
    ud->keepUnitExtra = true;
    editVal(&ud->kills, val, false, 0, action);
}

void editFacet(RGE_Static_Object* unit, float val, int action)
{
    if (!unit->vfptr->isMoving(unit))
    {
        unsigned __int8 facet = unit->facet;
        float val2 = val * unit->sprite->facet_num / 16;
        editValRangeWrapAround(&facet, val2, 0, unit->sprite->facet_num, action);
        RGE_Static_Object__set_facet(unit, facet);
    }
}

void editCivOverride(RGE_Static_Object* unit, float val, int action)
{
    auto edit_civ_override_val = [=]()
        {
            editVal(&unit->gbg_civ_override, val, false, 0, action);
            TRIBE_Screen_Game* game_screen = ((TRIBE_Game*)(*base_game))->game_screen;
            if (game_screen && game_screen->game_obj == unit)
                game_screen->object_panel->vfptr->set_redraw(game_screen->object_panel, 1);
        };

    if (unit->sprite == unit->master_obj->vfptr->get_sprite(unit->master_obj, unit))
    {
        edit_civ_override_val();
        unit->vfptr->new_sprite(unit, unit->master_obj->vfptr->get_sprite(unit->master_obj, unit));
    }
    else if (unit->sprite == unit->master_obj->vfptr->get_sprite2(unit->master_obj, unit))
    {
        edit_civ_override_val();
        unit->vfptr->new_sprite(unit, unit->master_obj->vfptr->get_sprite2(unit->master_obj, unit));
    }
    else if (unit->sprite == unit->master_obj->vfptr->get_death_sprite(unit->master_obj, unit))
    {
        edit_civ_override_val();
        unit->vfptr->new_sprite(unit, unit->master_obj->vfptr->get_death_sprite(unit->master_obj, unit));
    }
    else if (unit->sprite == unit->master_obj->vfptr->get_undead_sprite(unit->master_obj, unit))
    {
        edit_civ_override_val();
        unit->vfptr->new_sprite(unit, unit->master_obj->vfptr->get_undead_sprite(unit->master_obj, unit));
    }
    else if (unit->master_obj->master_type >= 30 &&
        unit->sprite == ((RGE_Master_Moving_Object*)unit->master_obj)->vfptr->get_move_sprite((RGE_Master_Moving_Object*)unit->master_obj, unit))
    {
        edit_civ_override_val();
        unit->vfptr->new_sprite(unit, ((RGE_Master_Moving_Object*)unit->master_obj)->vfptr->get_move_sprite((RGE_Master_Moving_Object*)unit->master_obj, unit));
    }
    else if (unit->master_obj->master_type >= 30 &&
        unit->sprite == ((RGE_Master_Moving_Object*)unit->master_obj)->vfptr->get_run_sprite((RGE_Master_Moving_Object*)unit->master_obj, unit))
    {
        edit_civ_override_val();
        unit->vfptr->new_sprite(unit, ((RGE_Master_Moving_Object*)unit->master_obj)->vfptr->get_run_sprite((RGE_Master_Moving_Object*)unit->master_obj, unit));
    }
    else if (unit->master_obj->master_type >= 50 &&
        unit->sprite == ((RGE_Master_Combat_Object*)unit->master_obj)->vfptr->get_fight_sprite((RGE_Master_Combat_Object*)unit->master_obj, unit))
    {
        edit_civ_override_val();
        unit->vfptr->new_sprite(unit, ((RGE_Master_Combat_Object*)unit->master_obj)->vfptr->get_fight_sprite((RGE_Master_Combat_Object*)unit->master_obj, unit));
    }
    else
    {
        edit_civ_override_val();
    }
}

bool __stdcall effectUnitVar_do_single_line_effect(
    RGE_Static_Object* unit,
    char* str,
    bool do_effect,
    char* error_msg,
    size_t error_msg_size)
{
    int action;
    char var[32];
    var[0] = '\0';
    float val = 0.0f;
    float val2 = 0.0f;
    char* s_heap = NULL;
    char s_stack[0x100];
    size_t str_size = strlen(str) + 1;
    size_t s_size;
    char* s;
    char error_msg_local[0x100];

    auto log_error_and_return = [&](const char* format, ...)
        {
            va_list ap;
            va_start(ap, format);
            vsnprintf(error_msg_local, _countof(error_msg_local), format, ap);
            log("%s", error_msg_local);
            if (error_msg)
                strlcpy(error_msg, error_msg_local, error_msg_size);
            va_end(ap);
            free(s_heap);
            return false;
        };

    if (str_size > _countof(s_stack))
    {
        s_heap = (char*)malloc(str_size);
        s = s_heap;
        s_size = str_size;
    }
    else
    {
        s = s_stack;
        s_size = _countof(s_stack);
    }
    strlcpy(s, str, s_size);
    char* pch = strtok(s, " ");
    if (pch)
    {
        if (!strcmp(pch, "SET"))
            action = 0;
        else if (!strcmp(pch, "ADD"))
            action = 1;
        else if (!strcmp(pch, "MUL"))
            action = 2;
        else
            return log_error_and_return("Error: unknown command: %s", pch);
    }
    else
    {
        return log_error_and_return("Error: string parse failed");
    }

    pch = strtok(NULL, " ");
    if (pch)
        strlcpy(var, pch, _countof(var));
    else
    {
        return log_error_and_return("Error: string parse failed");
    }

    pch = strtok(NULL, " ");
    if (pch)
        sscanf_s(pch, "%f", &val);
    else
    {
        return log_error_and_return("Error: string parse failed");
    }
    if (!strcmp(var, "HP"))
    {
        do_effect ? editHP(unit, val, action) : noop;
    }
    else if (!strcmp(var, "HPPercent"))
    {
        do_effect ? editHPPercent(unit, val, action) : noop;
    }
    else if (!strcmp(var, "SP"))
    {
        do_effect ? editSP(unit, val, action) : noop;
    }
    else if (!strcmp(var, "SPPercent"))
    {
        do_effect ? editSPPercent(unit, val, action) : noop;
    }
    else if (!strcmp(var, "ReloadCooldown"))
    {
        do_effect ? editReloadCooldown(unit, val, action) : noop;
    }
    else if (!strcmp(var, "ReloadCooldownPercent"))
    {
        do_effect ? editReloadCooldownPercent(unit, val, action) : noop;
    }
    else if (!strcmp(var, "Resources"))
    {
        do_effect ? editResources(unit, val, action) : noop;
    }
    else if (!strcmp(var, "MiscCounter1"))
    {
        do_effect ? editCounter(unit, val, action, 1) : noop;
    }
    else if (!strcmp(var, "MiscCounter2"))
    {
        do_effect ? editCounter(unit, val, action, 2) : noop;
    }
    else if (!strcmp(var, "MiscCounter3"))
    {
        do_effect ? editCounter(unit, val, action, 3) : noop;
    }
    else if (!strcmp(var, "MiscCounter4"))
    {
        do_effect ? editCounter(unit, val, action, 4) : noop;
    }
    else if (!strcmp(var, "MiscCounter5"))
    {
        do_effect ? editCounter(unit, val, action, 5) : noop;
    }
    else if (!strcmp(var, "HPRegen"))
    {
        pch = strtok(NULL, " ");
        if (pch)
        {
            sscanf_s(pch, "%f", &val2);
            do_effect ? editHPRegen(unit, val, 10 * val2) : noop;
        }
        else
        {
            return log_error_and_return("Error: string parse failed");
        }
    }
    else if (!strcmp(var, "HPRegenPercent"))
    {
        pch = strtok(NULL, " ");
        if (pch)
        {
            sscanf_s(pch, "%f", &val2);
            do_effect ? editHPRegenPercent(unit, val, 10 * val2) : noop;
        }
        else
        {
            return log_error_and_return("Error: string parse failed");
        }
    }
    else if (!strcmp(var, "Kills"))
    {
        do_effect ? editKills(unit, val, action) : noop;
    }
    else if (!strcmp(var, "Facet"))
    {
        do_effect ? editFacet(unit, val, action) : noop;
    }
    else if (!strcmp(var, "CivOverride"))
    {
        do_effect ? editCivOverride(unit, val, action) : noop;
    }
    else
    {
        return log_error_and_return("Error: unknown variable %s", var);
    }

    if (str_size > _countof(s_stack))
        free(s_heap);

    if (error_msg && error_msg_size > 0)
        error_msg[0] = '\0';

    return true;
}

bool __stdcall effectUnitVar_do_multi_line_effect(
    RGE_Static_Object* unit,
    char* str,
    bool do_effect,
    char* error_msg,
    size_t error_msg_size)
{
    char* s_heap = NULL;
    char s_stack[0x400];
    size_t str_size = strlen(str) + 1;
    size_t s_size;
    char* s;
    bool result = true;

    if (str_size > _countof(s_stack))
    {
        s_heap = (char*)malloc(str_size);
        s = s_heap;
        s_size = str_size;
    }
    else
    {
        s = s_stack;
        s_size = _countof(s_stack);
    }
    strlcpy(s, str, s_size);
    char* pch = strtok(s, "\r\n");
    char* com_strs[64];
    int str_count = 0;
    while (pch && str_count < 64)
    {
        com_strs[str_count++] = pch;
        pch = strtok(NULL, "\r\n");
    }
    for (int i = 0; i < str_count; i++)
        if (result)
            result = effectUnitVar_do_single_line_effect(unit, com_strs[i], do_effect, error_msg, error_msg_size);
        else
            effectUnitVar_do_single_line_effect(unit, com_strs[i], do_effect, NULL, 0);

    if (str_size > _countof(s_stack))
        free(s_heap);

    return result;
}
