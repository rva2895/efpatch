#include "stdafx.h"
#include "personinfo.h"

float* const game_info_version = (float* const)0x0068EC1C;

void __stdcall person_info_read(RGE_Person_Info* pi, int infile)
{
    if (*game_info_version < 1.2f)
    {
        pi->queue_overlay_enabled = 0;
        pi->spectator_overlay_enabled = 0;
    }
    else
    {
        rge_read(infile, &pi->queue_overlay_enabled, sizeof(pi->queue_overlay_enabled));
        rge_read(infile, &pi->spectator_overlay_enabled, sizeof(pi->spectator_overlay_enabled));
    }
}

__declspec(naked) void on_person_info_constructor_1() //0047690B
{
    __asm
    {
        push    edi
        push    esi
        call    person_info_read
        mov     ecx, [esp + 10h]
        mov     eax, esi
        mov     esi, 00476911h
        jmp     esi
    }
}

void __stdcall person_info_create(RGE_Person_Info* pi)
{
    pi->queue_overlay_enabled = 0;
    pi->spectator_overlay_enabled = 0;
}

__declspec(naked) void on_person_info_constructor_2() //00476C17
{
    __asm
    {
        push    ebp
        call    person_info_create
        pop     esi
        pop     ebp
        pop     ebx
        retn    10h
    }
}

void __stdcall person_info_save(RGE_Person_Info* pi, int outfile)
{
    rge_write(outfile, &pi->queue_overlay_enabled, sizeof(pi->queue_overlay_enabled));
    rge_write(outfile, &pi->spectator_overlay_enabled, sizeof(pi->spectator_overlay_enabled));
}

__declspec(naked) void on_person_info_save() //004771BE
{
    __asm
    {
        push    esi
        push    edi
        call    person_info_save
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        retn    4
    }
}

#pragma optimize( "s", on )
void setPersonInfoHooks()
{
    setHook((void*)0x0047690B, on_person_info_constructor_1);
    setHook((void*)0x00476C17, on_person_info_constructor_2);
    setHook((void*)0x004771BE, on_person_info_save);

    writeDword(0x0047770E, sizeof(RGE_Person_Info));
    writeDword(0x00477D1A, sizeof(RGE_Person_Info));

    writeDword(0x0047793A, 0x3F99999A); //1.2f
}
#pragma optimize( "", on )
