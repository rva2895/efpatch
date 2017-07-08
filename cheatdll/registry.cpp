#include "stdafx.h"

#include "registry.h"

CONFIG_DATA cd_default =
{
	1,   //fps
	1,   //dsoundhook
	5,   //bufs
	250, //delay
	CC,  //version
	1,   //ask
	0,   //alt civ letter
	0,   //res unlock
	0,   //editor autosave
	300, //editor autosave interval
	0,   //widescrn on
	-1,  //x
	-1,  //y
	0,   //window mode
	0    //large maps
};

void regGet (CONFIG_DATA* cd)
{
	HKEY hKeyCU;
	HKEY hKey;
	unsigned long disposition;
	unsigned long type,size;

	long regResult = RegOpenCurrentUser (KEY_ALL_ACCESS, &hKeyCU);
	regResult = RegCreateKeyEx (
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
	}
	else
	{
	if (regResult == 0)
	{
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

		RegCloseKey (hKey);
		RegCloseKey (hKeyCU);
	}
	else
	{
	
	}
	}

}

void regSet (CONFIG_DATA* cd)
{
	HKEY hKeyCU;
	HKEY hKey;
	unsigned long disposition;
	unsigned long type;

	long regResult = RegOpenCurrentUser (KEY_ALL_ACCESS, &hKeyCU);
	regResult = RegCreateKeyEx (
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

		RegCloseKey (hKey);
		RegCloseKey (hKeyCU);
	}
	else
	{
	
	}

}
