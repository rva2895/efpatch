#include "stdafx.h"
#include "researchrepeat.h"

uint8_t research_param = 0;

__declspec(naked) void on_do_tech_check_param() //005BFEF7
{
    __asm
    {
        mov     al, research_param
        test    al, 1
        jnz     skip_researched_check
        movsx   ecx, word ptr [ebp + 8]
        shl     ecx, 4
        mov     edx, [ebp - 20h]
        mov     eax, [edx]
        movsx   ecx, word ptr [eax + ecx + 4]
        cmp     ecx, 3
        jge     skip_tech
skip_researched_check:
        mov     al, research_param
        test    al, 2
        jnz     skip_civ_check
        movsx   edx, word ptr [ebp + 8]
        shl     edx, 4
        mov     eax, [ebp - 20h]
        mov     ecx, [eax]
        movsx   edx, word ptr [ecx + edx + 4]
        cmp     edx, 0FFFFFFFFh
        jle     skip_tech
skip_civ_check:
        xor     ecx, ecx
        mov     research_param, cl
        mov     eax, 005BFF2Bh
        jmp     eax
skip_tech:
        xor     ecx, ecx
        mov     research_param, cl
        mov     eax, 005C0181h
        jmp     eax
    }
}

__declspec(naked) void on_effect_research_tech() //005F2F72
{
    __asm
    {
        mov     eax, [edi + 0Ch]
        test    eax, 0FFFFFFFCh
        jnz     no_set_param
        mov     research_param, al
no_set_param:
        mov     ecx, [edx + 1D94h]
        mov     eax, 005BFEC9h
        call    eax
        xor     eax, eax
        mov     research_param, al
        mov     eax, 005F2F7Dh
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setResearchRepeatHooks()
{
    setHook((void*)0x005F2F72, on_effect_research_tech);
    setHook((void*)0x005BFEF7, on_do_tech_check_param);
}
#pragma optimize( "", on )
