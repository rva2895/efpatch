#include "stdafx.h"
#include "network.h"
#include <MMSystem.h>

__declspec(naked) void network1() //00431D10, also check
{
    __asm
    {
        push    esi
        mov     esi, ecx
        mov     eax, [ecx]
        test    eax, eax
        jz      short loc_5D2004
        push    eax
        mov     eax, 00632B42h
        call    eax
        xor     eax, eax
        add     esp, 4
loc_5D2004:
        mov     [esi + 8], eax
        mov     [esi + 4], eax
        mov     [esi + 0Ch], eax
        mov     [esi], eax
        pop     esi
        ret
    }
}

__declspec(naked) void network2() //004352F6
{
    __asm
    {
        cmp     dword ptr [ebp + 1E3Ch], 4
        jge     short loc_5D42DF
        cmp     al, 58h
        jz      short loc_5D42E3
        cmp     al, 43h
        jnz     short loc_5D42C2
        mov     dl, [ebx + 1Ch]
        and     dl, dl
        jnz     short loc_5D42DF
loc_5D42C2:
        pop     edi
        pop     esi
        pop     ebp
        xor     eax, eax
        pop     ebx
        add     esp, 3A4h
        ret     14h
loc_5D42DF:
        cmp     al, 58h
        jnz     short loc_5D4301
loc_5D42E3:
        mov     eax, 0043533Bh
        jmp     eax
loc_5D4301:
        mov     eax, 00435359h
        jmp     eax
    }
}

__declspec(naked) void network3() //004353D5
{
    __asm
    {
        push    esi
        push    edx
        mov     edx, [ebx + 8]
        push    eax
        push    ecx
        push    ebx
        push    edx
        mov     ecx, ebp
        mov     eax, 00438A00h
        call    eax
        pop     edi
        pop     esi
        pop     ebp
        mov     eax, 1
        pop     ebx
        add     esp, 3A4h
        ret     14h
    }
}

__declspec(naked) void sub_5D96E0()
{
    __asm
    {
        mov     eax, [esp + 4]
        cmp     eax, 1
        jb      short loc_5D970F
        xor     edx, edx
        mov     dx, [ecx + 12DCh]
        cmp     eax, edx
        ja      short loc_5D970F
        mov     eax, [ecx + eax * 4 + 1FB8h]
        cmp     eax, 2
        jz      short loc_5D9707
        cmp     eax, 5
        jnz     short loc_5D970F
loc_5D9707:
        mov     eax, 1
        ret     4
loc_5D970F:
        xor     eax, eax
        ret     4
    }
}

__declspec(naked) void network4() //00438690
{
    __asm
    {
        sub     esp, 8
        push    ebx
        push    ebp
        push    esi
        mov     eax, [ecx + 10E4h]
        push    edi
        test    eax, eax
        jz      short loc_5D6D01
        mov     ebx, 1
        mov     esi, ecx
        cmp     [ecx + 12DCh], bx
        jnb     short loc_5D6D09
loc_5D6D01:
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        add     esp, 8
        ret
loc_5D6D09:
        push    ebx
        mov     ecx, esi
        call    sub_5D96E0
        test    eax, eax
        jz      short loc_5D6D73
        cmp     ebx, [esi + 10E0h]
        jz      short loc_5D6D73
        lea     eax, [ebx - 1]
        mov     ecx, [esi + 1DE0h]
        shl     eax, 10h
        lea     eax, [eax + eax * 2]
        mov     edx, [esi + ebx * 4 + 1DE4h]
        add     ecx, eax
        lea     eax, [edx + 1]
        mov     [esp + 14h], ecx
        mov     ebp, eax
loc_5D6D3E:
        and     eax, 1FFFh
        mov     edx, [esp + 14h]
        shl     eax, 3
        lea     eax, [eax + eax * 2]
        add     eax, edx
        cmp     [eax + 4], ebp
        jnz     short loc_5D6D73
        mov     edi, eax
        mov     edx, [eax + 10h]
        mov     eax, [edi + 0Ch]
        mov     ecx, [edi + 8]
        push    1
        push    edx
        mov     edx, [edi]
        push    eax
        push    edx
        push    ecx
        mov     ecx, esi
        mov     eax, 004340D0h
        call    eax
        inc     ebp
        mov     eax, ebp
        jmp     short loc_5D6D3E
loc_5D6D73:
        inc     ebx
        cmp     [esi + 12DCh], bx
        jnb     short loc_5D6D09
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        add     esp, 8
        retn
    }
}

__declspec(naked) void network5() //00432437
{
    __asm
    {
        cmp     esi, 180000h
        mov     [edi + 4], ebx
        mov     [edi + 8], ebx
        mov     [edi + 0Ch], ebx
        mov     [edi + 10h], ebx
        mov     [edi + 14h], ebx
        jb      short loc_43241B
        xor     esi, esi
        mov     eax, 0043244Dh
        jmp     eax
loc_43241B:
        mov     eax, 0043241Bh
        jmp     eax
    }
}

__declspec(naked) void network6() //00434603
{
    __asm
    {
        //cmp     edi, 17FFE8h
        cmp     edi, 180000h
        mov     [esi + 4], ebx
        mov     [esi + 8], ebx
        mov     [esi + 10h], ebx
        mov     [esi + 14h], ebx
        jb      short loc_4345E5
        xor     ebx, ebx
        mov     eax, 00434619h
        jmp     eax
loc_4345E5:
        mov     eax, 004345E5h
        jmp     eax
    }
}

__declspec(naked) void network7() //004352D3
{
    __asm
    {
        mov     [ebx + 0Ch], ecx
        mov     [ebx + 10h], ecx
        mov     [ebx + 14h], ecx
        mov     eax, 004352D9h
        jmp     eax
    }
}

__declspec(naked) void network8() //0043891B
{
    __asm
    {
        cmp     ebx, 30000h
        mov     [esi + 4], ebp
        mov     [esi + 8], ebp
        mov     [esi + 0Ch], ebp
        mov     [esi + 10h], ebp
        mov     [esi + 14h], ebp
        mov     eax, 0043892Dh
        jmp     eax
    }
}

__declspec(naked) void network9() //004389BB
{
    __asm
    {
        mov     [esi + 0Ch], ebp
        mov     [esi + 10h], ebp
        mov     [esi + 14h], ebp
        mov     eax, 004389C1h
        jmp     eax
    }
}

__declspec(naked) void network10() //0043CE2D
{
    __asm
    {
        //cmp     ebp, 30000h
        cmp     ebp, 20000h
        mov     [edi + 4], ebx
        mov     [edi + 8], ebx
        mov     [edi + 0Ch], ebx
        mov     [edi + 10h], ebx
        mov     [edi + 14h], ebx
        mov     eax, 0043CE3Fh
        jmp     eax
    }
}

__declspec(naked) void network11() //00438A00
{
    __asm
    {
        push    esi
        mov     eax, [ecx + 10E4h]
        push    edi
        test    eax, eax
        jnz     short loc_5D7091
        pop     edi
        pop     esi
        ret     18h
loc_5D7091:
        mov     eax, [esp + 20h]
        mov     edi, ecx
        dec     eax
        shl     eax, 10h
        mov     edx, [edi + 1DE0h]
        lea     ecx, [eax + eax * 2]
        mov     eax, [esp + 0Ch]
        add     edx, ecx
        mov     esi, eax
        and     eax, 1FFFh
        push    ebx
        shl     eax, 3
        lea     ebx, [eax + eax * 2]
        mov     eax, [esp + 1Ch]
        add     ebx, edx
        mov     edx, [esp + 20h]
        mov     [ebx + 0Ch], eax
        mov     eax, [esp + 18h]
        mov     [ebx + 10h], edx
        mov     ecx, eax
        mov     [ebx + 4], esi
        mov     [ebx + 8], eax
        test    eax, eax
        jz      short loc_5D7117
        mov     edx, [ebx + 14h]
        cmp     eax, edx
        jle     short loc_5D7103
        mov     ecx, [ebx]
        push    eax
        and     ecx, ecx
        jz      short loc_5D70EF
        push    ecx
        mov     eax, 00632B42h
        call    eax
        add     esp, 4
loc_5D70EF:
        mov     eax, 00632B9Dh
        call    eax
        add     esp, 4
        mov     ecx, [ebx + 8]
        mov     [ebx], eax
        test    eax, eax
        jz      short loc_5D7117
        mov     [ebx + 14h], ecx
loc_5D7103:
        mov     edi, [ebx]
        mov     esi, [esp + 14h]
        mov     edx, ecx
        shr     ecx, 2
        rep movsd
        mov     ecx, edx
        and     ecx, 3
        rep movsb
loc_5D7117:
        pop     ebx
        pop     edi
        pop     esi
        ret     18h
    }
}

__declspec(naked) void network12() //00438B00
{
    __asm
    {
        push    esi
        push    edi
        mov     eax, [ecx + 10E4h]
        mov     edi, ecx
        test    eax, eax
        jnz     short loc_5D7193
        pop     edi
        pop     esi
        ret     10h
loc_5D7193:
        mov     eax, [esp + 0Ch]
        push    ebx
        mov     esi, eax
        and     eax, 1FFFh
        mov     ebx, [edi + 1DDCh]
        shl     eax, 4
        add     ebx, eax
        mov     eax, [esp + 18h]
        mov     [ebx + 4], esi
        mov     ecx, eax
        mov     [ebx + 8], eax
        test    eax, eax
        jz      short loc_5D71F9
        mov     edx, [ebx + 0Ch]
        cmp     eax, edx
        jle     short loc_5D71E5
        mov     ecx, [ebx]
        push    eax
        and     ecx, ecx
        jz      short loc_5D71D1
        push    ecx
        mov     eax, 00632B42h
        call    eax
        add     esp, 4
loc_5D71D1:
        mov     eax, 00632B9Dh
        call    eax
        add     esp, 4
        mov     ecx, [ebx + 8]
        mov     [ebx], eax
        test    eax, eax
        jz      short loc_5D71F9
        mov     [ebx + 0Ch], ecx
loc_5D71E5:
        mov     edi, [ebx]
        mov     esi, [esp + 14h]
        mov     edx, ecx
        shr     ecx, 2
        rep movsd
        mov     ecx, edx
        and     ecx, 3
        rep movsb
loc_5D71F9:
        pop     ebx
        pop     edi
        pop     esi
        ret     10h
    }
}

__declspec(naked) void sub_5D3540()
{
    __asm
    {
        mov     edx, [ecx + 4710h]
        xor     eax, eax
        test    edx, edx
        jnz     short locret_5D3570
        mov     edx, [esp + 14h]
        //mov     eax, dword_790284
        mov     eax, 006A376Ch
        mov     eax, [eax]
        push    edx
        mov     edx, [esp + 14h]
        push    edx
        mov     edx, [esp + 14h]
        mov     ecx, [eax]
        push    edx
        mov     edx, [esp + 14h]
        push    edx
        mov     edx, [esp + 14h]
        push    edx
        push    eax
        call    dword ptr [ecx + 68h]
locret_5D3570:
        retn    14h
    }
}

__declspec(naked) void network13() //00438DD0
{
    __asm
    {
        sub     esp, 10h
        push    ebx
        push    ebp
        push    esi
        mov     esi, ecx
        mov     eax, [ecx + 10E4h]
        push    edi
        test    eax, eax
        jz      short loc_5D74E5
        mov     ebx, 1
        lea     edi, [esp + 18h]
        cmp     [esi + 12DCh], bx
        jnb     short loc_5D74ED
loc_5D74E5:
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        add     esp, 10h
        ret
loc_5D74ED:
        mov     dword ptr [edi], 58h
        call    ds:timeGetTime
        mov     [esp + 10h], eax
loc_5D74FD:
        push    ebx
        mov     ecx, esi
        call    sub_5D96E0
        test    eax, eax
        jz      short loc_5D756A
        cmp     ebx, [esi + 10E0h]
        jz      short loc_5D756A
        mov     edx, [esi + ebx * 4 + 1DE4h]
        mov     ecx, [esi + ebx * 4 + 13E8h]
        lea     ebp, [edx + 1]
        mov     eax, [esp + 10h]
        cmp     ebp, ecx
        jnz     short loc_5D753A
        mov     ecx, eax
        mov     edx, [esi + ebx * 4 + 1410h]
        sub     ecx, edx
        cmp     ecx, 64h
        jb      short loc_5D756A
loc_5D753A:
        mov     [esi + ebx * 4 + 13E8h], ebp
        //mov     ecx, dword_790F94
        mov     ecx, 6A4484h
        mov     ecx, [ecx]
        mov     [esi + ebx * 4 + 1410h], eax
        and     ecx, ecx
        jz      short loc_5D7574
        mov     [edi + 4], ebp
        push    8
        mov     eax, [esi + ebx * 4 + 1E48h]
        push    edi
        push    0
        push    eax
        push    ecx
        mov     ecx, esi
        call    sub_5D3540
loc_5D756A:
        inc     ebx
        cmp     [esi + 12DCh], bx
        jnb     short loc_5D74FD
loc_5D7574:
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        add     esp, 10h
        retn
    }
}

__declspec(naked) void network14() //004391F0
{
    __asm
    {
        push    ebx
        push    ebp
        push    esi
        mov     ebx, [esp + 14h]
        push    edi
        xor     edi, edi
        mov     esi, ecx
        cmp     [ecx + ebx * 4 + 1E48h], edi
        jz      short loc_5D789D
        //cmp     dword_790F94, edi
        mov     eax, 6A4484h
        mov     eax, [eax]
        cmp     eax, edi
        jnz     short loc_5D78A4
loc_5D789D:
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        ret     8
loc_5D78A4:
        call    ds:timeGetTime
        mov     edx, [esp + 14h]
        mov     ecx, [esi + ebx * 4 + 1438h]
        cmp     edx, 64h
        jl      short loc_5D7938
        lea     edi, [edx + 2]
        cmp     edx, ecx
        jg      short loc_5D78D1
        mov     ebp, [esi + ebx * 4 + 14B4h]
        mov     ecx, eax
        sub     ecx, ebp
        cmp     ecx, 64h
        jb      short loc_5D7938
loc_5D78D1:
        mov     [esi + ebx * 4 + 1438h], edi
        mov     [esi + ebx * 4 + 14B4h], eax
        dec     edx
        mov     ebp, 2
loc_5D78E5:
        inc     edx
        mov     edi, edx
        and     edx, 1FFFh
        mov     eax, [esi + 1DDCh]
        shl     edx, 4
        add     eax, edx
        cmp     [eax + 4], edi
        jnz     short loc_5D793F
        //mov     ecx, dword_790F94
        mov     ecx, 6A4484h
        mov     ecx, [ecx]
        mov     edi, eax
        mov     edx, [eax + 8]
        mov     eax, [edi]
        push    edx
        mov     edx, [esi + ebx * 4 + 1E48h]
        push    eax
        push    0
        push    edx
        push    ecx
        mov     ecx, esi
        call    sub_5D3540
loc_5D791F:
        mov     edx, [edi + 8]
        add     [esi + 471Ch], edx
        test    eax, eax
        jnz     short loc_5D7932
        add     [esi + 12C4h], edx
loc_5D7932:
        mov     edx, [edi + 4]
        dec     ebp
        jg      short loc_5D78E5
loc_5D7938:
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        retn    8
loc_5D793F:
        dec     edi
        mov     [esi + ebx * 4 + 1438h], edi
        pop     edi
loc_5D7948:
        pop     esi
        pop     ebp
        pop     ebx
        retn    8
    }
}

__declspec(naked) void network15() //004398A8
{
    __asm
    {
        mov     ecx, ebp
        mov     eax, 00438B00h
        call    eax
        mov     eax, 00439986h
        jmp     eax
    }
}

__declspec(naked) void network16() //00439986
{
    __asm
    {
        mov     ecx, [ebp + 133Ch]
        inc     ecx
        cmp     [esp + 38h], 3Eh
        mov     [ebp + 133Ch], ecx
        jnz     short loc_5D7FAC
        mov     ecx, [esp + 18h]
        mov     eax, [ebp + 10E0h]
        push    0FFFFFFFFh
        push    0
        mov     dx, [ecx + 1]
        mov     ecx, 004399ACh
        jmp     ecx
loc_5D7FAC:
        mov     eax, 004399C8h
        jmp     eax
    }
}

__declspec(naked) void network17() //004388F0
{
    __asm
    {
        push    ebx
        push    ebp
        push    esi
        push    edi
        mov     edi, ecx
        xor     ebp, ebp
        xor     ebx, ebx
loc_5D6F2A:
        mov     esi, [edi + 1DE0h]
        add     esi, ebx
        mov     eax, [esi]
        cmp     eax, ebp
        jz      short loc_5D6F41
        push    eax
        mov     eax, 00632B42h
        call    eax
        add     esp, 4
loc_5D6F41:
        add     ebx, 18h
        mov     [esi], ebp
        cmp     ebx, 17FFE8h
        mov     [esi + 4], ebp
        mov     [esi + 8], ebp
        mov     [esi + 0Ch], ebp
        mov     [esi + 10h], ebp
        mov     [esi + 14h], ebp
        jbe     short loc_5D6F2A
        xor     ebx, ebx
loc_5D6F5F:
        mov     esi, [edi + 1DDCh]
        add     esi, ebx
        mov     eax, [esi]
        cmp     eax, ebp
        jz      short loc_5D6F76
        push    eax
        mov     eax, 00632B42h
        call    eax
        add     esp, 4
loc_5D6F76:
        add     ebx, 10h
        mov     [esi], ebp
        cmp     ebx, 1FFF0h
        mov     [esi + 4], ebp
        mov     [esi + 8], ebp
        mov     [esi + 0Ch], ebp
        jbe     short loc_5D6F5F
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        ret
    }
}

__declspec(naked) void network18() //00432469
{
    __asm
    {
        mov     [edi], ebx
        mov     [edi + 4], ebx
        mov     [edi + 8], ebx
        mov     [edi + 0Ch], ebx
        add     esi, 10h
        cmp     esi, 20000h
        mov     ecx, 00432487h
        jmp     ecx
    }
}

__declspec(naked) void network19() //0043463B
{
    __asm
    {
        add     ebx, 10h
        mov     [esi], eax
        mov     [esi + 4], eax
        mov     [esi + 8], eax
        mov     [esi + 0Ch], eax
        cmp     ebx, 20000h
        mov     ecx, 00434659h
        jmp     ecx
    }
}

__declspec(naked) void network20() //00432457
{
    __asm
    {
        mov     eax, [edi]
        cmp     eax, ebx
        mov     ecx, 0043245Eh
        jmp     ecx
    }
}

__declspec(naked) void network21() //00434623
{
    __asm
    {
        mov     eax, [esi]
        test    eax, eax
        mov     ecx, 0043462Eh
        jmp     ecx
    }
}

__declspec(naked) void __stdcall sub_5D6FB0(int)
{
    __asm
    {
        push    ebx
        push    ebp
        push    esi
        push    edi
        mov     eax, [esp + 14h]
        xor     ebp, ebp
        dec     eax
        shl     eax, 10h
        lea     eax, [eax + eax * 2]
        mov     ebx, [ecx + 1DE0h]
        xor     edi, edi
        add     ebx, eax
loc_5D6FCB:
        lea     esi, [ebx + edi]
        mov     eax, [esi]
        cmp     eax, ebp
        jz      short loc_5D6FDD
        push    eax; void *
        mov     eax, 00632B42h
        call    eax
        add     esp, 4
loc_5D6FDD:
        mov     [esi], ebp
        mov     [esi + 4], ebp
        mov     [esi + 8], ebp
        mov     [esi + 0Ch], ebp
        mov     [esi + 10h], ebp
        mov     [esi + 14h], ebp
        add     edi, 18h
        cmp     edi, 30000h
        jb      short loc_5D6FCB
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        ret     4
    }
}

__declspec(naked) void network22() //00438980
{
    __asm
    {
        jmp     sub_5D6FB0
    }
}

__declspec(naked) void network23() //004352A0
{
    __asm
    {
        push    esi
        call    sub_5D6FB0
        mov     ecx, 004352E4h
        jmp     ecx
    }
}

//setHook((void*)0x00434623, network21);

#pragma optimize( "s", on )
void setNetworkHooks()
{
    writeDword(0x004473AC, 0xFFFFFA24);
    writeDword(0x004473C3, 0x7D0);
    writeDword(0x00442D1C, 0xC044);

    writeDword(0x00431673, 0x20004);
    writeByte(0x00431698, 0x24);
    writeDword(0x004316A7, 0x2000);
    writeByte(0x004316AC, 0x10);
    writeDword(0x004316B0, 0x2000);
    writeDword(0x004316B9, 0x90666666);

    writeDword(0x004316BE, 0x180004);
    writeByte(0x00431698, 0x24);
    writeDword(0x004316F2, 0x10000);
    writeByte(0x004316F7, 0x18);
    writeDword(0x004316FB, 0x10000);
    writeDword(0x00431704, 0x90666666);

    //8B C1 31 C9 89 08 89 48  04 89 48 08 89 48 0C C3
    writeDword(0x00431CF0, 0xC931C18B);
    writeDword(0x00431CF4, 0x48890889);
    writeDword(0x00431CF8, 0x08488904);
    writeDword(0x00431CFC, 0xC30C4889);

    //89 7E 14 5F 5E C3
    writeDword(0x00431D83, 0x5F147E89);
    writeDword(0x00431D87, 0x9090C35E);

    writeWord(0x00431FA3, 0x55EB);
    writeByte(0x00432011, 0x10);

    writeWord(0x00434A60, 0x26EB);

    writeDword(0x00435396, 0x90909090);
    writeDword(0x0043539A, 0x90909090);
    writeByte(0x0043539E, 0x90);

    writeWord(0x004353B1, 0x6C76);

    writeWord(0x00435466, 0x56EB);

    writeByte(0x00435874, 0x38);

    //writeDword(0x00431B0D, 0x30000); 
    writeDword(0x00431B0D, 0x17FFE8 + 0x18);
    ///writeDword(0x004352DE, 0x30000);
    ///writeDword(0x004352DE, 0x17FFE8 + 0x18);
    ///writeDword(0x004389C6, 0x30000);
    ///writeDword(0x004389C6, 0x17FFE8 + 0x18);
    //writeDword(0x0043A3DA, 0x30000);
    writeDword(0x0043A3DA, 0x17FFE8 + 0x18);
    
    //
    writeByte(0x00431B26, 0x10);
    writeDword(0x00431B29, 0x1FFF0/* + 0x10*/);

    writeByte(0x0043A3F1, 0x10);
    writeDword(0x0043A3F4, 0x1FFF0/* + 0x10*/);

    writeByte(0x0043CE54, 0x10);
    writeDword(0x0043CE57, 0x1FFF0/* + 0x10*/);
    //

    writeDword(0x004389E0, 0x90C3C031);

    writeWord(0x004373A6, 0x1EEB); //!!!!!!!!!!!!!!!!!!!!!!!
    writeWord(0x004373A6, 0x1BEB);

    writeWord(0x00438C10, 0x08C2);
    writeByte(0x00438C12, 0x00);

    writeByte(0x00438CE0, 0xC3);

    writeByte(0x00439880, 1);
    
    writeDword(0x004398AF, 0x90909090);
    writeWord(0x004398B3, 0x9090);
    writeDword(0x004398B9, 0x90909090);
    writeDword(0x004398BD, 0x90909090);
    writeByte(0x004398C1, 0x90);

    writeDword(0x0043C3AD, 0xC044);
    writeDword(0x0043C3B7, 0xC044);

    setHook((void*)0x00431D10, network1);
    setHook((void*)0x004352F6, network2);
    setHook((void*)0x004353D5, network3);
    setHook((void*)0x00438690, network4);
    setHook((void*)0x00432437, network5);
    setHook((void*)0x00434603, network6);
    //setHook((void*)0x004352D3, network7);
    //setHook((void*)0x0043891B, network8);
    //setHook((void*)0x004389BB, network9);
    setHook((void*)0x0043CE2D, network10);
    setHook((void*)0x00438A00, network11);
    setHook((void*)0x00438B00, network12);
    setHook((void*)0x00438DD0, network13);
    setHook((void*)0x004391F0, network14);
    setHook((void*)0x004398A8, network15);
    setHook((void*)0x00439986, network16);
    setHook((void*)0x004388F0, network17);
    setHook((void*)0x00432469, network18);
    //
    setHook((void*)0x0043463B, network19);
    setHook((void*)0x00432457, network20);
    //
    setHook((void*)0x00434623, network21);
    //
    setHook((void*)0x00438980, network22);
    setHook((void*)0x004352A0, network21);


    //vote panel
    //writeWord(0x0046EFCE, 0x9090);
    //writeWord(0x0046F000, 0x9090);
    //writeWord(0x0046EF0B, 0x9066);
}
#pragma optimize( "", on )

//check stack of sub 435550
