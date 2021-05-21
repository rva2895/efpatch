#include "stdafx.h"

#include "zannfort.h"
#include "researchrepeat.h"
#include "advtriggereffect.h"

void (__thiscall* researchTech) (void*, short) =
    (void (__thiscall*) (void*, short)) 0x005BFEC9;

__declspec(naked) void* __stdcall getPropertyObject2(void* player, int id)
{
    __asm
    {
        push    ebp
        mov     ebp, esp
        mov     eax, player
        mov     eax, [eax + 74h]
        mov     ecx, id
        mov     eax, [eax + ecx * 4]
        leave
        ret     8
    }
}
#pragma warning(push)
#pragma warning(disable:4100)
__declspec(naked) float* __fastcall player_getResources(void* player)
{
    __asm
    {
        mov     eax, [ecx + 0ACh]
        ret
    }
}
#pragma warning(pop)

const char zann_str_1[] = "MUL Cost2Amount 0.75";

void zann_resetCosts1(void* c)
{
    //set to 1/4
    advTriggerEffectActual(getPropertyObject2(c, 2544), "SET Cost2Amount 40"); //base 160
    advTriggerEffectActual(getPropertyObject2(c, 2546), "SET Cost2Amount 40"); //base 160
    advTriggerEffectActual(getPropertyObject2(c, 1775), "SET Cost2Amount 50"); //base 200
    advTriggerEffectActual(getPropertyObject2(c, 1777), "SET Cost2Amount 50"); //base 200
    advTriggerEffectActual(getPropertyObject2(c, 1779), "SET Cost2Amount 150"); //base 600
}

void zann_resetCosts2(void* c)
{
    //add 3/4
    advTriggerEffectActual(getPropertyObject2(c, 2544), "ADD Cost2Amount 120"); //base 160
    advTriggerEffectActual(getPropertyObject2(c, 2546), "ADD Cost2Amount 120"); //base 160
    advTriggerEffectActual(getPropertyObject2(c, 1775), "ADD Cost2Amount 150"); //base 200
    advTriggerEffectActual(getPropertyObject2(c, 1777), "ADD Cost2Amount 150"); //base 200
    advTriggerEffectActual(getPropertyObject2(c, 1779), "ADD Cost2Amount 450"); //base 600
}

void __stdcall zannfort_onChange(void* c)
{
    //research* res = (research*)**((void***)c + 2);
    //removeCheckResearchedOnce();
    //researchTech(c, 800);
    int nForts = (int)player_getResources(c)[134];        //standing forts

    zann_resetCosts1(c);

    for (int i = 0; i < nForts; i++)
    {
        advTriggerEffectActual(getPropertyObject2(c, 2544), zann_str_1);
        advTriggerEffectActual(getPropertyObject2(c, 2546), zann_str_1);
        advTriggerEffectActual(getPropertyObject2(c, 1775), zann_str_1);
        advTriggerEffectActual(getPropertyObject2(c, 1777), zann_str_1);
        advTriggerEffectActual(getPropertyObject2(c, 1779), zann_str_1);
    }

    zann_resetCosts2(c);
}

/*__declspec(naked) void onNewFort() //00554B8A
{
    __asm
    {
        push    ebx
        push    3F800000h
        push    0ADh
        mov     eax, [ecx]
        call    dword ptr[eax + 80h]
        call    removeCheckResearchedOnce
        mov     ecx, [esi + 18h]
        push    2D6h             //research id
        mov     ecx, [ecx + 1D94h]
        //
        push    ecx
        //push    ecx
        //call    restest
        pop     ecx
        //
        mov     eax, 005BFEC9h   //researchTech
        push    00554B9Dh
        jmp     eax
    }
}*/

__declspec(naked) void onFortBuilt() //00554B81
{
    __asm
    {
        call    dword ptr[edx + 80h]
        mov     ecx, [esi + 18h]
        //mov     ecx, [ecx + 1D94h]
        push    ecx
        call    zannfort_onChange
        mov     eax, 00554B87h
        jmp     eax
    }
}

__declspec(naked) void onFortDestroyed() //00554D9C
{
    __asm
    {
        call    dword ptr [edx + 80h]
        mov     ecx, [esi + 18h]
        //mov     ecx, [ecx + 1D94h]
        push    ecx
        call    zannfort_onChange
        mov     eax, 00554DA2h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setZannFortHooks()
{
    setHook((void*)0x00554B81, onFortBuilt);
    setHook((void*)0x00554D9C, onFortDestroyed);
}
#pragma optimize( "", on )
