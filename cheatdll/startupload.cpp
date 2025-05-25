#include "stdafx.h"
#include "registry.h"
#include "startupload.h"

extern CONFIG_DATA cd;

char* getCmdFilename(char* s)
{
    char* p = s + strlen(s) - 1;
    bool k = (*p == '\"');
    if (k)
        *p = 0;
    while (k ? (*p != '\"') : (*p != ' '))
        p--;
    p++;
    //if (k)
    //    p++;
    char* f = make_str_copy(p);
    return f;
}

char* __stdcall checkCmdLine(const char* command_line, const char* ext)
{
    char* cmdLine = make_str_copy(command_line);

    _strupr(cmdLine);

    char* ext_start = strstr(cmdLine, ext);
    if (ext_start)
    {
        char* filename = getCmdFilename(cmdLine);
        free(cmdLine);
        return filename;
    }
    else
    {
        free(cmdLine);
        return NULL;
    }
}

bool startup_game = false;
bool replay_startup = false;
bool edit_scenario = false;
char* startup_filename = NULL;

void copy_filename(const char* s)
{
    startup_filename = make_str_copy(s);
}

void __stdcall setup_startup_filename(TRIBE_Game* game)
{
    if (startup_game)
        strlcpy(game->startup_game, startup_filename, _countof(game->startup_game));
    else if (edit_scenario)
        strlcpy(game->edit_scenario, startup_filename, _countof(game->edit_scenario));
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
    return strstr(s, "\\");
}

char* __stdcall load_game_or_scen_abs_path_sub(bool scen, char* dest, const char* name)
{
    if ((scen ? edit_scenario : startup_game) && is_abs_path(name))
        strlcpy(dest, name, 300);

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
    char* filename;
    char* command_line = GetCommandLine();

    //savegame
    if (filename = checkCmdLine(command_line, ".GA1"))
    {
        cd.gameVersion = VER_CC;
        startup_game = true;
        copy_filename(filename);
    }
    else if (filename = checkCmdLine(command_line, ".GA2"))
    {
        cd.gameVersion = VER_EF;
        startup_game = true;
        copy_filename(filename);
    }
    //recorded game
    else if (filename = checkCmdLine(command_line, ".MG1"))
    {
        cd.gameVersion = VER_CC;
        startup_game = true;
        replay_startup = true;
        copy_filename(filename);
    }
    else if (filename = checkCmdLine(command_line, ".MG2"))
    {
        cd.gameVersion = VER_EF;
        startup_game = true;
        replay_startup = true;
        copy_filename(filename);
    }
    //scenario
    else if (filename = checkCmdLine(command_line, ".SCX"))
    {
        cd.gameVersion = VER_CC;
        edit_scenario = true;
        copy_filename(filename);
    }
    else if (filename = checkCmdLine(command_line, ".SC1"))
    {
        cd.gameVersion = VER_CC;
        edit_scenario = true;
        copy_filename(filename);
    }
    else if (filename = checkCmdLine(command_line, ".SC2"))
    {
        cd.gameVersion = VER_EF;
        edit_scenario = true;
        copy_filename(filename);
    }

    setHook((void*)0x005E4A83, on_game_setup);
    setHook((void*)0x0061D928, load_game_abs_path);
    setHook((void*)0x005E5662, load_game_set_replay_mode);
    setHook((void*)0x00620F74, load_scen_abs_path);

    return startup_game || edit_scenario;
}
#pragma optimize( "", on )
