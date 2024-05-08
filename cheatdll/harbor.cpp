#include "stdafx.h"
#include "harbor.h"

__declspec(naked) void on_command_make_bldg() //005D022C
{
    __asm
    {
        cmp     ecx, 199
        jz      loc_5D02D6
        cmp     ecx, 1001
        jz      loc_5D02D6
        cmp     ecx, 1576
        jz      loc_5D02D6
        cmp     ecx, 7085
        jz      loc_5D02D6

        mov     eax, 005D0250h
        jmp     eax

loc_5D02D6:
        mov     eax, 005D02D6h
        jmp     eax
    }
}

__declspec(naked) void on_action_build_2() //005661E3
{
    __asm
    {
        cmp     eax, 199
        jz      loc_566200
        cmp     eax, 1001
        jz      loc_566200
        cmp     eax, 1576
        jz      loc_566200
        cmp     eax, 7085
        jz      loc_566200

        mov     eax, 005661F8h
        jmp     eax

loc_566200:
        mov     eax, 00566200h
        jmp     eax
    }
}

bool __stdcall check_show_garrison_button(short object_group)
{
    switch (object_group)
    {
    case 14:
    case 44:
    case 46:
    case 47:
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 58:
    case 61:
        return true;
    default:
        return false;
    }
}

__declspec(naked) void on_show_garrison_button() //0050317A
{
    __asm
    {
        push    eax
        call    check_show_garrison_button
        test    al, al
        jz      no_garrison_button

        mov     eax, 0050319Bh
        jmp     eax

no_garrison_button:
        mov     eax, 00503191h
        jmp     eax
    }
}

__declspec(naked) void check_ship_garrison_type()
{
    __asm
    {
        test    al, 20h
        jnz     loc_5F7BD6
        
        mov     eax, 005F7BBFh
        jmp     eax

loc_5F7BD6:
        mov     eax, 005F7BD6h
        jmp     eax
    }
}

DWORD garrison_type_offsets[] =
{
    0x005F7BBB,
    0x005F7BB5,
    0x005F7BA9,
    0x005F7BAF,
    0x005F7BA3,
    0x005F7BBF,
    (DWORD)check_ship_garrison_type
};

__declspec(naked) void pointer_vars_drop_at_harbor() //005D463E
{
    __asm
    {
        cmp     ecx, 45
        jz      loc_5D4783
        cmp     ecx, 7085
        jz      loc_5D4783

        push    005D4647h
        ret

loc_5D4783:
        push    005D4783h
        ret
    }
}

__declspec(naked) void target_task_harbor() //005F7D2C
{
    __asm
    {
        mov     eax, 45
        mov     ecx, 7085

        push    005F7D31h
        ret
    }
}

#pragma optimize( "s", on )
void setHarborHooks()
{
    setHook((void*)0x005D022C, on_command_make_bldg);
    setHook((void*)0x005661E3, on_action_build_2);
    setHook((void*)0x0050317A, on_show_garrison_button);

    writeByte(0x005F83F4 + 14 - 1, 6); //garrison type utility trawler class
    writeDword(0x00699204 + 14 * 4, 0x0160); //enable garrison icon trawler class
    writeDword(0x005F7B9F, (DWORD)garrison_type_offsets);

    setHook((void*)0x005D463E, pointer_vars_drop_at_harbor);
    setHook((void*)0x005F7D2C, target_task_harbor);
}
#pragma optimize( "", on )
