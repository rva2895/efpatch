#include "stdafx.h"
#include "core.h"

__declspec(naked) void core1() //00606920
{
    __asm
    {
        mov     eax, [esp + 4]
        fld     [esp + 8]
        fmul    dword ptr [ecx]
        fld     [esp + 8]
        fmul    dword ptr [ecx + 4]
        fld     [esp + 8]
        fmul    dword ptr [ecx + 8]
        fxch    st(2)
        fstp    dword ptr [eax]
        fstp    dword ptr [eax + 4]
        fstp    dword ptr [eax + 8]
        ret     8
    }
}

__declspec(naked) void core2() //006069C0
{
    __asm
    {
        mov     eax, [esp + 4]
        fld     dword ptr [ecx]
        fdiv    [esp + 8]
        fld     dword ptr [ecx + 4]
        fdiv    [esp + 8]
        fld     dword ptr [ecx + 8]
        fdiv    [esp + 8]
        fxch    st(2)
        fstp    dword ptr [eax]
        fstp    dword ptr [eax + 4]
        fstp    dword ptr [eax + 8]
        ret     8
    }
}

__declspec(naked) void core3() //00606A10
{
    __asm
    {
        mov     edx, [esp + 8]
        mov     eax, [esp + 4]
        fld     dword ptr [edx]
        fadd    dword ptr [ecx]
        fld     dword ptr [edx + 4]
        fadd    dword ptr [ecx + 4]
        fld     dword ptr [edx + 8]
        fadd    dword ptr [ecx + 8]
        fxch    st(2)
        fstp    dword ptr [eax]
        fstp    dword ptr [eax + 4]
        fstp    dword ptr [eax + 8]
        ret     8
    }
}

__declspec(naked) void core4() //00606A60, operator minus
{
    __asm
    {
        mov     edx, [esp+8]
        mov     eax, [esp+4]
        fld     dword ptr [ecx]
        fsub    dword ptr [edx]
        fld     dword ptr [ecx+4]
        fsub    dword ptr [edx+4]
        fld     dword ptr [ecx+8]
        fsub    dword ptr [edx+8]
        fxch    st(2)
        fstp    dword ptr [eax]
        fstp    dword ptr [eax+4]
        fstp    dword ptr [eax+8]
        ret     8
    }
}


__declspec(naked) void core5() //00606AB0
{
    __asm
    {
        mov     ebx, [ecx]
        mov     edx, [ecx+4]
        mov     ecx, [ecx+8]
        mov     eax, [esp+4]
        xor     ebx, 80000000h
        xor     edx, 80000000h
        xor     ecx, 80000000h
        mov     [eax], ebx
        mov     [eax+4], edx
        mov     [eax+8], ecx
        retn    4
    }
}

__declspec(naked) void core6() //00606B00
{
    __asm
    {
        mov     eax, [esp+4]
        mov     edx, [esp+8]
        fld     dword ptr [ecx+4]
        fmul    dword ptr [edx+8]
        fld     dword ptr [edx+4]
        fmul    dword ptr [ecx+8]
        fsubp   st(1), st
        fstp    dword ptr [eax]
        fld     dword ptr [edx]
        fmul    dword ptr [ecx+8]
        fld     dword ptr [ecx]
        fmul    dword ptr [edx+8]
        fsubp   st(1), st
        fstp    dword ptr [eax+4]
        fld     dword ptr [edx+4]
        fmul    dword ptr [ecx]
        fld     dword ptr [ecx+4]
        fmul    dword ptr [edx]
        fsubp   st(1), st
        fstp    dword ptr [eax+8]
        retn    8
    }
}

__declspec(naked) void core7() //00606970
{
    __asm
    {
        mov     eax, ecx
        mov     ecx, edx
        fld     [esp+4]
        fmul    dword ptr [ecx]
        fld     [esp+4]
        fmul    dword ptr [ecx+4]
        fld     [esp+4]
        fmul    dword ptr [ecx+8]
        fxch    st(2)
        fstp    dword ptr [eax]
        fstp    dword ptr [eax+4]
        fstp    dword ptr [eax+8]
        retn    4
    }
}

//

/*
__declspec(naked) int __stdcall ftol_new_sse() //00632BAC
{
    __asm
    {
        sub     esp, 4
        fstp    dword ptr [esp]
        movss   xmm0, dword ptr [esp]
        cvttss2si eax, xmm0
        cdq
        add     esp, 4
        ret
    }
}

__declspec(naked) int __stdcall ftol_new_sse_fixed(double val)
{
    __asm
    {
        cvttsd2si   eax, mmword ptr [esp + 4]
        cdq
        retn    8
    }
}

__declspec(naked) int __stdcall ftol_new_sse_fixed_2() //00632BAC
{
    __asm
    {
        sub     esp, 8
        fstp    qword ptr [esp]
        cvttsd2si   eax, mmword ptr [esp]
        cdq
        add     esp, 8
        retn
    }
}
*/

/*
const unsigned char ftol_new_sse_packed[] =
{ 0x83, 0xEC, 0x04, 0xD9, 0x1C, 0x24, 0xF3, 0x0F, 0x10, 0x04, 0x24, 0xF3, 0x0F, 0x2C, 0xC0, 0x99, 0x83, 0xC4, 0x04, 0xC3 };
const unsigned char ftol_new_sse_fixed_2_packed[] =
{ 0x83, 0xEC, 0x08, 0xDD, 0x1C, 0x24, 0xF2, 0x0F, 0x2C, 0x04, 0x24, 0x99, 0x83, 0xC4, 0x08, 0xC3 };
*/
/*
const unsigned char ftol_new_up[] =
{ 0x83, 0xEC, 0x08, 0xDD, 0x0C, 0x24, 0x58, 0x5A, 0xC3, 0x90 };
*/
//member function wrappers to make compiler behave

/*
__declspec(naked) float __stdcall unit_distance_to_position_new_wr(void* p, float x_to, float y_to, float z_to)
{
    __asm
    {
        mov     eax, [esp]
        mov     ecx, [esp + 4]
        add     esp, 4
        mov     [esp], eax
        jmp     UNIT::unit_distance_to_position_new
    }
}

__declspec(naked) int __stdcall unit_unknown_moving_update_end_wr(void* p)
{
    __asm
    {
        mov     eax, [esp]
        mov     ecx, [esp + 4]
        add     esp, 4
        mov     [esp], eax
        jmp     UNIT::unit_unknown_moving_update_end
    }
}
*/

#pragma optimize( "s", on )
void setCoreHooks()
{
    setHook((void*)0x00606920, core1);
    setHook((void*)0x006069C0, core2);
    setHook((void*)0x00606A10, core3);
    setHook((void*)0x00606A60, core4);
    setHook((void*)0x00606AB0, core5);
    setHook((void*)0x00606B00, core6);
    setHook((void*)0x00606970, core7);

    setHook((void*)0x005FD030, (void*)0x00619FB0);

    //float (__thiscall UNIT::*unit_distance_to_position_new_p)(float, float, float) = &UNIT::unit_distance_to_position_new;
    //setHook((void*)0x00551C60, (void*&)unit_distance_to_position_new_p);

    //int(__thiscall UNIT::*unit_unknown_moving_update_end_p)() = &UNIT::unit_unknown_moving_update_end;
    //setHook((void*)0x004A3D90, (void*&)unit_unknown_moving_update_end_p);
}
#pragma optimize( "", on )

//disable static upd - useless
//disable animated upd - useless
//disable moving upd - useless
//disable action upd - some improvement
//disable rge combat upd - only improv when disable unit ai update
//disable tribe combat upd - useless
