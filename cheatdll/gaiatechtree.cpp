#include "stdafx.h"
#include "gaiatechtree.h"

__declspec(naked) void gaia_tech_tree() //004623C3
{
    __asm
    {
        mov     eax, [esp + 244h]
        test    eax, eax
        jz      short loc_7CB025
        cmp     dword ptr [eax], 00667AB0h
        jnz     short loc_7CB025
        mov     ecx, base_game
        mov     ecx, [ecx]
        mov     eax, [ecx + 420h]
        mov     ecx, [eax + 4Ch]
        mov     eax, [ecx + 4]
        
loc_7CB025:
        mov     ecx, 004623CAh
        jmp     ecx
    }
}

void setGaiaTechTreeHooks()
{
    setHook((void*)0x004623C3, gaia_tech_tree);
}
