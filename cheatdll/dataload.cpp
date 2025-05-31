#include "stdafx.h"
#include "dataload.h"

#include "convert.h"
#include "resgenbldgs.h"
#include "expldroid.h"
#include "jedimaster.h"
#include "airtoair.h"
#include "jediholo.h"
#include "terrain.h"
#include "startupload.h"

extern int current_save_game_version;
int current_loaded_version = CURRENT_VERSION;

char* ground_to_air_path;
char* jedi_holo_path;

bool load_data_always = false;

/*
bool __stdcall on_deny_save_game_popup()
{
    TPanel* panel = panel_system->currentPanelValue;
    if (current_loaded_version != CURRENT_VERSION)
    {
        TEasy_Panel__popupOKDialog((TEasy_Panel*)panel, "Error", NULL, 450, 100, 1);
        return false;
    }
    else
        return true;
}

__declspec(naked) void on_deny_save_game() //0052871F
{
    __asm
    {
        call    on_deny_save_game_popup
        lea     edi, [esi + 7E8h]
        test    al, al
        jnz     display_save_game_screen
        mov     eax, 00528AEAh
        jmp     eax
display_save_game_screen:
        mov     eax, 00528725h
        jmp     eax
    }
}
*/

int load_game_data(TRIBE_Game* game, int version)
{
    char* data_file;
    char* data_prefix;

    switch (version)
    {
    case 0:
        data_file = DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.4.0\\genie_x2.dat";
        data_prefix = DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.4.0\\";
        break;
    case 1:
    case 2:
        data_file = DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.4.1\\genie_x2.dat";
        data_prefix = DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.4.1\\";
        break;
    case 3:
        data_file = DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.4.2\\genie_x2.dat";
        data_prefix = DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.4.2\\";
        break;
    case 4:
        data_file = DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.5.0\\genie_x2.dat";
        data_prefix = DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.5.0\\";
        break;
    case 5:
        data_file = DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.5.1\\genie_x2.dat";
        data_prefix = DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.5.1\\";
        break;
    case 6:
        data_file = DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.5.2\\genie_x2.dat";
        data_prefix = DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.5.2\\";
        break;
    case 7:
        data_file = DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.5.3\\genie_x2.dat";
        data_prefix = DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.5.3\\";
        break;
    case 8:
        data_file = DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.5.4\\genie_x2.dat";
        data_prefix = DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.5.4\\";
        break;
    case 9:
        data_file = DATA_FOLDER_PREFIX_FROM_ROOT"genie_x2.dat";
        data_prefix = DATA_FOLDER_PREFIX_FROM_ROOT;
        break;
#if CURRENT_VERSION != 9
#error Must update for new CURRENT_VERSION
#endif
    default:
        data_file = DATA_FOLDER_PREFIX_FROM_ROOT"genie_x2.dat";
        data_prefix = DATA_FOLDER_PREFIX_FROM_ROOT;
        break;
    }

    log("Loading game data, data_file = %s, data_prefix = %s", data_file, data_prefix);

    //_countof(game->prog_info->game_data_file) is MAX_PATH + 1 but let's be extra safe
    strlcpy(game->prog_info->game_data_file, data_file, MAX_PATH);

    if (game->world)
    {
        game->world->vfptr->__vecDelDtor(game->world, 1);
        game->world = NULL;
    }

    current_loaded_version = version;

    int retval = TRIBE_Game__load_game_data(game);

    //clear legacy jedi-holo and ground-to-air
    memset((void*)0x007B1000, 0, 0x1000);

    snprintf(ground_to_air_path, MAX_PATH, "%sground-to-air.txt", data_prefix);
    snprintf(jedi_holo_path, MAX_PATH, "%sjedi-holo.txt", data_prefix);

    writeDword(0x007B2046, (DWORD)jedi_holo_path);
    writeDword(0x007B21B6, (DWORD)ground_to_air_path);

    initExplDroid(data_prefix, "expl.txt");
    setJediMasterHooks(data_prefix, "master.txt", "padawan.txt");
    initBldgResProdList(data_prefix, "resgen.txt");
    setConvertHooks(data_prefix, "unconv.txt");
    initAirToAir(data_prefix, "air-to-air.txt");
    setJediHoloHooks(data_prefix, "jedi-holo.txt");
    loadTerrainTxt(data_prefix, "terrain.txt");

    char dll_name[MAX_PATH];
    if (!strcmp(data_prefix, DATA_FOLDER_PREFIX_FROM_ROOT))
        snprintf(dll_name, _countof(dll_name), DATA_FOLDER_PREFIX_FROM_ROOT"..\\language_x2.dll");
    else
        snprintf(dll_name, _countof(dll_name), "%slanguage_x2.dll", data_prefix);

    FreeLibrary(*hInstance_dll);
    *hInstance_dll = LoadLibrary(dll_name);

    log("Game data loaded");

    return retval;
}

unsigned __int8 __stdcall check_savegame_version_and_load_world(TRIBE_Game* game, const char* filename, int load_mode)
{
    char full_path[300];
    char dummy_str[0xE48];
    char ver_str[8];
    float version;

    if (load_mode == 5)
    {
        if (RGE_Base_Game__mpSaveLocal(*base_game))
            sprintf(full_path, "%s", filename);
        else
            sprintf(full_path, "%s%s", (*base_game)->prog_info->multisave_dir, filename);
    }
    else
    {
        sprintf(full_path, "%s%s", (*base_game)->prog_info->save_dir, filename);
    }

    int file_handle = rge_open(load_game_or_scen_abs_path_sub(false, full_path, filename), 0x8000);
    if (file_handle == -1)
        return 0;

    int dummy;
    if (load_mode == 3 || load_mode == 4)
    {
        read_internal(file_handle, &dummy, 4);
        read_internal(file_handle, &dummy, 4);
    }
    if (load_mode == 5)
        read_internal(file_handle, dummy_str, 0xE48);

    rge_read(file_handle, ver_str, 8);
    rge_read(file_handle, &version, 4);

    current_save_game_version = -1;
    if (!strncmp(ver_str, "VER 9.8", 8))
    {
        current_save_game_version = 0;
    }
    else if (!strncmp(ver_str, "VER 9.9", 8))
    {
        int sub_version;
        rge_read(file_handle, &sub_version, sizeof(sub_version));
        if (sub_version >= 1)
            current_save_game_version = sub_version;
    }
    rge_close(file_handle);

    if (current_save_game_version == -1)
        return 0;

    if ((current_loaded_version != current_save_game_version) || load_data_always)
    {
        return load_game_data(game, current_save_game_version);
    }
    else
    {
        return TRIBE_Game__load_game_data(game);
    }
}

__declspec(naked) void on_load_game() //005EC672
{
    __asm
    {
        mov     ecx, [esp + 118h]   //filename
        mov     eax, [esp + 11Ch]   //replay game
        neg     eax
        sbb     eax, eax
        and     al, 0FDh
        add     eax, 6
        push    eax
        push    ecx
        push    ebp
        call    check_savegame_version_and_load_world
        test    al, al
        jz      on_load_game_error
        mov     eax, 005EC709h
        jmp     eax

on_load_game_error:
        mov     eax, 005EC6D3h
        jmp     eax
    }
}

int __fastcall TRIBE_Game__load_game_data_default(TRIBE_Game* game)
{
    if ((current_loaded_version != CURRENT_VERSION) || load_data_always)
        return load_game_data(game, CURRENT_VERSION);
    else
        return TRIBE_Game__load_game_data(game);
}

__declspec(naked) void on_start_game() //005EBC1A
{
    __asm
    {
        mov     ecx, ebp
        call    TRIBE_Game__load_game_data_default
        test    eax, eax
        jz      on_start_game_error
        mov     eax, 005EBC83h
        jmp     eax

on_start_game_error:
        mov     eax, 005EBCDCh
        jmp     eax
    }
}

__declspec(naked) void on_restore_game() //005EBE58
{
    __asm
    {
        mov     ecx, [esp + 124h]   //filename
        push    5
        push    ecx
        push    ebp
        call    check_savegame_version_and_load_world
        test    al, al
        jz      on_restore_game_error
        mov     eax, 005EBEBEh
        jmp     eax

on_restore_game_error:
        mov     eax, 005EBF5Dh
        jmp     eax
    }
}

void __cdecl setDataLoadHooks_atexit()
{
    free(ground_to_air_path);
    free(jedi_holo_path);
}

#pragma optimize( "s", on )
void setDataLoadHooks()
{
    setHook((void*)0x005EC672, on_load_game);
    setHook((void*)0x005EBC1A, on_start_game);
    setHook((void*)0x005EBE58, on_restore_game);

    setHookCall((void*)0x0051831E, TRIBE_Game__load_game_data_default);
    setHookCall((void*)0x00538904, TRIBE_Game__load_game_data_default);
    writeNops(0x0051831C, 2);
    writeNops(0x005EBBC8, 6);
    writeNops(0x005388E6, 2);

    //setHook((void*)0x0052871F, on_deny_save_game);

    ground_to_air_path = (char*)malloc(MAX_PATH);
    jedi_holo_path = (char*)malloc(MAX_PATH);

    if (file_exists(DATA_FOLDER_PREFIX_FROM_ROOT"reload_data"))
        load_data_always = true;
    else
        load_data_always = false;

    char* on_or_off = load_data_always ? "ON" : "OFF";
    log("Force data reload is %s", on_or_off);

    efpatch_atexit(setDataLoadHooks_atexit);
}
#pragma optimize( "", on )
