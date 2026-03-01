#include "stdafx.h"

#include "effects.h"
#include "advtriggereffect.h"
#include "effectUnitVar.h"
#include "triggerdesc.h"

extern std::map<const std::string, const CHANGE_UNIT_MASTER_PARAMS&> unit_master_lookup_table;

char* s;

extern const char* resourceNames[];

void __stdcall c_default(condition*, int) {};
void __stdcall e_default(effect*, int) {};

void __stdcall c_timer(condition* p, int)
{
    sprintf(s + strlen(s), " (%d)", p->timer);
}

void __stdcall c_quantity(condition* p, int)
{
    sprintf(s + strlen(s), " (%d)", p->quantity);
}

//void __stdcall c_per_mille(condition* p, int)
//{
//    sprintf(s + strlen(s), " (%d)", p->quantity);
//}

void __stdcall c_attrib(condition* p, int)
{
    //char* name;
    //name = resourceNames[p->quantity];
    sprintf(s + strlen(s), " (%s: %d)", resourceNames[p->attrib_list], p->quantity);
}

void __stdcall c_player(condition* p, int)
{
    sprintf(s + strlen(s), " (P%d)", p->player);
}

void __stdcall c_player_tech(condition* p, int)
{
    sprintf(s + strlen(s), " (P%d: %d)", p->player, p->tech);
}

int get_alliance_state_string_id(int state)
{
    switch (state)
    {
    case 0: //Ally
        return 10701;
    case 1: //Neutral
        return 10702;
    case 3: //Enemy
        return 10703;
    default: //Unknown
        return 1531;
    }
}

void __stdcall c_alliance_state(condition* p, int)
{
    sprintf(s + strlen(s), " (P%d->P%d: %s)", p->player, p->timer, get_string(get_alliance_state_string_id(p->ai_signal)));
}

extern const char* var_names[];

void __stdcall c_var(condition* p, int)
{
    char* op;
    switch (p->id)
    {
    case 0x1B:
        (p->trigger & 1) ? op = ">=" : op = "<";
        break;
    case 0x1C:
        (p->trigger & 1) ? op = "=" : op = "!=";
        break;
    default:
        op = "";
        break;
    }
    sprintf(s + strlen(s), " (%s %s %d)", var_names[p->ai_signal], op, p->timer);
}

void __stdcall c_civ(condition* p, int)
{
    sprintf(s + strlen(s), " (P%d: %s)", p->player, get_string(10230 + p->ai_signal));
}

void(__stdcall* condPrint[]) (condition*, int) =
{
    c_default, //none
    c_default, //bring obj to obj
    c_default, //bring obj to area
    c_quantity, //own obj
    c_quantity, //own fewer obj
    c_quantity, //obj in area
    c_default, //dest obj
    c_default, //capt obj
    c_attrib, //acc attrib
    c_player_tech, //research tech
    c_timer, //timer
    c_default, //obj sel
    c_default, //ai sign
    c_player, //pl def
    c_default, //obj has t
    c_default, //obj vis
    c_default, //obj not vis
    c_player_tech, //researching tech
    c_quantity, //units garrisoned
    c_default, //diff
    c_quantity, //own fewer fnd
    c_quantity, //sel obj in area
    c_quantity, //pow obj in area
    c_default, //units q past pop cap
    c_quantity, //per mille chance
    c_quantity, //area explored
    c_alliance_state, //alliance state
    c_var, //var ge
    c_var, //var e
    c_civ, //player civ
    c_default //or
};

std::string trigger_get_formatted_ui_string(const char* str, bool truncate_newline, size_t max_len)
{
    std::string str_formatted(str);
    size_t pos_newline = str_formatted.find('\r');
    if (pos_newline == std::string::npos)
    {
        pos_newline = str_formatted.find('\n');
    }
    size_t trunc_pos = min(pos_newline, max_len);
    if (trunc_pos != std::string::npos)
    {
        str_formatted.resize(trunc_pos);
        str_formatted += " ...";
    }
    return str_formatted;
}

void __stdcall e_send_chat(effect* p, int)
{
    sprintf(s + strlen(s), " (P%d: ", p->source_player);
    if (!p->str)
    {
        sprintf(s + strlen(s), ")");
    }
    else
    {
        strcpy(s + strlen(s), trigger_get_formatted_ui_string(p->str, true, 20).c_str());
        sprintf(s + strlen(s), ")");
    }
}

void __stdcall e_tribute(effect* p, int)
{
    sprintf(s + strlen(s), " (P%d->P%d: %s: %d)",
        p->source_player, p->target_player, resourceNames[p->attrib_list], p->quantity);
}

void __stdcall e_ownership(effect* p, int)
{
    sprintf(s + strlen(s), " (P%d->P%d)",
        p->source_player, p->target_player);
}

void __stdcall e_str(effect* p, int)
{
    sprintf(s + strlen(s), " (%s)", p->str ? trigger_get_formatted_ui_string(p->str, true, 20).c_str() : "");
}

void __stdcall e_quantity(effect* p, int)
{
    sprintf(s + strlen(s), " (%d)", p->quantity);
}

void __stdcall e_player_unit(effect* p, int)
{
    sprintf(s + strlen(s), " (P%d: %d)", p->source_player, p->obj_list);
}

void __stdcall e_player_tech(effect* p, int)
{
    sprintf(s + strlen(s), " (P%d: %d)", p->source_player, p->tech);
}

void __stdcall e_player(effect* p, int)
{
    sprintf(s + strlen(s), " (P%d)", p->source_player);
}

void __stdcall e_stance(effect* p, int)
{
    int st;
    if (p->ai_trigger_number == -1)
    {
        st = 1511; //"Freeze Unit"
    }
    else
    {
        switch (p->ai_trigger_number)
        {
        case 1:
            st = 1512; //"Aggressive"
            break;
        case 2:
            st = 1513; //"Defensive"
            break;
        case 3:
            st = 1514; //"Stand Ground"
            break;
        case 4:
            st = 1515; //"No Attack"
            break;
        default:
            st = 1531; //"Unknown"
            break;
        }
    }
    sprintf(s + strlen(s), " (%s)", get_string(st));
}

/*
void __stdcall e_command(effect* p, int)
{
    char* st;
    switch (p->ai_trigger_number)
    {
    default:
        st = "TODO";
        break;
    }
    sprintf(s + strlen(s), " (%s)", st);
}
*/

void __stdcall e_trigger(effect* p, int)
{
    TRIBE_Trigger* t = TRIBE_Trigger_System__get_trigger((*base_game)->world->trigger_system, p->trigger);
    sprintf(s + strlen(s), " (%s)", t ? t->name : get_string(1532));
}

void __stdcall e_alliance(effect* p, int)
{
    sprintf(s + strlen(s), " (P%d->P%d: %s)", p->source_player, p->target_player, get_string(get_alliance_state_string_id(p->alliance)));
}

void __stdcall e_str_obj_master(effect* p, int)
{
    char error_msg[0x100];
    bool result = advTriggerEffect_do_multi_line_effect(NULL, NULL, NULL, p->str, false, error_msg, _countof(error_msg));
    
    sprintf(s + strlen(s), " (%s)", result ? trigger_get_formatted_ui_string(p->str, true, 20).c_str() : error_msg);
}

void __stdcall e_str_obj_var(effect* p, int)
{
    char error_msg[0x100];
    bool result = effectUnitVar_do_multi_line_effect(NULL, p->str, false, error_msg, _countof(error_msg));
    sprintf(s + strlen(s), " (%s)", result ? trigger_get_formatted_ui_string(p->str, true, 20).c_str() : error_msg);
}

void __stdcall e_location(effect* p, int)
{
    sprintf(s + strlen(s), " (%d,%d)", p->location_x, p->location_y);
}

extern int terrains_loaded;
extern int* terrain_language_dll;

void __stdcall e_terrain(effect* p, int)
{
    sprintf(s + strlen(s), " (%s)",
        get_string((p->ai_trigger_number >= 0) && (p->ai_trigger_number < terrains_loaded) ? terrain_language_dll[p->ai_trigger_number] : 1531));
}

void __stdcall e_goal(effect* p, int)
{
    sprintf(s + strlen(s), " (P%d: %d)", p->source_player, p->ai_trigger_number);
}

void __stdcall e_copy_obj(effect* p, int)
{
    sprintf(s + strlen(s), " (%d -> %d)",
        p->quantity, p->obj_list);
}

void __stdcall e_transform_obj(effect* p, int)
{
    sprintf(s + strlen(s), " (-> %d)",
        p->quantity);
}

void(__stdcall* effectPrint[]) (effect*, int) =
{
    e_default, //none
    e_alliance, //change alliance
    e_player_tech, //research tech
    e_send_chat, //send chat
    e_default, //play sound
    e_tribute, //tribute
    e_default, //unlock gate
    e_default, //lock gate
    e_trigger, //activate trigger
    e_trigger, //deactivate trigger
    e_goal, //ai script goal
    e_player_unit, //create object
    e_location, //task object
    e_player, //victory
    e_default, //kill object
    e_default, //remove object
    e_location, //scroll view
    e_location, //unload
    e_ownership, //change ownership
    e_location, //patrol
    e_str, //display inst
    e_default, //clear inst
    e_stance, //freeze unit
    e_default, //enable adv buttons
    e_quantity, //damage obj
    e_player_unit, //place fnd
    e_str, //change name
    e_quantity, //change hp
    e_quantity, //change atk
    e_default, //stop unit
    e_location, //snap view
    e_default, //disable adv btn
    e_player_tech, //enable tech
    e_player_tech, //disable tech
    e_player_unit, //enable unit
    e_player_unit, //disable unit
    e_default, //flash obj
    e_default, //turn input off
    e_default, //turn input on
    e_quantity, //change speed
    e_quantity, //give ability
    e_quantity, //remove ability
    e_str_obj_master, //change own master
    e_str_obj_master, //change player master
    e_player, //explore
    e_str_obj_var, //change var
    e_terrain, //terrain
    e_player, //defeat
    e_copy_obj, //copy obj
    e_transform_obj, //transform obj
    e_location, //teleport
    //e_command, //command
    e_default //breakpoint
};

void __stdcall scanCond(condition* p, int i)
{
    sprintf(s, "C#%d: ", i);
    if (!(p->trigger & 0xFF))
        strcpy(s + strlen(s), "!");
    sprintf(s + strlen(s), "%s", get_string(1700 + p->id));
    condPrint[p->id](p, i);
}

void __stdcall scanEffect(effect* p, int i)
{
    sprintf(s, "E#%d: %s", i, get_string(1800 + p->id));
    effectPrint[p->id](p, i);
}

__declspec(naked) void onCondition() //0053D96B
{
    __asm
    {
        //edi - index
        //ebx - condition ptr
        push    edi
        push    ebx
        call    scanCond
        //edx - string
        mov     edx, s
        mov     ecx, [esi + 0DACh]
        mov     eax, 0053D9BDh
        jmp     eax
    }
}

__declspec(naked) void onEffect() //0053DA8D
{
    __asm
    {
        //edi - index
        //ebx - condition ptr
        push    edi
        push    ebx
        call    scanEffect
        //edx - string
        mov     edx, s
        mov     ecx, [esi + 0DACh]
        mov     eax, 0053DADFh
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setTriggerDescHooks()
{
    s = new char[0x400];

    setHook((void*)0x0053D96B, onCondition);
    setHook((void*)0x0053DA8D, onEffect);
}
#pragma optimize( "", on )
