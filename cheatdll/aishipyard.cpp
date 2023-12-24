#include "stdafx.h"
#include "aishipyard.h"

int __stdcall check_ai_shipyard_adjacent_tile(TribeInformationAIModule* ai, int x, int y)
{
    return TribeInformationAIModule__hasAdjacentTileType(ai, x, y, 2) || TribeInformationAIModule__hasAdjacentTileType(ai, x, y, 37);
}

__declspec(naked) void on_ai_shipyard() //00587728
{
    __asm
    {
        push    ebx
        push    ebp
        push    esi
        call    check_ai_shipyard_adjacent_tile
        mov     ecx, 00587733h
        jmp     ecx
    }
}

#pragma optimize( "s", on )
void setAIShipyardHooks()
{
    setHook((void*)0x00587728, on_ai_shipyard);
}
#pragma optimize( "", on )
