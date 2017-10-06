#pragma once

#include "stdafx.h"
#include "triggerdesc.h"

struct prop_object
{
	void* _vfptr;
	char type;
	int unk008;
	int unk00C;
	short Unitline;
	char MinTechLevel;
	short LanguageDLLName;
	short LanguageDLLCreation;
	short id1;
	short id2;
	short id3;
	short unit_class;
	//TODO: fill

};

struct unit
{
	void* _vfptr;		//0
	int unk04;
	int unk08;
	int unk0C;
	int ordinal;		//10
	prop_object* prop_object;
	void* player;
	int unk1C;
	int unk20;			//20
	int unk24;
	int unk28;
	int unk2C;
	int unk30;			//30
	int unk34;
	int unk38;
	float hp;
	float sp;			//40
	int unk44;
	float x;
	float y;
	int unk50;			//50
	float resources;
	//TODO: fill

};

struct trigger
{
	int enabled;		//0
	int unk04;
	int unk08;
	char* desc;
	int unk10;			//10
	char* name;
	condition** conditions;
	int unk1C;
	int n_conditions;	//20
	int unk24;
	effect** effects;
	int unk2C;
	int n_effects;		//30
	int unk34;
	int unk38;
	int activated_at;
};

void setTriggerUnitHooks();
