#include "stdafx.h"
#include "playeroptions.h"

#define CURRENT_PLAYER_OPTION_LEVEL 0x10 //old: F, next: 10

#pragma warning(push)
#pragma warning(disable:4100)
__declspec(naked) PLAYER_EXTRA* __fastcall get_player_extra(RGE_Player* player)
{
    __asm
    {
        mov     eax, [ecx + 98h]
        mov     al, [ecx + 22h]
        ret
    }
}

__declspec(naked) void __fastcall add_player_extra(RGE_Player* player, PLAYER_EXTRA* pe)
{
    __asm
    {
        mov     [ecx + 22h], dl
        and     edx, 0FFFFFF00h
        and     dword ptr [ecx + 98h], 0FFh
        or      dword ptr [ecx + 98h], edx
        ret
    }
}
#pragma warning(pop)

__declspec(noinline) bool is_player_extra_enabled(RGE_Player* player)
{
    unsigned char option = *((unsigned char*)player + 0x21);
    return option >= 0x10;
}

void __stdcall player_extra_create(RGE_Player* player)
{
    PLAYER_EXTRA* pe = new PLAYER_EXTRA;
    memset(pe, 0, sizeof(PLAYER_EXTRA));
    add_player_extra(player, pe);
}

void __stdcall player_extra_delete(RGE_Player* player)
{
    delete get_player_extra(player);
}

void __stdcall player_extra_read(RGE_Player* player, int handle)
{
    if (is_player_extra_enabled(player))
    {
        PLAYER_EXTRA* pe = get_player_extra(player);
        int num_saved_loc;
        rge_read(handle, &num_saved_loc, sizeof(num_saved_loc));
        for (int i = 0; i < num_saved_loc; i++)
        {
            rge_read(handle, &pe->view_loc[i].set, sizeof(pe->view_loc[i].set));
            rge_read(handle, &pe->view_loc[i].x, sizeof(pe->view_loc[i].x));
            rge_read(handle, &pe->view_loc[i].y, sizeof(pe->view_loc[i].y));
        }
    }
}

void __stdcall player_extra_write(RGE_Player* player, int handle)
{
    if (is_player_extra_enabled(player))
    {
        PLAYER_EXTRA* pe = get_player_extra(player);
        int num_saved_loc = NUM_SAVED_PLAYER_LOC;
        rge_write(handle, &num_saved_loc, sizeof(num_saved_loc));
        for (int i = 0; i < num_saved_loc; i++)
        {
            rge_write(handle, &pe->view_loc[i].set, sizeof(pe->view_loc[i].set));
            rge_write(handle, &pe->view_loc[i].x, sizeof(pe->view_loc[i].x));
            rge_write(handle, &pe->view_loc[i].y, sizeof(pe->view_loc[i].y));
        }
    }
}

__declspec(naked) void on_read_option() //004BF64D
{
    __asm
    {
        mov     ecx, 0077D4B0h
        mov     al, [ecx]
        mov     [esi + 21h], al
        push    ebp
        push    esi
        call    player_extra_read
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
        push    esi
        call    player_extra_create
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
        push    ebp
        call    player_extra_create
        mov     eax, 004BF99Dh
        jmp     eax
    }
}

__declspec(naked) void on_delete_option() //004BFFD5
{
    __asm
    {
        mov     edi, [esi + 78h]
        xor     ebx, ebx
        cmp     edi, ebx
        push    esi
        call    player_extra_delete
        mov     eax, 004BFFDCh
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
        call    rge_write
        push    esi
        push    ebx
        call    player_extra_write
        mov     eax, 004C1C3Dh
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
    setHook((void*)0x004BFFD5, on_delete_option);

    //evac area oos fix
    setHook((void*)0x004A6E95, on_evac_area);
}
#pragma optimize( "", on )
