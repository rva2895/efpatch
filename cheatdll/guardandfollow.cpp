#include "stdafx.h"
#include "guardandfollow.h"

__declspec(naked) void on_do_command_guard() //0044C436
{
    __asm
    {
        call    RGE_Game_World__object
        test    eax, eax
        jz      guard_bad_object
        mov     ecx, 0044C43Bh
        jmp     ecx

guard_bad_object:
        mov     ecx, 0044C593h
        jmp     ecx
    }
}

__declspec(naked) void on_do_command_follow() //0044C5B6
{
    __asm
    {
        call    RGE_Game_World__object
        test    eax, eax
        jz      follow_bad_object
        mov     ecx, 0044C5BBh
        jmp     ecx

follow_bad_object:
        mov     ecx, 0044C722h
        jmp     ecx
    }
}

#pragma optimize( "s", on )
void setGuardAndFollowHooks()
{
    setHook((void*)0x0044C436, on_do_command_guard);
    setHook((void*)0x0044C5B6, on_do_command_follow);
}
#pragma optimize( "", on )
