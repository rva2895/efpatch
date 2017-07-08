#pragma once

#include <windows.h>

#define REGPATH "Software\\LucasArts Entertainment Company LLC\\Star Wars Galactic Battlegrounds: Expanding Fronts\\Configurator"

#define CC 0
#define EF 1

struct CONFIG_DATA
{
	int useFPS;
	int useDShook;
	int nBufs;
	int timeout;
	int gameVersion;
	int askAtStartup;
	int useAltCivLetter;
	int unlockResources;
	int editorAutosave;
	int editorAutosaveInterval;
	int widescrnEnabled;
	int xres;
	int yres;
	int windowMode;
	int largeMaps;
};

void regGet (CONFIG_DATA*);
void regSet (CONFIG_DATA*);
