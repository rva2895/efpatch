#include "stdafx.h"
#include "buildingconv.h"

__declspec(naked) void buildingconv_first_in_stack() //00568156
{
    __asm
    {
        jnz     buildingconv_first_in_stack_set_melee_range
        
        fld     dword ptr [ebx + 70h]
        fmul    dword ptr [esi + 44h]
        fstp    dword ptr [esi + 44h]
        jmp     buildingconv_first_in_stack_continue

buildingconv_first_in_stack_set_melee_range:
        mov     dword ptr [esi + 44h], 3F000000h
        
buildingconv_first_in_stack_continue:
        mov     eax, 0056815Fh
        jmp     eax
    }
}

__declspec(naked) void buildingconv_action_type_if_ordered() //0040912D
{
    __asm
    {
        jnz     buildingconv_action_type_if_ordered_set_melee_range

        fld     dword ptr [eax + 70h]
        fmul    dword ptr [edx]
        fstp    dword ptr [edx]
        jmp     buildingconv_action_type_if_ordered_continue

buildingconv_action_type_if_ordered_set_melee_range:
        mov     dword ptr [edx], 3F000000h

buildingconv_action_type_if_ordered_continue:
        mov     eax, 00409149h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setBuildingConvHooks()
{
    setHook((void*)0x00568156, buildingconv_first_in_stack);
    setHook((void*)0x0040912D, buildingconv_action_type_if_ordered);
}
#pragma optimize( "", on )
