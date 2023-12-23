#include "stdafx.h"
#include "resgenbldgs.h"

int numberOfResProducers = 0;
resGen* resProducersData = NULL;

bool resGenHooksInstalled = false;

extern int current_loaded_version;

void initBldgResProdList(const char* prefix, const char* filename)
{
    char c;
    int value;
    int id;
    int res;
    log("Loading resource generating buildings list");
    char full_filename[0x100];
    snprintf(full_filename, _countof(full_filename), "%s%s", prefix, filename);
    FILE* f = fopen(full_filename, "rt");
    if (f)
    {
        numberOfResProducers = 0;
        if (resProducersData)
        {
            free(resProducersData);
            resProducersData = NULL;
        }
        while (fscanf(f, "%d %d %c %d", &id, &res, &c, &value) > 0)
        {
            resGen resgen_entry;
            memset(&resgen_entry, 0, sizeof(resGen));
            bool resgen_entry_valid;
            switch (c)
            {
            case 'f':
            case 'c':
                resgen_entry.useControlRes = 0;
                resgen_entry.constantResAmount = value;
                resgen_entry_valid = true;
                break;
            case 'r':
                resgen_entry.useControlRes = 1;
                resgen_entry.controlResID = value;
                resgen_entry_valid = true;
                break;
            default:
                log("Error: invalid resgen option '%c'", c);
                resgen_entry_valid = false;
                break;
            }
            if (resgen_entry_valid)
            {
                numberOfResProducers++;
                resProducersData = (resGen*)realloc(resProducersData, numberOfResProducers * sizeof(resGen));
                resgen_entry.unitID = id;
                resgen_entry.resID = res;
                resProducersData[numberOfResProducers - 1] = resgen_entry;
            }
        }

        fclose(f);

        if (!resGenHooksInstalled)
        {
            setHook((void*)0x005553C6, resGenHook);
            resGenHooksInstalled = true;
        }
    }
    else
        log("Warning: %s not found, resource generating buildings disabled", full_filename);
}

void __stdcall doResGen(TRIBE_Building_Object* obj)
{
    for (int i = 0; i < numberOfResProducers; i++)
    {
        if (obj->master_obj->id == resProducersData[i].unitID)
        {
            float amount;
            if (resProducersData[i].useControlRes)
                amount = obj->owner->attributes[resProducersData[i].controlResID] / 60.0f;
            else
                amount = resProducersData[i].constantResAmount / 60.0f;
            
            if (current_loaded_version >= 6)
            {
                amount *= obj->master_obj->work_rate;
                TRIBE_Player__add_attribute_num(obj->owner, resProducersData[i].resID, amount, 1);
            }
            else
                obj->owner->attributes[resProducersData[i].resID] += amount;
        }
    }
}

__declspec(naked) int resGenHook()
{
    __asm
    {
        push    ecx
        push    esi

        call    doResGen

        pop     ecx
        mov     eax, [esi + 14h];
        cmp     word ptr [eax + 18h], 68h
        mov     eax, 005553CEh
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setResGenHooks()
{
    initBldgResProdList(DATA_FOLDER_PREFIX_FROM_ROOT, "resgen.txt");
}
#pragma optimize( "", on )
