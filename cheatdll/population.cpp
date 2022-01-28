#include "stdafx.h"
#include "population.h"

__declspec(naked) int createPopList()
{
    __asm
    {
        mov     edi, 5h
        lea     ecx, [edi + edi * 4]
        push    ecx
        mov     eax, 00520279h
        jmp     eax
    }
}

__declspec(naked) int popListContCreation()
{
    __asm
    {
        jle     createPopListCont
        mov     ecx, 006A3684h
        mov     ecx, [ecx]
        mov     edx, 005202ADh
        jmp     edx

createPopListCont:
        lea     ecx, [edi + edi * 4]
        push    ecx
        mov     eax, 00520279h
        jmp     eax
    }
}

const float f1000 = 1000.0;

__declspec(naked) void getPopCap_new() //005EF1B0
{
    __asm
    {
        xor     eax, eax
        mov     al, [ecx + 1446h]
        lea     eax, [eax + eax * 4]
        ret
    }
}

__declspec(naked) void getPopCap_fix1() //005202AD
{
    __asm
    {

        xor     eax, eax
        mov     al, [ecx + 1446h]
        mov     ecx, [esi + 0B28h]
        mov     edx, 005202BDh
        jmp     edx
    }
}

__declspec(naked) void setPopCap_new() //005EF240
{
    __asm
    {
        mov     eax, 0CCCCCCCDh
        mul     dword ptr [esp + 4]
        shr     edx, 2
        mov     [ecx + 1446h], dl
        ret     4
    }
}

char pop_cap_string[0x18];
bool use_extended_pop_cap_ai;

char* __fastcall getPopCapString(int c)
{
    if (!use_extended_pop_cap_ai && c > 250)
        c = 250;
    sprintf_s(pop_cap_string, _countof(pop_cap_string), "POPULATION-CAP-%d", c);
    return pop_cap_string;
}

__declspec(naked) void onAIPopCap() //0057F13A
{
    __asm
    {
        mov     ecx, eax
        call    getPopCapString
        push    eax
        mov     ecx, 0057F197h
        jmp     ecx
    }
}

#pragma optimize( "s", on )
void setPopulationHooks(int version)
{
    use_extended_pop_cap_ai = (version == VER_EF);

    setHook((void*)0x00520273, createPopList);
    setHook((void*)0x005202A5, popListContCreation);
    setHook((void*)0x0057F13A, onAIPopCap);

    setHook((void*)0x005EF1B0, getPopCap_new);
    //setHook((void*)0x005EF240, setPopCap_new);
    writeDword(0x006001E8, (DWORD)&setPopCap_new - 0x006001EC);
    setHook((void*)0x005202AD, getPopCap_fix1);

    //fix 75 -> 15 (75/5)
    writeByte(0x004F1ADE, 15);
    writeByte(0x00508B8E, 15);
    writeByte(0x00520846, 15);
    writeByte(0x005E424E, 15);
    writeByte(0x005EC382, 15);

    //getPopCap and eax, 0FFh
    //writeDword(0x005202B9, 0xFFFFFFFF); //useless
    writeDword(0x004412D8, 0xFFFFFFFF);
    writeDword(0x0051E58E, 0xFFFFFFFF);
    writeDword(0x00541D47, 0xFFFFFFFF);
    writeDword(0x0057F136, 0xFFFFFFFF);
    writeDword(0x005FECBA, 0xFFFFFFFF);
    writeDword(0x00601366, 0xFFFFFFFF);
    writeDword(0x0060270D, 0xFFFFFFFF);
    writeDword(0x006031D1, 0xFFFFFFFF);

    //print new pop steps
    writeByte(0x0052029E, 5);
    writeByte(0x005202A1, 0xC8); //1000 / 5 = 200
    writeByte(0x005202A2, 0x00);

    //editor pop limit
    writeDword(0x005CF063, (DWORD)&f1000);
    writeDword(0x005CF074, *(DWORD*)&f1000);
}
#pragma optimize( "", on )
