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

extern void* objPanelPtr;

extern int(__thiscall* objPanelDrawItem) (void*, int, int, int, int, int, int, int, int);

void __cdecl objPanel(UNIT* unit)
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
        if (unit->prop_object->id1 == resProducersData[i].unitID)
        {
            if (resProducersData[i].useControlRes)
                res = resources[resProducersData[i].controlResID];
            else
                res = ((float)resProducersData[i].constantResAmount);

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
            objPanelDrawItem(objPanelPtr, localItemCounter++, frame, 6, val, 0, 0, 0, langID);
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
    if (unit->state == 0)
        return;

    int (__thiscall* RGE_Static_Object__garrisoned_count)(UNIT * this_) =
        (int(__thiscall*)(UNIT*))(*((DWORD*)unit->_vfptr + 162));

    //exit if has garrison
    if (RGE_Static_Object__garrisoned_count(unit) > 0)
        return;

    if (unit->prop_object->type == 80)
    {
        int (__thiscall* TRIBE_Building_Object__work_status)(UNIT* this_, __int16* work_type, __int16* work_target, __int16* progress, char* name, __int16 name_size) =
            (int (__thiscall*)(UNIT*, __int16*, __int16*, __int16*, char*, __int16))(*((DWORD*)unit->_vfptr + 252));

        bool (__thiscall* TRIBE_Building_Object__production_queue_status)(UNIT* this_, __int16* master_id, __int16* progress) =
            (bool (__thiscall*)(UNIT*, __int16*, __int16*))0x00557B20;

        __int16 work_type;
        __int16 work_target;
        __int16 progress;
        __int16 master_id;

        //exit if working (researching etc)
        TRIBE_Building_Object__work_status(unit, &work_type, &work_target, &progress, NULL, 0);
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
        if (TRIBE_Building_Object__production_queue_status(unit, &master_id, &progress))
            return;
    }

    if (unit->prop_object->type >= 70)
    {
        if (unit->prop_object->speed > 0.0f)
        {
            val = unit->prop_object->speed * 100;
            is2ndCol[localItemCounter++] = 1;
            objPanelDrawItem(objPanelPtr, cntr++, 24, 6, val, 0, 0, 0, 42042);
        }

        if ((unit->prop_object->reload_time_1 > 0.0f) &&
            ((unit->prop_object->projectile_unit_id != -1) ||
            (unit->prop_object->unit_class != 18)))
        {
            val = unit->prop_object->reload_time_1 * 100;
            is2ndCol[localItemCounter++] = 1;
            objPanelDrawItem(objPanelPtr, cntr++, 30, 6, val, 0, 0, 0, 42043);
        }

        if (unit->prop_object->blast_radius > 0.0f)
        {
            val = unit->prop_object->blast_radius * 100;
            is2ndCol[localItemCounter++] = 1;
            objPanelDrawItem(objPanelPtr, cntr++, 28, 6, val, 0, 0, 0, 42044);
        }

        UNIT_EXTRA* ud = getUnitExtra(unit);
        if (ud && ud->countersUsed)
        {
            if (ud->miscCounter1 > 0.0f)
            {
                val = ud->miscCounter1 * 100;
                is2ndCol[localItemCounter++] = 1;
                objPanelDrawItem(objPanelPtr, cntr++, 29, 6, val, 0, 0, 0, 42050);
            }
        }
    }
}
