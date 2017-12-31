#pragma once

void setZannFortHooks ();

//research tech: [this + 8]: ptr to researches

struct research
{
	char* name;
	char* name2;
	short required_techs[6];
	short min_required_techs;
	short civ;
	short full_mode;
	short cost1_type;
	short cost2_type;
	short cost3_type;
	short cost1_amount;
	short cost2_amount;
	short cost3_amount;
	char cost1_paid;
	char cost2_paid;
	char cost3_paid;
	char padding1;
	short time;
	short effect;
	short type;
	short icon;
	char button;
	char padding2;
	short location;
	short lang_name;
	short lang_desc;
	short padding3;
	int lang_help;
	int lang_tech_tree;
	int hotkey;

	//char p[0x40];
};
