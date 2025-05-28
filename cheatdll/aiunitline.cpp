#include "stdafx.h"
#include "aiunitline.h"

__declspec(naked) void on_ai_build() //00575C7C
{
    __asm
    {
        call    TribeInformationAIModule__getFirstAvailableInLine
        cmp     eax, -1
        jz      ai_build_bad_unitline
        mov     esi, eax
        mov     eax, 00575C83h
        jmp     eax

ai_build_bad_unitline:
        pop     esi
        ret
    }
}

__declspec(naked) void on_ai_build_forward() //00575D6C
{
    __asm
    {
        call    TribeInformationAIModule__getFirstAvailableInLine
        cmp     eax, -1
        jz      ai_build_forward_bad_unitline
        mov     esi, eax
        mov     eax, 00575D73h
        jmp     eax

ai_build_forward_bad_unitline:
        pop     esi
        ret
    }
}

__declspec(naked) void on_ai_build_wall() //00575ED0
{
    __asm
    {
        call    TribeInformationAIModule__getFirstAvailableInLine
        cmp     eax, -1
        jz      ai_build_wall_bad_unitline
        mov     esi, eax
        mov     eax, 00575ED7h
        jmp     eax

ai_build_wall_bad_unitline:
        pop     edi
        pop     esi
        ret
    }
}

__declspec(naked) void on_ai_train() //005773FF
{
    __asm
    {
        call    TribeInformationAIModule__getFirstAvailableInLine
        cmp     eax, -1
        jz      ai_train_bad_unitline
        mov     esi, eax
        mov     eax, 00577406h
        jmp     eax

ai_train_bad_unitline:
        pop     edi
        pop     esi
        ret
    }
}

__declspec(naked) void on_static_object_setup() //0048C1F4
{
    __asm
    {
        mov     ax, [edi + 10h] //unit_line
        mov     [esi + 10h], ax
        mov     al, [edi + 12h] //min_tech_level
        mov     [esi + 12h], al

        lea     ecx, [esi + 8]
        mov     dword ptr [ecx], 0
        mov     eax, 0048C1FDh
        jmp     eax
    }
}

__declspec(naked) void on_static_object_setup4() //0048D13B
{
    __asm
    {
        mov     ax, [edi + 10h] //unit_line
        mov     [esi + 10h], ax
        mov     al, [edi + 12h] //min_tech_level
        mov     [esi + 12h], al

        lea     ecx, [esi + 8]
        mov     dword ptr [ecx], 0
        mov     eax, 0048D144h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setAIUnitlineHooks()
{
    setHook((void*)0x00575C7C, on_ai_build);
    setHook((void*)0x00575D6C, on_ai_build_forward);
    setHook((void*)0x00575ED0, on_ai_build_wall);
    setHook((void*)0x005773FF, on_ai_train);

    setHook((void*)0x0048C1F4, on_static_object_setup);
    setHook((void*)0x0048D13B, on_static_object_setup4);
}
#pragma optimize( "", on )
