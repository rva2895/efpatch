#include "stdafx.h"
#include "cliff.h"

#pragma warning(push)
#pragma warning(disable:4100)
__declspec(naked) void __stdcall setCliffType(int id, RGE_Map* map) //base id
{
    __asm
    {
        push    ebp
        mov     ebp, esp
        push    edi
        push    esi
        push    ebx
        or      edi, 0FFFFFFFFh
        xor     ebx, ebx
        mov     esi, [ebp + 0Ch]    //ptr

        push    3FC00000h
        push    3FC00000h
        push    edi
        push    edi
        push    10h
        mov     eax, [ebp + 8]    //id
        push    eax
        push    ebx
        push    ebx
        push    ebx
        push    1
        mov     ecx, esi
        call    TRIBE_Map__setup_cliff_type
        push    3FC00000h
        push    3FC00000h
        push    edi
        push    edi
        push    12h
        mov     eax, [ebp + 8]    //id
        push    eax
        push    ebx
        push    ebx
        push    1
        push    ebx
        mov     ecx, esi
        call    TRIBE_Map__setup_cliff_type
        push    3FC00000h
        push    3FC00000h
        push    edi
        push    edi
        push    11h
        mov     eax, [ebp + 8]    //id
        push    eax
        push    ebx
        push    1
        push    ebx
        push    ebx
        mov     ecx, esi
        call    TRIBE_Map__setup_cliff_type
        push    3FC00000h
        push    3FC00000h
        push    edi
        push    edi
        push    13h
        mov     eax, [ebp + 8]    //id
        push    eax
        push    1
        push    ebx
        push    ebx
        push    ebx
        mov     ecx, esi
        call    TRIBE_Map__setup_cliff_type
        push    3FC00000h
        push    3FC00000h
        push    edi
        push    edi
        push    14h
        mov     eax, [ebp + 8]    //id
        add     eax, 2
        push    eax
        push    ebx
        push    ebx
        push    ebx
        push    edi
        mov     ecx, esi
        call    TRIBE_Map__setup_cliff_type
        push    3FC00000h
        push    3FC00000h
        push    edi
        push    edi
        push    16h
        mov     eax, [ebp + 8]    //id
        inc     eax
        push    eax
        push    ebx
        push    ebx
        push    edi
        push    ebx
        mov     ecx, esi
        call    TRIBE_Map__setup_cliff_type
        push    3FC00000h
        push    3FC00000h
        push    edi
        push    edi
        push    15h
        mov     eax, [ebp + 8]    //id
        add     eax, 2
        push    eax
        push    ebx
        push    edi
        push    ebx
        push    ebx
        mov     ecx, esi
        call    TRIBE_Map__setup_cliff_type
        push    3FC00000h
        push    3FC00000h
        push    edi
        push    edi
        push    17h
        mov     eax, [ebp + 8]    //id
        inc     eax
        push    eax
        push    edi
        push    ebx
        push    ebx
        push    ebx
        mov     ecx, esi
        call    TRIBE_Map__setup_cliff_type
        push    40000000h
        push    3FC00000h
        push    edi
        push    edi
        push    ebx
        mov     eax, [ebp + 8]    //id
        add     eax, 6
        push    eax
        push    ebx
        push    ebx
        push    1
        push    edi
        mov     ecx, esi
        call    TRIBE_Map__setup_cliff_type
        push    40000000h
        push    3F800000h
        push    edi
        push    edi
        push    9
        mov     eax, [ebp + 8]    //id
        add     eax, 8
        push    eax
        push    ebx
        push    edi
        push    edi
        push    ebx
        mov     ecx, esi
        call    TRIBE_Map__setup_cliff_type
        push    3FC00000h
        push    3F800000h
        push    edi
        push    edi
        push    6
        mov     eax, [ebp + 8]    //id
        add     eax, 4
        push    eax
        push    edi
        push    1
        push    ebx
        push    ebx
        mov     ecx, esi
        call    TRIBE_Map__setup_cliff_type
        push    3FC00000h
        push    3FC00000h
        push    edi
        push    edi
        push    3
        mov     eax, [ebp + 8]    //id
        push    eax
        push    1
        push    ebx
        push    ebx
        push    1
        mov     ecx, esi
        call    TRIBE_Map__setup_cliff_type
        push    3FC00000h
        push    40000000h
        push    edi
        push    edi
        push    0Ch
        mov     eax, [ebp + 8]    //id
        add     eax, 3
        push    eax
        push    ebx
        push    ebx
        push    edi
        push    1
        mov     ecx, esi
        call    TRIBE_Map__setup_cliff_type
        push    3FC00000h
        push    3FC00000h
        push    edi
        push    edi
        push    0Fh
        mov     eax, [ebp + 8]    //id
        push    eax
        push    ebx
        push    1
        push    1
        push    ebx
        mov     ecx, esi
        call    TRIBE_Map__setup_cliff_type
        push    3F800000h
        push    3FC00000h
        push    edi
        push    edi
        push    0Eh
        mov     eax, [ebp + 8]    //id
        add     eax, 5
        push    eax
        push    1
        push    edi
        push    ebx
        push    ebx
        mov     ecx, esi
        call    TRIBE_Map__setup_cliff_type
        push    3F800000h
        push    40000000h
        push    edi
        push    edi
        push    0Dh
        mov     eax, [ebp + 8]    //id
        add     eax, 7
        push    eax
        push    edi
        push    ebx
        push    ebx
        push    edi
        mov     ecx, esi
        call    TRIBE_Map__setup_cliff_type
        push    3FC00000h
        push    3FC00000h
        push    5
        mov     eax, [ebp + 8]    //id
        push    eax
        push    4
        push    eax
        push    ebx
        push    1
        push    ebx
        push    1
        mov     ecx, esi
        call    TRIBE_Map__setup_cliff_type
        push    3FC00000h
        push    3FC00000h
        push    2
        mov     eax, [ebp + 8]    //id
        push    eax
        push    1
        push    eax
        push    1
        push    ebx
        push    1
        push    ebx
        mov     ecx, esi
        call    TRIBE_Map__setup_cliff_type
        push    3FC00000h
        push    3FC00000h
        push    0Ah
        mov     eax, [ebp + 8]    //id
        add     eax, 2
        push    eax
        push    0Bh
        push    eax
        push    ebx
        push    edi
        push    ebx
        push    edi
        mov     ecx, esi
        call    TRIBE_Map__setup_cliff_type
        push    3FC00000h
        push    3FC00000h
        push    7
        mov     eax, [ebp + 8]    //id
        inc     eax
        push    eax
        push    8
        push    eax
        push    edi
        push    ebx
        push    edi
        push    ebx
        mov     ecx, esi
        call    TRIBE_Map__setup_cliff_type

        pop     ebx
        pop     esi
        pop     edi
        mov     esp, ebp
        pop     ebp
        retn    8
    }
}
#pragma warning(pop)

int cliff_type = 0x108;

__declspec(naked) void find_cliff_1() //005CAFF2
{
    __asm
    {
        push    eax
        mov     eax, cliff_type
        cmp     di, ax
        jl      short loc_5CB000
        add     eax, 9
        cmp     di, ax
        jle     short loc_5CB044

loc_5CB000:
        pop     eax
        mov     edi, 005CB000h
        jmp     edi

loc_5CB044:
        pop     eax
        mov     edi, 005CB044h
        jmp     edi
    }
}

__declspec(naked) void find_cliff_2() //005CB02C
{
    __asm
    {
        push    eax
        mov     eax, cliff_type
        cmp     bx, ax
        jl      short loc_5CB03A
        add     eax, 9
        cmp     bx, ax
        jle     short loc_5CB053

loc_5CB03A:
        pop     eax
        mov     ebx, 005CB03Ah
        jmp     ebx

loc_5CB053:
        pop     eax
        mov     ebx, 005CB053h
        jmp     ebx
    }
}

__declspec(naked) void find_cliff_3() //005CB080
{
    __asm
    {
        push    eax
        mov     eax, cliff_type
        cmp     si, ax
        jl      short loc_5CB08E
        add     eax, 9
        cmp     si, ax
        jle     short loc_5CB0D3

loc_5CB08E:
        pop     eax
        mov     esi, 005CB08Eh
        jmp     esi

loc_5CB0D3:
        pop     eax
        mov     esi, 005CB0D3h
        jmp     esi
    }
}

__declspec(naked) void find_cliff_4() //005CB0B3
{
    __asm
    {
        push    eax
        mov     eax, cliff_type
        cmp     dx, ax
        jl      short loc_5CB0C1
        add     eax, 9
        cmp     dx, ax
        jle     short loc_5CB0DF

loc_5CB0C1:
        pop     eax
        mov     edx, 005CB0C1h
        jmp     edx

loc_5CB0DF:
        pop     eax
        mov     edx, 005CB0DFh
        jmp     edx
    }
}

#define RMS_CLIFF_TYPE_ID 256

__declspec(naked) void onRmsCliff() //004E3780
{
    __asm
    {
        mov     eax, [esp + 4]
        mov     eax, [eax + 68h]
        cmp     eax, RMS_CLIFF_TYPE_ID
        jz      _set_cliff
        mov     edx, 004E3787h
        jmp     edx

_set_cliff:
        mov     edx, [ecx + 140h]
        mov     edx, [edx + 34h] //RGE_Map
        mov     eax, [ecx + 46678h]
        push    edx
        push    eax
        mov     cliff_type, eax
        call    setCliffType
        mov     edx, 004E37F7h
        jmp     edx
    }
}

__declspec(naked) void onResetCliffType() //004DFA63
{
    __asm
    {
        mov     edx, [ebp + 140h]
        mov     edx, [edx + 34h] //RGE_Map
        mov     eax, 108h
        push    edx
        push    eax
        mov     cliff_type, eax
        call    setCliffType
        mov     ecx, [ebp + 140h]
        mov     edx, 004DFA69h
        jmp     edx
    }
}

#pragma optimize( "s", on )
void setCliffTypeHooks()
{
    setHook((void*)0x004E3780, onRmsCliff);
    setHook((void*)0x004DFA63, onResetCliffType);

    setHook((void*)0x005CAFF2, find_cliff_1);
    setHook((void*)0x005CB02C, find_cliff_2);
    setHook((void*)0x005CB080, find_cliff_3);
    setHook((void*)0x005CB0B3, find_cliff_4);
}
#pragma optimize( "", on )
