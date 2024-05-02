#include "stdafx.h"
#include "fixedpos.h"

/*
__declspec(naked) void set_team_together_fix() //00521C29
{
    __asm
    {
        cmp     eax, 0
        jz      _tt_0
        cmp     eax, 1
        jz      _tt_1
        cmp     eax, 2
        jz      _tt_end
_tt_1:
        xor     eax, eax
        jmp     _tt_end
_tt_0:
        mov    eax, 1
_tt_end:
        push    eax
        mov     eax, 00521C2Fh
        jmp     eax
    }
}

__declspec(naked) void set_team_together_fix_2() //00520643
{
    __asm
    {
        cmp     eax, 0
        jz      _tt_0
        cmp     eax, 1
        jz      _tt_1
        cmp     eax, 2
        jz      _tt_end
_tt_1:
        xor     eax, eax
        jmp     _tt_end
_tt_0:
        mov    eax, 1
_tt_end:
        push    eax
        mov     eax, 00520649h
        jmp     eax
    }
}

int init_3_state = 0;

__declspec(naked) void set_team_together_3_state_init() //004B962E
{
    __asm
    {
        mov     ecx, [edi]
        mov     eax, init_3_state
        test    eax, eax
        jz      _tt_no_3_state
        xor     eax, eax
        mov     init_3_state, eax
        push    3
        jmp     _tt_3_state_call
_tt_no_3_state:
        push    2
_tt_3_state_call:
        mov     eax, 004C4CF0h
        call    eax
        mov     eax, 004B9637h
        jmp     eax
    }
}

__declspec(naked) void set_team_together_set_3_state_flag() //0051F49A
{
    __asm
    {
        mov     edx, 1
        mov     init_3_state, edx
        call    dword ptr [eax + 11Ch]
        mov     edx, 0051F4A0h
        jmp     edx
    }
}

__declspec(naked) void get_team_together_inverted_full()
{
    __asm
    {
        xor     eax, eax
        mov     al, [ecx + 1438h]
        retn
    }
}

__declspec(naked) void get_team_together_full_fix() //00520636
{
    __asm
    {
        mov     edi, [eax]
        call    get_team_together_inverted_full
        mov     edx, 0052063Dh
        jmp     edx
    }
}

__declspec(naked) void get_team_together_inverted_patch() //005EF140
{
    __asm
    {
        xor     eax, eax
        mov     al, [ecx + 1438h]
        cmp     al, 2
        jnz     _gtt_p_end
        xor     al, al
_gtt_p_end:
        retn
    }
}
*/

int get_free_color(int* p, bool* colors_used)
{
    UNREFERENCED_PARAMETER(p);
    for (int i = 1; i < 9; i++)
        if (!colors_used[i])
        {
            colors_used[i] = true;
            return i;
        }
    return -1;
}

void fix_colors(int* p)
{
    bool colors_used[9] = { false };
    colors_used[0] = true;

    for (int i = 0; i < 8; i++)
        colors_used[p[i]] = true;

    for (int i = 0; i < 8; i++)
        if (p[i] == 0)
            p[i] = get_free_color(p, colors_used);
}

void __stdcall rnd_pos_fill(int* p)
{
    memset(p, 0, sizeof(int) * 8);

    for (int i = 7; i >= 0; i--)
        p[TRIBE_Game__playerColor(*(TRIBE_Game**)base_game, i) - 1] = i + 1;

    fix_colors(p);
}

__declspec(naked) void remove_rnd_pos() //004E789B
{
    __asm
    {
        lea     eax, [esp + 54h]
        push    edx
        push    eax
        call    rnd_pos_fill
        pop     edx

        mov     ecx, [esp + 1Ch]
        mov     ecx, [ecx + 3F6CCh]
        mov     eax, 004E78A5h
        jmp     eax
    }
}

__declspec(naked) void colors_create_game_1() //005ECD86
{
    __asm
    {
        cmp     eax, 2
        jz      short loc_7E38CE
        cmp     eax, 4
        jnz     loc_4432BA

loc_7E38CE:
        lea     ecx, [ebx - 1]
        push    ecx
        mov     ecx, ebp
        call    TRIBE_Game__playerColor
        cmp     eax, 9
        jnz     loc_443217

        mov     edx, 005ECDCBh
        jmp     edx

loc_4432BA:
        mov     eax, 005ECE50h
        jmp     eax

loc_443217:
        mov     ecx, 005ECD94h
        jmp     ecx
    }
}

__declspec(naked) void colors_start_game_1() //005198A6
{
    __asm
    {
        call    TRIBE_Game__playerColor
        cmp     eax, 9
        jz      loc_4FF12E
        
        mov     ecx, 005198ABh
        jmp     ecx

loc_4FF12E:
        mov     ecx, 005198B3h
        jmp     ecx
    }
}

__declspec(naked) void colors_fill_player_color() //0051C951
{
    __asm
    {
        mov     ecx, base_game
        mov     ecx, [ecx]
        cmp     byte ptr [ecx + 9AEh], 1
        jz      loc_7E386A
        cmp     dword ptr [esp + 18h], 0
        jnz     loc_501D71

loc_7E386A:
        mov     eax, [esi + 0BC4h]
        
        mov     edi, 0051C957h
        jmp     edi

loc_501D71:
        mov     ecx, 0051C961h
        jmp     ecx
    }
}

__declspec(naked) void colors_fill_players_1() //0051CBE7
{
    __asm
    {
        cmp     eax, 9
        jz      loc_502081
        mov     ecx, base_game
        mov     ecx, [ecx]
        
        mov     edx, 0051CBEDh
        jmp     edx

loc_502081:
        mov     ecx, 0051CC62h
        jmp     ecx
    }
}

__declspec(naked) void colors_fill_players_2() //0051CD8E
{
    __asm
    {
        push    eax
        mov     ecx, comm
        mov     ecx, [ecx]
        //call    TCommunications_Handler__WhoAmI
        //cmp     eax, [ecx + 200Ch]
        call    TCommunications_Handler__IsHost
        test    eax, eax
        pop     eax
        jz      short loc_7E3824
        cmp     ebx, 4
        jnz     short loc_7E3824
        mov     ecx, 1
        jmp     loc_502139
        
loc_7E3824:
        xor     ecx, ecx
        cmp     eax, ebp
        setz    cl

loc_502139:
        mov     eax, 0051CD93h
        jmp     eax
    }
}

__declspec(naked) void colors_fill_players_3() //0051CF2E
{
    __asm
    {
        cmp     ebx, 2
        jz      loc_5022D1
        cmp     ebx, 4
        jnz     loc_5022DF

loc_5022D1:
        mov     ecx, 0051CF33h
        jmp     ecx

loc_5022DF:
        mov     ecx, 0051CF41h
        jmp     ecx
    }
}

__declspec(naked) void colors_fill_players_4() //0051D1E7
{
    __asm
    {
        mov     ecx, base_game
        mov     ecx, [ecx]
        mov     al, [ecx + 9ADh]
        test    al, al
        jz      short loc_7E3948
        test    esi, esi
        jnz     loc_5025A2

loc_7E3948:
        cmp     ebp, 1
        jnz     loc_5025A2
        
        mov     eax, 0051D1ECh
        jmp     eax

loc_5025A2:
        mov     eax, 0051D204h
        jmp     eax
    }
}

__declspec(naked) void colors_set_player_humanity_wrapper()
{
    __asm
    {
        cmp     dword ptr [esp + 24h], 00516832h
        jz      jmp_set_player_humanity
        ret     8

jmp_set_player_humanity:
        mov     eax, 0043AF70h
        jmp     eax
    }
}

__declspec(naked) void colors_fill_player_colors() //0051EA9D
{
    __asm
    {
        cmp     ebx, 9
        jnz     short loc_7E38B3
        mov     ecx, [edi]
        mov     word ptr [esp + 18h], 3Fh   //2Dh
        jmp     loc_503F32

loc_7E38B3:
        lea     ecx, [esp + 18h]
        push    ebx
        
        mov     eax, 0051EAA2h
        jmp     eax

loc_503F32:
        mov     eax, 0051EAB2h
        jmp     eax
    }
}

__declspec(naked) void colors_update_summary() //0051D862
{
    __asm
    {
        call    TribeMPSetupScreen__fillPlayerColor
        push    0
        push    edi
        mov     ecx, ebp
        call    TribeMPSetupScreen__fillPlayerTeam
        test    edi, edi
        jnz     short loc_7E424F
        mov     [ebp + 0BC4h], edi
        mov     [ebp + 0BC8h], edi
        
loc_7E424F:
        mov     eax, 0051D867h
        jmp     eax
    }
}

void __stdcall setup_colors()
{
    int color_used_count[8] = { 0 };
    int humanity;
    int color;
    int max_players;

    if (RGE_Base_Game__singlePlayerGame(*base_game))
        max_players = RGE_Base_Game__numberPlayers(*base_game);
    else
        max_players = 8;

    for (int i = 0; i < max_players; i++)
    {
        humanity = TCommunications_Handler__GetPlayerHumanity(*comm, i + 1);
        if (humanity == 2 || humanity == 4)
        {
            color = TRIBE_Game__playerColor(*(TRIBE_Game**)base_game, i);
            if (color != 9)
                color_used_count[color - 1]++;
        }
    }

    for (int i = 0; i < max_players; i++)
    {
        if (TCommunications_Handler__GetPlayerHumanity(*comm, i + 1) == 4)
        {
            color = TRIBE_Game__playerColor(*(TRIBE_Game**)base_game, i);
            if (color != 9 && color_used_count[color - 1] > 1)
            {
                color_used_count[color - 1]--;
                TRIBE_Game__setPlayerColor(*(TRIBE_Game**)base_game, i, 9);
            }
        }
    }

    for (int i = 0; i < max_players; i++)
    {
        humanity = TCommunications_Handler__GetPlayerHumanity(*comm, i + 1);
        if (humanity == 2 || humanity == 4)
        {
            color = TRIBE_Game__playerColor(*(TRIBE_Game**)base_game, i);
            if (color == 9)
            {
                do
                {
                    color = rand_internal() % 8;
                } while (color_used_count[color] > 0);
                color_used_count[color]++;
                TRIBE_Game__setPlayerColor(*(TRIBE_Game**)base_game, i, color + 1);
            }
        }
    }
}

__declspec(naked) void colors_launch_game() //00519C7C
{
    __asm
    {
        call    setup_colors
        mov     ecx, base_game
        mov     ecx, [ecx]
        mov     eax, 00519C82h
        jmp     eax
    }
}

__declspec(naked) void colors_launch_game_v2() //00519BD3
{
    __asm
    {
        call    setup_colors
        mov     eax, [esi]
        mov     ecx, esi
        call    dword ptr [eax + 17Ch]
        mov     ebx, 00519BDDh
        jmp     ebx
    }
}

#pragma optimize( "s", on )
void setFixedPosHooks()
{
    /*setHook((void*)0x00521C29, set_team_together_fix);
    setHook((void*)0x00520643, set_team_together_fix_2);
    setHook((void*)0x004B962E, set_team_together_3_state_init);
    setHook((void*)0x0051F49A, set_team_together_set_3_state_flag);
    setHook((void*)0x00520636, get_team_together_full_fix);
    setHook((void*)0x005EF140, get_team_together_inverted_patch);*/

    setHook((void*)0x004E789B, remove_rnd_pos);

    writeNops(0x005ECDF2, 2);
    writeWord(0x00516CB9, 0x08B2);
    writeByte(0x005192CB, 1);
    writeByte(0x00519809, 9);
    writeNops(0x0051CD9B, 2);
    writeNops(0x0051CDAA, 2);
    writeWord(0x0051D30B, 0xF685);
    writeByte(0x0051D30E, 0x84);
    writeByte(0x0051E9D0, 0xEB);

    writeDword(0x0051E9F3, (DWORD)colors_set_player_humanity_wrapper - 0x0051E9F7);

    writeByte(0x0051EA83, 0xEB);
    writeByte(0x0051EACA, 8);
    writeByte(0x0051EB2A, 9);
    writeDword(0x0051EB74, 0x0051EB78);
    writeDword(0x0051EB78, 0xFFFF6852);
    writeDword(0x0051EB7C, 0x9AEB00FF);

    setHook((void*)0x005ECD86, colors_create_game_1);
    setHook((void*)0x005198A6, colors_start_game_1);
    setHook((void*)0x0051C951, colors_fill_player_color);
    setHook((void*)0x0051CBE7, colors_fill_players_1);
    setHook((void*)0x0051CD8E, colors_fill_players_2);
    setHook((void*)0x0051CF2E, colors_fill_players_3);
    setHook((void*)0x0051EA9D, colors_fill_player_colors);
    setHook((void*)0x0051D1E7, colors_fill_players_4);
    //setHook((void*)0x00519C7C, colors_launch_game);
    //writeDword(0x006612AC, (DWORD)colors_launch_game_v2);
    setHook((void*)0x00519BD3, colors_launch_game_v2);

    setHook((void*)0x0051D862, colors_update_summary);

    writeNops(0x0051D967, 7);
    writeNops(0x0051D996, 8);
}
#pragma optimize( "", on )

//in UP: FB5E8, FE9EF, 152AF0, 152B04
