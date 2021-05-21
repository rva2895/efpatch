#pragma once

#include <windows.h>

#ifndef _CHEATDLL_CC
#ifdef TARGET_VOOBLY
#define REGPATH "Software\\Voobly\\Voobly\\game\\16\\EXE Patch"
#else
#define REGPATH "Software\\LucasArts Entertainment Company LLC\\Star Wars Galactic Battlegrounds: Expanding Fronts\\Configurator"
#endif
#else
#define REGPATH "Software\\LucasArts Entertainment Company LLC\\Star Wars Galactic Battlegrounds: Clone Campaigns\\Configurator"
#endif

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
    int crashReporting;
    int gridTerrain;
    int smallTrees;
    int minimap7;
    int largeText;
    int delinkVolume;
    int keydown;
    //std::string lang;
};

void regGet(CONFIG_DATA*);
void regSet(const CONFIG_DATA*);
