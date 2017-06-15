#pragma once

void setEffectHooks ();

void triggerDisplayHook ();
void triggerInputTableHook ();

void effectUnitVar ();

void __stdcall effectUnitVarActual (void* unit, char* str);

struct assign
{
	char*  str;
	short  offset;
	//char   size;
	char   type;
	//bool   cap;
	//int    capOffset;
};

#define T_INT8 0
#define T_INT16 1
#define T_INT32 2
#define T_FLOAT 3
#define T_PTR_G 4
