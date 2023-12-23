#include "stdafx.h"
#include "objpanel.h"
#include "resgenbldgs.h"
#include "casts.h"

extern int itemCounter;
extern char* is2ndCol;

extern int numberOfResProducers;
extern resGen* resProducersData;

extern int currentPlayer;
extern float* resources;

extern TRIBE_Panel_Object* objPanelPtr;

extern int current_loaded_version;

void __cdecl objPanel(RGE_Static_Object* unit)
{
    //50731 - SLP in interfac.drs
    //if (unit)
    //    objPanelDrawItem (5, 2, 0, 1, 1, 1, 1, 42013);

    if (!currentPlayer)
        return;

    memset(is2ndCol, 0, 0x10);
    //is2ndCol [itemCounter] = 1;
    //is2ndCol [itemCounter+1] = 1;
    int cntr = 4;
    int localItemCounter = itemCounter;
    int val;

    int frame;
    int langID;

    //RESGEN

    float res;

    for (int i = 0; i < numberOfResProducers; i++)
    {
        if (unit->master_obj->id == resProducersData[i].unitID)
        {
            if (resProducersData[i].useControlRes)
                res = resources[resProducersData[i].controlResID];
            else
                res = ((float)resProducersData[i].constantResAmount);

            if (current_loaded_version >= 6)
            {
                TRIBE_Building_Object* bld_obj = (TRIBE_Building_Object*)unit;
                if (bld_obj->master_obj->master_type == 80)
                    res *= bld_obj->master_obj->work_rate;
            }

            if (resProducersData[i].resID == 0)       //food
            {
                frame = 2;
                langID = 42045;
            }
            else if (resProducersData[i].resID == 1)  //carbon
            {
                frame = 0;
                langID = 42046;
            }
            else if (resProducersData[i].resID == 2)  //ore
            {
                frame = 1;
                langID = 42048;
            }
            else if (resProducersData[i].resID == 3)  //nova
            {
                frame = 3;
                langID = 42047;
            }
            else                                      //unknown
            {
                frame = 5;
                langID = 42049;
            }

            val = res * 100;
            TRIBE_Panel_Object__draw_item(objPanelPtr, localItemCounter++, frame, 6, val, 0, 0, 0, langID);
        }
    }

    /*switch (id) //workers
    {
    case 56: //fisher
    case 57:
        frame = 1;
        langID = 1;
        worker = 1;
    case 120: //forager
    case 354:
        frame = 1;
        langID = 1;
        worker = 1;
    case 122: //hunter
    case 216:
        frame = 1;
        langID = 1;
        worker = 1;
    case 123: //carbon collector
    case 218:
        frame = 1;
        langID = 1;
        worker = 1;
    case 124: //ore gatherer
    case 220:
        frame = 1;
        langID = 1;
        worker = 1;
    case 214: //farmer
    case 259:
        frame = 1;
        langID = 1;
        worker = 1;
    case 579: //nova gatherer
    case 581:
        frame = 1;
        langID = 1;
        worker = 1;
    case 590: //herder
    case 592:
        frame = 1;
        langID = 1;
        worker = 1;
    }

    if (worker)
    {
        workrate = *(float*)((int)propObj+0x110);
        val = workrate * 100;
        objPanelDrawItem (localItemCounter++, frame, 6, val, 0, 0, 0, langID);
    }*/

    //exit if incomplete unit
    if (unit->object_state == 0)
        return;

    //exit if has garrison
    if (unit->vfptr->garrisoned_count(unit) > 0)
        return;

    if (unit->master_obj->master_type == 80)
    {
        TRIBE_Building_Object* building_obj = (TRIBE_Building_Object*)unit;

        __int16 work_type;
        __int16 work_target;
        __int16 progress;
        __int16 master_id;

        //exit if working (researching etc)
        TRIBE_Building_Object__work_status(building_obj, &work_type, &work_target, &progress, NULL, 0);
        switch (work_type)
        {
        case 0x66:
        case 0x67:
        case 0x7B:
        case 0x7C:
        case 0x7D:
            return;
        default:
            break;
        }
        //exit if has units in queue
        if (TRIBE_Building_Object__production_queue_status(building_obj, &master_id, &progress))
            return;
    }

    if (unit->master_obj->master_type >= 70)
    {
        RGE_Combat_Object* combat_obj = (RGE_Combat_Object*)unit;
        if (combat_obj->master_obj->speed > 0.0f)
        {
            val = combat_obj->master_obj->speed * 100;
            is2ndCol[localItemCounter++] = 1;
            TRIBE_Panel_Object__draw_item(objPanelPtr, cntr++, 24, 6, val, 0, 0, 0, 42042);
        }

        if (combat_obj->master_obj->speed_of_attack > 0.0f && combat_obj->master_obj->weapon_num > 0)
        {
            val = combat_obj->master_obj->speed_of_attack * 100;
            is2ndCol[localItemCounter++] = 1;
            TRIBE_Panel_Object__draw_item(objPanelPtr, cntr++, 30, 6, val, 0, 0, 0, 42043);
        }

        if (combat_obj->master_obj->area_effect_range > 0.0f)
        {
            val = combat_obj->master_obj->area_effect_range * 100;
            is2ndCol[localItemCounter++] = 1;
            TRIBE_Panel_Object__draw_item(objPanelPtr, cntr++, 28, 6, val, 0, 0, 0, 42044);
        }

        UNIT_EXTRA* ud = getUnitExtra(unit);
        if (ud && ud->countersUsed)
        {
            if (ud->miscCounter1 > 0.0f)
            {
                val = ud->miscCounter1 * 100;
                is2ndCol[localItemCounter++] = 1;
                TRIBE_Panel_Object__draw_item(objPanelPtr, cntr++, 29, 6, val, 0, 0, 0, 42050);
            }
        }
    }
}
