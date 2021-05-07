#include "stdafx.h"

#include "registry.h"

extern const CONFIG_DATA cd_default =
{
	1,   //fps
	0,   //dsoundhook
	5,   //bufs
	250, //delay
	VER_EF,  //version
	1,   //ask
	0,   //alt civ letter
	0,   //res unlock
	0,   //editor autosave
	300, //editor autosave interval
	0,   //widescrn on
	-1,  //x
	-1,  //y
	0,   //window mode
	0,   //large maps
	1,   //crash reporting
	0,   //grid terrain
	0,   //small trees
	0,   //minimap 7
	0    //large text
	//"en" //lang
};

void regGet(CONFIG_DATA* cd)
{
	HKEY hKeyCU;
	HKEY hKey;
	unsigned long disposition;
	unsigned long type, size;

	long regResult = RegOpenCurrentUser(KEY_ALL_ACCESS, &hKeyCU);
	regResult = RegCreateKeyEx(
		hKeyCU,
		REGPATH,
		0,
		0,
		0,
		KEY_ALL_ACCESS,
		0,
		&hKey,
		&disposition);

	if (disposition == REG_CREATED_NEW_KEY)
	{
		*cd = cd_default;
		RegCloseKey(hKey);
		log("Created new registry key with default settings");
	}
	else
	{
		if (regResult == 0)
		{
			size = sizeof(int);

			if (RegQueryValueEx(
				hKey,
				"Enable FPS",
				0,
				&type,
				(BYTE*)&cd->useFPS,
				&size))
				cd->useFPS = cd_default.useFPS;

			if (RegQueryValueEx(
				hKey,
				"Enable DSoundhook",
				0,
				&type,
				(BYTE*)&cd->useDShook,
				&size))
				cd->useDShook = cd_default.useDShook;

			if (RegQueryValueEx(
				hKey,
				"Number of Buffers",
				0,
				&type,
				(BYTE*)&cd->nBufs,
				&size))
				cd->nBufs = cd_default.nBufs;

			if (RegQueryValueEx(
				hKey,
				"Delay",
				0,
				&type,
				(BYTE*)&cd->timeout,
				&size))
				cd->timeout = cd_default.timeout;

			if (RegQueryValueEx(
				hKey,
				"Launch Version",
				0,
				&type,
				(BYTE*)&cd->gameVersion,
				&size))
				cd->gameVersion = cd_default.gameVersion;

			if (RegQueryValueEx(
				hKey,
				"Ask At Startup",
				0,
				&type,
				(BYTE*)&cd->askAtStartup,
				&size))
				cd->askAtStartup = cd_default.askAtStartup;

			if (RegQueryValueEx(
				hKey,
				"Use Alternative List",
				0,
				&type,
				(BYTE*)&cd->useAltCivLetter,
				&size))
				cd->useAltCivLetter = cd_default.useAltCivLetter;

			if (RegQueryValueEx(
				hKey,
				"Unlock Resources",
				0,
				&type,
				(BYTE*)&cd->unlockResources,
				&size))
				cd->unlockResources = cd_default.unlockResources;

			if (RegQueryValueEx(
				hKey,
				"Editor Autosave",
				0,
				&type,
				(BYTE*)&cd->editorAutosave,
				&size))
				cd->editorAutosave = cd_default.editorAutosave;

			if (RegQueryValueEx(
				hKey,
				"Editor Autosave Interval",
				0,
				&type,
				(BYTE*)&cd->editorAutosaveInterval,
				&size))
				cd->editorAutosaveInterval = cd_default.editorAutosaveInterval;

			if (RegQueryValueEx(
				hKey,
				"Resolution Patch Enabled",
				0,
				&type,
				(BYTE*)&cd->widescrnEnabled,
				&size))
				cd->widescrnEnabled = cd_default.widescrnEnabled;

			if (RegQueryValueEx(
				hKey,
				"Screen Size X",
				0,
				&type,
				(BYTE*)&cd->xres,
				&size))
				cd->xres = cd_default.xres;

			if (RegQueryValueEx(
				hKey,
				"Screen Size Y",
				0,
				&type,
				(BYTE*)&cd->yres,
				&size))
				cd->yres = cd_default.yres;

			if (RegQueryValueEx(
				hKey,
				"Window Mode",
				0,
				&type,
				(BYTE*)&cd->windowMode,
				&size))
				cd->windowMode = cd_default.windowMode;

			if (RegQueryValueEx(
				hKey,
				"Large Maps",
				0,
				&type,
				(BYTE*)&cd->largeMaps,
				&size))
				cd->largeMaps = cd_default.largeMaps;

			if (RegQueryValueEx(
				hKey,
				"Crash Reporting",
				0,
				&type,
				(BYTE*)&cd->crashReporting,
				&size))
				cd->crashReporting = cd_default.crashReporting;

			if (RegQueryValueEx(
				hKey,
				"Grid Terrain",
				0,
				&type,
				(BYTE*)&cd->gridTerrain,
				&size))
				cd->gridTerrain = cd_default.gridTerrain;

			if (RegQueryValueEx(
				hKey,
				"Small Trees",
				0,
				&type,
				(BYTE*)&cd->smallTrees,
				&size))
				cd->smallTrees = cd_default.smallTrees;

			if (RegQueryValueEx(
				hKey,
				"Dark Grey",
				0,
				&type,
				(BYTE*)&cd->minimap7,
				&size))
				cd->minimap7 = cd_default.minimap7;

			if (RegQueryValueEx(
				hKey,
				"Large Text",
				0,
				&type,
				(BYTE*)&cd->largeText,
				&size))
				cd->largeText = cd_default.largeText;

			/*char language[32];
			size = 32;

			if (RegQueryValueEx(
				hKey,
				"Language",
				0,
				&type,
				(BYTE*)language,
				&size))
				cd->lang = cd_default.lang;
			else
			{
				language[31] = 0;
				cd->lang = language;
			}*/
			RegCloseKey(hKey);
			log("Successfully read settings from the registry");
		}
		else
		{
			MessageBox(0, "Error: cannot access application registry key. Using default settings", "Error", MB_ICONEXCLAMATION);
			*cd = cd_default;
			log("Failed to create registry key, using default settings");
		}
	}

	RegCloseKey(hKeyCU);
}

void regSet(const CONFIG_DATA* cd)
{
	HKEY hKeyCU;
	HKEY hKey;
	unsigned long disposition;
	unsigned long type;

	long regResult = RegOpenCurrentUser(KEY_ALL_ACCESS, &hKeyCU);
	regResult = RegCreateKeyEx(
		hKeyCU,
		REGPATH,
		0,
		0,
		0,
		KEY_ALL_ACCESS,
		0,
		&hKey,
		&disposition);

	if (regResult == 0)
	{
		type = REG_DWORD;

		RegSetValueEx(
			hKey,
			"Enable FPS",
			0,
			type,
			(BYTE*)&cd->useFPS,
			sizeof(cd->useFPS));

		RegSetValueEx(
			hKey,
			"Enable DSoundhook",
			0,
			type,
			(BYTE*)&cd->useDShook,
			sizeof(cd->useDShook));

		RegSetValueEx(
			hKey,
			"Number of Buffers",
			0,
			type,
			(BYTE*)&cd->nBufs,
			sizeof(cd->nBufs));

		RegSetValueEx(
			hKey,
			"Delay",
			0,
			type,
			(BYTE*)&cd->timeout,
			sizeof(cd->timeout));

		RegSetValueEx(
			hKey,
			"Launch Version",
			0,
			type,
			(BYTE*)&cd->gameVersion,
			sizeof(cd->gameVersion));

		RegSetValueEx(
			hKey,
			"Ask At Startup",
			0,
			type,
			(BYTE*)&cd->askAtStartup,
			sizeof(cd->askAtStartup));

		RegSetValueEx(
			hKey,
			"Use Alternative List",
			0,
			type,
			(BYTE*)&cd->useAltCivLetter,
			sizeof(cd->useAltCivLetter));

		RegSetValueEx(
			hKey,
			"Unlock Resources",
			0,
			type,
			(BYTE*)&cd->unlockResources,
			sizeof(cd->unlockResources));

		RegSetValueEx(
			hKey,
			"Editor Autosave",
			0,
			type,
			(BYTE*)&cd->editorAutosave,
			sizeof(cd->editorAutosave));

		RegSetValueEx(
			hKey,
			"Editor Autosave Interval",
			0,
			type,
			(BYTE*)&cd->editorAutosaveInterval,
			sizeof(cd->editorAutosaveInterval));

		RegSetValueEx(
			hKey,
			"Resolution Patch Enabled",
			0,
			type,
			(BYTE*)&cd->widescrnEnabled,
			sizeof(cd->widescrnEnabled));

		RegSetValueEx(
			hKey,
			"Screen Size X",
			0,
			type,
			(BYTE*)&cd->xres,
			sizeof(cd->xres));

		RegSetValueEx(
			hKey,
			"Screen Size Y",
			0,
			type,
			(BYTE*)&cd->yres,
			sizeof(cd->yres));

		RegSetValueEx(
			hKey,
			"Window Mode",
			0,
			type,
			(BYTE*)&cd->windowMode,
			sizeof(cd->windowMode));

		RegSetValueEx(
			hKey,
			"Large Maps",
			0,
			type,
			(BYTE*)&cd->largeMaps,
			sizeof(cd->largeMaps));

		RegSetValueEx(
			hKey,
			"Crash Reporting",
			0,
			type,
			(BYTE*)&cd->crashReporting,
			sizeof(cd->crashReporting));

		RegSetValueEx(
			hKey,
			"Grid Terrain",
			0,
			type,
			(BYTE*)&cd->gridTerrain,
			sizeof(cd->gridTerrain));

		RegSetValueEx(
			hKey,
			"Small Trees",
			0,
			type,
			(BYTE*)&cd->smallTrees,
			sizeof(cd->smallTrees));

		RegSetValueEx(
			hKey,
			"Dark Grey",
			0,
			type,
			(BYTE*)&cd->minimap7,
			sizeof(cd->minimap7));

		RegSetValueEx(
			hKey,
			"Large Text",
			0,
			type,
			(BYTE*)&cd->largeText,
			sizeof(cd->largeText));

		/*type = REG_SZ;

		RegSetValueEx(
			hKey,
			"Language",
			0,
			type,
			(BYTE*)cd->lang.c_str(),
			cd->lang.length() + 1);*/

		RegCloseKey(hKey);
		log("Successfully written settings to the registry");
	}
	else
	{
		MessageBox(0, "Error: cannot access application registry key. Settings not saved", "Error", MB_ICONEXCLAMATION);
		log("Failed to create registry key, settings not saved");
	}
	RegCloseKey(hKeyCU);
}
