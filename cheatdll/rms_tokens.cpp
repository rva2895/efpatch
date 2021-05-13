#include "stdafx.h"
#include "rms_tokens.h"

const char cliff_type_str[] = "cliff_type";
const char resource_delta_str[] = "resource_delta";

__declspec(naked) void onRmsCliffTokenRegister() //004E1096
{
    __asm
    {
        mov     eax, 004E1D40h
        call    eax
        //register cliff type
        xor     ecx, ecx
        push    ecx
        push    ecx
        push    ecx
        push    2
        push    RMS_CLIFF_TYPE_ID
        push    ecx
        push    offset cliff_type_str
        mov     ecx, esi
        mov     eax, 004E1D40h
        call    eax
        //resource_delta
        //xor     ecx, ecx
        //push    ecx
        //push    ecx
        //push    ecx
        //push    2
        //push    RMS_OBJECT_RESOURCE_DELTA
        //push    ecx
        //push    offset resource_delta_str
        //mov     ecx, esi
        //mov     eax, 004E1D40h
        //call    eax
        mov     eax, 004E109Bh
        jmp     eax
    }
}

void setRmsTokenHooks()
{
    setHook((void*)0x004E1096, onRmsCliffTokenRegister);
}
