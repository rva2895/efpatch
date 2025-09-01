#include "stdafx.h"
#include "iuserpatch.h"

#include "savegamever.h"
#include "rundll.h"
#include "resolution.h"
#include "registry.h"
#include "memory.h"
#include "textrender.h"
#include "chatcommand.h"
#ifdef VOOBLY_EF
#include "palette.h"
#include "setup.h"
#endif

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

__declspec(naked) void onVooblyWidescreenBug1() //004A940B
{
    __asm
    {
        and     eax, 3F00h
        cmp     eax, 500h
        push    004A9415h
        ret
    }
}

__declspec(naked) void onVooblyWidescreenBug2() //004BE13F
{
    __asm
    {
        and     edx, 3F00h
        cmp     edx, 500h
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

/*
__declspec(naked) void onVooblyWidescreenBug5() //00502881
{
    __asm
    {
        mov     eax, 004B7450h
        call    eax
        mov     eax, [esp + 1Ch]
        //
        mov     eax, 1280
        mov     ecx, [esp + 20h]
        mov     ecx, 1200
        cmp     eax, 1280
        jge     rec_ui_large
        cmp     eax, 1024
        jge     rec_ui_medium
        mov     edi, 4
        //lea     ebx, [ecx - 166]
        mov     ebx, 24Bh

rec_ui_finish:
        mov     eax, 00502887h
        mov     ecx, [esi + 1188h]
        jmp     eax

rec_ui_large:
        mov     edi, 18h
        //lea     ebx, [ecx - 324]
        mov     ebx, 320h
        jmp     rec_ui_finish

rec_ui_medium:
        mov     edi, 0Fh
        //lea     ebx, [ecx - 181]
        mov     ebx, 1B2h
        jmp     rec_ui_finish
    }
}
*/

__declspec(naked) void onVooblyFixTechTree()
{
    __asm
    {
        push    0FFFFFFFFh
        mov     eax, 005068C2h
        jmp     eax
    }
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

#ifdef VOOBLY_EF
const char voobly_ef_data_file[] = DATA_FOLDER_PREFIX_FROM_ROOT"genie_x2.dat";
const char voobly_ef_language_file[] = DATA_FOLDER_PREFIX_FROM_ROOT"..\\language_x2.dll";
#endif

extern CONFIG_DATA cd;

bool isVooblyWidescreenInstalled()
{
    DWORD d;
    ReadProcessMemory(GetCurrentProcess(), (DWORD*)0x005068C0, &d, sizeof(d), NULL);
    return d != 0x2B68FF6A;
}

const char widescrnErrorMsg[] =
    "You have enabled both Voobly resolution tool and \"Enable Built-in Widescreen\" option.\n\n"
#ifdef VOOBLY_EF
    "Please turn off Voobly resolution tool";
#else
    "Please either turn off Voobly resolution tool, or uncheck \"Enable Built-in Widescreen\" option";
#endif

void __stdcall delayed_start_process()
{
    trySetProcessDPIAware();
    initialSetup();

    bool voobly_widescreen_installed = isVooblyWidescreenInstalled();
    if (voobly_widescreen_installed && cd.widescrnEnabled)
    {
        MessageBox(NULL, widescrnErrorMsg, "Error", MB_ICONERROR);
        exit(0);
    }
#ifdef VOOBLY_EF
    if (voobly_widescreen_installed)
    {
        MessageBox(NULL, "You have enabled Voobly resolution tool, which is incompatible with Expanding Fronts mod\n\n"
            "Please turn off Voobly resolution tool, and check \"Enable Built-in Widescreen\" option", "Error", MB_ICONERROR);
        exit(0);
    }
    if (!expanding_fronts)
    {
        MessageBox(NULL, "This patch must be used with the Expanding Fronts mod.\n\n"
            "To play Clone Campaigns, use \"EXE Patch\" instead", "Error", MB_ICONERROR);
        exit(0);
    }

    installPalette();
    writeDword(0x0048F0E5, (DWORD)voobly_ef_data_file);
    writeDword(0x005E40A3, (DWORD)voobly_ef_language_file);

    //copy assets
    do_assets_setup();

    if (cd.widescrnEnabled)
        resolutionTool(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), true, true);
#else
    if (expanding_fronts)
    {
        MessageBox(NULL, "This patch cannot be used with Expanding Fronts mod\n\n"
            "To play Expanding Fronts, use \"EF EXE Patch\" instead", "Error", MB_ICONERROR);
        exit(0);
    }

    if (cd.widescrnEnabled)
        resolutionTool(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), false, true);
#endif

    if (cd.textRendering)
        setTextRenderHooks(cd.gameVersion);

    new_allocator_install();
}

void __stdcall delayed_end_process()
{
    new_allocator_uninstall();
    closeLog();
}

__declspec(naked) void delayed_start_hook() //0048EFD3
{
    __asm
    {
        call    delayed_start_process
        or      ecx, 0FFFFFFFFh
        xor     eax, eax
        mov     ebx, 0048EFD8h
        jmp     ebx
    }
}

__declspec(naked) void delayed_end_hook() //0048F7FC
{
    __asm
    {
        push    eax
        call    delayed_end_process
        pop     eax
        add     esp, 1B88h
        retn    10h
    }
}

bool CUserPatch::Init(struct UserPatchConfig_t &config)
{
    // Write DLL version to Voobly log
#ifdef VOOBLY_EF
    g_pVoobly->Log(EFPATCH_VERSION);
#else
    g_pVoobly->Log(USERPATCH_VERSION);
#endif

    initLog();

    // Write 3.1 exe version string    
#ifndef VOOBLY_EF
    g_pVoobly->Write(0x689BA4, "332E32");
#endif

    g_pVoobly->WriteJump(0x0048EFD3, delayed_start_hook);
    g_pVoobly->WriteJump(0x0048F7FC, delayed_end_hook);

    if (strstr(config.VooblyModDirPath, "Data Patch"))
    {
        g_pVoobly->Log("Data patch is ON");
        dataPatch = true;
        //setTerrainGenHooks();
        setSaveGameVerHooks(true);

        writeByte(0x00557DD6, 53);   //gungan foundation: 53
    }
    else if (strstr(config.VooblyModDirPath, VOOBLY_DATA_MOD_NAME))
    {
        expanding_fronts = true;
        setSaveGameVerHooks(false);
        g_pVoobly->Log("Running in EF mode");

        writeByte(0x00557DD6, 53);   //gungan foundation: 53
    }
    else
    {
        setSaveGameVerHooks(false);
        g_pVoobly->Log("Data patch is OFF");
    }

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
    //g_pVoobly->WriteJump(0x00502848, onVooblyWidescreenBug5);
    //voobly tech tree bug
    writeDword(0x004FA41C, (DWORD)onVooblyFixTechTree - 0x004FA420);
    writeDword(0x004FBB6F, (DWORD)onVooblyFixTechTree - 0x004FBB73);

    //tech tree mirror random fix
    g_pVoobly->WriteJump(0x0051834E, onVooblyMirrorRandomTechTree);

    FlushInstructionCache(GetCurrentProcess(), NULL, 0);

    // Apply patches from bin2cpp tool
    bool bSuccess = true;//ApplyPatchList();

    return bSuccess;
}

bool CUserPatch::OnChatMessage(const char *text)
{
    if (!strcmp(text, "/version"))
    {
#ifdef VOOBLY_EF
        g_pVoobly->ChatMessage("EF EXE Patch", "%s", EFPATCH_VERSION);
#else
        char* str = dataPatch ? "ON" : "OFF";
        g_pVoobly->ChatMessage("EXE Patch", "%s, Data patch: %s", USERPATCH_VERSION, str);
#endif
        return true;
    }
    else
        return check_chat_command(text);
}
#endif
