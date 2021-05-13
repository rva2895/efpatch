#include "stdafx.h"

#include "votepanel.h"

//votes:
//0 - wait
//1 - nonexistent
//2 - continue
//3 - save
//4 - dropped
//5 - waiting (?)
#define ST_WAIT 0
#define ST_NONE 1
#define ST_CONT 2
#define ST_SAVE 3
#define ST_DROP 4
#define ST_PEND 5

int getVoteResult(int votes[8])
{
    int s = 0;
    int c = 0;
    int w = 0;
    int n = 0;
    for (int i = 0; i < 8; i++)
    {
        switch (votes[i])
        {
        case ST_WAIT:
            w++;
            break;
        case ST_CONT:
            c++;
            break;
        case ST_SAVE:
            s++;
            break;
        default:
            break;
        }
    }
    n = s + c + w;
    if (w > n / 2)
        return ST_WAIT;
    else if (s > n / 2)
        return ST_SAVE;
    else if (c > n / 2)
        return ST_CONT;
    else
        return ST_WAIT;
}

int __fastcall getGameCanContinue2(int votes[8])
{
    return getVoteResult(votes) == ST_CONT;
}

int __fastcall getGameCanSaveAndExit2(int votes[8])
{
    return getVoteResult(votes) == ST_SAVE;
}

__declspec(naked) void getGameCanContinue_fix2() //0046F030
{
    __asm
    {
        mov     esi, ecx
        mov     ecx, edx
        call    getGameCanContinue2
        mov     ecx, esi
        test    eax, eax
        jz      _cannot_continue
        mov     eax, 0046F055h
        jmp     eax
_cannot_continue:
        mov     eax, 0046F1AEh
        jmp     eax
    }
}

__declspec(naked) void getGameCanSaveAndExit_fix2() //0046F1CE
{
    __asm
    {
        call    getGameCanSaveAndExit2
        test    eax, eax
        jz      _cannot_save
        mov     eax, 0046F1E7h
        jmp     eax
_cannot_save:
        mov     eax, 0046F231h
        jmp     eax
    }
}

__declspec(naked) void statusCheck_fix1() //0046E008
{
    __asm
    {
        mov     [esp + 19Ch], esi
        inc     esi
        mov     eax, 0046E00Fh
        jmp     eax
    }
}

__declspec(naked) void statusCheck_fix2() //0046EC0A
{
    __asm
    {
        test    eax, eax
        jz      short loc_46EC79
        xor     esi, esi
        inc     esi
        mov     eax, 0046EC10h
        jmp     eax
loc_46EC79:
        mov     eax, 0046EC79h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setVotePanelHooks()
{
    setHook((void*)0x0046F030, getGameCanContinue_fix2);
    setHook((void*)0x0046F1CE, getGameCanSaveAndExit_fix2);

    //setHook((void*)0x0046EC0E, statusCheck_fix);

    //writeByte(0x0046E072, 9); ???
    //writeByte (0x0043AF44, 0x40);

    setHook((void*)0x0046E008, statusCheck_fix1);
    writeByte(0x0046E072, 9);
    setHook((void*)0x0046EC0A, statusCheck_fix2);
    writeByte(0x0046EC25, 9);
    
    //save and exit 31 F6 46 90 90 90
    writeDword(0x0046EFC7, 0x90909046);
    writeByte(0x046EFC5, 0x31);
    writeByte(0x046EFC6, 0xF6);
}
#pragma optimize( "", on )
