#include "stdafx.h"
#include "editcontrol.h"

__declspec(naked) void edit_panel_destructor() //004C8896
{
    __asm
    {
        mov     edx, comm
        mov     edx, [edx]
        mov     ecx, base_game
        mov     ecx, [ecx]
        mov     eax, [edx + 2070h]
        mov     [esi + 0F8h], edi
        test    eax, eax
        mov     edx, [ecx + 17B4h] //1820h
        jnz     short loc_7E0C82
        test    edx, edx
        jz      short loc_7E0C82
        mov     eax, [edx + 10BCh] //1070h
        test    eax, eax
        jz      short loc_7E0C82
        mov     edx, [esi + 20h]
        mov     eax, [edx + 4]
        push    eax; hWnd
        call    SetFocus

loc_7E0C82:
        mov     eax, 004C88A0h
        jmp     eax
    }
}

__declspec(naked) void edit_panel_sub_wnd_proc() //004C9551
{
    __asm
    {
        jz      loc_54E6AB
        mov     edx, panel_system
        mov     edx, [edx + 1Ch]
        test    edx, edx
        jz      short loc_7E0CC3
        cmp     dword ptr [edx], 00659B58h
        jnz     short loc_7E0CC3
        mov     ecx, esi
        mov     eax, 004C8B70h
        call    eax
        mov     edx, [esi + 40h]
        mov     ecx, [edx + 808h]
        mov     edx, [ecx]
        push    0
        push    eax
        call    dword ptr [edx + 10Ch]

loc_7E0CC3:
        mov     eax, 004C964Ch
        jmp     eax

loc_54E6AB:
        mov     eax, 004C9557h
        jmp     eax
    }
}

__declspec(naked) void edit_panel_draw() //004CA051
{
    __asm
    {
        mov     edx, comm
        mov     edx, [edx]
        mov     ecx, base_game
        mov     ecx, [ecx]
        mov     eax, [edx + 2070h]
        mov     edx, [ecx + 17B4h]
        test    eax, eax
        jnz     short loc_7E0C32
        test    edx, edx
        jz      short loc_7E0C32
        mov     eax, [edx + 10BCh]
        test    eax, eax
        jz      short loc_7E0C32
        add     esp, 8
        jmp     loc_54EE07

loc_7E0C32:
        call    ShowWindow
        
loc_54EE07:
        mov     eax, 004CA057h
        jmp     eax
    }
}

__declspec(naked) void edit_panel_send_quick_message_dialog() //00461D04
{
    __asm
    {
        mov     ecx, 006AAEF0h
        mov     ecx, [ecx]
        push    1
        push    ecx
        push    1
        push    0Bh
        push    19h
        push    195h
        push    5
        push    82h
        push    0
        lea     edi, [esi + 804h]
        push    41h
        push    006A1C60h
        push    edi
        push    esi
        mov     ecx, esi
        mov     eax, 004BA1F0h
        call    eax //create_edit
        test    eax, eax
        jz      loc_5B5E3B
        push    0
        push    1
        push    0
        push    0Bh
        push    19h
        push    190h
        push    6
        push    82h
        push    0
        lea     eax, [esi + 808h]
        push    eax
        push    esi
        mov     ecx, esi
        mov     eax, 004B9A80h
        call    eax //create_text
        test    eax, eax
        jz      loc_5B5E3B
        
        mov     eax, 00461D71h
        jmp     eax

loc_5B5E3B:
        mov     eax, 00461D7Bh
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setEditControlHooks()
{
    setHook((void*)0x004C8896, edit_panel_destructor);
    setHook((void*)0x004C9551, edit_panel_sub_wnd_proc);
    setHook((void*)0x004CA051, edit_panel_draw);
    setHook((void*)0x00461D04, edit_panel_send_quick_message_dialog);
}
#pragma optimize( "", on )
