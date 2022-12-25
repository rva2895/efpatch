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

/*const unsigned char ftol_new_up[] =
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

__declspec(naked) void softUnitCollision_v2() //004BE8F0
{
    __asm
    {
        mov     eax, [ecx + 0Ch]
        mov     edx, [esp + 4]
        shl     edx, 5
        mov     ecx, [eax + 0BF18h]
        mov     eax, [esp + 8]
        push    ebx
        push    ebp
        mov     ecx, [ecx + eax * 4]
        xor     ebx, ebx
        push    esi
        push    edi
        mov     bl, [ecx + edx + 0Dh]
        xor     edi, edi
        test    ebx, ebx
        jle     short loc_55A920
        mov     ebp, [ecx + edx + 14h]
        mov     edx, [esp + 20h]
loc_55A8DF:
        mov     ecx, [ebp + 0]
        mov     eax, [ecx + 14h]
        cmp     dword ptr [eax + 44h], 0
        jz      short loc_55A90A

        mov     al, [eax + 78h]
        cmp     al, byte ptr [esp + 24h]
        jnz     loc_55A90A

        test    edx, edx
        jle     short loc_55A914
        mov     esi, [ecx + 10h]
        mov     ecx, [esp + 1Ch]
        xor     eax, eax

loc_55A8F8:
        cmp     esi, [ecx]
        jz      short loc_55A906
        inc     eax
        add     ecx, 4
        cmp     eax, edx
        jge     short loc_55A914
        jmp     short loc_55A8F8

loc_55A906:
        cmp     eax, edx
        jge     short loc_55A914

loc_55A90A:
        inc     edi
        add     ebp, 4
        cmp     edi, ebx
        jge     short loc_55A920
        jmp     short loc_55A8DF

loc_55A914:
        pop     edi
        pop     esi
        pop     ebp
        mov     eax, 1
        pop     ebx
        ret     14h

loc_55A920:
        pop     edi
        pop     esi
        pop     ebp
        xor     eax, eax
        pop     ebx
        ret     14h
    }
}

__declspec(naked) void addWaypointAt_1_v2()
{
    __asm
    {
        push    esi
        mov     esi, ecx
        mov     ecx, [esi + 0Ch]
        mov     eax, [esi + 10h]
        and     ecx, ecx
        jz      loc_55B469
        cmp     ecx, eax
        jl      short loc_7B6F4A
        mov     ecx, [esi + 10h]
        shl     ecx, 1
        push    ecx
        mov     ecx, esi
        mov     eax, 004BCBA0h
        call    eax //BPath::EnsureRoomInWaypointArray(int)
        test    eax, eax
        jnz     short loc_7B6F4A
        pop     esi
        ret     4

loc_7B6F4A:
        mov     eax, [esi + 0Ch]
        lea     eax, [eax + eax * 2]
        mov     ecx, [esp + 8]
        shl     eax, 2
        mov     edx, [esi + 14h]
        add     edx, eax
        mov     eax, [ecx + 4]
        mov     [edx + 4], eax
        mov     eax, [ecx]
        mov     ecx, [ecx + 8]
        mov     [edx], eax
        mov     eax, [esi + 0Ch]
        mov     [edx + 8], ecx
        lea     edx, [eax + 1]
        lea     ecx, [eax + eax * 2]
        mov     [esi + 0Ch], edx
        mov     esi, [esi + 14h]
        shl     ecx, 2
        add     ecx, esi

loc_7B6F80:
        mov     eax, [ecx]
        mov     edx, [esi]
        mov     [esi], eax
        mov     [ecx], edx
        mov     eax, [ecx + 4]
        mov     edx, [esi + 4]
        mov     [esi + 4], eax
        mov     [ecx + 4], edx
        mov     eax, [ecx + 8]
        mov     edx, [esi + 8]
        mov     [esi + 8], eax
        mov     [ecx + 8], edx
        add     esi, 0Ch
        sub     ecx, 0Ch
        cmp     esi, ecx
        jb      short loc_7B6F80
        mov     eax, 1
        pop     esi
        ret     4

loc_55B469:
        mov     edx, 004BC539h
        jmp     edx
    }
}

__declspec(naked) void BPath_v2()
{
    __asm
    {
        mov     eax, ecx
        mov     ecx, [esp + 4]
        mov     [eax + 8], ecx
        xor     ecx, ecx
        mov     [eax + 0Ch], ecx
        mov     [eax + 10h], ecx
        mov     [eax + 14h], ecx
        mov     [eax + 18h], ecx
        mov     [eax + 1Ch], ecx
        mov     dword ptr [eax + 20h], 6
        mov     dword ptr [eax + 24h], 4
        mov     [eax + 28h], ecx
        mov     [eax + 2Ch], ecx
        mov     [eax + 30h], ecx
        mov     [eax], eax
        mov     [eax + 4], eax
        ret     4
    }
}

__declspec(naked) void EnsureRoomInWaypointArray_v2()
{
    __asm
    {
        mov     eax, [esp + 4]
        push    esi
        push    edi
        lea     eax, [eax + eax * 2]
        push    ebx
        push    ebp
        shl     eax, 2
        mov     ebx, ecx
        push    eax
        mov     eax, 00632B9Dh
        call    eax //operator new(uint)
        mov     ebp, eax
        mov     esi, [ebx + 14h]
        add     esp, 4
        test    esi, esi
        jz      short loc_55BA29
        mov     eax, [ebx + 0Ch]
        push    esi
        test    eax, eax
        jle     short loc_55BA21
        lea     ecx, [eax + eax * 2]
        mov     edi, ebp
        rep movsd

loc_55BA21:
        mov     eax, 00632B42h
        call    eax //operator delete(void*)
        add     esp, 4

loc_55BA29:
        mov     eax, [esp + 14h]
        mov     [ebx + 14h], ebp
        mov     [ebx + 10h], eax
        pop     ebp
        pop     ebx
        pop     edi
        pop     esi
        ret     4
    }
}

__declspec(naked) void IntersectsSegment_v2_sub()
{
    __asm
    {
        fld     dword ptr [ecx + 4]
        fsub    dword ptr [edx + 4]
        fld     dword ptr [ecx]
        fsub    dword ptr [edx]
        fld     st(3)
        fmul    st, st(2)
        fld     st(1)
        fmul    st, st(4)
        add     esi, 1
        fsubp   st(1), st
        fldz
        fcomip  st, st(1)
        fstp    dword ptr [esp + 14h]
        fld     st(4)
        fsub    dword ptr [edx + 4]
        fld     st(6)
        fsub    dword ptr [edx]
        fxch    st(1)
        fstp    dword ptr [esp + 0Ch]
        fstp    dword ptr [esp + 10h]
        fld     dword ptr [esp + 0Ch]
        fmul    st, st(1)
        fld     dword ptr [esp + 10h]
        fmul    st, st(3)
        lea     ecx, [ebx + esi * 8]
        mov     eax, 0
        lea     edx, [ebx + esi * 8 - 8]
        fsubp   st(1), st
        fstp    st(2)
        fstp    st
        jz      short loc_7E5F36
        fld     dword ptr [esp + 10h]
        fmul    st, st(2)
        fld     dword ptr [esp + 0Ch]
        fmul    st, st(4)
        fld     dword ptr [esp + 14h]
        fmul    dword ptr [esp + 8]
        fxch    st(2)
        fsubp   st(1), st
        fld     dword ptr [esp + 14h]
        fmul    dword ptr [esp + 4]
        fxch    st(3)
        ja      short loc_7E5F10
        fcomi   st, st(2)
        jb      short loc_7E5F30
        fcomip  st, st(3)
        ja      short loc_7E5F32
        fcomi   st, st(1)
        jb      short loc_7E5F32
        fcomip  st, st(2)
        fstp    st
        ja      short loc_7E5F36
        mov     eax, 1
        fstp    st
        ret

loc_7E5F10:
        fcomi   st, st(2)
        ja      short loc_7E5F30
        fcomip  st, st(3)
        jb      short loc_7E5F32
        fcomi   st, st(1)
        ja      short loc_7E5F32
        fcomip  st, st(2)
        fstp    st
        jb      short loc_7E5F36
        mov     eax, 1
        fstp    st
        ret

        align   10h

loc_7E5F30:
        fstp    st

loc_7E5F32:
        fstp    st
        fstp    st

loc_7E5F36:
        fstp    st
        ret
    }
}

__declspec(naked) void IntersectsSegment_v2()
{
    __asm
    {
        push    ebx
        mov     eax, [ecx]
        push    esi
        push    edi
        test    eax, eax
        jle     short loc_7E5E5B
        mov     ebx, [ecx + 4]
        mov     ecx, [esp + 14h]
        mov     edx, [esp + 18h]
        mov     esi, 1
        fld     dword ptr [ecx]
        fld     dword ptr [ecx + 4]
        sub     esp, 0Ch
        fld     dword ptr [edx]
        fsub    st, st(2)
        lea     ecx, [ebx + esi * 8]
        cmp     esi, eax
        fld     dword ptr [edx + 4]
        fsub    st, st(2)
        push    0B3D6BF95h
        lea     edx, [ecx - 8]
        push    3F800001h
        jz      short loc_7E5E4D
        mov     edi, eax

loc_7E5E40:
        call    IntersectsSegment_v2_sub
        test    eax, eax
        jnz     short loc_7E5E54
        cmp     esi, edi
        jl      short loc_7E5E40

loc_7E5E4D:
        mov     ecx, ebx
        call    IntersectsSegment_v2_sub

loc_7E5E54:
        ffreep  st(2)
        add     esp, 14h
        ffreep  st(2)

loc_7E5E5B:
        pop     edi
        pop     esi
        pop     ebx
        ret     14h
    }
}

__declspec(naked) void AddtoObjectsList_v2()
{
    __asm
    {
        mov     edx, [ecx + 20h]
        push    ebx
        mov     eax, [ecx + 24h]
        push    esi
        mov     ebx, ecx
        push    edi
        cmp     edx, eax
        jl      short loc_56AB32
        shl     eax, 2
        mov     [ebx + 24h], eax
        lea     eax, [eax + eax * 4]
        mov     esi, [ebx + 28h]
        shl     eax, 2
        push    eax; unsigned int
        mov     eax, 00632B9Dh
        call    eax //operator new(uint)
        mov     edi, eax
        add     esp, 4
        mov     [ebx + 28h], eax
        test    esi, esi
        jz      short loc_56AB32
        mov     eax, [ebx + 20h]
        lea     ecx, [eax + eax * 4]
        push    esi
        rep movsd
        mov     eax, 00632B42h
        call    eax //operator delete(void*)
        add     esp, 4

loc_56AB32:
        mov     edx, [ebx + 28h]
        mov     eax, [ebx + 20h]
        mov     esi, [esp + 10h]
        lea     ecx, [eax + eax * 4]
        inc     eax
        lea     edi, [edx + ecx * 4]
        mov     [ebx + 20h], eax
        mov     edx, [esi]
        mov     eax, [esi + 4]
        mov     ecx, [esi + 8]
        mov     ebx, [esi + 0Ch]
        mov     esi, [esi + 10h]
        mov     [edi], edx
        mov     [edi + 4], eax
        mov     [edi + 8], ecx
        mov     [edi + 0Ch], ebx
        mov     [edi + 10h], esi
        pop     edi
        pop     esi
        pop     ebx
        ret     8
    }
}

__declspec(naked) void InsideHull_v2()
{
    __asm
    {
        push    esi
        mov     esi, [ecx]
        mov     edx, [ecx + 4]
        xor     eax, eax
        and     esi, esi
        jle     loc_56B17B
        lea     ecx, [edx + esi * 8 - 8]
        fld     dword ptr [esp + 8]
        fld     dword ptr [esp + 0Ch]
        fld     dword ptr [ecx + 4]

loc_56B100:
        fld     dword ptr [edx + 4]
        dec     esi
        fcomi   st, st(2)
        fxch    st(1)
        ja      short loc_56B121    //ja
        fcomip  st, st(2)
        ja      short loc_56B140    //ja
        mov     ecx, edx
        add     edx, 8
        and     esi, esi
        jg      short loc_56B100
        fstp    st
        fstp    st
        fstp    st
        pop     esi
        ret     8

loc_56B121:
        fcomip  st, st(2)
        jbe     short loc_56B140    //jbe
        mov     ecx, edx
        add     edx, 8
        and     esi, esi
        jg      short loc_56B100
        fstp    st
        fstp    st
        fstp    st
        pop     esi
        ret     8

        align   10h

loc_56B140:
        fld     st
        fld     dword ptr [ecx + 4]
        fld     dword ptr [edx]
        fld     dword ptr [ecx]
        fsub    st, st(1)
        fld     st(5)
        fsub    st, st(5)
        fxch    st(4)
        fsubp   st(3), st
        fmulp   st(3), st
        mov     ecx, edx
        fmul    st, st(1)
        faddp   st(2), st
        add     edx, 8
        fmul    st, st(4)
        fldz
        fcomip  st, st(1)
        jae     short loc_56B168    //ja
        fxch    st(1)

loc_56B168:
        fcomip  st, st(1)
        ja      short loc_56B16F    //ja
        xor eax, 1

loc_56B16F:
        fstp    st
        and     esi, esi
        jg      short loc_56B100
        fstp    st
        fstp    st
        fstp    st

loc_56B17B:
        pop     esi
        ret     8
    }
}

__declspec(naked) void InsideHull_old()
{
    __asm
    {
        sub     esp, 10h
        push    ebx
        push    esi
        push    edi

        mov     esi, 004A7BC6h
        jmp     esi
    }
}

struct pointf
{
    float x;
    float y;
};

bool __stdcall call_hulls(void* hull, float x, float y)
{
    bool (__thiscall * h_new)(void*, float, float) = (bool (__thiscall*)(void*, float, float))InsideHull_v2;
    bool (__thiscall * h_old)(void*, float, float) = (bool(__thiscall*)(void*, float, float))InsideHull_old;

    bool result_new = h_new(hull, x, y);
    bool result_old = h_old(hull, x, y);

    if (result_new != result_old)
    {
        int nPoints = *(int*)hull;
        pointf* points = *(pointf**)((void**)hull + 1);
        //log("=====> InsideHull() diff, Hull:")
        char buf[0x400];
        char b[0x100];
        sprintf(buf, "====> InsideHull() diff, Hull: ");
        for (int i = 0; i < nPoints; i++)
        {
            sprintf(b, "(%f,%f) ", points[i].x, points[i].y);
            strcat(buf, b);
        }
        sprintf(b, "params: %f, %f", x, y);
        strcat(buf, b);
        log("%s", buf);
        __debugbreak();
    }

    return result_old;
}

__declspec(naked) void InsideHull_v2_dispatcher()
{
    __asm
    {
        mov     eax, [esp + 4]
        mov     edx, [esp + 8]
        push    edx
        push    eax
        push    ecx
        call    call_hulls
        ret     8
    }
}

__declspec(naked) void addTileObjects_sub_v2()
{
    __asm
    {
        push    ebx
        mov     ebx, ecx
        push    esi
        mov     eax, [ebx + 28h]
        push    edi
        test    eax, eax
        jz      short loc_7B6DA4
        mov     edx, [ebx + 20h]
        mov     esi, [esp + 10h]
        test    edx, edx
        jle     short loc_7B6DA7
        mov     ecx, [esi + 10h]
        add     eax, 10h
        mov     esi, edx
        and     edx, 0FFFFFFFCh
        jle     short loc_7B6D92
        xor     edi, edi
        
        align   10h

loc_7B6D70:
        cmp     [eax], ecx
        jz      short loc_7B6D9E
        cmp     [eax + 14h], ecx
        jz      short loc_7B6D9E
        cmp     [eax + 28h], ecx
        jz      short loc_7B6D9E
        cmp     [eax + 3Ch], ecx
        jz      short loc_7B6D9E
        add     edi, 4
        add     eax, 50h
        cmp     edi, edx
        jl      short loc_7B6D70
        and     esi, 3
        jz      short loc_7B6DA4

loc_7B6D92:
        cmp     [eax], ecx
        jz      short loc_7B6D9E
        add     eax, 14h
        dec     esi
        jg      short loc_7B6D92
        jmp     short loc_7B6DA4

loc_7B6D9E:
        pop     edi
        pop     esi
        pop     ebx
        ret     8

loc_7B6DA4:
        mov     edx, [ebx + 20h]

loc_7B6DA7:
        mov     eax, [ebx + 24h]
        cmp     edx, eax
        jl      short loc_7B6DE0
        shl     eax, 2
        mov     [ebx + 24h], eax
        lea     eax, [eax + eax * 4]
        mov     esi, [ebx + 28h]
        shl     eax, 2
        push    eax
        mov     eax, 00632B9Dh
        call    eax //operator new(uint)
        mov     edi, eax
        add     esp, 4
        mov     [ebx + 28h], eax
        test    esi, esi
        jz      short loc_7B6DE0
        mov     eax, [ebx + 20h]
        lea     ecx, [eax + eax * 4]
        push    esi
        rep movsd
        mov     eax, 00632B42h
        call    eax //operator delete(void*)
        add     esp, 4

loc_7B6DE0:
        mov     edx, [ebx + 28h]
        mov     eax, [ebx + 20h]
        mov     esi, [esp + 10h]
        lea     ecx, [eax + eax * 4]
        inc     eax
        lea     edi, [edx + ecx * 4]
        mov     [ebx + 20h], eax
        mov     edx, [esi]
        mov     eax, [esi + 4]
        mov     ecx, [esi + 8]
        mov     ebx, [esi + 0Ch]
        mov     esi, [esi + 10h]
        mov     [edi], edx
        mov     [edi + 4], eax
        mov     [edi + 8], ecx
        mov     [edi + 0Ch], ebx
        mov     [edi + 10h], esi
        pop     edi
        pop     esi
        pop     ebx
        ret     8
    }
}

__declspec(naked) void addShapeToHulls_2() //004A9FE0
{
    __asm
    {
        cmp     ebp, eax
        jz      short loc_56D63A
        
        mov     ecx, 004A9F5Eh
        jmp     ecx

loc_56D63A:
        mov     ecx, 004AA031h
        jmp     ecx
    }
}

__declspec(naked) void addShapeToHulls_1() //004A9FD8
{
    __asm
    {
        mov     eax, [edi + 4]
        mov     ecx, [esp + 14h]
        dec     eax
        mov     edx, [edi + 0Ch]
        cmp     ecx, eax
        jnz     short loc_56D604
        mov     [esp + 14h], ebp

loc_56D604:
        mov     ecx, [edx + eax * 4]
        mov     [edi + 4], eax
        mov     [edx + ebp * 4], ecx
        mov     [edx + eax * 4], esi
        mov     ebx, [esp + 10h]
        xor     ebp, ebp
        test    eax, eax
        jg      short loc_56D5C5

        mov     ecx, 004A9FE0h
        jmp     ecx

loc_56D5C5:
        mov     ecx, 004A9F6Fh
        jmp     ecx
    }
}

__declspec(naked) void isMoving_v2() //004A3D90
{
    __asm
    {
        mov     edx, [ecx + 120h]
        xor     eax, eax
        and     edx, edx
        jz      short locret_573732
        cmp     eax, [edx + 8]
        setnz   al

locret_573732:
        retn
    }
}

__declspec(naked) void checkAcceleration_patch() //004A6569
{
    __asm
    {
        mov     ecx, [esi + 14h]
        push    eax
        mov     edi, [ecx]
        call    dword ptr [edi + 54h]
        mov     eax, [esi + 120h]
        fstp    dword ptr [esp + 14h]
        test    eax, eax
        jz      loc_575F91
        push    eax
        mov     ecx, [esi + 14h]
        call    dword ptr [edi + 58h]
        mov     edi, [esi]
        fstp    dword ptr [esp + 10h]
        mov     ecx, esi
        push    eax
        call    dword ptr [edi + 31Ch]

        mov     eax, 004A6644h
        jmp     eax

loc_575F91:
        mov     eax, 004A6833h
        jmp     eax
    }
}

__declspec(naked) void addWaypointAt_2_v2()
{
    __asm
    {
        push    esi
        mov     esi, ecx
        mov     eax, [esi + 10h]
        mov     ecx, [esi + 0Ch]
        cmp     ecx, eax
        jl      short loc_7B6E62
        mov     ecx, [esi + 10h]
        shl     ecx, 1
        push    ecx
        mov     ecx, esi
        mov     eax, 004BCBA0h
        call    eax //BPath::EnsureRoomInWaypointArray(int)
        test    eax, eax
        jnz     short loc_7B6E62
        pop     esi
        ret     4

loc_7B6E62:
        mov     eax, [esi + 0Ch]
        lea     eax, [eax + eax * 2]
        mov     ecx, [esp + 8]
        shl     eax, 2
        mov     edx, [esi + 14h]
        add     edx, eax
        mov     eax, [ecx]
        mov     dword ptr [edx + 4], 0
        mov     ecx, [ecx + 8]
        mov     [edx], eax
        mov     eax, [esi + 0Ch]
        mov     [edx + 8], ecx
        inc     eax
        mov     [esi + 0Ch], eax
        pop     esi
        ret     4
    }
}

__declspec(naked) void addWaypointAt_3_v2()
{
    __asm
    {
        push    esi
        mov     esi, ecx
        mov     eax, [esi + 10h]
        mov     ecx, [esi + 0Ch]
        cmp     ecx, eax
        jl      short loc_7B6EB2
        mov     ecx, [esi + 10h]
        shl     ecx, 1
        push    ecx
        mov     ecx, esi
        mov     eax, 004BCBA0h
        call    eax //BPath::EnsureRoomInWaypointArray(int)
        test    eax, eax
        jnz     short loc_7B6EB2
        pop     esi
        ret     4

loc_7B6EB2:
        mov     eax, [esi + 0Ch]
        lea     eax, [eax + eax * 2]
        mov     ecx, [esp + 8]
        shl     eax, 2
        mov     edx, [esi + 14h]
        add     edx, eax
        mov     eax, [ecx]
        mov     dword ptr [edx + 4], 0
        mov     ecx, [ecx + 8]
        mov     [edx], eax
        mov     eax, [esi + 0Ch]
        mov     [edx + 8], ecx
        inc     eax
        mov     [esi + 0Ch], eax
        dec     eax
        jle     short loc_7B6F08
        lea     ecx, [eax + eax * 2]
        mov     esi, [esi + 14h]
        shl     ecx, 2
        add     ecx, esi

loc_7B6EEA:
        mov     eax, [ecx]
        mov     edx, [esi]
        mov     [esi], eax
        mov     [ecx], edx
        mov     eax, [ecx + 8]
        mov     edx, [esi + 8]
        mov     [esi + 8], eax
        mov     [ecx + 8], edx
        add     esi, 0Ch
        sub     ecx, 0Ch
        cmp     esi, ecx
        jb      short loc_7B6EEA

loc_7B6F08:
        mov     eax, 1
        pop     esi
        ret     4
    }
}

__declspec(naked) void unit_group_update_v2() //0047ED89
{
    __asm
    {
        cmp     al, 5
        jz      loc_595BEA
        cmp     al, 0Bh
        jz      loc_595BEA

        mov     eax, 0047EDADh
        jmp     eax

loc_595BEA:
        mov     eax, 0047EF63h
        jmp     eax
    }
}

__declspec(naked) void combat_object_update_v2() //00444DA3
{
    __asm
    {
        lea     edx, [ecx + 174h]
        push    esi
        push    ebx
        mov     eax, [edx]
        mov     esi, ecx
        test    eax, eax
        jz      short loc_5CEB4E
        push    eax
        mov     ebx, [ecx + 18h]
        fldz
        fld     dword ptr [esp]
        mov     ecx, [ebx + 8Ch]
        fsub    dword ptr [ecx + 0A8h]
        xor     eax, eax
        add     esp, 4
        fst     dword ptr [edx]
        fcomip  st, st(1)
        //mov     ecx, esi
        fstp    st
        jnb     short loc_5CEB4E
        mov     [edx], eax

loc_5CEB4E:
        mov     eax, 00444DECh
        jmp     eax
    }
}

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

    /*if (IsProcessorFeaturePresent(PF_SSE3_INSTRUCTIONS_AVAILABLE))
    {
        log("SSE3 detected, using fisttp ftol");
        writeData(0x00632BAC, ftol_new_up, sizeof(ftol_new_up));
    }
    else
        log("SSE3 not detected");*/

    //float (__thiscall UNIT::*unit_distance_to_position_new_p)(float, float, float) = &UNIT::unit_distance_to_position_new;
    //setHook((void*)0x00551C60, (void*&)unit_distance_to_position_new_p);

    //int(__thiscall UNIT::*unit_unknown_moving_update_end_p)() = &UNIT::unit_unknown_moving_update_end;
    //setHook((void*)0x004A3D90, (void*&)unit_unknown_moving_update_end_p);

    //do_auto_scroll
    writeByte(0x00616BAF, 0xA1);
    writeDword(0x00616BB0, 0x006A3660);
    writeDword(0x00616BB4, 0x90909090);
    writeByte(0x00616BB8, 0x90);
    writeWord(0x00616BB9, 0xC085);
    //pick_tile
    writeWord(0x00611034, 0x0EEB);
    //handle_keys
    writeByte(0x005FD482, 0xA1);
    writeDword(0x005FD483, 0x006A3660);
    writeDword(0x005FD487, 0x90909090);
    writeByte(0x005FD48B, 0x90);
    writeWord(0x005FD48C, 0xC085);
    //tribe_action_hunt::~tribe_action_hunt
    writeDword(0x004061B4, 0x0040A8E0 - 0x004061B8);
    //CB1C2 !!
    //
    //is_gate_openable
    BYTE is_gate_openable_patch[] = { 0x8B, 0x41, 0x14, 0x56, 0x66, 0x83, 0x78, 0x1E, 0x08, 0x75, 0x51, 0x8B, 0xF1, 0x57 };
    writeData(0x005582F0, is_gate_openable_patch, sizeof(is_gate_openable_patch));
    //capture_frame_3d_square
    writeDword(0x0054CE9A, 0x90909090);
    writeDword(0x0054CE9E, 0x90909090);
    writeByte(0x0054CEA5, 0x90);
    writeWord(0x0054CEA6, 0x9090);
    //get_node
    writeByte(0x00547547, 0x90);
    writeWord(0x00547548, 0x9090);
    //decode_mp3_data
    writeWord(0x00546BB7, 0x85EB);
    //handle_mouse_pointing_at
    writeByte(0x005007D3, 0x90);
    writeDword(0x005007D4, 0x158B9090);
    writeDword(0x005007D8, 0x006A3660);
    writeWord(0x005007DC, 0xD221);
    //TMessagePanel::handle_idle !!
    //
    //find_tile_path
    
    writeByte(0x004BD8F5, 0x90);
    writeDword(0x004BD8F6, 0x90909090);
    writeWord(0x004BD8D4, 0x9090);
    writeWord(0x004BD8D9, 0x17EB);
    
    setHook((void*)0x004BE8F0, softUnitCollision_v2);

    //copy_path
    writeDword(0x004BEA5E, 0xFFFFDACE);

    writeDword(0x004BEC28, (DWORD)addWaypointAt_1_v2 - 0x004BEC2C);

    setHook((void*)0x004BC380, BPath_v2);
    
    //add_waypoint_at_start
    writeDword(0x004BC4A6, 0x5002E0C1);
    writeWord(0x004BC4AA, 0x10EB);
    //add_waypoint_at_end
    writeDword(0x004BC546, 0x5002E0C1);
    writeWord(0x004BC54A, 0x10EB);

    setHook((void*)0x004BCBA0, EnsureRoomInWaypointArray_v2);
    
    //get_new_hull
    writeDword(0x004AA0C4, 0x140);
    writeDword(0x004AA0DD, 0x10);

    //does_shape_intersect
    writeDword(0x004A7441, ((DWORD)IntersectsSegment_v2 - 0x004A7445));
    writeDword(0x004A7480, ((DWORD)IntersectsSegment_v2 - 0x004A7484));
    writeDword(0x004A74BF, ((DWORD)IntersectsSegment_v2 - 0x004A74C3));
    writeDword(0x004A74FE, ((DWORD)IntersectsSegment_v2 - 0x004A7502));
    //does_hull_intersect
    writeDword(0x004A75F4, ((DWORD)IntersectsSegment_v2 - 0x004A75F8));
    
    //these cause desync
    //setHook((void*)0x004A7620, AddtoObjectsList_v2);
    //setHook((void*)0x004A7BC0, InsideHull_v2_dispatcher);
    
    //add_tile_objects
    writeDword(0x004A9E0B, ((DWORD)addTileObjects_sub_v2 - 0x004A9E0F));
    
    writeDword(0x004A9F6B, 0x71);
    writeByte(0x004A9F96, 0x49);
    setHook((void*)0x004A9FE0, addShapeToHulls_2);
    setHook((void*)0x004A9FD8, addShapeToHulls_1);
    
    setHook((void*)0x004A3D90, isMoving_v2);

    //calculate_future_velocity
    writeByte(0x004A3E59, 0x16);

    setHook((void*)0x004A6569, checkAcceleration_patch);
    
    //find_path
    writeDword(0x004984D2, 0x0002405A);

    writeDword(0x0049856E, (DWORD)addWaypointAt_2_v2 - 0x00498572);
    writeDword(0x004985A9, (DWORD)addWaypointAt_3_v2 - 0x004985AD);

    //add_path_node
    BYTE add_path_node_patch[] = { 0x8B, 0x5D, 0x1C, 0x8B, 0x0C, 0xB8, 0x8B, 0x45, 0x10, 0x8B, 0x0C, 0x91, 0x8D, 0x0C, 0x41, 0x89, 0x4D, 0xF4, 0x8A, 0x09, 0xEB, 0x20 };
    writeData(0x00498A03, add_path_node_patch, sizeof(add_path_node_patch));

    //----------

    //get_new_node
    writeByte(0x004991C8, 0xEB);

    //delete_dynamic_tile
    BYTE delete_dynamic_tile_patch[] = { 0x39, 0x38, 0x74, 0x14, 0x8B, 0xF0, 0x8B, 0x40, 0x04, 0xEB, 0xF5 };
    writeData(0x004977EE, delete_dynamic_tile_patch, sizeof(delete_dynamic_tile_patch));
    
    //create_column_line
    writeDword(0x004792B5, 0x3E800000);
    writeDword(0x004792D2, 0x0018D64A);
    writeDword(0x0047930B, 0x3F000000);
    writeDword(0x00479315, 0x0018D607);
    //create_phalanx
    writeDword(0x0047A3C8, 0x3F000000);
    writeDword(0x0047A3D2, 0x0018C54A);

    setHook((void*)0x0047ED89, unit_group_update_v2);
    setHook((void*)0x00444DA3, combat_object_update_v2);

    //rge_animated_object_destructor
    writeDword(0x0046FE74, 0x000DB808);

    //pick_best_target
    writeDword(0x006692E8, 0x00619FB0);

    //object vftable shortcuts
    /*
    writeDword(0x006638C8, 0x004A47B0);
    writeDword(0x00663428, 0x004A47B0);
    writeDword(0x0065C12C, 0x004A47B0);
    writeDword(0x0065B89C, 0x00444DA0);
    writeDword(0x0065BB20, 0x004A47B0);
    writeDword(0x0065785C, 0x004A47B0);
    writeDword(0x0065516C, 0x004A47B0);
    writeDword(0x0065C12C, 0x004A47B0);
    */
}
#pragma optimize( "", on )

//159ACD - computer player value, probably useless optimization
//BPath::waypoint - probably useless optimization
//TPanel::handle_idle - optimization?
//00563390 - useless?
//other obstruction manager stuff
//LineIntersect - probably useful optimization

//disable static upd - useless
//disable animated upd - useless
//disable moving upd - useless
//disable action upd - some improvement
//disable rge combat upd - only improv when disable unit ai update
//disable tribe combat upd - useless
//smoothMidLevelPath
//doIntelligentMovementUpdate - probably useless optimization
//update_mip_map_square
//get_tile_info
//add_path_node cont.
//add_node_to_stack
//get_map_reference_tile
//get_map_reference_area
//get_hotkey
//calculateMetrics - probably useless
//moveUnit - probably useless
//RGE_Active_Animated_Sprite::update
//RGE_Active_Sprite_List::update
