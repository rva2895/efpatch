#include "stdafx.h"
#include "commandbuttons.h"

bool allow_incomplete_foundation = false;

__declspec(naked) void get_selected_objects_to_command_test_state() //004C3618
{
    __asm
    {
        cmp     allow_incomplete_foundation, 1
        jz      allow_incomplete
        cmp     byte ptr [esi + 58h], 2
        jnz     loc_4C3709
allow_incomplete:
        mov     eax, 004C3622h
        jmp     eax
loc_4C3709:
        mov     eax, 004C3709h
        jmp     eax
    }
}

__declspec(naked) void on_rally_point_mouse_up() //005FB9C0
{
    __asm
    {
        mov     allow_incomplete_foundation, 1
        call    dword ptr [eax + 0B8h]
        mov     allow_incomplete_foundation, 0
        mov     ecx, 005FB9C6h
        jmp     ecx
    }
}

__declspec(naked) void on_rally_point_minimap() //005FA19E
{
    __asm
    {
        mov     allow_incomplete_foundation, 1
        call    dword ptr [edx + 0B8h]
        mov     allow_incomplete_foundation, 0
        mov     ecx, 005FA1A4h
        jmp     ecx
    }
}

__declspec(naked) void on_rally_point_remove() //004FC664
{
    __asm
    {
        mov     allow_incomplete_foundation, 1
        call    dword ptr [edx + 0B8h]
        mov     allow_incomplete_foundation, 0
        mov     ecx, 004FC66Ah
        jmp     ecx
    }
}

__declspec(naked) void on_rally_point_mouse_up_2() //00619389
{
    __asm
    {
        mov     allow_incomplete_foundation, 1
        call    dword ptr [edx + 0B8h]
        mov     allow_incomplete_foundation, 0
        mov     ecx, 0061938Fh
        jmp     ecx
    }
}

__declspec(naked) void on_rally_point_mouse_up_3() //006198E9
{
    __asm
    {
        mov     allow_incomplete_foundation, 1
        call    dword ptr [edx + 0B8h]
        mov     allow_incomplete_foundation, 0
        mov     ecx, 006198EFh
        jmp     ecx
    }
}

__declspec(naked) void on_display_back_to_work() //00505B98
{
    __asm
    {
        cmp     ax, 6Dh
        jz      display_back_to_work_button
        cmp     ax, 52h
        jz      display_back_to_work_button
        cmp     ax, 4Fh
        jz      display_back_to_work_button
        mov     eax, 00505BCDh
        jmp     eax
display_back_to_work_button:
        mov     eax, 00505BA4h
        jmp     eax
    }
}

void setCommandButtonsHooks()
{
    writeDword(0x00505ADF, 0x90909090);
    writeWord(0x00505AE3, 0x9090);

    //writeDword(0x004C361C, 0x90909090);
    //writeWord(0x004C3620, 0x9090);

    setHook((void*)0x005FB9C0, on_rally_point_mouse_up);
    setHook((void*)0x005FA19E, on_rally_point_minimap);
    setHook((void*)0x004FC664, on_rally_point_remove);
    setHook((void*)0x00619389, on_rally_point_mouse_up_2);
    setHook((void*)0x006198E9, on_rally_point_mouse_up_3);
    setHook((void*)0x004C3618, get_selected_objects_to_command_test_state);

    setHook((void*)0x00505B98, on_display_back_to_work);
}
