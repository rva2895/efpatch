#include "stdafx.h"
#include "langdll.h"

extern int current_loaded_version;

__declspec(naked) void langdll_1() //0048BB12
{
    __asm
    {
        lea     ecx, [ecx + ebp + 52h]
        movsx   ebp, byte ptr [ecx]
        cmp     ebp, 31h
        mov     ebp, 27h
        jz      _high_civ_1
        mov     ebp, 31h
_high_civ_1:
        movsx   ecx, byte ptr [ecx + 1]
        sub     ecx, ebp
        mov     eax, 0048BB1Ah
        jmp     eax
    }
}

__declspec(naked) void langdll_2() //0048BB62
{
    __asm
    {
        lea     ecx, [ecx + edx + 52h]
        movsx   edx, byte ptr [ecx]
        cmp     edx, 31h
        mov     edx, 27h
        jz      _high_civ_2
        mov     edx, 31h
_high_civ_2:
        movsx   ecx, byte ptr [ecx + 1]
        sub     ecx, edx
        mov     eax, 0048BB6Ah
        jmp     eax
    }
}

__declspec(naked) void langdll_3() //0048BB90
{
    __asm
    {
        lea     ecx, [ecx + edx + 52h]
        movsx   edx, byte ptr [ecx]
        cmp     edx, 31h
        mov     edx, 27h
        jz      _high_civ_3
        mov     edx, 31h
_high_civ_3:
        movsx   ecx, byte ptr [ecx + 1]
        sub     ecx, edx
        mov     eax, 0048BB98h
        jmp     eax
    }
}

__declspec(naked) void langdll_4() //0048BBBE
{
    __asm
    {
        lea     ecx, [ecx + edx + 52h]
        movsx   edx, byte ptr [ecx]
        cmp     edx, 31h
        mov     edx, 27h
        jz      _high_civ_4
        mov     edx, 31h
_high_civ_4:
        movsx   ecx, byte ptr [ecx + 1]
        sub     ecx, edx
        mov     eax, 0048BBC6h
        jmp     eax
    }
}

__declspec(naked) void langdll_1_1() //0048BC12
{
    __asm
    {
        lea     ecx, [ecx + ebp + 52h]
        movsx   ebp, byte ptr [ecx]
        cmp     ebp, 31h
        mov     ebp, 27h
        jz      _high_civ_1_1
        mov     ebp, 31h
_high_civ_1_1:
        movsx   ecx, byte ptr [ecx + 1]
        sub     ecx, ebp
        mov     eax, 0048BC1Ah
        jmp     eax
    }
}

__declspec(naked) void langdll_2_1() //0048BC5D
{
    __asm
    {
        lea     ecx, [ecx + edx + 52h]
        movsx   edx, byte ptr [ecx]
        cmp     edx, 31h
        mov     edx, 27h
        jz      _high_civ_2_1
        mov     edx, 31h
_high_civ_2_1:
        movsx   ecx, byte ptr [ecx + 1]
        sub     ecx, edx
        mov     eax, 0048BC65h
        jmp     eax
    }
}

__declspec(naked) void langdll_3_1() //0048BC8B
{
    __asm
    {
        lea     ecx, [ecx + edx + 52h]
        movsx   edx, byte ptr [ecx]
        cmp     edx, 31h
        mov     edx, 27h
        jz      _high_civ_3_1
        mov     edx, 31h
_high_civ_3_1:
        movsx   ecx, byte ptr [ecx + 1]
        sub     ecx, edx
        mov     eax, 0048BC93h
        jmp     eax
    }
}

__declspec(naked) void langdll_4_1() //0048BCBA
{
    __asm
    {
        lea     ecx, [ecx + edx + 52h]
        movsx   edx, byte ptr [ecx]
        cmp     edx, 31h
        mov     edx, 27h
        jz      _high_civ_4_1
        mov     edx, 31h
_high_civ_4_1:
        movsx   ecx, byte ptr [ecx + 1]
        sub     ecx, edx
        mov     eax, 0048BCC2h
        jmp     eax
    }
}

__declspec(naked) void cargoTrader_langdll_1() //0048BB76
{
    __asm
    {
        cmp     si, 931
        jnz     short loc_48BBA4
        mov     eax, 0048BB7Ch
        jmp     eax
loc_48BBA4:
        mov     eax, 0048BBA4h
        jmp     eax
    }
}

__declspec(naked) void cargoTrader_langdll_2() //0048BC71
{
    __asm
    {
        cmp     word ptr [edx + 18h], 931
        jnz     short loc_48BC9F
        mov     eax, 0048BC77h
        jmp     eax
loc_48BC9F:
        mov     eax, 00048BC9Fh
        jmp     eax
    }
}

__declspec(naked) void medic_worker() //0054BA2B
{
    __asm
    {
        lea     ecx, [edi + eax + 52h]
        movsx   edx, byte ptr [ecx]
        cmp     edx, 31h
        mov     edx, 26h
        jz      _high_civ_medic
        mov     edx, 30h
_high_civ_medic:
        pop     edi
        pop     ebx
        movsx   ecx, byte ptr [ecx + 1]
        sub     ecx, edx
        sub     ecx, 2
        jz      _worker_bio
        sub     ecx, 4
        jz      _worker_bio
        sub     ecx, 4
        jz      _worker_bio
        mov     ecx, 0054BA44h
        jmp     ecx
_worker_bio:
        mov     ecx, 0054BA49h
        jmp     ecx
    }
}

int __stdcall isJediTemple(short id)
{
    switch (id)
    {
    case 30:
    case 31:
    case 104:
    case 3593:
    case 3594:
    case 3595:
        return true;
    default:
        return false;
    }
}

__declspec(naked) void jedi_temple_id_1() //0048BC9F
{
    __asm
    {
        push    ecx
        push    edx
        mov     ax, word ptr [edx + 18h]
        push    eax
        call    isJediTemple
        pop     edx
        pop     ecx
        test    eax, eax
        jz      _not_temple_1
        mov     eax, 0048BCA6h
        jmp     eax
_not_temple_1:
        mov     eax, 0048BCCEh
        jmp     eax
    }
}

__declspec(naked) void jedi_temple_id_2() //0048BBA4
{
    __asm
    {
        push    ecx
        push    edx
        push    esi
        call    isJediTemple
        pop     edx
        pop     ecx
        test    eax, eax
        jz      _not_temple_2
        mov     eax, 0048BBAAh
        jmp     eax
_not_temple_2:
        mov     eax, 0048BBD2h
        jmp     eax
    }
}

__declspec(naked) void databank_txt_filename() //0050A279
{
    __asm
    {
        mov     eax, current_loaded_version
        cmp     eax, 9
        mov     eax, 19811
        jge     new_databank_txt_filename_offset
        add     eax, 600

new_databank_txt_filename_offset:
        lea     ecx, [ebp + eax]
        mov     eax, 0050A27Fh
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setLangDllHooks()
{
    writeDword(0x0048BB21, 6);        //worker names, worker name offset nearby
    writeDword(0x0048BC21, 6);

    //worker names
    writeDword(0x0048BB26, 34600 + 0x10000);
    writeDword(0x0048BC26, 34600 + 0x10000 + 1000);

    writeDword(0x0048BB21, 9);
    writeDword(0x0048BC21, 9);

    //cargo names
    writeDword(0x0048BB9C, 34421 + 0x10000);
    writeDword(0x0048BC97, 34421 + 0x10000 + 1000);

    //commander names
    writeDword(0x0048BB6E, 34401 + 0x10000);
    writeDword(0x0048BC69, 34401 + 0x10000 + 1000);

    //temple names
    writeDword(0x0048BBCA, 34381 + 0x10000); //34441 -> 34381
    writeDword(0x0048BCC6, 34381 + 0x10000 + 1000);

    setHook((void*)0x0048BB12, langdll_1);
    setHook((void*)0x0048BB62, langdll_2);
    setHook((void*)0x0048BB90, langdll_3);
    setHook((void*)0x0048BBBE, langdll_4);

    setHook((void*)0x0048BC12, langdll_1_1);
    setHook((void*)0x0048BC5D, langdll_2_1);
    setHook((void*)0x0048BC8B, langdll_3_1);
    setHook((void*)0x0048BCBA, langdll_4_1);

    setHook((void*)0x0048BB76, cargoTrader_langdll_1);
    setHook((void*)0x0048BC71, cargoTrader_langdll_2);

    //setHook((void*)0x0048BC9F, jedi_temple_id_1);
    //setHook((void*)0x0048BBA4, jedi_temple_id_2);

    setHook((void*)0x0054BA2B, medic_worker);
    writeByte(0x0054B3E9, CIV_COUNT + 1);            //wrong icon (civ > 8) fix

    //databank txt filename
    //writeDword(0x0050A27B, 19811);
    setHook((void*)0x0050A279, databank_txt_filename);
}
#pragma optimize( "", on )
