#include "stdafx.h"
#include "tchat.h"

__declspec(naked) void on_review_chat_messages() //00506D1E
{
    __asm
    {
        mov     ecx, tchat
        mov     ecx, [ecx]
        cmp     [esp + 12Ch], ebx
        mov     edx, [ecx + 120h]
        mov     eax, [ecx + 124h]
        jge     short loc_7B41C4
        mov     ebx, [ebp + 1204h]
        mov     esi, [ebp + 1860h]
        and     edx, edx
        jnz     short loc_7B4197
        cmp     eax, ebx
        cmovg   eax, ebx
        cmp     esi, eax
        cmovl   esi, eax
        jmp     short loc_7B41B4

loc_7B4197:
        cmp     eax, [ebp + 1864h]
        jz      short loc_7B41B4
        mov     ecx, esi
        cmp     eax, esi
        jl      short loc_7B41A7
        add     ecx, edi

loc_7B41A7:
        add     eax, ebx
        cmp     eax, ecx
        cmovg   esi, eax
        cmp     esi, edi
        jl      short loc_7B41B4
        sub     esi, edi

loc_7B41B4:
        dec     ebx
        mov     [esp + 10h], esi
        mov     [ebp + 1864h], esi
        jmp     loc_529628

loc_7B41C4:
        mov     [esp + 10h], ebx
        mov     ecx, [ebp + 1204h]
        mov     esi, [ebp + 1864h]
        and     edx, edx
        jnz     short loc_7B41E2
        sub     eax, 7
        cmp     esi, eax
        cmovg   esi, eax
        jmp     short loc_7B41F8

loc_7B41E2:
        mov     ecx, esi
        cmp     eax, esi
        jge     short loc_7B41EA
        sub     ecx, edi

loc_7B41EA:
        sub     eax, 7
        cmp     eax, ecx
        cmovl   esi, eax
        and     esi, esi
        jge     short loc_7B41F8
        add     esi, edi

loc_7B41F8:
        mov     [esp + 1Ch], esi
        mov     [ebp + 1860h], esi

        mov     ecx, 00506E78h
        jmp     ecx

loc_529628:
        mov     ecx, 00506D48h
        jmp     ecx
    }
}

int dword_7A5040;
int dword_7A5044;

__declspec(naked) void on_add_chat_msg_1() //0042D742
{
    __asm
    {
        mov     eax, comm
        mov     eax, [eax]
        lea     ecx, [esp + 14h]
        mov     edx, [eax + 2070h]
        test    ecx, ecx
        mov     eax, [esp + 1420h]
        jz      short loc_7B414E
        test    edx, edx
        mov     edx, [ebx + eax * 4 + 2Ch]
        jnz     short loc_7B4126
        test    edx, edx
        jnz     short loc_7B414E

loc_7B4126:
        mov     eax, [ebx + 120h]
        mov     edx, [ebx + 124h]
        mov     dword ptr [esp + 0E8h], 2E2E2Eh

        mov     dword_7A5040, eax
        mov     dword_7A5044, edx
        jmp     loc_5E28FF

loc_7B414E:
        pop     edi
        pop     esi
        pop     ebp
        xor     eax, eax
        pop     ebx
        add     esp, 1404h
        retn    14h

loc_5E28FF:
        mov     eax, 0042D75Fh
        jmp     eax
    }
}

__declspec(naked) void on_add_chat_msg_2() //0042D759
{
    __asm
    {
        mov     eax, [ebx + 120h]
        mov     edx, [ebx + 124h]
        mov     dword ptr [esp + 0E8h], 2E2E2Eh
        mov     dword_7A5040, eax
        mov     dword_7A5044, edx
        
        mov     eax, 0042D75Fh
        jmp     eax
    }
}

__declspec(naked) void on_add_chat_msg_3() //0042D952
{
    __asm
    {
        jnz     loc_5E2B03
        mov     ecx, [ebx + 124h]
        lea     ecx, [ebx + ecx * 4 + 54h]
        push    ecx
        push    dword ptr [ecx]
        call    free_internal
        add     esp, 4
        pop     ecx
        mov     dword ptr[ecx], 0
        mov     eax, dword_7A5040
        mov     [ebx + 120h], eax
        mov     eax, dword_7A5044
        mov     [ebx + 124h], eax
        xor     eax, eax
        pop     edi
        pop     esi
        pop     ebp
        
        mov     ebx, 0042D957h
        jmp     ebx

loc_5E2B03:
        mov     esi, 0042D961h
        jmp     esi
    }
}

const char aCDSS[] = "@%c%d%s%s";

__declspec(naked) void on_add_chat_msg_4() //0042D9EC
{
    __asm
    {
        mov     edx, [esp + 1434h]
        add     edx, 23h
        push    edx
        push    offset aCDSS

        mov     ecx, 0042D9F1h
        jmp     ecx
    }
}

__declspec(naked) void on_clear_chat() //0042DAE8
{
    __asm
    {
        xor     eax, eax
        fldz
        lea     edi, [ebx + 30h]
        fstp    qword ptr [ebx + 120h]
        lea     ecx, [eax + 8]
        rep     stosd
        pop     edi
        pop     esi
        pop     ebx
        retn
    }
}

#pragma optimize( "s", on )
void setTChatHooks()
{
    //writeByte(0x005FED95, 0x90);

    writeByte(0x0042D76A, 0x31);

    //get chat msg
    writeByte(0x0042DAA7, 0x11);
    writeDword(0x0042DAB9, 0xEB32C083);
    writeByte(0x0042DABD, 0xEF);
    //clear chat
    setHook((void*)0x0042DAE8, on_clear_chat);

    setHook((void*)0x00506D1E, on_review_chat_messages);

    setHook((void*)0x0042D742, on_add_chat_msg_1);
    setHook((void*)0x0042D759, on_add_chat_msg_2);
    setHook((void*)0x0042D952, on_add_chat_msg_3);
    setHook((void*)0x0042D9EC, on_add_chat_msg_4);

    writeByte(0x0042D9F9, 0x18);
}
#pragma optimize( "", on )

//in UP:    26F46:	49	90 save chat
//          28385:	8B	E9 load chat
