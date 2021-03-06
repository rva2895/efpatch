#include "stdafx.h"

#include "terrain_gen.h"

__declspec(naked) void onChangeTerrain() //00557C1A
{
    __asm
    {
        cmp     dword ptr [eax + 1DCh], 7
        jnz     _terrain_continue
        mov     ecx, [edi + 14h]
        cmp     word ptr [ecx + 18h], 32h
        jnz     _terrain_no_change
        jmp     _terrain_end
_terrain_continue:
        cmp     dword ptr [eax + 1DCh], 4   //swamp
        jnz     _terrain_end
        mov     ecx, [edi + 14h]
        cmp     word ptr [ecx + 18h], 32h   //if farm - change
        jz      _terrain_end
_terrain_no_change:
        mov     ebx, 00557CE1h
        jmp     ebx
_terrain_end:
        mov     eax, 00557C31h
        jmp     eax
    }
}

__declspec(naked) void onChangeTerrain2() //00557E45
{
    __asm
    {
        cmp     cl, 4             //swamp
        jnz     _terrain_change
        cmp     dl, 7             //farms
        jz      _terrain_change
        cmp     dl, 8
        jz      _terrain_change
        cmp     dl, 1Dh
        jz      _terrain_change
        cmp     dl, 1Eh
        jz      _terrain_change
        cmp     dl, 1Fh
        jz      _terrain_change
        cmp     dl, 30
        jz      _terrain_change
        mov     ecx, 00557E57h
        jmp     ecx
_terrain_change:
        mov     [eax], dl
        mov     esi, [esp + 10h]
        mov     edi, 00557E4Bh
        jmp     edi
    }
}

void setTerrainGenHooks()
{
    //writeByte(0x00557DD6, 53);   //gungan foundation: 53

    setHook((void*)0x00557E45, onChangeTerrain2);
}
