#include "stdafx.h"
#include "trigger_unit.h"

#include <regex>

#pragma message("***trigger_unit.cpp: ---NOTE TO SELF---: Complete UNIT and PROPOBJ struct definition")

void patch_trigger(trigger* t, int player, float x, float y)
{
	float xr;
	float yr;
	for (int i = 0; i < t->n_conditions; i++)
	{
		if ((t->conditions[i]->area_x1 != -1) ||
			(t->conditions[i]->area_x2 != -1) ||
			(t->conditions[i]->area_y1 != -1) ||
			(t->conditions[i]->area_y2 != -1))
		{
			xr = (float)(t->conditions[i]->area_x2 - t->conditions[i]->area_x1 + 1) / 2;
			yr = (float)(t->conditions[i]->area_y2 - t->conditions[i]->area_y1 + 1) / 2;
			if ((xr > 0) && (yr > 0))
			{
				t->conditions[i]->area_x1 = x - xr;
				t->conditions[i]->area_x2 = x + xr;
				t->conditions[i]->area_y1 = y - yr;
				t->conditions[i]->area_y2 = y + yr;
				/*if (t->conditions[i]->area_x1 < 0)
					t->conditions[i]->area_x1 = 0;
				if (t->conditions[i]->area_x2 < 0)
					t->conditions[i]->area_x2 = 0;
				if (t->conditions[i]->area_y1 < 0)
					t->conditions[i]->area_y1 = 0;
				if (t->conditions[i]->area_y2 < 0)
					t->conditions[i]->area_y2 = 0;*/
			}
		}
		if ((t->conditions[i]->player > 0) && (player != -1))
			t->conditions[i]->player = player;
	}
	for (int i = 0; i < t->n_effects; i++)
	{
		if ((t->effects[i]->area_x1 != -1) ||
			(t->effects[i]->area_x2 != -1) ||
			(t->effects[i]->area_y1 != -1) ||
			(t->effects[i]->area_y2 != -1))
		{
			xr = (float)(t->effects[i]->area_x2 - t->effects[i]->area_x1 + 1) / 2;
			yr = (float)(t->effects[i]->area_y2 - t->effects[i]->area_y1 + 1) / 2;
			if ((xr > 0) && (yr > 0))
			{
				t->effects[i]->area_x1 = x - xr;
				t->effects[i]->area_x2 = x + xr - 1;
				t->effects[i]->area_y1 = y - yr;
				t->effects[i]->area_y2 = y + yr - 1;
				/*if (t->effects[i]->area_x1 < 0)
					t->effects[i]->area_x1 = 0;
				if (t->effects[i]->area_x2 < 0)
					t->effects[i]->area_x2 = 0;
				if (t->effects[i]->area_y1 < 0)
					t->effects[i]->area_y1 = 0;
				if (t->effects[i]->area_y2 < 0)
					t->effects[i]->area_y2 = 0;*/
			}
		}
		if ((t->effects[i]->location_x != -1) && (t->effects[i]->location_y != -1))
		{
			t->effects[i]->location_x = x;
			t->effects[i]->location_y = y;
		}

		if ((t->effects[i]->source_player > 0) && (player != -1))
			t->effects[i]->source_player = player;
	}
}

void(__thiscall* trigger_process) (void*, int) = (void(__thiscall*) (void*, int))0x005F49E0;

void __stdcall trigger_process2(trigger* t, int arg0)
{
	if (strlen(t->desc) > 0)
	{
		std::regex r("\\$unit\\:([0-9]+),\\$player:([0-9]);(\r*\n*.*)*",
			std::regex_constants::icase);

		std::smatch h;
		std::string desc(t->desc);

		if (std::regex_match(desc, h, r))
		{
			int id;
			std::ssub_match h_sub = h[1];
			std::string match = h_sub.str();
			sscanf(match.c_str(), "%d", &id);

			int player;
			h_sub = h[2];
			match = h_sub.str();
			sscanf(match.c_str(), "%d", &player);

			void* ptr = *(void**)(*(int*)(*(int*)(*(int*)(*(int*)(*(int*)(0x6A3684) + 0x17B4) + 0x126C) + 0x4C) + player * 4) + 0x78);
			UNIT** units_ptr = *(UNIT***)((int)ptr + 4);
			int n = *(int*)((int)ptr + 8);

			UNIT** units = (UNIT**)malloc(sizeof(UNIT*)*n);
			memcpy(units, units_ptr, sizeof(UNIT*)*n);

			for (int i = 0; i < n; i++)
			{
				if (units[i]->prop_object->id1 == id)
				{
					if ((units[i]->prop_object->type == 80) && (*(float*)((int)units[i] + 0x230) < *(short*)((int)units[i]->prop_object + 0x18E)))
						continue;
					patch_trigger(t, player, units[i]->x, units[i]->y);
					trigger_process(t, arg0);
				}
			}

			free(units);
		}
	}
	else
		trigger_process(t, arg0);
}

__declspec(naked) void on_trigger() //005F54D2
{
	__asm
	{
		mov     ecx, [esi + 10h]
		push    ebx
		mov     ecx, [ecx + edi * 4]
		push    ecx
		call    trigger_process2
		mov     eax, 005F54DEh
		jmp     eax
	}
}

void setTriggerUnitHooks()
{
	//setHook((void*)0x005F54D2, &on_trigger);
}
