#include "stdafx.h"

#include "globalvar.h"

__declspec(naked) void setVarHook ()
{
	__asm
	{
		mov   [ebx+0Ah], al
		push  007B2ADDh
		ret
	}
}

__declspec(naked) void setVarEffect ()
{
	__asm
	{
		mov   eax, [edi + 6Ch]
		push  eax
		call  setVar
		push  005F3DB1h
		ret
	}
}

GLOBAL_VAR* vars;
int nVars;

GLOBAL_VAR* findVar (const char* name)
{
	for (int i = 0; i < nVars; i++)
	{
		if (!strcmp (name, vars[i].name))
			return &vars[i];
	}
	return 0;
}

void __stdcall setVar (const char* str)
{
	char szVar [20];
	char szVal [20];
	sscanf (str, "%s %s", szVar, szVal);
	GLOBAL_VAR* var;
	if (var = findVar (szVar))
	{
		if (var->len < strlen (szVal))
		{
			free (var->val);
			var->val = (char*) malloc (strlen(szVal)+1);
			strcpy (var->val, szVal);
			var->len = strlen (var->val);
		}
		else
			strcpy (var->val, szVal);
	}
	else
	{
		nVars++;
		vars = (GLOBAL_VAR*) realloc (vars, nVars*sizeof(GLOBAL_VAR));
		vars[nVars-1].name = (char*) malloc (strlen(szVar)+1);
		strcpy (vars[nVars-1].name, szVar);
		vars[nVars-1].val = (char*) malloc (strlen(szVal)+1);
		strcpy (vars[nVars-1].val, szVal);
		vars[nVars-1].len = strlen (szVal);
	}
}