#pragma once

struct GLOBAL_VAR
{
	char* name;
	char* val;
	int len;
};

void setVarHook ();
void setVarEffect ();

void __stdcall setVar (const char* str);
