#include "stdafx.h"
#include "hotkeyjump.h"

void* __stdcall findUnit(TRIBE_Player* this_, __int16 id, RGE_Static_Object* begin, __int16 unk)
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
        return TRIBE_Player__find_obj(this_, id, begin, unk);
    }
    else
    {
        RGE_Static_Object* current = TRIBE_Player__find_obj(this_, id, begin, unk);
        begin = current;
        if (current)
            do
            {
                if (current && current->vfptr->is_powered(current, 1))
                    break;
                else
                    current = TRIBE_Player__find_obj(this_, id, current, unk);
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

//jump to gather point crash fix
__declspec(naked) void on_jump_to_gather_point() //00506838
{
    __asm
    {
        test    esi, esi
        jz      skip_jump_to_gather_point
        mov     eax, [esi + 14h]
        cmp     byte ptr [eax + 4], 80
        jb      skip_jump_to_gather_point
        mov     eax, [esi + 214h]
        test    eax, eax
        mov     eax, 00506840h
        jmp     eax
skip_jump_to_gather_point:
        mov     eax, 005068B1h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setHotkeyJumpHooks()
{
    setHook((void*)0x00501FE3, onFindNext);
    setHook((void*)0x00506838, on_jump_to_gather_point);
}
#pragma optimize( "", on )
