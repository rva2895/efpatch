#include "stdafx.h"
#include "rms_tokens.h"
#include "create_object.h"

int res_delta = 0;

__declspec(naked) void onCreateObjectScan() //004E39EB
{
    __asm
    {
        mov     esi, [esi + 68h]
        cmp     esi, RMS_OBJECT_RESOURCE_DELTA
        jnz     _not_res_delta
        mov     ebx, [ecx + 46678h]
        mov     res_delta, ebx
        mov     al, 1
        ret     4
_not_res_delta:
        add     esi, 0FFFFFFCFh
        mov     ecx, 004E39F1h
    }
}

void setRmsCreateObjectHooks()
{

}
