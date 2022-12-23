#include "stdafx.h"
#include "tooltip.h"

void __stdcall parse_tooltip_text(char* str, int string_id, RGE_Static_Object* object)
{
    char buf[1024];
    if (object)
    {
        switch (string_id)
        {
        case 43024:
            if (object->master_obj->master_type >= 50)
            {
                strcpy_safe(buf, _countof(buf), str);
                snprintf(str, 1024, "%s\nAttack bonuses:", buf);
                for (int i = 0; i < ((RGE_Combat_Object*)object)->master_obj->weapon_num; i++)
                {
                    snprintf(buf, _countof(buf), "\nClass %hd, value %hd",
                        ((RGE_Combat_Object*)object)->master_obj->weapon->type, ((RGE_Combat_Object*)object)->master_obj->weapon->value);
                    strcat_s(str, 1024, buf);
                }
            }
            break;
        default:
            break;
        }
    }
}

__declspec(naked) void on_tooltip() //00500A93
{
    __asm
    {
        mov     eax, 004B76C0h
        call    eax
        mov     eax, [esi + 1280h]
        mov     ecx, [esi + 136Ch]
        lea     edx, [esi + 1374h]
        push    eax
        push    ecx
        push    edx
        call    parse_tooltip_text

        mov     edi, 00500A98h
        jmp     edi
    }
}

void setTooltipHooks()
{
    setHook((void*)0x00500A93, on_tooltip);
}
