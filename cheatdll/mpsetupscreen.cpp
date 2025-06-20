#include "stdafx.h"
#include "mpsetupscreen.h"

void* __fastcall TribeMPSetupScreenExtraOptionsDialog__vector_deleting_destructor(TribeMPSetupScreenExtraOptionsDialog* this_, DWORD dummy, unsigned int a2);
int __fastcall TribeMPSetupScreenExtraOptionsDialog__action(TribeMPSetupScreenExtraOptionsDialog* this_, DWORD dummy, TPanel* fromPanel, int actionIn, unsigned int a1, unsigned int a2);

const char aExtraOptionsTitle[] = "MP Setup Screen Extra Options";

int extra_options_button_pic_index = -1;

EXTRA_GAME_OPTIONS extra_options;

const DWORD TribeMPSetupScreenExtraOptionsDialog__vftable[] =
{
    (DWORD)TribeMPSetupScreenExtraOptionsDialog__vector_deleting_destructor,
    (DWORD)TPanel__setup,
    (DWORD)TPanel__set_rect,
    (DWORD)TPanel__set_rect2,
    (DWORD)TPanel__set_color,
    (DWORD)TPanel__set_active,
    0x004557A0,
    0x004B5E30,
    (DWORD)TPanel__set_positioning,
    0x004B5F20,
    (DWORD)TPanel__set_fixed_position,
    (DWORD)TPanel__set_redraw,
    (DWORD)TPanel__set_overlapped_redraw,
    (DWORD)TEasy_Panel__draw_setup,
    (DWORD)TPanel__draw_finish,
    (DWORD)TEasy_Panel__draw,
    (DWORD)TPanel__draw_rect,
    (DWORD)TPanel__draw_offset,
    (DWORD)TPanel__draw_rect2,
    (DWORD)TPanel__draw_offset2,
    (DWORD)TPanel__paint,
    (DWORD)TPanel__wnd_proc,
    (DWORD)TPanel__handle_idle,
    (DWORD)TPanel__handle_size,
    (DWORD)TPanel__handle_paint,
    (DWORD)TPanel__handle_key_down,
    (DWORD)TPanel__handle_char,
    (DWORD)nullsub_26,
    (DWORD)nullsub_26,
    (DWORD)nullsub_26,
    (DWORD)nullsub_26,
    (DWORD)TEasy_Panel__handle_mouse_down,
    (DWORD)TEasy_Panel__handle_mouse_move,
    (DWORD)TPanel__handle_mouse_up,
    (DWORD)TPanel__handle_mouse_dbl_click,
    (DWORD)TPanel__handle_mouse_wheel,
    0x00408B20,
    (DWORD)TPanel__handle_mouse_xbuttons,
    0x00408B20,
    0x00408B10,
    0x00408B10,
    0x00408B10,
    0x00408B10,
    0x00408B10,
    0x00408B10,
    0x00408B10,
    0x00408B10,
    0x00408B10,
    0x00408B10,
    0x00408B10,
    0x00408B40,
    0x00408B00,
    (DWORD)TribeMPSetupScreenExtraOptionsDialog__action,
    (DWORD)TPanel__get_true_render_rect,
    (DWORD)TPanel__is_inside,
    (DWORD)TDialogPanel__set_focus,
    (DWORD)TPanel__set_tab_order,
    (DWORD)TPanel__set_tab_order2,
    (DWORD)TPanel__renderArea,
    (DWORD)TPanel__get_help_info,
    (DWORD)TPanel__stop_sound_system,
    (DWORD)TPanel__restart_sound_system,
    0x004BF920,
    (DWORD)TPanel__handle_reactivate,
    (DWORD)TPanel__pointing_at,
    (DWORD)TEasy_Panel__get_ideal_height,
    (DWORD)TEasy_Panel__get_ideal_width,
    (DWORD)TEasy_Panel__draw_background,
    (DWORD)TEasy_Panel__set_ideal_size,
    (DWORD)TEasy_Panel__create_button,
    (DWORD)TEasy_Panel__create_button2,
    (DWORD)TEasy_Panel__create_check_box,
    (DWORD)TEasy_Panel__create_radio_button,
    (DWORD)TEasy_Panel__create_text,
    (DWORD)TEasy_Panel__create_text2,
    (DWORD)TEasy_Panel__create_text3,
    (DWORD)TEasy_Panel__create_text4,
    (DWORD)TEasy_Panel__create_text5,
    (DWORD)TEasy_Panel__create_text6,
    (DWORD)TEasy_Panel__create_input,
    (DWORD)TEasy_Panel__create_edit,
    (DWORD)TEasy_Panel__create_drop_down,
    (DWORD)TEasy_Panel__create_list,
    (DWORD)TEasy_Panel__create_scrollbar,
    (DWORD)TEasy_Panel__create_auto_scrollbar,
    (DWORD)TEasy_Panel__create_vert_slider,
    (DWORD)TEasy_Panel__create_horz_slider,
    (DWORD)TEasy_Panel__create_picture,
    (DWORD)TEasy_Panel__create_picture2,
    (DWORD)TEasy_Panel__create_timeline,
    (DWORD)TEasy_Panel__position_panel,
    (DWORD)TDialogPanel__setup,
    (DWORD)TDialogPanel__setup2,
    1,
    2,
    4,
    8,
    0x10,
    0x20,
    0x40,
    0x80,
    0x100,
    0x200,
    0x400,
    0x800,
    0x1000,
    0x2000,
    0x4000,
    0x8000,
    0x10000,
    0x20000,
    0x40000,
    0x80000,
    0x100000,
    0x200000,
    0x400000,
    0x800000,
    0x1000000,
    0x2000000,
    0x4000000,
    0x8000000,
    0x10000000,
    0x20000000,
    0x40000000,
    0x80000000
};

void __stdcall TribeMPSetupScreenExtraOptionsDialog__destructor(TribeMPSetupScreenExtraOptionsDialog* this_)
{
    this_->vfptr = (TDialogPanelVtbl*)TribeMPSetupScreenExtraOptionsDialog__vftable;
    TPanel__delete_panel((TPanel*)this_, (TPanel**)&this_->title);
    TPanel__delete_panel((TPanel*)this_, (TPanel**)&this_->option_allow_spectators_title);
    TPanel__delete_panel((TPanel*)this_, (TPanel**)&this_->option_hide_names_title);
    TPanel__delete_panel((TPanel*)this_, (TPanel**)&this_->option_allow_spectators_button);
    TPanel__delete_panel((TPanel*)this_, (TPanel**)&this_->option_hide_names_button);

    TPanel__delete_panel((TPanel*)this_, (TPanel**)&this_->ok_button);
    TPanel__delete_panel((TPanel*)this_, (TPanel**)&this_->cancel_button);
    TDialogPanel__destructor((TDialogPanel*)this_);
}

void* __fastcall TribeMPSetupScreenExtraOptionsDialog__vector_deleting_destructor(TribeMPSetupScreenExtraOptionsDialog* this_, DWORD dummy, unsigned int a2)
{
    UNREFERENCED_PARAMETER(dummy);

    TribeMPSetupScreenExtraOptionsDialog__destructor(this_);
    if ((a2 & 1) != 0)
        operator_delete_internal(this_);
    return this_;
}

#define OPTION_ALLOW_SPECTATORS_ACTION_ID 1
#define OPTION_HIDE_NAMES_ACTION_ID 2
#define OK_ACTION_ID 3
#define CANCEL_ACTION_ID 4

int __fastcall TribeMPSetupScreenExtraOptionsDialog__action(TribeMPSetupScreenExtraOptionsDialog* this_, DWORD dummy, TPanel* fromPanel, int actionIn, unsigned int a1, unsigned int a2)
{
    UNREFERENCED_PARAMETER(dummy);

    UNREFERENCED_PARAMETER(fromPanel);
    UNREFERENCED_PARAMETER(a2);

    //a1 = action_id
    switch (a1)
    {
    /*
    case OPTION_HIDE_NAMES_ACTION_ID:
        if (actionIn == 1) //button click
        {
            int state = TButtonPanel__get_state((TButtonPanel*)fromPanel);
            //__debugbreak();
        }

        break;
    */
    case OK_ACTION_ID:
        if (actionIn == 1)
        {
            if (TCommunications_Handler__IsHost(*comm))
            {
                extra_options.allow_spectators = TButtonPanel__get_state((TButtonPanel*)this_->option_allow_spectators_button);
                extra_options.hide_names = TButtonPanel__get_state((TButtonPanel*)this_->option_hide_names_button);
                //if (extra_options.hide_names)
                //{
                    //for (int i = 1; i < 9; i++)
                    //    TCommunications_Handler__SetPlayerName(*comm, i, "Hidden");

                    TribeMPSetupScreen__fillPlayers((TribeMPSetupScreen*)this_->parent_panel);
                //}
                (*base_game)->vfptr->send_game_options(*base_game);
            }
            
            TPanelSystem__destroyPanel(panel_system, aExtraOptionsTitle);
        }
        break;
    case CANCEL_ACTION_ID:
    {
        if (actionIn == 1)
        {
            TPanelSystem__destroyPanel(panel_system, aExtraOptionsTitle);
        }
    }
        break;
    default:
        break;
    }
    return 1;
}

int __stdcall Comm_PreprocessMessages(char* data)
{
    if (*data == 10)
    {
        MessageBox(0, "Received", "OK", 0);
        return 1;
    }
    else
        return 0;
}

__declspec(naked) void Comm_PreprocessMessages_wr() //0043531A
{
    __asm
    {
        push    ebx
        call    Comm_PreprocessMessages_wr
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        add     esp, 3A4h
        ret     14h
    }
}

#define EXTRA_OPTIONS_WINDOW_WIDTH 560
#define EXTRA_OPTIONS_WINDOW_HEIGHT 410
#define EXTRA_OPTIONS_WINDOW_VERT_MARGIN 15
#define EXTRA_OPTIONS_WINDOW_HORZ_MARGIN 20
#define EXTRA_OPTIONS_WINDOW_CLIENT_AREA_TOP_MARGIN 80
#define EXTRA_OPTIONS_WINDOW_CLIENT_AREA_BOTTOM_MARGIN 50

#define EXTRA_OPTIONS_WINDOW_CHECKBOX_SIZE 28
#define EXTRA_OPTIONS_WINDOW_OPTION_TEXT_OFFSET 2
#define EXTRA_OPTIONS_WINDOW_BUTTON_WIDTH 120
#define EXTRA_OPTIONS_WINDOW_BUTTON_HEIGHT 36
#define EXTRA_OPTIONS_WINDOW_OPTION_SPACING 50

TribeMPSetupScreenExtraOptionsDialog* __stdcall TribeMPSetupScreenExtraOptionsDialog__TribeMPSetupScreenExtraOptionsDialog(
    TribeMPSetupScreenExtraOptionsDialog* this_,
    TScreenPanel* parent_panel_in
)
{
    TDialogPanel__TDialogPanel((TDialogPanel*)this_, (char*)aExtraOptionsTitle);

    this_->vfptr = (TDialogPanelVtbl*)TribeMPSetupScreenExtraOptionsDialog__vftable;

    TDialogPanel__setup2(
        (TDialogPanel*)this_,
        parent_panel_in->vfptr->renderArea(parent_panel_in),
        (TPanel*)parent_panel_in,
        EXTRA_OPTIONS_WINDOW_WIDTH,
        EXTRA_OPTIONS_WINDOW_HEIGHT,
        (char*)TEasy_Panel__get_popup_info_file((TEasy_Panel*)parent_panel_in),
        TEasy_Panel__get_popup_info_id((TEasy_Panel*)parent_panel_in),
        1);
        
    TEasy_Panel__create_text6(
        (TEasy_Panel*)this_,
        (TPanel*)this_,
        &this_->title,
        "Additional Options",
        EXTRA_OPTIONS_WINDOW_HORZ_MARGIN,
        EXTRA_OPTIONS_WINDOW_VERT_MARGIN,
        EXTRA_OPTIONS_WINDOW_WIDTH - (EXTRA_OPTIONS_WINDOW_HORZ_MARGIN * 2),
        EXTRA_OPTIONS_WINDOW_CLIENT_AREA_TOP_MARGIN - EXTRA_OPTIONS_WINDOW_VERT_MARGIN * 2,
        1,
        1,
        0,
        0);

    TEasy_Panel__create_check_box(
        (TEasy_Panel*)this_,
        (TPanel*)this_,
        &this_->option_allow_spectators_button,
        EXTRA_OPTIONS_WINDOW_HORZ_MARGIN,
        EXTRA_OPTIONS_WINDOW_CLIENT_AREA_TOP_MARGIN,
        EXTRA_OPTIONS_WINDOW_CHECKBOX_SIZE,
        EXTRA_OPTIONS_WINDOW_CHECKBOX_SIZE,
        0,
        OPTION_ALLOW_SPECTATORS_ACTION_ID
    );

    TEasy_Panel__create_check_box(
        (TEasy_Panel*)this_,
        (TPanel*)this_,
        &this_->option_hide_names_button,
        EXTRA_OPTIONS_WINDOW_HORZ_MARGIN,
        EXTRA_OPTIONS_WINDOW_CLIENT_AREA_TOP_MARGIN + EXTRA_OPTIONS_WINDOW_OPTION_SPACING,
        EXTRA_OPTIONS_WINDOW_CHECKBOX_SIZE,
        EXTRA_OPTIONS_WINDOW_CHECKBOX_SIZE,
        0,
        OPTION_HIDE_NAMES_ACTION_ID
    );

    TButtonPanel__set_state(this_->option_allow_spectators_button, extra_options.allow_spectators);
    TButtonPanel__set_state(this_->option_hide_names_button, extra_options.hide_names);
    if (!TCommunications_Handler__IsHost(*comm))
    {
        TButtonPanel__set_disabled(this_->option_allow_spectators_button, 1);
        TButtonPanel__set_disabled(this_->option_hide_names_button, 1);
    }

    TEasy_Panel__create_text6(
        (TEasy_Panel*)this_,
        (TPanel*)this_,
        &this_->option_allow_spectators_title,
        "Allow Spectators",
        EXTRA_OPTIONS_WINDOW_HORZ_MARGIN + EXTRA_OPTIONS_WINDOW_CHECKBOX_SIZE,
        EXTRA_OPTIONS_WINDOW_CLIENT_AREA_TOP_MARGIN + EXTRA_OPTIONS_WINDOW_OPTION_TEXT_OFFSET,
        EXTRA_OPTIONS_WINDOW_WIDTH - (EXTRA_OPTIONS_WINDOW_HORZ_MARGIN * 2),
        EXTRA_OPTIONS_WINDOW_CHECKBOX_SIZE,
        0,
        0,
        0,
        0
    );

    TEasy_Panel__create_text6(
        (TEasy_Panel*)this_,
        (TPanel*)this_,
        &this_->option_hide_names_title,
        "Hide Player Names",
        EXTRA_OPTIONS_WINDOW_HORZ_MARGIN + EXTRA_OPTIONS_WINDOW_CHECKBOX_SIZE,
        EXTRA_OPTIONS_WINDOW_CLIENT_AREA_TOP_MARGIN + EXTRA_OPTIONS_WINDOW_OPTION_TEXT_OFFSET + EXTRA_OPTIONS_WINDOW_OPTION_SPACING,
        EXTRA_OPTIONS_WINDOW_WIDTH - (EXTRA_OPTIONS_WINDOW_HORZ_MARGIN * 2),
        EXTRA_OPTIONS_WINDOW_CHECKBOX_SIZE,
        0,
        0,
        0,
        0
    );

    TEasy_Panel__create_button2(
        (TEasy_Panel*)this_,
        (TPanel*)this_,
        &this_->ok_button,
        "OK",
        NULL,
        (EXTRA_OPTIONS_WINDOW_WIDTH - (3 * EXTRA_OPTIONS_WINDOW_HORZ_MARGIN)) / 2 - EXTRA_OPTIONS_WINDOW_BUTTON_WIDTH + EXTRA_OPTIONS_WINDOW_HORZ_MARGIN,
        EXTRA_OPTIONS_WINDOW_HEIGHT - EXTRA_OPTIONS_WINDOW_CLIENT_AREA_BOTTOM_MARGIN,
        EXTRA_OPTIONS_WINDOW_BUTTON_WIDTH,
        EXTRA_OPTIONS_WINDOW_BUTTON_HEIGHT,
        0,
        0,
        OK_ACTION_ID
    );

    TEasy_Panel__create_button2(
        (TEasy_Panel*)this_,
        (TPanel*)this_,
        &this_->cancel_button,
        "Cancel",
        NULL,
        (EXTRA_OPTIONS_WINDOW_WIDTH - (3 * EXTRA_OPTIONS_WINDOW_HORZ_MARGIN)) / 2 - EXTRA_OPTIONS_WINDOW_BUTTON_WIDTH + 2 * EXTRA_OPTIONS_WINDOW_HORZ_MARGIN + EXTRA_OPTIONS_WINDOW_BUTTON_WIDTH,
        EXTRA_OPTIONS_WINDOW_HEIGHT - EXTRA_OPTIONS_WINDOW_CLIENT_AREA_BOTTOM_MARGIN,
        EXTRA_OPTIONS_WINDOW_BUTTON_WIDTH,
        EXTRA_OPTIONS_WINDOW_BUTTON_HEIGHT,
        0,
        0,
        CANCEL_ACTION_ID
    );

    return this_;
}

void __stdcall display_dialog(TribeMPSetupScreen* setup_screen)
{
    TribeMPSetupScreenExtraOptionsDialog* dialog = (TribeMPSetupScreenExtraOptionsDialog*)operator_new_internal(sizeof(TribeMPSetupScreenExtraOptionsDialog));

    TribeMPSetupScreenExtraOptionsDialog__TribeMPSetupScreenExtraOptionsDialog(dialog, (TScreenPanel*)setup_screen);
}

__declspec(naked) void mp_setup_screen_destructor() //00516F5C
{
    __asm
    {
        call    TPanelSystem__destroyPanel
        push    offset aExtraOptionsTitle
        mov     ecx, panel_system
        call    TPanelSystem__destroyPanel
        mov     eax, 00516F61h
        jmp     eax
    }
}

void __stdcall set_my_game_options_wr(TCommunications_Handler* comm, char* vpData, unsigned int dwSize)
{
    char* new_data = (char*)malloc(dwSize + sizeof(EXTRA_GAME_OPTIONS));
    memcpy(new_data, vpData, dwSize);
    memcpy(new_data + dwSize, &extra_options, sizeof(EXTRA_GAME_OPTIONS));
    log("SetMyGameOptions(): sent options: allow_spectators=%d, hide_names=%d",
        extra_options.allow_spectators, extra_options.hide_names);
    TCommunications_Handler__SetMyGameOptions(comm, new_data, dwSize + sizeof(EXTRA_GAME_OPTIONS));
    free(new_data);
}

__declspec(naked) void set_my_game_options_1() //0042A421
{
    __asm
    {
        push    0B0h
        push    eax
        push    ecx
        call    set_my_game_options_wr
        mov     eax, 0042A42Ch
        jmp     eax
    }
}

__declspec(naked) void set_my_game_options_2() //005EE60C
{
    __asm
    {
        push    eax
        push    ebx
        mov     ecx, [ecx + 1B4h]
        push    ecx
        call    set_my_game_options_wr
        mov     eax, 005EE619h
        jmp     eax
    }
}

void* __fastcall TCommunications_Handler__GetMyGameOptions_new(TCommunications_Handler* comm, DWORD dummy, unsigned int* dwSize)
{
    UNREFERENCED_PARAMETER(dummy);

    *dwSize = comm->PlayerOptions.DataSizeToFollow;

    uint32_t magic = EXTRA_OPTIONS_MAGIC;
    if (!memcmp(comm->OptionsData + *dwSize - sizeof(EXTRA_GAME_OPTIONS) + offsetof(EXTRA_GAME_OPTIONS, magic), &magic, sizeof(magic)))
    {
        memcpy(&extra_options, comm->OptionsData + *dwSize - sizeof(EXTRA_GAME_OPTIONS), sizeof(EXTRA_GAME_OPTIONS));
        /*
        if (extra_options.hide_names)
        {
            TribeMPSetupScreen* setup_screen = (TribeMPSetupScreen*)TPanelSystem__panel(panel_system, "MP Setup Screen");
            if (setup_screen)
                TribeMPSetupScreen__fillPlayers(setup_screen);
        }
        */
        log("GetMyGameOptions(): received options: allow_spectators=%d, hide_names=%d",
            extra_options.allow_spectators, extra_options.hide_names);
    }
    else
    {
        log("GetMyGameOptions(): no valid magic!");
    }

    return comm->OptionsData;
}

TButtonPanel* mp_setup_screen_extra_options_button = NULL;

void __stdcall on_create_mp_setup_screen(TribeMPSetupScreen* setup_screen)
{
    TEasy_Panel__create_button(
        (TEasy_Panel*)setup_screen,
        (TPanel*)setup_screen,
        &mp_setup_screen_extra_options_button,
        -1,
        0,
        230,
        498,
        37,
        37,
        0,
        0,
        0);

    TButtonPanel__setDrawType(mp_setup_screen_extra_options_button, 9);
    TButtonPanel__set_picture_info(mp_setup_screen_extra_options_button, setup_screen->button_cmd_pic, extra_options_button_pic_index, 0, 0, 1, 0);
    TEasy_Panel__set_rollover((TEasy_Panel*)setup_screen, (TPanel*)mp_setup_screen_extra_options_button, 30215, -1);
}

__declspec(naked) void on_create_mp_setup_screen_wr() //00515AB4
{
    __asm
    {
        call    TEasy_Panel__create_button
        push    eax
        push    esi
        call    on_create_mp_setup_screen
        pop     eax
        mov     ecx, 00515AB9h
        jmp     ecx
    }
}

void __stdcall on_mp_setup_screen_init_vars()
{
#ifdef TARGET_VOOBLY
    extra_options.allow_spectators = true;
#else
    extra_options.allow_spectators = false;
#endif
    extra_options.hide_names = false;;
    (*base_game)->vfptr->send_game_options(*base_game);
}

__declspec(naked) void on_mp_setup_screen_init_vars_wr() //00516D91
{
    __asm
    {
        call    on_mp_setup_screen_init_vars
        mov     ecx, 00516D9Ch
        jmp     ecx
    }
}

void __stdcall mp_setup_screen_button_set_disabled(TribeMPSetupScreen* setup_screen)
{
    TButtonPanel__set_disabled(setup_screen->techTreeButton, setup_screen->i_am_ready);
    TButtonPanel__set_disabled(mp_setup_screen_extra_options_button, setup_screen->i_am_ready);
}

__declspec(naked) void on_mp_setup_screen_button_set_disabled_1() //0051808C
{
    __asm
    {
        push    esi
        call    mp_setup_screen_button_set_disabled
        mov     eax, 0051809Eh
        jmp     eax
    }
}

__declspec(naked) void on_mp_setup_screen_button_set_disabled_2() //00520F79
{
    __asm
    {
        push    esi
        call    mp_setup_screen_button_set_disabled
        mov     eax, 00520F8Bh
        jmp     eax
    }
}

void __stdcall on_mp_setup_screen_destructor(TribeMPSetupScreen* setup_screen)
{
    TPanel__delete_panel((TPanel*)setup_screen, (TPanel**)&mp_setup_screen_extra_options_button);
}

__declspec(naked) void on_mp_setup_screen_destructor_wr() //005173D0
{
    __asm
    {
        call    TPanel__delete_panel
        push    esi
        call    on_mp_setup_screen_destructor
        mov     eax, 005173D5h
        jmp     eax
    }
}

int __stdcall check_mp_setup_screen_action(TribeMPSetupScreen* setup_screen, TPanel* fromPanel, int actionIn)
{
    if (fromPanel == (TPanel*)mp_setup_screen_extra_options_button && actionIn == 1)
    {
        display_dialog(setup_screen);
        return 1;
    }
    else
    {
        return 0;
    }
}

__declspec(naked) void on_mp_setup_screen_action() //005182C1
{
    __asm
    {
        cmp     ebx, [esi + 0D38h]
        jz      show_net_info_btn

        push    ebp
        push    ebx
        push    esi
        call    check_mp_setup_screen_action
        test    eax, eax
        jnz     setup_screen_action_processed

        mov     eax, 005182DCh
        jmp     eax

setup_screen_action_processed:
        mov     ecx, 00519175h
        jmp     ecx

show_net_info_btn:
        mov     eax, 005182C9h
        jmp     eax
    }
}

#define HIDE_NAMES_PLAYER_NAME "Hidden"

char* __fastcall TCommunications_Handler__GetPlayerName_new(TCommunications_Handler* comm, DWORD dummy, unsigned int PlayerNo)
{
    UNREFERENCED_PARAMETER(dummy);

    if (extra_options.hide_names)
    {
        TCommunications_Handler__SetPlayerName(comm, PlayerNo, HIDE_NAMES_PLAYER_NAME);
        return HIDE_NAMES_PLAYER_NAME;
    }
    else if (PlayerNo > comm->MaxGamePlayers)
        return NULL;
    else if (PlayerNo == comm->Me)
        return comm->MyFriendlyName;
    else
        return comm->FriendlyName[PlayerNo].Text;
}

char* __fastcall RGE_Game_Info__get_current_player_name_new(RGE_Game_Info* game_info)
{
    if (extra_options.hide_names)
        return HIDE_NAMES_PLAYER_NAME;
    else if (game_info->current_person >= 0)
        return game_info->people_info[game_info->current_person]->name;
    else
        return NULL;
}

#pragma optimize( "s", on )
void setMPSetupScreenHooks(int ver)
{
    setHook((void*)0x00516F5C, mp_setup_screen_destructor);
    setHook((void*)0x00515AB4, on_create_mp_setup_screen_wr);
    setHook((void*)0x005173D0, on_mp_setup_screen_destructor_wr);
    setHook((void*)0x005182C1, on_mp_setup_screen_action);
    setHook((void*)0x00516D91, on_mp_setup_screen_init_vars_wr);
    setHook((void*)0x0051808C, on_mp_setup_screen_button_set_disabled_1);
    setHook((void*)0x00520F79, on_mp_setup_screen_button_set_disabled_2);


    setHook((void*)0x0042A421, set_my_game_options_1);
    setHook((void*)0x005EE60C, set_my_game_options_2);
    setHook((void*)0x0043DC60, TCommunications_Handler__GetMyGameOptions_new);

    memset(&extra_options, 0, sizeof(EXTRA_GAME_OPTIONS));
    extra_options.allow_spectators = false;
    extra_options.hide_names = false;
    extra_options.magic = EXTRA_OPTIONS_MAGIC;

    if (ver == VER_EF)
        extra_options_button_pic_index = 17;
    else
        extra_options_button_pic_index = 8;
    //

    setHook((void*)0x0043B5A0, TCommunications_Handler__GetPlayerName_new);
    //setHook((void*)0x00478210, RGE_Game_Info__get_current_player_name_new);

    //setHook((void*)0x0043531A, Comm_PreprocessMessages_wr);
}
#pragma optimize( "", on )
