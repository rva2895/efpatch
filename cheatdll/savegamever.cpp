#include "stdafx.h"

char ver[4];

#ifndef TARGET_VOOBLY
#define EXE_PATCH_VERSION "N/A"
#endif

void __stdcall displayVersionError(TEasy_Panel* ptr)
{
    char buf[0x100];
    char s2[0x100];
    ver[3] = 0;
    if (!strcmp(ver, "9.4"))
    {
        strlcpy(ver, EXE_PATCH_VERSION, _countof(ver));
        strlcpy(s2, "\nTurn Data Patch off to watch this game", _countof(s2));
    }
    else if (!strcmp(ver, "2.2"))
        strlcpy(s2, "\nTurn Data Patch on to watch this game", _countof(s2));
    else
        strlcpy(s2, "", _countof(s2));

    if (strcmp(ver, EXE_PATCH_VERSION))
        snprintf(buf, _countof(buf), "This file was recorded with a different version of the game\n"
            "Current version: " EXE_PATCH_VERSION ", "
            "File version: %s%s",
            ver, s2);
    else
        snprintf(buf, _countof(buf), "Cannot load that saved game");

    TEasy_Panel__popupOKDialog(ptr, buf, NULL, 450, 100, 1);
}

__declspec(naked) void onResult() //0050AD06
{
    __asm
    {
        test    eax, eax
        jnz     _no_error
        push    ebx
        call    displayVersionError
_no_error:
        mov     ebx, 0050AE72h
        jmp     ebx
    }
}

__declspec(naked) void onGetSaveGameVersion() //0061D9A5
{
    __asm
    {
        mov     esi, 0069E564h
        lea     eax, [esp + 1Ch]
        mov     edx, [eax]
        mov     ecx, offset ver
        mov     [ecx], edx
        sub     eax, 4
        mov     edx, 0061D9AEh
        jmp     edx
    }
}

#pragma optimize( "s", on )
void setSaveGameVerHooks(bool datap)
{
    UNREFERENCED_PARAMETER(datap);
    //if (datap)
    //    writeData(0x0069E568, EXE_PATCH_VERSION, strlen(EXE_PATCH_VERSION) + 1);
    setHook((void*)0x0050AD06, onResult);
    setHook((void*)0x0061D9A5, onGetSaveGameVersion);
}
#pragma optimize( "", on )
