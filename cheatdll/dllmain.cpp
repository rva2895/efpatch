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
#include "registry.h"
#include "crashreporter.h"
#include "rundll.h"

__declspec(naked) int pathFindHook()
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
}

int retPntPnt;
int unitPtrPtr;
int counter;
int nextUnitPtrPtr;

__declspec(naked) int patrol ()
{
	__asm
	{
		mov     eax, retPnt
		mov     retPntPnt, eax
		mov     eax, [esp+10h] //counter
		test    eax, eax
		jz      zeroUnits
		mov     counter, eax
		/*push   ebx
		push   edi
		push   edx
	}
	log ("Executing patrol trigger effect, unit count: %d", counter);
	__asm
	{
		pop     edx
		pop     edi
		pop     ebx*/
		xor     esi, esi
		cmp     eax, ebx
		jle     defCase //jumptable 005F2B53 default case
		lea     ecx, [esp+134h] //unitPtr
		mov     unitPtrPtr, ecx
		mov     nextUnitPtrPtr, ecx
continute:
		lea     ecx, [edi+1Ch] //force field Set Objects...
		mov     dword ptr [ecx], 1
		/*push   ebx
		push   edi
		push   edx
	}
	log ("Patroling unit 0x%X", *(int*)unitPtrPtr);
	__asm
	{
		pop     edx
		pop     edi
		pop     ebx*/
		
		push    0x5F3926
		ret
retPnt:
		mov     eax, nextUnitPtrPtr
		add     eax, 4
		mov     nextUnitPtrPtr, eax
		mov     eax, [eax]
		mov     ecx, unitPtrPtr
		mov     [ecx], eax

		//mov     eax, unitPtrPtr
		//mov     eax, [eax]
		//mov     eax, [ebx]
		//mov     ecx, unitPtrPtr
		//mov     [ecx], eax
		jmp     continute
zeroUnits:
		push    0x5F3DB1
		ret

defCase:
		push    0x5F3DB1
		ret
	}
}

void (__stdcall *patrolFunc) () = (void (__stdcall *)())0x0044EB40;

__declspec(naked) int origPatrolEnding ()
{
	__asm
	{
		call    patrolFunc
		mov     eax, counter
		dec     eax
		jz      retrn
		cmp     eax, -1
		jz      retrn
		mov     counter, eax
		//
		//mov     eax, [esp+10h]
		//inc     esi
		//add     ebx, 4
		//cmp     esi, eax
		//jnl     retrn
		mov     eax, retPntPnt
		push    eax
		ret
retrn:
		push    005F3DB1h
		ret
	}
}

CONFIG_DATA cd;

void getSettings()
{
	regGet(&cd);

	bool key = GetAsyncKeyState(VK_SHIFT);

	if (cd.askAtStartup || key)
	{
		DialogBox(GetModuleHandle("efpatch.dll"), MAKEINTRESOURCE(IDD_DIALOG1_), 0, MainDlgProc);

		regGet(&cd);
	}
}

//int cntr = 0;

void __stdcall fixCur(HWND hWnd, LPRECT rect)
{
	GetClientRect(hWnd, rect);
	SetCursor(0);
	//cntr++;
	//if (!(cntr % 1000))
	//	MessageBox (hWnd, "1000 calls", "wow thats a lot", 0);
}

__declspec(naked) void wndtmp() //00616C0F
{
	__asm
	{
		push    00616C15h
		jmp     fixCur
	}
}

void setHooksCC()
{
	log("Setting EF-independent hooks...");

	DWORD wt;
	WriteProcessMemory(GetCurrentProcess(), (void*)0x0068F14C, "error.txt\0", 10, &wt);

	setTimelineHooks();

	setScrollHooks();
	setMapCopyHooks();

	if (cd.windowMode)
	{
		if (LoadLibrary("wndmode.dll"))
			log("Loaded wndmode.dll");
		else
			log("No wndmode.dll. Using ddraw window mode if possible");

		setHook((void*)0x00616C0F, &wndtmp);

		setByte(0x0047166E, 0x90);
		setByte(0x0047166F, 0x90);
		setInt(0x00471670, 0x90909090);
		setInt(0x00471674, 0x90909090);
		setInt(0x00471678, 0x90909090);
		setInt(0x0047167C, 0x90909090);
		setByte(0x00471680, 0x90);
		setByte(0x00471681, 0x90);
		//}
		//else
		//	MessageBox (0, "Cannot open wndmode.dll. Window mode disabled.", "Error",
		//		MB_ICONERROR);
	}

//#ifdef _DEBUG
	setAdvCheatHooks();
//#endif

		//Trigger object overflow fix
		//setByte(0x5F2AF8, 0x65);
		//setByte(0x5F2AF9, 0x11);
		//setByte(0x5F2B02, 0x65);
		//setByte(0x5F2B03, 0x11);
		//setByte(0x5F2C5F, 0x65);
		//setByte(0x5F2C60, 0x11);
		//setByte(0x5F3DB5, 0x65);
		//setByte(0x5F3DB6, 0x11);
		//setByte(0x5F3DC8, 0x65);
		//setByte(0x5F3DC9, 0x11);

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

#ifndef _CHEATDLL_CC
	if (cd.largeMaps)
		setMapSizeHooks();
	else
#endif
#ifndef _CC_COMPATIBLE
		setMapSizeHooks_legacy();
#endif

#ifdef _CHEATDLL_CC
#ifndef _CC_COMPATIBLE
	setTerrainGenHooks();
	setSaveGameVersionHooks();
#endif
#endif

	setFileNameHooks(cd.gameVersion);

#ifndef _CC_COMPATIBLE
	setPopulationHooks();

	setResearchRepeatHooks();

	setConditionHooks();
	setEffectHooks();
#endif

	if (cd.editorAutosave)
		setAutosaveHooks(cd.editorAutosaveInterval);

	setDRSLoadHooks(cd.gameVersion, cd.widescrnEnabled);

	setTriggerDescHooks();

#ifndef _CC_COMPATIBLE
	setEditorStatusHooks();
	setTerrainLoadHooks(cd.gameVersion);
#endif

	setGameSpeedHooks();
}

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

void setHooksEF()
{
	//setHook ((void*)0x005E55DB, &sc1Hook);

	log("setHooksEF() started");

	setStartupLoadHooks();

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

	//setByte (0x00444A10, 0xE8); //remove 1.0f check in getDamage function
	//setByte (0x00444A11, 0x13);

	setGameModeHooks();

	setChatGaiaToAllHooks();

	setTextColorHooks();

	setObjectPanelHooks();

	initCivUnitTable();
	initCivResearchTable();

	setResCheatHooks();

	setCastHooks();

	setAIHooks();

	setHotkeyHooks();

	setZannFortHooks();

	setProcessUnitHooks();

	//setTerrainAmount ();

	setCliffTypeHooks();

	//setTriggerUnitHooks();

	//disabled units crash
	setByte(0x00539793, 0x02); //esp
	setByte(0x005397B8, 0x02);
	setByte(0x005399EE, 0x02);
	setByte(0x00539C50, 0x02);
	setByte(0x00539E74, 0x02);
	setByte(0x005397FE, 0x02); //arg_0
	setByte(0x00539914, 0x02); //arg_4
	setByte(0x005399AD, 0x02);
	setByte(0x00539B56, 0x02);
	setByte(0x00539C2C, 0x02);
	setByte(0x00539D71, 0x02);
	setByte(0x00539E50, 0x02);
	setByte(0x00539AE0, 0x88); //Dest
	setByte(0x00539B00, 0x84);
	setByte(0x005397E1, CIV_COUNT + 1);
	setByte(0x005397D0, 0xE0);

	if (!cd.animatedWater)
		setAnimatedTerrainHooks();

	log("setHooks() finished");
}

char verStr2[] = "1.2e";
char verStr3[] = "1.3e";
char verStr4[] = "1.4e";
char verStr5[] = "1.5e";
char verStr6[] = "1.6e";
char verStr7[] = "1.7e";
char verStr8[] = "1.8e";
char verStr9[] = "1.9e";
char ver1x[] = "1.X";

char verCC3 [] = "1.3";
char verCC4[] = "1.4";
char verCC5[] = "1.5";

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
		mov     eax, offset verStr9
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
		jz      _1_3
		mov     eax, 00689BA8h
		ret     4
_1_3:
		mov     eax, offset verCC3
		ret     4
	}
}

void updateVersionEF()
{
	setByte(0x00689534, 4); //EF 1.3e
	//strcpy ((char*)0x00689BA4, verStr);
	setHook((void*)0x0042C3E1, &verHookEF);
}

void updateVersionCC()
{
#ifndef _CC_COMPATIBLE
	setByte(0x00689534, 3); //CC 1.3
	//strcpy ((char*)0x00689BA4, verStr);
	setHook((void*)0x0042C3E1, &verHookCC);
#endif // !_CC_COMPATIBLE
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

void initialSetup()
{
	new_memory_pages = VirtualAlloc(0, 0x1000, MEM_COMMIT, PAGE_READWRITE);

	getSettings();

	log("Settings (1/4): fps = %d, ds = %d, b = %d, to = %d, v = %d, a = %d",
		cd.useFPS, cd.useDShook, cd.nBufs, cd.timeout, cd.gameVersion, cd.askAtStartup);
	log("Settings (2/4): civl = %d, res = %d, auto = %d, autoint = %d",
		cd.useAltCivLetter, cd.unlockResources, cd.editorAutosave, cd.editorAutosaveInterval);
	log("Settings (3/4): wide = %d, wx = %d, wy = %d, wnd = %d",
		cd.widescrnEnabled, cd.xres, cd.yres, cd.windowMode);
	log("Settings (4/4): large = %d, aniw = %d",
		cd.largeMaps, cd.animatedWater);

#ifdef _DEBUG
	setTestHook();
#endif // _DEBUG

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
		setByte(0x289BA4, 0x32);
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

	if (cd.widescrnEnabled)
		resolutionTool(cd.xres, cd.yres);

#ifndef _CHEATDLL_CC
	if (cd.useDShook)
		initDsoundhook();
#endif
	if (!cd.useFPS)
	{
		unsigned char fps = 0x20;
		WriteProcessMemory(GetCurrentProcess(), (void*)0x61E92C, &fps, 1, 0);
	}

	log("Initial setup complete, returning");
}

BOOL __stdcall DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
#ifndef _CHEATDLL_CC
		fixCurrentDir();   //for EF
#endif

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
