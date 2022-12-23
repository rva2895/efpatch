#include "stdafx.h"
#include "playeroptions.h"

#define CURRENT_PLAYER_OPTION_LEVEL 0x0E //old: D, next: E

__declspec(naked) void on_read_option() //004BF64D
{
    __asm
    {
        mov     ecx, 0077D4B0h
        mov     al, [ecx]
        mov     [esi + 21h], al

        push    4
        lea     edx, [esi + 8]
        mov     ecx, ebp
        mov     eax, 004BF654h
        jmp     eax
    }
}

__declspec(naked) void on_setup_option_1() //004BF23A
{
    __asm
    {
        mov     dword ptr [esi + 0Ch], 0Ah
        mov     byte ptr [esi + 21h], CURRENT_PLAYER_OPTION_LEVEL
        mov     eax, 004BF241h
        jmp     eax
    }
}

__declspec(naked) void on_setup_option_2() //004BF996
{
    __asm
    {
        mov     dword ptr [ebp + 8], 6
        mov     byte ptr [ebp + 21h], CURRENT_PLAYER_OPTION_LEVEL
        mov     eax, 004BF99Dh
        jmp     eax
    }
}

__declspec(naked) void on_save_option() //004C1C2F
{
    __asm
    {
        push    1
        lea     edx, [ebx + 21h]
        mov     ecx, esi
        mov     eax, 004C1C38h
        jmp     eax
    }
}

//new evac tile

__declspec(naked) void on_evac_area() //004A6E95
{
    __asm
    {
        mov     eax, [ebx + 18h]
        mov     al, [eax + 21h]
        cmp     al, 0Bh
        jz      old_evac_tile

        xor     eax, eax
        cmp     esi, 1
        setnz   al
        mov     esi, eax
        mov     eax, [esp + 18h]
        test    eax, eax
        jg      loc_576480
        mov     al, [esp + 13h]
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        add     esp, 34h
        retn    18h

loc_576480:
        mov     ecx, 004A6D60h
        jmp     ecx

old_evac_tile:
        mov     ecx, [esp + 38h]
        xor     eax, eax
        cmp     esi, 1
        setnz   al
        mov     esi, eax
        mov     eax, 004A6EA3h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setPlayerOptionsHooks()
{
    setHook((void*)0x004BF64D, on_read_option);
    setHook((void*)0x004BF23A, on_setup_option_1);
    setHook((void*)0x004BF996, on_setup_option_2);
    setHook((void*)0x004C1C2F, on_save_option);

    //evac area oos fix
    setHook((void*)0x004A6E95, on_evac_area);
}
#pragma optimize( "", on )
