#include "stdafx.h"
#include "airtoair.h"

short* airToAirIDs = NULL;
int airToAirN = 0;
bool airToAirInstalled = false;

int __stdcall get_adjusted_target_type_id(int target_type, short id)
{
    for (int i = 0; i < airToAirN; i++)
        if (id == airToAirIDs[i])
            return 4;
    return target_type;
}

__declspec(naked) void onAirTarget() //0041C658
{
    __asm
    {
        mov     eax, [esi + 14h]
        mov     ax, [eax + 18h]
        push    eax
        push    ecx
        call    get_adjusted_target_type_id
        mov     ecx, eax
        cmp     ecx, 0Fh
        movsx   eax, di

        mov     edx, 0041C65Eh
        jmp     edx
    }
}

void air_to_air_hooks()
{
    setHook((void*)0x0041C658, onAirTarget);
}

void initAirToAir(const char* prefix, const char* filename)
{
    log("Loading air-to-air unit list");
    char full_filename[0x100];
    snprintf(full_filename, _countof(full_filename), "%s%s", prefix, filename);
    FILE* f = fopen(full_filename, "rt");
    if (f)
    {
        short id;
        airToAirN = 0;
        if (airToAirIDs)
        {
            free(airToAirIDs);
            airToAirIDs = NULL;
        }

        while (fscanf(f, "%hd", &id) > 0)
        {
            airToAirN++;
            airToAirIDs = (short*)realloc(airToAirIDs, airToAirN * sizeof(short));
            airToAirIDs[airToAirN - 1] = id;
        }

        fclose(f);
        if (!airToAirInstalled)
        {
            air_to_air_hooks();
            airToAirInstalled = true;
        }
    }
    else
        log("Warning: %s not found, using default settings", full_filename);
}
