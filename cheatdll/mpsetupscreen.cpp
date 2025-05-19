#include "stdafx.h"
#include "mpsetupscreen.h"

void* TribeMPSetupScreenExtraOptionsDialog__vector_deleting_destructor_wr();
int TribeMPSetupScreenExtraOptionsDialog__action_wr();
int __fastcall TribeMPSetupScreenExtraOptionsDialog__action(TribeMPSetupScreenExtraOptionsDialog* this_, DWORD dummy, TPanel* fromPanel, int actionIn, unsigned int a1, unsigned int a2);

const char aExtraOptionsTitle[] = "TribeMPSetupScreenExtraOptionsDialog";

EXTRA_GAME_OPTIONS extra_options;

const DWORD TribeMPSetupScreenExtraOptionsDialog__vftable[] =
{
    (DWORD)TribeMPSetupScreenExtraOptionsDialog__vector_deleting_destructor_wr,
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
    TPanel__delete_panel((TPanel*)this_, (TPanel**)&this_->option1_title);
    TPanel__delete_panel((TPanel*)this_, (TPanel**)&this_->option1_button);

    TPanel__delete_panel((TPanel*)this_, (TPanel**)&this_->ok_button);
    TPanel__delete_panel((TPanel*)this_, (TPanel**)&this_->cancel_button);
    TDialogPanel__destructor((TDialogPanel*)this_);
}

void* __stdcall TribeMPSetupScreenExtraOptionsDialog__vector_deleting_destructor(TribeMPSetupScreenExtraOptionsDialog* this_, unsigned int a2)
{
    TribeMPSetupScreenExtraOptionsDialog__destructor(this_);
    if ((a2 & 1) != 0)
        operator_delete_internal(this_);
    return this_;
}

__declspec(naked) void* TribeMPSetupScreenExtraOptionsDialog__vector_deleting_destructor_wr()
{
    __asm
    {
        mov     eax, [esp + 4]
        push    eax
        push    ecx
        call    TribeMPSetupScreenExtraOptionsDialog__vector_deleting_destructor
        ret     4
    }
}

#define OPTION_1_ACTION_ID 1
#define OK_ACTION_ID 2
#define CANCEL_ACTION_ID 3

int __fastcall TribeMPSetupScreenExtraOptionsDialog__action(TribeMPSetupScreenExtraOptionsDialog* this_, DWORD dummy, TPanel* fromPanel, int actionIn, unsigned int a1, unsigned int a2)
{
    UNREFERENCED_PARAMETER(dummy);
    //a1 = action_id
    switch (a1)
    {
    case OPTION_1_ACTION_ID:
        if (actionIn == 1) //button click
        {
            int state = TButtonPanel__get_state((TButtonPanel*)fromPanel);
            //__debugbreak();
        }

        break;
    case OK_ACTION_ID:
        if (actionIn == 1)
        {
            if (TCommunications_Handler__IsHost(*comm))
            {
                extra_options.hide_names = TButtonPanel__get_state((TButtonPanel*)this_->option1_button);
                if (extra_options.hide_names)
                {
                    //for (int i = 1; i < 9; i++)
                    //    TCommunications_Handler__SetPlayerName(*comm, i, "Hidden");

                    TribeMPSetupScreen__fillPlayers((TribeMPSetupScreen*)this_->parent_panel);
                }
                (*base_game)->vfptr->send_game_options(*base_game);
            }
            
            TPanelSystem__destroyPanel(panel_system, aExtraOptionsTitle);
        }
        break;
    case CANCEL_ACTION_ID:
    {
        if (actionIn == 1)
        {
            int msg = 10;
            __debugbreak();
            TCommunications_Handler__CommOut(*comm, 0, 10, &msg, 4, 0);
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

__declspec(naked) int TribeMPSetupScreenExtraOptionsDialog__action_wr()
{
    __asm
    {
        push    esi
        push    edi
        mov     esi, ecx
        mov     eax, [esp + 0Ch]
        mov     ecx, [esp + 10h]
        mov     edx, [esp + 14h]
        mov     edi, [esp + 18h]
        push    edi
        push    edx
        push    ecx
        push    eax
        push    esi
        call    TribeMPSetupScreenExtraOptionsDialog__action

        pop     edi
        pop     esi
        ret     10h
    }
}

#define EXTRA_OPTIONS_WINDOW_WIDTH 560
#define EXTRA_OPTIONS_WINDOW_HEIGHT 410
#define EXTRA_OPTIONS_WINDOW_VERT_MARGIN 15
#define EXTRA_OPTIONS_WINDOW_HORZ_MARGIN 20
#define EXTRA_OPTIONS_WINDOW_CLIENT_AREA_TOP_MARGIN 100
#define EXTRA_OPTIONS_WINDOW_CLIENT_AREA_BOTTOM_MARGIN 60

#define EXTRA_OPTIONS_WINDOW_CHECKBOX_SIZE 30
#define EXTRA_OPTIONS_WINDOW_BUTTON_WIDTH 120
#define EXTRA_OPTIONS_WINDOW_BUTTON_HEIGHT 40

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
        "Options",
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
        &this_->option1_button,
        EXTRA_OPTIONS_WINDOW_HORZ_MARGIN,
        EXTRA_OPTIONS_WINDOW_CLIENT_AREA_TOP_MARGIN,
        EXTRA_OPTIONS_WINDOW_CHECKBOX_SIZE,
        EXTRA_OPTIONS_WINDOW_CHECKBOX_SIZE,
        0,
        OPTION_1_ACTION_ID
    );

    TButtonPanel__set_state(this_->option1_button, extra_options.hide_names);
    if (!TCommunications_Handler__IsHost(*comm))
        TButtonPanel__set_disabled(this_->option1_button, 1);

    TEasy_Panel__create_text6(
        (TEasy_Panel*)this_,
        (TPanel*)this_,
        &this_->option1_title,
        "Hide Player Names",
        EXTRA_OPTIONS_WINDOW_HORZ_MARGIN + EXTRA_OPTIONS_WINDOW_CHECKBOX_SIZE,
        EXTRA_OPTIONS_WINDOW_CLIENT_AREA_TOP_MARGIN,
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
        "OK", "text2",
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
        "Cancel", "text2",
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
    log("SetMyGameOptions(): sent options: hide_names=%d", extra_options.hide_names);
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

void* __stdcall TCommunications_Handler__GetMyGameOptions_new(TCommunications_Handler* comm, unsigned int* dwSize)
{

    *dwSize = comm->PlayerOptions.DataSizeToFollow;

    DWORD magic = EXTRA_OPTIONS_MAGIC;
    if (!memcmp(comm->OptionsData + *dwSize - sizeof(EXTRA_GAME_OPTIONS) + offsetof(EXTRA_GAME_OPTIONS, magic), &magic, sizeof(DWORD)))
    {
        memcpy(&extra_options, comm->OptionsData + *dwSize - sizeof(EXTRA_GAME_OPTIONS), sizeof(EXTRA_GAME_OPTIONS));
        log("GetMyGameOptions(): received options: hide_names=%d", extra_options.hide_names);
    }
    else
    {
        log("GetMyGameOptions(): no valid magic!");
    }

    return comm->OptionsData;
}

__declspec(naked) void get_my_game_options() //0043DC60
{
    __asm
    {
        mov     edx, [esp + 4]
        push    edx
        push    ecx
        call    TCommunications_Handler__GetMyGameOptions_new
        ret     4
    }
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
        495,
        37,
        37,
        0,
        0,
        0);
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

char* __stdcall TCommunications_Handler__GetPlayerName_new(TCommunications_Handler* comm, unsigned int PlayerNo)
{
    if (extra_options.hide_names)
    {
        //TCommunications_Handler__SetPlayerName(comm, PlayerNo, "Hidden");
        return "Hidden";
    }
    else if (PlayerNo > comm->MaxGamePlayers)
        return NULL;
    else if (PlayerNo == comm->Me)
        return comm->MyFriendlyName;
    else
        return comm->FriendlyName[PlayerNo].Text;
}

__declspec(naked) char* comm_get_player_name_wr() //0043B5A0
{
    __asm
    {
        mov     eax, [esp + 4]
        push    eax
        push    ecx
        call    TCommunications_Handler__GetPlayerName_new
        ret     4
    }
}

char* __fastcall RGE_Game_Info__get_current_player_name_new(RGE_Game_Info* game_info)
{
    if (extra_options.hide_names)
        return "Hidden";
    else if (game_info->current_person >= 0)
        return game_info->people_info[game_info->current_person]->name;
    else
        return NULL;
}

void __stdcall TCommunications_Handler__SwitchPlayers(TCommunications_Handler* fcomm, int p1, int p2)
{

}

#pragma optimize( "s", on )
void setMPSetupScreenHooks()
{
    setHook((void*)0x00516F5C, mp_setup_screen_destructor);
    setHook((void*)0x00515AB4, on_create_mp_setup_screen_wr);
    setHook((void*)0x005173D0, on_mp_setup_screen_destructor_wr);
    setHook((void*)0x005182C1, on_mp_setup_screen_action);

    setHook((void*)0x0042A421, set_my_game_options_1);
    setHook((void*)0x005EE60C, set_my_game_options_2);
    setHook((void*)0x0043DC60, get_my_game_options);

    memset(&extra_options, 0, sizeof(EXTRA_GAME_OPTIONS));
    extra_options.magic = EXTRA_OPTIONS_MAGIC;

    //

    setHook((void*)0x0043B5A0, comm_get_player_name_wr);
    //setHook((void*)0x00478210, RGE_Game_Info__get_current_player_name_new);

    setHook((void*)0x0043531A, Comm_PreprocessMessages_wr);
}
#pragma optimize( "", on )
