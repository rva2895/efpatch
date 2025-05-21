#pragma once

bool __stdcall effectUnitVar_do_single_line_effect(
    RGE_Static_Object* unit,
    char* str,
    bool do_effect,
    char* error_msg,
    size_t error_msg_size);

bool __stdcall effectUnitVar_do_multi_line_effect(
    RGE_Static_Object* unit,
    char* str,
    bool do_effect,
    char* error_msg,
    size_t error_msg_size);
