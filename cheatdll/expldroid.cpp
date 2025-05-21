#include "stdafx.h"
#include "expldroid.h"

__int16* expl_list = NULL;
int expl_n = 0;
bool expl_installed = false;

bool __fastcall is_not_expl(__int16 id)
{
    return !is_id_in_list(id, expl_list, expl_n);
}

__declspec(naked) void expl1() //0040215F
{
    __asm
    {
        mov     cx, [eax + 18h]
        call    is_not_expl
        test    al, al
        mov     eax, 00402165h
        jmp     eax
    }
}

__declspec(naked) void expl2() //0055A881
{
    __asm
    {
        mov     cx, [ecx + 18h]
        call    is_not_expl
        test    al, al
        mov     eax, 0055A887h
        jmp     eax
    }
}

__declspec(naked) void expl3() //0055BE40
{
    __asm
    {
        push    ecx
        push    edx
        mov     cx, [ebp + 18h]
        call    is_not_expl
        test    al, al
        pop     edx
        pop     ecx
        mov     eax, 0055BE46h
        jmp     eax
    }
}

__declspec(naked) void expl4() //005B6DF2
{
    __asm
    {
        mov     cx, [eax + 18h]
        call    is_not_expl
        test    al, al
        mov     eax, 005B6DF8h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void __cdecl expl_droid_hooks()
{
    setHook((void*)0x0040215F, &expl1);
    setHook((void*)0x0055A881, &expl2);
    setHook((void*)0x0055BE40, &expl3);
    setHook((void*)0x005B6DF2, &expl4);
}
#pragma optimize( "", on )

void initExplDroid(const char* prefix, const char* filename)
{
    load_ids_from_txt(
        prefix,
        filename,
        &expl_list,
        &expl_n,
        &expl_installed,
        expl_droid_hooks,
        "Loading suicide attack unit list"
    );
}
