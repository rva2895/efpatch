#pragma once

void advTriggerEffect();
void __stdcall advTriggerEffect_do_single_line_effect(RGE_Master_Static_Object* master, RGE_Static_Object* obj, RGE_Player* player, const char* s);
void __stdcall advTriggerEffect_do_multi_line_effect(RGE_Master_Static_Object* master, RGE_Static_Object* obj, RGE_Player* player, const char* s);
void build_unit_master_variable_lookup_table();
