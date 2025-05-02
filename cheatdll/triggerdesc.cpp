#include "stdafx.h"

#include <regex>
#include <map>
#include "effects.h"
#include "advtriggereffect.h"
#include "triggerdesc.h"

extern std::map<const std::string, const CHANGE_UNIT_MASTER_PARAMS&> unit_master_lookup_table;

char* s;

extern const char* resourceNames[];

const char* condNames[] =
{
    "None",
    "Bring Object to Area",
    "Bring Object to Object",
    "Own Objects",
    "Own Fewer Objects",
    "Objects in Area",
    "Destroy Object",
    "Capture Object",
    "Attrib",                //0x08
    "Research Tech",
    "Timer",
    "Object Selected",
    "AI Signal",
    "Player Defeated",
    "Object Has Target",
    "Object Visible",
    "Object Not Visible",    //0x10
    "Researching Tech",
    "Units Garrisoned",
    "Difficulty",
    "Own Fewer Foundations",
    "Selected Obj In Area",
    "Powered Obj In Area",
    "Units Queued Past Pop Cap",
#ifndef _CC_COMPATIBLE
    "Per Mille Chance",        //0x18
    "Area Explored",
    "Alliance",
    "Var",
    "Var",
    "Civ",
#endif
};

const char* effectNames[] =
{
    "None",
    "Alliance",
    "Research Tech",
    "Chat",
    "Play Sound",
    "Trib",
    "Unlock Gate",
    "Lock Gate",
    "Trigger On",        //0x08
    "Trigger Off",
    "AI Script Goal",
    "Create Object",
    "Task",
    "Victory",
    "Kill Object",
    "Remove Object",
    "Scroll View",        //0x10
    "Unload",
    "Ownership",
    "Patrol",
    "Instr",
    "Clear Instructions",
    "Stance",
    "Enable Adv Btns",
    "Damage Object",    //0x18
    "Place Foundation",
    "Name",
    "Change HP",
    "Change Attack",
    "Stop Unit",
    "Snap View",
    "Disable Adv Btns",
    "Enable Tech",        //0x20
    "Disable Tech",
    "Enable Unit",
    "Disable Unit",
    "Flash Objects",
    "Turn Input Off",
    "Turn Input On",
#ifndef _CC_COMPATIBLE
    "Change Speed",
    "Give Ability",        //0x28
    "Remove Ability",
    "Own",
    "Plr",
    "Explore",
    "Var",
    "Terrain",
    "Defeat",
    "Copy Obj",
    "Xform Obj",
    "Teleport",
    //"Command",
    "Breakpoint"
#endif
};

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

const char* alliance_states[] =
{
    "Ally",
    "Neutral",
    "",
    "Enemy"
};

void __stdcall c_alliance_state(condition* p, int)
{
    sprintf(s + strlen(s), " (P%d->P%d: %s)", p->player, p->timer, alliance_states[p->ai_signal]);
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
    sprintf(s + strlen(s), " (P%d: %s)", p->player, get_string(10230 + p->ai_signal).c_str());
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
#ifndef _CC_COMPATIBLE
    c_quantity, //per mille chance
    c_quantity, //area explored
    c_alliance_state, //alliance state
    c_var, //var ge
    c_var, //var e
    c_civ //player civ
#endif
};

void __stdcall e_send_chat(effect* p, int)
{
    sprintf(s + strlen(s), " (P%d: ", p->source_player);
    if (!p->str)
    {
        sprintf(s + strlen(s), ")");
    }
    else if (strlen(p->str) > 20)
    {
        char* ptr = s + strlen(s) + 20;
        strncpy(s + strlen(s), p->str, 20);
        sprintf(ptr, "...)");
    }
    else
    {
        strcpy(s + strlen(s), p->str);
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
    sprintf(s + strlen(s), " (%s)", p->str ? p->str : "");
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
    if (p->ai_trigger_number == -1)
        sprintf(s + strlen(s), " (%s)", "Freeze Unit");
    else
    {
        char* st;
        switch (p->ai_trigger_number)
        {
        case 1:
            st = "Aggressive";
            break;
        case 2:
            st = "Defensive";
            break;
        case 3:
            st = "Stand Ground";
            break;
        case 4:
            st = "No Attack";
            break;
        default:
            st = "";
            break;
        }
        sprintf(s + strlen(s), " (%s)", st);
    }
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
    sprintf(s + strlen(s), " (%s)", t ? t->name : "none");
}

void __stdcall e_alliance(effect* p, int)
{
    sprintf(s + strlen(s), " (P%d->P%d: %s)", p->source_player, p->target_player, alliance_states[p->alliance]);
}

bool check_data(char* str_)
{
    if (!str_)
        return false;

    std::regex r("^([^ \t]+) ([^ \t]+) ([0-9]+ )?([-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?)$",
        std::regex_constants::icase);

    std::smatch h;
    std::string str(str_);
    if (std::regex_match(str, h, r))
    {
        std::ssub_match h_sub = h[1];
        std::string match = h_sub.str();
        if ((match == "SET") || (match == "ADD") || (match == "MUL"))
        {
            h_sub = h[2];
            match = h_sub.str();
            std::map<const std::string, const CHANGE_UNIT_MASTER_PARAMS&>::iterator r2 = unit_master_lookup_table.find(match);
            if (r2 != unit_master_lookup_table.end())
            {
                if ((*r2).second.val_type == T_ARMOR_CLASS)
                {
                    h_sub = h[3];
                    match = h_sub.str();
                    return match.length() > 0;
                }
                else
                {
                    h_sub = h[3];
                    match = h_sub.str();
                    return match.length() == 0;
                }
            }
            else
                return false;
        }
        else
            return false;
    }
    else
        return false;
}

void __stdcall e_str_data(effect* p, int)
{
    sprintf(s + strlen(s), " (%s)", check_data(p->str) ? p->str : "<syntax error>");
}

void __stdcall e_location(effect* p, int)
{
    sprintf(s + strlen(s), " (%d,%d)", p->location_x, p->location_y);
}

extern char** terrain_names;
extern int terrains_loaded;

void __stdcall e_terrain(effect* p, int)
{
    sprintf(s + strlen(s), " (%s)",
        (p->ai_trigger_number >= 0) && (p->ai_trigger_number < terrains_loaded) ? terrain_names[p->ai_trigger_number] : "Unknown");
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
    sprintf(s + strlen(s), " (To %d)",
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
#ifndef _CC_COMPATIBLE
    e_quantity, //change speed
    e_quantity, //give ability
    e_quantity, //remove ability
    e_str_data, //change data
    e_str_data, //change prop obj
    e_player, //explore
    e_str, //change var
    e_terrain, //terrain
    e_player, //defeat
    e_copy_obj, //copy obj
    e_transform_obj, //transform obj
    e_location, //teleport
    //e_command, //command
    e_default //breakpoint
#endif
};

void __stdcall scanCond(condition* p, int i)
{
    sprintf(s, "C#%d: ", i);
    if (!(p->trigger & 0xFF))
        strcpy(s + strlen(s), "!");
    sprintf(s + strlen(s), "%s", condNames[p->id]);
    condPrint[p->id](p, i);
}

void __stdcall scanEffect(effect* p, int i)
{
    sprintf(s, "E#%d: %s", i, effectNames[p->id]);
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
