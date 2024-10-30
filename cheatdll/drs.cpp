#include "stdafx.h"
#include "drs.h"

struct drs_file_info
{
    std::string filename;
    bool load_in_cc;
    bool load_in_ef;
    bool load_in_non_wide;
    bool prefix_required;
    bool no_mapping;
};

//for legacy drs load
/*
drs_file_info drs_files[] =   //cc      ef     non-wide prefix  no mapping
{
    {"sounds.drs",              true,   true,   true,   false,  true },
    {"sounds_x1.drs",           true,   true,   true,   false,  true },
    {"sounds_x2.drs",           false,  true,   true,   true,   true },
    {"graphics.drs",            true,   false,  true,   true,   false },
    {"graphics_p1.drs",         false,  true,   true,   true,   false },
    {"graphics_x1.drs",         true,   false,  true,   true,   false },
    {"graphics_x1_p1.drs",      false,  true,   true,   true,   false },
    {"graphics_x2.drs",         false,  true,   true,   true,   false },
    {"terrain.drs",             true,   false,  true,   true,   false },
    {"terrain_p1.drs",          false,  true,   true,   true,   false },
    {"terrain_x1.drs",          true,   false,  true,   true,   false },
    {"terrain_x1_p1.drs",       false,  true,   true,   true,   false },
    {"terrain_x2.drs",          false,  true,   true,   true,   false },
    {"wide.drs",                true,   false,  false,  true,   false },
    {"wide_p1.drs",             false,  true,   false,  true,   false },
    {"interfac_x2.drs",         false,  true,   true,   true,   false },
    {"interfac.drs",            true,   false,  true,   true,   false },
    {"interfac_p1.drs",         false,  true,   true,   true,   false },
    {"interfac_x1.drs",         true,   false,  true,   true,   false },
    {"interfac_x1_p1.drs",      false,  true,   true,   true,   false },
    //{"interfac_x2.drs",         false,  true,   true,   true,   false },
    {"gamedata_x1.drs",         true,   false,  true,   true,   false },
    {"gamedata_x2.drs",         false,  true,   true,   true,   false }
};
*/

//for new drs load
drs_file_info drs_files[] =   //cc      ef     non-wide prefix  no mapping
{
    {"sounds.drs",              true,   true,   true,   false,  true },
    {"sounds_x1.drs",           true,   true,   true,   false,  true },
    {"sounds_x2.drs",           false,  true,   true,   true,   false },
    {"graphics.drs",            true,   false,  true,   true,   false },
    {"graphics_p1.drs",         false,  true,   true,   true,   false },
    {"graphics_x1.drs",         true,   false,  true,   true,   false },
    {"graphics_x1_p1.drs",      false,  true,   true,   true,   false },
    {"graphics_x2.drs",         false,  true,   true,   true,   false },
    {"terrain.drs",             true,   false,  true,   true,   false },
    {"terrain_p1.drs",          false,  true,   true,   true,   false },
    {"terrain_x1.drs",          true,   false,  true,   true,   false },
    {"terrain_x1_p1.drs",       false,  true,   true,   true,   false },
    {"terrain_x2.drs",          false,  true,   true,   true,   false },
    {"interfac.drs",            true,   false,  true,   true,   false },
    {"interfac_p1.drs",         false,  true,   true,   true,   false },
    {"interfac_x1.drs",         true,   false,  true,   true,   false },
    {"interfac_x1_p1.drs",      false,  true,   true,   true,   false },
    {"interfac_x2.drs",         false,  true,   true,   true,   false },
    {"wide.drs",                true,   false,  false,  true,   false },
    {"wide_p1.drs",             false,  true,   false,  true,   false },
    {"gamedata_x1.drs",         true,   false,  true,   true,   false },
    {"gamedata_x2.drs",         false,  true,   true,   true,   false }
};

__declspec(naked) void drsHeaderEnd() //004D4FE8
{
    __asm
    {
        mov     eax, [ecx + 50h]
        add     eax, ecx
        cmp     esi, eax
        jz      drs_outside
        mov     eax, [esp + 10h]
        mov     edx, [esi]
        cmp     edx, eax
        jnz     drs_outside
        mov     edx, 004D5020h
        jmp     edx
drs_outside:
        mov     edx, 004D4FF2h
        jmp     edx
    }
}

bool drs_mode_ef;
bool drs_mode_wide;

bool is_loaded_in_current_mode(drs_file_info& drs_file)
{
    return (!drs_mode_ef && drs_file.load_in_cc || drs_mode_ef && drs_file.load_in_ef) && (drs_mode_wide || drs_file.load_in_non_wide);
}

std::string get_adjusted_name(drs_file_info& drs_file)
{
    if (drs_mode_ef && !drs_file.no_mapping)
        return DATA_FOLDER_PREFIX_FROM_DATA + drs_file.filename;
    else
        return drs_file.filename;
}

void __stdcall setup_drs_files(TRIBE_Game* game)
{
    log("Loading DRS files...");
    int n_drs_files = sizeof(drs_files) / sizeof(drs_files[0]);
    for (int i = 0; i < n_drs_files; i++)
        if (is_loaded_in_current_mode(drs_files[i]))
        {
            char buf[0x100];
            std::string filename = get_adjusted_name(drs_files[i]);
            strcpy_safe(buf, _countof(buf), filename.c_str());
            RESFILE_open_new_resource_file(buf, "swbg", game->prog_info->resource_dir, drs_files[i].no_mapping);
            log("Opened %s", buf);
        }
}

__declspec(naked) void drs_load_hook() //005E4B5A
{
    __asm
    {
        push    ebp
        call    setup_drs_files
        mov     eax, 005E4C46h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setDRSLoadHooks(int ver, bool wide)
{
    drs_mode_ef = ver == VER_EF;
    drs_mode_wide = wide;

    setHook((void*)0x005E4B5A, drs_load_hook);

    //heap corruption
    setHook((void*)0x004D4FE8, drsHeaderEnd);
}
#pragma optimize( "", on )
