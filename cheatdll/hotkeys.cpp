#include "stdafx.h"

#ifndef _CHEATDLL_CC

const char hotkeys[] = {
     1,  7,  6, //monitor
     2,  8,  3, //interceptor        4 -> 3
     3,  8,  4, //attacker           5 -> 4
     4, 10,  4, //transport mech
     5, 18,  3, //vornskyr
     6,  5,  7, //aa battery
     7,  5,  8, //mine
     8, 13,  1, //cargo freighter
     9, 12,  2, //nightsister hunter 3 -> 2
    10, 12,  1  //new master
};

void* hotkeyTestRet;
void* hotkeyTestCont;

__declspec(naked) void hotkeyTest() //(00563010, 005625D0) <- offsets not used
{
    __asm
    {
        push    ecx
        mov     eax, [esp + 8]
        mov     byte ptr [esp + 3], 0
        cmp     eax, 10             // 9 -> 10
        ja      _noHK
        test    eax, eax
        jz      _noHK
        lea     edx, [esp + 3]
        push    edx
        lea     edx, [esp + 3]
        push    edx
        lea     edx, [esp + 3]
        push    edx
        dec     eax
        lea     eax, [eax + eax * 2]
        mov     edx, offset hotkeys
        add     edx, eax
        add     edx, 2
        mov     al, byte ptr [edx]
        push    eax                 //id
        dec     edx
        mov     al, byte ptr [edx]
        push    eax                 //group

        mov     eax, hotkeyTestRet
        push    eax
        mov     eax, 00486B20h
        jmp     eax
_noHK:
        mov     edx, hotkeyTestCont
        jmp     edx
    }
}

__declspec(naked) void hotkeyTestUnit() //00563010
{
    __asm
    {
        mov     eax, 005631F0h
        mov     hotkeyTestRet, eax
        mov     eax, 0056301Ah
        mov     hotkeyTestCont, eax
        jmp     hotkeyTest
    }
}

__declspec(naked) void hotkeyTestBldg() //005625D0
{
    __asm
    {
        mov     eax, 00562620h
        mov     hotkeyTestRet, eax
        mov     eax, 005625DAh
        mov     hotkeyTestCont, eax
        jmp     hotkeyTest
    }
}

#pragma optimize( "s", on )
void setGroupNumbers()
{
    writeByte(0x00561462, 9);  //5
    writeByte(0x00561478, 7);  //7
    writeByte(0x00561483, 5);  //8  6 -> 5
    writeByte(0x00561499, 5);  //10
    writeByte(0x005614AF, 3);  //12 4 -> 3
    writeByte(0x005614BA, 2);  //13
    writeByte(0x005614F1, 4);  //18
}
#pragma optimize( "", on )

__declspec(naked) void hotkeyOptionsLoad() //005625C1
{
    __asm
    {
        push    edi
        mov     edi, 00486DA0h

        push    17499        //monitor
        push    6            //id
        push    7            //group
        mov     ecx, esi
        call    edi
        push    17508        //interceptor
        push    3            //id           4 -> 3
        push    8            //group
        mov     ecx, esi
        call    edi
        push    461          //attacker
        push    4            //id           5 -> 4
        push    8            //group
        mov     ecx, esi
        call    edi
        push    17553        //transport mech
        push    4            //id
        push    10           //group
        mov     ecx, esi
        call    edi
        push    17482        //vornskyr
        push    3            //id
        push    18           //group
        mov     ecx, esi
        call    edi
        push    973          //aa battery
        push    7            //id
        push    5            //group
        mov     ecx, esi
        call    edi
        push    17563        //mine
        push    8            //id
        push    5            //group
        mov     ecx, esi
        call    edi
        push    70           //cargo freighter
        push    1            //id
        push    13           //group
        mov     ecx, esi
        call    edi
        push    78           //nightsister hunter
        push    2            //id           3 -> 2
        push    12           //group
        mov     ecx, esi
        call    edi

        pop     edi
        pop     esi
        ret     4
    }
}

__declspec(naked) void hotkeyDefaultSet() //00561C72
{
    __asm
    {
        push    edi
        mov     edi, 00563BB0h

        push    2
        push    12h
        mov     ecx, esi
        call    edi

        push    6            //monitor
        push    7
        mov     ecx, esi
        call    edi

        push    3            //interceptor          4 -> 3
        push    8
        mov     ecx, esi
        call    edi

        push    4            //attacker             5 -> 4
        push    8
        mov     ecx, esi
        call    edi

        push    4            //transport mech
        push    10
        mov     ecx, esi
        call    edi

        push    7            //aa battery
        push    5
        mov     ecx, esi
        call    edi

        push    8            //mine
        push    5
        mov     ecx, esi
        call    edi

        push    3            //vornskr
        push    18
        mov     ecx, esi
        call    edi

        push    1            //cargo freighter
        push    13
        mov     ecx, esi
        call    edi

        push    2            //nightsister hunter 3 -> 2
        push    12
        mov     ecx, esi
        call    edi

        pop     edi
        pop     esi
        ret
    }
}

__declspec(naked) void hotkeyDefaultMonitor()
{
    __asm
    {
        push    16378
        push    0
        push    0
        push    0
        push    45h
        push    6
        push    7
        mov     eax, 00486BC0h
        call    eax
        retn    8
    }
}

__declspec(naked) void hotkeyDefaultInterceptor()
{
    __asm
    {
        push    16379
        push    0
        push    0
        push    0
        push    57h
        push    3        //4 -> 3
        push    8
        mov     eax, 00486BC0h
        call    eax
        retn    8
    }
}

__declspec(naked) void hotkeyDefaultAttacker()
{
    __asm
    {
        push    16380
        push    0
        push    0
        push    0
        push    46h
        push    4        //5 -> 4
        push    8
        mov     eax, 00486BC0h
        call    eax
        retn    8
    }
}

__declspec(naked) void hotkeyDefaultTransportMech()
{
    __asm
    {
        push    16381
        push    0
        push    0
        push    0
        push    57h
        push    4
        push    10
        mov     eax, 00486BC0h
        call    eax
        retn    8
    }
}

__declspec(naked) void hotkeyDefaultAABattery()
{
    __asm
    {
        push    16382
        push    0
        push    0
        push    0
        push    52h
        push    7
        push    5
        mov     eax, 00486BC0h
        call    eax
        retn    8
    }
}

__declspec(naked) void hotkeyDefaultMine()
{
    __asm
    {
        push    16383
        push    0
        push    0
        push    0
        push    43h
        push    8
        push    5
        mov     eax, 00486BC0h
        call    eax
        retn    8
    }
}

__declspec(naked) void hotkeyDefaultVornskr()
{
    __asm
    {
        push    16384
        push    0
        push    0
        push    0
        push    57h
        push    3
        push    18
        mov     eax, 00486BC0h
        call    eax
        retn    8
    }
}

__declspec(naked) void hotkeyDefaultCargoFreighter()
{
    __asm
    {
        push    16364
        push    0
        push    0
        push    0
        push    51h
        push    1
        push    13
        mov     eax, 00486BC0h
        call    eax
        retn    8
    }
}

__declspec(naked) void hotkeyDefaultNightsisterHunter()
{
    __asm
    {
        push    16305
        push    0
        push    0
        push    0
        push    44h
        push    2        //3 -> 2
        push    12
        mov     eax, 00486BC0h
        call    eax
        retn    8
    }
}

const DWORD hotkeyDefaultsGroup5[] =  //build defense
{
    0x005644CA,
    0x005644E3,
    0x005644FC,
    0x00564515,
    0x0056452E,
    0x00564547,
    0x00564560,
    (DWORD)&hotkeyDefaultAABattery,
    (DWORD)&hotkeyDefaultMine
};

const DWORD hotkeyDefaultsGroup7[] =  //shipyard
{
    0x0056461E,
    0x00564637,
    0x00564650,
    0x00564669,
    0x00564682,
    0x0056469B,
    (DWORD)&hotkeyDefaultMonitor
};

const DWORD hotkeyDefaultsGroup8[] =  //airbase
{
    0x005646C8,
    0x005646E1,
    0x005646FA,
    //0x00564713,
    (DWORD)&hotkeyDefaultInterceptor,
    (DWORD)&hotkeyDefaultAttacker
};

const DWORD hotkeyDefaultsGroup10[] =  //mech factory
{
    0x005647B8,
    0x005647D1,
    0x005647EA,
    0x00564803,
    (DWORD)&hotkeyDefaultTransportMech
};

//const DWORD hotkeyDefaultsGroup12[] =  //jedi temple
//{

//};

const DWORD hotkeyDefaultsGroup13[] =  //spaceport
{
    0x005648CA,
    (DWORD)&hotkeyDefaultCargoFreighter
};

__declspec(naked) void group18_fix() //00564ACF
{
    __asm
    {
        sub     eax, 0
        jz      _id_0
        dec     eax
        jz      _id_1
        dec     eax
        jz      _id_2
        dec     eax
        jnz     _no_id
        jmp     hotkeyDefaultVornskr
_id_0:
        mov     eax, 00564B0Ch
        jmp     eax
_id_1:
        mov     eax, 00564AF3h
        jmp     eax
_id_2:
        mov     eax, 00564ADAh
        jmp     eax
_no_id:
        ret     8
    }
}

__declspec(naked) void group13() //005648C2
{
    __asm
    {
        cmp     eax, 1
        ja      _locret_13
        jmp     hotkeyDefaultsGroup13[eax*4]
_locret_13:
        ret     8
    }
}

__declspec(naked) void group12() //00564883
{
    __asm
    {
        sub     eax, 1
        jz      _jump_2
        cmp     eax, 1
        jz      hotkeyDefaultNightsisterHunter
        ret     8
_jump_2:
        mov     eax, 0056488Ch
        jmp     eax
    }
}

#endif

#pragma optimize( "s", on )
void setHotkeysHooks()
{
#ifndef _CHEATDLL_CC
    setGroupNumbers();

    setHook((void*)0x005625C1, hotkeyOptionsLoad);
    setHook((void*)0x00563010, hotkeyTestUnit);
    setHook((void*)0x005625D0, hotkeyTestBldg);

    setHook((void*)0x00561C72, hotkeyDefaultSet);

    writeByte(0x005644BC, 8);
    writeDword(0x005644C6, (DWORD)hotkeyDefaultsGroup5);

    writeByte(0x00564610, 6);
    writeDword(0x0056461A, (DWORD)hotkeyDefaultsGroup7);

    writeByte(0x005646BA, 4);    //5 -> 4
    writeDword(0x005646C4, (DWORD)hotkeyDefaultsGroup8);

    writeByte(0x005647AA, 4);
    writeDword(0x005647B4, (DWORD)hotkeyDefaultsGroup10);

    setHook((void*)0x00564ACF, group18_fix);

    setHook((void*)0x005648C2, group13);
    setHook((void*)0x00564883, group12);
    //new master id: 2 -> 1
    writeByte(0x00562467, 1);
    writeByte(0x0056489A, 1);
    //*.hki -> *.hk2
    writeByte(0x0068EF6F, 0x32);
#endif
}
#pragma optimize( "", on )
