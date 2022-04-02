#include "stdafx.h"
#include "registry.h"

extern const CONFIG_DATA cd_default =
{
    1,   //fps
    0,   //dsoundhook
    5,   //bufs
    250, //delay
#ifdef TARGET_VOOBLY
#ifdef VOOBLY_EF
    VER_EF,
#else
    VER_CC,
#endif
#else
    VER_EF,  //version
#endif
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
#ifdef TARGET_VOOBLY
    0,
#else
    1,   //crash reporting
#endif
    0,   //grid terrain
    0,   //small trees
    0,   //minimap 7
    0,   //large text
    0,   //delink volume
    0,   //keydown hotkeys
    0,   //text rendering
    0,   //mod count
    NULL //mods
    //"en" //lang
};

#ifdef TARGET_VOOBLY
#define CONFIG_KEY_ACCESS KEY_QUERY_VALUE
#else
#define CONFIG_KEY_ACCESS KEY_ALL_ACCESS
#endif

template<class T>
void query_reg_option(HKEY hKey, const char* option_name, T& option, const T& default_option)
{
    unsigned long type;
#ifndef TARGET_VOOBLY
    unsigned long size = sizeof(T);
    if (RegQueryValueEx(
        hKey,
        option_name,
        0,
        &type,
        (BYTE*)&option,
        &size))
        option = default_option;
#else
    char str[0x100];
    unsigned long size = 0x100;
    if (RegQueryValueEx(
        hKey,
        option_name,
        0,
        &type,
        (BYTE*)str,
        &size))
        option = default_option;
    else
    {
        strupr(str);
        if (strstr(str, "TRUE"))
            option = 1;
        else if (strstr(str, "FALSE"))
            option = 0;
        else
            option = default_option;
    }
#endif
}

template<class T>
void set_reg_option(HKEY hKey, const char* option_name, const T& option, unsigned long reg_type)
{
#ifndef TARGET_VOOBLY
    RegSetValueEx(
        hKey,
        option_name,
        0,
        reg_type,
        (BYTE*)&option,
        sizeof(option));
#endif
}

void regGet(CONFIG_DATA* cd)
{
    HKEY hKeyCU;
    HKEY hKey;
    unsigned long disposition;

    long regResult = RegOpenCurrentUser(CONFIG_KEY_ACCESS, &hKeyCU);
    regResult = RegCreateKeyEx(
        hKeyCU,
        REGPATH,
        0,
        0,
        0,
        CONFIG_KEY_ACCESS,
        0,
        &hKey,
        &disposition);
#ifndef TARGET_VOOBLY
    if (disposition == REG_CREATED_NEW_KEY)
    {
        *cd = cd_default;
        RegCloseKey(hKey);
        log("Created new registry key with default settings");
    }
    else
    {
#endif
        if (regResult == 0)
        {
            query_reg_option(hKey, "Enable FPS", cd->useFPS, cd_default.useFPS);
            query_reg_option(hKey, "Enable DSoundhook", cd->useDShook, cd_default.useDShook);
            query_reg_option(hKey, "Number of Buffers", cd->nBufs, cd_default.nBufs);
            query_reg_option(hKey, "Delay", cd->timeout, cd_default.timeout);
            query_reg_option(hKey, "Launch Version", cd->gameVersion, cd_default.gameVersion);
            query_reg_option(hKey, "Ask At Startup", cd->askAtStartup, cd_default.askAtStartup);
            query_reg_option(hKey, "Use Alternative List", cd->useAltCivLetter, cd_default.useAltCivLetter);
            query_reg_option(hKey, "Unlock Resources", cd->unlockResources, cd_default.unlockResources);
            query_reg_option(hKey, "Editor Autosave", cd->editorAutosave, cd_default.editorAutosave);
            query_reg_option(hKey, "Editor Autosave Interval", cd->editorAutosaveInterval, cd_default.editorAutosaveInterval);
#ifdef TARGET_VOOBLY
            query_reg_option(hKey, "Enable Built-in Widescreen", cd->widescrnEnabled, cd_default.widescrnEnabled);
#else
            query_reg_option(hKey, "Resolution Patch Enabled", cd->widescrnEnabled, cd_default.widescrnEnabled);
#endif
            query_reg_option(hKey, "Screen Size X", cd->xres, cd_default.xres);
            query_reg_option(hKey, "Screen Size Y", cd->yres, cd_default.yres);
            query_reg_option(hKey, "Window Mode", cd->windowMode, cd_default.windowMode);
            query_reg_option(hKey, "Large Maps", cd->largeMaps, cd_default.largeMaps);
            query_reg_option(hKey, "Crash Reporting", cd->crashReporting, cd_default.crashReporting);
            query_reg_option(hKey, "Grid Terrain", cd->gridTerrain, cd_default.gridTerrain);
            query_reg_option(hKey, "Small Trees", cd->smallTrees, cd_default.smallTrees);
            query_reg_option(hKey, "Dark Minimap Grey", cd->minimap7, cd_default.minimap7);
            query_reg_option(hKey, "Large Text", cd->largeText, cd_default.largeText);
            query_reg_option(hKey, "Delink System Volume", cd->delinkVolume, cd_default.delinkVolume);
            query_reg_option(hKey, "Keydown Object Hotkeys", cd->keydown, cd_default.keydown);
            query_reg_option(hKey, "Alternative Text Rendering", cd->textRendering, cd_default.textRendering);

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
#ifndef TARGET_VOOBLY
            MessageBox(NULL, "Error: cannot access application registry key. Using default settings", "Error", MB_ICONEXCLAMATION);
#endif
            *cd = cd_default;
            log("Failed to create registry key, using default settings");
        }
#ifndef TARGET_VOOBLY
    }
#endif

    RegCloseKey(hKeyCU);
}

void regSet(const CONFIG_DATA* cd)
{
    HKEY hKeyCU;
    HKEY hKey;
    unsigned long disposition;

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
        set_reg_option(hKey, "Enable FPS", cd->useFPS, REG_DWORD);
        set_reg_option(hKey, "Enable DSoundhook", cd->useDShook, REG_DWORD);
        set_reg_option(hKey, "Number of Buffers", cd->nBufs, REG_DWORD);
        set_reg_option(hKey, "Delay", cd->timeout, REG_DWORD);
        set_reg_option(hKey, "Launch Version", cd->gameVersion, REG_DWORD);
        set_reg_option(hKey, "Ask At Startup", cd->askAtStartup, REG_DWORD);
        set_reg_option(hKey, "Use Alternative List", cd->useAltCivLetter, REG_DWORD);
        set_reg_option(hKey, "Unlock Resources", cd->unlockResources, REG_DWORD);
        set_reg_option(hKey, "Editor Autosave", cd->editorAutosave, REG_DWORD);
        set_reg_option(hKey, "Editor Autosave Interval", cd->editorAutosaveInterval, REG_DWORD);
#ifdef TARGET_VOOBLY
        set_reg_option(hKey, "Enable Built-in Widescreen", cd->widescrnEnabled, REG_DWORD);
#else
        set_reg_option(hKey, "Resolution Patch Enabled", cd->widescrnEnabled, REG_DWORD);
#endif
        set_reg_option(hKey, "Screen Size X", cd->xres, REG_DWORD);
        set_reg_option(hKey, "Screen Size Y", cd->yres, REG_DWORD);
        set_reg_option(hKey, "Window Mode", cd->windowMode, REG_DWORD);
        set_reg_option(hKey, "Large Maps", cd->largeMaps, REG_DWORD);
        set_reg_option(hKey, "Crash Reporting", cd->crashReporting, REG_DWORD);
        set_reg_option(hKey, "Grid Terrain", cd->gridTerrain, REG_DWORD);
        set_reg_option(hKey, "Small Trees", cd->smallTrees, REG_DWORD);
        set_reg_option(hKey, "Dark Minimap Grey", cd->minimap7, REG_DWORD);
        set_reg_option(hKey, "Large Text", cd->largeText, REG_DWORD);
        set_reg_option(hKey, "Delink System Volume", cd->delinkVolume, REG_DWORD);
        set_reg_option(hKey, "Keydown Object Hotkeys", cd->keydown, REG_DWORD);
        set_reg_option(hKey, "Alternative Text Rendering", cd->textRendering, REG_DWORD);
        
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
        MessageBox(NULL, "Error: cannot access application registry key. Settings not saved", "Error", MB_ICONEXCLAMATION);
        log("Failed to create registry key, settings not saved");
    }
    RegCloseKey(hKeyCU);
}
