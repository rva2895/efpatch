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
#include "mbs.h"
#include "languagedlloverride.h"
#include "worlddump.h"
#include "functionlist.h"
#include "tribe_command.h"
#include "techtree.h"
#include "dataload.h"
#ifdef TARGET_VOOBLY
#include "legacypatch.h"
#include "iuserpatch.h"
#endif

CONFIG_DATA cd;

void getSettings()
{
    regGet(&cd);
#ifndef TARGET_VOOBLY
    bool key = GetKeyState(VK_SHIFT) & 0x8000;

    if (cd.askAtStartup || key)
    {
        DialogBox(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDD_DIALOG_STARTUP), 0, MainDlgProc);

        regGet(&cd);
    }
#endif
}

//const float screen_fade = 0.001f; //default 0.001

bool __stdcall check_panel_name(const char* n)
{
    return (!strcmp(n, "Blank Screen"));
}

__declspec(naked) void on_set_panel() //004B4960
{
    __asm
    {
        mov     eax, [esp + 4]
        push    ecx
        push    eax
        call    check_panel_name
        test    al, al
        pop     ecx
        jnz     blank_panel
        mov     eax, [esp + 4]
        push    ebx
        xor     ebx, ebx
        mov     edx, 004B4967h
        jmp     edx
blank_panel:
        ret     8
    }
}

__declspec(naked) void annex_unit_crash_mitigation() //00555640
{
    __asm
    {
        mov     ecx, [esi + 1Ch]
        test    ecx, ecx
        jz      mitigate_annex_crash
        movsx   eax, ax
        mov     edx, 00555646h
        jmp     edx
mitigate_annex_crash:
        mov     edx, 005556C9h
        jmp     edx
    }
}

#pragma optimize( "s", on )
void setHooksCC()
{
    log("Setting EF-independent hooks...");

    //read-only fix for data\*.dat
    writeDword(0x004D5B62, GENERIC_READ);

    //faster screen fade in/out
    //writeDword(0x0042DEA6, (DWORD)&screen_fade);
    //writeDword(0x0042DF5B, (DWORD)&screen_fade);
    //writeData(0x0042DDA0, "\xC2\x14\x00", 3);

    //remove blank screen
    //setHook((void*)0x004B4960, on_set_panel);
    
    //renderer fix (THIS_COD)
    BYTE nops[25];
    memset(nops, 0x90, 25);
    writeData(0x0064DC8D, nops, 25);
    writeData(0x0068F14C, "error.txt", 10);

    //workaround for annex unit crash from state 0
    setHook((void*)0x00555640, annex_unit_crash_mitigation);

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

    setFileNameHooks(cd.gameVersion);

#ifndef _CC_COMPATIBLE
    setPopulationHooks(cd.gameVersion);
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
    //setExtraTerrainHooks_CC();        //Enabled this when using +1 terrain DAT!
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

#ifdef TARGET_VOOBLY
    setRecBrowseHooks(cd.gameVersion);
#endif
    setElevationHooks();

    setNetworkHooks();

    setCoreHooks();
    setRecordRestoreHooks();
    setMouseOverrideHooks();

    setNotifyHooks();

    //disabled ungrouped AI alliance
    writeByte(0x0061E2EB, 0xEB);

    if (cd.keydown)
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

    setMbsHooks();

    //one player in MP
    writeByte(0x005154D9, 0xEB);
    writeByte(0x00519A06, 0xEB);

    setHotkeysHooks(cd.gameVersion);
    setLanguageDllOverrideHooks();

    setAIHooks(cd.gameVersion);

    setObjectPanelHooks(cd.gameVersion);

    setTerrainGenHooks(cd.gameVersion);

    setTribeCommandHooks(cd.gameVersion);

    setResearchRepeatHooks();
    setConditionHooks();
    setEffectHooks();

    //setWorldDumpHooks();

    //from old patch:
    //full map print ratios
    writeByte(0x0045D4BD, 8);
    writeByte(0x0045D4CA, 1);
    writeByte(0x0045D4DC, 0xFF);

    //post game view for sp
    writeByte(0x004F87AD, 0x4C);
    writeByte(0x004F89CE, 0xEB);
    writeByte(0x004F89CF, 0x1F);
    writeByte(0x004F89E1, 0xEB);
    writeByte(0x004F8A13, 0x66);
    writeByte(0x004F8A14, 0x90);

    //remove high graphics fambaa ring
    writeByte(0x0061F4A4, 0xEB);

    //function hook!
    //setFunctionListHooks();
}
#pragma optimize( "", on )

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

    log("setHooksEF() started");

    setLangDllHooks();

    setExtraTerrainHooks();

    fixIconLoadingRoutines();
    fixCivLetterFunction();

    initExplDroid("data\\", "expl.txt");
    setJediMasterHooks("data\\", "master.txt", "padawan.txt");
    setConvertHooks("data\\", "unconv.txt");

    setResGenHooks();

    setDecayHooks();

    //writeByte (0x00444A10, 0xE8); //remove 1.0f check in getDamage function
    //writeByte (0x00444A11, 0x13);

    setGameModeHooks();

    setChatGaiaToAllHooks();

    setTextColorHooks();

    initCivUnitTable();
    initCivResearchTable();

    setResCheatHooks();

    setCastHooks();

    setZannFortHooks();

    setProcessUnitHooks();

    //setTerrainAmount();

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
    //    setAnimatedTerrainHooks();

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

    setTechTreeHooks();
    setDataLoadHooks();

    log("setHooks() finished");
}
#pragma optimize( "", on )

const char ver1x[] = "1.X";

const char verCC2[] = "1.2";
const char verCC3[] = "1.3";
const char verCC4[] = "1.4";
const char verCC5[] = "1.5";

char version_string[8];

char* __stdcall get_version_str_ef(BYTE v)
{
    BYTE v_minor;
    BYTE v_major;
    if (v == 127)
    {
        strcpy(version_string, ver1x);
    }
    else if (v > 8)
    {
        v_minor = (v + 29 - 9) % 7;
        v_major = (v + 29 - 9) / 7;
        sprintf_s(version_string, _countof(version_string), "1.%hhu.%hhu", v_major, v_minor);
    }
    else if (v > 0)
    {
        v_major = v - 1;
        sprintf_s(version_string, _countof(version_string), "1.%hhue", v_major);
    }
    else
    {
        version_string[0] = '\0';
    }
    return version_string;
}

__declspec(naked) void verHookEF_v2() //0042C3D1
{
    __asm
    {
        push    eax
        call    get_version_str_ef
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
    //writeByte(0x00689534, 8); //EF 1.7e
    writeByte(0x00689534, 9); //EF 1.4.1, new format
    setHook((void*)0x0042C3D1, verHookEF_v2);
}

void updateVersionCC()
{
#ifndef TARGET_VOOBLY
#ifndef _CC_COMPATIBLE
    writeByte(0x00689534, 3); //CC 1.2
    //strcpy ((char*)0x00689BA4, verStr);
    setHook((void*)0x0042C3E1, verHookCC);
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

    install_legacy_patch();
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
    log("Settings (3/4): wide = %d, wx = %d, wy = %d, wnd = %d, grey = %d",
        cd.widescrnEnabled, cd.xres, cd.yres, cd.windowMode, cd.minimap7);
    log("Settings (4/4): large = %d, crash = %d, keydown = %d, delink = %d",
        cd.largeMaps, cd.crashReporting, cd.keydown, cd.delinkVolume);

    setTestHook();

#ifndef _CHEATDLL_CC
    switch (cd.gameVersion)
    {
    case VER_CC:
#ifndef TARGET_VOOBLY
        revertToX1();     //for EF
#endif
        setHooksCC();

        //updateVersionCC();
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

    //chat display time
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
