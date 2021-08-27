#include "stdafx.h"
#include "techtree.h"

extern int current_save_game_version;

__declspec(naked) void techs_size_1() //005C2B20
{
    __asm
    {
        xor     ecx, ecx
        push    0D4h
        mov     cx, [esi + 18h]
        push    ecx
        call    calloc_internal
        mov     [esi + 1Ch], eax
        mov     ax, [esi + 18h]
        add     esp, 8
        xor     ebp, ebp
        test    ax, ax
        mov     ecx, 005C2B3Dh
        jmp     ecx
    }
}

__declspec(naked) void tech_size_2() //005C2CA3
{
    __asm
    {
        mov     ax, [esi + 18h]
        add     ebx, 0D4h
        mov     edx, 005C2CACh
        jmp     edx
    }
}

__declspec(naked) void tech_size_3() //005C306F
{
    __asm
    {
        mov     ax, [esi + 18h]
        xor     ebp, ebp
        test    ax, ax
        mov     edx, 005C3076h
        jmp     edx
    }
}

__declspec(naked) void tech_size_4() //005C3189
{
    __asm
    {
        mov     ax, [esi + 18h]
        add     ebx, 0D4h
        mov     edx, 005C3192h
        jmp     edx
    }
}

__declspec(naked) void tech_size_5() //005C25F0
{
    __asm
    {
        mov     ax, [esi + 18h]
        xor     ebp, ebp
        cmp     ax, bx
        mov     edx, 005C25F7h
        jmp     edx
    }
}

__declspec(naked) void tech_size_6() //005C264F
{
    __asm
    {
        mov     dx, [esi + 18h]
        add     edi, 0D4h
        mov     eax, 005C2658h
        jmp     eax
    }
}

__declspec(naked) void tech_size_7() //005C16CE
{
    __asm
    {
        mov     [esi], al
        mov     [esi + 8], al
        mov     [esi + 10h], ax
        mov     [esi + 18h], ax
        mov     edx, 005C16DAh
        jmp     edx
    }
}

__declspec(naked) void tech_size_8() //005C1756
{
    __asm
    {
        mov     dx, [ebx + 18h]
        mov     [ebp + 18h], dx
        mov     eax, 005C175Ch
        jmp     eax
    }
}

__declspec(naked) void tech_size_9() //005C1724
{
    __asm
    {
        mov     [ebp + 10h], ax
        mov     [ebp + 18h], ax
        mov     edx, 005C172Bh
        jmp     edx
    }
}

__declspec(naked) void tech_size_10() //005C179F
{
    __asm
    {
        mov     dx, [ebp + 18h]
        mov     [ebp + 14h], eax
        mov     eax, 005C17A5h
        jmp     eax
    }
}

__declspec(naked) void tech_size_11() //005C1ACD
{
    __asm
    {
        mov     ax, [ebp + 18h]
        mov     edi, [ebp + 1Ch]
        mov     ecx, 005C1AD3h
        jmp     ecx
    }
}

__declspec(naked) void tech_size_12() //005C1AEE
{
    __asm
    {
        mov     ax, [ebp + 18h]
        xor     esi, esi
        test    ax, ax
        mov     ecx, 005C1AF5h
        jmp     ecx
    }
}

__declspec(naked) void tech_size_13() //005C1BED
{
    __asm
    {
        mov     cx, [ebp + 18h]
        inc     eax
        add     esi, 0D4h
        mov     edx, 005C1BF7h
        jmp     edx
    }
}

__declspec(naked) void tech_size_14() //005C32BC
{
    __asm
    {
        mov     bx, [ecx + 18h]
        mov     edi, ebx
        test    edi, edi
        mov     ebx, 005C32C3h
        jmp     ebx
    }
}

__declspec(naked) void tech_size_15() //005C3931
{
    __asm
    {
        mov     ax, [esi + 18h]
        mov     ebx, [edi + 1D94h]
        xor     ebp, ebp
        test    ax, ax
        mov     ecx, 005C393Eh
        jmp     ecx
    }
}

__declspec(naked) void tech_size_16() //005C3987
{
    __asm
    {
        add     edi, 0D4h
        mov     ax, [ecx + 18h]
        mov     esi, 005C3990h
        jmp     esi
    }
}

__declspec(naked) void tech_size_17() //005C3B07
{
    __asm
    {
        mov     dx, [esi + 18h]
        mov     edi, edx
        test    edi, edi
        mov     edx, 005C3B0Eh
        jmp     edx
    }
}

__declspec(naked) void tech_size_18() //005C3CCE
{
    __asm
    {
        mov     dx, [esi + 18h]
        mov     edi, edx
        test    edi, edi
        mov     edx, 005C3CD5h
        jmp     edx
    }
}

__declspec(naked) void tech_size_19() //005C3D4B
{
    __asm
    {
        mov     dx, [eax + 18h]
        xor     ecx, ecx
        test    dx, dx
        mov     edx, 005C3D52h
        jmp     edx
    }
}

__declspec(naked) void tech_size_20() //005C3E95
{
    __asm
    {
        mov     dx, [esi + 18h]
        mov     edi, edx
        test    edi, edi
        mov     edx, 005C3E9Ch
        jmp     edx
    }
}

__declspec(naked) void tech_size_21() //005C3EFA
{
    __asm
    {
        mov     dx, [eax + 18h]
        add     edi, 0D4h
        cmp     ecx, edx
        mov     [esp + 14h], ecx
        mov     ecx, 005C3F09h
        jmp     ecx
    }
}

__declspec(naked) void tech_size_22() //005C401A
{
    __asm
    {
        mov     ax, [ecx + 18h]
        xor     esi, esi
        cmp     ax, dx
        mov     eax, 005C4021h
        jmp     eax
    }
}

__declspec(naked) void tech_size_23() //005C403F
{
    __asm
    {
        xor     eax, eax
        mov     ax, [ecx + 18h]
        cmp     esi, eax
        mov     eax, 005C4046h
        jmp     eax
    }
}

__declspec(naked) void tech_size_24() //005C40E2
{
    __asm
    {
        mov     ax, [ecx + 18h]
        xor     esi, esi
        test    ax, ax
        mov     edx, 005C40E9h
        jmp     edx
    }
}

__declspec(naked) void tech_size_25() //005C40FF
{
    __asm
    {
        mov     ax, [ecx + 18h]
        add     edx, 0D4h
        cmp     esi, eax
        mov     eax, 005C410Ah
        jmp     eax
    }
}

__declspec(naked) void tech_size_26() //005C4150
{
    __asm
    {
        mov     cx, [esi + 18h]
        xor     eax, eax
        test    ecx, ecx
        mov     edx, 005C4157h
        jmp     edx
    }
}

__declspec(naked) void tech_size_27() //005C41AD
{
    __asm
    {
        mov     cx, [esi + 18h]
        xor     edi, edi
        mov     ebx, [eax]
        mov     eax, 005C41B4h
        jmp     eax
    }
}

__declspec(naked) void tech_size_28() //005C4219
{
    __asm
    {
        mov     cx, [esi + 18h]
        xor     eax, eax
        test    ecx, ecx
        mov     edx, 005C4220h
        jmp     edx
    }
}

__declspec(naked) void tech_size_29() //005C42D5
{
    __asm
    {
        mov     cx, [edx + 18h]
        xor     eax, eax
        test    ecx, ecx
        mov     esi, 005C42DCh
        jmp     esi
    }
}

__declspec(naked) void tech_size_30() //005C4384
{
    __asm
    {
        xor     ecx, ecx
        mov     cx, [edx + 18h]
        test    ecx, ecx
        mov     edi, 005C438Bh
        jmp     edi
    }
}

__declspec(naked) void tech_size_31() //005C4487
{
    __asm
    {
        xor     ecx, ecx
        mov     cx, [edi + 18h]
        test    ecx, ecx
        push    005C448Eh
        ret
    }
}

__declspec(naked) void tech_size_32() //005C452B
{
    __asm
    {
        mov     dx, [ebp + 18h]
        xor     edi, edi
        xor     eax, eax
        test    edx, edx
        push    005C4534h
        ret
    }
}

__declspec(naked) void tech_size_33() //005C5096
{
    __asm
    {
        mov     ecx, [ecx + 24h]
        xor     edx, edx
        mov     dx, [ebp + 18h]
        push    005C509Eh
        ret
    }
}

__declspec(naked) void tech_size_34() //005C538F
{
    __asm
    {
        xor     ecx, ecx
        mov     cx, [esi + 18h]
        mov     edx, [edx + eax * 4]
        push    005C5397h
        ret
    }
}

__declspec(naked) void tech_size_35() //005C55D0
{
    __asm
    {
        xor     eax, eax
        push    ebx
        mov     ax, [ecx + 18h]
        push    005C55D6h
        ret
    }
}

__declspec(naked) void tech_size_36() //005C57B1
{
    __asm
    {
        xor     esi, esi
        mov     bx, [ebp + 18h]
        test    ebx, ebx
        push    005C57B8h
        ret
    }
}

__declspec(naked) void tech_size_37() //005C5980
{
    __asm
    {
        xor     edx, edx
        mov     bx, [ebp + 18h]
        test    ebx, ebx
        push    005C5987h
        ret
    }
}

__declspec(naked) void tech_size_38() //005C5B70
{
    __asm
    {
        xor     edx, edx
        mov     bx, [ebp + 18h]
        test    ebx, ebx
        push    005C5B77h
        ret
    }
}

__declspec(naked) void tech_size_39() //005C5C70
{
    __asm
    {
        xor     edx, edx
        push    esi
        mov     dx, [ecx + 18h]
        push    005C5C76h
        ret
    }
}

/////

__declspec(naked) void age_unit_size_1() //005C1839
{
    __asm
    {
        mov     ecx, [ebp + 4]
        mov     ax, [esi + ecx + 14h]
        test    ax, ax
        push    005C1842h
        ret
    }
}

__declspec(naked) void age_unit_size_2() //005C1865
{
    __asm
    {
        mov     cx, [eax + 14h]
        mov     edi, [eax + 18h]
        push    005C186Bh
        ret
    }
}

__declspec(naked) void age_unit_size_3() //005C2752
{
    __asm
    {
        mov     eax, [esi + 4]
        mov     ax, [eax + ebx + 14h]
        test    ax, ax
        push    005C275Bh
        ret
    }
}

__declspec(naked) void age_unit_size_4() //005C2774
{
    __asm
    {
        mov     edx, [esi + 4]
        xor     ecx, ecx
        mov     cx, [edx + ebx + 14h]
        push    005C277Dh
        ret
    }
}

__declspec(naked) void age_unit_size_5() //005C4595
{
    __asm
    {
        mov     cx, [ebx + 14h]
        xor     eax, eax
        push    ebp
        push    edi
        test    cx, cx
        push    005C459Eh
        ret
    }
}

__declspec(naked) void age_unit_size_6() //005C4665
{
    __asm
    {
        mov     dx, [ebx + 14h]
        inc     eax
        cmp     eax, edx
        push    005C466Bh
        ret
    }
}

__declspec(naked) void age_unit_size_7() //005C4675
{
    __asm
    {
        mov     cx, [ebx + 14h]
        xor     eax, eax
        test    cx, cx
        push    005C467Ch
        ret
    }
}

__declspec(naked) void age_unit_size_8() //005C46C9
{
    __asm
    {
        mov     cx, [ebx + 14h]
        inc     eax
        cmp     eax, ecx
        push    005C46CFh
        ret
    }
}

__declspec(naked) void age_unit_size_9() //005C2D77
{
    __asm
    {
        add     ecx, ebx
        mov     ax, [ecx + 14h]
        test    ax, ax
        push    005C2D7Eh
        ret
    }
}

#pragma optimize( "s", on )
void setTechTreeHooks()
{
    //tech size
    writeByte(0x005C26A1, 2);
    writeByte(0x005C2CF1, 2);
    
    //age unit size
    writeByte(0x005C2746, 2);
    writeByte(0x005C2D68, 2);
    writeDword(0x005C2D84, 0xFFFF);
    writeDword(0x005C275E, 0xFFFF);
    writeDword(0x005C1845, 0xFFFF);

    setHook((void*)0x005C2B20, techs_size_1);
    setHook((void*)0x005C2CA3, tech_size_2);
    setHook((void*)0x005C306F, tech_size_3);
    setHook((void*)0x005C3189, tech_size_4);
    setHook((void*)0x005C25F0, tech_size_5);
    setHook((void*)0x005C264F, tech_size_6);
    setHook((void*)0x005C16CE, tech_size_7);
    setHook((void*)0x005C1756, tech_size_8);
    setHook((void*)0x005C1724, tech_size_9);
    setHook((void*)0x005C179F, tech_size_10);
    setHook((void*)0x005C1ACD, tech_size_11);
    setHook((void*)0x005C1AEE, tech_size_12);
    setHook((void*)0x005C1BED, tech_size_13);
    setHook((void*)0x005C32BC, tech_size_14);
    setHook((void*)0x005C3931, tech_size_15);
    setHook((void*)0x005C3987, tech_size_16);
    setHook((void*)0x005C3B07, tech_size_17);
    setHook((void*)0x005C3CCE, tech_size_18);
    setHook((void*)0x005C3D4B, tech_size_19);
    setHook((void*)0x005C3E95, tech_size_20);
    setHook((void*)0x005C3EFA, tech_size_21);
    setHook((void*)0x005C401A, tech_size_22);
    setHook((void*)0x005C403F, tech_size_23);
    setHook((void*)0x005C40E2, tech_size_24);
    setHook((void*)0x005C40FF, tech_size_25);
    setHook((void*)0x005C4150, tech_size_26);
    setHook((void*)0x005C41AD, tech_size_27);
    setHook((void*)0x005C4219, tech_size_28);
    setHook((void*)0x005C42D5, tech_size_29);
    setHook((void*)0x005C4384, tech_size_30);
    setHook((void*)0x005C4487, tech_size_31);
    setHook((void*)0x005C452B, tech_size_32);
    setHook((void*)0x005C5096, tech_size_33);
    setHook((void*)0x005C538F, tech_size_34);
    setHook((void*)0x005C55D0, tech_size_35);
    setHook((void*)0x005C57B1, tech_size_36);
    setHook((void*)0x005C5980, tech_size_37);
    setHook((void*)0x005C5B70, tech_size_38);
    setHook((void*)0x005C5C70, tech_size_39);
    setHook((void*)0x005C1839, age_unit_size_1);
    setHook((void*)0x005C1865, age_unit_size_2);
    setHook((void*)0x005C2752, age_unit_size_3);
    setHook((void*)0x005C2774, age_unit_size_4);
    setHook((void*)0x005C4595, age_unit_size_5);
    setHook((void*)0x005C4665, age_unit_size_6);
    setHook((void*)0x005C4675, age_unit_size_7);
    setHook((void*)0x005C46C9, age_unit_size_8);
    setHook((void*)0x005C2D77, age_unit_size_9);
}
#pragma optimize( "", on )
