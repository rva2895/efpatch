#include "stdafx.h"

#define DARK_GREY_COLOR 11

bool __fastcall is_carbon_rock(short id)
{
    switch (id)
    {
    case 217:
    case 1117:
    case 1160:
    case 1341:
    case 2089:
    case 2090:
    case 2092:
    case 2093:
    case 2772:
    case 2775:
    case 2777:
    case 4115:
    case 4957:
        return true;
    default:
        return false;
    }
}

__declspec(naked) void grey_load_dat_unit() //0048C45F
{
    __asm
    {
        mov     cx, [edi + 18h]
        call    is_carbon_rock
        test    al, al
        jz      not_carbon_rock
        mov     byte ptr [edi + 9Eh], 0
not_carbon_rock:
        push    1
        lea     edx, [edi + 9Fh]
        mov     eax, 0048C467h
        jmp     eax
    }
}

__declspec(naked) void grey_load_dat_color() //0042E072
{
    __asm
    {
        mov     edx, [esi + 4]
        cmp     edx, 6
        jnz     not_grey
        mov     dword ptr [esi + 20h], DARK_GREY_COLOR
not_grey:
        push    4
        lea     edx, [esi + 24h]
        mov     eax, 0042E077h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setMinimapHooks()
{
    setHook((void*)0x0048C45F, grey_load_dat_unit);
    setHook((void*)0x0042E072, grey_load_dat_color);
}
#pragma optimize( "", on )
