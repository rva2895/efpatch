#include "stdafx.h"

#include "terrain.h"

int t_ver;

char** terrain_names = NULL;

int* terrain_language_dll = NULL;
extern BYTE* terrain_array;
int terrains_loaded = 0;

void __stdcall terrain1_sub(TDropDownPanel* dropdown)
{
    if (t_ver != VER_EF)
    {
        TDropDownPanel__append_line2(dropdown, 0x297D, 0);
        TDropDownPanel__append_line2(dropdown, 0x2980, 1);
        TDropDownPanel__append_line2(dropdown, 10926, 2);
        TDropDownPanel__append_line2(dropdown, 0x2993, 3);
        TDropDownPanel__append_line2(dropdown, 0x2983, 4);
        TDropDownPanel__append_line2(dropdown, 0x299A, 5);
        TDropDownPanel__append_line2(dropdown, 10622, 6);
        TDropDownPanel__append_line2(dropdown, 10920, 7);
        TDropDownPanel__append_line2(dropdown, 10921, 8);
        TDropDownPanel__append_line2(dropdown, 0x2991, 9);
        TDropDownPanel__append_line2(dropdown, 0x297F, 10);
        TDropDownPanel__append_line2(dropdown, 0x2992, 11);
        TDropDownPanel__append_line2(dropdown, 0x2990, 12);
        TDropDownPanel__append_line2(dropdown, 0x2981, 13);
        TDropDownPanel__append_line2(dropdown, 0x2998, 14);
        TDropDownPanel__append_line2(dropdown, 0x2A2D, 15);
        TDropDownPanel__append_line2(dropdown, 0x2A2E, 16);
        TDropDownPanel__append_line2(dropdown, 0x29A8, 17);
        TDropDownPanel__append_line2(dropdown, 0x2999, 18);
        TDropDownPanel__append_line2(dropdown, 0x2984, 19);
        TDropDownPanel__append_line2(dropdown, 0x2982, 20);
        TDropDownPanel__append_line2(dropdown, 0x29A9, 21);
        TDropDownPanel__append_line2(dropdown, 0x2985, 22);
        TDropDownPanel__append_line2(dropdown, 0x2994, 23);
        TDropDownPanel__append_line2(dropdown, 0x2995, 24);
        TDropDownPanel__append_line2(dropdown, 0x2996, 25);
        TDropDownPanel__append_line2(dropdown, 0x299B, 26);
        TDropDownPanel__append_line2(dropdown, 10927, 27);
        TDropDownPanel__append_line2(dropdown, 10929, 28);
        TDropDownPanel__append_line2(dropdown, 10922, 29);
        TDropDownPanel__append_line2(dropdown, 10925, 30);
        TDropDownPanel__append_line2(dropdown, 10924, 31);
        TDropDownPanel__append_line2(dropdown, 0x29AA, 32);
        TDropDownPanel__append_line2(dropdown, 10668, 33);
        TDropDownPanel__append_line2(dropdown, 0x29AB, 34);
        TDropDownPanel__append_line2(dropdown, 0x29AD, 35);
        TDropDownPanel__append_line2(dropdown, 10928, 36);
        TDropDownPanel__append_line2(dropdown, 10930, 37);
        TDropDownPanel__append_line2(dropdown, 0x29B5, 38);
        TDropDownPanel__append_line2(dropdown, 0x29B6, 39);
        TDropDownPanel__append_line2(dropdown, 10931, 40);
        TDropDownPanel__append_line2(dropdown, 10790, 41);
        TDropDownPanel__append_line2(dropdown, 0x2A27, 42);
        TDropDownPanel__append_line2(dropdown, 0x2A28, 43);
        TDropDownPanel__append_line2(dropdown, 0x2A29, 44);
        TDropDownPanel__append_line2(dropdown, 0x2A2A, 45);
        TDropDownPanel__append_line2(dropdown, 0x2A2B, 46);
        TDropDownPanel__append_line2(dropdown, 0x2A2C, 47);
        TDropDownPanel__append_line2(dropdown, 10923, 48);
        TDropDownPanel__append_line2(dropdown, 0x2A2F, 49);
        TDropDownPanel__append_line2(dropdown, 0x2A30, 50);
        TDropDownPanel__append_line2(dropdown, 0x29B7, 51);
        TDropDownPanel__append_line2(dropdown, 0x29B8, 52);
        //TDropDownPanel__append_line2(dropdown, 10931, 53);
        //TDropDownPanel__append_line2(dropdown, 10931, 54);
        //TDropDownPanel__append_line2(dropdown, 1, 55);
        //TDropDownPanel__append_line2(dropdown, 2, 56);
    }
    else
    {
        for (int i = 0; i < terrains_loaded; i++)
            if (terrain_array[i])
                TDropDownPanel__append_line2(dropdown, terrain_language_dll[i], i);
    }
}

__declspec(naked) int terrain1()
{
    __asm
    {
        mov     ecx, [edi]
        push    ecx
        call    terrain1_sub
        mov     ecx, 0052A388h
        jmp     ecx
    }
}

void (__thiscall* terrain2_load) (void* this_, int language_dll_id, int terrain) =
    (void (__thiscall*) (void*, int, int))0x0053AB20;

void __stdcall terrain2_sub(void* this_)
{
    if (t_ver != VER_EF)
    {
        terrain2_load(this_, 10926, 2);
        terrain2_load(this_, 0x2993, 3);
        terrain2_load(this_, 0x299A, 5);
        terrain2_load(this_, 10920, 7);
        terrain2_load(this_, 10921, 8);
        terrain2_load(this_, 0x2992, 0xB);
        terrain2_load(this_, 0x2998, 0xE);
        terrain2_load(this_, 0x2A2D, 0xF);
        terrain2_load(this_, 0x2A2E, 0x10);
        terrain2_load(this_, 0x29A8, 0x11);
        terrain2_load(this_, 0x2999, 0x12);
        terrain2_load(this_, 0x29A9, 0x15);
        terrain2_load(this_, 0x2996, 0x19);
        terrain2_load(this_, 0x299B, 0x1A);
        terrain2_load(this_, 10927, 0x1B);
        terrain2_load(this_, 10929, 0x1C);
        terrain2_load(this_, 10922, 0x1D);
        terrain2_load(this_, 10925, 0x1E);
        terrain2_load(this_, 10924, 0x1F);
        terrain2_load(this_, 0x29AA, 0x20);
        terrain2_load(this_, 10668, 0x21);
        terrain2_load(this_, 0x29AB, 0x22);
        terrain2_load(this_, 0x29AD, 0x23);
        terrain2_load(this_, 10928, 0x24);
        terrain2_load(this_, 10930, 0x25);
        terrain2_load(this_, 0x29B5, 0x26);
        terrain2_load(this_, 0x29B6, 0x27);
        terrain2_load(this_, 10931, 0x28);
        terrain2_load(this_, 10790, 0x29);
        terrain2_load(this_, 0x2A27, 0x2A);
        terrain2_load(this_, 0x2A28, 0x2B);
        terrain2_load(this_, 0x2A29, 0x2C);
        terrain2_load(this_, 0x2A2A, 0x2D);
        terrain2_load(this_, 0x2A2B, 0x2E);
        terrain2_load(this_, 0x2A2C, 0x2F);
        terrain2_load(this_, 10923, 0x30);
        terrain2_load(this_, 0x2A2F, 0x31);
        terrain2_load(this_, 0x2A30, 0x32);
        terrain2_load(this_, 0x29B7, 0x33);
        terrain2_load(this_, 0x29B8, 0x34);
    }
    else
    {
        for (int i = 0; i < terrains_loaded; i++)
            switch (i)
            {
            case 0:
            case 1:
            case 4:
            case 6:
            case 9:
            case 10:
            case 12:
            case 13:
            case 19:
            case 20:
            case 22:
            case 23:
            case 24:
                break;
            default:
                if (terrain_array[i])
                    terrain2_load(this_, terrain_language_dll[i], i);
                break;
            }
    }
}

__declspec(naked) int terrain2()
{
    __asm
    {
        mov     ecx, ebx
        push    ecx
        call    terrain2_sub
        mov     ecx, 0053B53Bh
        jmp     ecx
    }
}

void loadTerrainTxt(const char* prefix, const char* filename)
{
    log("Loading terrain data...");
    char full_filename[0x100];
    snprintf(full_filename, _countof(full_filename), "%s%s", prefix, filename);
    FILE* f = fopen(full_filename, "rt");
    if (f)
    {
        if (terrain_names)
        {
            for (int i = 0; i < terrains_loaded; i++)
                free(terrain_names[i]);
            free(terrain_names);
            terrain_names = NULL;
        }

        terrains_loaded = 0;
        memset(terrain_array, 0, TERRAIN_COUNT);
        for (; fscanf(f, "%hhu,%d", &terrain_array[terrains_loaded], &terrain_language_dll[terrains_loaded]) > 0; terrains_loaded++)
            ;
        log("Loaded %d terrains", terrains_loaded);
        fclose(f);

        HMODULE lang = LoadLibrary("language.dll");
        HMODULE lang_x2 = LoadLibrary("language_x2.dll");

        terrain_names = (char**)malloc(terrains_loaded*sizeof(char**));
        for (int i = 0; i < terrains_loaded; i++)
        {
            char buf[0x100];
            int n = language_dll_load(terrain_language_dll[i], buf, 0xFF);
            if (n > 0)
            {
                terrain_names[i] = (char*)malloc(n + 1);
                strcpy_safe(terrain_names[i], n + 1, buf);
            }
            else
            {
                terrain_names[i] = (char*)malloc(10);
                strcpy_safe(terrain_names[i], 10, "FORBIDDEN");
            }
#ifdef _DEBUG
            log("Terrain %d - %s", i, terrain_names[i]);
#endif
        }

        FreeLibrary(lang);
        FreeLibrary(lang_x2);

        log("Terrain data loaded");
    }
    else
    {
        log("Error: %s not found, aborting", full_filename);
        MessageBox(0, "Error: terrain.txt was not found. Check installation integrity.", "Error", MB_ICONERROR);
        exit(0);
    }
}

void setTerrainLoadHooks(int ver)
{
    terrain_array = (BYTE*)malloc(TERRAIN_COUNT * sizeof(BYTE));
    terrain_language_dll = (int*)malloc(TERRAIN_COUNT * sizeof(int));

    t_ver = ver;
    if (ver == VER_EF)
        loadTerrainTxt(DATA_FOLDER_PREFIX_FROM_ROOT, "terrain.txt");

    setHook((void*)0x0052A30A, terrain1);
    setHook((void*)0x0053B3C1, terrain2);
}
