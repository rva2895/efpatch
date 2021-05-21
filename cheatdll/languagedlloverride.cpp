#include "stdafx.h"
#include "languagedlloverride.h"

HINSTANCE* hInstance_dll = (HINSTANCE*)0x0078F81C;
HINSTANCE our_dll = NULL;

int (__stdcall* sub_487760)(HINSTANCE hInstance, UINT uID, char* dest, int cchBufferMax) =
    (int (__stdcall*) (HINSTANCE, UINT, char*, int))0x00487760;

int (__thiscall* sub_4258D0) (void* this_, UINT uID, char* dest, int cchBufferMax) =
    (int (__thiscall*)(void*, UINT uID, char*, int cchBufferMax))0x004258D0;

char* __stdcall loadDLLString_new(UINT uID, char* dest, int cchBufferMax, void* this_)
{
    if (!our_dll || !sub_487760(our_dll, uID, dest, cchBufferMax))
        if (!*hInstance_dll || !sub_487760(*hInstance_dll, uID, dest, cchBufferMax))
            sub_4258D0(this_, uID, dest, cchBufferMax);
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
#ifdef TARGET_VOOBLY
    our_dll = GetModuleHandle("userpatch.dll");
#else
    our_dll = GetModuleHandle("efpatch.dll");
#endif
    setHook((void*)0x005E6A30, loadDllString_wrapper);
};
