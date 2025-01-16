#include "stdafx.h"
#include "aiactions.h"

TRIBE_Player** thisPlayer = (TRIBE_Player**)0x0077FDAC;
TribeInformationAIModule** thisInformationAI = (TribeInformationAIModule**)0x0077FDBC;

short __fastcall get_player_built_object_category_count(TRIBE_Player* player, int c)
{
    return (*(short**)((unsigned __int8*)player + 0x1038))[c];
}

RGE_Static_Object* __stdcall find_first_object(RGE_Player_Object_List* list, int obj_id)
{
    for (int i = 0; i < list->Number_of_objects; i++)
        if (list->List[i]->master_obj->id == obj_id && list->List[i]->object_state == 2)
            return list->List[i];
    return NULL;
}

void __fastcall ai_delete_building_new(int obj_id)
{
    if (get_player_built_object_category_count(*thisPlayer, obj_id) >= 1)
    {
        int other_obj_id = obj_id;
        int unit_line = TribeInformationAIModule__getObjectLineType(*thisInformationAI, obj_id);
        if (unit_line != -1)
            other_obj_id = TribeInformationAIModule__getFirstInLine(*thisInformationAI, unit_line);

        RGE_Static_Object* obj = find_first_object((*thisPlayer)->objects, obj_id);
        if (!obj && other_obj_id != obj_id)
            obj = find_first_object((*thisPlayer)->objects, other_obj_id);
        if (!obj)
            obj = find_first_object((*thisPlayer)->sleeping_objects, obj_id);
        if (!obj && other_obj_id != obj_id)
            obj = find_first_object((*thisPlayer)->sleeping_objects, other_obj_id);

        if (obj)
            TRIBE_Player__command_delete_object(*thisPlayer, obj);
    }
}

int(__thiscall* AIExpert__defAction)(AIExpert* this_, const char* name, unsigned __int8 argCount, void(__fastcall* function)()) =
    (int(__thiscall*)(AIExpert*, const char*, unsigned __int8, void(__fastcall*)()))0x00410F60;

void __stdcall init_new_ai_actions(AIExpert* expert)
{
    AIExpert__defAction(expert, "delete-complete-building", 1, (void(__fastcall*)())ai_delete_building_new);
}

__declspec(naked) void on_def_actions() //00577BF1
{
    __asm
    {
        push    esi
        call    init_new_ai_actions
        mov     ecx, [esp + 8]
        pop     esi

        mov     eax, 00577BF6h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setAIActionsHooks()
{
    setHook((void*)0x00577BF1, on_def_actions);
}
#pragma optimize( "", on )
