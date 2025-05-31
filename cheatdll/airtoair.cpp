#include "stdafx.h"
#include "airtoair.h"

__int16* airtoair_list = NULL;
int airtoair_n = 0;
bool airtoair_installed = false;

int __fastcall get_adjusted_target_type_id(int target_type, short id)
{
    if (is_id_in_list(id, airtoair_list, airtoair_n))
        return 4;
    else
        return target_type;
}

__declspec(naked) void onAirTarget() //0041C658
{
    __asm
    {
        mov     eax, [esi + 14h]
        mov     dx, [eax + 18h]
        call    get_adjusted_target_type_id
        mov     ecx, eax
        cmp     ecx, 0Fh
        movsx   eax, di

        mov     edx, 0041C65Eh
        jmp     edx
    }
}

void __cdecl air_to_air_hooks_atexit()
{
    free(airtoair_list);
}

void __cdecl air_to_air_hooks()
{
    setHook((void*)0x0041C658, onAirTarget);

    efpatch_atexit(air_to_air_hooks_atexit);
}

void initAirToAir(const char* prefix, const char* filename)
{
    load_ids_from_txt(
        prefix,
        filename,
        &airtoair_list,
        &airtoair_n,
        &airtoair_installed,
        air_to_air_hooks,
        "Loading air-to-air unit list"
    );
}
