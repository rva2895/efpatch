#include "stdafx.h"

#include "gamemode.h"

DWORD selectGameModeListJump[] =
{
    0x0051F955,
    0x0051F934,
    0x0051F938,
    0x0051F930,
    0x0051F948,
    0x0051F93C,
    0x0051F940,
    0x0051F944,
    0xFFFFFFFF     //new, edit
};

__declspec(naked) int newModeItem()   //put on 51F908
{
    __asm
    {
        mov     ecx, [esi + 0AF0h]
        push    7
        push    62000
        mov     eax, 004C82D0h
        call    eax
        mov     ecx, [esi + 0AF0h]
        push    0051F90Eh
        ret
    }
}

__declspec(naked) int pushMode()
{
    __asm
    {
        push    7
        push    0051F957h
        ret
    }
}

__declspec(naked) int hookGameModeSet() //put on 521B5D
{
    __asm
    {
        cmp     eax, 7
        jnz     end
        push    8
        push    00521B0Dh
        ret
end:
        mov     ecx, [esi + 0B10h]
        push    00521B63h
        ret
    }
}

void setGameModeHooks()
{
    /*writeByte (0x51F926, 8);
    writeDword (0x51F92C, (int)selectGameModeListJump);
    selectGameModeListJump[8] = (int)&pushMode;
    setHook ((void*)0x51F908, &newModeItem);
    setHook ((void*)0x521B5D, &hookGameModeSet);*/
}
