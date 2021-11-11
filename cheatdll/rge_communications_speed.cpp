#include "stdafx.h"
#include "rge_communications_speed.h"

__declspec(naked) void onBufferTimeToUse() //00447B79
{
    __asm
    {
        sub     ecx, edx
        inc     ecx
        imul    ecx, eax
        push    00447B7Eh
        ret
    }
}

int dword_7A5390[9] = { 0 };
int dword_7A53B4 = 0;
int dword_7A53B8 = 0;

__declspec(naked) void onAnalyzeGameSpeed() //00448036
{
    __asm
    {
        mov     edx, [esi + 11Ch]
        cmp     eax, 5
        jnb     short loc_7EA902
        push    0044803Bh
        ret

loc_7EA902:
        mov     ebx, 11h
        mov     edi, 1
        mov     ebp, ebx
        add     esi, 1E8h

loc_7EA914:
        mov     eax, [esp + 10h]
        push    edi; PlayerNo
        mov     ecx, [eax + 108h]; this
        mov     eax, 0043B000h
        call    eax //TCommunications_Handler::IsPlayerHuman(uint)
        test    eax, eax
        jz      short loc_7EA939
        mov     eax, [esi - 28h]
        cmp     ebx, eax
        ja      short loc_7EA931
        mov     ebx, eax

loc_7EA931:
        mov     eax, [esi]
        cmp     ebp, eax
        ja      short loc_7EA939
        mov     ebp, eax

loc_7EA939:
        inc     edi
        add     esi, 4
        cmp     edi, 9
        jle     short loc_7EA914
        mov     edx, 1Eh
        xor     ecx, ecx

loc_7EA949:
        cmp     ebp, edx
        jbe     short loc_7EA956
        inc     ecx
        add     edx, 1Eh
        cmp     ecx, 8
        jl      short loc_7EA949

loc_7EA956:
        mov     eax, dword_7A5390[ecx * 4]
        shr     edx, 2
        inc     eax
        cmp     edx, 64h; 'd'
        jbe     short loc_7EA96B
        mov     edx, 64h; 'd'

loc_7EA96B:
        mov     dword_7A5390[ecx * 4], eax
        mov     dword_7A53B4, ebp
        mov     dword_7A53B8, edx
        mov     esi, ebx
        cmp     esi, 96h; '–'
        jbe     short loc_7EA98D
        mov     esi, 96h; '–'

loc_7EA98D:
        mov     ecx, [esp + 10h]
        mov     eax, esi
        mov     ecx, [ecx + 12Ch]
        sub     eax, ecx
        cdq
        xor     eax, edx
        sub     eax, edx
        mov     eax, ebp
        cmp     esi, 11h
        jnb     short loc_7EA9AC
        mov     esi, 11h

loc_7EA9AC:
        add     eax, dword_7A53B8
        xor     edx, edx
        div     esi
        xor     edx, edx
        mov     edi, eax
        mov     eax, 7D0h
        div     esi
        xor     edx, edx
        mov     ecx, eax
        mov     eax, 11h
        div     esi
        mov     [esp + 18h], ecx
        mov     ebx, eax
        inc     ebx
        cmp     ebx, 2
        ja      short loc_7EA9DD
        mov     ebx, 2

loc_7EA9DD:
        cmp     edi, ecx
        jb      short loc_7EA9E3
        mov     edi, ecx

loc_7EA9E3:
        cmp     edi, ebx
        ja      short loc_7EA9E9
        mov     edi, ebx

loc_7EA9E9:
        push    00448104h
        ret
    }
}

void setRGECommunicationsSpeedHooks()
{
    writeByte(0x00448129, 0xEB);
    writeByte(0x00447A13, 0x11);
    writeByte(0x00447947, 0x11);
    writeByte(0x0044794B, 0x11);
    writeByte(0x004474E4, 0x1E);

    setHook((void*)0x00447B79, onBufferTimeToUse);
    setHook((void*)0x00448036, onAnalyzeGameSpeed);
}
