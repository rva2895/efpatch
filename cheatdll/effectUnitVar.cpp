#include "stdafx.h"

#include "effects.h"
#include "casts.h"

void editVal (float* valPtr, float val, bool useMax, float max, int action)
{
	//float* hp = (float*)((int)unit+0x3C);
	switch (action)
	{
	case 0: //SET
		*valPtr = val;
		break;
	case 1: //ADD
		*valPtr += val;
		break;
	case 2: //MUL
		*valPtr *= val;
		break;
	default:
		break;
	}
	if (useMax)
		if (*valPtr > max)
			*valPtr = max;
}

void editHP (void* unit, float val, int action)
{
	editVal ((float*)((int)unit+0x3C), val, false, 0, action);
}

void editHPPercent (void* unit, float val, int action)
{
	int propObj = *(int*)((int)unit+0x14);
	float maxHP = *(short*)(propObj+0x32);
	editVal ((float*)((int)unit+0x3C), val*maxHP/100, false, 0, action);
}

void editSP(void* unit, float val, int action)
{
	editVal((float*)((int)unit + 0x40), val, false, 0, action);
}

void editResources(void* unit, float val, int action)
{
	editVal((float*)((int)unit + 0x54), val, false, 0, action);
}

void editSPPercent (void* unit, float val, int action)
{
	int propObj = *(int*)((int)unit+0x14);
	float maxHP = *(short*)(propObj+0x32);
	editVal ((float*)((int)unit+0x40), val*maxHP/100, false, 0, action);
}

void editReloadCooldown (void* unit, float val, int action)
{
	editVal ((float*)((int)unit+0x174), val, false, 0, action);
}

void editReloadCooldownPercent (void* unit, float val, int action)
{
	int propObj = *(int*)((int)unit+0x14);
	float reloadTime = *(short*)(propObj+0x150);
	editVal ((float*)((int)unit+0x174), val*reloadTime/100, false, 0, action);
}

void editHPRegen (void* unit, float val, float val2)
{
	specialDamage (unit, 47, -val, 0);
	specialDamage (unit, 48, val2, 0);
}

void editHPRegenPercent (void* unit, float val, float val2)
{
	specialDamage (unit, 49, -val, 0);
	specialDamage (unit, 50, val2, 0);
}

void __stdcall effectUnitVarActual (void* unit, char* str)
{
	int action;
	char var [50];
	float val;
	float val2;
	char* s = (char*) malloc (strlen(str)+1);
	strcpy (s, str);
	char* pch = strtok (s, " ");
	//while (pch != NULL)
	//{
	if (pch)
	{
		if (!strcmp (pch, "SET"))
			action = 0;
		else if (!strcmp (pch, "ADD"))
			action = 1;
		else if (!strcmp (pch, "MUL"))
			action = 2;
		else
		{
			action = -1;
			free (s);
			return;
		}
		//strcpy (cmd, pch);
	}
	else
	{
		free (s);
		return;
	}
	pch = strtok (NULL, " ");
	if (pch)
		strcpy (var, pch);

	pch = strtok (NULL, " ");
	if (pch)
		sscanf (pch, "%f", &val);

	if (!strcmp (var, "wew lad"))
	{
		MessageBox (0, "wew lad", "test", 0);
	}
	else if (!strcmp (var, "HP"))
	{
		editHP (unit, val, action);
	}
	else if (!strcmp (var, "HPPercent"))
	{
		editHPPercent (unit, val, action);
	}
	else if (!strcmp (var, "SP"))
	{
		editSP (unit, val, action);
	}
	else if (!strcmp (var, "SPPercent"))
	{
		editSPPercent (unit, val, action);
	}
	else if (!strcmp (var, "ReloadCooldown"))
	{
		editReloadCooldown (unit, val, action);
	}
	else if (!strcmp (var, "ReloadCooldownPercent"))
	{
		editReloadCooldownPercent (unit, val, action);
	}
	else if (!strcmp(var, "Resources"))
	{
		editResources(unit, val, action);
	}
	else if (!strcmp (var, "HPRegen"))
	{
		pch = strtok (NULL, " ");
		if (pch)
			sscanf (pch, "%f", &val2);
		editHPRegen (unit, val, 10*val2);
	}
	else if (!strcmp (var, "HPRegenPercent"))
	{
		pch = strtok (NULL, " ");
		if (pch)
			sscanf (pch, "%f", &val2);
		editHPRegenPercent (unit, val, 10*val2);
	}

	free (s);
}
