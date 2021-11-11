#include "stdafx.h"
#include "patroldelay.h"

int patrol_delay_enabled = 1;
bool patrol_control_flag = 0;

__declspec(naked) void on_patrol_main_view() //005FB72B
{
    __asm
    {
        mov     edx, patrol_delay_enabled
        mov     eax, [esp + 4Ch]
        and     edx, edx
        jnz     short loc_7D0C17
        neg     eax
        sbb     eax, eax
        and     eax, 4
        jmp     short loc_7D0C21

loc_7D0C17:
        neg     eax
        sbb     eax, eax
        and     eax, 0FFFFFFFCh
        add     eax, 4

loc_7D0C21:
        mov     patrol_control_flag, al
        mov     eax, 005FA9E0h
        call    eax //TRIBE_Main_View::send_patrol_command
        mov     patrol_control_flag, 0
        
        mov     eax, 005FB730h
        jmp     eax
    }
}

__declspec(naked) void on_patrol_diamond_map() //005FA546
{
    __asm
    {
        mov     edx, patrol_delay_enabled
        mov     eax, [esp + 34h]
        and     edx, edx
        jnz     short loc_7D0C57
        neg     eax
        sbb     eax, eax
        and     eax, 4
        jmp     short loc_7D0C61

loc_7D0C57:
        neg     eax
        sbb     eax, eax
        and     eax, 0FFFFFFFCh
        add     eax, 4

loc_7D0C61:
        mov     patrol_control_flag, al
        mov     eax, 005FA9E0h
        call    eax //TRIBE_Main_View::send_patrol_command
        mov     patrol_control_flag, 0
        
        mov     eax, 005FA54Bh
        jmp     eax
    }
}

__declspec(naked) void on_do_command_patrol() //0044C78A
{
    __asm
    {
        mov     ecx, eax
        movsx   edx, byte ptr [edi + 3]
        mov     dword ptr [ecx + 138h], 2710h
        //inc     eax
        //shl     eax, 4
        //lea     edx, [eax + eax * 2]
        //shl     eax, 7
        //sub     eax, edx
        mov     eax, 5
        sub     eax, edx
        lea     eax, [eax + eax * 4]
        lea     eax, [eax + eax * 4]
        lea     eax, [eax + eax * 4]
        shl     eax, 4
        mov     dword ptr [ecx + 134h], 0
        mov     [ecx + 140h], eax
        mov     eax, 004222C0h
        call    eax //UnitAIModule::zeroPatrolInformation(void)
        
        mov     eax, 0044C7CBh
        jmp     eax
    }
}

__declspec(naked) void on_command_patrol() //0044EBC0
{
    __asm
    {
        mov     [esi + 1], bl
        and     al, 0Ch
        mov     cl, patrol_control_flag
        mov     [esi + 3], cl

        mov     ecx, 0044EBC5h
        jmp     ecx
    }
}

#pragma optimize( "s", on )
void setPatrolDelayHooks()
{
    setHook((void*)0x005FB72B, on_patrol_main_view);
    setHook((void*)0x005FA546, on_patrol_diamond_map);
    setHook((void*)0x0044C78A, on_do_command_patrol);
    setHook((void*)0x0044EBC0, on_command_patrol);
}
#pragma optimize( "", on )
