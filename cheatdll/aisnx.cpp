#include "stdafx.h"

#include "aisnx.h"

void __fastcall setUnitTypeBuilders(int id, int count)
{
    //builder counters per ID
    uint8_t* buildersPerID = *(uint8_t**)(*(DWORD*)0x0077FDAC + 0x103C);
    buildersPerID[id] = (uint8_t)count;
}

int __fastcall getUnitTypeBuilders(int id, void* player)
{
    uint8_t* buildersPerID = *(uint8_t**)((DWORD)player + 0x103C);
    return buildersPerID[id];
}

const char aSetUnitTypeBuilders[] = "set-builders-type";

__declspec(naked) void registerSNX() //00577B59
{
    __asm
    {
        push    edi
        mov     edi, 00410F60h   //registerAIToken()
        mov     ecx, esi
        push    00577280h   //setSN()
        push    2           //num of tokens
        push    00692940h   //set-strategic-number
        call    edi
        mov     ecx, esi
        push    offset setUnitTypeBuilders
        push    2
        push    offset aSetUnitTypeBuilders
        call    edi
        pop     edi
        mov     ecx, 00577B6Ch
        jmp     ecx
    }
}

#pragma optimize( "s", on )
void setAISNXHooks()
{
    setHook((void*)0x00577B59, registerSNX);
}
#pragma optimize( "", on )
