#include "stdafx.h"
#include "editorui.h"

#define SED_SIDEBAR_STRETCH 102

__declspec(naked) void sed_button_size_new_condition() //005369D9
{
    __asm
    {
        push    112 + SED_SIDEBAR_STRETCH / 2
        push    112 + SED_SIDEBAR_STRETCH / 2
        push    1
        push    0
        mov     eax, 005369E1h
        jmp     eax
    }
}

__declspec(naked) void sed_button_size_new_effect() //00536A08
{
    __asm
    {
        push    112 + SED_SIDEBAR_STRETCH / 2
        push    112 + SED_SIDEBAR_STRETCH / 2
        push    1
        push    0
        mov     eax, 00536A10h
        jmp     eax
    }
}

__declspec(naked) void sed_button_size_new_trigger() //00536CEE
{
    __asm
    {
        push    75 + SED_SIDEBAR_STRETCH / 3
        push    75 + SED_SIDEBAR_STRETCH / 3
        push    1
        push    0
        mov     eax, 00536CF6h
        jmp     eax
    }
}

__declspec(naked) void sed_button_size_delete_trigger() //00536D1D
{
    __asm
    {
        push    75 + SED_SIDEBAR_STRETCH / 3
        push    75 + SED_SIDEBAR_STRETCH / 3
        push    1
        push    0
        mov     eax, 00536D25h
        jmp     eax
    }
}

__declspec(naked) void sed_button_size_info_trigger() //00536D4C
{
    __asm
    {
        push    75 + SED_SIDEBAR_STRETCH / 3
        push    75 + SED_SIDEBAR_STRETCH / 3
        push    1
        push    0
        mov     eax, 00536D54h
        jmp     eax
    }
}

__declspec(naked) void sed_button_size_units_units() //005368ED
{
    __asm
    {
        push    113 + SED_SIDEBAR_STRETCH / 2
        push    113 + SED_SIDEBAR_STRETCH / 2
        push    1
        push    0
        mov     eax, 005368F5h
        jmp     eax
    }
}

__declspec(naked) void sed_button_size_units_bldgs() //0053691C
{
    __asm
    {
        push    114 + SED_SIDEBAR_STRETCH / 2
        push    114 + SED_SIDEBAR_STRETCH / 2
        push    2
        push    1
        mov     eax, 00536924h
        jmp     eax
    }
}

__declspec(naked) void sed_button_size_units_heroes() //0053694B
{
    __asm
    {
        push    113 + SED_SIDEBAR_STRETCH / 2
        push    113 + SED_SIDEBAR_STRETCH / 2
        push    1
        push    0
        mov     eax, 00536953h
        jmp     eax
    }
}

__declspec(naked) void sed_button_size_units_other() //0053697F
{
    __asm
    {
        push    114 + SED_SIDEBAR_STRETCH / 2
        push    114 + SED_SIDEBAR_STRETCH / 2
        push    2
        push    1
        mov     eax, 00536987h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setEditorUIHooks()
{
    setHook((void*)0x005369D9, sed_button_size_new_condition);
    setHook((void*)0x00536A08, sed_button_size_new_effect);
    setHook((void*)0x00536CEE, sed_button_size_new_trigger);
    setHook((void*)0x00536D1D, sed_button_size_delete_trigger);
    setHook((void*)0x00536D4C, sed_button_size_info_trigger);
    setHook((void*)0x005368ED, sed_button_size_units_units);
    setHook((void*)0x0053691C, sed_button_size_units_bldgs);
    setHook((void*)0x0053694B, sed_button_size_units_heroes);
    setHook((void*)0x0053697F, sed_button_size_units_other);

    //Trigger effect delete btn
    writeDword(0x005369AA, 225 + SED_SIDEBAR_STRETCH);
    writeDword(0x005369AF, 225 + SED_SIDEBAR_STRETCH);

    //Trigger list box
    writeDword(0x00536B60, 200 + SED_SIDEBAR_STRETCH);
    writeDword(0x00536B65, 200 + SED_SIDEBAR_STRETCH);

    //Trigger list box 2
    writeDword(0x00536C2D, 200 + SED_SIDEBAR_STRETCH);
    writeDword(0x00536C32, 200 + SED_SIDEBAR_STRETCH);

    //Trigger sidebar bg
    writeDword(0x00536B16, 225 + SED_SIDEBAR_STRETCH);
    writeDword(0x00536B1B, 225 + SED_SIDEBAR_STRETCH);

    //Conditions and effects bg
    writeDword(0x00536D77, 225 + SED_SIDEBAR_STRETCH);
    writeDword(0x00536D7C, 225 + SED_SIDEBAR_STRETCH);

    //Conditions and effects list box
    writeDword(0x00536DAF, 200 + SED_SIDEBAR_STRETCH);
    writeDword(0x00536DB4, 200 + SED_SIDEBAR_STRETCH);

    //Conditions and effects list box 2
    writeDword(0x00536E7D, 200 + SED_SIDEBAR_STRETCH);
    writeDword(0x00536E82, 200 + SED_SIDEBAR_STRETCH);

    //Units list box
    writeDword(0x00536A95, 230 + SED_SIDEBAR_STRETCH);
    writeDword(0x00536A9A, 230 + SED_SIDEBAR_STRETCH);

    //???
    writeDword(0x00536DAF, 200 + SED_SIDEBAR_STRETCH);
    writeDword(0x00536DB4, 200 + SED_SIDEBAR_STRETCH);
}
#pragma optimize( "", on )
