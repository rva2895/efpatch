#pragma once

void setTriggerDescHooks();

struct condition
{
	void*	vfptr;
	int		id;
	int		unk1;
	int		quantity;
	int		attrib_list;
	int		obj;
	int		nextobj;
	int		obj_list_type;
	int		player;
	int		tech;
	int		timer;
	int		trigger;
	int		area_x1;
	int		area_y1;
	int		area_x2;
	int		area_y2;
	int		obj_group;
	int		obj_type;
	int		ai_signal;
};

struct effect
{
	void*	vfptr;
	int		id;
	int		unk1;
	int		ai_trigger_number;
	int		quantity;
	int		attrib_list;
	int		alliance;
	int		obj;
	int		obj2;
	int		obj_list;
	int		source_player;
	int		target_player;
	int		tech;
	int		message;
	int		sound;
	int		timer;
	int		trigger;
	int		location_x;
	int		location_y;
	int		area_x1;
	int		area_y1;
	int		area_x2;
	int		area_y2;
	int		obj_group;
	int		obj_type;
	int		number;
	int		unk2;
	char*	str;
};
