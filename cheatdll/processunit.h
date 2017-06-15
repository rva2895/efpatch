#pragma once

void setProcessUnitHooks ();

struct trigger_data
{
	char** table_ptr;
	int    effect;
	int    zero1;
	int    AI_Trigger_Number;
	int    Quantity;
	int    Attributes_List;
	int    Alliance_State;
	int    Objects1;
	int    Objects2;
	int    Object;
	int    Source_Player;
	int    Target_Player;
	int    Technologies;
	int    Message;
	int    Sound_File;
	int    Timer;
	int    Trigger_List;
	int    Location_x;
	int    Location_y;
	int    Area_x1;
	int    Area_y1;
	int    Area_x2;
	int    Area_y2;
	int    Object_Group;
	int    Object_Type;
	int    Number;
	int    zero2;
	char*  str;
	int    zero3;
};

void __stdcall make_unit (void*, int);
