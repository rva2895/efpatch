#include "stdafx.h"
#include "rec.h"
#include <MMSystem.h>

#define REC_SPEED_COUNT 12

const float game_speeds[] =
{
    0.5, 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 10.0
};

void __stdcall setGameSpeed_new(TRIBE_Command* cmd, float spd, int isInc)
{
    int i;
    for (i = 0; (i < _countof(game_speeds)) && (spd > game_speeds[i]); i++)
        ;
    if (isInc) i++; else i--;
    if ((i >= 0) && (i < _countof(game_speeds)))
        TRIBE_Command__command_game_speed(cmd, game_speeds[i]);
}

__declspec(naked) void onCheckGameSpeed() //0050189E
{
    __asm
    {
        push    eax
        push    edx
        mov     ecx, [esi + 68h]
        push    ecx
        call    setGameSpeed_new
        pop     edi
        pop     esi
        pop     ecx
        retn    4
    }
}

__declspec(naked) void onSlowDown() //004FAF6B
{
    __asm
    {
        mov     ecx, [eax + 540h]
        test    ecx, ecx
        jle     rec_no_slow_down
        dec     ecx

rec_no_slow_down:
        mov     [eax + 540h], ecx
        mov     eax, 004FAF75h
        jmp     eax
    }
}

__declspec(naked) void onSpeedUp() //004FAF38
{
    __asm
    {
        mov     ecx, [eax + 540h]
        cmp     ecx, REC_SPEED_COUNT
        jge     rec_no_speed_up
        inc     ecx

rec_no_speed_up:
        mov     [eax + 540h], ecx
        mov     eax, 004FAF42h
        jmp     eax
    }
}

__declspec(naked) void onSpeedNormal() //004FAF9F
{
    __asm
    {
        mov     ecx, 2
        mov     [eax + 540h], ecx
        mov     eax, 004FAFA9h
        jmp     eax
    }
}

float rec_speed = 0.0f;

void __stdcall onPrintTime2(char* s)
{
    char s2[0x100];
    strlcpy(s2, s, _countof(s2));
    if (isRec())
        sprintf(s, "%s (%2.2f)", s2, rec_speed);
        //sprintf(s, "%s (%3.3f / %3.3f)", s2, rec_speed, avg_speed);
    else
        sprintf(s, "%s", s2);
}

__declspec(naked) void onPrintTime() //005E01E6
{
    __asm
    {
        lea     eax, [esp + 64h]
        push    eax
        call    onPrintTime2

        mov     edi, 20h
        mov     eax, 005E02B2h
        jmp     eax
    }
}

void __stdcall do_rec_speed_adjust(int speed)
{
    switch (speed)
    {
    case 0:
        rec_speed = 0.50f;
        break;
    case 1:
        rec_speed = 0.75f;
        break;
    case 2:
        rec_speed = 1.00f;
        break;
    case 3:
        rec_speed = 1.25f;
        break;
    case 4:
        rec_speed = 1.50f;
        break;
    case 5:
        rec_speed = 2.00f;
        break;
    case 6:
        rec_speed = 2.50f;
        break;
    case 7:
        rec_speed = 3.00f;
        break;
    case 8:
        rec_speed = 4.00f;
        break;
    case 9:
        rec_speed = 6.00f;
        break;
    case 10:
        rec_speed = 10.00f;
        break;
    case 11:
        rec_speed = 16.00f;
        break;
    case 12:
        rec_speed = INFINITY;
        break;
    default:
        rec_speed = 1.00f;
        break;
    }
}

__declspec(naked) void onCheckSpeed2() //0061EB81
{
    __asm
    {
        push    ecx
        push    eax
        mov     eax, [ebp - 10h]
        mov     [edi + 204h], eax
        call    do_rec_speed_adjust
        pop     ecx

        mov     eax, 0061EBA1h
        jmp     eax
    }
}

int __stdcall do_new_rec_speed_control(RGE_Game_World* world)
{
    unsigned int t = timeGetTime();
    if (world->mReplayTimeStart)
    {
        unsigned int delta = t - world->mReplayTimeStart;
        if ((float)world->mReplayRecordedInterval > (float)delta * world->game_speed * rec_speed)
            return 0;
    }
    world->mReplayTimeStart = t;
    return 1;
}

__declspec(naked) void new_rec_speed_control() //0061EABA
{
    __asm
    {
        push    edi
        call    do_new_rec_speed_control
        test    eax, eax
        jz      rec_speed_control_end
        mov     eax, 0061EAFAh
        jmp     eax

rec_speed_control_end:
        mov     eax, 0061E884h
        jmp     eax
    }
}

__declspec(naked) void fixDefaultRecSpeed() //0042E8E2
{
    __asm
    {
        mov     [esi + 530h], ebx //turn off demo loop
        mov     [esi + 52Ch], ebx //turn off view lock
        mov     dword ptr [esi + 540h], 2
        mov     eax, 0042E8F4h
        jmp     eax
    }
}

int __stdcall check_main_view_redraw(TRIBE_Screen_Game* game_screen, unsigned int t, unsigned int wt_delta)
{
    if (isRec())
    {
        int result = 0;
        switch ((*comm)->mCommandLog->mReplaySpeed)
        {
        default:
        case 0:
        case 1:
        case 2:
        case 3:
            result = (wt_delta || t - game_screen->last_view_time >= game_screen->view_interval);
            break;
        case 4:
        case 5:
        case 6:
            result = (t - game_screen->last_view_time >= 16);
            break;
        case 7:
        case 8:
        case 9:
            result = (t - game_screen->last_view_time >= 33);
            break;
        case 10:
        case 11:
            result = (t - game_screen->last_view_time >= 100);
            break;
        case 12:
            result = (t - game_screen->last_view_time >= 333);
            break;
        }
        if ((*comm)->mCommandLog->mReplaySpeed > 5 && result)
        {
            game_screen->time_panel->vfptr->set_redraw(game_screen->time_panel, 1);
            game_screen->time_panel->parent_panel->vfptr->set_redraw(game_screen->time_panel->parent_panel, 1);
        }
        return result;
    }
    else
        return (wt_delta || t - game_screen->last_view_time >= game_screen->view_interval);
}

__declspec(naked) void on_main_view_redraw() //004F8DD6
{
    __asm
    {
        mov     ebx, [esp + 18h]
        mov     edi, [esp + 10h]
        push    ebx
        push    edi
        push    esi
        call    check_main_view_redraw
        test    eax, eax
        jz      skip_main_view_redraw
        
        mov     eax, 004F8DF6h
        jmp     eax

skip_main_view_redraw:
        mov     eax, 004F8E09h
        jmp     eax
    }
}

int __stdcall check_time_panel_redraw(TRIBE_Panel_Time* time_panel)
{
    TRIBE_Screen_Game* game_screen = ((TRIBE_Game*)(*base_game))->game_screen;
    if (game_screen
        && game_screen->time_panel == time_panel
        && time_panel->clock_type == 2
        && isRec()
        && (*comm)->mCommandLog->mReplaySpeed > 5)
        return false;
    else
        return time_panel->game_time != time_panel->last_game_time;
}

__declspec(naked) void on_time_panel_redraw() //005DFFA5
{
    __asm
    {
        push    esi
        call    check_time_panel_redraw
        test    eax, eax
        jz      skip_time_panel_redraw

        mov     eax, 005DFFB5h
        jmp     eax

skip_time_panel_redraw:
        mov     eax, 005DFFD4h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setGameSpeedHooks()
{
    setHook((void*)0x0050189E, onCheckGameSpeed);

    //recorded game
    writeByte(0x0061EBC2, REC_SPEED_COUNT);
    setHook((void*)0x0061EB81, onCheckSpeed2);

    writeNops(0x0061EB66, 6);
    writeNops(0x0061EB6F, 6);

    setHook((void*)0x0061EABA, new_rec_speed_control);

    setHook((void*)0x004FAF6B, onSlowDown);
    setHook((void*)0x004FAF38, onSpeedUp);
    setHook((void*)0x004FAF9F, onSpeedNormal);

    setHook((void*)0x005E01E6, onPrintTime);

    setHook((void*)0x0042E8E2, fixDefaultRecSpeed);

    setHook((void*)0x004F8DD6, on_main_view_redraw);
    setHook((void*)0x005DFFA5, on_time_panel_redraw);
}
#pragma optimize( "", on )
