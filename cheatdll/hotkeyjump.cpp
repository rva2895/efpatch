#include "stdafx.h"

bool(__thiscall* unit_isPowered)(void *this_, int unk) =
    (bool(__thiscall*) (void*, int))0x0054BDA0;

void* __stdcall findUnit(void* this_, __int16 id, void* begin, __int16 unk)
{
    /*void* current = player_findNextUnit(this_, id, begin, unk);
    while (begin != current)
    {
        if (current && (unit_isPowered(current, 1) || begin))
            break;
        else
        {
            void* prev = current;
            current = player_findNextUnit(this_, id, current, unk);
            if (current == prev)
                break;
        }
    }
    return current;*/
    if (begin)
    {
        return WorldPlayer__find_obj(this_, id, begin, unk);
    }
    else
    {
        void* current = WorldPlayer__find_obj(this_, id, begin, unk);
        begin = current;
        if (current)
            do
            {
                if (current && unit_isPowered(current, 1))
                    break;
                else
                    current = WorldPlayer__find_obj(this_, id, current, unk);
            } while (begin != current);
        return current;
    }
}

__declspec(naked) void onFindNext() //00501FE3
{
    __asm
    {
        push    eax
        call    findUnit
        mov     esi, eax
        mov     eax, 00501FECh
        jmp     eax
    }
}
void setHotkeyJumpHooks()
{
    setHook((void*)0x00501FE3, onFindNext);
}
