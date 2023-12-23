#include "stdafx.h"
#include "conversion.h"
#include "casts.h"

extern int current_loaded_version;

bool check_purged(RGE_Static_Object* obj)
{
    UNIT_EXTRA* ud = getUnitExtra(obj);
    return ud && ud->hasBeenPurged;
}

bool __stdcall check_meditation_and_purge(TRIBE_Action_Convert* act)
{
    RGE_Static_Object* target_obj = act->target_obj;
    if (current_loaded_version >= 6) //1.5.2+
    {
        if (target_obj && ((act->obj->owner->vfptr->isAlly(act->obj->owner, target_obj->owner->id) == 1)
            || check_purged(target_obj)))
        {
            unsigned char state = act->state;
            if ((state == 7 || state == 6 || state == 4) && act->obj->owner->attributes[193] <= 0.0f)
                act->obj->vfptr->set_attribute(act->obj, 34, 0);
            act->vfptr->set_state(act, 1);
            act->obj->vfptr->notify(act->obj, act->obj->id, act->obj->id, 507, 604, 0, 0);
            return true;
        }
        else
            return false;
    }
    else if (target_obj && act->obj->owner->vfptr->isAlly(act->obj->owner, target_obj->owner->id) == 1)
    {
        unsigned char state = act->state;
        if ((state == 7 || state == 6 || state == 4) && act->target_obj->owner->attributes[193] <= 0.0f)
            act->obj->vfptr->set_attribute(act->obj, 34, 0);
        act->vfptr->set_state(act, 1);
        act->obj->vfptr->notify(act->obj, act->obj->id, act->obj->id, 507, 604, 0, 0);
        return true;
    }
    else
        return false;
}

void __stdcall onPurgeAction(RGE_Static_Object* obj)
{
    obj->vfptr->die_die_die(obj);
    if (current_loaded_version >= 6)
    {
        UNIT_EXTRA* ud = createUnitExtra(obj);
        ud->hasBeenPurged = true;
    }
}

__declspec(naked) void onConversionUpdate() //00568699
{
    __asm
    {
        push    esi
        call    check_meditation_and_purge
        test    al, al
        jz      conversion_check_continue

        mov     ecx, 00568722h
        jmp     ecx

conversion_check_continue:
        mov     ecx, 0056872Ch
        jmp     ecx
    }
}

__declspec(naked) void onPurged() //00568BE4
{
    __asm
    {
        push    ecx
        call    onPurgeAction
        mov     ecx, 00568BFDh
        jmp     ecx
    }
}

void __stdcall reset_jedi_force(RGE_Static_Object* obj)
{
    if (current_loaded_version >= 6
        && (obj->master_obj->object_group == 50 || obj->master_obj->object_group == 51)
        && obj->attribute_type_held == 34
        && (obj->attribute_amount_held > obj->owner->attributes[213]))
        obj->vfptr->set_attribute(obj, 34, obj->owner->attributes[213]);
}

__declspec(naked) void onUnitConverted() //00568BF7
{
    __asm
    {
        call    dword ptr [eax + 0BCh]
        mov     eax, [esi + 10h]
        push    eax
        call    reset_jedi_force
        mov     ecx, 00568BFDh
        jmp     ecx
    }
}

bool __stdcall is_class_melee_range_convertable(short c)
{
    switch (c)
    {
    case 9:     //aa turret
    case 10:    //turret
    case 18:    //building
        return true;
    case 57:    //fambaa shield
        return current_loaded_version >= 6;
    default:
        return false;
    }
}

__declspec(naked) void meleeRangeFirstInStack() //00568139
{
    __asm
    {
        push    edi
        call    is_class_melee_range_convertable
        test    al, al
        jz      melee_range_first_in_stack_not_melee

        mov     ecx, 00568148h
        jmp     ecx

melee_range_first_in_stack_not_melee:
        mov     ecx, 0056815Fh
        jmp     ecx
    }
}

__declspec(naked) void meleeRangeActionTypeIfOrdered() //00409104
{
    __asm
    {
        push    ecx
        push    edx
        push    eax
        call    is_class_melee_range_convertable
        pop     edx
        pop     ecx
        test    al, al
        jz      melee_range_action_type_if_ordered_not_melee

        mov     eax, 00409116h
        jmp     eax

melee_range_action_type_if_ordered_not_melee:
        mov     eax, 00409149h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setConversionHooks()
{
    setHook((void*)0x00568699, onConversionUpdate);
    setHook((void*)0x00568BE4, onPurged);
    setHook((void*)0x00568BF7, onUnitConverted);
    setHook((void*)0x00568139, meleeRangeFirstInStack);
    setHook((void*)0x00409104, meleeRangeActionTypeIfOrdered);
}
#pragma optimize( "", on )
