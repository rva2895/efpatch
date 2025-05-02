#include "stdafx.h"
#include "loadfilelist.h"
#include <io.h>

int loadfilelist_version;

const char* convert_list_id_to_file_extension(int id)
{
    switch (id)
    {
    case 2:
        if (loadfilelist_version == VER_EF)
            return ".sc2";
        else
            return NULL;
    case 1:
        return ".sc1";
    case 0:
        return ".scx";
    default:
        return "";
    }
}

void __stdcall sed_open_action_1(TRIBE_Screen_Sed_Open* sed_open)
{
    char Filename[MAX_PATH];
    const char* ext = convert_list_id_to_file_extension(
        TTextPanel__get_id2((TTextPanel*)sed_open->list, TTextPanel__currentLineNumber((TTextPanel*)sed_open->list)));
    if (ext)
    {
        snprintf(
            Filename,
            _countof(Filename),
            "%s%s%s",
            (*base_game)->prog_info->scenario_dir,
            TTextPanel__get_text((TTextPanel*)sed_open->list),
            ext);
        _unlink(Filename);
    }
}

void __stdcall savegamescreen_action_1(TribeSaveGameScreen* savegamescreen)
{
    char Filename[MAX_PATH];
    const char* ext = savegamescreen->modeValue ?
        (convert_list_id_to_file_extension(
            TTextPanel__get_id2((TTextPanel*)savegamescreen->list,
                TTextPanel__currentLineNumber((TTextPanel*)savegamescreen->list)))) :
        (loadfilelist_version == VER_EF ? ".ga2" : ".ga1");

    if (ext)
    {
        snprintf(
            Filename,
            _countof(Filename),
            "%s%s%s",
            savegamescreen->modeValue ? (*base_game)->prog_info->scenario_dir : (*base_game)->prog_info->save_dir,
            TTextPanel__get_text((TTextPanel*)savegamescreen->list),
            ext);
        _unlink(Filename);
    }
}

void __stdcall sed_open_action_2(TRIBE_Screen_Sed_Open* sed_open)
{
    char Filename[MAX_PATH];
    const char* ext = convert_list_id_to_file_extension(
        TTextPanel__get_id2((TTextPanel*)sed_open->list, TTextPanel__currentLineNumber((TTextPanel*)sed_open->list)));
    if (!ext)
    {
        TEasy_Panel__popupOKDialog(
            (TEasy_Panel*)sed_open,
            "This scenario requires Expanding Fronts. Please start the game in Expanding Fronts mode",
            0,
            450,
            100,
            1);
        return;
    }

    snprintf(
        Filename,
        _countof(Filename),
        "%s%s%s",
        (*base_game)->prog_info->scenario_dir,
        TTextPanel__get_text((TTextPanel*)sed_open->list),
        ext);

    HANDLE hFile = CreateFileA(Filename, 0x80000000, 1u, 0, 3u, 0x80u, 0);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
        snprintf(
            Filename,
            _countof(Filename),
            "%s%s",
            TTextPanel__get_text((TTextPanel*)sed_open->list),
            ext);
        TRIBE_Game__start_scenario_editor((TRIBE_Game*)*base_game, Filename, 0, 0);
    }
    else
        TEasy_Panel__popupOKDialog2((TEasy_Panel*)sed_open, 9423, 0, 450, 100, 1);
}

void __fastcall TRIBE_Screen_Sed_Open__fillList_new(TRIBE_Screen_Sed_Open* sed_open) //0053FC50
{
    auto sed_open_fill_list = [](TRIBE_Screen_Sed_Open* sed_open, const char* s, int n)
        {
            char Filename[MAX_PATH];
            snprintf(Filename, _countof(Filename), s, (*base_game)->prog_info->scenario_dir);
            _finddata_t fileinfo;
            intptr_t fd = _findfirst(Filename, &fileinfo);
            if (fd != -1)
            {
                do
                {
                    size_t len = strlen(fileinfo.name) + 1;
                    strncpy(Filename, fileinfo.name, len - 5);
                    Filename[len - 5] = '\0';
                    TTextPanel__append_line((TTextPanel*)sed_open->list, Filename, n, NULL);
                } while (_findnext(fd, &fileinfo) != -1);
                _findclose(fd);
            }
        };

    TTextPanel__empty_list((TTextPanel*)sed_open->list);
    sed_open->list->sorted = 1;

    if (loadfilelist_version == VER_EF)
        sed_open_fill_list(sed_open, "%s*.sc2", 2);

    sed_open_fill_list(sed_open, "%s*.sc1", 1);
    sed_open_fill_list(sed_open, "%s*.scx", 0);
}

void __fastcall TribeSaveGameScreen__fillList_new(TribeSaveGameScreen* savegamescreen) //00528BF0
{
    auto savegamescreen_fill_list = [](TribeSaveGameScreen* savegamescreen, const char* s, const char* dir, int n)
        {
            char Filename[MAX_PATH];
            snprintf(Filename, _countof(Filename), s, dir);
            _finddata_t fileinfo;
            intptr_t fd = _findfirst(Filename, &fileinfo);
            if (fd != -1)
            {
                do
                {
                    size_t len = strlen(fileinfo.name) + 1;
                    strncpy(Filename, fileinfo.name, len - 5);
                    Filename[len - 5] = '\0';
                    TTextPanel__append_line((TTextPanel*)savegamescreen->list, Filename, n, NULL);
                } while (_findnext(fd, &fileinfo) != -1);
                _findclose(fd);
            }
        };

    TTextPanel__empty_list((TTextPanel*)savegamescreen->list);
    savegamescreen->list->sorted = 1;

    if (savegamescreen->modeValue)
    {
        if (loadfilelist_version == VER_EF)
            savegamescreen_fill_list(savegamescreen, "%s*.sc2", (*base_game)->prog_info->scenario_dir, 2);

        savegamescreen_fill_list(savegamescreen, "%s*.sc1", (*base_game)->prog_info->scenario_dir, 1);
        savegamescreen_fill_list(savegamescreen, "%s*.scx", (*base_game)->prog_info->scenario_dir, 0);
    }
    else
    {
        if (loadfilelist_version == VER_EF)
            savegamescreen_fill_list(savegamescreen, "%s*.ga2", (*base_game)->prog_info->save_dir, 2);
        else
            savegamescreen_fill_list(savegamescreen, "%s*.ga1", (*base_game)->prog_info->save_dir, 2);
    }
}

void __fastcall TribeSelectScenarioScreen__fillScenarios_new(TribeSelectScenarioScreen* sel_scen) //00540B70
{
    auto sel_scen_fill_list = [](TribeSelectScenarioScreen* sel_scen, const char* s, int n)
        {
            char Filename[MAX_PATH];
            snprintf(Filename, _countof(Filename), s, (*base_game)->prog_info->scenario_dir);
            _finddata_t fileinfo;
            intptr_t fd = _findfirst(Filename, &fileinfo);
            if (fd != -1)
            {
                do
                {
                    size_t len = strlen(fileinfo.name) + 1;
                    strncpy(Filename, fileinfo.name, len - 5);
                    Filename[len - 5] = '\0';
                    if (strcmp(Filename, "gencomb") && strncmp(Filename, "default", 7u))
                        TTextPanel__append_line((TTextPanel*)sel_scen->scenarioList, Filename, n, NULL);
                } while (_findnext(fd, &fileinfo) != -1);
                _findclose(fd);
            }
        };

    TTextPanel__empty_list((TTextPanel*)sel_scen->scenarioList);
    sel_scen->scenarioList->sorted = 1;

    if (loadfilelist_version == VER_EF)
        sel_scen_fill_list(sel_scen, "%s*.sc2", 2);

    sel_scen_fill_list(sel_scen, "%s*.sc1", 1);
    sel_scen_fill_list(sel_scen, "%s*.scx", 0);
}

__declspec(naked) void sed_open_action_1_wr() //0053FE51
{
    __asm
    {
        push    esi
        call    sed_open_action_1

        mov     ecx, [esi + 7F0h]
        mov     eax, 0053FED1h
        jmp     eax
    }
}

__declspec(naked) void savegamescreen_action_1_wr() //00528E2C
{
    __asm
    {
        push    esi
        call    savegamescreen_action_1

        mov     ecx, [esi + 7F0h]
        mov     eax, 00528EE3h
        jmp     eax
    }
}

__declspec(naked) void sed_open_action_2_wr() //0053FFE1
{
    __asm
    {
        push    esi
        call    sed_open_action_2

        mov     eax, 0054011Ah
        jmp     eax
    }
}

int __stdcall get_scenario_checksum()
{
    char Filename[MAX_PATH];
    char* scenario_name = RGE_Base_Game__scenarioName(*base_game);
    int cs = 0;
    if (loadfilelist_version == VER_EF)
    {
        snprintf(Filename, _countof(Filename), "%s.sc2", scenario_name);
        cs = RGE_Base_Game__get_scenario_checksum(*base_game, Filename);
    }
    if (!cs)
    {
        snprintf(Filename, _countof(Filename), "%s.sc1", scenario_name);
        cs = RGE_Base_Game__get_scenario_checksum(*base_game, Filename);
    }
    if (!cs)
    {
        snprintf(Filename, _countof(Filename), "%s.scx", scenario_name);
        cs = RGE_Base_Game__get_scenario_checksum(*base_game, Filename);
    }
    return cs;
}

__declspec(naked) void scenario_checksum_1_wr() //00517E42
{
    __asm
    {
        call    get_scenario_checksum
        mov     [esi + 0BCCh], eax
        mov     eax, 00517EBAh
        jmp     eax
    }
}

__declspec(naked) void scenario_checksum_2_wr() //0051DA7A
{
    __asm
    {
        call    get_scenario_checksum
        mov     [ebp + 0BCCh], eax
        mov     eax, 0051DAFEh
        jmp     eax
    }
}

__declspec(naked) void scenario_checksum_3_wr() //0051DB25
{
    __asm
    {
        call    get_scenario_checksum
        mov     edi, eax
        mov     eax, 0051DB95h
        jmp     eax
    }
}

__declspec(naked) void scenario_checksum_4_wr() //005219D4
{
    __asm
    {
        call    get_scenario_checksum
        mov     ecx, 00521A3Bh
        jmp     ecx
    }
}

__declspec(naked) void scenario_checksum_5_wr() //00522143
{
    __asm
    {
        call    get_scenario_checksum
        mov     ecx, 005221B9h
        jmp     ecx
    }
}

RGE_Scenario* __stdcall get_scenario_info(char* scenario_name)
{
    char Filename[MAX_PATH];
    RGE_Scenario* s = NULL;
    if (loadfilelist_version == VER_EF)
    {
        snprintf(Filename, _countof(Filename), "%s.sc2", scenario_name);
        s = RGE_Base_Game__get_scenario_info(*base_game, Filename, 0);
    }
    if (!s)
    {
        snprintf(Filename, _countof(Filename), "%s.sc1", scenario_name);
        s = RGE_Base_Game__get_scenario_info(*base_game, Filename, 0);
    }
    if (!s)
    {
        snprintf(Filename, _countof(Filename), "%s.scx", scenario_name);
        s = RGE_Base_Game__get_scenario_info(*base_game, Filename, 0);
    }
    return s;
}

__declspec(naked) void scenario_info_1_wr() //005EC3D2
{
    __asm
    {
        push    edi
        call    get_scenario_info
        mov     ecx, 005EC405h
        jmp     ecx
    }
}

RGE_Scenario* __stdcall get_scenario_info2()
{
    return get_scenario_info(RGE_Base_Game__scenarioName(*base_game));
}

__declspec(naked) void scenario_info_2_wr() //0051D69B
{
    __asm
    {
        call    get_scenario_info2
        mov     ecx, 0051D6E5h
        jmp     ecx
    }
}

char* __stdcall scenario_create_game(char* dest)
{
    char Filename[MAX_PATH];
    char* scenario_name = RGE_Base_Game__scenarioName(*base_game);
    dest[0] = '\0';
    if (loadfilelist_version == VER_EF)
    {
        snprintf(Filename, _countof(Filename), "%s%s.sc2", (*base_game)->prog_info->scenario_dir, scenario_name);
        if (file_exists(Filename))
            snprintf(dest, MAX_PATH, "%s.sc2", scenario_name);
    }
    if (!dest[0])
    {
        snprintf(Filename, _countof(Filename), "%s%s.sc1", (*base_game)->prog_info->scenario_dir, scenario_name);
        if (file_exists(Filename))
            snprintf(dest, MAX_PATH, "%s.sc1", scenario_name);
    }
    if (!dest[0])
    {
        snprintf(Filename, _countof(Filename), "%s%s.scx", (*base_game)->prog_info->scenario_dir, scenario_name);
        if (file_exists(Filename))
            snprintf(dest, MAX_PATH, "%s.scx", scenario_name);
    }
    return dest;
}

__declspec(naked) void scenario_create_game_wr() //005ECBCE
{
    __asm
    {
        lea     eax, [esp + 40Ch]
        push    eax
        call    scenario_create_game
        mov     ecx, 005ECC48h
        jmp     ecx
    }
}

#pragma optimize( "s", on )
void setLoadFileListHooks(int version)
{
    loadfilelist_version = version;

    setHook((void*)0x0053FE51, sed_open_action_1_wr);
    setHook((void*)0x00528E2C, savegamescreen_action_1_wr);
    setHook((void*)0x0053FFE1, sed_open_action_2_wr);
    setHook((void*)0x0053FC50, TRIBE_Screen_Sed_Open__fillList_new);
    setHook((void*)0x00528BF0, TribeSaveGameScreen__fillList_new);
    setHook((void*)0x00540B70, TribeSelectScenarioScreen__fillScenarios_new);
    setHook((void*)0x00517E42, scenario_checksum_1_wr);
    setHook((void*)0x0051DA7A, scenario_checksum_2_wr);
    setHook((void*)0x0051DB25, scenario_checksum_3_wr);
    setHook((void*)0x005219D4, scenario_checksum_4_wr);
    setHook((void*)0x00522143, scenario_checksum_5_wr);
    setHook((void*)0x005EC3D2, scenario_info_1_wr);
    setHook((void*)0x0051D69B, scenario_info_2_wr);
    setHook((void*)0x005ECBCE, scenario_create_game_wr);
}
#pragma optimize( "", on )
