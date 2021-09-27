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
        cmp     cl, 4             //swamps
        jz      terrain_may_not_change
        cmp     cl, 59
        jz      terrain_may_not_change
        cmp     cl, 142
        jz      terrain_may_not_change
        cmp     cl, 235
        jnz     terrain_change
terrain_may_not_change:
        cmp     dl, 7             //farms
        jz      terrain_change
        cmp     dl, 8
        jz      terrain_change
        cmp     dl, 29
        jz      terrain_change
        cmp     dl, 30
        jz      terrain_change
        cmp     dl, 31
        jz      terrain_change
        cmp     dl, 48
        jz      terrain_change
        cmp     dl, 226
        jz      terrain_change
        cmp     dl, 227
        jz      terrain_change
        cmp     dl, 228
        jz      terrain_change
        mov     ecx, 00557E57h
        jmp     ecx
terrain_change:
        mov     [eax], dl
        mov     esi, [esp + 10h]
        mov     edi, 00557E4Bh
        jmp     edi
    }
}

extern int current_loaded_version;

int __stdcall get_geo_terrain(int terrain)
{
    if (current_loaded_version >= 1)
        switch (terrain)
        {
        case 7:
            return 226;
        case 8:
            return 228;
        case 29:
            return 227;
        default:
            return terrain;
        }
    else
        return terrain;
}

__declspec(naked) void onFarmTerrain() //00557DAA
{
    __asm
    {
        mov     al, [ecx + 0A5h]
        cmp     eax, 2
        jz      farm_gungan
        cmp     eax, 10
        jz      farm_geo
        mov     ecx, 00557DE5h
        jmp     ecx
farm_gungan:
        mov     ecx, 00557DB5h
        jmp     ecx
farm_geo:
        mov     eax, [esp + 1Ch]
        and     eax, 0FFh
        push    eax
        call    get_geo_terrain
        mov     dl, al
        mov     ecx, 00557DF1h
        jmp     ecx
    }
}

__declspec(naked) void onFarmPick() //005FD0F3
{
    __asm
    {
        cmp     al, 7
        jz      pick_farm
        cmp     al, 8
        jz      pick_farm
        cmp     al, 29
        jz      pick_farm
        cmp     al, 30
        jz      pick_farm
        cmp     al, 31
        jz      pick_farm
        cmp     al, 48
        jz      pick_farm
        cmp     al, 226
        jz      pick_farm
        cmp     al, 227
        jz      pick_farm
        cmp     al, 228
        jz      pick_farm
        mov     eax, 005FD221h
        jmp     eax
pick_farm:
        mov     eax, 005FD10Fh
        jmp     eax
    }
}

__declspec(naked) void onFarmDead1() //00557C4E
{
    __asm
    {
        jz      farm_dead1
        cmp     bl, 31
        jz      farm_dead1
        cmp     bl, 228
        jz      farm_dead1
        mov     eax, 00557C5Fh
        jmp     eax
farm_dead1:
        mov     eax, 00557C55h
        jmp     eax
    }
}

__declspec(naked) void onFarmDead2() //00553BB7
{
    __asm
    {
        cmp     eax, 8
        jz      farm_dead2
        cmp     eax, 31
        jz      farm_dead2
        cmp     eax, 228
        jz      farm_dead2
        mov     eax, 00553BC1h
        jmp     eax
farm_dead2:
        mov     eax, 00553BCAh
        jmp     eax
    }
}

__declspec(naked) void onFarmDead3() //005539C8
{
    __asm
    {
        cmp     eax, 8
        jz      farm_dead3
        cmp     eax, 31
        jz      farm_dead3
        cmp     eax, 228
        jz      farm_dead3
        mov     eax, 005539D2h
        jmp     eax
farm_dead3:
        mov     eax, 005539DBh
        jmp     eax
    }
}

void setTerrainGenHooks(int ver)
{
    //writeByte(0x00557DD6, 53);   //gungan foundation: 53

    setHook((void*)0x00557E45, onChangeTerrain2);
    if (ver == VER_EF)
    {
        setHook((void*)0x00557DAA, onFarmTerrain);
        setHook((void*)0x005FD0F3, onFarmPick);
        setHook((void*)0x00557C4E, onFarmDead1);
        setHook((void*)0x00553BB7, onFarmDead2);
        setHook((void*)0x005539C8, onFarmDead3);
    }
}
