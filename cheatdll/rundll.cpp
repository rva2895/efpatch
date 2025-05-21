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

int WINAPI WinMain_dll(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

    SetProcessDPIAware();
    fixCurrentDir();
    initLog();
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
        panel->vfptr == (TPanelVtbl*)0x006607F4 || panel->vfptr == (TPanelVtbl*)0x00662128 || panel->vfptr == (TPanelVtbl*)0x00661F3C)
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
    if (msg == WM_APP + 1000) //updatebk
    {
        update_load_save_game_panel((TPanel*)wParam);
    }
    
    return WndProc_exe(hWnd, msg, wParam, lParam);
}
