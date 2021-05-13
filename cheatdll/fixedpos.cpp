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

void setFixedPosHooks()
{
    setHook((void*)0x00521C29, set_team_together_fix);
    setHook((void*)0x00520643, set_team_together_fix_2);
    setHook((void*)0x004B962E, set_team_together_3_state_init);
    setHook((void*)0x0051F49A, set_team_together_set_3_state_flag);
    setHook((void*)0x00520636, get_team_together_full_fix);
    setHook((void*)0x005EF140, get_team_together_inverted_patch);
}
