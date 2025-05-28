#include "stdafx.h"
#include "aiattack.h"

bool __stdcall use_special_attack_type2(TribeInformationAIModule* info_ai, RGE_Static_Object* obj, float* val)
{
    int type2 = TribeTacticalAIModule__strategicNumber(&info_ai->md->tacticalAI, 107); //sn-special-attack-type2
    if (type2 >= 1600 && type2 < 1700)
    {
        if (obj->master_obj->object_group == (type2 - 1600))
        {
            *val = 1000.0f;
            return true;
        }
    }
    else if (type2 > 0)
    {
        if (obj->master_obj->id == type2)
        {
            *val = 1000.0f;
            return true;
        }
    }
    else
    {
        if (obj->master_obj->unit_line != -1 && obj->master_obj->unit_line == -type2)
        {
            *val = 1000.0f;
            return true;
        }
    }

    return false;
}

__declspec(naked) void on_special_attack_type2() //00592AA4
{
    __asm
    {
        lea     eax, [esi + 20h]
        push    eax
        push    edi
        push    ebp
        call    use_special_attack_type2
        test    al, al
        jz      not_use_special_attack_type2
        mov     eax, 00592C05h
        jmp     eax

not_use_special_attack_type2:
        mov     eax, 00592AD2h
        jmp     eax
    }
}

void setAIAttackHooks()
{
    setHook((void*)0x00592AA4, on_special_attack_type2);
}
