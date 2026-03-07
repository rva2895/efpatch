#include "stdafx.h"
#include "registry.h"
#include "startupload.h"

#include <shellapi.h>

std::wstring startup_filename;

bool check_command_line_argument(LPWSTR arg, const wchar_t* ext)
{
    std::wstring cmd_arg(arg);
    _wcsupr(&cmd_arg[0]);
    cmd_arg.resize(wcslen(cmd_arg.c_str()));

    if (cmd_arg.find(ext) != std::wstring::npos)
    {
        startup_filename = arg;
        return true;
    }
    else
    {
        return false;
    }
}

bool check_command_line(LPWSTR* command_line, int num_args, const wchar_t* ext)
{
    for (int i = 0; i < num_args; i++)
        if (check_command_line_argument(command_line[i], ext))
            return true;

    return false;
}

bool startup_game = false;
bool replay_startup = false;
bool edit_scenario = false;

void __stdcall setup_startup_filename(TRIBE_Game* game)
{
    if (startup_game)
        strlcpy(game->startup_game, WideToUTF8_c_str(startup_filename), _countof(game->startup_game));
    else if (edit_scenario)
        strlcpy(game->edit_scenario, WideToUTF8_c_str(startup_filename), _countof(game->edit_scenario));
}

__declspec(naked) void on_game_setup() //005E4A83
{
    __asm
    {
        push    ecx
        call    setup_startup_filename
        xor     ebx, ebx
        push    esi
        push    edi
        cmp     [ebp + 60h], ebx
        mov     ecx, ebp
        mov     eax, 005E4A8Ah
        jmp     eax
    }
}

bool is_abs_path(const char* s)
{
    return strchr(s, '\\') != NULL;
}

char* __stdcall load_game_or_scen_abs_path_sub(bool scen, char* dest, const char* name)
{
    if ((scen ? edit_scenario : startup_game) && is_abs_path(name))
        strlcpy(dest, name, 300);

    if ((startup_game || edit_scenario) && (*base_game)->player_game_info)
        RGE_Game_Info__retrieve_program_options((*base_game)->player_game_info);

    return dest;
}

__declspec(naked) void load_game_abs_path() //0061D928
{
    __asm
    {
        mov     eax, [esp + 1138h]
        lea     ecx, [esp + 0A8h]
        push    eax
        push    ecx
        push    0
        call    load_game_or_scen_abs_path_sub

        mov     edx, 8000h
        mov     ecx, eax
        mov     eax, 0061D934h
        jmp     eax
    }
}

__declspec(naked) void load_scen_abs_path() //00620F74
{
    __asm
    {
        mov     eax, [esp + 148h]
        lea     ecx, [esp + 18h]
        push    eax
        push    ecx
        push    1
        call    load_game_or_scen_abs_path_sub

        mov     edx, 8000h
        mov     ecx, eax
        mov     eax, 00620F7Dh
        jmp     eax
    }
}

__declspec(naked) void load_game_set_replay_mode() //005E5662
{
    __asm
    {
        xor     ecx, ecx
        mov     cl, replay_startup
        push    ecx
        push    eax
        mov     ecx, ebp
        call    TRIBE_Game__load_game
        mov     ecx, 005E55F4h
        jmp     ecx
    }
}

#pragma optimize( "s", on )
bool setStartupLoadHooks()
{
    int num_args = 0;
    LPWSTR* command_line = CommandLineToArgvW(GetCommandLine(), &num_args);

    //savegame
    if (check_command_line(command_line, num_args, L".GA1"))
    {
        cd.gameVersion = VER_CC;
        startup_game = true;
    }
    else if (check_command_line(command_line, num_args, L".GA2"))
    {
        cd.gameVersion = VER_EF;
        startup_game = true;
    }
    //recorded game
    else if (check_command_line(command_line, num_args, L".MG1"))
    {
        cd.gameVersion = VER_CC;
        startup_game = true;
        replay_startup = true;
    }
    else if (check_command_line(command_line, num_args, L".MG2"))
    {
        cd.gameVersion = VER_EF;
        startup_game = true;
        replay_startup = true;
    }
    //scenario
    else if (check_command_line(command_line, num_args, L".SCX"))
    {
        edit_scenario = true;
    }
    else if (check_command_line(command_line, num_args, L".SC1"))
    {
        edit_scenario = true;
    }
    else if (check_command_line(command_line, num_args, L".SC2"))
    {
        cd.gameVersion = VER_EF;
        edit_scenario = true;
    }

    LocalFree(command_line);

    setHook((void*)0x005E4A83, on_game_setup);
    setHook((void*)0x0061D928, load_game_abs_path);
    setHook((void*)0x005E5662, load_game_set_replay_mode);
    setHook((void*)0x00620F74, load_scen_abs_path);

    return startup_game || edit_scenario;
}
#pragma optimize( "", on )
