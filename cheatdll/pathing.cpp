#include "stdafx.h"
#include "pathing.h"

bool __stdcall get_new_pathing_delay_behavior(void* player)
{
    unsigned char o = *((unsigned char*)player + 0x21);
    switch (o)
    {
    case 0xB:
    case 0xC:
        return false;
    default:
        return true;
    }
}

int __stdcall WorldPlayerBase__availablePathingAttempts_new(UNIT* pathing_unit, void* player, int numWaitDelays)
{
    bool try_low_pathing_delay = (*((unsigned char*)player + 0xA0) && get_new_pathing_delay_behavior(player)) ? true : false;

    int max_pathing_delay;

    if (try_low_pathing_delay)
    {
        switch (pathing_unit->prop_object->unit_class)
        {
        case 11: //cruiser
        case 13: //destroyer
        case 15: //frigate 1
        case 16: //aa destroyer 1
        case 17: //transport ship
        case 20: //projectile
        case 32: //artillery
        case 33: //aa mobile
        case 34: //undeployed cannon
        case 35: //pummel
        case 36: //cannon
        case 39: //frigate 2
        case 40: //aa destroyer 2
        case 43: //bomber
        case 44: //bounty hunter
        case 46: //commander
        case 47: //scout
        case 48: //fighter
        case 49: //grenade trooper
        case 50: //jedi
        case 51: //jedi w/holocron
        case 52: //trooper
        case 53: //war machine
        case 54: //medic
        case 55: //aa trooper
        case 56: //mounted trooper
        case 57: //fambaa shield generator
        case 59: //air transport
        case 61: //power droid
        case 62: //air cruiser
        case 63: //geo warrior
        case 64: //jedi starfighter
            max_pathing_delay = 0;
            break;
        default:
            max_pathing_delay = *((int*)player + 3);
            break;
        }
    }
    else
        max_pathing_delay = *((int*)player + 3);

    //int max_pathing_delay = *((int*)player + 3);
    if (max_pathing_delay != -1 && numWaitDelays > max_pathing_delay)
        return 1;
    int max_pathing_attempts = *((int*)player + 2);
    if (max_pathing_attempts == -1)
        return 1;
    else
        return *((int*)player + 4) < max_pathing_attempts;
}

__declspec(naked) void pathing_bmotion() //0049D15C
{
    __asm
    {
        push    ecx
        mov     ecx, [esi + 0B8h]
        push    ecx
        call    WorldPlayerBase__availablePathingAttempts_new
        mov     ecx, 0049D161h
        jmp     ecx
    }
}

__declspec(naked) void pathing_moving_object() //004A49B9
{
    __asm
    {
        push    ecx
        push    esi
        call    WorldPlayerBase__availablePathingAttempts_new
        mov     ecx, 004A49BEh
        jmp     ecx
    }
}

#pragma optimize( "s", on )
void setPathingHooks()
{
    setHook((void*)0x0049D15C, pathing_bmotion);
    setHook((void*)0x004A49B9, pathing_moving_object);
}
#pragma optimize( "", on )
