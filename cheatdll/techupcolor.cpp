#include "stdafx.h"
#include "techupcolor.h"

#pragma warning(push)
#pragma warning(disable:4100)
__declspec(naked) void __stdcall sendTechChat2(int player, char* s, int unk)
{
    __asm
    {
        mov     eax, [esp + 0Ch] //unk
        mov     ecx, [esp + 8]   //str
        mov     edx, [esp + 4]   //player
        push    0
        push    eax
        push    edx
        push    ecx
        mov     ecx, 006A35D8h
        mov     ecx, [ecx]
        push    0
        mov     eax, 0042D5E0h
        call    eax
        retn    0Ch
    }
}
#pragma warning(pop)

int __stdcall get_player_id(int player_number)
{
    if (player_number == -1)
        return -1;
    for (int i = 1; i <= 8; i++)
        if (RGE_Base_Game__playerID(*base_game, i) == player_number)
            return i;
    return 0;
}

__declspec(naked) void onTechUpSend() //005E9990
{
    __asm
    {
        push    edi
        call    get_player_id
        push    eax
        call    sendTechChat2
        mov     eax, 005E9995h
        jmp     eax
    }
}

__declspec(naked) void on_rec_chat() //0061FCD8
{
    __asm
    {
        mov     ecx, [ebp - 38h]
        push    1
        push    0
        push    ecx
        call    get_player_id
        push    eax
        mov     eax, 0061FCE0h
        jmp     eax
    }
}

__declspec(naked) void on_attack_notification() //005CEC6A
{
    __asm
    {
        mov     edx, [ecx + 8]
        push    edx
        call    get_player_id
        mov     edx, eax
        lea     eax, [esp + 18h]
        mov     ecx, 005CEC71h
        jmp     ecx
    }
}

#pragma optimize( "s", on )
void setTechUpColorHooks()
{
    setHook((void*)0x005E9990, onTechUpSend);

    setHook((void*)0x0061FCD8, on_rec_chat);
    setHook((void*)0x005CEC6A, on_attack_notification);
}
#pragma optimize( "", on )
