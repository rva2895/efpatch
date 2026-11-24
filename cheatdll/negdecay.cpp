#include "stdafx.h"
#include "negdecay.h"

__declspec(naked) int negDecay() //0054F123
{
    __asm
    {
        jnz     jump
        fld     dword ptr [esi + 54h]
        fcomp   dword ptr [ecx + 80h]
        fnstsw  ax
        test    ah, 41h
        jnz     jumpgood
        mov     eax, [ecx + 80h]
        mov     [esi + 54h], eax
jumpgood:
        mov     ecx, 0054F1B6h
        jmp     ecx
jump:
        mov     ecx, 0054F129h
        jmp     ecx
    }
}

#pragma optimize( "s", on )
void setDecayHooks()
{
    // retired 2026-11-24 (owner-approved bake-in): the update replacement
    // carries the patched semantics - the attribute_rot test-mask rewrite
    // (the // NEGDECAY marker, X87_NE_ORD) and the negDecay amount clamp
    // (the // NEGDECAY marker) - see src/engine/object/RGE_Static_Object.cpp;
    // after hooking RGE_Static_Object::update (0x0054EF00) these writes
    // would patch the replacement's own code
    //writeByte(0x54F0F6, 0x40);
    //setHook((void*)0x0054F123, negDecay);
}
#pragma optimize( "", on )
