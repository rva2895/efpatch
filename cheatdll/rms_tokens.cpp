#include "stdafx.h"
#include "rms_tokens.h"

void __stdcall register_new_rms_tokens(RGE_RMM_Script_Controller* sc)
{
    RGE_RMM_Script_Controller__add_token(sc, "cliff_type", 0, RMS_CLIFF_TYPE_ID, 2, 0, 0, 0);
    //RGE_RMM_Script_Controller__add_token(sc, "base_elevation", 0, RMS_LAND_BASE_ELEV, 2, 0, 0, 0);
    //RGE_RMM_Script_Controller__add_token(sc, "base_elevation", 0, RMS_LAND_BASE_ELEV, 2, 0, 0, 0);
}

__declspec(naked) void onRmsCliffTokenRegister() //004E1096
{
    __asm
    {
        mov     eax, 004E1D40h
        call    eax

        push    esi
        call    register_new_rms_tokens

        mov     eax, 004E109Bh
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setRmsTokenHooks()
{
    setHook((void*)0x004E1096, onRmsCliffTokenRegister);
}
#pragma optimize( "", on )
