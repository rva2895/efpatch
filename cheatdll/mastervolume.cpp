#include "stdafx.h"
#include "mastervolume.h"

__declspec(naked) void mastervolume1() //00545F22
{
    __asm
    {
        mov     edx, 006A3684h
        mov     edx, [edx]
        mov     ecx, [edx + 80h]
        mov     edx, [ecx + 1Ch]
        mov     ecx, 00545F28h
        jmp     ecx
    }
}

__declspec(naked) void mastervolume2() //00544930
{
    __asm
    {
        push    esi
        mov     eax, [esp + 8]
        mov     esi, ecx
        cmp     eax, 0FFFFECDCh
        jg      short loc_4D9EE3
        mov     eax, 0FFFFD8F0h

loc_4D9EE3:
        mov     edx, [esp + 0Ch]
        test    edx, edx
        jz      short loc_4D9EEE
        mov     [esi + 1Ch], eax

loc_4D9EEE:
        mov     ecx, [esi + 128h]
        test    ecx, ecx
        jz      short loc_4D9F29
        mov     ecx, [esi + 258h]
        mov     edx, [esi + 254h]
        sub     ecx, edx
        add     eax, 2710h
        imul    ecx, eax
        mov     eax, 0D1B71759h
        mul     ecx
        mov     eax, [esi + 12Ch]
        shr     edx, 0Dh
        mov     [esi + 29Ch], edx
        xor     eax, eax
        mov     [esi + 14h], eax

loc_4D9F29:
        pop     esi
        retn    8
    }
}

__declspec(naked) void mastervolume3() //005450E4
{
    __asm
    {
        call    dword ptr [edx + 34h]
        mov     ecx, 006A3684h
        mov     ecx, [ecx]
        mov     edx, [ecx + 80h]
        mov     eax, [edx + 1Ch]
        mov     edx, [esi + 1Ch]
        push    eax
        push    edx
        mov     ecx, [edx]
        call    dword ptr [ecx + 3Ch]
        mov     eax, [esi + 1Ch]
        mov     ecx, 005450EAh
        jmp     ecx
    }
}

__declspec(naked) void sub_4D8940()
{
    __asm
    {
        mov     eax, [ecx + 7Ch]
        test    eax, eax
        jz      short loc_4D8959
        mov     ecx, [esp + 4]

loc_4D894B:
        mov     edx, [eax + 8]
        cmp     [edx], ecx
        jz      short loc_4D895E
        mov     eax, [eax + 4]
        test    eax, eax
        jnz     short loc_4D894B

loc_4D8959:
        xor eax, eax
        retn    4

loc_4D895E:
        mov     eax, [eax + 8]
        retn    4
    }
}

__declspec(naked) void mastervolume4() //0050E199
{
    __asm
    {
        mov     eax, 00544930h
        call    eax
        push    edi
        mov     edi, 8

loc_7D005B:
        mov     edx, 006A3684h
        mov     edx, [edx]
        mov     ecx, [edx + 94h]
        test    ecx, ecx
        jz      short loc_7D00A2
        mov     eax, [edx + edi]
        push    eax
        call    sub_4D8940
        test    eax, eax
        jz      short loc_7D00A2
        mov     ecx, eax
        mov     eax, [ecx + 258h]
        test    eax, eax
        jz      short loc_7D00A2
        mov     edx, 006A3684h
        mov     edx, [edx]
        mov     ecx, [edx + 80h]
        mov     edx, [ecx + 1Ch]
        push    edx
        push    eax
        mov     ecx, [eax]
        call    dword ptr [ecx + 3Ch]
        add     edi, 4
        cmp     edi, 10h
        jnz     short loc_7D005B

loc_7D00A2:
        pop     edi
        mov     eax, 0050E19Eh
        jmp     eax
    }
}

void setMasterVolumeHooks()
{
    writeDword(0x00546010, 0x900008C2);
    writeByte(0x00546D32, 0x88);
    writeByte(0x00546FC3, 0x88);
    writeDword(0x005451B4, 0x900004C2);

    writeByte(0x0050BD1B, 0x10);
    writeByte(0x0050BD1D, 8);
    writeByte(0x0050BD25, 0x50);

    writeByte(0x0050BD46, 0xA);
    writeByte(0x0050BD48, 5);
    writeByte(0x0050BD50, 0x32);

    writeWord(0x004AD986, 0xE124);
    writeByte(0x004AD98C, 0x7F);

    writeByte(0x00459F3B, 0x10);
    writeByte(0x00459F3D, 8);
    writeByte(0x00459F45, 0x50);

    writeByte(0x0045A047, 0xA);
    writeByte(0x0045A049, 5);
    writeByte(0x0045A051, 0x32);

    setHook((void*)0x00545F22, mastervolume1);
    setHook((void*)0x00544930, mastervolume2);
    setHook((void*)0x005450E4, mastervolume3);
    setHook((void*)0x0050E199, mastervolume4);
}
