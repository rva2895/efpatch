#include "stdafx.h"

#include "effects.h"
#include "editpropertyobject.h"
#include "advtriggereffect.h"

void __stdcall changePropertyObject_do_call(int player_id, int object_id, const char* s)
{
    if ((object_id != -1) && (player_id < ((TRIBE_Game*)(*base_game))->game_screen->world->player_num))
    {
        RGE_Player* player = (RGE_Player*)((TRIBE_Game*)(*base_game))->game_screen->world->players[player_id];
        if (object_id < player->master_object_num && player->master_objects[object_id])
            advTriggerEffect_do_multi_line_effect(player->master_objects[object_id], NULL, player, s, true, NULL, 0);
    }
}

__declspec(naked) void changePropertyObjectHook()
{
    __asm
    {
        mov     eax, [edi + 6Ch]
        mov     ecx, [edi + 24h]
        mov     edx, [edi + 28h]
        push    eax
        push    ecx
        push    edx
        call    changePropertyObject_do_call

        mov     ebx, 005F3DB1h
        jmp     ebx
    }
}
