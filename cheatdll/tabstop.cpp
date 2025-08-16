#include "stdafx.h"
#include "tabstop.h"

void __stdcall tabstop_sed_menu_setup(TRIBE_Dialog_Sed_Menu* menu)
{
    TPanel__set_tab_order2((TPanel*)menu, (TPanel**)menu->buttons, _countof(menu->buttons));
    TPanel__set_curr_child((TPanel*)menu, (TPanel*)menu->buttons[6]);
}

__declspec(naked) void tabstop_sed_menu() //0045FB9D
{
    __asm
    {
        mov     [eax + 2B4h], ebx
        push    esi
        call    tabstop_sed_menu_setup
        mov     eax, 0045FBA3h
        jmp     eax
    }
}

void __stdcall tabstop_sed_menu_button_setup(TRIBE_Screen_Sed* sed)
{
    sed->menu_button->hotkey = VK_F10;
    sed->menu_button->hotkey_alt = 0;
    sed->menu_button->hotkey_ctrl = 0;
    sed->menu_button->hotkey_shift = 0;
}

__declspec(naked) void tabstop_sed_menu_button() //00536762
{
    __asm
    {
        call    dword ptr [eax + 0E0h]
        push    esi
        call    tabstop_sed_menu_button_setup
        mov     eax, 00536768h
        jmp     eax
    }
}

__declspec(naked) void tabstop_main_menu_sp() //0050F2F8
{
    __asm
    {
        mov     eax, [esi + 864h]
        mov     [esp + 18h], edx
        mov     edx, [esi + 860h]
        mov     [esp + 20h], edx
        mov     ecx, 0050F302h
        jmp     ecx
    }
}

#pragma optimize( "s", on )
void setTabstopHooks()
{
    setHook((void*)0x0045FB9D, tabstop_sed_menu);
    setHook((void*)0x00536762, tabstop_sed_menu_button);

    setHook((void*)0x0050F2F8, tabstop_main_menu_sp);

    writeByte(0x0050F31B, 0x24);
    writeDword(0x0050F303, 3);
    writeDword(0x0050F31D, 4);
}
#pragma optimize( "", on )
