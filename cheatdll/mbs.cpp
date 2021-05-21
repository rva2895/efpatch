#include "stdafx.h"
#include "hotkeys.h"

//sub_445A10 -> sub_5EF170

uint8_t mbs_enabled = 1;

__declspec(naked) void sub_5EF170_new()
{
    __asm
    {
        mov     eax, [ecx + 1440h]
        and     eax, 7
        retn
    }
}

__declspec(naked) void mbs_1() //00617D6B, 13945
{
    __asm
    {
        mov     edx, [edx + 18h]    //0Ch -> 18h
        cmp     [eax + 18h], edx    //should be player vtable
        jnz     short loc_413967
        mov     eax, 004C3530h
        call    eax
        mov     edx, 00617D70h
        jmp     edx
loc_413967:
        mov     ecx, 00617D7Fh
        jmp     ecx
    }
}

__declspec(naked) void mbs_sort_compare_function()
{
    __asm
    {
        mov     eax, [esp + 4]
        mov     edx, [esp + 8]
        mov     ecx, [eax + 4]
        mov     eax, [edx + 4]
        sub     eax, ecx
        jg      short loc_7D94B9
        jl      short loc_7D94B5
        retn

loc_7D94B5:
        or      eax, 0FFFFFFFFh
        retn

loc_7D94B9:
        mov     eax, 1
        retn
    }
}

__declspec(naked) void __stdcall mbs_do_command(void*) //actually __thiscall
{
    __asm
    {
        mov     esi, [esp + 4]
        push    edi
        push    ebx
        push    ebp
        sub     esp, 200h
        mov     [esp + 10h], ecx
        xor     ebx, ebx
        test    mbs_enabled, 1
        jnz     short loc_7D938D
        mov     edx, 006A35E0h //dword_791200
        mov     edx, [edx]
        mov     ecx, [edx + 1614h] //no change?
        mov     edx, [ecx + 4] //no change?
        and     edx, edx
        jle     short loc_7D940A

loc_7D938D:
        mov     edx, 006A3684h //dword_7912A0
        mov     edx, [edx]
        movsx   edi, byte ptr [esi + 6]
        mov     ecx, [edx + 420h] // +424h -> +420h
        mov     [esp + 14h], ecx
        mov     [esp + 18h], ebx
        dec     edi
        jl      short loc_7D940A

loc_7D93A8:
        mov     eax, [esi + edi * 4 + 8]
        mov     ecx, [esp + 14h]
        push    eax
        mov     eax, 00623DB0h
        call    eax //sub_411460
        test    eax, eax
        jz      short loc_7D93E5
        cmp     byte ptr [eax + 58h], 2 // +48h -> +58h
        mov     edx, [eax + 14h] // +8 -> +14h
        jnz     short loc_7D93E5
        cmp     byte ptr [edx + 4], 50h
        jnz     short loc_7D93E5
        movsx   edx, word ptr [eax + 1FCh] // +1C8h -> +1FCh
        cmp     edx, 0Fh
        jge     short loc_7D93E5
        mov     ecx, [eax + 10h] // +4 -> +10h
        add     [esp + 18h], edx
        mov     [esp + ebx * 8 + 30h], ecx
        mov     [esp + ebx * 8 + 34h], edx
        inc     ebx

loc_7D93E5:
        dec     edi
        jge     short loc_7D93A8
        mov     eax, ebx
        mov     edx, [esp + 18h]
        lea     eax, [eax + eax * 2]
        movsx   edi, byte ptr [esi + 7]
        lea     eax, [eax + eax * 4]
        sub     eax, edx
        mov     [esp + 1Ch], eax
        jle     short loc_7D940A
        cmp     eax, edi
        jge     short loc_7D9406
        mov     edi, eax

loc_7D9406:
        and     edi, edi
        jg      short loc_7D9417

loc_7D940A:
        add     esp, 200h
        pop     ebp
        pop     ebx
        pop     edi
        pop     esi
        retn    4

loc_7D9417:
        mov     [esp + 20h], ebx
        movsx   ebp, word ptr [esi + 4]
        mov     [esp + 8], bp
        lea     eax, [esp + 30h]
        push    mbs_sort_compare_function
        push    8
        push    ebx
        push    eax
        mov     eax, 00635C9Fh
        call    eax //_qsort
        add     esp, 10h

loc_7D9439:
        mov     ebx, [esp + 20h]
        dec     ebx
        jnz     short loc_7D9447
        mov     word ptr [esp + ebx * 8 + 36h], di
        jmp     short loc_7D9468

loc_7D9447:
        lea     ecx, [esp + ebx * 8 + 34h]
        add     dword ptr [ecx], 10001h
        dec     edi
        jz      short loc_7D9468
        and     ebx, ebx
        jz      short loc_7D9464
        mov     ax, [ecx]
        cmp     ax, [ecx - 8]
        jl      short loc_7D9439
        dec     ebx
        jge     short loc_7D9447

loc_7D9464:
        and     edi, edi
        jnz     short loc_7D9439

loc_7D9468:
        mov     ebx, [esp + 20h]

loc_7D946C:
        dec     ebx
        mov     ax, word ptr[esp + ebx * 8 + 36h]
        test    ax, ax
        jz      short loc_7D9492
        mov     edx, [esp + ebx * 8 + 30h]
        mov     ecx, [esp + 10h]
        mov     [esp + 0Ah], ax
        mov     [esp + 4], edx
        push    esp
        mov     eax, 005BC530h
        call    eax //sub_468630
        and     ebx, ebx
        jnz     short loc_7D946C

loc_7D9492:
        add     esp, 200h
        pop     ebp
        pop     ebx
        pop     edi
        retn    4
    }
}

__declspec(naked) void mbs_make_command()
{
    __asm
    {
        push    esi
        push    edi
        push    ebx
        push    ebp
        mov     edx, [esp + 18h]
        lea     ebp, ds:8[edx * 4]
        mov     ebx, ecx
        push    ebp
        push    1
        mov     eax, 00632D33h
        call    eax //_calloc
        add     esp, 8
        test    eax, eax
        jz      short loc_7D9359
        mov     byte ptr [eax], 81h
        mov     edx, [esp + 14h]
        mov     ecx, [esp + 18h]
        mov     [eax + 4], dx
        mov     edx, [esp + 20h]
        mov     [eax + 6], cl
        mov     [eax + 7], dl
        lea     edi, [eax + 8]
        mov     esi, [esp + 1Ch]
        rep movsd
        mov     edx, [esp + 24h]
        mov     ecx, [edx + 18h] // +0Ch -> +18h
        mov     edx, [ecx + 0A0h] // +9Ch -> +0A0h
        mov     ecx, ebx
        push    edx
        push    ebp
        push    eax
        mov     eax, 0044CFD0h
        call    eax //sub_5C8F70

loc_7D9359:
        pop     ebp
        pop     ebx
        pop     edi
        pop     esi
        retn    14h
    }
}

__declspec(naked) void mbs_button() //005020C0
{
    __asm
    {
        test    mbs_enabled, 1
        jz      short loc_7D9013
        cmp     word ptr [ecx + 1284h], 1 // +1234h -> +1284h
        jg      short loc_7D9077

loc_7D9013:
        sub     esp, 208h
        push    ebp
        push    esi
        push    edi
        mov     edi, ecx
        mov     ecx, 006A3684h //dword_7912A0
        mov     ecx, [ecx]
        push    0
        mov     eax, 00428590h
        call    eax //sub_5E7380
        test    eax, eax
        jnz     short loc_7D906B
        mov     ecx, [edi + 10C8h] // +107Ch -> +10C8h
        mov     eax, 005D93C0h
        call    eax //sub_44D9C0
        cmp     al, 3
        jnz     short loc_7D904B
        mov     ecx, [edi + 10C8h] // +107Ch -> +10C8h
        push    1
        mov     eax, 005D93A0h
        call    eax //sub_44D9A0

loc_7D904B:
        mov     ecx, 006A3684h //dword_7912A0
        mov     ecx, [ecx]
        mov     eax, 00428750h
        call    eax //sub_5E7560
        mov     edx, eax
        mov     eax, [edi + 1280h] // +1230h -> +1280h
        test    eax, eax
        jz      short loc_7D906B
        cmp     edx, [eax + 18h] // +0Ch -> +18h
        jz      loc_52470A

loc_7D906B:
        pop     edi
        pop     esi
        pop     ebp
        add     esp, 208h
        retn    8

loc_7D9077:
        sub     esp, 408h
        push    ebp
        push    ebx
        push    esi
        push    edi
        mov     edi, ecx
        mov     ecx, 006A3684h //dword_7912A0
        mov     ecx, [ecx]
        push    0
        mov     eax, 00428590h
        call    eax //sub_5E7380
        test    eax, eax
        jnz     loc_7D92E7
        mov     ecx, [edi + 10C8h] // +107Ch -> +10C8h
        mov     eax, 005D93C0h
        call    eax //sub_44D9C0
        cmp     al, 3
        jnz     short loc_7D90B4
        mov     ecx, [edi + 10C8h] // +107Ch -> +10C8h
        push    1
        mov     eax, 005D93A0h
        call    eax //sub_44D9A0

loc_7D90B4:
        mov     ebp, [edi + 1280h] // +1230h -> +1280h
        and     ebp, ebp
        jz      loc_7D92E7
        mov     ecx, 006A3684h //dword_7912A0
        mov     ecx, [ecx]
        mov     eax, 00428750h
        call    eax //sub_5E7560
        cmp     eax, [ebp + 18h] // +0Ch -> +18h
        jnz     loc_7D92E7
        mov     ecx, [ebp + 14h] // +8 -> +14h
        cmp     byte ptr [ecx + 4], 50h
        jnz     loc_7D92E7
        movsx   ebp, word ptr [ecx + 18h] // +10h -> +18h
        lea     ecx, [eax + 1C8h] // +1C4h -> +1C8h
        mov     eax, [eax + 26Ch] // +268h -> +26Ch
        xor     edx, edx
        dec     eax
        jl      loc_7D92E7
        xor     ebx, ebx
        mov     [esp + 10h], edx

loc_7D9102:
        mov     esi, [ecx + eax * 4]
        and     esi, esi
        jz      short loc_7D9133
        cmp     byte ptr [esi + 58h], 2 // +48h -> +58h
        jnz     short loc_7D9133
        mov     edx, [esi + 14h] // +8 -> +14h
        cmp     [edx + 18h], bp // +10h -> +18h
        jnz     short loc_7D9133
        movsx   edx, word ptr [esi + 1FCh] // +1C8h -> +1FCh
        cmp     edx, 0Fh
        jge     short loc_7D9133
        mov     esi, [esi + 10h] // +4 -> +10h
        add     [esp + 10h], edx
        mov     [esp + ebx * 4 + 220h], esi
        inc     ebx

loc_7D9133:
        dec     eax
        jge     short loc_7D9102
        mov     eax, ebx
        lea     eax, [eax + eax * 2]
        lea     eax, [eax + eax * 4]
        sub     eax, [esp + 10h]
        mov     dword ptr [esp + 218h], ebx
        mov     [esp + 21Ch], eax
        jg      short loc_7D919C
        mov     ecx, 006A3684h //dword_7912A0
        mov     ecx, [ecx]
        push    0
        push    0
        push    3
        mov     eax, 0042CD70h
        call    eax //sub_5EB990
        mov     ecx, 006A3684h //dword_7912A0
        mov     ecx, [ecx]
        lea     eax, [esp + 18h]
        push    200h
        push    eax
        mov     edx, [ecx]
        push    0C10h
        call    dword ptr [edx + 28h]
        push    0
        lea     ecx, [esp + 1Ch]
        push    24h
        push    ecx
        push    1
        mov     ecx, edi
        mov     eax, 004FDFA0h
        call    eax //sub_520030
        pop     edi
        pop     esi
        pop     ebx
        pop     ebp
        add     esp, 408h
        retn    8

loc_7D919C:
        mov     esi, [esp + 420h]
        cmp     eax, esi
        jge     short loc_7D91A9
        mov     esi, eax

loc_7D91A9:
        mov     ebp, [esp + 41Ch]
        and     si, si
        jle     loc_7D92A4
        movsx   edx, si
        push    0
        push    0
        mov     [esp + 18h], edx
        push    1
        fild    dword ptr [esp + 1Ch]
        push    ecx
        mov     ecx, 006A3684h //dword_7912A0
        mov     ecx, [ecx]
        lea     eax, [esp + 20h]
        fstp    dword ptr [esp]
        push    eax
        push    ebp
        mov     eax, 00428750h
        call    eax //sub_5E7560
        mov     ecx, eax
        mov     eax, 005D0B70h
        call    eax //sub_457CB0
        test    al, al
        jnz     loc_7D92A4
        xor     ebx, ebx
        cmp     si, 1
        jle     short loc_7D9235

loc_7D91F6:
        dec     esi
        push    0
        movsx   ecx, si
        mov     [esp + 18h], ecx
        push    0
        fild    dword ptr [esp + 1Ch]
        push    1
        push    ecx
        mov     ecx, 006A3684h //dword_7912A0
        mov     ecx, [ecx]
        lea     edx, [esp + 20h]
        fstp    dword ptr [esp]
        push    edx
        push    ebp
        mov     eax, 00428750h
        call    eax //sub_5E7560
        mov     ecx, eax
        mov     eax, 005D0B70h
        call    eax //sub_457CB0
        cmp     al, 1
        jz      short loc_7D9230
        cmp     si, 1
        jg      short loc_7D91F6
        jmp     short loc_7D9235

loc_7D9230:
        mov     ebx, 1

loc_7D9235:
        mov     ax, word ptr [esp + 10h]
        cmp     ax, 0BBDh
        jz      short loc_7D924E
        cmp     ax, 0BE6h
        jz      short loc_7D924E
        push    0
        push    0
        push    3
        jmp     short loc_7D9254

loc_7D924E:
        push    0
        push    0
        push    24h

loc_7D9254:
        mov     ecx, 006A3684h //dword_7912A0
        mov     ecx, [ecx]
        mov     eax, 0042CD70h
        call    eax //sub_5EB990
        mov     ecx, 006A3684h //dword_7912A0
        mov     ecx, [ecx]
        lea     edx, [esp + 18h]
        push    200h
        push    edx
        movsx   edx, word ptr [esp + 18h]
        mov     eax, [ecx]
        push    edx
        push    1
        push    67h
        call    dword ptr [eax + 2Ch]
        push    0FFFFFFFFh
        push    0
        mov     ecx, [edi + 11C8h] // +1178h -> +11C8h
        push    0FFFFFFFFh
        push    0
        push    0
        push    0
        push    0
        lea     eax, [esp + 34h]
        push    24h
        push    eax
        push    1
        mov     eax, 004CCE30h
        call    eax //sub_54A1F0
        and     ebx, ebx
        jz      short loc_7D92E7

loc_7D92A4:
        mov     eax, 006A3684h //dword_7912A0
        mov     eax, [eax]
        mov     edx, [edi + 126Ch] // +121C -> +126Ch
        mov     [eax + 428h], bp // +42Ch -> +428h
        mov     byte ptr [eax + 42Ch], 0 // +430h -> +42Ch
        mov     ecx, [edx + 68h]
        mov     word ptr [eax + 42Ah], 0 // +42Eh -> +42Ah
        mov     edx, [edi + 1280h] // +1230h -> +1280h
        mov     ebx, dword ptr [esp + 218h]
        lea     eax, [esp + 220h]
        push    edx
        push    esi
        push    eax
        push    ebx
        push    ebp
        call    mbs_make_command

loc_7D92E7:
        pop     edi
        pop     esi
        pop     ebx
        pop     ebp
        add     esp, 408h
        retn    8

loc_52470A:
        mov     ecx, 0050210Ah
        jmp     ecx
    }
}

__declspec(naked) void mbs_on_select() //004C2F61
{
    __asm
    {
        mov     ecx, [edi + 26Ch] // +268h -> +26Ch
        and     ecx, ecx
        jz      short loc_7D951C
        cmp     byte ptr [esi + 5Eh], 50h // +4Eh -> +5Eh
        jnz     short loc_7D951C
        mov     edx, [esi + 14h] // +8 -> +14h
        cmp     byte ptr [edx + 95h], 4 //   !!!!!!
        jge     short loc_7D951C
        cmp     byte ptr [esi + 48h], 2 //   !!!!!!
        jnz     short loc_7D9522
        sub     esp, 0Ch
        mov     ecx, esi
        lea     edx, [esp + 4]
        mov     eax, esp
        or      dword ptr [esp + 8], 0FFFFFFFFh
        push    0
        push    0
        push    eax
        lea     eax, [esp + 14h]
        push    edx
        mov     edx, [ecx]
        push    eax
        call    dword ptr [edx + 37Ch] //           !!!!!!!!
        mov     ecx, [edi + 26Ch] // +268h -> +26Ch
        mov     ax, word ptr [esp + 8]
        add     esp, 0Ch
        cmp     ax, 67h
        jz      short loc_7D9522

loc_7D951C:
        mov     [edi + 1C4h], esi // +1C0h -> +1C4h

loc_7D9522:
        inc     ecx
        mov     eax, [esp + 14h]
        mov     [edi + 26Ch], ecx // +268h -> +26Ch
        mov     ecx, 004C2F78h
        jmp     ecx
    }
}

__declspec(naked) void TRIBE_Command__do_command_mbs()
{
    __asm
    {
        push    esi
        mov     ecx, edi
        call    mbs_do_command
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        pop     ecx
        retn    4
    }
}

__declspec(naked) void TRIBE_Command__do_command_shift_delete()
{
    __asm
    {
        push    esi
        push    edi
        call    shift_delete_do_command
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        pop     ecx
        retn    4
    }
}

const uint32_t dispatch_command_offsets[] =
{
    0x005B9EF3,
    0x005B9E1A,
    0x005B9E2A,
    0x005B9E3A,
    0x005B9EE3,
    0x005B9E5A,
    0x005B9E4A,
    0x005B9F45,
    0x005B9F8A,
    0x005B9F9A,
    0x005B9FAA,
    0x005B9FE0,
    0x005BA085,
    0x005B9FF0,
    0x005BA095,
    0x005BA0A5,
    0x005BA000,
    0x005BA055,
    0x005BA065,
    0x005BA075,
    0x005BA0B5,
    0x005BA0C5,
    0x005BA0D5,
    0x005BA11B,
    0x005BA161,
    0x005BA185,
    0x005BA195,
    0x005BA1A5,
    0x005BA1B5,
    0x005BA1D5,
    (uint32_t)TRIBE_Command__do_command_mbs,
    (uint32_t)TRIBE_Command__do_command_shift_delete
};

__declspec(naked) void mbs_2() //00601DEA, 29EFD
{
    __asm
    {
        cmp     eax, 0FFFFFFFDh
        jz      short loc_7AEFA1
        dec     eax
        jnz     loc_42A02E; jumptable 00429FA8 default case
        jmp     loc_429F04

loc_7AEFA1:
        cmp     [ebp + 48h], bx
        mov     esi, ebx
        jle     loc_42A02E; jumptable 00429FA8 default case
        mov     ecx, [ebp + 4Ch]
        mov     eax, [ecx]
        mov     ecx, [ebp + 6Ch]
        test    eax, eax
        jz      short loc_7AEFCD
        movsx   edi, word ptr [eax + 0CF6h]
        push    1
        mov     [ecx + 1B84h], edi        //offset is the same
        mov     eax, 005F6D20h    //call    sub_435C00
        call    eax
loc_7AEFCD:
        mov     eax, [ebp + 4Ch]
        push    1
        push    edi
        push    7
        push    0Ah
        mov     ecx, [eax + esi * 4]
        mov     ecx, [ecx + 84h]
        //call    sub_424C70
        mov     eax, 00608520h
        call    eax
        movsx   edx, word ptr [ebp + 48h]
        inc     esi
        cmp     esi, edx
        jl      short loc_7AEFCD
loc_42A02E:
        mov     ecx, 00601F1Bh
        jmp     ecx
loc_429F04:
        mov     esi, 00601DF1h
        jmp     esi
    }
}

#pragma optimize( "s", on )
void setMbsHooks()
{
    //writeDword(0x00600F6E, (DWORD)&sub_5EF170_new - 0x00600F72); //28FB1
    //writeDword(0x00602629, (DWORD)&sub_5EF170_new - 0x0060262D); //2A72A, maybe for tech cap?
    //writeByte(0x0060262F, 0xFE);
    //writeByte(0x00602632, 4);
    //writeDword(0x00541A0D, (DWORD)&sub_5EF170_new - 0x00541A11);
    //writeDword(0x0051E22B, (DWORD)&sub_5EF170_new - 0x0051E22F);
    //writeDword(0x004E546A, (DWORD)&sub_5EF170_new - 0x004E546E);
    setHook((void*)0x00617D6B, mbs_1);
    setHook((void*)0x005020C0, mbs_button);
    setHook((void*)0x004C2F61, mbs_on_select);

    writeDword(0x005B9E16, (DWORD)dispatch_command_offsets);
    writeByte(0x005BA2DD, 0x1E); //dispatch table offset
    writeByte(0x005BA2DE, 0x1F);
    writeByte(0x005B9E04, 0x7E); //max command id -> 0x7E
}
#pragma optimize( "", on )
