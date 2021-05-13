#include "stdafx.h"

/*#include "savegamever.h"
#include "casts.h"  //for read and write save functions

char current_version[8];
char loaded_version[8];

char oldVer[] = "VER 9.4";
char newVer[] = "VER 9.5";

int old_save_file_ver2;

void __stdcall strcpy_wr(char* dst, char* src)
{
    strcpy(dst, src);
}

__declspec(naked) void verLoadHook2() //0061D9A5
{
    __asm
    {
        lea     esi, [esp + 18h]
        mov     ecx, offset oldVer
        push    ecx
        push    esi
        call    strcmp_wr
        test    eax, eax
        jnz     ver_cont
        mov     eax, 1
        mov     old_save_file_ver2, eax
        mov     ecx, 0061D9DBh
        jmp     ecx
ver_cont:
        mov     ecx, offset newVer
        push    ecx
        push    esi
        call    strcmp_wr
        test    eax, eax
        jnz     bad_ver
        //test for balance patch ver now
        sub     esp, 8
        mov     eax, esp
        push    8
        push    eax
        push    edi
        call    readSaveFile
        push    esp
        push    offset loaded_version
        call    strcpy_wr
        push    esp
        mov     eax, offset current_version
        push    eax
        call    strcmp_wr
        add     esp, 8
        test    eax, eax
        jnz     bad_ver
        xor     eax, eax
        mov     old_save_file_ver2, eax

        push    0EBh
        push    0061D9A5h
        call    writeByte
        push    4Ah
        push    0061D9A6h
        call    writeByte
        add     esp, 10h

        push    4
        mov     edx, 69E4D4h
        mov     eax, 0061D99Eh
        jmp     eax
bad_ver:
        xor     eax, eax
        mov     old_save_file_ver2, eax
        mov     eax, 0061D9DBh
        jmp     eax
    }
}

__declspec(naked) void verSaveHook2() //00620583
{
    __asm
    {
        push    8
        push    offset newVer
        push    esi
        call    writeSaveFile
        push    8
        push    offset current_version
        push    esi
        call    writeSaveFile
        mov     ecx, 00620591h
        jmp     ecx
    }
}

#pragma warning(push)
#pragma warning(disable:4100)
__declspec(naked) void __stdcall errorScreen
    (char* s, int unk0, int x, int y, int unk1, int ptr)
{
    __asm
    {
        mov     ecx, [esp + 18h]
        mov     eax, [esp]   //old ret addr
        mov     [esp + 18h], eax //save in unused var
        add     esp, 4
        mov     eax, 4BB250h
        call    eax
        pop     eax    //restore old ret addr
        jmp     eax
    }
}
#pragma warning(pop)

void __stdcall displayVersionError(int ptr)
{
    char buf[0x100];
    if (old_save_file_ver2)
        sprintf(buf, "This game was recorded with a different version of the game\n"
            "Current version: %s, "
            "recorded game version: 1.1",
            current_version);
    else
        sprintf(buf, "This game was recorded with a different version of balance patch.\n"
            "Current version: %s, "
            "recorded game version: %s\n",
            current_version,
            loaded_version);

    errorScreen(buf, 0, 450, 100, 1, ptr);
}

__declspec(naked) void onResult() //0050AD06
{
    __asm
    {
        test    al, al
        jnz     _no_error
        push    ebx
        call    displayVersionError
_no_error:
        mov     ebx, 0050AE72h
        jmp     ebx
    }
}

void setSaveGameVersionHooks()
{
    memset(current_version, 0, 8);
    strcpy(current_version, _BALANCE_PATCH_VER);

    setHook((void*)0x00620583, verSaveHook2);
    setHook((void*)0x0061D997, verLoadHook2);
    //setHook ((void*)0x0061DBBA, &verLoadHook2);
    setHook((void*)0x0050AD06, onResult);
}
*/

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
#define EXE_PATCH_VERSION "(none)"
#endif

void __stdcall displayVersionError(void* ptr)
{
    char buf[0x100];
    char s2[0x100];
    ver[3] = 0;
    if (!strcmp(ver, "9.4"))
    {
        strcpy(ver, "1.1");
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
