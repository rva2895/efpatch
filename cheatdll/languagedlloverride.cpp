#include "stdafx.h"
#include "languagedlloverride.h"

HINSTANCE our_dll = NULL;

char* __fastcall TRIBE_Game__get_string3_new(TRIBE_Game* game, DWORD dummy, int string_id, char* str, int max_len)
{
    UNREFERENCED_PARAMETER(dummy);

    if (!our_dll || !MyLoadStringA(our_dll, string_id, str, max_len))
        if (!*hInstance_dll || !MyLoadStringA(*hInstance_dll, string_id, str, max_len))
            RGE_Base_Game__get_string3((RGE_Base_Game*)game, string_id, str, max_len);
    str[max_len - 1] = 0;
    return str;
}

void setLanguageDllOverrideHooks()
{
    our_dll = GetModuleHandle(DLL_NAME);
    setHook((void*)0x005E6A30, TRIBE_Game__get_string3_new);
};
