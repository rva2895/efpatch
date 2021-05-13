#include "stdafx.h"

__declspec(naked) void onCheckAttrib() //0054BA83
{
    __asm
    {
        mov     edx, [ecx + 14h]
        mov     bx, [edx + 18h]        //unit id
        cmp     bx, 3643
        jz      _no_draw
        cmp     bx, 3642
        jz      _no_draw
        cmp     bx, 3644
        jz      _no_draw
        cmp     bx, 3645
        jz      _no_draw
        mov     edx, [esp + 8]
        push    esi
        mov     ebx, 0054BA88h
        jmp     ebx
_no_draw:
        mov     ebx, 0054BA7Dh
        jmp     ebx
    }
}

void setAnimatedTerrainHooks()
{
    setHook((void*)0x0054BA83, onCheckAttrib);
}
