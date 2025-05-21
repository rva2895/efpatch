#pragma once

void advTriggerEffect();

bool __stdcall advTriggerEffect_do_single_line_effect(
    RGE_Master_Static_Object* master,
    RGE_Static_Object* obj,
    RGE_Player* player,
    const char* s,
    bool do_effect,
    char* error_msg,
    size_t error_msg_size);

bool __stdcall advTriggerEffect_do_multi_line_effect(
    RGE_Master_Static_Object* master,
    RGE_Static_Object* obj,
    RGE_Player* player,
    const char* s,
    bool do_effect,
    char* error_msg,
    size_t error_msg_size);

void build_unit_master_variable_lookup_table();
