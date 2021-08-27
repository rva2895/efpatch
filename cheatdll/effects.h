#pragma once

enum master_data_types
{
    T_INT8,
    T_INT16,
    T_INT32,
    T_FLOAT,
    T_PTR_G
};

enum master_data_operations
{
    OP_SET,
    OP_ADD,
    OP_MUL
};

struct assign
{
    char* str;
    uint32_t offset;
    master_data_types type;
};

void setEffectHooks();
void triggerDisplayHook();
void effectUnitVar();
void __stdcall effectUnitVarActual(UNIT* unit, char* str);
uint32_t getArrayIndex(const char* txt, master_data_types* type);
