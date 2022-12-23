#include "stdafx.h"
#include "rec.h"

__declspec(naked) int __stdcall isRec()
{
    __asm
    {
        mov     ecx, 006A3684h
        mov     ecx, [ecx]
        mov     ecx, [ecx + 17B4h]
        test    ecx, ecx
        jz      not_rec
        mov     ecx, [ecx + 126Ch]
        test    ecx, ecx
        jz      not_rec
        mov     ecx, [ecx + 0Ch]
        test    ecx, ecx
        jz      not_rec
        mov     edx, [ecx + 1614h]
        mov     eax, [edx + 4]
        test    eax, eax
        setg    al
        movzx   eax, al
        ret
not_rec:
        xor     eax, eax
        ret
    }
}

__declspec(naked) void onMenuInit() //0045E425
{
    __asm
    {
        cmp     dword ptr [esp + 18h], 1
        jz      cont_menu_init
        call    isRec
        mov     [esp + 18h], eax
cont_menu_init:
        mov     ecx, 006A3684h
        mov     ecx, [ecx]
        mov     eax, 0045E42Bh
        jmp     eax
    }
}

const DWORD color_list[] =
{
    0x00EBA66E,
    0x006464FF,
    0x0000FF00,
    0x0000FFFF,
    0x00FFFF00,
    0x00E86CF1,
    0x00B9B9B9,
    0x000182FF,
    0x00FFFFFF
};

__declspec(naked) void rec_player_list_color() //00502C12
{
    __asm
    {
        mov     ecx, [eax + 9Ch]
        push    edi
        push    ecx
        mov     ecx, [esi + 1188h]

        mov     [esp + 18h], eax
        mov     eax, 004C82A0h
        call    eax //TDropDownPanel::append_line(char*, long)
        cmp     edi, 1
        jnz     loc_7CD8A9
        mov     ecx, [esi + 1188h] //TDropDownPanel
        mov     eax, [ecx + 100h] //TListPanel
        mov     edx, [eax + 0F8h]
        mov     byte ptr [eax + 168h], 0Bh
        jmp     loc_7CD8B0

loc_7CD8A9:
        mov     ecx, [esp + 14h]
        mov     edx, [ecx + 2Ch]

loc_7CD8B0:
        mov     eax, [esp + 10h]
        mov     dword ptr [edx + 10h], 1
        mov     ecx, [eax + 164h] //rge_color_table
        mov     [esp + 14h], edx
        mov     eax, [ecx + 4]
        xor     ecx, ecx
        and     eax, 7
        inc     edi
        mov     ecx, color_list[eax * 4]
        mov     eax, [esi + 126Ch]
        mov     [edx + 1Ch], ecx
        mov     [edx + 14h], ecx
        
        mov     ecx, 00502C2Ch
        jmp     ecx
    }
}

void __stdcall make_eof_string(char* dest, long t)
{
    snprintf(dest, 240, "Unexpected end of file. Replaying stopped. - worldtime: %ld", t);
}

__declspec(naked) void rec_unexpected_eof() //0061FD04
{
    __asm
    {
        jz      rec_eof
        mov     ecx, 0061FCA4h
        jmp     ecx

rec_eof:
        mov     ecx, base_game
        mov     ecx, [ecx]
        mov     eax, [ecx + 17B4h]
        test    eax, eax
        jz      stop_replaying
        cmp     dword ptr [eax + 181Ch], 0
        jnz     stop_replaying

        mov     ecx, base_game
        mov     ecx, [ecx]
        mov     edx, [ecx]
        call    dword ptr [edx + 18h]
        mov     ecx, 006ADBB8h
        mov     eax, [ecx + 10h]
        mov     esi, eax
        test    esi, esi
        jz      stop_replaying

        mov     eax, [edi + 10h]
        push    eax
        lea     eax, [ebp - 274h]
        push    eax
        call    make_eof_string

        mov     eax, 0061FD50h
        jmp     eax

stop_replaying:
        mov     eax, 0061FD69h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setRecHooks()
{
    setHook((void*)0x0045E425, onMenuInit);
    writeWord(0x0061FCD6, 0x9090); //remove chat rec ownership check
    setHook((void*)0x00502C12, rec_player_list_color);
    writeByte(0x004F55C5, 0x14); //shrink rec player dropdown arrow
    setHook((void*)0x0061FD04, rec_unexpected_eof);
}
#pragma optimize( "", on )

void recSwitch(int p)
{
    if (isRec())
    {
        if (p == 0)
        {
            RGE_Player__unselect_object(RGE_Base_Game__get_player(*base_game));
            TRIBE_Game__set_player(*(TRIBE_Game**)base_game, 0);
        }
        else if ((p > 0) && (p <= 8))
        {
            for (int i = 1; i < (*base_game)->world->player_num; i++)
            {
                if (((*base_game)->world->players[i]->color_table->id + 1) == p)
                {
                    RGE_Player__unselect_object(RGE_Base_Game__get_player(*base_game));
                    TRIBE_Game__set_player(*(TRIBE_Game**)base_game, (short)i);
                }
            }
        }
    }
}
