#include "stdafx.h"
#include "cargotrader.h"

extern int current_loaded_version;

//66 83 7A 1E 2D -> cmp     word ptr [edx + 1Eh], 2Dh
//66 83 7B 1E 2D -> cmp     word ptr [ebx + 1Eh], 2Dh

__declspec(naked) void cargoTrader_1() //005CED83
{
    __asm
    {
        cmp     eax, 45
        jz      short loc_5CEDB6
        push    005CED8Bh
        ret
loc_5CEDB6:
        push    005CEDB6h
        ret
    }
}

__declspec(naked) void cargoTrader_2() //005F901A
{
    __asm
    {
        cmp     eax, 931
        jz      short loc_5F9021
        cmp     eax, 4069
        jz      short loc_5F9021
        cmp     eax, 4778
        jz      short loc_5F9021
        cmp     eax, 4779
        jz      short loc_5F9021
        cmp     eax, 5904
        jz      short loc_5F9021
        cmp     eax, 5905
        jz      short loc_5F9021
        push    005F9061h
        ret
loc_5F9021:
        push    005F9021h
        ret
    }
}

__declspec(naked) void onTarget_air() //0041C687
{
    __asm
    {
        cmp     eax, 3Eh
        jz      loc_41C691
        cmp     eax, 3Bh
        jz      loc_41C691
        mov     eax, [esp + 0Ch]
        mov     eax, [eax + 14h]
        cmp     word ptr [eax + 18h], 4069  //cargo freighter
        jz      loc_41C691
        cmp     word ptr [eax + 18h], 4778
        jz      loc_41C691
        cmp     word ptr [eax + 18h], 5904
        jz      loc_41C691
        mov     eax, 0041C6EBh
        jmp     eax
loc_41C691:
        mov     eax, 0041C691h
        jmp     eax
    }
}

__declspec(naked) void onTarget_ground() //0041C6AF
{
    __asm
    {
        cmp     eax, 40h
        jz      loc_41C6EB
        cmp     eax, 3Bh
        jz      loc_41C6EB
        mov     eax, [esp + 0Ch]
        mov     eax, [eax + 14h]
        cmp     word ptr [eax + 18h], 4069  //cargo freighter
        jz      loc_41C6EB
        cmp     word ptr [eax + 18h], 4778
        jz      loc_41C6EB
        cmp     word ptr [eax + 18h], 5904
        jz      loc_41C6EB
        mov     eax, 0041C6B9h
        jmp     eax
loc_41C6EB:
        mov     eax, 0041C6EBh
        jmp     eax
    }
}

bool __fastcall gbg_is_aircraft_new(RGE_Static_Object* obj)
{
    switch (obj->master_obj->object_group)
    {
    case 43:
    case 48:
    case 59:
    case 62:
    case 63:
    case 64:
        return true;
    case 45:
        return current_loaded_version >= 7
            && (obj->master_obj->id == 4069 || obj->master_obj->id == 4778 || obj->master_obj->id == 5904);
    default:
        return false;
    }
}

#pragma optimize( "s", on )
void setCargoTraderHooks()
{
    static BYTE c_edx[] = { 0x66, 0x83, 0x7A, 0x1E, 0x2D, 0x90 };
    writeData(0x0041ECF9, c_edx, 6);
    writeData(0x00572C08, c_edx, 6);
    writeData(0x00572CA2, c_edx, 6);
    writeData(0x00572D5A, c_edx, 6);
    writeData(0x00572F55, c_edx, 6);
    writeData(0x00573134, c_edx, 6);
    c_edx[2] = 0x7B;
    writeData(0x00572E72, c_edx, 6);

    setHook((void*)0x0041C687, onTarget_air);
    setHook((void*)0x0041C6AF, onTarget_ground);

    setHook((void*)0x005CED83, cargoTrader_1);
    setHook((void*)0x005F901A, cargoTrader_2);

    setHook((void*)0x0054B400, gbg_is_aircraft_new);
}
#pragma optimize( "", on )
