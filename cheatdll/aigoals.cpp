#include "stdafx.h"

#include "ai.h"
#include "aigoals.h"

#define AI_GOAL_MAX 256

__declspec(naked) void aigoal_write_save() //00585CCB
{
    __asm
    {
        mov     edi, [ebx + 108h]
        mov     ebp, AI_GOAL_MAX
        mov     edx, 00585CD6h
        jmp     edx
    }
}

__declspec(naked) void aigoal_load_save() //00583D44
{
    __asm
    {
        push    AI_GOAL_MAX*4
        call    malloc
        add     esp, 4
        mov     [esi + 108h], eax
        mov     edi, eax
        mov     dword ptr [esp + 48h], AI_GOAL_MAX
        mov     edx, 00583D52h
        jmp     edx
    }
}

__declspec(naked) void aigoal_destructor() //00584DF9
{
    __asm
    {
        mov     eax, [esi + 108h]
        push    eax
        call    free
        add     esp, 4
        mov     [esp + 28h], 0Bh
        mov     ecx, 00584E01h
        jmp     ecx
    }
}

__declspec(naked) void aigoal_constructor() //0058363D
{
    __asm
    {
        push    AI_GOAL_MAX*4
        call    malloc
        add     esp, 4
        mov     [esi + 108h], eax
        mov     edx, eax
        mov     eax, edi
        mov     ecx, 00583645h
        jmp     ecx
    }
}

__declspec(naked) void aigoal_goal() //0058AA40
{
    __asm
    {
        mov     eax, [esp + 4]
        test    eax, eax
        jl      short loc_58AA57
        cmp     eax, AI_GOAL_MAX
        jge     short loc_58AA57
        mov     ecx, [ecx + 108h]
        mov     eax, [ecx + eax * 4]
        ret     4
loc_58AA57:
        or      eax, 0FFFFFFFFh
        ret     4
    }
}

__declspec(naked) void aigoal_set_goal() //0058AA60
{
    __asm
    {
        mov     eax, [esp + 4]
        test    eax, eax
        jl      short locret_58AA78
        cmp     eax, AI_GOAL_MAX
        jge     short locret_58AA78
        mov     edx, [esp + 8]
        mov     ecx, [ecx + 108h]
        mov     [ecx + eax * 4], edx
locret_58AA78:
        ret     8
    }
}

#pragma optimize( "s", on )
void setAIGoalsHooks()
{
    writeDword(0x00583646, AI_GOAL_MAX);

    setHook((void*)0x00585CCB, aigoal_write_save);
    setHook((void*)0x00583D44, aigoal_load_save);
    setHook((void*)0x00584DF9, aigoal_destructor);
    setHook((void*)0x0058363D, aigoal_constructor);
    setHook((void*)0x0058AA40, aigoal_goal);
    setHook((void*)0x0058AA60, aigoal_set_goal);
}
#pragma optimize( "", on )
