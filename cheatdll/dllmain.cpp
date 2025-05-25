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
#include "commandbuttons.h"
#include "rge_communications_speed.h"
#include "tchat.h"
#include "patroldelay.h"
#include "playeroptions.h"
#include "guardandfollow.h"
#include "trade.h"
#include "pathing.h"
#include "resfile.h"
#include "bldg_heal_rate_save.h"
#include "gaiatechtree.h"
#include "los.h"
#include "techtree_ui.h"
#include "tooltip.h"
#include "textrender.h"
#include "miscbugfix.h"
#include "editcontrol.h"
#include "attribute_held.h"
#include "buildingconv.h"
#include "revealmap.h"
#include "spectate.h"
#include "maplist.h"
#include "TCommunications_Handler2.h"
#include "land.h"
#include "script.h"
#include "airtoair.h"
#include "conversion.h"
#include "rmslog.h"
#include "hunt.h"
#include "hideineditor.h"
#include "jediholo.h"
#include "hotfix.h"
#include "shield.h"
#include "harbor.h"
#include "textsize.h"
#include "wallbuild.h"
#include "farm.h"
#include "sleepingobjects.h"
#include "fog.h"
#include "loadfilelist.h"
#include "mpsetupscreen.h"
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

    if (!setStartupLoadHooks() && (cd.askAtStartup || key))
        DialogBox(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDD_DIALOG_STARTUP), 0, VersionSelectDlgProc);
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

#pragma optimize( "s", on )
void setHooksCC()
{
    log("setHooksCC() started");

    setMiscBugfixHooks(cd.gameVersion);

    //faster screen fade in/out
    //writeDword(0x0042DEA6, (DWORD)&screen_fade);
    //writeDword(0x0042DF5B, (DWORD)&screen_fade);
    //writeData(0x0042DDA0, "\xC2\x14\x00", 3);

    //remove blank screen
    //setHook((void*)0x004B4960, on_set_panel);

    setTimelineHooks();

    setScrollHooks();
    setMapCopyHooks();
    
#ifndef TARGET_VOOBLY
    if (cd.windowMode)
        setWndModeHooks();
#endif

    if (cd.useAltCivLetter)
        setAltCivLetter();

    if (cd.unlockResources)
        setResListHooks();

    if (cd.unlockObjects)
        setHideInEditorHooks();

    setTechUpColorHooks();

    setRmsEditorHooks();

    setVotePanelHooks();
    setFlareHooks();

    setEditorEnhHooks(cd.gameVersion);

#ifndef TARGET_VOOBLY
    if (cd.largeMaps && cd.gameVersion == VER_EF)
        setMapSizeHooks(cd.gameVersion);
    else
        setMapSizeHooks_legacy(cd.gameVersion);
#else
    setMapSizeHooks_legacy(cd.gameVersion);
#endif

    setFileNameHooks(cd.gameVersion);

    setPopulationHooks(cd.gameVersion);

    if (cd.editorAutosave)
        setAutosaveHooks(cd.gameVersion, cd.editorAutosaveInterval);

    setDRSLoadHooks(cd.gameVersion, cd.widescrnEnabled);

    setTriggerDescHooks();

    setEditorStatusHooks();
    setTerrainLoadHooks(cd.gameVersion);

    setGameSpeedHooks();

    if (cd.minimap7)
        setMinimapHooks();

    if (!check_dwrite_available())
        cd.textRendering = 0;

    if (cd.largeText)
        setTextSizeHooks(cd.textRendering);
    
    setRecHooks();
    setHotkeyJumpHooks();

    setRecBrowseHooks(cd.gameVersion);

    setElevationHooks();

    setNetworkHooks();

    setCoreHooks();
    setRecordRestoreHooks();
    setMouseOverrideHooks();

    setNotifyHooks();

    if (cd.keydown)
        setHotkeyHooks();

    //setOverlayHooks();

    //setStatusScreenHooks();

    setFixedPosHooks();

    if (cd.delinkVolume)
        setMasterVolumeHooks();

    setMbsHooks();

    setHotkeysHooks(cd.gameVersion);
    setLanguageDllOverrideHooks();

    setAIHooks(cd.gameVersion);

    setObjectPanelHooks(cd.gameVersion);

    setTerrainGenHooks(cd.gameVersion);

    setTribeCommandHooks(cd.gameVersion);

    setResearchRepeatHooks();
    setConditionHooks();
    setEffectHooks();

    setCommandButtonsHooks();
    setGaiaTechTreeHooks();
    setTechTreeUIHooks();
    //setTooltipHooks();

    setLOSHooks();

    //setWorldDumpHooks();

    setRGECommunicationsSpeedHooks();
    setTChatHooks();
    setPatrolDelayHooks();

    setPlayerOptionsHooks();

    setGuardAndFollowHooks();
    //setTradeHooks();
    //setPathingHooks();

    //setResFileHooks();

    setAttributeHeldHooks();

    if (cd.chatBox)
        setEditControlHooks();

    if (cd.fog)
        setFogOfWarHooks(cd.gameVersion);

    setRevealMapHooks();

    setRMSLogHooks();

    //setSpectateHooks();

    //function hook!
    //setFunctionListHooks();

    //setTCommunications_Handler2hooks();

    setWallBuildHooks();

    setLoadFileListHooks(cd.gameVersion);

    //setMPSetupScreenHooks();

    log("setHooksCC() complete");
}
#pragma optimize( "", on )

const char efDatabank[] = "stream\\ef_databank%d.mp3";
const char efCiv[] = "stream\\ef_civ%d.mp3";
const char efShadow[] = DATA_FOLDER_PREFIX_FROM_ROOT"shadow_x2.col";
const char efBlendomatic[] = DATA_FOLDER_PREFIX_FROM_ROOT"blendomatic_x2.dat";
const char efICM[] = DATA_FOLDER_PREFIX_FROM_ROOT"view_icm_x2.dat";
//const char efMenubk[] = "stream\\ef_menu_skb.mp3";
const char efMenubk[] = "stream\\ef_menu_yavin.mp3";

#pragma optimize( "s", on )
void setHooksEF()
{
    log("setHooksEF() started");

    //filename hooks
    writeDword(0x0042E0CE, (DWORD)efShadow);
    writeDword(0x00609CEB, (DWORD)efBlendomatic);
    writeDword(0x00609ABA, (DWORD)efICM);

    //taunt offset
    writeDword(0x00460E9F, 61999);

    //fixed campaigns
    writeByte(0x00690C78, 0x65);
    writeByte(0x00690FD8, 0x65);
    writeByte(0x00691218, 0x65);
    writeByte(0x00690C80, 0x32);
    writeByte(0x00690FD0, 0x32);

    setAdvCheatHooks();

    setLangDllHooks();

    setExtraTerrainHooks();

    fixIconLoadingRoutines();
    fixCivLetterFunction();

    initExplDroid(DATA_FOLDER_PREFIX_FROM_ROOT, "expl.txt");
    setJediMasterHooks(DATA_FOLDER_PREFIX_FROM_ROOT, "master.txt", "padawan.txt");
    setConvertHooks(DATA_FOLDER_PREFIX_FROM_ROOT, "unconv.txt");
    initAirToAir(DATA_FOLDER_PREFIX_FROM_ROOT, "air-to-air.txt");
    setJediHoloHooks(DATA_FOLDER_PREFIX_FROM_ROOT, "jedi-holo.txt");

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

    //sc1 -> sc2
    writeByte(0x00690C96, 0x32);
    writeByte(0x00690CAB, 0x32);
    writeByte(0x00690CBF, 0x32);
    writeByte(0x006915D5, 0x32);
    writeByte(0x00692018, 0x32);
    writeByte(0x00620A6C, 0x32);

    setBldgHealRateSaveHooks();

    setBuildingConvHooks();
    setConversionHooks();

    setMapListHooks();

    setHuntHooks();
    setShieldHooks();
    setHarborHooks();
    setFarmHooks();

    setSleepingObjectsHooks();

    /*
#ifdef TARGET_VOOBLY
#ifdef VOOBLY_EF
    setHotfixHooks();
#endif
#endif
    */

    //setLandRMSHooks();
    //setScriptRMSHooks();

    log("setHooksEF() complete");
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
        strlcpy(version_string, ver1x, _countof(version_string));
    }
    else if (v > 8)
    {
        v_minor = (v + 29 - 9) % 7;
        v_major = (v + 29 - 9) / 7;
        snprintf(version_string, _countof(version_string), "1.%hhu.%hhu", v_major, v_minor);
    }
    else if (v > 0)
    {
        v_major = v - 1;
        snprintf(version_string, _countof(version_string), "1.%hhue", v_major);
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
    writeByte(0x00689534, 20); //EF 1.5.5, new format
    setHook((void*)0x0042C3D1, verHookEF_v2);
}

void updateVersionCC()
{
#ifndef TARGET_VOOBLY
    writeByte(0x00689534, 3); //CC 1.2
    //strcpy ((char*)0x00689BA4, verStr);
    setHook((void*)0x0042C3E1, verHookCC);
#endif
}

void* new_memory_pages;

#ifdef TARGET_VOOBLY
extern bool expanding_fronts;
#endif

const char x1_dat_file[] = DATA_FOLDER_PREFIX_FROM_ROOT"genie_x1_p1.dat";

extern float screen_scale_factor;

void initialSetup()
{
#ifdef TARGET_VOOBLY
    log("Notice: running in Voobly mode");

    install_legacy_patch();
#endif

    new_memory_pages = VirtualAlloc(0, 0x1000, MEM_COMMIT, PAGE_READWRITE);

    getSettings();

    //install_language(cd.lang);
    
    screen_scale_factor = (float)GetDeviceCaps(GetDC(NULL), LOGPIXELSX) / 96;
    log("Set screen scale factor = %f", screen_scale_factor);

#ifdef TARGET_VOOBLY
    cd.gameVersion = expanding_fronts;
#endif

    log("Settings (1/5): fps = %d, ds = %d, b = %d, to = %d, v = %d, a = %d",
        cd.useFPS, cd.useDShook, cd.nBufs, cd.timeout, cd.gameVersion, cd.askAtStartup);
    log("Settings (2/5): civl = %d, res = %d, auto = %d, autoint = %d",
        cd.useAltCivLetter, cd.unlockResources, cd.editorAutosave, cd.editorAutosaveInterval);
    log("Settings (3/5): wide = %d, wx = %d, wy = %d, wauto = %d, wnd = %d, large = %d",
        cd.widescrnEnabled, cd.xres, cd.yres, cd.autoScreenSize, cd.windowMode, cd.largeMaps);
    log("Settings (4/5): grey = %d, largetext = %d, delink = %d, keydown = %d",
        cd.minimap7, cd.largeText, cd.delinkVolume, cd.keydown);
    log("Settings (5/5): textrender = %d, chatbox = %d, fog = %d, obj = %d, crash = %d",
        cd.textRendering, cd.chatBox, cd.fog, cd.unlockObjects, cd.crashReporting);

    setTestHook();

    switch (cd.gameVersion)
    {
    case VER_CC:
#ifndef TARGET_VOOBLY
        revertToX1();     //for EF
#endif
        setHooksCC();

#ifndef TARGET_VOOBLY
        writeDword(0x0048F0E5, (DWORD)x1_dat_file);
#endif

        //updateVersionCC();
        break;
    case VER_EF:
        setHooksCC();
        setHooksEF();

        updateVersionEF();

        break;
    default:
        break;
    }

#ifndef TARGET_VOOBLY
    if (cd.widescrnEnabled)
        resolutionTool(
            cd.autoScreenSize ? GetSystemMetrics(SM_CXSCREEN) : cd.xres,
            cd.autoScreenSize ? GetSystemMetrics(SM_CYSCREEN) : cd.yres,
            cd.gameVersion,
            true);
#endif

    /*
    if (cd.useDShook)
        initDsoundhook();
    */

#ifndef TARGET_VOOBLY
    if (!cd.useFPS)
    {
        writeByte(0x0061E92C, 0x20);

        //map redraw interval
        writeDword(0x004F2F1D, 200);
    }
    else
    {
        writeDword(0x005DDBA4, 100);
        writeDword(0x005DDB7B, 100);
        writeNops(0x005DDB73, 2);

        //map redraw interval
        writeDword(0x004F2F1D, 100);
    }
#else
    //fps and UI bar
    writeByte(0x0061E92C, 0x10);
    writeDword(0x005DDBA4, 100);
    writeDword(0x005DDB7B, 100);
    writeNops(0x005DDB73, 2);

    //map redraw interval
    writeDword(0x004F2F1D, 100);
#endif

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
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    default:
        break;
    }
    return TRUE;
}
#endif
