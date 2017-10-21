#pragma once

struct UNIT_EXTRA
{
	bool    speedReductionEnabled;
	float   speedReductionTime;
	float   speedReductionModifier;

	bool    stealthOffEnabled;
	float   stealthOffTime;

	bool    reloadTimeEnabled;
	float   reloadTimeModifier;
	float   reloadTimeTime;

	bool    hpDrainEnabled;
	float   hpDrainPerSecond;
	float   hpDrainTime;
	float   hpDrainLeftover;

	bool    hpDrainPercentEnabled;
	float   hpDrainPercentPerSecond;
	float   hpDrainPercentTime;
	float   hpDrainPercentLeftover;

	bool    countersUsed;
	float   miscCounter1;
	float   miscCounter2;
	float   miscCounter3;
	float   miscCounter4;
	float   miscCounter5;
};

void setCastHooks ();

UNIT_EXTRA* __fastcall getUnitExtra (void* unit);
void __fastcall addUnitExtra (void* unit, UNIT_EXTRA* ud);

void __fastcall removeUnitExtra(void* unit);

void __stdcall writeSaveFile (int id, void* buffer, int size);
void __stdcall readSaveFile (int id, void* buffer, int size);

void __stdcall specialDamage (void* unit, short type, int damage, int armor);

int __stdcall strcmp_wr (char* s1, char* s2);
