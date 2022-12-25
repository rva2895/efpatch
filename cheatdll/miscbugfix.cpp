#include "stdafx.h"
#include "miscbugfix.h"

__declspec(naked) void annex_unit_crash_mitigation() //00555640
{
    __asm
    {
        mov     ecx, [esi + 1Ch]
        test    ecx, ecx
        jz      mitigate_annex_crash
        movsx   eax, ax
        mov     edx, 00555646h
        jmp     edx

mitigate_annex_crash:
        mov     edx, 005556C9h
        jmp     edx
    }
}

__declspec(naked) void darkside_cheat_fix() //0060429C
{
    __asm
    {
        xor     eax, eax
        mov     ax, [esi + 48h]
        cmp     edi, eax
        jge     darkside_cheat_bad_player
        mov     eax, [esi + 4Ch]
        mov     ecx, [eax + edi * 4]
        mov     eax, 006042A2h
        jmp     eax

darkside_cheat_bad_player:
        mov     eax, 006044CAh
        jmp     eax
    }
}

__declspec(naked) void error_txt_fopen_fix() //0048F7BD
{
    __asm
    {
        add     esp, 8
        test    eax, eax
        jz      skip_error_txt_write
        mov     esi, eax
        mov     eax, 0048F7C2h
        jmp     eax

skip_error_txt_write:
        mov     eax, 0048F7D7h
        jmp     eax
    }
}

__declspec(naked) void chat_stutter_fix() //004CD00A
{
    __asm
    {
        cmp     byte ptr [eax], 0
        jz      no_streaming_sound
        mov     ecx, 004CD012h
        jmp     ecx

no_streaming_sound:
        mov     ecx, 004CD192h
        jmp     ecx
    }
}

__declspec(naked) void random_sound_start() //00517484
{
    __asm
    {
        cmp     eax, 1Eh
        jz      random_sound_start_rnd
        cmp     eax, 1Fh
        jz      random_sound_start_rnd
        mov     ecx, 0051749Dh
        jmp     ecx

random_sound_start_rnd:
        mov     ecx, 00517489h
        jmp     ecx
    }
}

__declspec(naked) void random_sound_countdown() //0051B2A1
{
    __asm
    {
        cmp     eax, 1Eh
        jz      random_sound_countdown_rnd
        cmp     eax, 1Fh
        jz      random_sound_countdown_rnd
        mov     ecx, 0051B2BAh
        jmp     ecx

random_sound_countdown_rnd:
        mov     ecx, 0051B2A6h
        jmp     ecx
    }
}

#pragma optimize( "s", on )
void setMiscBugfixHooks()
{
    //read-only fix for data\*.dat
    writeDword(0x004D5B62, GENERIC_READ);

    //darkside cheat crash
    setHook((void*)0x0060429C, darkside_cheat_fix);

    //error.txt fix
    writeData(0x0068F14C, "error.txt", 10);
    setHook((void*)0x0048F7BD, error_txt_fopen_fix);

    //renderer fix (THIS_COD)
    BYTE nops[25];
    memset(nops, 0x90, 25);
    writeData(0x0064DC8D, nops, 25);

    //workaround for annex unit crash from state 0
    setHook((void*)0x00555640, annex_unit_crash_mitigation);

    //MP mouse lag
    writeByte(0x0049F686, 0x0C);
    writeWord(0x0049F906, 0x9090);
    writeByte(0x00429541, 0xEB);

    //wndproc loop delay
    writeDword(0x00425EA6, 0x0674023C);
    writeDword(0x00425EAA, 0x013C042C);
    writeByte(0x00425EAE, 0x77);

    //one player in MP
    writeByte(0x005154D9, 0xEB);
    writeByte(0x00519A06, 0xEB);

    //disabled ungrouped AI alliance
    writeByte(0x0061E2EB, 0xEB);

    //from old patch:
    //full map print ratios
    writeByte(0x0045D4BD, 8);
    writeByte(0x0045D4CA, 1);
    writeByte(0x0045D4DC, 0xFF);

    //post game view for sp
    writeByte(0x004F87AD, 0x4C);
    writeByte(0x004F89CE, 0xEB);
    writeByte(0x004F89CF, 0x1F);
    writeByte(0x004F89E1, 0xEB);
    writeByte(0x004F8A13, 0x66);
    writeByte(0x004F8A14, 0x90);

    //remove high graphics fambaa ring
    writeByte(0x0061F4A4, 0xEB);
    
    //chat stutter
    setHook((void*)0x004CD00A, chat_stutter_fix);

    //high civ UI fix
    writeByte(0x004FDB68, 0);

    //timeGetTime unsigned
    writeByte(0x004CD8EE, 0x72);
    writeByte(0x004F9980, 0x84);

    //chat display time
    writeDword(0x004CCAD0, 15000);

    //mirror random sound fix
    setHook((void*)0x00517484, random_sound_start);
    setHook((void*)0x0051B2A1, random_sound_countdown);
}
#pragma optimize( "", on )
