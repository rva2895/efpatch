#include "stdafx.h"
#include "fixedpos.h"

__declspec(naked) void set_team_together_fix() //00521C29
{
    __asm
    {
        cmp     eax, 0
        jz      _tt_0
        cmp     eax, 1
        jz      _tt_1
        cmp     eax, 2
        jz      _tt_end
_tt_1:
        xor     eax, eax
        jmp     _tt_end
_tt_0:
        mov    eax, 1
_tt_end:
        push    eax
        mov     eax, 00521C2Fh
        jmp     eax
    }
}

__declspec(naked) void set_team_together_fix_2() //00520643
{
    __asm
    {
        cmp     eax, 0
        jz      _tt_0
        cmp     eax, 1
        jz      _tt_1
        cmp     eax, 2
        jz      _tt_end
_tt_1:
        xor     eax, eax
        jmp     _tt_end
_tt_0:
        mov    eax, 1
_tt_end:
        push    eax
        mov     eax, 00520649h
        jmp     eax
    }
}

int init_3_state = 0;

__declspec(naked) void set_team_together_3_state_init() //004B962E
{
    __asm
    {
        mov     ecx, [edi]
        mov     eax, init_3_state
        test    eax, eax
        jz      _tt_no_3_state
        xor     eax, eax
        mov     init_3_state, eax
        push    3
        jmp     _tt_3_state_call
_tt_no_3_state:
        push    2
_tt_3_state_call:
        mov     eax, 004C4CF0h
        call    eax
        mov     eax, 004B9637h
        jmp     eax
    }
}

__declspec(naked) void set_team_together_set_3_state_flag() //0051F49A
{
    __asm
    {
        mov     edx, 1
        mov     init_3_state, edx
        call    dword ptr [eax + 11Ch]
        mov     edx, 0051F4A0h
        jmp     edx
    }
}

__declspec(naked) void get_team_together_inverted_full()
{
    __asm
    {
        xor     eax, eax
        mov     al, [ecx + 1438h]
        retn
    }
}

__declspec(naked) void get_team_together_full_fix() //00520636
{
    __asm
    {
        mov     edi, [eax]
        call    get_team_together_inverted_full
        mov     edx, 0052063Dh
        jmp     edx
    }
}

__declspec(naked) void get_team_together_inverted_patch() //005EF140
{
    __asm
    {
        xor     eax, eax
        mov     al, [ecx + 1438h]
        cmp     al, 2
        jnz     _gtt_p_end
        xor     al, al
_gtt_p_end:
        retn
    }
}

int lowest_unused_color(int* p)
{
    int c = 0;
    for (int i = 0; i < 8; i++)
        if (p[i] == c)

    return c;
}

void __stdcall rnd_pos_fill(int* p)
{
    /*p[0] =
    p[1] = 2;
    p[2] = 3;
    p[3] = 4;
    p[4] = 5;
    p[5] = 6;
    p[6] = 7;
    p[7] = 8;*/
    memset(p, 0, sizeof(int) * 8);
    for (int i = 0; i < 8; i++)
        p[Game__playerColor(*base_game, i) - 1] = i + 1;
    for (int i = 0; i < 8; i++)
        if (p[i] == 0)
            p[i] = lowest_unused_color(p);
}

__declspec(naked) void remove_rnd_pos() //004E789B
{
    __asm
    {
        lea     eax, [esp + 54h]
        xor     ecx, ecx
//color_set_loop:
        //inc     ecx
        //mov     [eax], ecx
        //add     eax, 4
        //cmp     ecx, 8
        //jl      color_set_loop

        push    edx
        push    eax
        call    rnd_pos_fill
        pop     edx

        mov     ecx, [esp + 1Ch]
        mov     ecx, [ecx + 3F6CCh]
        mov     eax, 004E78A5h
        jmp     eax
    }
}

void setFixedPosHooks()
{
    /*setHook((void*)0x00521C29, set_team_together_fix);
    setHook((void*)0x00520643, set_team_together_fix_2);
    setHook((void*)0x004B962E, set_team_together_3_state_init);
    setHook((void*)0x0051F49A, set_team_together_set_3_state_flag);
    setHook((void*)0x00520636, get_team_together_full_fix);
    setHook((void*)0x005EF140, get_team_together_inverted_patch);*/

    //setHook((void*)0x004E789B, remove_rnd_pos);
}
