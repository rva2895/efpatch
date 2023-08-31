#include "stdafx.h"
#include "miscbugfix.h"

__declspec(naked) void annex_unit_crash_mitigation() //00555640
{
    __asm
    {
        mov     ecx, [esi + 1Ch]
        test    ecx, ecx
        jz      mitigate_annex_crash
        movsx   eax, ax
        mov     edx, 00555646h
        jmp     edx

mitigate_annex_crash:
        mov     edx, 005556C9h
        jmp     edx
    }
}

__declspec(naked) void darkside_cheat_fix() //0060429C
{
    __asm
    {
        xor     eax, eax
        mov     ax, [esi + 48h]
        cmp     edi, eax
        jge     darkside_cheat_bad_player
        mov     eax, [esi + 4Ch]
        mov     ecx, [eax + edi * 4]
        mov     eax, 006042A2h
        jmp     eax

darkside_cheat_bad_player:
        mov     eax, 006044CAh
        jmp     eax
    }
}

__declspec(naked) void error_txt_fopen_fix() //0048F7BD
{
    __asm
    {
        add     esp, 8
        test    eax, eax
        jz      skip_error_txt_write
        mov     esi, eax
        mov     eax, 0048F7C2h
        jmp     eax

skip_error_txt_write:
        mov     eax, 0048F7D7h
        jmp     eax
    }
}

__declspec(naked) void chat_stutter_fix() //004CD00A
{
    __asm
    {
        cmp     byte ptr [eax], 0
        jz      no_streaming_sound
        mov     ecx, 004CD012h
        jmp     ecx

no_streaming_sound:
        mov     ecx, 004CD192h
        jmp     ecx
    }
}

__declspec(naked) void random_sound_start() //00517484
{
    __asm
    {
        cmp     eax, 1Eh
        jz      random_sound_start_rnd
        cmp     eax, 1Fh
        jz      random_sound_start_rnd
        mov     ecx, 0051749Dh
        jmp     ecx

random_sound_start_rnd:
        mov     ecx, 00517489h
        jmp     ecx
    }
}

__declspec(naked) void random_sound_countdown() //0051B2A1
{
    __asm
    {
        cmp     eax, 1Eh
        jz      random_sound_countdown_rnd
        cmp     eax, 1Fh
        jz      random_sound_countdown_rnd
        mov     ecx, 0051B2BAh
        jmp     ecx

random_sound_countdown_rnd:
        mov     ecx, 0051B2A6h
        jmp     ecx
    }
}

void __stdcall minimap_terrain_do_switch(TRIBE_Screen_Game* game_screen)
{
    game_screen->vfptr->set_redraw(game_screen, 1);
    game_screen->map_view->vfptr->set_redraw(game_screen->map_view, 2);
}

__declspec(naked) void minimap_terrain_1() //004FA199
{
    __asm
    {
        pop     eax
        push    ebp
        call    minimap_terrain_do_switch
        mov     eax, 004FA1A1h
        jmp     eax
    }
}

__declspec(naked) void minimap_terrain_2() //004FC12E
{
    __asm
    {
        pop     eax
        push    esi
        call    minimap_terrain_do_switch
        mov     eax, 004FC135h
        jmp     eax
    }
}

void __stdcall do_set_view_loc_minimap_update()
{
    TRIBE_Screen_Game* game_screen = ((TRIBE_Game*)(*base_game))->game_screen;
    if (game_screen)
        game_screen->map_view->vfptr->set_redraw(game_screen->map_view, 1);
}

__declspec(naked) void set_view_loc_minimap_update() //004C21A2
{
    __asm
    {
        mov     [edx + 178h], eax
        call    do_set_view_loc_minimap_update
        ret     0Ch
    }
}

unsigned int idle_found_timer = 0;
bool idle_button_blink_state = false;
bool idle_button_blinking = false;

void __stdcall do_handle_idle_button(TRIBE_Screen_Game* game_screen, unsigned int t, unsigned int wt_delta)
{
    bool set_active = false;
    bool reset_blink_timer = false;
    if (wt_delta)
    {
        RGE_Player_Object_List* list = RGE_Base_Game__get_player(*base_game)->objects;
        bool idle_found = false;
        for (int i = 0; i < list->Number_of_objects; i++)
        {
            RGE_Static_Object* obj = list->List[i];
            if (obj && obj->vfptr->gbg_isCivilianUnit(obj) && obj->vfptr->is_idle(obj))
            {
                idle_found = true;
                break;
            }
        }
        if (idle_found)
        {
            if (t - idle_found_timer > 500)
            {
                set_active = true;
                if (!idle_button_blinking)
                {
                    idle_button_blinking = true;
                    reset_blink_timer = true;
                }
            }
        }
        else
        {
            idle_found_timer = t;
            set_active = false;
            if (idle_button_blinking)
            {
                idle_button_blinking = false;
                reset_blink_timer = true;
            }
        }
    }
    if (reset_blink_timer || t - game_screen->gbg_last_unknown_time >= game_screen->gbg_unknown_interval)
    {
        if (idle_button_blinking)
            idle_button_blink_state = !idle_button_blink_state;
        else
            idle_button_blink_state = false;

        short button_pic_index = idle_button_blink_state ? 16 : 12;
        TButtonPanel__set_picture((TButtonPanel*)game_screen->buttons[25], 0, game_screen->game_icon_pic, button_pic_index);

        game_screen->gbg_last_unknown_time = t;
    }
}

__declspec(naked) void on_handle_idle_button() //004F8AEB
{
    __asm
    {
        mov     ecx, [esp + 18h]
        mov     [esp + 10h], eax
        push    ecx
        push    eax
        push    esi
        call    do_handle_idle_button

        mov     eax, [esp + 18h]
        mov     ecx, 004F8AF3h
        jmp     ecx
    }
}

#pragma optimize( "s", on )
void setMiscBugfixHooks()
{
    //read-only fix for data\*.dat
    writeDword(0x004D5B62, GENERIC_READ);

    //darkside cheat crash
    setHook((void*)0x0060429C, darkside_cheat_fix);

    //error.txt fix
    writeData(0x0068F14C, "error.txt", 10);
    setHook((void*)0x0048F7BD, error_txt_fopen_fix);

    //renderer fix (THIS_COD)
    writeNops(0x0064DC8D, 25);

    //workaround for annex unit crash from state 0
    setHook((void*)0x00555640, annex_unit_crash_mitigation);

    //MP mouse lag
    writeByte(0x0049F686, 0x0C);
    writeWord(0x0049F906, 0x9090);
    writeByte(0x00429541, 0xEB);

    //wndproc loop delay
    writeDword(0x00425EA6, 0x0674023C);
    writeDword(0x00425EAA, 0x013C042C);
    writeByte(0x00425EAE, 0x77);

    //one player in MP
    writeByte(0x005154D9, 0xEB);
    writeByte(0x00519A06, 0xEB);

    //disabled ungrouped AI alliance
    writeByte(0x0061E2EB, 0xEB);

    //from old patch:
    //full map print ratios
    writeByte(0x0045D4BD, 8);
    writeByte(0x0045D4CA, 1);
    writeByte(0x0045D4DC, 0xFF);

    //post game view for sp
    writeByte(0x004F87AD, 0x4C);
    writeByte(0x004F89CE, 0xEB);
    writeByte(0x004F89CF, 0x1F);
    writeByte(0x004F89E1, 0xEB);
    writeByte(0x004F8A13, 0x66);
    writeByte(0x004F8A14, 0x90);

    //remove high graphics fambaa ring
    writeByte(0x0061F4A4, 0xEB);
    
    //chat stutter
    setHook((void*)0x004CD00A, chat_stutter_fix);

    //high civ UI fix
    writeByte(0x004FDB68, 0);

    //timeGetTime unsigned
    writeByte(0x004CD8EE, 0x72);
    writeByte(0x004F9980, 0x84);

    //chat display time
    writeDword(0x004CCAD0, 15000);

    //mirror random sound fix
    setHook((void*)0x00517484, random_sound_start);
    setHook((void*)0x0051B2A1, random_sound_countdown);

    //minimap terrain toggle
    setHook((void*)0x004FA199, minimap_terrain_1);
    setHook((void*)0x004FC12E, minimap_terrain_2);

    //minimap scroll update
    setHook((void*)0x004C21A2, set_view_loc_minimap_update);

    //remove forced map redraw
    writeNops(0x0061F04E, 7);

    //map redraw on pause
    writeByte(0x004F8E0B, 0xEB);

    //remove GetMessage loop
    writeByte(0x00425EC1, 0xEB);

    //new idle worker button handling
    setHook((void*)0x004F8AFB, (void*)0x004F8C78);
    setHook((void*)0x004F8AEB, on_handle_idle_button);

    //#include_drs
    writeNops(0x004E27A6, 6);

    //#include
    writeNops(0x004E2773, 6);
}
#pragma optimize( "", on )
