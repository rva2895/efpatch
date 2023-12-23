#include "stdafx.h"
#include "casts.h"
#include <iostream>
#include <vector>
#include <algorithm>

#include "processunit.h"
#include "dataload.h"

//TODO:
//         1: process getDamage funcion [done?]
//         2: add attacked units to vector [done]
//         3: save/load [done]
//         4: remove deleted units from vector [done]
//         5: remove units with expired effects from map [deprecated]
//         6: detect effects (speed) [done?]
//         7: detect effects (stealth) [done]
//         8: detect effects (reload time)
//         9: detect effects (hp drain) [done]

int current_save_game_version;

void __stdcall processUnitExtra(RGE_Static_Object* unit, int timerRes)
{
    UNIT_EXTRA* ud = getUnitExtra(unit);
    if (ud)
    {
        if (ud->speedReductionEnabled)
        {
            ud->speedReductionTime -= *(float*)&timerRes;
            if (ud->speedReductionTime <= 0)
            {
                ud->speedReductionTime = 0;
                ud->speedReductionEnabled = 0;
            }
        }
        if (ud->stealthOffEnabled)
        {
            ud->stealthOffTime -= *(float*)&timerRes;
            if (ud->stealthOffTime <= 0)
            {
                ud->stealthOffTime = 0;
                ud->stealthOffEnabled = 0;
            }
        }
        if (ud->reloadTimeEnabled)
        {
            ud->reloadTimeTime -= *(float*)&timerRes;
            if (ud->reloadTimeTime <= 0)
            {
                ud->reloadTimeTime = 0;
                ud->reloadTimeEnabled = 0;
            }
        }
        if (ud->hpDrainEnabled)
        {
            ud->hpDrainTime -= *(float*)&timerRes;
            if (ud->hpDrainTime <= 0)
            {
                ud->hpDrainTime = 0;
                ud->hpDrainEnabled = 0;
            }
            else
            {
                float maxHP = unit->master_obj->hp;
                float* hp = &unit->hp;
                ud->hpDrainLeftover += *(float*)&timerRes * ud->hpDrainPerSecond;
                float intp;
                modf(ud->hpDrainLeftover, &intp);
                if (abs(intp) > 0.0)
                {
                    ud->hpDrainLeftover -= intp;
                    *hp -= intp;
                    if (*hp > maxHP)
                        *hp = maxHP;
                }
            }
        }
        if (ud->hpDrainPercentEnabled)
        {
            ud->hpDrainPercentTime -= *(float*)&timerRes;
            if (ud->hpDrainPercentTime <= 0)
            {
                ud->hpDrainPercentTime = 0;
                ud->hpDrainPercentEnabled = 0;
            }
            else
            {
                float maxHP = unit->master_obj->hp;
                float* hp = &unit->hp;
                ud->hpDrainPercentLeftover +=
                    *(float*)&timerRes * ud->hpDrainPercentPerSecond*maxHP;
                float intp;
                modf(ud->hpDrainPercentLeftover, &intp);
                if (abs(intp) > 0.0)
                {
                    ud->hpDrainPercentLeftover -= intp;
                    *hp -= intp;
                    if (*hp > maxHP)
                        *hp = maxHP;
                }
            }
        }
        
        if (!ud->keepUnitExtra &&
            !ud->speedReductionEnabled &&
            !ud->stealthOffEnabled &&
            !ud->hpDrainEnabled &&
            !ud->hpDrainPercentEnabled &&
            !ud->reloadTimeEnabled &&
            !ud->countersUsed &&
            !ud->hasBeenPurged)
            removeUnitExtra(unit);
    }
}

__declspec(naked) void processUnitHook() //00444DA0, 0054EF00
{
    __asm
    {
        //mov     eax, [ecx+18h]
        //mov     eax, [eax+8Ch]
        //mov     eax, dword ptr [eax+0A8h]
        //push    esi
        //mov     esi, ecx
        //push    eax
        //push    ecx
        //call    processUnitExtra
        //sub     esp, 8
        //push    00444DA6h
        //ret

        push    ecx
        push    ebx
        xor     ebx, ebx
        mov     eax, [ecx + 18h]
        mov     eax, [eax + 8Ch]
        mov     eax, dword ptr [eax + 0A8h]
        push    esi
        mov     esi, ecx
        push    eax
        push    ecx
        call    processUnitExtra
        mov     eax, 0054EF07h
        jmp     eax
    }
}

uint32_t __stdcall getSpeedModifier(RGE_Static_Object* unit) //returns 32 bit float in eax
{
    UNIT_EXTRA* ud = getUnitExtra(unit);
    return (ud && ud->speedReductionEnabled) ? *(uint32_t*)&ud->speedReductionModifier : 0x3F800000; //1.0f
}

bool __stdcall getStealthOff(RGE_Static_Object* unit) //zero: stealth off disabled, non-zero: stealth off enabled
{
    UNIT_EXTRA* ud = getUnitExtra(unit);
    return (ud && ud->stealthOffEnabled);
}

RGE_Static_Object* targetUnit;
bool isDamage;

__declspec(naked) void getDamageGetUnit() //004449C3
{
    __asm
    {
        mov     edx, [esp + 10h]
        cmp     edx, 00444847h
        setz    dl
        mov     isDamage, dl
        mov     targetUnit, ecx
        mov     edx, [ecx + 14h]
        mov     eax, [ecx + 28h]
        push    ebx
        push    ebp
        mov     ebp, 004449CBh
        jmp     ebp
    }
}

void __stdcall specialDamage(RGE_Static_Object* unit, short type, int damage, int armor)
{
    if (!unit)
    {
        if (!isDamage)
            return;
        unit = targetUnit;
    }

    if (type == 1000)      //death star fix
        return;

    if (unit->master_obj->master_type < 70)
        return;

#ifdef _DEBUG
    int ltype = type;
    log("---SPECIAL DAMAGE---: type %d, dmg %d, armor %d", ltype, damage, armor);
#endif

    UNIT_EXTRA* ud = createUnitExtra(unit);

    if (armor == 1000)
        armor = 0;

    switch (type)
    {
    case 41: //speed reduction, value
        ud->speedReductionEnabled = true;
        ud->speedReductionModifier = 1.0f - (damage * (100.0f - (float)armor) / 100.0f) / 100.0f;
        break;
    case 42:
        ud->speedReductionEnabled = true;
        ud->speedReductionTime = (damage * (100.0f - (float)armor) / 100.0f) / 10.0f;
        break;
    case 43:
        ud->stealthOffEnabled = true;
        break;
    case 44:
        ud->stealthOffTime = (damage * (100.0f - (float)armor) / 100.0f) / 10.0f;
        break;
    case 45:
        //ud->stealthOffEnabled = 1;
        break;
    case 46:
        //ud->stealthOffTime = (float)(damage-armor) / 1000;
        break;
    case 47:
        ud->hpDrainEnabled = true;
        ud->hpDrainPerSecond = (damage * (100.0f - (float)armor) / 100.0f) / 60.0f;
        ud->hpDrainLeftover = 0.0f;
        break;
    case 48:
        ud->hpDrainTime = (damage * (100.0f - (float)armor) / 100.0f) / 10.0f;
        break;
    case 49:
        ud->hpDrainPercentEnabled = true;
        ud->hpDrainPercentPerSecond = (damage * (100.0f - (float)armor) / 100.0f) / 6000.0f;
        ud->hpDrainPercentLeftover = 0.0f;
        break;
    case 50:
        ud->hpDrainPercentTime = (damage * (100.0f - (float)armor) / 100.0f) / 10.0f;
        break;
    /*case 51:
        result_dmg = damage - armor;
        if (result_dmg < 0)
            result_dmg = 0;
        unit->sp -= (float)result_dmg;
        if (unit->sp < 0.0f)
            unit->sp = 0.0f;
        break;*/
    default:
        break;
    }
}

__declspec(naked) void onGetDamage() //00444A7C
{
    __asm
    {
        cmp     dx, 41
        jl      onGetDamage_end
        movsx   eax, word ptr [esi + 2]   //damage
        mov     ecx, [esp + 28h]          //armor
        push    0
        fst     [esp]
        push    edx                       //save edx
        push    ecx                       //armor
        push    eax                       //damage
        push    edx                       //type
        push    0                         //unit - 0 means use targetUnit
        call    specialDamage
        pop     edx
        fld     [esp]
        fstp    st
        add     esp, 4
        mov     eax, 00444AA9h
        jmp     eax
onGetDamage_end:
        movsx   eax, word ptr [esi + 2]
        mov     [esp + 1Ch], eax
        mov     eax, 00444A84h
        jmp     eax
    }
}

__declspec(naked) void speed1() //0044B7E2
{
    __asm
    {
        push    eax
        push    ecx
        push    edx
        push    esi
        call    getSpeedModifier
        sub     esp, 4
        mov     [esp], eax
        fld     dword ptr[esp]
        mov     edx, [esp + 4]
        mov     ecx, [esp + 8]
        mov     eax, [esp + 0Ch]
        fld     dword ptr [eax + 0D0h]
        fmul    st(1), st
        fstp    st
        add     esp, 10h
        mov     eax, 0044B7E8h
        jmp     eax
    }
}

__declspec(naked) void speed2() //004A28C0
{
    __asm
    {
        push    ecx
        push    ecx
        call    getSpeedModifier
        mov     ecx, [esp]
        mov     [esp], eax
        //fld     dword ptr [esp]
        //add     esp, 4

        mov     edx, [ecx + 1Ch]
        test    edx, edx
        jz      short loc_4A28E4
        fld     dword ptr [edx + 74h]
        _emit   0xDC//fcomp   ds:dbl_6546B0
        _emit   0x1D
        _emit   0xB0
        _emit   0x46
        _emit   0x65
        _emit   0x00
        fnstsw  ax
        test    ah, 41h
        jnz     short loc_4A28E4
        mov     eax, [ecx + 14h]
        fld     dword ptr [esp]
        fld     dword ptr [eax + 0D0h]
        fmul    st(1), st
        fstp    st
        add     esp, 4
        fmul    dword ptr [edx + 74h]
        retn
        //.text:004A28E4 ; ---------------------------------------------------------------------------
        //.text:004A28E4
loc_4A28E4:
        mov     ecx, [ecx + 14h]
        fld     dword ptr [esp]
        fld     dword ptr [ecx + 0D0h]
        fmul    st(1), st
        fstp    st
        add     esp, 4
        retn
    }
}

__declspec(naked) void speed3() //004A3EAD
{
    __asm
    {
        push    esi
        call    getSpeedModifier
        mov     [esp - 4], eax
        //fld     dword ptr [esp]
        //add     esp, 4

        mov     ecx, [esi + 1Ch]
        test    ecx, ecx
        jz      short loc_4A3ED2
        fld     dword ptr [ecx + 74h]
        _emit   0xDC//fcomp   ds:dbl_6546B0
        _emit   0x1D
        _emit   0xB0
        _emit   0x46
        _emit   0x65
        _emit   0x00
        fnstsw  ax
        test    ah, 41h
        jnz     short loc_4A3ED2
        mov     eax, [esi + 14h]
        fld     dword ptr [esp - 4]
        fld     dword ptr [eax + 0D0h]
        fmul    st(1), st
        fstp    st
        fmul    dword ptr [ecx + 74h]
        jmp     speed3_end
        //.text:004A28E4 ; ---------------------------------------------------------------------------
        //.text:004A28E4
loc_4A3ED2:
        mov     ecx, [esi + 14h]
        fld     dword ptr [esp - 4]
        fld     dword ptr [ecx + 0D0h]
        fmul    st(1), st
        fstp    st
speed3_end:
        mov     eax, 004A3EDBh
        jmp     eax
    }
}

__declspec(naked) void speed4() //004A6250
{
    __asm
    {
        push    ecx
        push    ecx
        call    getSpeedModifier
        mov     ecx, [esp]
        mov     [esp], eax
        //fld     dword ptr [esp]
        //add     esp, 4

        mov     edx, [ecx + 1Ch]
        test    edx, edx
        jz      short loc_4A6276
        fld     dword ptr [edx + 74h]
        _emit   0xDC//fcomp   ds:dbl_6546B0
        _emit   0x1D
        _emit   0xB0
        _emit   0x46
        _emit   0x65
        _emit   0x00
        fnstsw  ax
        test    ah, 41h
        jnz     short loc_4A6276
        mov     eax, [ecx + 14h]
        fld     dword ptr [esp]
        fld     dword ptr [eax + 0D0h]
        fmul    st(1), st
        fstp    st
        add     esp, 4
        fmul    dword ptr [edx + 74h]
        retn    4
        //---------------------------------------------------------------------------
loc_4A6276:
        mov     ecx, [ecx + 14h]
        fld     dword ptr [esp]
        fld     dword ptr [ecx + 0D0h]
        fmul    st(1), st
        fstp    st
        add     esp, 4
        retn    4
    }
}

__declspec(naked) void stealth1() //0054BB42
{
    __asm
    {
        push    esi
        call    getStealthOff
        test    al, al
        jnz     stealth_off
        mov     eax, [esi + 14h]
        test    byte ptr [eax + 0ACh], 4
        mov     ecx, 0054BB4Ch
        jmp     ecx
stealth_off:
        pop     edi
        pop     esi
        xor     al, al
        pop     ebx
        retn    4
    }
}

#pragma warning(push)
#pragma warning(disable:4100)

void __stdcall readUnitExtra(RGE_Static_Object* unit, int stream)
{
    char flag;
    UNIT_EXTRA* ud;

    switch (current_save_game_version)
    {
    case 0:
        rge_read(stream, &flag, sizeof(flag));
        if (flag)
        {
            ud = createUnitExtra(unit);
            UNIT_EXTRA_OLD ud_old;
            rge_read(stream, &ud_old, sizeof(UNIT_EXTRA_OLD));

            ud->speedReductionEnabled = ud_old.speedReductionEnabled;
            ud->speedReductionModifier = ud_old.speedReductionModifier;
            ud->speedReductionTime = ud_old.speedReductionTime;

            ud->stealthOffEnabled = ud_old.stealthOffEnabled;
            ud->stealthOffTime = ud_old.stealthOffTime;

            ud->reloadTimeEnabled = ud_old.reloadTimeEnabled;
            ud->reloadTimeModifier = ud_old.reloadTimeModifier;
            ud->reloadTimeTime = ud_old.reloadTimeTime;

            ud->hpDrainEnabled = ud_old.hpDrainEnabled;
            ud->hpDrainPerSecond = ud_old.hpDrainPerSecond;
            ud->hpDrainTime = ud_old.hpDrainTime;
            ud->hpDrainLeftover = ud_old.hpDrainLeftover;

            ud->hpDrainPercentEnabled = ud_old.hpDrainPercentEnabled;
            ud->hpDrainPercentPerSecond = ud_old.hpDrainPercentPerSecond;
            ud->hpDrainPercentTime = ud_old.hpDrainPercentTime;
            ud->hpDrainPercentLeftover = ud_old.hpDrainPercentLeftover;

            ud->countersUsed = ud_old.countersUsed;
            ud->miscCounter1 = ud_old.miscCounter1;
            ud->miscCounter2 = ud_old.miscCounter2;
            ud->miscCounter3 = ud_old.miscCounter3;
            ud->miscCounter4 = ud_old.miscCounter4;
            ud->miscCounter5 = ud_old.miscCounter5;

            ud->hasBeenPurged = false;
            ud->keepUnitExtra = false;
            ud->animalTimer = 0;
        }
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        rge_read(stream, &flag, sizeof(flag));
        if (flag)
        {
            ud = createUnitExtra(unit);

            rge_read(stream, &ud->speedReductionEnabled, sizeof(ud->speedReductionEnabled));
            rge_read(stream, &ud->speedReductionModifier, sizeof(ud->speedReductionModifier));
            rge_read(stream, &ud->speedReductionTime, sizeof(ud->speedReductionTime));
            
            rge_read(stream, &ud->stealthOffEnabled, sizeof(ud->stealthOffEnabled));
            rge_read(stream, &ud->stealthOffTime, sizeof(ud->stealthOffTime));

            rge_read(stream, &ud->reloadTimeEnabled, sizeof(ud->reloadTimeEnabled));
            rge_read(stream, &ud->reloadTimeModifier, sizeof(ud->reloadTimeModifier));
            rge_read(stream, &ud->reloadTimeTime, sizeof(ud->reloadTimeTime));

            rge_read(stream, &ud->hpDrainEnabled, sizeof(ud->hpDrainEnabled));
            rge_read(stream, &ud->hpDrainPerSecond, sizeof(ud->hpDrainPerSecond));
            rge_read(stream, &ud->hpDrainTime, sizeof(ud->hpDrainTime));
            rge_read(stream, &ud->hpDrainLeftover, sizeof(ud->hpDrainLeftover));

            rge_read(stream, &ud->hpDrainPercentEnabled, sizeof(ud->hpDrainPercentEnabled));
            rge_read(stream, &ud->hpDrainPercentPerSecond, sizeof(ud->hpDrainPercentPerSecond));
            rge_read(stream, &ud->hpDrainPercentTime, sizeof(ud->hpDrainPercentTime));
            rge_read(stream, &ud->hpDrainPercentLeftover, sizeof(ud->hpDrainPercentLeftover));

            rge_read(stream, &ud->countersUsed, sizeof(ud->countersUsed));
            rge_read(stream, &ud->miscCounter1, sizeof(ud->miscCounter1));
            rge_read(stream, &ud->miscCounter2, sizeof(ud->miscCounter2));
            rge_read(stream, &ud->miscCounter3, sizeof(ud->miscCounter3));
            rge_read(stream, &ud->miscCounter4, sizeof(ud->miscCounter4));
            rge_read(stream, &ud->miscCounter5, sizeof(ud->miscCounter5));

            if (current_save_game_version >= 6)
            {
                rge_read(stream, &ud->hasBeenPurged, sizeof(ud->hasBeenPurged));
                rge_read(stream, &ud->keepUnitExtra, sizeof(ud->keepUnitExtra));
                rge_read(stream, &ud->animalTimer, sizeof(ud->animalTimer));
            }
            else
            {
                ud->hasBeenPurged = false;
                ud->keepUnitExtra = false;
                ud->animalTimer = 0;
            }
        }
        break;
#if CURRENT_VERSION != 6
#error Must update for new CURRENT_VERSION
#endif
    default:
        break;
    }
}

void __stdcall writeUnitExtra(RGE_Static_Object* unit, int stream)
{
    char flag;
    UNIT_EXTRA* ud = getUnitExtra(unit);
    if (ud)
    {
        flag = 1;
        rge_write(stream, &flag, sizeof(flag));
        
        rge_write(stream, &ud->speedReductionEnabled, sizeof(ud->speedReductionEnabled));
        rge_write(stream, &ud->speedReductionModifier, sizeof(ud->speedReductionModifier));
        rge_write(stream, &ud->speedReductionTime, sizeof(ud->speedReductionTime));

        rge_write(stream, &ud->stealthOffEnabled, sizeof(ud->stealthOffEnabled));
        rge_write(stream, &ud->stealthOffTime, sizeof(ud->stealthOffTime));

        rge_write(stream, &ud->reloadTimeEnabled, sizeof(ud->reloadTimeEnabled));
        rge_write(stream, &ud->reloadTimeModifier, sizeof(ud->reloadTimeModifier));
        rge_write(stream, &ud->reloadTimeTime, sizeof(ud->reloadTimeTime));

        rge_write(stream, &ud->hpDrainEnabled, sizeof(ud->hpDrainEnabled));
        rge_write(stream, &ud->hpDrainPerSecond, sizeof(ud->hpDrainPerSecond));
        rge_write(stream, &ud->hpDrainTime, sizeof(ud->hpDrainTime));
        rge_write(stream, &ud->hpDrainLeftover, sizeof(ud->hpDrainLeftover));

        rge_write(stream, &ud->hpDrainPercentEnabled, sizeof(ud->hpDrainPercentEnabled));
        rge_write(stream, &ud->hpDrainPercentPerSecond, sizeof(ud->hpDrainPercentPerSecond));
        rge_write(stream, &ud->hpDrainPercentTime, sizeof(ud->hpDrainPercentTime));
        rge_write(stream, &ud->hpDrainPercentLeftover, sizeof(ud->hpDrainPercentLeftover));

        rge_write(stream, &ud->countersUsed, sizeof(ud->countersUsed));
        rge_write(stream, &ud->miscCounter1, sizeof(ud->miscCounter1));
        rge_write(stream, &ud->miscCounter2, sizeof(ud->miscCounter2));
        rge_write(stream, &ud->miscCounter3, sizeof(ud->miscCounter3));
        rge_write(stream, &ud->miscCounter4, sizeof(ud->miscCounter4));
        rge_write(stream, &ud->miscCounter5, sizeof(ud->miscCounter5));

        rge_write(stream, &ud->hasBeenPurged, sizeof(ud->hasBeenPurged));
        rge_write(stream, &ud->keepUnitExtra, sizeof(ud->keepUnitExtra));
        rge_write(stream, &ud->animalTimer, sizeof(ud->animalTimer));
    }
    else
    {
        flag = 0;
        rge_write(stream, &flag, sizeof(flag));
    }
}

__declspec(naked) void writeSaveHook() //004AF323
{
    __asm
    {
        mov     ecx, [eax + edi * 4]
        push    ebx
        push    ecx
        push    ebx
        mov     edx, [ecx]
        call    dword ptr [edx + 0A0h]
        call    writeUnitExtra
        mov     eax, 004AF32Fh
        jmp     eax
    }
}

__declspec(naked) void readSaveHook() //004AEEEE
{
    __asm
    {
        mov     ecx, esi
        call    dword ptr [edx]
        push    edi
        push    eax
        call    readUnitExtra
        mov     ecx, 004AEED4h
        jmp     ecx
    }
}

bool __stdcall test_save_game_version(char* version, int stream, bool from_chapter)
{
    bool setup_dat_file_ret = false;
    if (!strncmp(version, "VER 9.4", 8))
    {
        if (!from_chapter) //workaround for a bug in previous version
            current_save_game_version = -1;
        return true;
    }
    else if (!strncmp(version, "VER 9.8", 8))
    {
        current_save_game_version = 0;
        if (!from_chapter)
            setup_dat_file_ret = setup_dat_file();
        return !setup_dat_file_ret;
    }
    else if (!strncmp(version, "VER 9.9", 8))
    {
        int sub_version;
        rge_read(stream, &sub_version, sizeof(sub_version));
        if (sub_version >= 1)
        {
            current_save_game_version = sub_version;
            if (!from_chapter)
                setup_dat_file_ret = setup_dat_file();
            return !setup_dat_file_ret;
        }
        else
            return false;
    }
    else
        return false;
}

__declspec(naked) void verLoadHook1() //0061D9A5
{
    __asm
    {
        lea     eax, [esp + 18h] //version
        push    0
        push    edi
        push    eax
        call    test_save_game_version
        test    al, al
        jz      bad_version1
        mov     eax, 0061D9F1h
        jmp     eax
bad_version1:
        mov     eax, 0061D9DBh
        jmp     eax
    }
}

__declspec(naked) void verLoadHook2() //0061DBBA
{
    __asm
    {
        lea     eax, [esp + 10h] //version
        push    1
        push    edi
        push    eax
        call    test_save_game_version
        test    al, al
        jz      bad_version2
        mov     eax, 0061DBFBh
        jmp     eax
bad_version2:
        mov     eax, 0061DBF0h
        jmp     eax
    }
}

void __stdcall write_save_game_version(int stream)
{
    int version = CURRENT_VERSION;
    rge_write(stream, &version, sizeof(version));
}

__declspec(naked) void verSaveHook1() //0062059F
{
    __asm
    {
        push    esi
        call    write_save_game_version

        mov     edx, [ebx]
        xor     eax, eax
        cmp     ebp, 2
        mov     ecx, 006205A6h
        jmp     ecx
    }
}

__declspec(naked) void verSaveHook2() //006206CE
{
    __asm
    {
        push    esi
        call    write_save_game_version

        mov     edx, [edi]
        push    0
        push    esi
        mov     ecx, edi
        mov     eax, 006206D5h
        jmp     eax
    }
}

__declspec(naked) void __fastcall removeUnitExtra(RGE_Static_Object* unit)
{
    __asm
    {
        mov     eax, [ecx + 78h]
        and     eax, 0FFFFFF00h
        mov     al, [ecx + 87h]
        test    eax, eax
        jz      no_free
        push    ecx
        push    eax
        call    free
        add     esp, 4
        pop     ecx
        and     dword ptr [ecx + 78h], 0FFh
        and     byte ptr [ecx + 87h], 0
no_free:
        ret
    }
}

__declspec(naked) void unit_remove_unit_extra_destructor() //0054B680
{
    __asm
    {
        push    ecx
        push    ebx
        push    esi
        mov     esi, ecx
        push    edi
        call    removeUnitExtra
        mov     ecx, esi
        mov     eax, 0054B686h
        jmp     eax
    }
}

__declspec(naked) void unit_remove_unit_extra_recycle_out() //0054C0A0
{
    __asm
    {
        push    ecx
        push    ebx
        push    esi
        mov     esi, ecx
        xor     ebx, ebx
        call    removeUnitExtra
        mov     ecx, esi
        mov     eax, 0054C0A7h
        jmp     eax
    }
}

__declspec(naked) UNIT_EXTRA* __fastcall getUnitExtra(RGE_Static_Object* unit)
{
    __asm
    {
        mov     eax, [ecx + 78h]
        mov     al, [ecx + 87h]
        ret
    }
}

__declspec(naked) void __fastcall addUnitExtra(RGE_Static_Object* unit, UNIT_EXTRA* ud)
{
    __asm
    {
        add     ecx, 78h
        mov     [ecx + 0Fh], dl
        and     edx, 0FFFFFF00h
        or      dword ptr [ecx], edx
        ret
    }
}
#pragma warning(pop)

UNIT_EXTRA* __fastcall createUnitExtra(RGE_Static_Object* unit)
{
    UNIT_EXTRA* ud = getUnitExtra(unit);
    if (!ud)
    {
        ud = (UNIT_EXTRA*)malloc(sizeof(UNIT_EXTRA));
        memset(ud, 0, sizeof(UNIT_EXTRA));
        addUnitExtra(unit, ud);
    }
    return ud;
}

const char unit_constructor_asm_fix[] = "\x89\x46\x78\x89\x86\x84\x00\x00\x00\x90\x90\x90\x90\x90\x90\x90\x90\x90";

#pragma optimize( "s", on )
void setCastHooks()
{
    setHook((void*)0x00444A7C, onGetDamage);
    setHook((void*)0x004449C3, getDamageGetUnit);

    setHook((void*)0x0044B7E2, speed1);
    setHook((void*)0x004A28C0, speed2);
    setHook((void*)0x004A3EAD, speed3);
    setHook((void*)0x004A6250, speed4);

    setHook((void*)0x0054BB42, stealth1);

    setHook((void*)0x004AF323, writeSaveHook);
    setHook((void*)0x004AEEEE, readSaveHook);
    setHook((void*)0x0061D9A5, verLoadHook1);
    setHook((void*)0x0061DBBA, verLoadHook2);
    setHook((void*)0x0062059F, verSaveHook1);
    setHook((void*)0x006206CE, verSaveHook2);

    writeData(0x0069E564, "VER 9.9", 8);

    setHook((void*)0x0054B680, unit_remove_unit_extra_destructor);
    setHook((void*)0x0054C0A0, unit_remove_unit_extra_recycle_out);
    writeData(0x0054B315, unit_constructor_asm_fix, 18);
    writeData(0x0054B638, unit_constructor_asm_fix, 18);

    setHook((void*)0x0054EF00, processUnitHook);
}
#pragma optimize( "", on )
