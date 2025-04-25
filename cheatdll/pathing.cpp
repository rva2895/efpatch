#include "stdafx.h"
#include "pathing.h"

int __stdcall get_pathing_version(RGE_Player* player)
{
    unsigned char option = *((unsigned char*)player + 0x21);
    if (option >= 0x10)
        return 1;
    else
        return 0;
}

int path_stats[100];

int __stdcall RGE_Player__availablePathingAttempts_new2(RGE_Static_Object* pathing_unit, RGE_Player* player, int numWaitDelays)
{
    path_stats[pathing_unit->master_obj->object_group]++;

    int pathingDelayCapValueToUse = player->pathingDelayCapValue;
    int pathingAttemptCapValueToUse = player->pathingAttemptCapValue;

    if (get_pathing_version(player) > 0)
    {
        switch (pathing_unit->master_obj->object_group)
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
            //pathingAttemptCapValueToUse = 200 * player->pathingAttemptCapValue;
            return 1;
            break;
        default:
            pathingAttemptCapValueToUse = player->pathingAttemptCapValue;
            break;
        }
    }
    else
    {
        pathingAttemptCapValueToUse = player->pathingAttemptCapValue;
    }

    if (player->pathingDelayCapValue != -1 && numWaitDelays > pathingDelayCapValueToUse)
        return 1;

    if (player->pathingAttemptCapValue == -1)
        return 1;
    else
        return player->currentUpdatePathingAttemptsValue < pathingAttemptCapValueToUse;
}

__declspec(naked) void pathing_bmotion() //0049D15C
{
    __asm
    {
        mov     eax, [esi + 0B8h]
        push    ecx
        push    eax
        call    RGE_Player__availablePathingAttempts_new2
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
        call    RGE_Player__availablePathingAttempts_new2
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
