#include "stdafx.h"
#include <stdio.h>

#pragma warning(push)
#pragma warning(disable:4100)
__declspec(naked) void __stdcall errorScreen
(char* s, int unk0, int x, int y, int unk1, void* ptr)
{
    __asm
    {
        mov     ecx, [esp + 18h]
        mov     eax, [esp]          //old ret addr
        mov     [esp + 18h], eax    //save in unused var
        add     esp, 4
        mov     eax, 4BB250h
        call    eax
        pop     eax                 //restore old ret addr
        jmp     eax
    }
}
#pragma warning(pop)

char ver[4];

#ifndef TARGET_VOOBLY
#define EXE_PATCH_VERSION "N/A"
#endif

void __stdcall displayVersionError(void* ptr)
{
    char buf[0x100];
    char s2[0x100];
    ver[3] = 0;
    if (!strcmp(ver, "9.4"))
    {
        strcpy(ver, EXE_PATCH_VERSION);
        strcpy(s2, "\nTurn Data Patch off to watch this game");
    }
    else if (!strcmp(ver, "2.2"))
        strcpy(s2, "\nTurn Data Patch on to watch this game");
    else
        strcpy(s2, "");

    if (strcmp(ver, EXE_PATCH_VERSION))
        sprintf(buf, "This file was recorded with a different version of the game\n"
            "Current version: " EXE_PATCH_VERSION ", "
            "File version: %s%s",
            ver, s2);
    else
        sprintf(buf, "Cannot load that saved game");

    errorScreen(buf, 0, 450, 100, 1, ptr);
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

void setSaveGameVerHooks(bool datap)
{
    if (datap)
        writeData(0x0069E568, EXE_PATCH_VERSION, strlen(EXE_PATCH_VERSION) + 1);
    setHook((void*)0x0050AD06, onResult);
    setHook((void*)0x0061D9A5, onGetSaveGameVersion);
}
