#include "stdafx.h"

#include "storemode.h"

__declspec(naked) void onBuildingDestruction() //00554429
{
    __asm
    {
        cmp     byte ptr [eax + ebp + 98h], 4
        jb      short loc_554457
        cmp     byte ptr[eax + ebp + 98h], 8
        jz      short loc_554457
        mov     ecx, 00554433h
        jmp     ecx
loc_554457:
        mov     ecx, 00554457h
        jmp     ecx
    }
}

void setStoreModeHooks()
{
    setHook((void*)0x00554429, onBuildingDestruction);
}
