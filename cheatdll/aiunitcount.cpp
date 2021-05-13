#include "stdafx.h"

#include "aiunitcount.h"
//#include <string.h>

//TODO:
//ctr 19 and 20 need effects (group ids)

__declspec(naked) void ctr1() //004C2B7B
{
    __asm
    {
        //mov     bp, [esi+ebp*2+1038h]
        mov     eax, [esi + 1038h]
        mov     bp, [eax + ebp * 2]
        mov     eax, 004C2B83h
        jmp     eax
    }
}

__declspec(naked) void ctr2() //004C43EA
{
    __asm
    {
        //add     [ecx+eax*2+1038h], si
        //cmp     eax, 0CA0h
        //ja      _errorctr
        mov     esi, [ecx + 1038h]
        lea     eax, [esi + eax * 2]
        mov     esi, [esp + 18h]
        add     word ptr [eax], si
        mov     eax, 004C43F6h
        jmp     eax
//_errorctr:
        //int     3
    }
}

__declspec(naked) void ctr3() //00576D60
{
    __asm
    {
        //cmp     word ptr [ecx+edi*2+1038h], 1
        mov     esi, [ecx + 1038h]
        cmp     word ptr [esi + edi * 2], 1
        mov     esi, 00576D69h
        jmp     esi
    }
}

__declspec(naked) void ctr4() //00576E7A
{
    __asm
    {
        //cmp     word ptr [ecx+esi*2+1038h], 1
        mov     ebx, [ecx + 1038h]
        cmp     word ptr [ebx + esi * 2], 1
        mov     ebx, 00576E83h
        jmp     ebx
    }
}

__declspec(naked) void ctr5() //0057AF02
{
    __asm
    {
        //movsx   edx, word ptr [ecx+eax*2+1038h]
        mov     edx, [ecx + 1038h]
        movsx   edx, word ptr [edx + eax * 2]
        mov     ecx, 0057AF0Ah
        jmp     ecx
    }
}

__declspec(naked) void ctr6() //0057AF30
{
    __asm
    {
        //movsx   esi, word ptr [eax+ecx*2+1038h]
        mov     esi, [eax + 1038h]
        movsx   esi, word ptr [esi + ecx * 2]
        mov     eax, 0057AF38h
        jmp     eax
    }
}

__declspec(naked) void ctr7() //0057BA0A
{
    __asm
    {
        //movsx   ecx, word ptr [edi+eax*2+1038h]
        mov     ecx, [edi + 1038h]
        movsx   ecx, word ptr [ecx + eax * 2]
        add     esi, ecx
        mov     ecx, 0057BA14h
        jmp     ecx
    }
}

__declspec(naked) void ctr8() //0057BA27
{
    __asm
    {
        //movsx   esi, word ptr [edi+edx*2+1038h]
        mov     esi, [edi + 1038h]
        movsx   esi, word ptr [esi + edx * 2]
        mov     eax, 0057BA2Fh
        jmp     eax
    }
}

__declspec(naked) void ctr9() //0057BD9C
{
    __asm
    {
        //movsx   ecx, word ptr [edi+eax*2+1038h]
        mov     ecx, [edi + 1038h]
        movsx   ecx, word ptr [ecx + eax * 2]
        add     esi, ecx
        mov     ecx, 0057BDA6h
        jmp     ecx
    }
}

__declspec(naked) void ctr10() //0057BDB9
{
    __asm
    {
        //movsx   esi, word ptr [edi+edx*2+1038h]
        mov     esi, [edi + 1038h]
        movsx   esi, word ptr [esi + edx * 2]
        mov     eax, 0057BDC1h
        jmp     eax
    }
}

__declspec(naked) void ctr11() //0057CE91
{
    __asm
    {
        //movsx   eax, word ptr [ebp+esi*2+1038h]
        mov     eax, [ebp + 1038h]
        movsx   eax, word ptr [eax + esi * 2]
        mov     ecx, 0057CE99h
        jmp     ecx
    }
}

__declspec(naked) void ctr12() //0057CEC5
{
    __asm
    {
        //movsx   edi, word ptr [ebp+esi*2+1038h]
        mov     edi, [ebp + 1038h]
        movsx   edi, word ptr [edi + esi * 2]
        mov     eax, 0057CECDh
        jmp     eax
    }
}

__declspec(naked) void ctr13() //0057DB28
{
    __asm
    {
        //movsx   eax, word ptr [ebp+esi*2+1038h]
        mov     eax, [ebp + 1038h]
        movsx   eax, word ptr [eax + esi * 2]
        mov     ecx, 0057DB30h
        jmp     ecx
    }
}

__declspec(naked) void ctr14() //0057DB5A
{
    __asm
    {
        //movsx   edi, word ptr [ebp+esi*2+1038h]
        mov     edi, [ebp + 1038h]
        movsx   edi, word ptr [edi + esi * 2]
        mov     eax, 0057DB62h
        jmp     eax
    }
}

__declspec(naked) void ctr15() //0057E5C9
{
    __asm
    {
        //movsx   edx, word ptr [ecx+eax*2+1038h]
        mov     edx, [ecx + 1038h]
        movsx   edx, word ptr [edx + eax * 2]
        mov     eax, 0057E5D1h
        jmp     eax
    }
}

__declspec(naked) void ctr16() //0057E5EA
{
    __asm
    {
        //movsx   esi, word ptr [eax+ecx*2+1038h]
        mov     esi, [eax + 1038h]
        movsx   esi, word ptr [esi + ecx * 2]
        mov     eax, 0057E5F2h
        jmp     eax
    }
}

__declspec(naked) void ctr17() //005B2065 //?
{
    __asm
    {
        //cmp     word ptr [ecx+10DCh], 0
        mov     edx, [ecx + 1038h]
        cmp     word ptr [edx + 0A4h], 0
        mov     edx, 005B206Dh
        jmp     edx
    }
}

__declspec(naked) void ctr18() //005B208B //?
{
    __asm
    {
        //cmp     word ptr [ecx+1112h], 0
        mov     edx, [ecx + 1038h]
        cmp     word ptr [edx + 0DAh], 0
        mov     edx, 005B2093h
        jmp     edx
    }
}

__declspec(naked) void ctr19() //005B20B1
{
    __asm
    {
        //cmp     word ptr [ecx+1CCCh], 0
        mov     edx, [ecx + 1038h]
        cmp     word ptr [edx + 0C94h], 0
        mov     edx, 005B20B9h
        jmp     edx
    }
}

__declspec(naked) void ctr20() //005B2065
{
    __asm
    {
        //cmp     word ptr [ecx+10DCh], 0
        mov     edx, [ecx + 1038h]
        cmp     word ptr [edx + 0A4h], 0
        mov     edx, 005B206Dh
        jmp     edx
    }
}

__declspec(naked) void ctr2_1() //004C2B5C
{
    __asm
    {
        //mov     bx, [esi+ebp*2+2F0h]
        mov     ecx, [esi + 2F0h]
        mov     bx, [ecx + ebp * 2]
        mov     ecx, 004C2B64h
        jmp     ecx
    }
}

__declspec(naked) void ctr2_2() //004C42DA
{
    __asm
    {
        //add     [ecx+eax*2+2F0h], si
        mov     esi, [ecx + 2F0h]
        lea     eax, [esi + eax * 2]
        mov     esi, [esp + 18h]
        add     word ptr [eax], si
        mov     eax, 004C42E6h
        jmp     eax
    }
}

__declspec(naked) void ctr2_3() //0057B022
{
    __asm
    {
        //movsx   edx, word ptr [ecx+eax*2+2F0h]
        mov     edx, [ecx + 2F0h]
        movsx   edx, word ptr [edx + eax * 2]
        mov     ecx, 0057B02Ah
        jmp     ecx
    }
}

__declspec(naked) void ctr2_4() //0057B050
{
    __asm
    {
        //movsx   esi, word ptr [eax+ecx*2+2F0h]
        mov     esi, [eax + 2F0h]
        movsx   esi, word ptr [esi + ecx * 2]
        mov     eax, 0057B058h
        jmp     eax
    }
}

__declspec(naked) void ctr2_5() //0057BB2A
{
    __asm
    {
        //movsx   ecx, word ptr [edi+eax*2+2F0h]
        mov     ecx, [edi + 2F0h]
        movsx   ecx, word ptr [ecx + eax * 2]
        add     esi, ecx
        mov     ecx, 0057BB34h
        jmp     ecx
    }
}

__declspec(naked) void ctr2_6() //0057BB47
{
    __asm
    {
        //movsx   esi, word ptr [edi+edx*2+2F0h]
        mov     esi, [edi + 2F0h]
        movsx   esi, word ptr [esi + edx * 2]
        mov     eax, 0057BB4Fh
        jmp     eax
    }
}

__declspec(naked) void ctr2_7() //0057E739
{
    __asm
    {
        //movsx   edx, word ptr [ecx+eax*2+2F0h]
        mov     edx, [ecx + 2F0h]
        movsx   edx, word ptr [edx + eax * 2]
        mov     ecx, 0057E741h
        jmp     ecx
    }
}

__declspec(naked) void ctr2_8() //0057E75A
{
    __asm
    {
        //movsx   esi, word ptr [eax+ecx*2+2F0h]
        mov     esi, [eax + 2F0h]
        movsx   esi, word ptr [esi + ecx * 2]
        mov     eax, 0057E762h
        jmp     eax
    }
}

__declspec(naked) void ctr2_9() //00596695
{
    __asm
    {
        mov     ecx, [ecx + 2F0h]
        cmp     word ptr [ecx + ebp * 2], ax
        mov     ecx, 0059669Dh
        jmp     ecx
    }
}

__declspec(naked) void ctr2_10() //005CEDE4
{
    __asm
    {
        mov     eax, [ebp + 2F0h]
        cmp     word ptr [eax + 434 * 2], 0
        mov     ecx, 005CEDECh
        jmp     ecx
    }
}

int unitCount = 0x1800;// 0xCA4;

void* __cdecl _memset(void* dst, int val, size_t size)
{
    return memset(dst, val, size);
}

__declspec(naked) void ctrInit() //004BFEDF
{
    __asm
    {
        mov     eax, unitCount;
        push    eax
        call    ds:[malloc]       //builders per type
        mov     [ebp + 103Ch], eax
        push    1
        push    eax
        call    ds:[_memset]
        add     esp, 0Ch
        mov     eax, unitCount
        shl     eax, 1
        push    eax
        call    ds:[malloc]
        push    0
        push    eax
        add     eax, 100h
        mov     [ebp + 1038h], eax
        call    ds:[_memset]
        add     esp, 8
        mov     ecx, [esp]  //
        shl     ecx, 2      //
        mov     [esp], ecx  //
        call    ds:[malloc]
        push    0
        push    eax
        add     eax, 100h
        mov     [ebp + 2F0h], eax
        call    ds:[_memset]
        lea     eax, [ebp + 1CB8h]
        push    0C8h
        push    0
        push    eax
        call    ds:[_memset]
        lea     eax, [ebp + 0F70h]
        push    0C8h
        push    0
        push    eax
        call    ds:[_memset]
        add     esp, 24h
        mov     eax, 004BFF0Fh
        jmp     eax
    }
}

__declspec(naked) void ctrReadSave() //004BF665
{
    __asm
    {
        push    edi
        push    ebx
        sub     esp, 4
        mov     edi, 004D5550h       //file read
        lea     edx, [esp]
        push    4
        mov     ecx, ebp
        call    edi                //get count
        mov     ebx, [esp]
        mov     unitCount, ebx
        shl     ebx, 1
        push    ebx
        call    ds:[malloc]
        mov     edx, eax
        add     eax, 100h
        mov     [esi + 2F0h], eax
        mov     ecx, ebp
        call    edi
        push    0C8h
        lea     edx, [esi + 0F70h]
        mov     ecx, ebp
        call    edi
        push    ebx
        call    ds:[malloc]
        mov     edx, eax
        add     eax, 100h
        mov     [esi + 1038h], eax
        mov     ecx, ebp
        call    edi
        shr     ebx, 1
        push    ebx
        call    ds:[malloc]
        mov     [esi + 103Ch], eax
        mov     edx, eax
        mov     ecx, ebp
        call    edi           //read builders type count
        add     esp, 4
        pop     ebx
        pop     edi
        mov     edx, 004BF69Bh
        jmp     edx
    }
}

__declspec(naked) void ctrWriteSave() //004C1C55
{
    __asm
    {
        push    ebp
        push    edi
        mov     ebp, 004D5790h
        mov     eax, unitCount
        push    eax
        shl     eax, 1
        mov     edi, eax
        lea     edx, [esp]
        push    4
        mov     ecx, esi
        call    ebp              //write count
        push    edi
        mov     edx, [ebx + 2F0h]
        sub     edx, 100h
        mov     ecx, esi
        call    ebp
        push    0C8h
        lea     edx, [ebx + 0F70h]
        mov     ecx, esi
        call    ebp
        push    edi
        mov     edx, [ebx + 1038h]
        sub     edx, 100h
        mov     ecx, esi
        call    ebp
        shr     edi, 1
        push    edi
        mov     edx, [ebx + 103Ch]
        mov     ecx, esi
        call    ebp           //write builders type count
        add     esp, 4
        pop     edi
        pop     ebp
        mov     edx, 004C1C8Bh
        jmp     edx
    }
}

__declspec(naked) void ctr1PrefabFix() //0057AF3F
{
    __asm
    {
        mov     esi, [eax + 1038h]
        movsx   ecx, word ptr [esi + 8Ch]  //prefab count
        movsx   esi, word ptr [esi + 7D2h] //underwater prefab count
        add     esi, ecx
        mov     eax, 0057AF4Fh
        jmp     eax
    }
}

__declspec(naked) void ctr2PrefabFix() //0057B05F
{
    __asm
    {
        mov     esi, [eax + 2F0h]
        movsx   ecx, word ptr [esi + 8Ch]  //prefab count
        movsx   esi, word ptr [esi + 7D2h] //underwater prefab count
        //add     esi, ecx
        mov     eax, 0057B06Dh
        jmp     eax
    }
}

__declspec(naked) void ctr2CCFix() //004C0745
{
    __asm
    {
        //cmp     word ptr [ecx+3CAh], 0
        mov     ecx, [ecx + 2F0h]
        cmp     word ptr [ecx + 0DAh], 0
        mov     esi, 004C074Dh
        jmp     esi
    }
}

__declspec(naked) void buf2_init() //00583966
{
    __asm
    {
        mov     eax, unitCount
        shl     eax, 2
        push    eax
        call    ds:[malloc]
        push    0
        push    eax
        call    ds:[_memset]
        mov     [esi + 3D8h], eax
        add     esp, 0Ch
        mov     edx, 4
        mov     eax, 0058397Ah
        jmp     eax
    }
}

__declspec(naked) void buf2_read() //00584927
{
    __asm
    {
        sub     esp, 4
        lea     edx, [esp]
        push    4
        mov     ecx, ebp
        mov     eax, 004D5550h
        call    eax
        pop     ebx
        mov     eax, ebx
        shl     eax, 2
        push    eax
        call    ds:[malloc]
        add     esp, 4
        mov     [esi + 3D8h], eax
        mov     edi, eax
        mov     edx, 00584932h
        jmp     edx
    }
}

__declspec(naked) void buf2_write() //005865C2
{
    __asm
    {
        mov     eax, unitCount
        push    eax
        lea     edx, [esp]
        push    4
        mov     ecx, esi
        mov     eax, 004D5790h
        call    eax
        pop     ebp
        mov     edi, [ebx + 3D8h]
        mov     edx, 005865CDh
        jmp     edx
    }
}

__declspec(naked) void buf2_1() //0058A87B
{
    __asm
    {
        mov     ecx, [esi + 3D8h]
        lea     eax, [ecx + edx * 4]
        inc     dword ptr [eax]
        mov     ebx, 0058A88Ch
        jmp     ebx
    }
}

__declspec(naked) void buf2_2() //0058AA0C
{
    __asm
    {
        mov     ecx, [esi + 3D8h]
        lea     eax, [ecx + edx * 4]
        dec     dword ptr [eax]
        mov     eax, 0058AA1Dh
        jmp     eax
    }
}

__declspec(naked) void ctr_free() //005CDCDF
{
    __asm
    {
        mov     eax, [esi + 2F0h]
        sub     eax, 100h
        push    eax
        call    ds:[free]
        mov     eax, [esi + 1038h]
        sub     eax, 100h
        push    eax
        call    ds:[free]
        mov     eax, [esi + 103Ch]
        push    eax
        call    ds:[free]
        add     esp, 0Ch
        push    esi
        mov     eax, 00632B42h
        call    eax
        add     esp, 4
        mov     eax, 005CDCE8h
        jmp     eax
    }
}

__declspec(naked) void buf2_free() //0059544F
{
    __asm
    {
        mov     eax, [esi + 1A34h]  //3D8+165C
        push    eax
        call    ds:[free]
        push    esi
        mov     eax, 00632B42h
        call    eax
        add     esp, 8
        mov     eax, 00595458h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setAIUnitCountHooks()
{
    setHook((void*)0x004C2B7B, ctr1);
    setHook((void*)0x004C43EA, ctr2);
    setHook((void*)0x00576D60, ctr3);
    setHook((void*)0x00576E7A, ctr4);
    setHook((void*)0x0057AF02, ctr5);
    setHook((void*)0x0057AF30, ctr6);
    setHook((void*)0x0057BA0A, ctr7);
    setHook((void*)0x0057BA27, ctr8);
    setHook((void*)0x0057BD9C, ctr9);
    setHook((void*)0x0057BDB9, ctr10);
    setHook((void*)0x0057CE91, ctr11);
    setHook((void*)0x0057CEC5, ctr12);
    setHook((void*)0x0057DB28, ctr13);
    setHook((void*)0x0057DB5A, ctr14);
    setHook((void*)0x0057E5C9, ctr15);
    setHook((void*)0x0057E5EA, ctr16);

    setHook((void*)0x005B2065, ctr17);
    setHook((void*)0x005B208B, ctr18);

    setHook((void*)0x004C2B5C, ctr2_1);
    setHook((void*)0x004C42DA, ctr2_2);
    setHook((void*)0x0057B022, ctr2_3);
    setHook((void*)0x0057B050, ctr2_4);
    setHook((void*)0x0057BB2A, ctr2_5);
    setHook((void*)0x0057BB47, ctr2_6);
    setHook((void*)0x0057E739, ctr2_7);
    setHook((void*)0x0057E75A, ctr2_8);
    setHook((void*)0x00596695, ctr2_9);
    setHook((void*)0x005CEDE4, ctr2_10);

    setHook((void*)0x004BFEF5, ctrInit);

    setHook((void*)0x0057AF3F, ctr1PrefabFix);
    setHook((void*)0x0057B05F, ctr2PrefabFix);

    setHook((void*)0x004C0745, ctr2CCFix);

    setHook((void*)0x004BF665, ctrReadSave);
    setHook((void*)0x004C1C55, ctrWriteSave);

    setHook((void*)0x00583966, buf2_init);
    setHook((void*)0x00584927, buf2_read);
    setHook((void*)0x005865C2, buf2_write);
    setHook((void*)0x0058A87B, buf2_1);
    setHook((void*)0x0058AA0C, buf2_2);

    setHook((void*)0x005CDCDF, ctr_free);
    setHook((void*)0x0059544F, buf2_free);

    writeDword(0x004C4251, unitCount);
    writeDword(0x004C4361, unitCount);
}
#pragma optimize( "", on )
