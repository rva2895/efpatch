#include "stdafx.h"
#include "attribute_held.h"

__int16 get_fixed_resource_id(__int16 attribute)
{
    switch (attribute)
    {
    case 0: //food
    case 15:
    case 16:
    case 17:
        return 0;
    default:
        return attribute;
    }

}

bool __stdcall test_food_attribute(__int16 attribute_held, __int16 task_attribute)
{
    return get_fixed_resource_id(attribute_held) == get_fixed_resource_id(task_attribute);
}

__declspec(naked) void on_action_gather_update() //0040560D
{
    __asm
    {
        fild    word ptr [edx + 8Ch]
        fst     dword ptr [esp + 14h]
        fld     dword ptr [eax + 54h]
        fcomip  st, st(1)
        jb      short loc_60348C
        fstp    st
        fstp    st
        fstp    st
        jmp     loc_6035BF

loc_60348C:
        fld     st(1)
        fmul    st, st(3)
        fst     dword ptr [esp + 10h]
        fadd    dword ptr [eax + 54h]
        fcomi   st, st(1)
        jbe     short loc_6034AB
        fsub    st, st(1)
        fld     dword ptr [esp + 10h]
        fsub    st, st(1)
        fstp    dword ptr [esp + 10h]
        fdiv    st, st(2)
        fsub    st(3), st

loc_6034AB:
        fstp    st

        mov     eax, 0040564Ah
        jmp     eax

loc_6035BF:
        mov     eax, 00405754h
        jmp     eax
    }
}

void __stdcall verify_food_resource(RGE_Static_Object* obj, __int16 attribute, bool keep_attribute)
{
    if (*(unsigned __int8*)((size_t)obj->owner + 0x21) >= 0x0E)
    {
        if (!keep_attribute)
        {
            if (!test_food_attribute(obj->attribute_type_held, attribute))
                obj->vfptr->set_attribute(obj, attribute, 0.0f);
            else
                obj->attribute_type_held = attribute;
        }
    }
    else
    {
        if (obj->attribute_type_held != attribute)
            obj->vfptr->set_attribute(obj, attribute, 0.0f);
    }
}

__declspec(naked) void on_action_hunt_first_in_stack() //0056A50E
{
    __asm
    {
        push    1
        push    eax
        push    ecx
        call    verify_food_resource

        mov     eax, 0056A51Fh
        jmp     eax
    }
}

__declspec(naked) void on_action_gather_first_in_stack() //0040438E
{
    __asm
    {
        push    1
        push    eax
        push    ecx
        call    verify_food_resource

        mov     eax, 0040439Fh
        jmp     eax
    }
}

__declspec(naked) void on_action_hunt_set_state() //0056AD94
{
    __asm
    {
        push    0
        push    eax
        push    ecx
        call    verify_food_resource

        mov     eax, 0056ADA5h
        jmp     eax
    }
}

__declspec(naked) void on_action_gather_set_state() //00404A93
{
    __asm
    {
        push    0
        push    eax
        push    ecx
        call    verify_food_resource

        mov     eax, 00404AA4h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setAttributeHeldHooks()
{
    setHook((void*)0x0056A50E, on_action_hunt_first_in_stack);
    setHook((void*)0x0040438E, on_action_gather_first_in_stack);

    setHook((void*)0x0056AD94, on_action_hunt_set_state);
    setHook((void*)0x00404A93, on_action_gather_set_state);

    setHook((void*)0x0040560D, on_action_gather_update);
}
#pragma optimize( "", on )
