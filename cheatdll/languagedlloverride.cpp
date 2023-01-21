#include "stdafx.h"
#include "languagedlloverride.h"

HINSTANCE our_dll = NULL;

char* __stdcall loadDLLString_new(UINT uID, char* dest, int cchBufferMax, RGE_Base_Game* this_)
{
    if (!our_dll || !MyLoadStringA(our_dll, uID, dest, cchBufferMax))
        if (!*hInstance_dll || !MyLoadStringA(*hInstance_dll, uID, dest, cchBufferMax))
            RGE_Base_Game__get_string3(this_, uID, dest, cchBufferMax);
    dest[cchBufferMax - 1] = 0;
    return dest;
}

__declspec(naked) int loadDllString_wrapper() //005E6A30
{
    __asm
    {
        push    ecx
        mov     eax, [esp + 8] //uID
        mov     ecx, [esp + 0Ch] //dest
        mov     edx, [esp + 10h] //cchBufferMax
        push    edx
        push    ecx
        push    eax
        call    loadDLLString_new
        retn    0Ch
    }
}

void setLanguageDllOverrideHooks()
{
    our_dll = GetModuleHandle(DLL_NAME);
    setHook((void*)0x005E6A30, loadDllString_wrapper);
};
