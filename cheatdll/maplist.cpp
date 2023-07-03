#include "stdafx.h"
#include "maplist.h"
#include <map>

std::map<int, MAP_INFO> map_list;

bool __stdcall check_map_list_presence()
{
    std::string ef_locales = get_string(13599);
    return (ef_locales.length() != 0 && !map_list.empty());
}

__declspec(naked) void on_append_line_1() //0051F82B
{
    __asm
    {
        call    TDropDownPanel__append_line2
        call    check_map_list_presence
        test    al, al
        jz      skip_map_list_append
        mov     ecx, [esi]
        push    3
        push    13599    //EF Locales
        call    TDropDownPanel__append_line2

skip_map_list_append:
        mov     eax, 0051F830h
        jmp     eax
    }
}

void setup_map_types_setup_screen(TribeMPSetupScreen* setup_screen)
{
    TDropDownPanel__set_line_by_id(setup_screen->mapFileTypeDrop, 3);
    TDropDownPanel__empty_list(setup_screen->mapTypeDrop);
    TDropDownPanel__set_sorted(setup_screen->mapTypeDrop, 1);

    for (auto it = map_list.begin(); it != map_list.end(); ++it)
    {
        TDropDownPanel__append_line2(setup_screen->mapTypeDrop, it->second.string_id, it->first);
    }

    if (!TDropDownPanel__set_line_by_id(setup_screen->mapTypeDrop, TRIBE_Game__mapType((TRIBE_Game*)*base_game)))
    {
        TDropDownPanel__set_line_by_id(setup_screen->mapTypeDrop, 101);
        TRIBE_Game__setMapType((TRIBE_Game*)*base_game, 101);
    }
}

void setup_map_types_restore_screen(TribeMPRestoreScreen* restore_screen)
{
    TDropDownPanel__set_line_by_id(restore_screen->mapFileTypeDrop, 3);
    TDropDownPanel__empty_list(restore_screen->mapTypeDrop);
    TDropDownPanel__set_sorted(restore_screen->mapTypeDrop, 1);

    for (auto it = map_list.begin(); it != map_list.end(); ++it)
    {
        TDropDownPanel__append_line2(restore_screen->mapTypeDrop, it->second.string_id, it->first);
    }

    if (!TDropDownPanel__set_line_by_id(restore_screen->mapTypeDrop, restore_screen->mHeaderInfo.gameSettings.mapType))
    {
        TDropDownPanel__set_line_by_id(restore_screen->mapTypeDrop, 101);
        TRIBE_Game__setMapType((TRIBE_Game*)*base_game, 101);
    }
}

bool __stdcall mp_setup_map_types_setup_screen(int map_type, TribeMPSetupScreen* setup_screen)
{
    if (map_type > 100 && map_type <= 300)
    {
        setup_map_types_setup_screen(setup_screen);
        return true;
    }
    else
        return false;
}

bool __stdcall mp_setup_map_types_restore_screen(int map_type, TribeMPRestoreScreen* restore_screen)
{
    if (map_type > 100 && map_type <= 300)
    {
        setup_map_types_restore_screen(restore_screen);
        return true;
    }
    else
        return false;
}

__declspec(naked) void mp_setup_get_settings() //0051FA49
{
    __asm
    {
        push    eax
        push    esi
        push    eax
        call    mp_setup_map_types_setup_screen
        test    al, al
        pop     eax
        jnz     mp_setup_handled

        add     eax, 0FFFFFFDAh
        cmp     eax, 16h
        mov     ecx, 0051FA4Fh
        jmp     ecx

mp_setup_handled:
        mov     eax, 0051FFCFh
        jmp     eax
    }
}

bool __stdcall mp_setup_action2(int list_type, TribeMPSetupScreen* setup_screen)
{
    if (list_type == 3)
    {
        setup_map_types_setup_screen(setup_screen);
        return true;
    }
    else
        return false;
}

__declspec(naked) void mp_setup_action() //005188FB
{
    __asm
    {
        call    TDropDownPanel__get_id
        push    eax
        push    esi
        push    eax
        call    mp_setup_action2
        test    al, al
        pop     eax
        jnz     mp_setup_action_handled

        mov     ecx, 00518900h
        jmp     ecx

mp_setup_action_handled:
        mov     eax, 00518E53h
        jmp     eax
    }
}

bool __stdcall map_list_set_rollover_ids_do(int map_type, int line_in, TTextPanel* text_panel)
{
    auto it = map_list.find(map_type);
    if (it != map_list.end())
    {
        TTextPanel__set_line_rollover_string_id(text_panel, line_in, it->second.rollover_string_id);
        return true;
    }
    else
        return false;
}

__declspec(naked) void map_list_set_rollover_ids() //005230D1
{
    __asm
    {
        call    TTextPanel__get_id2
        push    eax
        push    edi
        push    esi
        push    eax
        call    map_list_set_rollover_ids_do
        test    al, al
        pop     eax
        jnz     map_list_rollover_ids_handled

        mov     ecx, 005230D6h
        jmp     ecx

map_list_rollover_ids_handled:
        mov     eax, 005232B4h
        jmp     eax
    }
}

__declspec(naked) void mp_restore() //0051414C
{
    __asm
    {
        mov     eax, [esi + 1BFCh]
        push    eax
        push    esi
        push    eax
        call    mp_setup_map_types_restore_screen
        test    al, al
        pop     eax
        jnz     mp_restore_handled

        mov     ecx, 00514152h
        jmp     ecx

mp_restore_handled:
        mov     eax, 005146BFh
        jmp     eax
    }
}

int __stdcall map_list_get_string_id(int map_type)
{
    auto it = map_list.find(map_type);
    if (it != map_list.end())
    {
        return it->second.string_id;
    }
    else
        return 0;
}

__declspec(naked) void map_list_update_summary() //005413CC
{
    __asm
    {
        call    TRIBE_Game__mapType
        push    eax
        push    eax
        call    map_list_get_string_id
        test    eax, eax
        pop     ecx
        jnz     map_list_update_summary_handled

        mov     eax, ecx
        add     eax, 0FFFFFFF7h
        cmp     eax, 33h
        mov     ecx, 005413D7h
        jmp     ecx

map_list_update_summary_handled:
        mov     ecx, base_game
        mov     ecx, [ecx]
        push    eax
        mov     edx, [ecx]
        call    dword ptr [edx + 24h]

        mov     ecx, 005417B5h
        jmp     ecx
    }
}

int __stdcall map_list_generate_do(int map_type, char* dest)
{
    auto it = map_list.find(map_type);
    if (it != map_list.end())
    {
        strcpy_safe(dest, 21, it->second.filename);
        return it->second.drs_id;
    }
    else
        return 0;
}

__declspec(naked) void map_list_generate() //005C9EDD
{
    __asm
    {
        call    TRIBE_Game__mapType
        lea     edx, [esp + 18h]
        push    eax
        push    edx
        push    eax
        call    map_list_generate_do
        test    eax, eax
        pop     ecx
        jnz     map_list_generate_handled

        mov     eax, ecx
        add     eax, 0FFFFFFF7h
        cmp     eax, 34h
        mov     ecx, 005C9EE8h
        jmp     ecx

map_list_generate_handled:
        mov     ebp, eax

        mov     ecx, 005CA7BFh
        jmp     ecx
    }
}

__declspec(naked) void map_list_new_game() //00600AE6
{
    __asm
    {
        call    TRIBE_Game__mapType
        push    eax
        push    eax
        call    map_list_get_string_id
        test    eax, eax
        pop     ecx
        jnz     map_list_new_game_handled

        mov     eax, ecx
        add     eax, 0FFFFFFF7h
        cmp     eax, 34h
        mov     ecx, 00600AF1h
        jmp     ecx

map_list_new_game_handled:
        mov     ecx, base_game
        mov     ecx, [ecx]
        push    eax
        mov     edx, [ecx]
        call    dword ptr [edx + 24h]

        mov     ecx, 00600EF9h
        jmp     ecx
    }
}

__declspec(naked) void map_list_load_mp_save() //005133F3
{
    __asm
    {
        mov     ecx, [esp + 1724h]
        lea     eax, [ecx - 9]
        push    eax
        push    ecx
        call    map_list_get_string_id
        test    eax, eax
        pop     ecx
        jnz     map_list_load_mp_save_handled

        mov     eax, ecx
        mov     ecx, 005133FDh
        jmp     ecx

map_list_load_mp_save_handled:
        push    eax
        mov     eax, 00513506h
        jmp     eax
    }
}

void initMapList(const char* prefix, const char* filename)
{
    int id;
    int string_id;
    int rollover_string_id;
    int drs_id;
    char script_filename[21];
    log("Loading map list");

    if (!map_list.empty())
        map_list.clear();

    char full_filename[0x100];
    snprintf(full_filename, _countof(full_filename), "%s%s", prefix, filename);
    FILE* f = fopen(full_filename, "rt");
    if (f)
    {
        while (fscanf_s(f, "%d,%d,%d,%d,%s", &id, &string_id, &rollover_string_id, &drs_id, script_filename, _countof(script_filename)) > 0)
        {
            if (id > 100 && id <= 300)
            {
                MAP_INFO mi;
                mi.string_id = string_id;
                mi.rollover_string_id = rollover_string_id;
                mi.drs_id = drs_id;
                strcpy_safe(mi.filename, sizeof(mi.filename), script_filename);
                map_list.insert(std::pair<int, MAP_INFO>(id, mi));
            }
        }

        fclose(f);
    }
    else
        log("Warning: %s not found, extra map list disabled", full_filename);
}

#pragma optimize( "s", on )
void setMapListHooks()
{
    setHook((void*)0x0051F82B, on_append_line_1);
    setHook((void*)0x0051FA49, mp_setup_get_settings);
    setHook((void*)0x005188FB, mp_setup_action);
    setHook((void*)0x005230D1, map_list_set_rollover_ids);
    setHook((void*)0x0051414C, mp_restore);
    setHook((void*)0x005413CC, map_list_update_summary);
    setHook((void*)0x005C9EDD, map_list_generate);
    setHook((void*)0x00600AE6, map_list_new_game);
    setHook((void*)0x005133F3, map_list_load_mp_save);

    initMapList(DATA_FOLDER_PREFIX_FROM_ROOT, "maps.txt");
}
#pragma optimize( "", on )
