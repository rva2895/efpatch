#include "stdafx.h"

#include "ai.h"
#include "aiunitcount.h"
#include "aibuilders.h"
#include "aisnx.h"
#include "aicommandunits.h"
#include "aigoals.h"
#include "sngoal.h"
#include "ailog.h"
#include "aishipyard.h"
#include "aiactions.h"
#include "aitimer.h"
#include "aiunitline.h"

//#include <process.h>

void __fastcall print_token(char* s, short id)
{
    FILE* f = fopen("tokens.txt", "at");
    if (f)
    {
        fprintf(f, "%d %s\n", (int)id, s);
        fclose(f);
    }
}

__declspec(naked) void onAIToken() //00410EC0
{
    __asm
    {
        push    ecx
        mov     ecx, [esp + 8]
        mov     edx, [esp + 0Ch]
        call    print_token
        pop     ecx
        push    ecx
        mov     ax, [esp + 0Ch]
        mov     edx, 00410EC6h
        jmp     edx
    }
}
/*
struct ai_arguments
{
    DWORD arg0;
    DWORD arg4;
    DWORD argthis;
};

int (__thiscall* doAI)(void *, DWORD, DWORD) =
    (int (__thiscall*)(void *, DWORD, DWORD))0x0040F910;

void ai_thread(void* p)
{
    Sleep(5000);
    ai_arguments* args = (ai_arguments*)p;
    doAI((void*)args->argthis, args->arg0, args->arg4);
    free(p);
    return;
}

void __stdcall do_ai(DWORD arg0, DWORD arg4, DWORD argthis)
{
    ai_arguments* ai_struct = (ai_arguments*)malloc(sizeof(ai_arguments));
    ai_struct->arg0 = arg0;
    ai_struct->arg4 = arg4;
    ai_struct->argthis = argthis;
    _beginthread(ai_thread, 0, ai_struct);
}

__declspec(naked) void ondoai() //0059940E
{
    __asm
    {
        mov     eax, [esp]
        mov     edx, [esp + 4]
        mov     ecx, [esi + 738h]
        push    ecx
        push    edx
        push    eax
        call    do_ai
        add     esp, 8
        mov     ecx, [esi + 738h]
        mov     edx, 00599419h
        jmp     edx
    }
}
*/
void __stdcall printdefconst(char* s, short c)
{
    log("defconst %s %d", s, (int)c);
    if (strstr(s, "siege-unit-limit"))
        __debugbreak();
}

__declspec(naked) void ondefconst() //00552FD0
{
    __asm
    {
        mov     edx, [esp + 4]
        mov     eax, [esp + 8]
        push    ecx
        mov     ax, [eax + 2]
        push    eax
        push    edx
        call    printdefconst
        pop     ecx
        sub     esp, 8
        push    ebx
        push    ebp
        push    esi
        mov     eax, 00552FD6h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setAIHooks(int version)
{
    //MP AI selection
    writeNops(0x00515C17, 6);

    //rule limit: 1000 -> 10000
    writeDword(0x006055E2, 10000);
    writeDword(0x00599462, 20000);

    //string table limit: 2500 -> 25000
    writeDword(0x005FF242, 25000);
    writeDword(0x005FF3AD, 25000);
    writeDword(0x006054F0, 25000);

    //fix crash if order too many units
    setAICommandUnitsHooks();

    //shipyard on shore ice
    setAIShipyardHooks();

    setAIUnitlineHooks();

    if (version == VER_EF)
    {
        //setHook((void*)0x00552FD0, ondefconst);

        //setAILogHooks();
        //setHook((void*)0x0059940E, ondoai);
        //

        setAIBuildersCntrHooks();
        setAIUnitCountHooks();
        setAISNXHooks();
        setAIActionsHooks();
        setAITimerHooks();

        setAIGoalsHooks();

        load_goal_sn_txt();

        //civ leader offset
        writeDword(0x005ED17C, 3200);
        writeDword(0x005ED1C2, 3200);
        writeDword(0x005F0A06, 0x31B);

        //unit line fix

        //facts
        //building-type-count
        writeByte(0x0057AEE0, 10);
        writeByte(0x0057AEE5, 0);
        writeByte(0x0057AEE6, 0x7C);    //jle -> jl
        //building-type-count-total
        writeByte(0x0057B000, 10);
        writeByte(0x0057B005, 0);
        writeByte(0x0057B006, 0x7C);    //jle -> jl
        //can-afford-building
        writeByte(0x0057B108, 10);
        writeByte(0x0057B10D, 0);
        writeByte(0x0057B10E, 0x7C);    //jle -> jl
        //can-afford-unit
        writeNops(0x0057B214, 5);
        //can-build
        writeByte(0x0057B2AE, 10);
        writeByte(0x0057B2B3, 0);
        writeByte(0x0057B2B4, 0x7C);    //jle -> jl
        //can-build-with-escrow
        writeByte(0x0057B5DE, 10);
        writeByte(0x0057B5E3, 0);
        writeByte(0x0057B5E4, 0x7C);    //jle -> jl
        //can-train
        writeNops(0x0057B7D7, 5);
        //can-train-with-escrow
        writeNops(0x0057B857, 5);
        //cc-players-building-type-count
        writeByte(0x0057B9EF, 10);
        writeByte(0x0057B9F4, 0);
        writeByte(0x0057B9F5, 0x7C);    //jle -> jl
        //cc-players-unit-type-count
        writeNops(0x0057BD7F, 5);
        //players-building-type-count
        writeByte(0x0057CE6E, 10);
        writeByte(0x0057CE73, 0);
        writeByte(0x0057CE74, 0x7C);    //jle -> jl
        //players-unit-type-count
        writeNops(0x0057DB01, 5);
        //unit-type-count
        writeNops(0x0057E5A5, 5);
        //unit-type-count-total
        writeNops(0x0057E715, 5);
        //unit-available
        writeNops(0x0057E404, 5);
        //building-available
        writeByte(0x0057ADA6, 10);
        writeByte(0x0057ADAB, 0);
        writeByte(0x0057ADAC, 0x7C);    //jle -> jl

        //actions
        //build
        writeByte(0x00575C20, 10);
        writeByte(0x00575C25, 0);
        writeByte(0x00575C26, 0x7C);    //jle -> jl
        //build-forward
        writeByte(0x00575D10, 10);
        writeByte(0x00575D15, 0);
        writeByte(0x00575D16, 0x7C);    //jle -> jl
        //train
        writeNops(0x00577388, 5);
    }
}
#pragma optimize( "", on )
