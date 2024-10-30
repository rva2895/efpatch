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
        cmp     ecx, HARBOR_ID_FOUNDATION
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
        cmp     eax, HARBOR_ID_FOUNDATION
        jz      loc_566200

        mov     eax, 005661F8h
        jmp     eax

loc_566200:
        mov     eax, 00566200h
        jmp     eax
    }
}

bool __stdcall check_show_garrison_button(RGE_Master_Static_Object* master)
{
    switch (master->object_group)
    {
    case 13:    //destroyer
    case 14:    //trawler
    case 15:    //frigate 1
    //case 16:    //aa destroyer 1
    case 39:    //frigate 2
    //case 40:    //aa destroyer 2
    case 44:    //bh
    case 46:    //commander
    case 47:    //scout
    case 49:    //grenade trooper
    case 50:    //jedi
    case 51:    //jedi/holo
    case 52:    //trooper
    case 53:    //mech
    case 54:    //medic
    case 55:    //aa trooper
    case 56:    //mounted trooper
    case 58:    //worker
    case 61:    //power droid
        return true;
    default:
        return false;
    }
}

__declspec(naked) void on_show_garrison_button() //00503176
{
    __asm
    {
        push    ecx
        push    eax
        call    check_show_garrison_button
        pop     ecx
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
        cmp     ecx, HARBOR_ID_COMPLETE
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
        mov     ecx, HARBOR_ID_COMPLETE

        push    005F7D31h
        ret
    }
}

__declspec(naked) void harbor_garrison_1() //005551D3
{
    __asm
    {
        cmp     ebp, 14Fh
        jz      loc_555256_1
        cmp     ebp, HARBOR_ID_COMPLETE
        jz      loc_555256_1

        mov     eax, 005551DBh
        jmp     eax

loc_555256_1:
        mov     eax, 00555256h
        jmp     eax
    }
}

__declspec(naked) void harbor_garrison_2() //0055521E
{
    __asm
    {
        cmp     ebp, 14Fh
        jz      loc_555256_2
        cmp     ebp, HARBOR_ID_COMPLETE
        jz      loc_555256_2

        mov     eax, 00555226h
        jmp     eax

loc_555256_2:
        mov     eax, 00555256h
        jmp     eax
    }
}

__declspec(naked) void harbor_garrison_3() //005D4BD0
{
    __asm
    {
        cmp     ax, 52h
        jz      loc_5D4C06
        cmp     ax, HARBOR_ID_COMPLETE
        jz      loc_5D4C06

        mov     eax, 005D4BD6h
        jmp     eax

loc_5D4C06:
        mov     eax, 005D4C06h
        jmp     eax
    }
}

__declspec(naked) void harbor_garrison_4() //005D4C8D
{
    __asm
    {
        cmp     eax, 6Dh
        jz      loc_5D4CC0
        cmp     eax, HARBOR_ID_COMPLETE
        jz      loc_5D4CC0

        mov     eax, 005D4C92h
        jmp     eax

loc_5D4CC0:
        mov     eax, 005D4CC0h
        jmp     eax
    }
}

__declspec(naked) void harbor_power() //0054BDE3
{
    __asm
    {
        cmp     cx, 32h
        jz      loc_54BE36
        cmp     cx, HARBOR_ID_COMPLETE
        jz      loc_54BE36

        mov     ecx, 0054BDE9h
        jmp     ecx

loc_54BE36:
        mov     ecx, 0054BE36h
        jmp     ecx
    }
}

int __fastcall get_worker_and_trawler_count(RGE_New_Object_List* list)
{
    return RGE_New_Object_List__Count(list, -1, 58, -1, -1, -1, -1, -1, -1, 0) +
        RGE_New_Object_List__Count(list, -1, 14, -1, -1, -1, -1, -1, -1, 0);
}

__declspec(naked) void harbor_back_to_work_1() //00505BAB
{
    __asm
    {
        call    get_worker_and_trawler_count
        mov     edx, 00505BC1h
        jmp     edx
    }
}

__declspec(naked) void harbor_back_to_work_2() //005D56E8
{
    __asm
    {
        cmp     word ptr [ecx + 1Eh], 58
        jz      do_back_to_work
        cmp     word ptr [ecx + 1Eh], 14
        jz      do_back_to_work

        mov     eax, 005D5717h
        jmp     eax

do_back_to_work:
        mov     eax, 005D56EFh
        jmp     eax
    }
}

__declspec(naked) void harbor_back_to_work_3() //0055F15E
{
    __asm
    {
        cmp     word ptr [ecx + 1Eh], 58
        jz      back_to_work_save_info_1
        cmp     word ptr [ecx + 1Eh], 14
        jz      back_to_work_save_info_1

        mov     eax, 0055F1A9h
        jmp     eax

back_to_work_save_info_1:
        mov     eax, 0055F165h
        jmp     eax
    }
}

__declspec(naked) void harbor_back_to_work_4() //0055F198
{
    __asm
    {
        cmp     ax, 52h
        jz      loc_55F19E
        cmp     ax, HARBOR_ID_COMPLETE
        jz      loc_55F19E

        mov     eax, 0055F1A9h
        jmp     eax

loc_55F19E:
        mov     eax, 0055F19Eh
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setHarborHooks()
{
    setHook((void*)0x005D022C, on_command_make_bldg);
    setHook((void*)0x005661E3, on_action_build_2);
    setHook((void*)0x00503176, on_show_garrison_button);

    //garrison type
    writeByte(0x005F83F4 + 13 - 1, 6); //destroyer
    writeByte(0x005F83F4 + 14 - 1, 6); //trawler
    writeByte(0x005F83F4 + 15 - 1, 6); //frigate 1
    //writeByte(0x005F83F4 + 16 - 1, 6); //aa destroyer 1
    writeByte(0x005F83F4 + 39 - 1, 6); //frigate 2
    //writeByte(0x005F83F4 + 40 - 1, 6); //aa destroyer 2

    //enable garrison icon
    writeByte(0x00699204 + 13 * 4 + 1, 0x01); //destroyer
    writeByte(0x00699204 + 14 * 4 + 1, 0x01); //trawler
    writeByte(0x00699204 + 15 * 4 + 1, 0x01); //frigate 1
    //writeByte(0x00699204 + 16 * 4 + 1, 0x01); //aa destroyer 1
    writeByte(0x00699204 + 39 * 4 + 1, 0x01); //frigate 2
    //writeByte(0x00699204 + 40 * 4 + 1, 0x01); //aa destroyer 2

    writeDword(0x005F7B9F, (DWORD)garrison_type_offsets);

    setHook((void*)0x005D463E, pointer_vars_drop_at_harbor);
    setHook((void*)0x005F7D2C, target_task_harbor);

    setHook((void*)0x005551D3, harbor_garrison_1);
    setHook((void*)0x0055521E, harbor_garrison_2);
    setHook((void*)0x005D4BD0, harbor_garrison_3);
    setHook((void*)0x005D4C8D, harbor_garrison_4);
    setHook((void*)0x0054BDE3, harbor_power);
    setHook((void*)0x00505BAB, harbor_back_to_work_1);
    setHook((void*)0x005D56E8, harbor_back_to_work_2);
    setHook((void*)0x0055F15E, harbor_back_to_work_3);
    setHook((void*)0x0055F198, harbor_back_to_work_4);
}
#pragma optimize( "", on )
