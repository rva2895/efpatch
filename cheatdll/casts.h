#pragma once

struct UNIT_EXTRA
{
	int     unit;
	void*   vtable;
	int     speedReductionEnabled;
	float   speedReductionTime;
	float   speedReductionModifier;

	int     stealthOffEnabled;
	float   stealthOffTime;

	int     reloadTimeEnabled;
	float   reloadTimeModifier;
	float   reloadTimeTime;

	int     hpDrainEnabled;
	float   hpDrainPerSecond;
	float   hpDrainTime;
	float   hpDrainLeftover;

	int     hpDrainPercentEnabled;
	float   hpDrainPercentPerSecond;
	float   hpDrainPercentTime;
	float   hpDrainPercentLeftover;

	/*int     spawnEnabled;
	int     spawnMinTime;
	int     spawnMaxTime;   //-1 = unlimited
	int     spawnProb;      //0 to RAND_MAX
	float   spawnTime;
	float   spawnLastTime;
	int     spawnID;*/
	//float   spawn
};

void setCastHooks ();

void printMap ();

UNIT_EXTRA* __fastcall getUnitExtra (void* unit);
void __fastcall addUnitExtra (void* unit, UNIT_EXTRA* ud);

void __fastcall removeUnitExtra(void* unit);

void __stdcall writeSaveFile (int id, void* buffer, int size);
void __stdcall readSaveFile (int id, void* buffer, int size);

void __stdcall specialDamage (void* unit, short type, int damage, int armor);

int __stdcall strcmp_wr (char* s1, char* s2);
