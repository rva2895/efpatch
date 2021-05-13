// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "dsoundhook.h"

//TODO: investigate memory leak in DRS loading

#include "resource.h"
#include "versionselect.h"
#include "revertToX1.h"
#include "advtriggereffect.h"
#include "advcheat.h"
#include "population.h"
#include "terrain.h"
#include "resgenbldgs.h"
#include "editpropertyobject.h"
#include "civletter.h"
#include "icons.h"
#include "gamemode.h"
#include "test.h"
#include "negdecay.h"
#include "researchrepeat.h"
#include "techupcolor.h"
#include "chatgaiatoall.h"
#include "textcolor.h"
#include "expldroid.h"
#include "jedimaster.h"
#include "convert.h"
#include "editorres.h"
#include "civunit.h"
#include "civresearch.h"
#include "rescheat.h"
#include "rms.h"
#include "drs.h"
#include "objpanel.h"
#include "conditions.h"
#include "effects.h"
#include "casts.h"
#include "resolution.h"
#include "ai.h"
#include "startupload.h"
#include "hotkeys.h"
#include "zannfort.h"
#include "processunit.h"
#include "votepanel.h"
#include "flare.h"
#include "terrain_gen.h"
#include "filenames.h"
#include "savegamever.h"
#include "triggerdesc.h"
#include "editorenh.h"
#include "mapsize.h"
#include "autosave.h"
#include "editorstatus.h"
#include "cliff.h"
#include "animatedterrain.h"
#include "trigger_unit.h"
#include "scroll.h"
#include "mapcopy.h"
#include "gamespeed.h"
#include "timeline.h"
#include "langdll.h"
#include "cargotrader.h"
#include "minimap.h"
#include "oos.h"
#include "campaign.h"
#include "patrol.h"
#include "attackstance.h"
#include "medic.h"
#include "palette.h"
#include "rec.h"
#include "hotkeyjump.h"
#include "registry.h"
#include "crashreporter.h"
#include "rundll.h"
#include "recbrowse.h"
#include "elevation.h"
#include "network.h"
#include "core.h"
#include "recordrestore.h"
#include "mouseoverride.h"
#include "notify.h"
#include "wndmode.h"
#include "create_object.h"
#include "rms_tokens.h"
#include "hotkey.h"
#include "overlay.h"
#include "menu.h"
#include "localisation.h"
#include "video.h"
#include "storemode.h"
#include "statusscreen.h"
#include "fixedpos.h"
#include "mastervolume.h"
#ifdef TARGET_VOOBLY
#include "iuserpatch.h"
#endif

/*__declspec(naked) int pathFindHook()
{
	__asm
	{
		mov  dword ptr [esp + 4], 0
		mov  dword ptr [esp + 8], 0
		mov  dword ptr [esp + 0Ch], 0
		mov  dword ptr [esp + 10h], 0
		sub  esp, 10h
		_emit   0x8B
		_emit   0x44
		_emit   0x24
		_emit   0x28
		push 004B13A7h
		ret
	}
}

int lineVar;
int retAddr;
int (__stdcall *drawLine) (int x1, int y1, int x2, int y2, int color) =
	(int (__stdcall *) (int, int, int, int, int))0x473EA0;

#pragma warning(push)
#pragma warning(disable:4100)
__declspec(naked) int __stdcall drawLineX(int x1, int y1, int x2, int y2, int color)
{
	__asm
	{
		mov     ecx, lineVar
		pop     retAddr
		call    drawLine
		//drawLine (x1, y1, x2, y2, 0x45);
		push    retAddr
		ret
	}
}
#pragma warning(pop)

__declspec(naked) int flash()
{
	__asm
	{
		mov  ecx, [esi + 20h]
		mov  lineVar, ecx
	}
	drawLineX(200, 200, 300, 300, 0x35);
	drawLineX(200, 200, 400, 300, 0x85);
	//MessageBox (0, "fjdisfjds", "asdijasdjas", 0);
	__asm
	{

		//mov  eax, [esi+17Ch]
		//push 005F9412h //return to normal execution
		pop  edi
		pop  esi
		pop  ebp
		pop  ebx
		ret  10h
	}
}*/

CONFIG_DATA cd;

void getSettings()
{
	regGet(&cd);

	bool key = GetKeyState(VK_SHIFT) & 0x8000;

	if (cd.askAtStartup || key)
	{
		DialogBox(GetModuleHandle("efpatch.dll"), MAKEINTRESOURCE(IDD_DIALOG1_), 0, MainDlgProc);

		regGet(&cd);
	}
}

const float screen_fade = 0.1f; //default 0.001

#pragma optimize( "s", on )
void setHooksCC()
{
	log("Setting EF-independent hooks...");

	//read-only fix for data\*.dat
	writeDword(0x004D5B62, GENERIC_READ);

	//faster screen fade in/out
	//writeDword(0x0042DEA6, (DWORD)&screen_fade);
	//writeDword(0x0042DF5B, (DWORD)&screen_fade);
	
	//renderer fix (THIS_COD)
	BYTE* nops = (BYTE*)malloc(25);
	memset(nops, 0x90, 25);
	writeData(0x0064DC8D, nops, 25);
	writeData(0x0068F14C, "error.txt\0", 10);
	free(nops);

	setTimelineHooks();

	setScrollHooks();
	setMapCopyHooks();

#ifndef TARGET_VOOBLY
	if (cd.windowMode)
		setWndModeHooks();
#endif

#ifndef TARGET_VOOBLY
#ifndef _CHEATDLL_CC
	setAdvCheatHooks();
#endif
#endif

		//Trigger object overflow fix
		//writeByte(0x5F2AF8, 0x65);
		//writeByte(0x5F2AF9, 0x11);
		//writeByte(0x5F2B02, 0x65);
		//writeByte(0x5F2B03, 0x11);
		//writeByte(0x5F2C5F, 0x65);
		//writeByte(0x5F2C60, 0x11);
		//writeByte(0x5F3DB5, 0x65);
		//writeByte(0x5F3DB6, 0x11);
		//writeByte(0x5F3DC8, 0x65);
		//writeByte(0x5F3DC9, 0x11);

	if (cd.useAltCivLetter)
		setAltCivLetter();

	if (cd.unlockResources)
		setResListHooks();

	setTechUpColorHooks();

	setRmsEditorHooks();

#ifndef _CC_COMPATIBLE
	setVotePanelHooks();
	setFlareHooks();
#endif

	setEditorEnhHooks();

#ifndef TARGET_VOOBLY
#ifndef _CHEATDLL_CC
	if (cd.largeMaps)
		setMapSizeHooks();
	else
#endif
#ifndef _CC_COMPATIBLE
#ifndef _CHEATDLL_CC
		setMapSizeHooks_legacy();
#endif
#endif
#else
	setMapSizeHooks_legacy();
#endif

#ifdef _CHEATDLL_CC
#ifndef _CC_COMPATIBLE
	//setTerrainGenHooks();		//Enabled this when using +1 terrain DAT!
	setSaveGameVersionHooks();
#endif
#endif

	setFileNameHooks(cd.gameVersion);

#ifndef _CC_COMPATIBLE

	if (cd.gameVersion)
		setPopulationHooks();

	setResearchRepeatHooks();

	setConditionHooks();
	setEffectHooks();
#endif

	if (cd.editorAutosave)
		setAutosaveHooks(cd.editorAutosaveInterval);

#ifndef TARGET_VOOBLY
	setDRSLoadHooks(cd.gameVersion, cd.widescrnEnabled);
#endif

	setTriggerDescHooks();

#ifndef _CC_COMPATIBLE
	setEditorStatusHooks();
	setTerrainLoadHooks(cd.gameVersion);
#endif

	setGameSpeedHooks();

#ifdef _CHEATDLL_CC
#ifndef _CC_COMPATIBLE
	//setExtraTerrainHooks_CC();		//Enabled this when using +1 terrain DAT!
#endif
#endif

	if (cd.minimap7)
		setMinimapHooks();

	if (cd.largeText)
		writeDword(0x004276F6, 116);

#ifndef TARGET_VOOBLY
	setStartupLoadHooks(cd.gameVersion);
#endif

	setRecHooks();
	setHotkeyJumpHooks();
	//setOOSHooks();

	//setRecBrowseHooks();
	setElevationHooks();

	setNetworkHooks();

	setCoreHooks();
	setRecordRestoreHooks();
	setMouseOverrideHooks();

	setNotifyHooks();

	//disabled ungrouped AI alliance
	writeByte(0x0061E2EB, 0xEB);

	setHotkeyHooks();

	//setOverlayHooks();

	//MP mouse lag
	writeByte(0x0049F686, 0x0C);
	writeWord(0x0049F906, 0x9090);
	writeByte(0x00429541, 0xEB);

	//wndproc loop delay
	writeDword(0x00425EA6, 0x0674023C);
	writeDword(0x00425EAA, 0x013C042C);
	writeByte(0x00425EAE, 0x77);

	//setStatusScreenHooks();

	//setFixedPosHooks();

	if (cd.delinkVolume)
		setMasterVolumeHooks();
}
#pragma optimize( "", on )

__declspec(naked) void sc1Hook()
{
	__asm
	{
		mov     word ptr [ebp + 17b8h], 31h
		lea     eax, [ebp + 17b8h]
		push    5e55ebh
		ret
	}
}

const char efDatabank[] = "stream\\ef_databank%d.mp3";
const char efCiv[] = "stream\\ef_civ%d.mp3";
const char efShadow[] = "data\\shadow_x2.col";
const char efBlendomatic[] = "data\\blendomatic_x2.dat";
const char efICM[] = "data\\view_icm_x2.dat";
const char efMenubk[] = "stream\\ef_menu_skb.mp3";

#pragma optimize( "s", on )
void setHooksEF()
{
	//filename hooks
	writeDword(0x0042E0CE, (DWORD)efShadow);
	writeDword(0x00609CEB, (DWORD)efBlendomatic);
	writeDword(0x00609ABA, (DWORD)efICM);
	//setHook ((void*)0x005E55DB, &sc1Hook);

	log("setHooksEF() started");

	setLangDllHooks();

	setExtraTerrainHooks();															//!!!

	//setHook ((void*)0x004B13A0, &pathFindHook);

	//setHook ((void*)0x004C1850, &prefabRes1);
	//setHook ((void*)0x005F9796, &flash);

	//setHook ((void*)0x005F395D, &origPatrolEnding);

	fixIconLoadingRoutines();
	fixCivLetterFunction();

	initExplDroid();
	setJediMasterHooks();
	setConvertHooks();//  !!

	setResGenHooks();

	setDecayHooks();

	//writeByte (0x00444A10, 0xE8); //remove 1.0f check in getDamage function
	//writeByte (0x00444A11, 0x13);

	setGameModeHooks();

	setChatGaiaToAllHooks();

	setTextColorHooks();

	setObjectPanelHooks();

	initCivUnitTable();
	initCivResearchTable();

	setResCheatHooks();

	setCastHooks();

	setAIHooks();

	setHotkeysHooks();

	setZannFortHooks();

	setProcessUnitHooks();

	//setTerrainAmount ();

	setCliffTypeHooks();

	setRmsTokenHooks();
	setRmsCreateObjectHooks();

	setPatrolHooks();
	setAttackStanceHooks();
	setMedicHooks();

	//setTriggerUnitHooks();

	//disabled units crash
	writeByte(0x00539793, 0x02); //esp
	writeByte(0x005397B8, 0x02);
	writeByte(0x005399EE, 0x02);
	writeByte(0x00539C50, 0x02);
	writeByte(0x00539E74, 0x02);
	writeByte(0x005397FE, 0x02); //arg_0
	writeByte(0x00539914, 0x02); //arg_4
	writeByte(0x005399AD, 0x02);
	writeByte(0x00539B56, 0x02);
	writeByte(0x00539C2C, 0x02);
	writeByte(0x00539D71, 0x02);
	writeByte(0x00539E50, 0x02);
	writeByte(0x00539AE0, 0x88); //Dest
	writeByte(0x00539B00, 0x84);
	writeByte(0x005397E1, CIV_COUNT + 1);
	writeByte(0x005397D0, 0xE0);

	//if (!cd.animatedWater)
	//	setAnimatedTerrainHooks();

	setCargoTraderHooks();

	writeDword(0x005174AF, (DWORD)efCiv);
	writeDword(0x0051B2CC, (DWORD)efCiv);
	writeDword(0x0050A37C, (DWORD)efDatabank);
	writeDword(0x0050DAE9, (DWORD)efMenubk);

	//setCampaignHooks();

	setPaletteHooks();

	setMenuHooks();

	//setVideoHooks();
	setStoreModeHooks();

	log("setHooks() finished");
}
#pragma optimize( "", on )

const char verStr2[] = "1.2e";
const char verStr3[] = "1.3e";
const char verStr4[] = "1.4e";
const char verStr5[] = "1.5e";
const char verStr6[] = "1.6e";
const char verStr7[] = "1.7e";
const char verStr8[] = "1.8e";
const char verStr9[] = "1.9e";
const char ver1x[] = "1.X";

const char verCC2[] = "1.2";
const char verCC3[] = "1.3";
const char verCC4[] = "1.4";
const char verCC5[] = "1.5";

__declspec(naked) void verHookEF() //0042C3E1
{
	__asm
	{
		dec     eax
		jz      _1_2e
		dec     eax
		jz      _1_3e
		dec     eax
		jz      _1_4e
		dec     eax
		jz      _1_5e
		dec     eax
		jz      _1_6e
		dec     eax
		jz      _1_7e
		dec     eax
		jz      _1_8e
		dec     eax
		jz      _1_9e
		mov     eax, 00689BA8h
		ret     4
_1_2e:
		mov     eax, offset verStr2
		ret     4
_1_3e:
		mov     eax, offset verStr3
		ret     4
_1_4e:
		mov     eax, offset verStr4
		ret     4
_1_5e:
		mov     eax, offset verStr5
		ret     4
_1_6e:
		mov     eax, offset verStr6
		ret     4
_1_7e:
		mov     eax, offset verStr7
		ret     4
_1_8e:
		mov     eax, offset verStr8
		ret     4
_1_9e:
		mov     eax, offset verStr9
		ret     4
	}
}

__declspec(naked) void verHookCC() //0042C3E1
{
	__asm
	{
		dec     eax
		jz      _1_2
		dec     eax
		jz      _1_3
		dec     eax
		jz      _1_4
		dec     eax
		jz      _1_5
		mov     eax, 00689BA8h
		ret     4
_1_2:
		mov     eax, offset verCC2
		ret     4
_1_3:
		mov     eax, offset verCC3
		ret     4
_1_4:
		mov     eax, offset verCC4
		ret     4
_1_5:
		mov     eax, offset verCC5
		ret     4
	}
}

void updateVersionEF()
{
	writeByte(0x00689534, 8); //EF 1.7e
	//strcpy ((char*)0x00689BA4, verStr);
	setHook((void*)0x0042C3E1, &verHookEF);
}

void updateVersionCC()
{
#ifndef TARGET_VOOBLY
#ifndef _CC_COMPATIBLE
	writeByte(0x00689534, 3); //CC 1.2
	//strcpy ((char*)0x00689BA4, verStr);
	setHook((void*)0x0042C3E1, &verHookCC);
#endif // !_CC_COMPATIBLE
#endif
}

void fixCurrentDir()
{
	char fname[256];
	GetModuleFileName(0, fname, 255);
	char* p = fname + strlen(fname);
	while (*--p != '\\');
	*p = 0;
	SetCurrentDirectory(fname);
}

void* new_memory_pages;

#ifdef TARGET_VOOBLY
extern bool expanding_fronts;
#endif

void initialSetup()
{
#ifdef TARGET_VOOBLY
	initLog();
	log("===============================================");
	log("Dll attached");
	log("===============================================");

	log("Compile time: " __DATE__ ", " __TIME__);
	log("Configuration: Release_Voobly");

	log("Notice: running in Voobly mode");
#endif

	new_memory_pages = VirtualAlloc(0, 0x1000, MEM_COMMIT, PAGE_READWRITE);

	getSettings();

	//install_language(cd.lang);

#ifdef TARGET_VOOBLY
	cd.gameVersion = expanding_fronts;
#endif

	log("Settings (1/4): fps = %d, ds = %d, b = %d, to = %d, v = %d, a = %d",
		cd.useFPS, cd.useDShook, cd.nBufs, cd.timeout, cd.gameVersion, cd.askAtStartup);
	log("Settings (2/4): civl = %d, res = %d, auto = %d, autoint = %d",
		cd.useAltCivLetter, cd.unlockResources, cd.editorAutosave, cd.editorAutosaveInterval);
	log("Settings (3/4): wide = %d, wx = %d, wy = %d, wnd = %d",
		cd.widescrnEnabled, cd.xres, cd.yres, cd.windowMode);
	log("Settings (4/4): large = %d, crash = %d",
		cd.largeMaps, cd.crashReporting);

	setTestHook();

#ifndef _CHEATDLL_CC
	switch (cd.gameVersion)
	{
	case VER_CC:
		revertToX1();     //for EF
		setHooksCC();

		updateVersionCC();
		break;
	case VER_EF:
		//revertToX1 ();               //remove!!!! <---
		//writeByte(0x289BA4, 0x32); //what is this???
		setHooksCC();
		setHooksEF();

		updateVersionEF();
		break;
	default:
		break;
	}
#else
	cd.gameVersion = VER_CC;
	revertToX1();     //for EF
	setHooksCC();

	updateVersionCC();
#endif

#ifndef TARGET_VOOBLY
	if (cd.widescrnEnabled)
		resolutionTool(cd.xres, cd.yres);
#endif

/*#ifndef _CHEATDLL_CC
	if (cd.useDShook)
		initDsoundhook();
#endif*/

#ifndef TARGET_VOOBLY
	if (!cd.useFPS)
		writeByte(0x0061E92C, 0x20);
	else
	{
		//unsigned long interval = 100;
		writeDword(0x005DDBA4, 100);
		writeDword(0x005DDB7B, 100);
		writeWord(0x005DDB73, 0x9090);
	}
#else
	//fps and UI bar
	writeByte(0x0061E92C, 0x10);
	writeDword(0x005DDBA4, 100);
	writeDword(0x005DDB7B, 100);
	writeWord(0x005DDB73, 0x9090);
#endif

	writeDword(0x004CCAD0, 15000);

	log("Initial setup complete, returning");

	//FlushInstructionCache(GetCurrentProcess(), NULL, NULL);
}

#ifndef TARGET_VOOBLY
BOOL __stdcall DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	UNREFERENCED_PARAMETER(lpReserved);
	UNREFERENCED_PARAMETER(hModule);
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
//#ifndef _CHEATDLL_CC
		fixCurrentDir();   //for EF
//#endif

		initLog();
		log("===============================================");
		log("Dll attached");
		log("===============================================");

		log("Compile time: " __DATE__ ", " __TIME__);
#ifdef _DEBUG
#ifdef _CHEATDLL_CC
		log("Configuration: DEBUG_CC");
#else
		log("Configuration: DEBUG");
#endif
#else
#ifdef _CHEATDLL_CC
		log("Configuration: RELEASE_CC");
#else
		log("Configuration: RELEASE");
#endif
#endif

		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		log("===============================================");
		log("DLL detached");
		log("===============================================");
		closeLog();

		break;
	default:
		break;
	}
	return TRUE;
}
#endif