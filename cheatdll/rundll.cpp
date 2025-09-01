#include "stdafx.h"
#include "rundll.h"
#include "autosave.h"
#include "editorstatus.h"
#include "cliff.h"
#include "terrain.h"
#include "crashreporter.h"
#include "registry.h"
#include "palette.h"
#include "rec.h"
#include "overlay.h"
#include "memory.h"
#include "textrender.h"
//#include "worlddump.h"

extern int placementSettings;
extern int cliff_type;

extern bool editorstatus_isValid;

extern CONFIG_DATA cd;

const int cliff_types_ef[] = { 0x108, 3971, 3981, 3991, 4196, 4206, 4216, 4226, 4236, 0 };
int current_cliff_index = 0;

#ifndef TARGET_VOOBLY

#include <CrashRpt.h>

void fixCurrentDir()
{
    char fname[256];
    GetModuleFileName(0, fname, 255);
    char* p = fname + strlen(fname);
    while (*--p != '\\');
    *p = 0;
    SetCurrentDirectory(fname);
}

extern crash_rpt::CrashRpt* g_crashRpt;

/*
void* old_stack;
DWORD old_esi;
DWORD old_edi;

__declspec(naked) int __stdcall winmain_wr(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    __asm
    {
        mov     old_esi, esi
        mov     old_edi, edi

        push    0x100000
        call    malloc
        add     esp, 4

        mov     edi, [esp + 4]
        mov     ecx, [esp + 8]
        mov     edx, [esp + 0Ch]
        mov     esi, [esp + 10h]

        mov     old_stack, esp
        add     eax, 0xFFFFC
        mov     esp, eax

        push    esi
        push    edx
        push    ecx
        push    edi
        call    WinMain_exe

        mov     esp, old_stack
        mov     esi, old_esi
        mov     edi, old_edi
        ret
    }
}
*/

int WINAPI WinMain_dll(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

    fixCurrentDir();
    initLog();
    trySetProcessDPIAware();
    installPalette();
    initialSetup();

    writeDword(0x00426509, (DWORD)WndProc_dll);

    int retval;

    if (cd.textRendering)
        setTextRenderHooks(cd.gameVersion);

    new_allocator_install();

    FlushInstructionCache(GetCurrentProcess(), NULL, 0);

    if (cd.largeMaps)
    {
        log("Large maps enabled, crash reporting forced OFF");
        cd.crashReporting = 0;
    }

    if (cd.crashReporting)
    {
        log("Crash reporting is ON");
#ifndef _DEBUG
        initCrashReporter();

        __try
        {
#endif
            log("Calling WinMain_exe");
            retval = WinMain_exe(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
#ifndef _DEBUG
        }
        __except (g_crashRpt->SendReport(GetExceptionInformation()))
        {
            ExitProcess(0);
        }
#endif
    }
    else
    {
        log("Crash reporting is OFF");
        log("Calling WinMain_exe");
        retval = WinMain_exe(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
        //retval = winmain_wr(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    }

    new_allocator_uninstall();

    log("WinMain_exe returned %d, exiting", retval);

    closeLog();

    return retval;
}
#endif

HWND hWnd_main = NULL;

extern bool isEditor;

extern TPanel* window_editorbk;

void update_editor_screen(TPanel* panel)
{
    if (panel && panel->vfptr == (TPanelVtbl*)0x00661C50)
    {
        panel->vfptr->handle_size(panel, panel->pnl_wid, panel->pnl_hgt);
        panel->vfptr->set_redraw(panel, 1);
    }
}

void __stdcall update_editor_bk()
{
    update_editor_screen(window_editorbk);
}

void update_load_save_game_panel(TPanel* panel)
{
    if (panel_system->currentPanelValue == panel && panel &&
        (panel->vfptr == (TPanelVtbl*)0x006607F4 || panel->vfptr == (TPanelVtbl*)0x00662128 || panel->vfptr == (TPanelVtbl*)0x00661F3C))
        panel->vfptr->set_redraw(panel, 1);
}

//int terrain_paint_mode = 0;    //default
#ifdef _DEBUG
//bool time_collect = false;
//bool time_stage_find = false;
//extern volatile bool answer;
//extern volatile bool answer_flag;
#endif // _DEBUG

extern bool world_dump_enabled;

/*
extern int oos_player_no;
extern std::string rms_error_1;
*/

int CALLBACK WndProc_dll(HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    if (!hWnd_main)
        hWnd_main = hWnd;

    if (msg == WM_KEYDOWN)
    {
        /*
#ifdef _DEBUG
        //timeGetTime debug
        if (LOWORD(wParam) == 'Y')                        //YES
        {
            answer = true;
            answer_flag = true;
        }
        if (LOWORD(wParam) == 'N')                        //NO
        {
            answer = false;
            answer_flag = true;
        }
        if (LOWORD(wParam) == 'A')                        //START
        {
            time_collect = true;
        }
        if (LOWORD(wParam) == 'Z')                        //STOP
        {
            time_collect = false;
            time_stage_find = true;
        }
        //
#endif
*/
        /*
        //TEMPORARY DUMP TOGGLE
        if (LOWORD(wParam) == VK_F7)
        {
            chat(world_dump_enabled ? "World dump turned OFF" : "World dump turned ON");
            world_dump_enabled = !world_dump_enabled;
        }
        */
        /*
        if (LOWORD(wParam) == VK_F8)                        //rec overlay
        {
            overlay_switch();
        }
        */
        if (isEditor)
        {
            if (LOWORD(wParam) == 'S')                        //grid - collision
            {
                if (GetKeyState(VK_CONTROL) & 0x8000)
                {
                    placementSettings++;
                    if (placementSettings > 3)
                        placementSettings = 0;
                    editorstatus_isValid = false;
                }
            }
            /*if (LOWORD(wParam) == 'L')                        //terrain transition
            {
                if (short x = GetKeyState(VK_CONTROL))
                {
                    terrain_paint_mode = !terrain_paint_mode;
                    terrain_transition_change(terrain_paint_mode);
                    editorstatus_isValid = false;
                }
            }*/
            
            if (LOWORD(wParam) == 'Q')                        //cliff type
            {
                if (GetKeyState(VK_CONTROL) & 0x8000)
                {
                    if (cd.gameVersion == VER_EF)
                    {
                        cliff_type = cliff_types_ef[++current_cliff_index];
                        if (!cliff_type)
                        {
                            current_cliff_index = 0;
                            cliff_type = cliff_types_ef[0];
                        }
                        setCliffType(cliff_type, (*base_game)->world->map);
                        editorstatus_isValid = false;
                    }
                }
            }

            /*
            auto generate_map_and_get_cs = [](TRIBE_Map* map, int size, int size_id, char* script, int map_id, int player_num)
                {
                    RGE_Player_Info pi;
                    memset(&pi, 0, sizeof(pi));
                    TRIBE_Game__setMapSize((TRIBE_Game*)*base_game, size_id);
                    TRIBE_Game__setMapType((TRIBE_Game*)*base_game, map_id);
                    map->vfptr->map_generate(map, size, size, script, &pi, player_num);
                    WORLD_DUMP wd;
                    wd.update_cs();
                    return wd.get_cs();
                };

            auto test_map = [&generate_map_and_get_cs](TRIBE_Map* map, char* script, int map_id)
                {
                    for (int i = 0; i < 30; i++)
                    {
                        (*base_game)->random_map_seed = 200 + i;
                        unsigned int cs_prev = 0;
                        for (int k = 0; k < 9; k++)
                        {
                            unsigned int cs = generate_map_and_get_cs(map, 220, 4, script, map_id, 9);
                            if (k != 0 && cs != cs_prev)
                                return false;
                            oos_player_no++;
                            cs_prev = cs;
                        }
                    }
                    return true;
                };

            auto do_rms_oos_test = [&generate_map_and_get_cs, &test_map]()
                {
                    TRIBE_Map* map = (TRIBE_Map*)(*base_game)->world->map;
                    TRIBE_Game* game = (TRIBE_Game*)*base_game;

                    TRIBE_Screen_Sed* screen_sed = (TRIBE_Screen_Sed*)TPanelSystem__getTop(panel_system);
                    if (screen_sed->vfptr != (TRIBE_Screen_SedVtbl*)0x00661C50)
                        return;

                    TRIBE_Main_View* main_view = screen_sed->main_view;
                    
                    RGE_Diamond_Map* map_view = (RGE_Diamond_Map*)screen_sed->map_view;
                    RGE_Player_Info pi;
                    memset(&pi, 0, sizeof(pi));

                    bool is_error = false;
                    int error_map_id = -1;

                    for (int k = 113; k < 114; k++)
                    {
                        if (!test_map(map, "56072_rms_ef_swamps_of_naboo", -1))
                        {
                            is_error = true;
                            error_map_id = k;
                            std::string str = "Map " + std::to_string(k) + " error\n";
                            OutputDebugString(str.c_str());
                            break;
                        }
                        else
                        {
                            std::string str = "Map " + std::to_string(k) + " ok\n";
                            OutputDebugString(str.c_str());
                        }
                    }

                    for (int j = 0; j < 8; ++j)
                    {
                        RGE_Tile_List__del_list(&(*base_game)->world->players[j]->tile_list);
                        RGE_Player__set_map_visible((RGE_Player*)(*base_game)->world->players[j]);
                    }
                    RGE_View__set_world((RGE_View*)main_view, (RGE_Game_World*)game->world);
                    RGE_Player* player = RGE_Base_Game__get_player(*base_game);
                    RGE_View__set_player((RGE_View*)main_view, player);
                    RGE_Diamond_Map__set_world(map_view, (RGE_Game_World*)game->world);
                    RGE_Diamond_Map__set_player(map_view, player);

                    if (is_error)
                        rms_error_1 = "Error map " + std::to_string(error_map_id);
                    else
                        rms_error_1 = "No error";
                };

            if (LOWORD(wParam) == 'L')                        //test
            {
                if (short x = GetKeyState(VK_CONTROL))
                {
                    do_rms_oos_test();
                    //editorstatus_isValid = false;
                }
            }
            */

            if (!editorstatus_isValid)
            {
                if (window_editorbk)
                    update_editor_bk();

                editorstatus_isValid = true;
            }
        }
        else
        {
            
        }
    }
    if (msg == WM_SYSKEYDOWN)
    {
        if ((LOWORD(wParam) >= '0') && (LOWORD(wParam) <= '9')) //rec switch player
        {
            if (GetKeyState(VK_MENU) & 0x8000)
            {
                recSwitch(LOWORD(wParam) - 0x30);
            }
        }
    }
    if (msg == WM_TIMER)
    {
        if (wParam == AUTOSAVE_TIMER)
        {
            editor_autosave();
            editorstatus_isValid = false;
        }
    }
    if (msg == WM_APP_ITEMCACHE_UPDATE_BK) //updatebk
    {
        update_load_save_game_panel((TPanel*)wParam);
    }
    
    return WndProc_exe(hWnd, msg, wParam, lParam);
}
