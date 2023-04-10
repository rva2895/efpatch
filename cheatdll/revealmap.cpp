#include "stdafx.h"
#include "revealmap.h"

//0 - Normal
//1 - Explored
//2 - All Visible
//3 - No Fog
//4 - Normal / Allied
//5 - Explored / Allied

void __stdcall on_new_game_explore(TRIBE_Game* game)
{
    TRIBE_Game__Visibility visibilityValue = game->tribe_game_options.visibilityValue;

    if (visibilityValue == 1 || visibilityValue == 2 || visibilityValue == 5)
    {
        for (int l = 1; l < game->world->player_num; ++l)
        {
            if (!game->world->players[l]->vfptr->computerPlayer(game->world->players[l]))
                RGE_Visible_Map__explore_all(game->world->players[l]->visible);
        }
    }

    if (visibilityValue == 2 || visibilityValue == 3)
        RGE_Base_Game__set_map_fog((RGE_Base_Game*)game, 0);

    if (visibilityValue == 4 || visibilityValue == 5)
    {
        for (int l = 1; l < game->world->player_num; ++l)
        {
            TRIBE_Player_Tech__do_tech(game->world->players[l]->tech_tree, 61); //holonet
        }
    }
}

__declspec(naked) void on_new_game_explore_wr() //005ED310
{
    __asm
    {
        push    ebp
        call    on_new_game_explore
        mov     eax, 005ED37Eh
        jmp     eax
    }
}

__declspec(naked) void on_player_update() //005CEACD
{
    __asm
    {
        cmp     eax, 1
        jz      player_update_explore
        cmp     eax, 2
        jz      player_update_explore
        cmp     eax, 5
        jz      player_update_explore
        mov     eax, 005CEB00h
        jmp     eax

player_update_explore:
        mov     eax, 005CEAF1h
        jmp     eax
    }
}

void __stdcall on_create_visibility_dropdown(TDropDownPanel* mapVisibilityList)
{
    TDropDownPanel__append_line2(mapVisibilityList, 9755, 0);
    TDropDownPanel__append_line2(mapVisibilityList, 9756, 1);
    TDropDownPanel__append_line2(mapVisibilityList, 9757, 2);
    TDropDownPanel__append_line2(mapVisibilityList, 9767, 4);
    TDropDownPanel__append_line2(mapVisibilityList, 9768, 5);
}

__declspec(naked) void on_create_visibility_dropdown_wr() //00520215
{
    __asm
    {
        push    ecx
        call    on_create_visibility_dropdown
        mov     eax, 00520245h
        jmp     eax
    }
}

DWORD __stdcall get_fog_flags(int visibility) //al: fog flag, ah: explore flag
{
    DWORD explore_flag;
    DWORD fog_flag;
    switch (visibility)
    {
    case 0:
    case 4:
    default:
        fog_flag = 1;
        explore_flag = 0;
        break;
    case 1:
    case 5:
        fog_flag = 1;
        explore_flag = 1;
        break;
    case 2:
        fog_flag = 0;
        explore_flag = 1;
        break;
    case 3:
        fog_flag = 0;
        explore_flag = 0;
        break;
    }
    return (explore_flag << 8) | fog_flag;
}

__declspec(naked) void on_setup_replay_controls() //00502A0E
{
    __asm
    {
        push    eax
        push    ecx
        call    get_fog_flags
        //edi: fog flag, ebx: explore flag
        mov     edi, eax
        and     edi, 0FFh
        xor     ebx, ebx
        mov     bl, ah
        pop     eax
        mov     ecx, 00502A3Ah
        jmp     ecx
    }
}

__declspec(naked) void on_screen_game_action() //004FAC7D
{
    __asm
    {
        push    ecx
        push    edx
        call    get_fog_flags
        //esi: fog flag, edi: explore flag
        mov     esi, eax
        and     esi, 0FFh
        shr     eax, 8
        mov     edi, eax
        pop     ecx
        mov     eax, 004FACB5h
        jmp     eax
    }
}

__declspec(naked) void visibility_on_mp_setup_update_summary() //0051E40D
{
    __asm
    {
        jz      mp_setup_summary_no_reveal
        cmp     eax, 4
        jz      mp_setup_summary_no_reveal
        push    0FA3h
        jmp     mp_setup_summary_end

mp_setup_summary_no_reveal:
        push    0FA4h

mp_setup_summary_end:
        mov     eax, 0051E41Bh
        jmp     eax
    }
}

__declspec(naked) void visibility_on_status_message_update_summary() //00541B9C
{
    __asm
    {
        jz      status_message_summary_no_reveal
        cmp     eax, 4
        jz      status_message_summary_no_reveal
        push    0FA3h
        jmp     status_message_summary_end

status_message_summary_no_reveal:
        push    0FA4h

status_message_summary_end:
        mov     eax, 00541BAAh
        jmp     eax
    }
}

__declspec(naked) void visibility_on_new_game() //006011B3
{
    __asm
    {
        jz      new_game_no_reveal
        cmp     eax, 4
        jz      new_game_no_reveal
        push    0FA3h
        jmp     new_game_end

new_game_no_reveal:
        push    0FA4h

new_game_end:
        mov     eax, 006011C1h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setRevealMapHooks()
{
    setHook((void*)0x005ED310, on_new_game_explore_wr);
    setHook((void*)0x005CEACD, on_player_update);
    setHook((void*)0x00520215, on_create_visibility_dropdown_wr);
    setHook((void*)0x00502A0E, on_setup_replay_controls);
    setHook((void*)0x004FAC7D, on_screen_game_action);
    setHook((void*)0x0051E40D, visibility_on_mp_setup_update_summary);
    setHook((void*)0x00541B9C, visibility_on_status_message_update_summary);
    setHook((void*)0x006011B3, visibility_on_new_game);
}
#pragma optimize( "", on )
