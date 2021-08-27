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

unsigned char ftol_new_sse_packed[] =
{ "\x83\xEC\x04\xD9\x1C\x24\xF3\x0F\x10\x04\x24\xF3\x0F\x2C\xC0\x99\x83\xC4\x04\xC3" };
unsigned char ftol_new_sse_fixed_2_packed[] =
{ "\x83\xec\x08\xdd\x1c\x24\xf2\x0f\x2c\x04\x24\x99\x83\xc4\x08\xc3"};

//member function wrappers to make compiler behave

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

LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
LARGE_INTEGER Frequency;

long long total = 0;
long long since_last_out = 0;

int n_updates = 0;

void __stdcall query_performance_in()
{
    //QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&StartingTime);
}

//cannon slow update

__declspec(naked) void performance_in()
{
    __asm
    {
        push    ecx
        call    query_performance_in
        pop     ecx
        push    ecx
        push    ebx
        push    esi
        mov     esi, ecx
        mov     eax, 0055A765h
        jmp     eax
    }
}

__declspec(naked) void remover()
{
    __asm
    {
        mov     eax, 1
        ret     4
    }
}

__declspec(naked) void remover_2() //update building
{
    __asm
    {
        //check cannon
        mov     eax, [ecx + 14h]
        mov     ax, [eax + 1Eh]
        cmp     ax, 6
        jz      dont_update
        cmp     ax, 7
        jz      dont_update
        cmp     ax, 8
        jz      dont_update
        cmp     ax, 9
        jz      dont_update
        cmp     ax, 10
        jz      dont_update
        cmp     ax, 18
        jz      dont_update
        cmp     ax, 34
        jz      dont_update
        cmp     ax, 36
        jz      dont_update
        cmp     ax, 20
        jz      dont_update
        cmp     ax, 61
        jz      dont_update

        sub     esp, 8
        push    ebx
        push    ebp
        mov     eax, 00555135h
        jmp     eax
dont_update:
        xor     al, al
        retn
    }
}

__declspec(naked) void remover_3()
{
    __asm
    {
        xor     eax, eax
        ret
    }
}

void __stdcall query_performance_out(UNIT* unit)
{
    QueryPerformanceCounter(&EndingTime);
    ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;

    ElapsedMicroseconds.QuadPart *= 1000000;
    ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;

    total += ElapsedMicroseconds.QuadPart;
    n_updates++;

    /*if ((total - since_last_out) > 1000000LL)
    {
        chat("World update, %lld ms", total / 1000);
        since_last_out = total;
    }*/
    /*if (ElapsedMicroseconds.QuadPart > 30000)
    {
        chat("TRIBE_Combat_Object::update (id %d) took more than 30ms - %dms", 
            (int)unit->prop_object->id1, ElapsedMicroseconds.QuadPart / 1000);
        WorldPlayerBase__select_object(getCurrentPlayer(), unit, 0);
    }*/
}

int n_queries = 0;

void __stdcall query_performance_check_and_reset()
{
    /*if (total > 100000)
    {
        chat("TRIBE_Combat_Object::update (total %d objects) took more than 100ms - %dms", n_updates, total / 1000);
    }*/
    //if (n_updates > 0)
    //    chat("%d objects, %dms", n_updates, total / 1000);
    n_updates = 0;
    //total = 0;
    n_queries++;
    if (n_queries > 10000)
    {
        chat("Avg per 10000 queries: %lld us per turn", total / n_queries);
        n_queries = 0;
    }
}

__declspec(naked) void performance_out()
{
    __asm
    {
        push    eax
        push    esi
        call    query_performance_out
        pop     eax
        mov     al, bl
        pop     esi
        pop     ebx
        pop     ecx
        retn
    }
}

__declspec(naked) void performance_out_2()
{
    __asm
    {
        push    eax
        push    esi
        call    query_performance_out
        pop     eax
        fstp    dword ptr [esi + 3Ch]
        pop     esi
        pop     ebx
        pop     ecx
        retn
    }
}

__declspec(naked) void performance_check_and_reset()
{
    __asm
    {
        call    query_performance_check_and_reset
        retn
    }
}

int high_sse;
int low_sse;

double f_val;
float s_val;

void __stdcall ftol_test(int high, int low)
{
    if ((high != high_sse) || (low != low_sse))
         __debugbreak();
        //volatile int k = 0;
    //if (*(unsigned int*)&high == 0x80000000)
    //   __debugbreak();
}

__declspec(naked) void ftol_old()
{
    __asm
    {
        push    ebp
        mov     ebp, esp
        add     esp, 0FFFFFFF4h

        sub     esp, 8
        fst     qword ptr [esp]
        fst     f_val
        //fst     s_val
        //fld     dword ptr [esp]
        call    ftol_new_sse_fixed
        //fld     dword ptr [esp - 8]
        mov     high_sse, edx
        mov     low_sse, eax

        fstcw   word ptr [ebp - 2]
        wait
        mov     ax, [ebp - 2]
        or      ah, 0Ch
        mov     [ebp - 4], ax
        fldcw   word ptr [ebp - 4]
        fistp   qword ptr [ebp - 0Ch]
        fldcw   word ptr [ebp - 2]
        mov     eax, [ebp - 0Ch]
        mov     edx, [ebp - 8]
        push    eax
        push    edx
        push    eax
        push    edx
        call    ftol_test
        pop     edx
        pop     eax
        leave
        retn
    }
}

__declspec(naked) void onUnitAIModule()
{
    __asm
    {
        mov     edx, [esi + 14h]
        cmp     byte ptr [edx + 4], 80
        jnz     upd_unit_ai
        mov     dx, [edx + 1Eh]
        cmp     dx, 6
        jmp      no_upd_unit_ai
        cmp     dx, 7
        jz      no_upd_unit_ai
        cmp     dx, 8
        jz      no_upd_unit_ai
        cmp     dx, 20
        jz      no_upd_unit_ai
        cmp     dx, 61
        jz      no_upd_unit_ai

        mov     edx, [esi + 14h]
        mov     dx, [edx + 18h]
        //cmp     dx, 12 //pc
        //jz      no_upd_unit_ai
        cmp     dx, 70 //prefab
        jz      no_upd_unit_ai

upd_unit_ai:
        mov     edx, 00419250h
        push    eax
        call    edx
no_upd_unit_ai:
        mov     eax, 004450AFh
        jmp     eax
    }
}

__declspec(naked) void disable_shielding()
{
    __asm
    {
        xor     al, al
        ret
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

    //setHook((void*)0x00632BAC, ftol_new_sse);
    //setHook((void*)0x00632BAC, ftol_old);
    //setHook((void*)0x00632BAC, ftol_new_sse_fixed_2);
    //writeData(0x00632BAC, ftol_new_sse_fixed_2_packed, sizeof(ftol_new_sse_fixed_2_packed));

    //float (__thiscall UNIT::*unit_distance_to_position_new_p)(float, float, float) = &UNIT::unit_distance_to_position_new;
    //setHook((void*)0x00551C60, (void*&)unit_distance_to_position_new_p);

    //int(__thiscall UNIT::*unit_unknown_moving_update_end_p)() = &UNIT::unit_unknown_moving_update_end;
    //setHook((void*)0x004A3D90, (void*&)unit_unknown_moving_update_end_p);

    //performance test
    /*setHook((void*)0x0055A760, performance_in);
    setHook((void*)0x0055AB53, performance_out);
    setHook((void*)0x0055AB69, performance_out);
    setHook((void*)0x0055AA53, performance_out);
    setHook((void*)0x0055AA68, performance_out_2);
    setHook((void*)0x004AF3D0, performance_check_and_reset);*/


    //QueryPerformanceFrequency(&Frequency);

    //remover
    //setHook((void*)0x00595D90, remover); //computer ai
    //setHook((void*)0x00555130, remover_2); //update building
    //setHook((void*)0x0055A760, remover_3); //update combat object
    //setHook((void*)0x0055A760, remover_3); //update combat object
    //unsigned char nops[] = "\x90\x90\x90\x90\x90";
    //writeData(0x004C1985, nops, sizeof(nops)-1);
    //unsigned char nops2[] = "\x33\xC0\x90\x90\x90";
    //unsigned char nops3[] = "\x83\xC4\x04\x90\x90"; //add esp, 4
    //writeData(0x00555903, nops2, sizeof(nops) - 1); //type 80 -> type 70
    //writeData(0x005556F6, nops2, sizeof(nops) - 1);
    //writeData(0x0055A8A6, nops2, sizeof(nops) - 1); //no upd type 50 when upd type 70
    //writeData(0x00444DF0, nops2, sizeof(nops) - 1); //no upd type 40 when upd type 50
    //writeData(0x004450AA, nops3, sizeof(nops3) - 1); //no upd unit ai module when upd type 50
    //setHook((void*)0x004450A9, onUnitAIModule);

    //writeData(0x00444DF0, nops2, sizeof(nops2) - 1); //no action upd


    //setHook((void*)0x0054BC40, disable_shielding);

    //writeByte(0x00555141, 0xEB);
}
#pragma optimize( "", on )

//disable static upd - useless
//disable animated upd - useless
//disable moving upd - useless
//disable action upd - some improvement
//disable rge combat upd - only improv when disable unit ai update
//disable tribe combat upd - useless