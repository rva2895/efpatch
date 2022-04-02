#include "stdafx.h"

#include "resgenbldgs.h"

int numberOfResProducers = 0;
resGen* resProducersData = NULL;

bool resGenHooksInstalled = false;

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
            numberOfResProducers++;
            resProducersData = (resGen*)realloc(resProducersData, numberOfResProducers * sizeof(resGen));
            resProducersData[numberOfResProducers - 1].unitID = id;
            resProducersData[numberOfResProducers - 1].resID = res;
            if (c == 'f')
            {
                resProducersData[numberOfResProducers - 1].useControlRes = 0;
                resProducersData[numberOfResProducers - 1].constantResAmount = value;
            }
            else
            {
                resProducersData[numberOfResProducers - 1].useControlRes = 1;
                resProducersData[numberOfResProducers - 1].controlResID = value;
            }
        }

        if (!resGenHooksInstalled)
        {
            setHook((void*)0x005553C6, resGenHook);
            resGenHooksInstalled = true;
        }
    }
    else
        log("Warning: %s not found, resource generating buildings disabled", full_filename);
}

void __stdcall doResGen(int unitID, float* resources)
{
    for (int i = 0; i < numberOfResProducers; i++)
    {
        if (unitID == resProducersData[i].unitID)
        {
            if (resProducersData[i].useControlRes)
                resources[resProducersData[i].resID] += (resources[resProducersData[i].controlResID] / 60);
            else
                resources[resProducersData[i].resID] += ((float)resProducersData[i].constantResAmount / 60);
        }
    }
}

__declspec(naked) int resGenHook()
{
    __asm
    {
        push    esi
        push    ecx
        mov     eax, [ecx + 0ACh]
        push    eax //resources
        mov     eax, [esi + 14h]
        xor     ecx, ecx
        mov     cx, word ptr [eax + 18h]
        push    ecx //unit id

        call    doResGen

        pop     ecx
        pop     esi
        mov     eax, [esi + 14h];
        cmp     word ptr [eax + 18h], 68h
        mov     eax, 005553CEh
        jmp     eax
    }
}

void setResGenHooks()
{
    initBldgResProdList(DATA_FOLDER_PREFIX_FROM_ROOT, "resgen.txt");
}
