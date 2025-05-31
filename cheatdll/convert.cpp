#include "stdafx.h"
#include "convert.h"

__int16* unconv_list = NULL;
int unconv_n = 0;
bool unconv_installed = false;

bool __fastcall is_unconv(__int16 id)
{
    return is_id_in_list(id, unconv_list, unconv_n);
}

__declspec(naked) void unConvertHook() //00567EDC
{
    __asm
    {
        push    ecx
        push    edx
        mov     ecx, ebx
        call    is_unconv
        test    al, al
        pop     edx
        pop     ecx
        jz      unconv_can_convert

        mov     eax, 0056823Ch
        jmp     eax

unconv_can_convert:
        mov     eax, 00567EF1h
        jmp     eax
    }
}

void __cdecl convert_hooks_atexit()
{
    free(unconv_list);
}

void __cdecl convert_hooks()
{
    setHook((void*)0x00567EDC, unConvertHook);

    efpatch_atexit(convert_hooks_atexit);
}

void setConvertHooks(const char* prefix, const char* filename)
{
    load_ids_from_txt(
        prefix,
        filename,
        &unconv_list,
        &unconv_n,
        &unconv_installed,
        convert_hooks,
        "Loading unconvertable unit list"
    );
}
