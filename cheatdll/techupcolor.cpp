#include "stdafx.h"

#include "techupcolor.h"

#pragma warning(push)
#pragma warning(disable:4100)
__declspec(naked) void __stdcall sendTechChat2(int player, char* s, int unk)
{
    __asm
    {
        mov     eax, [esp + 0Ch]    //unk
        mov     ecx, [esp + 8]    //str
        mov     edx, [esp + 4]  //player
        push    0
        push    eax
        push    edx
        push    ecx
        mov     ecx, 006A35D8h
        mov     ecx, [ecx]
        push    0
        mov     eax, 0042D5E0h
        call    eax
        retn    0Ch
    }
}
#pragma warning(pop)

__declspec(naked) void onTechUpSend() //005E9990
{
    __asm
    {
        //mov     edx, [esi + 420h]
        //mov     eax, [edx + 4Ch]
        //mov     ecx, [eax + edi * 4]
        //mov     eax, [ecx + 0A0h]
        //push    eax
        //
        push    edi
        //
        call    sendTechChat2
        mov     eax, 005E9995h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setTechUpColorHooks()
{
    setHook((void*)0x005E9990, onTechUpSend);
}
#pragma optimize( "", on )
