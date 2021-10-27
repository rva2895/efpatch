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

bool set_ef_hotkeys = false;

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

        mov     al, set_ef_hotkeys
        test    al, al
        jz      load_no_ef_hotkeys

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

load_no_ef_hotkeys:
        push    4351         //select all idle workers
        push    0x55         //id
        push    1            //group
        mov     ecx, esi
        call    edi

        push    4352         //select all idle military
        push    0x56         //id
        push    1            //group
        mov     ecx, esi
        call    edi

        push    4353         //select all command centers
        push    0x57         //id
        push    1            //group
        mov     ecx, esi
        call    edi

        push    4354         //select all power cores
        push    0x58         //id
        push    1            //group
        mov     ecx, esi
        call    edi

        push    4355         //select all animal nurseries
        push    0x59         //id
        push    1            //group
        mov     ecx, esi
        call    edi

        push    4356         //select all spaceports
        push    0x5A         //id
        push    1            //group
        mov     ecx, esi
        call    edi

        push    4357         //select all troop centers
        push    0x5B         //id
        push    1            //group
        mov     ecx, esi
        call    edi

        push    4358         //select all shipyards
        push    0x5C         //id
        push    1            //group
        mov     ecx, esi
        call    edi

        push    4359         //select all mech factories
        push    0x5D         //id
        push    1            //group
        mov     ecx, esi
        call    edi

        push    4360         //select all temples
        push    0x5E         //id
        push    1            //group
        mov     ecx, esi
        call    edi

        push    4361         //select all hvy wep factories
        push    0x5F         //id
        push    1            //group
        mov     ecx, esi
        call    edi

        push    4362         //select all airbases
        push    0x60         //id
        push    1            //group
        mov     ecx, esi
        call    edi

        push    4363         //select all fortresses
        push    0x61         //id
        push    1            //group
        mov     ecx, esi
        call    edi

        push    4350         //shift delete
        push    0x10         //id
        push    0            //group
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

        mov     al, set_ef_hotkeys
        test    al, al
        jz      default_no_ef_hotkeys

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

default_no_ef_hotkeys:
        push    0x55         //select all idle workers
        push    1
        mov     ecx, esi
        call    edi

        push    0x56         //select all idle military
        push    1
        mov     ecx, esi
        call    edi

        push    0x57         //select all command centers
        push    1
        mov     ecx, esi
        call    edi

        push    0x58         //select all power cores
        push    1
        mov     ecx, esi
        call    edi

        push    0x59         //select all animal nurseries
        push    1
        mov     ecx, esi
        call    edi

        push    0x5A         //select all spaceports
        push    1
        mov     ecx, esi
        call    edi

        push    0x5B         //select all troop centers
        push    1
        mov     ecx, esi
        call    edi

        push    0x5C         //select all shipyards
        push    1
        mov     ecx, esi
        call    edi

        push    0x5D         //select all mech factories
        push    1
        mov     ecx, esi
        call    edi

        push    0x5E         //select all temples
        push    1
        mov     ecx, esi
        call    edi

        push    0x5F         //select all hvy wep factories
        push    1
        mov     ecx, esi
        call    edi

        push    0x60         //select all airbases
        push    1
        mov     ecx, esi
        call    edi

        push    0x61         //select all fortresses
        push    1
        mov     ecx, esi
        call    edi

        push    0x10         //shift delete
        push    0
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

__declspec(naked) void hotkeyDefaultShiftDelete()
{
    __asm
    {
        push    -1
        push    1
        push    0
        push    0
        push    VK_DELETE
        push    0x10
        push    0
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

const DWORD hotkeyDefaultsGroup0[] =  //unit commands
{
    0x005640A1,
    0x005640BA,
    0x005640EC,
    0x00564B22,
    0x00564B22,
    0x00564105,
    0x0056411E,
    0x00564137,
    0x00564150,
    0x00564169,
    0x00564182,
    0x0056419B,
    0x005641B4,
    0x005641CA,
    0x005641E3,
    0x005640D3,
    (DWORD)&hotkeyDefaultShiftDelete
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

void (__thiscall* hotkey_set_default) (void* this_, int group, int id, int key_code, int ctrl_key, int alt_key, int shift_key, int string_id) =
    (void(__thiscall*) (void*, int, int, int, int, int, int, int))0x00486BC0;

void __stdcall defaultHotkeyGame_switch(int hotkey, void* this_)
{
    switch (hotkey)
    {
    case 0x55:
        hotkey_set_default(this_, 1, 0x55, VK_OEM_PERIOD, 0, 0, 1, -1);
        break;
    case 0x56:
        hotkey_set_default(this_, 1, 0x56, VK_OEM_COMMA, 0, 0, 1, -1);
        break;
    case 0x57:
        hotkey_set_default(this_, 1, 0x57, 'H', 0, 0, 1, 4159);
        break;
    case 0x58:
        hotkey_set_default(this_, 1, 0x58, 'P', 1, 0, 1, 4171);
        break;
    case 0x59:
        hotkey_set_default(this_, 1, 0x59, 'B', 1, 0, 1, 4151);
        break;
    case 0x5A:
        hotkey_set_default(this_, 1, 0x5A, 'S', 1, 0, 1, 4163);
        break;
    case 0x5B:
        hotkey_set_default(this_, 1, 0x5B, 'T', 1, 0, 1, 4153);
        break;
    case 0x5C:
        hotkey_set_default(this_, 1, 0x5C, 'Y', 1, 0, 1, 4162);
        break;
    case 0x5D:
        hotkey_set_default(this_, 1, 0x5D, 'M', 1, 0, 1, 4157);
        break;
    case 0x5E:
        hotkey_set_default(this_, 1, 0x5E, 'J', 1, 0, 1, 4166);
        break;
    case 0x5F:
        hotkey_set_default(this_, 1, 0x5F, 'H', 1, 0, 1, 4159);
        break;
    case 0x60:
        hotkey_set_default(this_, 1, 0x60, 'A', 1, 0, 1, 4175);
        break;
    case 0x61:
        hotkey_set_default(this_, 1, 0x61, 'D', 1, 0, 1, 4176);
        break;
    default:
        break;
    }
}

__declspec(naked) void onDefaultHotkey_game() //00563BC8
{
    __asm
    {
        add     eax, 0FFFFFFECh
        cmp     eax, 35h
        ja      new_game_hotkeys
        mov     edx, 00563BD4h
        jmp     edx

new_game_hotkeys:
        add     eax, 20
        push    ecx
        push    eax
        call    defaultHotkeyGame_switch
        retn    8
    }
}

__declspec(naked) void __stdcall hotkey_dispatch_scr_update(void* ptr)
{
    __asm
    {
        mov     ecx, [esp + 4]
        mov     ecx, [ecx + 10BCh]
        push    1
        mov     eax, [ecx]
        call    dword ptr [eax + 2Ch]
        retn    4
    }
}

bool __stdcall game_hotkey_dispatch(int hotkey, void* this_)
{
    int id_to_search = -1;
    UNIT* first_unit;
    UNIT* unit;
    int unit_count = 0;
    int play_sound = 1;
    void* player = BaseGame__get_player(*base_game);
    switch (hotkey)
    {
    case 0x55: //idle workers
        first_unit = (UNIT*)GameScreen__find_next_idle_unit(this_, 0x7FFFFFFF);
        if (first_unit)
        {
            WorldPlayerBase__unselect_object(player);
            unit = first_unit;
            do
            {
                if (unit->player == player &&
                    (unit->prop_object->type == 70 ||
                        (unit->prop_object->type == 80 && (unit->prop_object->unit_class == 34 || unit->prop_object->unit_class == 36))))
                {
                    WorldPlayerBase__select_one_object(player, unit, play_sound);
                    play_sound = 0;
                }
                unit_count++;
                unit = (UNIT*)GameScreen__find_next_idle_unit(this_, unit->ordinal);
            } while (unit_count < 40 && unit && unit != first_unit);
            hotkey_dispatch_scr_update(this_);
        }
        else
        {
            GameSoundEffectsManager__playSound(*base_game, 3, 0, 0);
        }
        return true;
        break;
    case 0x56: //idle military
        first_unit = (UNIT*)GameScreen__find_next_idle_military_unit(this_, 0x7FFFFFFF);
        if (first_unit)
        {
            WorldPlayerBase__unselect_object(player);
            unit = first_unit;
            do
            {
                if (unit->player == player &&
                    (unit->prop_object->type == 70 ||
                        (unit->prop_object->type == 80 && (unit->prop_object->unit_class == 34 || unit->prop_object->unit_class == 36))))
                {
                    WorldPlayerBase__select_one_object(player, unit, play_sound);
                    play_sound = 0;
                }
                unit_count++;
                unit = (UNIT*)GameScreen__find_next_idle_military_unit(this_, unit->ordinal);
            } while (unit_count < 40 && unit && unit != first_unit);
            hotkey_dispatch_scr_update(this_);
        }
        else
        {
            GameSoundEffectsManager__playSound(*base_game, 3, 0, 0);
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
    default:
        break;
    }
    if (id_to_search != -1)
    {
        first_unit = (UNIT*)WorldPlayer__find_obj(player, id_to_search, NULL, -1);
        if (first_unit)
        {
            WorldPlayerBase__unselect_object(player);
            unit = first_unit;
            do
            {
                WorldPlayerBase__select_one_object(player, unit, play_sound);
                play_sound = 0;
                unit_count++;
                unit = (UNIT*)WorldPlayer__find_obj(player, id_to_search, unit, -1);
            } while (unit_count < 40 && unit != first_unit);
            hotkey_dispatch_scr_update(this_);
        }
        else
        {
            GameSoundEffectsManager__playSound(*base_game, 3, 0, 0);
        }
        return true;
    }
    return false;
}

__declspec(naked) void onQueryCommand_game() //004F9F79
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

void __stdcall TRIBE_Command__command_shift_delete(void* this_, UNIT** units)
{
    int unit_count = 0;
    void* player = getCurrentPlayer();
    for (int i = 0; i < 40; i++)
        if (units[i])
            if (units[i]->player == player)
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
            uint8_t issuer = *(uint32_t*)((uint8_t*)player + 0xA0);
            *((uint8_t*)cmd + 1) = issuer;
            *((uint16_t*)cmd + 1) = unit_count;
            uint32_t* uid_ptr = ((uint32_t*)cmd + 1);
            for (int i = 0; i < 40; i++)
                if (units[i])
                {
                    *uid_ptr = units[i]->ordinal;
                    uid_ptr++;
                }
            RGE_Command__submit(this_, cmd, order_size, issuer);
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
        call    BaseGame__get_player
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
        UNIT* unit = (UNIT*)BaseWorld__object(*((void**)this_ + 1), *((uint32_t*)order + 1 + i));
        if (unit && unit->prop_object->type >= 70 && *(uint32_t*)((uint8_t*)unit->player + 0xA0) == player_id)
            kill_unit(unit);
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
#ifndef _CHEATDLL_CC
    if (version == VER_EF)
    {
        setGroupNumbers();
        
        setHook((void*)0x00563010, hotkeyTestUnit);
        setHook((void*)0x005625D0, hotkeyTestBldg);

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

        set_ef_hotkeys = true;
    }
#endif
    
    setHook((void*)0x005625C1, hotkeyOptionsLoad);
    setHook((void*)0x00561C72, hotkeyDefaultSet);

    setHook((void*)0x00563BC8, onDefaultHotkey_game);
    writeByte(0x00561436, 0x62); //game commands
    writeByte(0x0056142B, 0x11); //unit commands
    setHook((void*)0x004F9F79, onQueryCommand_game);

    writeByte(0x00564093, 0x10);
    writeDword(0x0056409D, (DWORD)hotkeyDefaultsGroup0);

    setHook((void*)0x004FA68F, dispatch_hotkey_unit);
}
#pragma optimize( "", on )
