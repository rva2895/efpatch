#include "stdafx.h"

#include "wndmode.h"

int isTechTree = 0;

__declspec(naked) void onTechTreeCreate() //00462352
{
    __asm
    {
        xor     eax, eax
        inc     eax
        mov     isTechTree, eax
        lea     eax, [esi + 1C4Ch]
        mov     ecx, 00462358h
        jmp     ecx
    }
}

__declspec(naked) void onTechTreeDestroy() //00463F68
{
    __asm
    {
        xor     eax, eax
        mov     isTechTree, eax
        mov     eax, esi
        pop     esi
        ret     4
    }
}

char* cmdline = 0;
bool normalmouse = false;

void __stdcall fixCur(HWND hWnd, LPRECT rect)
{
    if (!cmdline)
    {
        cmdline = GetCommandLine();
        if (strstr(cmdline, "NORMALMOUSE"))
            normalmouse = true;
    }
    GetClientRect(hWnd, rect);
    if (!normalmouse)
        if (!isTechTree)
            SetCursor(0);
}

__declspec(naked) void wndtmp() //00616C0F
{
    __asm
    {
        push    00616C15h
        jmp     fixCur
    }
}

int __stdcall new_is_mode_avail(int formal1, int formal2, int formal3)
{
    UNREFERENCED_PARAMETER(formal1);
    UNREFERENCED_PARAMETER(formal2);
    UNREFERENCED_PARAMETER(formal3);
    return true;
}

#pragma optimize( "s", on )
void setWndModeHooks()
{
    if (LoadLibrary("wndmode.dll"))
        log("Loaded wndmode.dll");
    else
        log("No wndmode.dll. Using ddraw window mode if possible");

    setHook((void*)0x00616C0F, wndtmp);

    writeNops(0x0047166E, 0x14);

    //tech tree mouse
    setHook((void*)0x00462352, onTechTreeCreate);
    setHook((void*)0x00463F68, onTechTreeDestroy);

    //display mode enum fix
    //writeWord(0x004267C9, 0xE990);
    setHook((void*)0x00471070, new_is_mode_avail);
}
#pragma optimize( "", on )
