#include "stdafx.h"

#include "iuserpatch.h"

#include "flare.h"
#include "filenames.h"
#include "terrain_gen.h"
#include "votepanel.h"
#include "timeline.h"
#include "scroll.h"
#include "gamespeed.h"
#include "savegamever.h"
#include "techupcolor.h"
#include "wndproc.h"
#include "rec.h"
#include "recbrowse.h"
#include "elevation.h"
#include "network.h"
#include "hotkeyjump.h"
#include "recordrestore.h"
#include "mouseoverride.h"
#include "rundll.h"

#include "worlddump.h"

#include <time.h>

#ifdef TARGET_VOOBLY

class CUserPatch : public IUserPatch
{
public:
    virtual bool Init(struct UserPatchConfig_t &config);
    virtual bool OnChatMessage(const char *text);

};

IVoobly *g_pVoobly = NULL;
CUserPatch g_UserPatch;

#ifndef INTEGRATED_USERPATCH
extern "C" __declspec(dllexport)
#endif
void GetUPInterface(IUserPatch **ppUserPatch, IVoobly *pVoobly)
{
    g_pVoobly = pVoobly;
    *ppUserPatch = &g_UserPatch;
}

bool dataPatch = false;
bool expanding_fronts = false;

void* voob_log = 0;

int last_t = 0;
int last_x = 0;
int last_y = 0;
int last_obj = 0;

extern void __stdcall pause_game();

void __stdcall bug_test(int reg, int x, int y, UNIT* unit)
{
    int t = time(NULL);
    if (reg == 0x500)
    {
        if (unit)
        {
            if (unit->ordinal != last_obj && t - last_t > 0)
            {
                last_t = t;
                last_obj = unit->ordinal;
                chat("Reached bug condition: obj=%d", unit->ordinal);
                void* player = getCurrentPlayer();
                WorldPlayerBase__unselect_object(player);
                WorldPlayerBase__select_object(player, unit, 1);
                WorldPlayerBase__set_view_loc(player, unit->x, unit->y, 0);
                pause_game();
            }
        }
        else
        {
            if (x != last_x && y != last_y && t - last_t > 0)
            {
                last_t = t;
                last_x = x;
                last_y = y;
                chat("Reached bug condition: x=%d, y=%d", x, y);
                void* player = getCurrentPlayer();
                WorldPlayerBase__unselect_object(player);
                WorldPlayerBase__set_view_loc(player, x, y, 0);
                pause_game();
            }
        }
    }
}

__declspec(naked) void onVooblyWidescreenBug1() //004A940B
{
    __asm
    {
        and     eax, 3F00h
        //
        //push    ecx
        //push    edx
        //push    eax

        //push    0
        //push    ebp
        //push    ebx
        //push    eax
        //call    bug_test
        //pop     eax
        //pop     edx
        //pop     ecx
        //
        cmp     eax, 500h
        //mov     ecx, 004A9415h
        //jmp     ecx
        push    004A9415h
        ret
    }
}

__declspec(naked) void onVooblyWidescreenBug2() //004BE13F
{
    __asm
    {
        and     edx, 3F00h
        //
        //push    ecx
        //push    edx
        //push    eax

        //push    [ebp + 18h]
        //push    0
        //push    0
        //push    edx
        //call    bug_test
        //pop     eax
        //pop     edx
        //pop     ecx
        //
        cmp     edx, 500h
        //mov     eax, 004BE14Bh
        //jmp     eax
        push    004BE14Bh
        ret
    }
}

__declspec(naked) void onVooblyCampaignBug() //005F674F
{
    __asm
    {
        fnstsw  ax
        test    ah, 1
        jz      short loc_5F675D
        push    500h
        mov     edx, 005F6762h
        jmp     edx
loc_5F675D:
        mov     edx, 005F675Dh
        jmp     edx
    }
}

__declspec(naked) void onVooblyWidescreenBug3() //0046A91A
{
    __asm
    {
        jnz     short loc_46A959
        push    0
        add     edx, 39Eh
        push    0046A924h
        ret
loc_46A959:
        push    0046A959h
        ret
    }
}

__declspec(naked) void onVooblyWidescreenBug4() //004290A3
{
    __asm
    {
        mov     eax, [esi]
        push    ebx
        push    edi
        push    400h
        mov     edx, 004290ACh
        jmp     edx
    }
}

__declspec(naked) void onVooblyFixTechTree()
{
    __asm
    {
        push    0FFFFFFFFh
        mov     eax, 005068C2h
        jmp     eax
    }
}

void __stdcall temp_test_(int a)
{
    char s[0x10];
    sprintf(s, "%d", a);
    MessageBox(0, s, "Test", 0);
    exit(0);
}

__declspec(naked) void onVooblyMirrorRandomTechTree() //0051834E
{
    __asm
    {
        cmp     eax, 1Eh
        jz      short fix_civ
        cmp     eax, 1Fh
        jz      short fix_civ
        mov     ecx, 0051835Ch
        jmp     ecx
fix_civ:
        mov     eax, 1
        mov     ecx, 00518358h
        jmp     ecx
    }
}

bool CUserPatch::Init(struct UserPatchConfig_t &config)
{
    // Write DLL version to Voobly log            
    g_pVoobly->Log(USERPATCH_VERSION);

    // Write 3.1 exe version string    
    g_pVoobly->Write(0x689BA4, "332E31");

    if (strstr(config.VooblyModDirPath, "Data Patch"))
    {
        g_pVoobly->Log("Data patch is ON");
        dataPatch = true;
        //setTerrainGenHooks();
        setSaveGameVerHooks(true);
    }
    else if (strstr(config.VooblyModDirPath, "Expanding Fronts"))
    {
        expanding_fronts = true;
        setSaveGameVerHooks(false);
        g_pVoobly->Log("Running in EF mode");
    }
    else
    {
        setSaveGameVerHooks(false);
        g_pVoobly->Log("Data patch is OFF");
    }

    //setTerrainGenHooks();

    // Write 2.2 exe version string    
    /*g_pVoobly->Write(0x689BA4, "322E32");

    setFileNameHooks(false);
    if (strstr(config.VooblyModDirPath, "Data Patch"))
    {
        g_pVoobly->Log("Data patch is ON");
        dataPatch = true;
        setTerrainGenHooks();
        setSaveGameVerHooks(true);
    }
    else
    {
        setSaveGameVerHooks(false);
        g_pVoobly->Log("Data patch is OFF");
    }

    //
    setFlareHooks();
    //

    setVotePanelHooks();
    setGameSpeedHooks();
    setTimelineHooks();
    setScrollHooks();
    setTechUpColorHooks();
    setWndProcHooks();
    setRecHooks();
    setElevationHooks();
    setRecBrowseHooks();
    setNetworkHooks();
    setHotkeyJumpHooks();
    setRecordRestoreHooks();
    setMouseOverrideHooks();

    //setTerrainGenHooks_v2();

    //UI bar update
    g_pVoobly->Write(0x005DDBA4, 100);
    g_pVoobly->Write(0x005DDB73, "9090");
    g_pVoobly->Write(0x005DDB7B, 100);

    //chat display time
    g_pVoobly->Write(0x004CCAD0, 15000);*/

    initialSetup();

    //wndproc for voobly
    writeDword(0x00426509, (DWORD)WndProc_dll);

    //fix voobly widescreen bugs
    /*writeDword(0x004A9411, 0x500);
    writeDword(0x004BE147, 0x500);
    writeDword(0x005F6757, 0x500);*/
    g_pVoobly->WriteJump(0x004A940B, onVooblyWidescreenBug1);
    g_pVoobly->WriteJump(0x004BE13F, onVooblyWidescreenBug2);
    //voobly campaign bug
    g_pVoobly->WriteJump(0x005F674F, onVooblyCampaignBug);
    //voobly various widescreen bugs
    g_pVoobly->WriteJump(0x0046A91A, onVooblyWidescreenBug3);
    g_pVoobly->WriteJump(0x004290A3, onVooblyWidescreenBug4);
    //voobly tech tree bug
    writeDword(0x004FA41C, (DWORD)onVooblyFixTechTree - 0x004FA420);
    writeDword(0x004FBB6F, (DWORD)onVooblyFixTechTree - 0x004FBB73);

    //tech tree mirror random fix
    g_pVoobly->WriteJump(0x0051834E, onVooblyMirrorRandomTechTree);

    // Apply patches from bin2cpp tool
    bool bSuccess = true;//ApplyPatchList();

    return bSuccess;
}

extern int max_worldtime;
extern unsigned int dump_objects(const char* filename);

extern int __fastcall get_gametime2();

bool CUserPatch::OnChatMessage(const char *text)
{
    if (!strcmp(text, "/upversion"))
    {
        char* str = dataPatch ? "ON" : "OFF";
        g_pVoobly->ChatMessage("UserPatch", "%s, Data patch: %s", USERPATCH_VERSION, str);
        return true;
    }
    /*
    if (!strcmp(text, "/dump-world"))
    {
        srand(time(0));
        unsigned int r = rand();
        char name[MAX_PATH];
        sprintf(name, "rge_dump_%08X.txt", r);
        chat("Dumping world to %s ...", name);
        dump_objects(name);
        chat("Dump complete");
        return true;
    }
    if (strstr(text, "/set-max"))
    {
        char d[0x100];
        int t;
        sscanf(text, "%s %d", d, &t);
        max_worldtime = t;
        chat("Set max worldtime to %d", t);
        return true;
    }
    if (!strcmp(text, "/worldtime"))
    {
        chat("Worldtime = %d", get_gametime2());
        return 1;
    }
    if (strstr(text, "/obj") || strstr(text, "/object"))
    {
        char d[0x100];
        int id;
        sscanf(text, "%s %d", d, &id);
        void* base_world = *(void**)((char*)*base_game + 0x420);
        if (base_world)
        {
            UNIT* unit = (UNIT*)BaseWorld__object(base_world, id);
            if (unit)
            {
                void* player = getCurrentPlayer();
                WorldPlayerBase__unselect_object(player);
                WorldPlayerBase__select_object(player, unit, 1);
            }
            else
                chat("Invalid object id");
        }

        return true;
    }
    if (strstr(text, "/goto"))
    {
        char d[0x100];
        float x, y;
        sscanf(text, "%s %f %f", d, &x, &y);
        void* player = getCurrentPlayer();
        WorldPlayerBase__set_view_loc(player, x, y, 0);
        return true;
    }
    if (strstr(text, "/cs"))
    {
        WORLD_DUMP wd;
        wd.update_cs();
        chat("CS=%u", wd.get_cs());
        return true;
    }*/

    return false;
}
#endif
