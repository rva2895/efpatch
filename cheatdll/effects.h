#pragma once

enum UNIT_MASTER_DATA_TYPE
{
    T_FORBIDDEN,
    T_UINT8,
    T_INT16,
    T_INT32,
    T_FLOAT,
    T_SPRITE_PTR,
    T_SOUND_PTR,
    T_ARMOR_CLASS
};

enum UNIT_MASTER_DATA_OPERATION
{
    OP_SET,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    //OP_AND,
    //OP_OR,
    OP_POW
};

struct CHANGE_UNIT_MASTER_PARAMS
{
    unsigned __int8 master_type;
    size_t offset;
    UNIT_MASTER_DATA_TYPE val_type;
    bool is_saved;
    //std::vector<std::string> val_names;
    std::vector<const char*> val_names;
};

void setEffectHooks();
void triggerDisplayHook();
void effectUnitVar();
void __stdcall effectUnitVarActual(RGE_Static_Object* unit, char* str);
