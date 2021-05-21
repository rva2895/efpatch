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

extern int placementSettings;
extern int cliff_type;

extern bool editorstatus_isValid;

extern void* scen_ptr;

extern CONFIG_DATA cd;

#ifndef TARGET_VOOBLY

#include <CrashRpt.h>

extern crash_rpt::CrashRpt* g_crashRpt;

int (WINAPI* WinMain_exe) (HINSTANCE, HINSTANCE, LPSTR, int) =
    (int (WINAPI*) (HINSTANCE, HINSTANCE, LPSTR, INT)) 0x0048EFC0;

extern "C" __declspec(dllexport) int WINAPI WinMain_dll(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

    log("WinMain_dll called");

#ifndef _CHEATDLL_CC
    installPalette();
#endif

    initialSetup();

    writeDword(0x00426509, (DWORD)&WndProc_dll);

    int retval;

#ifdef _CHEATDLL_CC
    cd.crashReporting = 0;
#endif

    if (cd.crashReporting)
    {
        log("Crash reporting is ON");
#ifndef _DEBUG
        initCrashReporter();

        __try
        {
#endif
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
        retval = WinMain_exe(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    }

    log("WinMain_exe returned %d, exiting", retval);
    return retval;
}
#endif

int (CALLBACK* WndProc_exe) (HWND, UINT, WPARAM, LPARAM) =
    (int (CALLBACK*) (HWND, UINT, WPARAM, LPARAM)) 0x00426530;

HWND hWnd_main = 0;

#ifdef TARGET_VOOBLY
int cliff_types[] = { 0x108, 0 };
#else
int cliff_types[] = { 0x108, 3971, 3981, 3991, 4196, 4206, 4216, 4226, 4236, 0};
#endif
int* cliff_types_ptr = &cliff_types[0];

extern bool isEditor;

extern DWORD window_editorbk;

__declspec(naked) void __stdcall update_editor_bk()
{
    __asm
    {
        mov     ecx, window_editorbk
        mov     eax, [ecx + 18h]
        mov     edx, [ecx + 14h]
        push    eax
        push    edx
        mov     eax, [ecx]
        call    dword ptr [eax + 5Ch]

        mov     ecx, window_editorbk
        push    1
        mov     eax, [ecx]
        call    dword ptr [eax + 2Ch]
        ret
    }
}

extern bool rec_cache_invalid;

#pragma warning(push)
#pragma warning(disable:4100)
__declspec(naked) void __stdcall update_window(void* wnd)
{
    __asm
    {
        mov     ecx, [esp + 4]
        mov     eax, [ecx + 18h]
        mov     edx, [ecx + 14h]
        //push    eax
        //push    edx
        //mov     eax, [ecx]
        //call    dword ptr[eax + 5Ch]

        mov     ecx, [esp + 4]
        mov     eax, [ecx]
        test    eax, eax
        jz      _skip_update_window
        push    1
        call    dword ptr [eax + 2Ch]
_skip_update_window:
        ret     4
    }
}
#pragma warning(pop)

HWND hWnd_global;

int terrain_paint_mode = 0;    //default
#ifdef _DEBUG
bool time_collect = false;
bool time_stage_find = false;
extern volatile bool answer;
extern volatile bool answer_flag;
#endif // _DEBUG

int CALLBACK WndProc_dll(HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    hWnd_global = hWnd;

    if (msg == WM_KEYDOWN)
    {
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
            
#ifndef _CHEATDLL_CC
#ifndef TARGET_VOOBLY
            if (LOWORD(wParam) == 'Q')                        //cliff type
            {
                if (GetKeyState(VK_CONTROL) & 0x8000)
                {
                    cliff_types_ptr++;
                    if (!*cliff_types_ptr)
                        cliff_types_ptr = &cliff_types[0];
                    cliff_type = *cliff_types_ptr;

                    setCliffType(cliff_type, scen_ptr);

                    editorstatus_isValid = false;
                }
            }
#endif
#endif
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
        if ((LOWORD(wParam) >= '1') && (LOWORD(wParam) <= '9')) //rec switch player
        {
            //if (short x = GetKeyState(VK_MENU))
            //{
            recSwitch(LOWORD(wParam) - 0x30);
            //}
        }
        /*if (LOWORD(wParam) == VK_F8)                        //rec overlay
        {
            overlay_switch();
        }*/
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
        if (!rec_cache_invalid)
            update_window((void*)wParam);
    }

    if (!hWnd_main)
        hWnd_main = hWnd;

    return WndProc_exe(hWnd, msg, wParam, lParam);
}
