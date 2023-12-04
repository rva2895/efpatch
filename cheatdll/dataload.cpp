#include "stdafx.h"
#include "dataload.h"

#include "convert.h"
#include "resgenbldgs.h"
#include "expldroid.h"
#include "jedimaster.h"
#include "airtoair.h"
#include "terrain.h"

extern int current_save_game_version;
char* data_file_name;
char* data_prefix;
int current_loaded_version = CURRENT_VERSION;

int* const current_handle = (int*)0x0077EF8C;
int* const compression_buffers = (int*)0x0077EF98;
int* const file_buffers = (int*)0x0077F0EC;
int* const current = (int*)0x0077EFA4;
int* const buffers = (int*)0x0077EF9C;
int* const point = (int*)0x0077EF94;
int* const flags = (int*)0x0077EF90;
int* const file_size = (int*)0x0077F0E8;
int* const rge_fio_bl = (int*)0x0077EFA0;
int* const rge_unknown = (int*)0x0077ED88;

int save_game__handle;
int save_game__compression_buffers;
int save_game__file_buffers;
int save_game__current;
int save_game__buffers;
int save_game__point;
int save_game__flags;
int save_game__file_size;
int save_game__rge_fio_bl;
int save_game__rge_unknown;

void setup_data_file_name(const char* data_file)
{
    strcpy_safe(data_file_name, MAX_PATH, data_file);
}

__declspec(naked) void onDataFileName() //0048F0FB
{
    __asm
    {
        mov     [esp + 1Ch], esi
        mov     data_file_name, esi
        mov     esi, edi
        mov     edi, 0048F101h
        jmp     edi
    }
}

__declspec(naked) void __stdcall unload_game_data() //
{
    __asm
    {
        push    esi
        mov     esi, base_game
        mov     esi, [esi]
        mov     ecx, [esi + 420h]
        test    ecx, ecx
        jz      no_data_unload
        mov     edx, [ecx]
        push    1
        call    dword ptr [edx + 0B8h]
no_data_unload:
        mov     dword ptr [esi + 420h], 0
        pop     esi
        retn
    }
}

char* ground_to_air_path;
char* jedi_holo_path;

void do_setup_dat_file(int use_logo_background, int show_loading_game)
{
    log("Reloading game data ...");
    //(*(void**)((uint8_t*)*base_game + 0x420)) = NULL;
    TRIBE_Game__show_status_message(*(TRIBE_Game**)base_game, "Loading game information...", NULL, 0, 0, use_logo_background);

    save_game__handle = *current_handle;
    save_game__compression_buffers = *compression_buffers;
    save_game__file_buffers = *file_buffers;
    save_game__current = *current;
    save_game__buffers = *buffers;
    save_game__point = *point;
    save_game__flags = *flags;
    save_game__file_size = *file_size;
    save_game__rge_fio_bl = *rge_fio_bl;
    save_game__rge_unknown = *rge_unknown;

    unload_game_data();
    log("Unloaded game data");
    TRIBE_Game__load_game_data(*(TRIBE_Game**)base_game);
    log("Loaded game data");
    log("Reloading txt files ...");

    *current_handle = save_game__handle;
    *compression_buffers = save_game__compression_buffers;
    *file_buffers = save_game__file_buffers;
    *current = save_game__current;
    *buffers = save_game__buffers;
    *point = save_game__point;
    *flags = save_game__flags;
    *file_size = save_game__file_size;
    *rge_fio_bl = save_game__rge_fio_bl;
    *rge_unknown = save_game__rge_unknown;

    //clear legacy jedi-holo and ground-to-air
    memset((void*)0x007B1000, 0, 0x1000);

    snprintf(ground_to_air_path, 0x100, "%sground-to-air.txt", data_prefix);
    snprintf(jedi_holo_path, 0x100, "%sjedi-holo.txt", data_prefix);

    writeDword(0x007B2046, (DWORD)jedi_holo_path);
    writeDword(0x007B21B6, (DWORD)ground_to_air_path);

    initExplDroid(data_prefix, "expl.txt");
    setJediMasterHooks(data_prefix, "master.txt", "padawan.txt");
    initBldgResProdList(data_prefix, "resgen.txt");
    setConvertHooks(data_prefix, "unconv.txt");
    initAirToAir(data_prefix, "air-to-air.txt");
    loadTerrainTxt(data_prefix, "terrain.txt");

    char dll_name[0x100];
    if (!strcmp(data_prefix, DATA_FOLDER_PREFIX_FROM_ROOT))
        snprintf(dll_name, _countof(dll_name), DATA_FOLDER_PREFIX_FROM_ROOT"..\\language_x2.dll");
    else
        snprintf(dll_name, _countof(dll_name), "%slanguage_x2.dll", data_prefix);

    FreeLibrary(*hInstance_dll);
    *hInstance_dll = LoadLibrary(dll_name);

    log("Reloaded txt files");

    if (show_loading_game)
        TRIBE_Game__show_status_message(*(TRIBE_Game**)base_game, "Loading game ...", NULL, 0, 0, use_logo_background);
    //TRIBE_Game__close_status_message(*base_game);
}

bool load_data_always = false;

void __stdcall setup_default_dat_file(int use_logo_background, int show_loading_game)
{
    if ((current_loaded_version != CURRENT_VERSION) || load_data_always)
    {
        setup_data_file_name(DATA_FOLDER_PREFIX_FROM_ROOT"genie_x2.dat");
        current_loaded_version = CURRENT_VERSION;
        data_prefix = DATA_FOLDER_PREFIX_FROM_ROOT;
        do_setup_dat_file(use_logo_background, show_loading_game);
    }
}

bool load_second_attempt = false;

bool setup_dat_file()
{
    switch (current_save_game_version)
    {
    case 0:
        setup_data_file_name(DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.4.0\\genie_x2.dat");
        data_prefix = DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.4.0\\";
        break;
    case 1:
    case 2:
        setup_data_file_name(DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.4.1\\genie_x2.dat");
        data_prefix = DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.4.1\\";
        break;
    case 3:
        setup_data_file_name(DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.4.2\\genie_x2.dat");
        data_prefix = DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.4.2\\";
        break;
    case 4:
        setup_data_file_name(DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.5.0\\genie_x2.dat");
        data_prefix = DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.5.0\\";
        break;
    case 5:
        setup_data_file_name(DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.5.1\\genie_x2.dat");
        data_prefix = DATA_FOLDER_PREFIX_FROM_ROOT"old\\1.5.1\\";
        break;
    case 6:
        setup_data_file_name(DATA_FOLDER_PREFIX_FROM_ROOT"genie_x2.dat");
        data_prefix = DATA_FOLDER_PREFIX_FROM_ROOT;
        break;
#if CURRENT_VERSION != 6
#error Must update for new CURRENT_VERSION
#endif
    default:
        setup_data_file_name(DATA_FOLDER_PREFIX_FROM_ROOT"genie_x2.dat");
        data_prefix = DATA_FOLDER_PREFIX_FROM_ROOT;
        break;
    }
    if ((current_loaded_version != current_save_game_version) || load_data_always)
    {
        current_loaded_version = current_save_game_version;
        do_setup_dat_file(1, 1);
        return !load_second_attempt;
    }
    else
        return false;
}

int load_arg_1;
int load_arg_2;

__declspec(naked) void on_call_load_new_game() //005EC79E
{
    __asm
    {
        mov     eax, [esp]
        mov     load_arg_1, eax
        mov     eax, [esp + 4]
        mov     load_arg_2, eax
        xor     eax, eax
        mov     load_second_attempt, al
        call    dword ptr [edx + 0E8h]
        and     eax, 0FFh
        test    eax, eax
        jnz     continue_load
        inc     eax
        mov     load_second_attempt, al
        mov     eax, load_arg_2
        push    eax
        mov     eax, load_arg_1
        push    eax
        mov     ecx, [ebp + 420h]
        mov     edx, [ecx]
        
        call    dword ptr [edx + 0E8h]
        xor     ecx, ecx
        mov     load_second_attempt, cl

continue_load:
        mov     ecx, 005EC7A4h
        jmp     ecx
    }
}

__declspec(naked) void on_start_game() //005EBAD4
{
    __asm
    {
        sub     esp, 200h
        push    eax
        push    ecx
        push    0
        push    1
        call    setup_default_dat_file
        pop     ecx
        pop     eax
        mov     edx, 005EBADAh
        jmp     edx
    }
}

__declspec(naked) void on_open_sed() //00529734
{
    __asm
    {
        push    0
        push    0
        call    setup_default_dat_file
        mov     ecx, esi
        mov     eax, 005388C0h
        call    eax
        mov     ecx, 0052973Bh
        jmp     ecx
    }
}

__declspec(naked) void on_tech_tree_button() //00518316
{
    __asm
    {
        push    0
        push    1
        call    setup_default_dat_file
        mov     ecx, base_game
        mov     ecx, [ecx]
        mov     eax, 0051831Eh
        jmp     eax
    }
}

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

bool file_exists(const char* filename)
{
    HANDLE hFile = CreateFile(
        filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
        return true;
    }
    else
        return false;
}

#pragma optimize( "s", on )
void setDataLoadHooks()
{
    setHook((void*)0x0048F0FB, onDataFileName);
    //setHook((void*)0x0061D020, on_del_game_info);
    setHook((void*)0x005EC79E, on_call_load_new_game);
    setHook((void*)0x005EBAD4, on_start_game);
    setHook((void*)0x00529734, on_open_sed);
    setHook((void*)0x00518316, on_tech_tree_button);

    //setHook((void*)0x0052871F, on_deny_save_game);

    ground_to_air_path = (char*)malloc(0x100);
    jedi_holo_path = (char*)malloc(0x100);

    if (file_exists(DATA_FOLDER_PREFIX_FROM_ROOT"reload_data"))
        load_data_always = true;
    else
        load_data_always = false;

    char* on_or_off = load_data_always ? "ON" : "OFF";
    log("Force data reload is %s", on_or_off);

    if (load_data_always)
    {
        writeData(0x005ED41A, "\xE8\x21\xEF\xE3\xFF", 5);
        log("Autoreplay forced off when force data reload is on");
    }
}
#pragma optimize( "", on )
