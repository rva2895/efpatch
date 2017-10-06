#include "stdafx.h"

#include "triggerdesc.h"

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
	"Attrib",				//0x08
	"Research Tech",
	"Timer",
	"Object Selected",
	"AI Signal",
	"Player Defeated",
	"Object Has Target",
	"Object Visible",
	"Object Not Visible",	//0x10
	"Researching Tech",
	"Units Garrisoned",
	"Difficulty",
	"Own Fewer Foundations",
	"Selected Obj In Area",
	"Powered Obj In Area",
	"Units Queued Past Pop Cap",
	"Per Mille Chance",		//0x18
	"Area Explored",
	"Alliance",
	"Var",
	"Var"
};

const char* effectNames[] =
{
	"None",
	"Alliance",
	"Research Tech",
	"Chat",
	"Play Sound",
	"Tribute",
	"Unlock Gate",
	"Lock Gate",
	"Trigger On",		//0x08
	"Trigger Off",
	"AI Script Goal",
	"Create Object",
	"Task Object",
	"Declare Victory",
	"Kill Object",
	"Remove Object",
	"Scroll View",		//0x10
	"Unload",
	"Ownership",
	"Patrol",
	"Instr",
	"Clear Instructions",
	"Freeze Unit",
	"Enable Adv Btns",
	"Damage Object",	//0x18
	"Place Foundation",
	"Name",
	"Change HP",
	"Change Attack",
	"Stop Unit",
	"Snap View",
	"Disable Adv Btns",
	"Enable Tech",		//0x20
	"Disable Tech",
	"Enable Unit",
	"Disable Unit",
	"Flash Objects",
	"Turn Input Off",
	"Turn Input On",
	"Change Speed",
	"Give Ability",		//0x28
	"Remove Ability",
	"Data",
	"Prop",
	"Explore",
	"Var",
	"Breakpoint"
};

void __stdcall condGeneral(condition* p, int index)
{
	int cn = *(int*)((int)p + 4);
	sprintf(s, "C#%d:%s", index, condNames[cn]);
}

void __stdcall nullsub(condition*, int) {};
void __stdcall nullsub(effect*, int) {};

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
//	sprintf(s + strlen(s), " (%d)", p->quantity);
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

char* alliance_states[] =
{
	"Ally",
	"Neutral",
	0,
	"Enemy"
};

void __stdcall c_alliance_state(condition* p, int)
{
	sprintf(s + strlen(s), " (P%d->P%d: %s)", p->player, p->timer, alliance_states[p->ai_signal]);
}

extern char* var_names[];

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
		break;
	}
	sprintf(s + strlen(s), " (%s %s %d)", var_names[p->ai_signal], op, p->timer);
}

void(__stdcall* condPrint[]) (condition*, int) =
{
	nullsub, //none
	nullsub, //bring obj to obj
	nullsub, //bring obj to area
	c_quantity, //own obj
	c_quantity, //own fewer obj
	c_quantity, //obj in area
	nullsub, //dest obj
	nullsub, //capt obj
	c_attrib, //acc attrib
	nullsub, //res tech
	c_timer, //timer
	nullsub, //obj sel
	nullsub, //ai sign
	c_player, //pl def
	nullsub, //obj has t
	nullsub, //obj vis
	nullsub, //obj not vis
	nullsub, //researching tech
	c_quantity, //units garrisoned
	nullsub, //diff
	c_quantity, //own fewer fnd
	c_quantity, //sel obj in area
	c_quantity, //pow obj in area
	nullsub, //units q past pop cap
	c_quantity, //per mille chance
	c_quantity, //area explored
	c_alliance_state, //alliance state
	c_var, //var ge
	c_var //var e
};

void __stdcall e_send_chat(effect* p, int)
{
	sprintf(s + strlen(s), " (P%d: ", p->source_player);
	if (strlen(p->str) > 20)
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
	sprintf(s + strlen(s), " (%s)", p->str);
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

void* getTrigger(int i)
{
	void* (__thiscall* f) (void*, int) = (void* (__thiscall*) (void*, int))0x5F5F20;
	return f((void*)*(int*)(*(int*)(*(int*)0x6A3684 + 0x420) + 0x2B8), i);
}

void __stdcall e_trigger(effect* p, int)
{
	void* t = getTrigger(p->trigger);
	if (t)
		sprintf(s + strlen(s), " (%s)", *(char**)((int)t + 0x14));
	else
		strcpy(s + strlen(s), " (none)");
}

void __stdcall e_alliance(effect* p, int)
{
	sprintf(s + strlen(s), " (P%d->P%d: %s)", p->source_player, p->target_player, alliance_states[p->alliance]);
}

void(__stdcall* effectPrint[]) (effect*, int) =
{
	nullsub, //none
	e_alliance, //change alliance
	e_player_tech, //research tech
	e_send_chat, //send chat
	nullsub, //play sound
	e_tribute, //tribute
	nullsub, //unlock gate
	nullsub, //lock gate
	e_trigger, //activate trigger
	e_trigger, //deactivate trigger
	nullsub, //ai script goal
	nullsub, //create object
	nullsub, //task object
	e_player, //declare victory
	nullsub, //kill object
	nullsub, //remove object
	nullsub, //scroll view
	nullsub, //unload
	e_ownership, //change ownership
	nullsub, //patrol
	e_str, //display inst
	nullsub, //clear inst
	nullsub, //freeze unit
	nullsub, //enable adv buttons
	e_quantity, //damage obj
	nullsub, //place fnd
	e_str, //change name
	e_quantity, //change hp
	e_quantity, //change atk
	nullsub, //stop unit
	nullsub, //snap view
	nullsub, //disable adv btn
	e_player_tech, //enable tech
	e_player_tech, //disable tech
	e_player_unit, //enable unit
	e_player_unit, //disable unit
	nullsub, //flash obj
	nullsub, //turn input off
	nullsub, //turn input on
	e_quantity, //change speed
	e_quantity, //give ability
	e_quantity, //remove ability
	e_str, //change data
	e_str, //change prop obj
	e_player, //explore
	e_str, //change var
	nullsub //breakpoint
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
		push	edi
		push	ebx
		call	scanCond
		//edx - string
		mov		edx, s
		mov		ecx, [esi + 0DACh]
		mov		eax, 0053D9BDh
		jmp		eax
	}
}

__declspec(naked) void onEffect() //0053DA8D
{
	__asm
	{
		//edi - index
		//ebx - condition ptr
		push	edi
		push	ebx
		call	scanEffect
		//edx - string
		mov		edx, s
		mov		ecx, [esi + 0DACh]
		mov		eax, 0053DADFh
		jmp		eax
	}
}

void setTriggerDescHooks()
{
	s = new char[256];

	setHook((void*)0x0053D96B, &onCondition);
	setHook((void*)0x0053DA8D, &onEffect);
}
