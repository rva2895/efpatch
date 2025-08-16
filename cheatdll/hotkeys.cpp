#include "stdafx.h"
#include "hotkeys.h"
#include "farm.h"
#include "harbor.h"

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
    10, 12,  1, //new master
    11,  3, 15, //sensor buoy
    12,  3, 16, //underwater prefab shelter
    13,  7,  7, //at-at swimmer
    14,  3, 17  //harbor
};

void* hotkeyTestRet;
void* hotkeyTestCont;

bool set_ef_hotkeys = false;

__declspec(naked) void hotkeyTest() //(00563010, 005625D0) <- offsets not used
{
    __asm
    {
        push    ecx
        mov     eax, [esp + 8]
        mov     byte ptr [esp + 3], 0
        cmp     eax, 16478          // sensor buoy
        jz      HK_buoy
        cmp     eax, 16001          // underwater prefab shelter
        jz      HK_underwater
        jmp     HK_continue

HK_buoy:
        mov     eax, 11
        jmp     HK_continue

HK_underwater:
        mov     edx, hotkeyTestRet
        cmp     edx, 00562620h
        jnz     HK_continue
        mov     eax, 12

HK_continue:
        cmp     eax, 14
        ja      HK_skip
        test    eax, eax
        jz      HK_skip
        lea     edx, [esp + 3]
        push    edx
        lea     edx, [esp + 7]
        push    edx
        lea     edx, [esp + 11]
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

HK_skip:
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

void __stdcall TRIBE_Hotkey_Handler__setup_new(TRIBE_Hotkey_Handler* hh_t, TRIBE_World* world)
{
    RGE_Hotkey_Handler* hh = (RGE_Hotkey_Handler*)hh_t;
    RGE_Hotkey_Handler__set_num_hotkey_groups(hh, set_ef_hotkeys ? 20 : 19);
    RGE_Hotkey_Handler__set_hotkey_groups(hh, 0, 17);   //unit commands
    RGE_Hotkey_Handler__set_hotkey_groups(hh, 1, set_ef_hotkeys ? 99 : 98); //game commands
    RGE_Hotkey_Handler__set_hotkey_groups(hh, 2, 16);
    RGE_Hotkey_Handler__set_hotkey_groups(hh, 3, set_ef_hotkeys ? 18 : 17); //build economic
    RGE_Hotkey_Handler__set_hotkey_groups(hh, 4, 7);
    RGE_Hotkey_Handler__set_hotkey_groups(hh, 5, set_ef_hotkeys ? 9 : 7);
    RGE_Hotkey_Handler__set_hotkey_groups(hh, 6, 5);
    RGE_Hotkey_Handler__set_hotkey_groups(hh, 7, set_ef_hotkeys ? 8 : 6);
    RGE_Hotkey_Handler__set_hotkey_groups(hh, 8, set_ef_hotkeys ? 5 : 4);
    RGE_Hotkey_Handler__set_hotkey_groups(hh, 9, 4);
    RGE_Hotkey_Handler__set_hotkey_groups(hh, 10, set_ef_hotkeys ? 5 : 4);
    RGE_Hotkey_Handler__set_hotkey_groups(hh, 11, 3);
    RGE_Hotkey_Handler__set_hotkey_groups(hh, 12, 3);
    RGE_Hotkey_Handler__set_hotkey_groups(hh, 13, set_ef_hotkeys ? 2 : 1);
    RGE_Hotkey_Handler__set_hotkey_groups(hh, 14, 12);
    RGE_Hotkey_Handler__set_hotkey_groups(hh, 15, 4);
    RGE_Hotkey_Handler__set_hotkey_groups(hh, 16, set_ef_hotkeys ? 2 : 1);
    RGE_Hotkey_Handler__set_hotkey_groups(hh, 17, 1);
    RGE_Hotkey_Handler__set_hotkey_groups(hh, 18, set_ef_hotkeys ? 4 : 3);
    if (set_ef_hotkeys)
        RGE_Hotkey_Handler__set_hotkey_groups(hh, 19, 2);

    RGE_Hotkey_Handler__set_hotkey_groups_name(hh, 0, 20000);
    RGE_Hotkey_Handler__set_hotkey_groups_name(hh, 1, 20001);
    RGE_Hotkey_Handler__set_hotkey_groups_name(hh, 2, 20002);
    RGE_Hotkey_Handler__set_hotkey_groups_name(hh, 3, 20003);
    RGE_Hotkey_Handler__set_hotkey_groups_name(hh, 4, 20016);
    RGE_Hotkey_Handler__set_hotkey_groups_name(hh, 5, 20017);
    RGE_Hotkey_Handler__set_hotkey_groups_name(hh, 6, 20004);
    RGE_Hotkey_Handler__set_hotkey_groups_name(hh, 7, 20007);
    RGE_Hotkey_Handler__set_hotkey_groups_name(hh, 8, 20008);
    RGE_Hotkey_Handler__set_hotkey_groups_name(hh, 9, 20009);
    RGE_Hotkey_Handler__set_hotkey_groups_name(hh, 10, 20010);
    RGE_Hotkey_Handler__set_hotkey_groups_name(hh, 11, 20011);
    RGE_Hotkey_Handler__set_hotkey_groups_name(hh, 12, 20012);
    RGE_Hotkey_Handler__set_hotkey_groups_name(hh, 13, 20013);
    RGE_Hotkey_Handler__set_hotkey_groups_name(hh, 14, 20014);
    RGE_Hotkey_Handler__set_hotkey_groups_name(hh, 15, 20015);
    RGE_Hotkey_Handler__set_hotkey_groups_name(hh, 16, 20005);
    RGE_Hotkey_Handler__set_hotkey_groups_name(hh, 17, 20018);
    RGE_Hotkey_Handler__set_hotkey_groups_name(hh, 18, 20019);
    if (set_ef_hotkeys)
        RGE_Hotkey_Handler__set_hotkey_groups_name(hh, 19, 20020);

    TRIBE_Hotkey_Handler__init_hotkey_names(hh_t, world);
    TRIBE_Hotkey_Handler__default_hotkeys(hh_t);
}

void __stdcall TRIBE_Hotkey_Handler__default_hotkeys_add(TRIBE_Hotkey_Handler* hh)
{
    TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 18, 2);

    if (set_ef_hotkeys)
    {
        TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 7, 6);   //monitor
        TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 7, 7);   //at-at swimmer
        TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 8, 3);   //interceptor
        TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 8, 4);   //attacker
        TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 10, 4);  //transport mech
        TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 5, 7);   //aa battery
        TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 5, 8);   //mine
        TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 18, 3);  //vornskr
        TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 13, 1);  //cargo freighter
        TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 12, 2);  //nightsister hunter

        TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 16, 1);  //farm auto queue
        TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 19, 0);  //aqua harvester add queue
        TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 19, 1);  //aqua harvester auto queue

        TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 1, 0x62); //select all harbors

        TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 3, 0x11); //build harbor
    }

    TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 1, 0x55);    //select all idle workers
    TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 1, 0x56);    //select all idle military
    TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 1, 0x57);    //select all command centers
    TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 1, 0x58);    //select all power cores
    TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 1, 0x59);    //select all animal nurseries
    TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 1, 0x5A);    //select all spaceports
    TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 1, 0x5B);    //select all troop centers
    TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 1, 0x5C);    //select all shipyards
    TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 1, 0x5D);    //select all mech factories
    TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 1, 0x5E);    //select all temples
    TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 1, 0x5F);    //select all hvy wep factories
    TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 1, 0x60);    //select all airbases
    TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 1, 0x61);    //select all fortresses

    TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 0, 0x10);    //shift delete

    TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 3, 0xF);     //build sensor buoy
    TRIBE_Hotkey_Handler__setDefaultHotkey(hh, 3, 0x10);    //build underwater prefab shelter
}

void __stdcall TRIBE_Hotkey_Handler__init_hotkey_names_add(TRIBE_Hotkey_Handler* hh_t)
{
    RGE_Hotkey_Handler* hh = (RGE_Hotkey_Handler*)hh_t;
    if (set_ef_hotkeys)
    {
        RGE_Hotkey_Handler__set_hotkey_name(hh, 7, 6, 17499);   //monitor
        RGE_Hotkey_Handler__set_hotkey_name(hh, 7, 7, 7799);    //at-at swimmer
        RGE_Hotkey_Handler__set_hotkey_name(hh, 8, 3, 17508);   //interceptor
        RGE_Hotkey_Handler__set_hotkey_name(hh, 8, 4, 461);     //attacker
        RGE_Hotkey_Handler__set_hotkey_name(hh, 10, 4, 17553);  //transport mech
        RGE_Hotkey_Handler__set_hotkey_name(hh, 18, 3, 17482);  //vornskyr
        RGE_Hotkey_Handler__set_hotkey_name(hh, 5, 7, 973);     //aa battery
        RGE_Hotkey_Handler__set_hotkey_name(hh, 5, 8, 17563);   //mine
        RGE_Hotkey_Handler__set_hotkey_name(hh, 13, 1, 70);     //cargo freighter
        RGE_Hotkey_Handler__set_hotkey_name(hh, 12, 2, 78);     //nightsister hunter

        RGE_Hotkey_Handler__set_hotkey_name(hh, 16, 1, 19074);  //farm auto queue
        RGE_Hotkey_Handler__set_hotkey_name(hh, 19, 0, 19073);  //aqua harvester add queue
        RGE_Hotkey_Handler__set_hotkey_name(hh, 19, 1, 19075);  //aqua harvester auto queue

        RGE_Hotkey_Handler__set_hotkey_name(hh, 1, 0x62, 4364); //select all harbors

        RGE_Hotkey_Handler__set_hotkey_name(hh, 3, 0x11, 5383); //build harbor
    }

    RGE_Hotkey_Handler__set_hotkey_name(hh, 1, 0x55, 4351);     //select all idle workers
    RGE_Hotkey_Handler__set_hotkey_name(hh, 1, 0x56, 4352);     //select all idle military
    RGE_Hotkey_Handler__set_hotkey_name(hh, 1, 0x57, 4353);     //select all command centers
    RGE_Hotkey_Handler__set_hotkey_name(hh, 1, 0x58, 4354);     //select all power cores
    RGE_Hotkey_Handler__set_hotkey_name(hh, 1, 0x59, 4355);     //select all animal nurseries
    RGE_Hotkey_Handler__set_hotkey_name(hh, 1, 0x5A, 4356);     //select all spaceports
    RGE_Hotkey_Handler__set_hotkey_name(hh, 1, 0x5B, 4357);     //select all troop centers
    RGE_Hotkey_Handler__set_hotkey_name(hh, 1, 0x5C, 4358);     //select all shipyards
    RGE_Hotkey_Handler__set_hotkey_name(hh, 1, 0x5D, 4359);     //select all mech factories
    RGE_Hotkey_Handler__set_hotkey_name(hh, 1, 0x5E, 4360);     //select all temples
    RGE_Hotkey_Handler__set_hotkey_name(hh, 1, 0x5F, 4361);     //select all hvy wep factories
    RGE_Hotkey_Handler__set_hotkey_name(hh, 1, 0x60, 4362);     //select all airbases
    RGE_Hotkey_Handler__set_hotkey_name(hh, 1, 0x61, 4363);     //select all fortresses

    RGE_Hotkey_Handler__set_hotkey_name(hh, 0, 0x10, 4350);     //shift delete

    RGE_Hotkey_Handler__set_hotkey_name(hh, 3, 0xF, 5478);      //build sensor buoy
    RGE_Hotkey_Handler__set_hotkey_name(hh, 3, 0x10, 5001);     //build underwater prefab shelter
}

void __stdcall TRIBE_Hotkey_Handler__setDefaultHotkey_new(TRIBE_Hotkey_Handler* hh_t, int groupID, int hotkey)
{
    RGE_Hotkey_Handler* hh = (RGE_Hotkey_Handler*)hh_t;
    switch (groupID)
    {
    case 0:
        switch (hotkey)
        {
        case 0:
            RGE_Hotkey_Handler__set_hotkey(hh, 0, 0, 66, 0, 0, 0, 4151);
            break;
        case 1:
            RGE_Hotkey_Handler__set_hotkey(hh, 0, 1, 86, 0, 0, 0, 4154);
            break;
        case 2:
            RGE_Hotkey_Handler__set_hotkey(hh, 0, 2, 82, 0, 0, 0, 4173);
            break;
        case 5:
            RGE_Hotkey_Handler__set_hotkey(hh, 0, 5, 83, 0, 0, 0, 4152);
            break;
        case 6:
            RGE_Hotkey_Handler__set_hotkey(hh, 0, 6, 76, 0, 0, 0, 4174);
            break;
        case 7:
            RGE_Hotkey_Handler__set_hotkey(hh, 0, 7, 80, 0, 0, 0, 4171);
            break;
        case 8:
            RGE_Hotkey_Handler__set_hotkey(hh, 0, 8, 85, 0, 0, 0, 4172);
            break;
        case 9:
            RGE_Hotkey_Handler__set_hotkey(hh, 0, 9, 89, 0, 0, 0, 4168);
            break;
        case 10:
            RGE_Hotkey_Handler__set_hotkey(hh, 0, 10, 67, 0, 0, 0, 4169);
            break;
        case 11:
            RGE_Hotkey_Handler__set_hotkey(hh, 0, 11, 71, 0, 0, 0, 4188);
            break;
        case 12:
            RGE_Hotkey_Handler__set_hotkey(hh, 0, 12, 46, 0, 0, 0, -1);
            break;
        case 13:
            RGE_Hotkey_Handler__set_hotkey(hh, 0, 13, 73, 0, 0, 0, 4156);
            break;
        case 14:
            RGE_Hotkey_Handler__set_hotkey(hh, 0, 14, 84, 0, 0, 0, 4158);
            break;
        case 15:
            RGE_Hotkey_Handler__set_hotkey(hh, 0, 15, 78, 0, 0, 0, 4181);
            break;
        case 16:    //shift delete
            RGE_Hotkey_Handler__set_hotkey(hh, 0, 16, VK_DELETE, 0, 0, 1, -1);
            
            break;
        default:
            return;
        }
        break;
    case 1:
        switch (hotkey)
        {
        case 20:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 20, 13, 0, 0, 0, -1);
            break;
        case 21:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 21, 107, 0, 0, 0, -1);
            break;
        case 22:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 22, 109, 0, 0, 0, -1);
            break;
        case 23:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 23, 190, 0, 0, 0, -1);
            break;
        case 24:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 24, 251, 0, 0, 0, -1);
            break;
        case 25:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 25, 36, 0, 0, 0, -1);
            break;
        case 26:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 26, 253, 0, 0, 0, -1);
            break;
        case 27:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 27, 32, 0, 0, 0, -1);
            break;
        case 28:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 28, 72, 0, 0, 0, 4159);
            break;
        case 29:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 29, 252, 0, 0, 0, -1);
            break;
        case 30:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 30, 83, 1u, 0, 0, 4163);
            break;
        case 31:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 31, 33, 0, 0, 0, -1);
            break;
        case 32:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 32, 34, 0, 0, 0, -1);
            break;
        case 33:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 33, 115, 0, 0, 0, -1);
            break;
        case 34:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 34, 86, 1u, 0, 0, 4161);
            break;
        case 35:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 35, 84, 1u, 0, 0, 4153);
            break;
        case 36:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 36, 77, 1u, 0, 0, 4165);
            break;
        case 37:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 37, 72, 1u, 0, 0, 4164);
            break;
        case 38:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 38, 89, 1u, 0, 0, 4162);
            break;
        case 39:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 39, 74, 1u, 0, 0, 4166);
            break;
        case 40:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 40, 87, 1u, 0, 0, 4184);
            break;
        case 41:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 41, 70, 1u, 0, 0, 4185);
            break;
        case 42:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 42, 82, 1u, 0, 0, 4167);
            break;
        case 43:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 43, 113, 0, 0, 0, -1);
            break;
        case 45:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 45, 122, 0, 0, 0, -1);
            break;
        case 46:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 46, 188, 0, 0, 0, -1);
            break;
        case 47:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 47, 0, 0, 0, 0, -1);
            break;
        case 48:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 48, 70, 0, 1u, 0, 4170);
            break;
        case 49:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 49, 68, 1u, 0, 0, 4183);
            break;
        case 50:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 50, 79, 1u, 0, 0, 4187);
            break;
        case 51:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 51, 67, 1u, 0, 0, 4189);
            break;
        case 52:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 52, 67, 0, 1u, 0, 4192);
            break;
        case 53:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 53, 82, 0, 1u, 0, 4193);
            break;
        case 54:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 54, 78, 0, 1u, 0, 4194);
            break;
        case 57:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 57, 68, 0, 1u, 0, 4195);
            break;
        case 59:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 59, 79, 0, 1u, 0, 4196);
            break;
        case 60:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 60, 84, 0, 1u, 0, 4197);
            break;
        case 61:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 61, 114, 0, 0, 0, -1);
            break;
        case 62:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 62, 123, 0, 0, 0, -1);
            break;
        case 63:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 63, 71, 0, 1u, 0, 4700);
            break;
        case 64:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 64, 8, 0, 0, 0, -1);
            break;
        case 65:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 65, 120, 0, 0, 0, -1);
            break;
        case 66:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 66, 74, 0, 0, 0, 4198);
            break;
        case 67:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 67, 191, 0, 0, 0, -1);
            break;
        case 68:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 68, 0, 0, 0, 0, -1);
            break;
        case 69:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 69, 78, 1u, 0, 0, 4190);
            break;
        case 70:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 70, 65, 1u, 0, 0, 4199);
            break;
        case 71:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 71, 77, 0, 1u, 0, 4200);
            break;
        case 72:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 72, 66, 1u, 0, 0, 4150);
            break;
        case 73:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 73, 80, 1u, 0, 0, 16012);
            break;
        case 0x55:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 0x55, VK_OEM_PERIOD, 0, 0, 1, -1);
            break;
        case 0x56:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 0x56, VK_OEM_COMMA, 0, 0, 1, -1);
            break;
        case 0x57:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 0x57, 'H', 0, 0, 1, 4159);
            break;
        case 0x58:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 0x58, 'P', 1, 0, 1, 4171);
            break;
        case 0x59:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 0x59, 'B', 1, 0, 1, 4151);
            break;
        case 0x5A:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 0x5A, 'S', 1, 0, 1, 4163);
            break;
        case 0x5B:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 0x5B, 'T', 1, 0, 1, 4153);
            break;
        case 0x5C:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 0x5C, 'Y', 1, 0, 1, 4162);
            break;
        case 0x5D:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 0x5D, 'M', 1, 0, 1, 4157);
            break;
        case 0x5E:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 0x5E, 'J', 1, 0, 1, 4166);
            break;
        case 0x5F:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 0x5F, 'H', 1, 0, 1, 4159);
            break;
        case 0x60:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 0x60, 'A', 1, 0, 1, 4175);
            break;
        case 0x61:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 0x61, 'D', 1, 0, 1, 4176);
            break;
        case 0x62:
            RGE_Hotkey_Handler__set_hotkey(hh, 1, 0x62, 'R', 1, 0, 1, 4167);
            break;
        default:
            return;
        }
        break;
    case 2:
        switch (hotkey)
        {
        case 0:
            RGE_Hotkey_Handler__set_hotkey(hh, 2, 0, 37, 0, 0, 0, -1);
            break;
        case 1:
            RGE_Hotkey_Handler__set_hotkey(hh, 2, 1, 39, 0, 0, 0, -1);
            break;
        case 2:
            RGE_Hotkey_Handler__set_hotkey(hh, 2, 2, 38, 0, 0, 0, -1);
            break;
        case 3:
            RGE_Hotkey_Handler__set_hotkey(hh, 2, 3, 40, 0, 0, 0, -1);
            break;
        default:
            return;
        }
        break;
    case 3:
        switch (hotkey)
        {
        case 0:
            RGE_Hotkey_Handler__set_hotkey(hh, 3, 0, 81, 0, 0, 0, 16313);
            break;
        case 1:
            RGE_Hotkey_Handler__set_hotkey(hh, 3, 1, 87, 0, 0, 0, 16301);
            break;
        case 2:
            RGE_Hotkey_Handler__set_hotkey(hh, 3, 2, 65, 0, 0, 0, 16353);
            break;
        case 3:
            RGE_Hotkey_Handler__set_hotkey(hh, 3, 3, 90, 0, 0, 0, 16346);
            break;
        case 4:
            RGE_Hotkey_Handler__set_hotkey(hh, 3, 4, 69, 0, 0, 0, 16376);
            break;
        case 5:
            RGE_Hotkey_Handler__set_hotkey(hh, 3, 5, 86, 0, 0, 0, 16319);
            break;
        case 6:
            RGE_Hotkey_Handler__set_hotkey(hh, 3, 6, 67, 0, 0, 0, 16344);
            break;
        case 7:
            RGE_Hotkey_Handler__set_hotkey(hh, 3, 7, 84, 0, 0, 0, 16339);
            break;
        case 8:
            RGE_Hotkey_Handler__set_hotkey(hh, 3, 8, 83, 0, 0, 0, 16309);
            break;
        case 9:
            RGE_Hotkey_Handler__set_hotkey(hh, 3, 9, 68, 0, 0, 0, 16305);
            break;
        case 10:
            RGE_Hotkey_Handler__set_hotkey(hh, 3, 10, 71, 0, 0, 0, 16377);
            break;
        case 11:
            RGE_Hotkey_Handler__set_hotkey(hh, 3, 11, 66, 0, 0, 0, 4155);
            break;
        case 12:
            RGE_Hotkey_Handler__set_hotkey(hh, 3, 12, 82, 0, 0, 0, 16302);
            break;
        case 13:
            RGE_Hotkey_Handler__set_hotkey(hh, 3, 13, 70, 0, 0, 0, 16303);
            break;
        case 14:
            RGE_Hotkey_Handler__set_hotkey(hh, 3, 14, 88, 0, 0, 0, 16340);
            break;
        case 15:    //sensor buoy
            RGE_Hotkey_Handler__set_hotkey(hh, 3, 15, 0x59, 0, 0, 0, 4162);
            break;
        case 16:    //underwater prefab shelter
            RGE_Hotkey_Handler__set_hotkey(hh, 3, 16, 0x4E, 0, 0, 0, 4160);
            break;
        case 17:    //harbor
            RGE_Hotkey_Handler__set_hotkey(hh, 3, 17, 'U', 0, 0, 0, 4172);
            break;
        default:
            return;
        }
        break;
    case 4:
        switch (hotkey)
        {
        case 0:
            RGE_Hotkey_Handler__set_hotkey(hh, 4, 0, 87, 0, 0, 0, 16349);
            break;
        case 1:
            RGE_Hotkey_Handler__set_hotkey(hh, 4, 1, 81, 0, 0, 0, 16364);
            break;
        case 2:
            RGE_Hotkey_Handler__set_hotkey(hh, 4, 2, 65, 0, 0, 0, 16361);
            break;
        case 3:
            RGE_Hotkey_Handler__set_hotkey(hh, 4, 3, 83, 0, 0, 0, 16357);
            break;
        case 4:
            RGE_Hotkey_Handler__set_hotkey(hh, 4, 4, 90, 0, 0, 0, 16367);
            break;
        case 5:
            RGE_Hotkey_Handler__set_hotkey(hh, 4, 5, 86, 0, 0, 0, 16317);
            break;
        case 6:
            RGE_Hotkey_Handler__set_hotkey(hh, 4, 6, 88, 0, 0, 0, 16350);
            break;
        default:
            return;
        }
        break;
    case 5:     //build defense
        switch (hotkey)
        {
        case 0:
            RGE_Hotkey_Handler__set_hotkey(hh, 5, 0, 65, 0, 0, 0, 16371);
            break;
        case 1:
            RGE_Hotkey_Handler__set_hotkey(hh, 5, 1, 83, 0, 0, 0, 16369);
            break;
        case 2:
            RGE_Hotkey_Handler__set_hotkey(hh, 5, 2, 87, 0, 0, 0, 16372);
            break;
        case 3:
            RGE_Hotkey_Handler__set_hotkey(hh, 5, 3, 69, 0, 0, 0, 16375);
            break;
        case 4:
            RGE_Hotkey_Handler__set_hotkey(hh, 5, 4, 68, 0, 0, 0, 16304);
            break;
        case 5:
            RGE_Hotkey_Handler__set_hotkey(hh, 5, 5, 81, 0, 0, 0, 16318);
            break;
        case 6:
            RGE_Hotkey_Handler__set_hotkey(hh, 5, 6, 90, 0, 0, 0, 16306);
            break;
        case 7:     //aa battery
            RGE_Hotkey_Handler__set_hotkey(hh, 5, 7, 0x52, 0, 0, 0, 16382);
            break;
        case 8:     //mine
            RGE_Hotkey_Handler__set_hotkey(hh, 5, 8, 0x43, 0, 0, 0, 16383);
            break;
        default:
            return;
        }
        break;
    case 6:
        switch (hotkey)
        {
        case 0:
            RGE_Hotkey_Handler__set_hotkey(hh, 6, 0, 65, 0, 0, 0, 16092);
            break;
        case 1:
            RGE_Hotkey_Handler__set_hotkey(hh, 6, 1, 66, 0, 0, 0, 4191);
            break;
        case 2:
            RGE_Hotkey_Handler__set_hotkey(hh, 6, 2, 87, 0, 0, 0, 4709);
            break;
        case 3:
            RGE_Hotkey_Handler__set_hotkey(hh, 6, 3, 83, 0, 0, 0, 16091);
            break;
        case 4:
            RGE_Hotkey_Handler__set_hotkey(hh, 6, 4, 68, 0, 0, 0, 16090);
            break;
        default:
            return;
        }
        break;
    case 7:     //shipyard
        switch (hotkey)
        {
        case 0:
            RGE_Hotkey_Handler__set_hotkey(hh, 7, 0, 81, 0, 0, 0, 16200);
            break;
        case 1:
            RGE_Hotkey_Handler__set_hotkey(hh, 7, 1, 68, 0, 0, 0, 16201);
            break;
        case 2:
            RGE_Hotkey_Handler__set_hotkey(hh, 7, 2, 83, 0, 0, 0, 16202);
            break;
        case 3:
            RGE_Hotkey_Handler__set_hotkey(hh, 7, 3, 70, 0, 0, 0, 16204);
            break;
        case 4:
            RGE_Hotkey_Handler__set_hotkey(hh, 7, 4, 65, 0, 0, 0, 16205);
            break;
        case 5:
            RGE_Hotkey_Handler__set_hotkey(hh, 7, 5, 87, 0, 0, 0, 16707);
            break;
        case 6:
            RGE_Hotkey_Handler__set_hotkey(hh, 7, 6, 0x45, 0, 0, 0, 16378);
            break;
        case 7:
            RGE_Hotkey_Handler__set_hotkey(hh, 7, 7, 0x52, 0, 0, 0, 16302);
            break;
        default:
            return;
        }
        break;
    case 8:     //airbase
        switch (hotkey)
        {
        case 0:
            RGE_Hotkey_Handler__set_hotkey(hh, 8, 0, 65, 0, 0, 0, 16008);
            break;
        case 1:
            RGE_Hotkey_Handler__set_hotkey(hh, 8, 1, 83, 0, 0, 0, 16013);
            break;
        case 2:
            RGE_Hotkey_Handler__set_hotkey(hh, 8, 2, 68, 0, 0, 0, 16006);
            break;
        case 3:
            if (set_ef_hotkeys) //interceptor
                RGE_Hotkey_Handler__set_hotkey(hh, 8, 3, 0x57, 0, 0, 0, 16379);
            else    //awing
                RGE_Hotkey_Handler__set_hotkey(hh, 8, 3, 86, 0, 0, 0, 16007);
            break;
        case 4:     //attacker
            RGE_Hotkey_Handler__set_hotkey(hh, 8, 4, 0x46, 0, 0, 0, 16380);
            break;
        default:
            return;
        }
        break;
    case 9:
        switch (hotkey)
        {
        case 0:
            RGE_Hotkey_Handler__set_hotkey(hh, 9, 0, 70, 0, 0, 0, 16068);
            break;
        case 1:
            RGE_Hotkey_Handler__set_hotkey(hh, 9, 1, 65, 0, 0, 0, 16071);
            break;
        case 2:
            RGE_Hotkey_Handler__set_hotkey(hh, 9, 2, 68, 0, 0, 0, 16057);
            break;
        case 3:
            RGE_Hotkey_Handler__set_hotkey(hh, 9, 3, 83, 0, 0, 0, 16032);
            break;
        default:
            return;
        }
        break;
    case 10:    //mech factory
        switch (hotkey)
        {
        case 0:
            RGE_Hotkey_Handler__set_hotkey(hh, 10, 0, 65, 0, 0, 0, 16003);
            break;
        case 1:
            RGE_Hotkey_Handler__set_hotkey(hh, 10, 1, 83, 0, 0, 0, 16049);
            break;
        case 2:
            RGE_Hotkey_Handler__set_hotkey(hh, 10, 2, 68, 0, 0, 0, 16046);
            break;
        case 3:
            RGE_Hotkey_Handler__set_hotkey(hh, 10, 3, 70, 0, 0, 0, 16047);
            break;
        case 4:
            RGE_Hotkey_Handler__set_hotkey(hh, 10, 4, 0x57, 0, 0, 0, 16381);
            break;
        default:
            return;
        }
        break;
    case 11:
        if (hotkey)
        {
            if (hotkey == 1)
            {
                RGE_Hotkey_Handler__set_hotkey(hh, 11, 1, 83, 0, 0, 0, 16080);
            }
            else if (hotkey == 2)
            {
                RGE_Hotkey_Handler__set_hotkey(hh, 11, 2, 68, 0, 0, 0, 16087);
            }
        }
        else
        {
            RGE_Hotkey_Handler__set_hotkey(hh, 11, 0, 65, 0, 0, 0, 16081);
        }
        break;
    case 12:    //temple
        switch (hotkey)
        {
        case 0:
            RGE_Hotkey_Handler__set_hotkey(hh, 12, 0, 65, 0, 0, 0, 16033);
            break;
        case 1:
            if (set_ef_hotkeys)
                RGE_Hotkey_Handler__set_hotkey(hh, 12, 1, 83, 0, 0, 0, 16035);
            break;
        case 2:
            if (set_ef_hotkeys)
                RGE_Hotkey_Handler__set_hotkey(hh, 12, 2, 0x44, 0, 0, 0, 16305);
            else
                RGE_Hotkey_Handler__set_hotkey(hh, 12, 2, 83, 0, 0, 0, 16035);
            break;
        default:
            return;
        }
        break;
    case 13:    //spaceport
        switch (hotkey)
        {
        case 0:
            RGE_Hotkey_Handler__set_hotkey(hh, 13, 0, 65, 0, 0, 0, 16038);
            break;
        case 1:
            RGE_Hotkey_Handler__set_hotkey(hh, 13, 1, 0x51, 0, 0, 0, 16364);
            break;
        default:
            return;
        }
        break;
    case 14:
        switch (hotkey)
        {
        case 1:
            RGE_Hotkey_Handler__set_hotkey(hh, 14, 1, 87, 0, 0, 0, 4701);
            break;
        case 2:
            RGE_Hotkey_Handler__set_hotkey(hh, 14, 2, 81, 0, 0, 0, 4702);
            break;
        case 3:
            RGE_Hotkey_Handler__set_hotkey(hh, 14, 3, 69, 0, 0, 0, 4703);
            break;
        case 4:
            RGE_Hotkey_Handler__set_hotkey(hh, 14, 4, 70, 0, 0, 0, 4704);
            break;
        case 5:
            RGE_Hotkey_Handler__set_hotkey(hh, 14, 5, 90, 0, 0, 0, 4180);
            break;
        case 6:
            RGE_Hotkey_Handler__set_hotkey(hh, 14, 6, 88, 0, 0, 0, 4178);
            break;
        case 7:
            RGE_Hotkey_Handler__set_hotkey(hh, 14, 7, 75, 0, 0, 0, 4179);
            break;
        case 8:
            RGE_Hotkey_Handler__set_hotkey(hh, 14, 8, 65, 0, 0, 0, 4175);
            break;
        case 9:
            RGE_Hotkey_Handler__set_hotkey(hh, 14, 9, 68, 0, 0, 0, 4176);
            break;
        case 10:
            RGE_Hotkey_Handler__set_hotkey(hh, 14, 10, 77, 0, 0, 0, 4157);
            break;
        case 11:
            RGE_Hotkey_Handler__set_hotkey(hh, 14, 11, 79, 0, 0, 0, 4177);
            break;
        default:
            return;
        }
        break;
    case 15:
        switch (hotkey)
        {
        case 0:
            RGE_Hotkey_Handler__set_hotkey(hh, 15, 0, 83, 0, 0, 0, 16065);
            break;
        case 1:
            RGE_Hotkey_Handler__set_hotkey(hh, 15, 1, 65, 0, 0, 0, 16004);
            break;
        case 2:
            RGE_Hotkey_Handler__set_hotkey(hh, 15, 2, 68, 0, 0, 0, 16001);
            break;
        case 3:
            RGE_Hotkey_Handler__set_hotkey(hh, 15, 3, 70, 0, 0, 0, 16002);
            break;
        default:
            return;
        }
        break;
    case 16:    //food proc cntr
        switch (hotkey)
        {
        case 0:
            RGE_Hotkey_Handler__set_hotkey(hh, 16, 0, 70, 0, 0, 0, 4710);
            break;
        case 1:
            RGE_Hotkey_Handler__set_hotkey(hh, 16, 1, 'T', 0, 0, 0, 4153);
            break;
        default:
            return;
        }
        break;
    case 17:
        if (!hotkey)
            RGE_Hotkey_Handler__set_hotkey(hh, 17, 0, 65, 0, 0, 0, 16005);
        break;
    case 18:
        switch (hotkey)
        {
        case 0:
            RGE_Hotkey_Handler__set_hotkey(hh, 18, 0, 65, 0, 0, 0, 16009);
            break;
        case 1:
            RGE_Hotkey_Handler__set_hotkey(hh, 18, 1, 83, 0, 0, 0, 16010);
            break;
        case 2:
            RGE_Hotkey_Handler__set_hotkey(hh, 18, 2, 68, 0, 0, 0, 16011);
            break;
        case 3:
            RGE_Hotkey_Handler__set_hotkey(hh, 18, 3, 0x57, 0, 0, 0, 16384);
            break;
        default:
            return;
        }
        break;
    case 19:    //harbor
        switch (hotkey)
        {
        case 0:
            RGE_Hotkey_Handler__set_hotkey(hh, 19, 0, 'G', 0, 0, 0, 4188);
            break;
        case 1:
            RGE_Hotkey_Handler__set_hotkey(hh, 19, 1, 'T', 0, 0, 0, 4153);
            break;
        default:
            return;
        }
        break;
    default:
        return;
    }
}

int __stdcall TRIBE_Hotkey_Handler__get_misc_key_add(TRIBE_Hotkey_Handler* hh_t, int hotkey)
{
    RGE_Hotkey_Handler* hh = (RGE_Hotkey_Handler*)hh_t;
    unsigned __int8 dummy = 0;
    switch (hotkey)
    {
    case AUTO_FARM_QUEUE_ENABLE_BUTTON:
    case AUTO_FARM_QUEUE_DISABLE_BUTTON:
        return RGE_Hotkey_Handler__get_hotkey(hh, 16, 1, &dummy, &dummy, &dummy);
    case AUTO_AQUA_HARVESTER_QUEUE_ENABLE_BUTTON:
    case AUTO_AQUA_HARVESTER_QUEUE_DISABLE_BUTTON:
        return RGE_Hotkey_Handler__get_hotkey(hh, 19, 1, &dummy, &dummy, &dummy);
    case ADD_AQUA_HARVESTER_QUEUE_BUTTON:
        return RGE_Hotkey_Handler__get_hotkey(hh, 19, 0, &dummy, &dummy, &dummy);
    default:
        return TRIBE_Hotkey_Handler__get_misc_key(hh_t, hotkey);
    }
}

__declspec(naked) void TRIBE_Hotkey_Handler__default_hotkeys_add_wr() //00561C72
{
    __asm
    {
        push    esi
        call    TRIBE_Hotkey_Handler__default_hotkeys_add
        pop     esi
        ret
    }
}

__declspec(naked) void TRIBE_Hotkey_Handler__init_hotkey_names_add_wr()  //005625C1
{
    __asm
    {
        push    esi
        call    TRIBE_Hotkey_Handler__init_hotkey_names_add
        pop     esi
        ret     4
    }
}

__declspec(naked) void TRIBE_Hotkey_Handler__setup_wr() //00561420
{
    __asm
    {
        mov     eax, [esp + 4]
        push    eax
        push    ecx
        call    TRIBE_Hotkey_Handler__setup_new
        ret     4
    }
}

__declspec(naked) void TRIBE_Hotkey_Handler__setDefaultHotkey_wr() //00563BB0
{
    __asm
    {
        mov     eax, [esp + 4]
        mov     edx, [esp + 8]
        push    edx
        push    eax
        push    ecx
        call    TRIBE_Hotkey_Handler__setDefaultHotkey_new
        ret     8
    }
}

__declspec(naked) void TRIBE_Hotkey_Handler__get_misc_key_wr() //004FE2F2
{
    __asm
    {
        push    edx
        push    ecx
        call    TRIBE_Hotkey_Handler__get_misc_key_add
        mov     ebx, 004FE2F8h
        jmp     ebx
    }
}

bool __stdcall game_hotkey_dispatch(int hotkey, TRIBE_Screen_Game* this_)
{
    int id_to_search = -1;
    RGE_Static_Object* first_unit;
    RGE_Static_Object* unit;
    int unit_count = 0;
    int play_sound = 1;
    RGE_Player* player = RGE_Base_Game__get_player(*base_game);
    switch (hotkey)
    {
    case 0x55: //idle workers
        first_unit = TRIBE_Screen_Game__find_next_idle_unit(this_, 0x7FFFFFFF);
        if (first_unit)
        {
            RGE_Player__unselect_object(player);
            unit = first_unit;
            do
            {
                if ((RGE_Player*)unit->owner == player &&
                    (unit->master_obj->master_type == 70 ||
                        (unit->master_obj->master_type == 80 && (unit->master_obj->object_group == 34 || unit->master_obj->object_group == 36))))
                {
                    RGE_Player__select_one_object(player, unit, play_sound);
                    play_sound = 0;
                }
                unit_count++;
                unit = TRIBE_Screen_Game__find_next_idle_unit(this_, unit->id);
            } while (unit_count < 40 && unit && unit != first_unit);
            ((TPanel*)this_->main_view)->vfptr->set_redraw((TPanel*)this_->main_view, 1);
        }
        else
        {
            RGE_Base_Game__play_sound(*base_game, 3, 0, 0);
        }
        return true;
        break;
    case 0x56: //idle military
        first_unit = TRIBE_Screen_Game__find_next_idle_military_unit(this_, 0x7FFFFFFF);
        if (first_unit)
        {
            RGE_Player__unselect_object(player);
            unit = first_unit;
            do
            {
                if ((RGE_Player*)unit->owner == player &&
                    (unit->master_obj->master_type == 70 ||
                        (unit->master_obj->master_type == 80 && (unit->master_obj->object_group == 34 || unit->master_obj->object_group == 36))))
                {
                    RGE_Player__select_one_object(player, unit, play_sound);
                    play_sound = 0;
                }
                unit_count++;
                unit = TRIBE_Screen_Game__find_next_idle_military_unit(this_, unit->id);
            } while (unit_count < 40 && unit && unit != first_unit);
            ((TPanel*)this_->main_view)->vfptr->set_redraw((TPanel*)this_->main_view, 1);
        }
        else
        {
            RGE_Base_Game__play_sound(*base_game, 3, 0, 0);
        }
        return true;
        break;
    case 0x57: //command center
        id_to_search = 0x6D;
        break;
    case 0x58: //power core
        id_to_search = 0xC;
        break;
    case 0x59: //animal nursery
        id_to_search = 0x13F;
        break;
    case 0x5A: //spaceport
        id_to_search = 0x54;
        break;
    case 0x5B: //troop center
        id_to_search = 0x57;
        break;
    case 0x5C: //shipyard
        id_to_search = 0x2D;
        break;
    case 0x5D: //mech factory
        id_to_search = 0x65;
        break;
    case 0x5E: //temple
        id_to_search = 0x68;
        break;
    case 0x5F: //hvy wep factory
        id_to_search = 0x31;
        break;
    case 0x60: //airbase
        id_to_search = 0x13D;
        break;
    case 0x61: //fortress
        id_to_search = 0x52;
        break;
    case 0x62: //harbor
        id_to_search = HARBOR_ID_COMPLETE;
        break;
    default:
        break;
    }
    if (id_to_search != -1)
    {
        first_unit = TRIBE_Player__find_obj((TRIBE_Player*)player, id_to_search, NULL, -1);
        if (first_unit)
        {
            RGE_Player__unselect_object(player);
            unit = first_unit;
            do
            {
                RGE_Player__select_one_object(player, unit, play_sound);
                play_sound = 0;
                unit_count++;
                unit = TRIBE_Player__find_obj((TRIBE_Player*)player, id_to_search, unit, -1);
            } while (unit_count < 40 && unit != first_unit);
            ((TPanel*)this_->main_view)->vfptr->set_redraw((TPanel*)this_->main_view, 1);
        }
        else
        {
            RGE_Base_Game__play_sound(*base_game, 3, 0, 0);
        }
        return true;
    }
    return false;
}

__declspec(naked) void dispatch_hotkey_game() //004F9F79
{
    __asm
    {
        add     eax, 0FFFFFFECh
        cmp     eax, 35h
        ja      new_game_hotkeys_query
        mov     edx, 004F9F85h
        jmp     edx

new_game_hotkeys_query:
        add     eax, 20
        push    ebp
        push    eax
        call    game_hotkey_dispatch
        test    al, al
        jz      new_game_hotkey_not_queried
        mov     eax, 004F9F93h
        jmp     eax
new_game_hotkey_not_queried:
        mov     eax, 004FA66Bh
        jmp     eax
    }
}

TButtonPanel* hotkey_setup_allow_dup_checkbox = NULL;
TTextPanel* hotkey_setup_allow_dup_text = NULL;
bool hotkey_setup_allow_dup_flag = false;

void __stdcall hotkey_setup_allow_dup_create_checkbox(TRIBE_Screen_Options* screen)
{
    TEasy_Panel__create_check_box(
        (TEasy_Panel*)screen,
        (TPanel*)screen,
        &hotkey_setup_allow_dup_checkbox,
        105,
        415,
        28,
        28,
        0,
        5
    );

    TEasy_Panel__create_text6(
        (TEasy_Panel*)screen,
        (TPanel*)screen,
        &hotkey_setup_allow_dup_text,
        "Allow Duplicates",
        135,
        417,
        190,
        30,
        0,
        0,
        0,
        0);

    hotkey_setup_allow_dup_flag = false;
}

void __stdcall hotkey_setup_allow_dup_delete_checkbox(TRIBE_Screen_Options* screen)
{
    TPanel__delete_panel((TPanel*)screen, (TPanel**)&hotkey_setup_allow_dup_checkbox);
    TPanel__delete_panel((TPanel*)screen, (TPanel**)&hotkey_setup_allow_dup_text);

    hotkey_setup_allow_dup_flag = false;
}

void __stdcall hotkey_setup_allow_dup_checkbox_action(TRIBE_Screen_Options* screen)
{
    hotkey_setup_allow_dup_flag = TButtonPanel__get_state(hotkey_setup_allow_dup_checkbox);
}

bool __stdcall hotkey_setup_allow_dup_test()
{
    return hotkey_setup_allow_dup_flag;
}

__declspec(naked) void hotkey_setup_allow_dup_1() //00527654
{
    __asm
    {
        push    esi
        call    hotkey_setup_allow_dup_create_checkbox
        mov     ecx, [esi + 83Ch]
        mov     eax, 0052765Ah
        jmp     eax
    }
}

__declspec(naked) void hotkey_setup_allow_dup_2() //00527043
{
    __asm
    {
        push    esi
        call    hotkey_setup_allow_dup_create_checkbox
        mov     ecx, [esi + 83Ch]
        mov     eax, 00527049h
        jmp     eax
    }
}

__declspec(naked) void hotkey_setup_allow_dup_3() //005277F0
{
    __asm
    {
        push    esi
        call    hotkey_setup_allow_dup_delete_checkbox
        mov     ecx, [esi + 7F4h]
        mov     eax, 005277F6h
        jmp     eax
    }
}

__declspec(naked) void hotkey_setup_allow_dup_4()
{
    __asm
    {
        push    esi
        call    hotkey_setup_allow_dup_checkbox_action
        mov     eax, 1
        mov     ecx, 00527E34h
        jmp     ecx
    }
}

const DWORD hotkey_setup_allow_dup_action_dispatch[] =
{
    0x527B52,
    0x527C47,
    0x527B67,
    0x527E0E,
    0x527C07,
    (DWORD)hotkey_setup_allow_dup_4
};

__declspec(naked) void hotkey_setup_allow_dup_5() //0056389D
{
    __asm
    {
        call    hotkey_setup_allow_dup_test
        test    al, al
        jz      hotkey_setup_allow_dup_false
        mov     edx, 005639E3h
        jmp     edx

hotkey_setup_allow_dup_false:
        mov     eax, [ebp + 0]
        mov     ecx, ebx
        push    eax
        mov     edx, 005638A3h
        jmp     edx
    }
}

void __stdcall TRIBE_Command__command_shift_delete(TRIBE_Command* this_, RGE_Static_Object** units)
{
    int unit_count = 0;
    RGE_Player* player = RGE_Base_Game__get_player(*base_game);
    for (int i = 0; i < 40; i++)
        if (units[i])
            if ((RGE_Player*)units[i]->owner == player)
                unit_count++;
            else
                units[i] = NULL;
    if (unit_count > 0)
    {
        int order_size = 4 + 4 * unit_count;
        void* cmd = calloc_internal(1, order_size);
        if (cmd)
        {
            *(uint8_t*)cmd = 0x82;
            uint8_t issuer = player->id;
            *((uint8_t*)cmd + 1) = issuer;
            *((uint16_t*)cmd + 1) = unit_count;
            uint32_t* uid_ptr = ((uint32_t*)cmd + 1);
            for (int i = 0; i < 40; i++)
                if (units[i])
                {
                    *uid_ptr = units[i]->id;
                    uid_ptr++;
                }
            RGE_Command__submit((RGE_Command*)this_, cmd, order_size, issuer);
        }
    }
}

__declspec(naked) void __fastcall GameScreen__command_shift_delete(void* this_)
{
    __asm
    {
        sub     esp, 0A4h
        push    ebx
        push    ebp
        mov     [esp + 0A8h], ecx
        push    esi
        push    edi
        lea     edi, [esp + 10h]
        xor     eax, eax
        mov     ecx, 40
        rep stosd
        mov     ecx, base_game
        mov     ecx, [ecx]
        call    RGE_Base_Game__get_player
        mov     edi, eax
        xor     ebx, ebx
        lea     ebp, [edi + 1C8h]

loc_5015BE:
        mov     eax, [edi + 26Ch]
        test    eax, eax
        jle     loc_501661
        mov     esi, [ebp + 0]
        test    esi, esi
        jz      short loc_50163B
        mov     ecx, [esi + 18h]
        cmp     ecx, edi
        jz      short loc_501608
        cmp     byte ptr [ecx + 0A6h], 2
        jnz     short loc_50163B
        mov     edx, [edi + 0A0h]
        mov     eax, [ecx]
        push    edx
        call    dword ptr [eax + 24h]
        test    eax, eax
        jz      short loc_50163B
        mov     ecx, [esi + 18h]
        mov     eax, [edi]
        mov     edx, [ecx + 0A0h]
        mov     ecx, edi
        push    edx
        call    dword ptr [eax + 24h]
        test    eax, eax
        jz      short loc_50163B

loc_501608:
        mov     ecx, [esi + 14h]
        mov     ax, [ecx + 1Eh]
        cmp     ax, 1
        jz      short loc_50163B
        cmp     ax, 1Ch
        jz      short loc_50163B
        cmp     ax, 15h
        jz      short loc_50163B
        cmp     word ptr [ecx + 18h], 32h
        jnz     short loc_50164E
        mov     eax, [esi + 18h]
        mov     ecx, [eax + 0A0h]
        mov     eax, [edi + 0A0h]
        cmp     ecx, eax
        jz      short loc_50164E

loc_50163B:
        inc     ebx
        add     ebp, 4
        cmp     ebx, 28h
        jl      loc_5015BE

        //make tribe command
        lea     eax, [esp + 10h]
        push    eax
        mov     edx, [esp + 0B4h]
        mov     eax, [edx + 126Ch]
        mov     ecx, [eax + 68h]
        push    ecx
        call    TRIBE_Command__command_shift_delete
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        add     esp, 0A4h
        retn

loc_50164E:
        //add unit ptr
        mov     [esp + ebx * 4 + 10h], esi
        jmp     loc_50163B

loc_501661:
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        add     esp, 0A4h
        retn
    }
}

__declspec(naked) void dispatch_hotkey_unit() //004FA68F
{
    __asm
    {
        mov     eax, 00486C80h
        call    eax
        cmp     eax, 0Ch
        jz      delete_single
        cmp     eax, 10h
        jz      delete_multi
        mov     eax, 004FA6BBh
        jmp     eax

delete_single:
        mov     eax, 004FA699h
        jmp     eax

delete_multi:
        mov     eax, [esp + 10h]
        test    eax, eax
        jnz     loc_4FA6B1
        mov     eax, 00690D40h
        mov     eax, [eax]
        test    eax, eax
        jz      loc_4FA6B1
        mov     ecx, ebp
        call    GameScreen__command_shift_delete

loc_4FA6B1:
        mov     eax, 004FA6B1h
        jmp     eax
    }
}

void __stdcall shift_delete_do_command(void* this_, void* order)
{
    uint32_t player_id = *((uint8_t*)order + 1);
    uint32_t n = *((uint16_t*)order + 1);
    for (int i = 0; i < n; i++)
    {
        TRIBE_Combat_Object* unit = (TRIBE_Combat_Object*)RGE_Game_World__object(*((RGE_Game_World**)this_ + 1), *((uint32_t*)order + 1 + i));
        if (unit && unit->master_obj->master_type >= 70 && unit->owner->id == player_id)
            unit->vfptr->cancel_object(unit);
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

#pragma optimize( "s", on )
void setHotkeysHooks(int version)
{
    if (version == VER_EF)
    {
        //new master id: 2 -> 1
        writeByte(0x00562467, 1);
        //*.hki -> *.hk2
        writeByte(0x0068EF6F, 0x32);

        set_ef_hotkeys = true;
    }

    setHook((void*)0x00561C72, TRIBE_Hotkey_Handler__default_hotkeys_add_wr);
    setHook((void*)0x005625C1, TRIBE_Hotkey_Handler__init_hotkey_names_add_wr);
    setHook((void*)0x00561420, TRIBE_Hotkey_Handler__setup_wr);
    setHook((void*)0x00563BB0, TRIBE_Hotkey_Handler__setDefaultHotkey_wr);
    setHook((void*)0x004FE2F2, TRIBE_Hotkey_Handler__get_misc_key_wr);

    setHook((void*)0x00563010, hotkeyTestUnit);
    setHook((void*)0x005625D0, hotkeyTestBldg);

    setHook((void*)0x004F9F79, dispatch_hotkey_game);
    setHook((void*)0x004FA68F, dispatch_hotkey_unit);

    //reset hotkey bug
    //writeByte(0x00527DC4, 0x18);
    //writeByte(0x00527DC0, 0x1C);
    //writeByte(0x00527DBC, 0x14);

    writeNops(0x00527CDE, 0x23);
    writeNops(0x00527DB9, 0x27);

    //allow duplicate hotkeys
    setHook((void*)0x00527654, hotkey_setup_allow_dup_1);
    setHook((void*)0x00527043, hotkey_setup_allow_dup_2);
    setHook((void*)0x005277F0, hotkey_setup_allow_dup_3);
    setHook((void*)0x0056389D, hotkey_setup_allow_dup_5);

    writeByte(0x00527B44, 5);
    writeDword(0x00527B4E, (DWORD)hotkey_setup_allow_dup_action_dispatch);
}
#pragma optimize( "", on )
