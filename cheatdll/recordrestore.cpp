#include "stdafx.h"

//dword_791200 = 006A35E0
//dword_7912A0 = 006A3684

//9B2 = 9AE
//9EC = 9E8
//10E0 = ???

__declspec(naked) void recrestore1() //0062005D
{
    __asm
    {
        push    2
        lea     edx, [ebx + 94h]
        mov     ecx, 006A3684h
        mov     ecx, [ecx]
        cmp     byte ptr [ecx + 9B2h], 0
        jz      loc_40D669
        mov     ecx, 006A35E0h
        mov     ecx, [ecx]
        mov     eax, [ecx + 10E0h]
        mov     ecx, 006A3684h
        mov     ecx, [ecx]
        mov     eax, [ecx + eax * 4 + 9ECh]
        mov     [edx], ax
loc_40D669:
        mov     eax, 00620065h
        jmp     eax
    }
}

__declspec(naked) void recrestore2() //005E904C
{
    __asm
    {
        mov     ebx, 006A3684h
        mov     ebx, [ebx]
        mov     eax, [ebx+1B4h]
        mov     ecx, [eax+4]
        test    ecx, ecx
        jz      short loc_7CA850
        mov     eax, [ecx]
        shr     eax, 4
        and     al, 1
        mov     [ebx+9D4h], al            //MIGHT BE WRONG: 9D8 -> 9D4
loc_7CA850:
        push    006899A8h
        mov     eax, 005E9051h
        jmp     eax
    }
}

__declspec(naked) void recrestore3() //005EBF5D
{
    __asm
    {
        mov     ecx, 006A3684h
        mov     ecx, [ecx]
        mov     eax, 005ED410h
        call    eax
        cmp     edi, ebx
        jz      loc_44250A
        mov     ecx, edi
        mov     eax, 005EBF63h
        jmp     eax
loc_44250A:
        mov     eax, 005EBFAFh
        jmp     eax
    }
}

__declspec(naked) void recrestore4() //005EC198
{
    __asm
    {
        mov     ecx, 006A3684h
        mov     ecx, [ecx]
        mov     eax, 005ED410h
        call    eax
        mov     ecx, [ebp + 1B4h]
        mov     eax, 005EC19Eh
        jmp     eax
    }
}

__declspec(naked) void recrestore5() //005F00E5
{
    __asm
    {
        mov     eax, 005E8E30h
        call    eax
        mov     ecx, 006A35E0h
        mov     ecx, [ecx]
        push    2
        mov     eax, 0043AA10h
        call    eax
        mov     eax, 005F00EAh
        jmp     eax
    }
}

__declspec(naked) void recrestore6() //005F05F4
{
    __asm
    {
        mov     eax, 005E8E30h
        call    eax
        mov     ecx, 006A35E0h
        mov     ecx, [ecx]
        push    2
        mov     eax, 0043AA10h
        call    eax
        mov     eax, 005F05F9h
        jmp     eax
    }
}

__declspec(naked) void recrestore7() //005BB42F
{
    __asm
    {
        mov     ecx, 006A3684h
        mov     ecx, [ecx]
        mov     eax, 0042C300h
        call    eax
        test    eax, eax
        jz      loc_467827
        push    844h
        mov     eax, 005BB434h
        jmp     eax
loc_467827:
        mov     ecx, 005BB74Ah
        jmp     ecx
    }
}

__declspec(naked) void recrestore8() //0050A4ED
{
    __asm
    {
        mov     [esi + 800h], ebx
        mov     [esi + 804h], ebx
        mov     [esi + 808h], ebx
        mov     eax, 0050A4F3h
        jmp     eax
    }
}

__declspec(naked) void recrestore9() //0050A6AB
{
    __asm
    {
        //vtable + 0Ch
        mov     eax, [esi]
        push    0
        push    0
        push    10h
        push    10h
        push    2Fh
        push    1E0h
        lea     ecx, [esi + 804h]
        push    ecx
        push    esi
        mov     ecx, esi
        call    dword ptr [eax + 11Ch]
        test    eax, eax
        jz      loc_512ED1
        mov     edx, [esi]
        push    1
        push    0
        push    0
        push    0Bh
        push    26h
        push    9Eh
        push    32h
        push    1F0h
        push    34E6h
        lea     eax, [esi + 808h]
        push    eax
        push    esi
        mov     ecx, esi
        call    dword ptr [edx + 130h]
        test    eax, eax
        jz      loc_512ED1
        mov     ecx, [esi + 7F8h]
        mov     eax, 0050A6B1h
        jmp     eax
loc_512ED1:
        mov     eax, 0050A796h
        jmp     eax
    }
}

__declspec(naked) void recrestore10() //0050A855
{
    __asm
    {
        push    edi
        mov     edi, 00428520h
        push    eax
        call    edi
        lea     eax, [esi + 804h]
        mov     ecx, esi
        push    eax
        call    edi
        lea     eax, [esi + 808h]
        mov     ecx, esi
        push    eax
        call    edi
        pop     edi
        mov     eax, 0050A85Bh
        jmp     eax
    }
}

__declspec(naked) void recrestore11() //0050AC47
{
    __asm
    {
        mov     ecx, [ebx + 804h]
        mov     eax, 004C5280h
        call    eax
        mov     ecx, 006A3684h
        mov     ecx, [ecx]
        mov     [ecx + 9D4h], al            //9D8 -> 9D4
        mov     ecx, [ebx + 7F0h]
        mov     eax, 0050AC4Dh
        jmp     eax
    }
}

__declspec(naked) void recrestore12() //0042EAA8
{
    __asm
    {
        mov     ecx, 006A3684h
        mov     ecx, [ecx]
        cmp     byte ptr [ecx+9AEh], 0
        jnz     short loc_7CA988
        movsx   eax, word ptr [eax+94h]
        jmp     loc_5E036C
loc_7CA988:
        mov     eax, 006A35E0h
        mov     eax, [eax]
        mov     edx, [eax+10E0h]
        mov     eax, [ecx+edx*4+9E8h]
loc_5E036C:
        mov     ecx, 0042EAB3h
        jmp     ecx
    }
}

#pragma optimize( "s", on )
void setRecordRestoreHooks()
{
    writeByte(0x005BB4D4, 2);
    writeDword(0x005109B3, 0x80C);
    writeDword(0x004FB439, 0x80C);

    setHook((void*)0x0062005D, recrestore1);
    setHook((void*)0x005E904C, recrestore2);
    setHook((void*)0x005EBF5D, recrestore3);
    setHook((void*)0x005EC198, recrestore4);
    setHook((void*)0x005F00E5, recrestore5);
    setHook((void*)0x005F05F4, recrestore6);
    setHook((void*)0x005BB42F, recrestore7);
    setHook((void*)0x0050A4ED, recrestore8);
    setHook((void*)0x0050A6AB, recrestore9);
    setHook((void*)0x0050A855, recrestore10);
    setHook((void*)0x0050AC47, recrestore11);
    setHook((void*)0x0042EAA8, recrestore12);
}
#pragma optimize( "", on )
