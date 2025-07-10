#include "stdafx.h"
#include "loadfilelist.h"

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

struct fillList_callback_param
{
    TTextPanel* list;
    int n;
    bool exclude;
};

void __cdecl fillList_callback(const char* filename, void* param)
{
    fillList_callback_param* p = (fillList_callback_param*)param;
    size_t len = strlen(filename) + 1;
    char Filename[MAX_PATH];
    strncpy(Filename, filename, len - 5);
    Filename[len - 5] = '\0';
    if (!p->exclude || strcmp(Filename, "gencomb") && strncmp(Filename, "default", 7u))
        TTextPanel__append_line(p->list, Filename, p->n, NULL);
}

void __fastcall TRIBE_Screen_Sed_Open__fillList_new(TRIBE_Screen_Sed_Open* sed_open) //0053FC50
{
    auto sed_open_fill_list = [](TRIBE_Screen_Sed_Open* sed_open, const char* s, int n)
        {
            char Filename[MAX_PATH];
            snprintf(Filename, _countof(Filename), s, (*base_game)->prog_info->scenario_dir);
            fillList_callback_param param;
            param.list = (TTextPanel*)sed_open->list;
            param.n = n;
            param.exclude = false;
            findfirst_callback(Filename, fillList_callback, &param);
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
            fillList_callback_param param;
            param.list = (TTextPanel*)savegamescreen->list;
            param.n = n;
            param.exclude = false;
            findfirst_callback(Filename, fillList_callback, &param);
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
            fillList_callback_param param;
            param.list = (TTextPanel*)sel_scen->scenarioList;
            param.n = n;
            param.exclude = true;
            findfirst_callback(Filename, fillList_callback, &param);
        };

    TTextPanel__empty_list((TTextPanel*)sel_scen->scenarioList);
    sel_scen->scenarioList->sorted = 1;

    if (loadfilelist_version == VER_EF)
        sel_scen_fill_list(sel_scen, "%s*.sc2", 2);

    sel_scen_fill_list(sel_scen, "%s*.sc1", 1);
    sel_scen_fill_list(sel_scen, "%s*.scx", 0);
}

void __fastcall TribeLoadSavedGameScreen__fillList_new(TribeLoadSavedGameScreen* loadsavedgamescreen) //0050A880
{
    auto loadsavedgamescreen_fill_list = [](TribeLoadSavedGameScreen* loadsavedgamescreen, const char* s, const char* dir, int n)
        {
            char Filename[MAX_PATH];
            snprintf(Filename, _countof(Filename), s, dir);
            fillList_callback_param param;
            param.list = (TTextPanel*)loadsavedgamescreen->list;
            param.n = n;
            param.exclude = false;
            findfirst_callback(Filename, fillList_callback, &param);
        };

    TTextPanel__empty_list((TTextPanel*)loadsavedgamescreen->list);
    loadsavedgamescreen->list->sorted = 1;

    if (loadfilelist_version == VER_EF)
    {
        loadsavedgamescreen_fill_list(loadsavedgamescreen, "%s*.ga2", (*base_game)->prog_info->save_dir, 0);
        loadsavedgamescreen_fill_list(loadsavedgamescreen, "%s*.mg2", (*base_game)->prog_info->save_dir, 1);
    }
    else
    {
        loadsavedgamescreen_fill_list(loadsavedgamescreen, "%s*.ga1", (*base_game)->prog_info->save_dir, 0);
        loadsavedgamescreen_fill_list(loadsavedgamescreen, "%s*.mg1", (*base_game)->prog_info->save_dir, 1);
    }
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

void __stdcall set_autosave_name(RGE_Game_World* world)
{
    strlcpy(world->auto_save_name, "autosave.ga1", 100);
    if (loadfilelist_version == VER_EF)
        world->auto_save_name[11] = '2';
}

__declspec(naked) void autosave_load_game() //0061DA0D
{
    __asm
    {
        push    ebp
        call    set_autosave_name
        mov     ecx, 0061DA9Dh
        jmp     ecx
    }
}

__declspec(naked) void autosave_new_game() //0061E4A1
{
    __asm
    {
        push    ebp
        call    set_autosave_name
        mov     ecx, 0061E531h
        jmp     ecx
    }
}

void __stdcall loadsavedgamescreen_action(TribeLoadSavedGameScreen* loadsavedgamescreen, int n, const char* name)
{
    const char* ext;
    char Filename[MAX_PATH];
    if (n == 1)
        ext = loadfilelist_version == VER_EF ? ".mg2" : ".mg1";
    else
        ext = loadfilelist_version == VER_EF ? ".ga2" : ".ga1";
    snprintf(Filename, _countof(Filename), "%s%s%s", (*base_game)->prog_info->save_dir, name, ext);
    if (file_exists(Filename))
        snprintf(Filename, _countof(Filename), "%s%s", name, ext);
    if (!TRIBE_Game__load_game((TRIBE_Game*)*base_game, Filename, n == 1 ? 3 : 0))
        TEasy_Panel__popupOKDialog2((TEasy_Panel*)loadsavedgamescreen, 9473, 0, 450, 100, 1);
}

__declspec(naked) void loadsavedgamescreen_action_wr() //0050AC8F
{
    __asm
    {
        lea     ecx, [esp + 114h]
        push    ecx
        push    eax
        push    ebx
        call    loadsavedgamescreen_action
        mov     eax, 0050AD91h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setLoadFileListHooks(int version)
{
    loadfilelist_version = version;

    setHook((void*)0x0053FE51, sed_open_action_1_wr);
    setHook((void*)0x00528E2C, savegamescreen_action_1_wr);
    setHook((void*)0x0053FFE1, sed_open_action_2_wr);
    setHook((void*)0x0050AC8F, loadsavedgamescreen_action_wr);
    setHook((void*)0x0053FC50, TRIBE_Screen_Sed_Open__fillList_new);
    setHook((void*)0x00528BF0, TribeSaveGameScreen__fillList_new);
    setHook((void*)0x0050A880, TribeLoadSavedGameScreen__fillList_new);
    setHook((void*)0x00540B70, TribeSelectScenarioScreen__fillScenarios_new);
    setHook((void*)0x00517E42, scenario_checksum_1_wr);
    setHook((void*)0x0051DA7A, scenario_checksum_2_wr);
    setHook((void*)0x0051DB25, scenario_checksum_3_wr);
    setHook((void*)0x005219D4, scenario_checksum_4_wr);
    setHook((void*)0x00522143, scenario_checksum_5_wr);
    setHook((void*)0x005EC3D2, scenario_info_1_wr);
    setHook((void*)0x0051D69B, scenario_info_2_wr);
    setHook((void*)0x005ECBCE, scenario_create_game_wr);

    //remove autosave count
    setHook((void*)0x0061DA0D, autosave_load_game);
    setHook((void*)0x0061E4A1, autosave_new_game);
}
#pragma optimize( "", on )
