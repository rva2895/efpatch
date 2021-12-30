#include "stdafx.h"

#include "common.h"

void** base_game = (void**)0x006A3684;
void** panel_system = (void**)0x006ADBB8;
int* world_update_counter = (int*)0x007A22F8;

#ifdef TARGET_VOOBLY
extern IVoobly *g_pVoobly;
#endif

void __cdecl writeByte(DWORD addr, BYTE val)
{
#ifndef TARGET_VOOBLY
    WriteProcessMemory(GetCurrentProcess(), (void*)addr, &val, 1, NULL);
#else
    g_pVoobly->Write(addr, &val, sizeof(BYTE));
#endif
}

void __cdecl writeWord(DWORD addr, WORD val)
{
#ifndef TARGET_VOOBLY
    WriteProcessMemory(GetCurrentProcess(), (void*)addr, &val, 2, NULL);
#else
    g_pVoobly->Write(addr, &val, sizeof(WORD));
#endif
}

void __cdecl writeDword(DWORD addr, DWORD val)
{
#ifndef TARGET_VOOBLY
    WriteProcessMemory(GetCurrentProcess(), (void*)addr, &val, 4, NULL);
#else
    g_pVoobly->Write(addr, val);
#endif
}

void __cdecl writeByteF(DWORD addr, BYTE val)
{
#ifndef TARGET_VOOBLY
    WriteProcessMemory(GetCurrentProcess(), (void*)(addr + 0x400000), &val, 1, NULL);
#else
    g_pVoobly->Write(addr + 0x400000, &val, sizeof(BYTE));
#endif
}

void __cdecl writeDwordF(DWORD addr, DWORD val)
{
#ifndef TARGET_VOOBLY
    WriteProcessMemory(GetCurrentProcess(), (void*)(addr + 0x400000), &val, 4, NULL);
#else
    g_pVoobly->Write(addr + 0x400000, val);
#endif
}

void __cdecl writeData(DWORD addr, const void* data, int len)
{
#ifndef TARGET_VOOBLY
    WriteProcessMemory(GetCurrentProcess(), (void*)addr, data, len, NULL);
#else
    g_pVoobly->Write(addr, data, len);
#endif
}

void __cdecl setHook(void* addr, void* newAddr)
{
#ifndef TARGET_VOOBLY
    BYTE data[8];
    data[3] = 0xE9;
    *((DWORD*)data + 1) = (DWORD)newAddr - (DWORD)addr - 5;
    WriteProcessMemory(GetCurrentProcess(), addr, data + 3, 5, NULL);
#else
    g_pVoobly->WriteJump((DWORD)addr, newAddr);
#endif
}

__declspec(naked) int __stdcall getWindowX()
{
    __asm
    {
        mov     eax, 006A3684h
        mov     eax, [eax]
        mov     eax, [eax + 24h]
        mov     eax, [eax + 8F4h]
        ret
    }
}

__declspec(naked) int __stdcall getWindowY()
{
    __asm
    {
        mov     eax, 006A3684h
        mov     eax, [eax]
        mov     eax, [eax + 24h]
        mov     eax, [eax + 8FCh]
        ret
    }
}

void* __stdcall getCurrentPlayer()
{
    return BaseGame__get_player(*base_game);
}

int __stdcall language_dll_load(UINT id, char* buf, int nmax)
{
    //HMODULE lang = GetModuleHandle("language_x2.dll");
    //if (!lang)
    //    LoadLibrary("language_x2.dll");
    //int e = GetLastError();
    int n = LoadString(GetModuleHandle("language_x2.dll"), id, buf, nmax);
    //e = GetLastError();
    if (n == 0)
        return LoadString(GetModuleHandle("language.dll"), id, buf, nmax);
    else
        return n;
}

//6A35D8 <- chat this

void __stdcall sendChat(char* s, int p)
{
    __asm
    {
        mov     eax, 006A35D8h
        mov     ecx, [eax]
        mov     edx, s
        mov     eax, p
        push    0
        push    0
        push    eax     //player
        push    edx     //str
        push    0       //int
        mov     eax, 0042D5E0h
        call    eax
    }
}

void __cdecl chat(char* format, ...)
{
    char s[0x100];
    va_list ap;
    va_start(ap, format);
    vsprintf(s, format, ap);
    sendChat(s, -1);
    va_end(ap);
}

__declspec(naked) int __stdcall getMapSize()
{
    __asm
    {
        mov     eax, 006A3684h
        mov     eax, [eax]
        mov     eax, [eax + 420h]
        mov     eax, [eax + 34h]
        mov     eax, [eax + 8]
        ret
    }
}

int __stdcall player_get_n_selection(void* player)
{
    return *(int*)((DWORD)player + 0x26C);
}

UNIT** __stdcall player_get_selection(void* player)
{
    return (UNIT**)((DWORD)player + 0x1C8);
}

float __stdcall player_get_camera_x(void* player)
{
    return *(float*)((DWORD)player + 0x178);
}
float __stdcall player_get_camera_y(void* player)
{
    return *(float*)((DWORD)player + 0x17C);
}

void* __stdcall get_TRIBE_Command()
{
    return *(void**)(*(DWORD*)(*(DWORD*)0x006A3684 + 0x420) + 0x68);
}

__declspec(naked) void* __stdcall get_main_view()
{
    __asm
    {
        mov     ecx, 006A3684h
        mov     ecx, [ecx]
        mov     edx, [ecx]
        call    dword ptr [edx + 30h]
        ret
    }
}

#pragma warning(push)
#pragma warning(disable:4100)
__declspec(naked) void __stdcall unit_change_ownership(UNIT* unit, void* new_owner)
{
    __asm
    {
        mov     ecx, [esp + 4]
        mov     eax, [esp + 8]
        push    eax
        mov     eax, [ecx]
        call    dword ptr [eax + 0BCh]
        ret     8
    }
}

__declspec(naked) void __stdcall kill_unit(UNIT* unit)
{
    __asm
    {
        mov     ecx, [esp + 4]
        mov     eax, [ecx]
        call    dword ptr [eax + 3DCh]
        retn    4
    }
}
#pragma warning(pop)

__declspec(naked) void* __stdcall get_top_panel()
{
    __asm
    {
        mov     ecx, panel_system
        mov     eax, [ecx + 10h]
        ret
    }
}

void* (__thiscall* BaseWorld__object)(void* this_, int oID) =
    (void* (__thiscall*) (void*, int))0x00623DB0;

void* (__thiscall* BaseGame__get_player)(void* globalPtr) =
    (void* (__thiscall*)(void* globalPtr))0x00428750;

int (__thiscall* BaseGame__playerID)(void* this_, int player_index) =
    (int (__thiscall*)(void*, int))0x0042CCC0;

bool (__thiscall* BaseGame__allowCheatCodes)(void* this_) =
    (bool (__thiscall*) (void*))0x0042C330;

bool (__thiscall* BaseGame__singlePlayerGame)(void* this_) =
    (bool (__thiscall*) (void*))0x0042C2F0;

bool (__thiscall* BaseGame__getRecordGame)(void* this_) =
    (bool (__thiscall*)(void*))0x0042C340;

void (__thiscall* BaseGame__setRecordGame)(void* this_, bool v) =
    (void (__thiscall*)(void*, bool))0x0042C550;

void (__thiscall* Game__show_status_message)(void* this_, char* messageIn, char* info_file, int info_id, int show_settings, int use_logo_background) =
    (void (__thiscall*)(void*, char*, char*, int, int, int))0x005E7EC0;

void (__thiscall* Game__close_status_message)(void* this_) =
    (void (__thiscall*)(void*))0x005E7FE0;

void (__thiscall* Game__set_player)(void* this_, __int16 new_player_id) =
    (void (__thiscall*)(void*, __int16))0x005E68B0;

int (__thiscall* Game__playerColor)(void* this_, int player_num) =
    (int (__thiscall*)(void*, int))0x005EF060;

void* (__thiscall* GameScreen__find_next_idle_unit)(void* this_, int last_object_id) =
    (void* (__thiscall*) (void*, int))0x00506340;

void* (__thiscall* GameScreen__find_next_idle_military_unit)(void* this_, int last_object_id) =
    (void* (__thiscall*) (void*, int))0x005064B0;

void* (__thiscall* WorldPlayer__find_obj)(void* this_, __int16 obj_id, void* after_obj, __int16 obj_id_2) =
    (void* (__thiscall*) (void*, __int16, void*, __int16))0x005CFA20;

int (__thiscall* WorldPlayerBase__select_object)(void* this_, void* unit, int play_sound) =
    (int (__thiscall*) (void*, void*, int))0x004C2DC0;

int (__thiscall* WorldPlayerBase__select_one_object)(void* this_, void* unit, int play_sound) =
    (int (__thiscall*) (void*, void*, int))0x004C2ED0;

int (__thiscall* WorldPlayerBase__unselect_object)(void* this_) =
    (int (__thiscall*) (void*))0x004C3050;

int (__thiscall* WorldPlayerBase__set_view_loc)(void* player, float x, float y, int spectatingView) =
    (int (__thiscall*) (void*, float, float, int))0x004C2010;

int (__thiscall* GameSoundEffectsManager__playSound)(void* this_, int soundId, int pan, int volume) =
    (int (__thiscall*) (void*, int, int, int))0x0042CD70;

int (__thiscall* RGE_Command__submit)(void* command, void* order, int order_size, int issuer) =
    (int (__thiscall*) (void*, void*, int, int))0x0044CFD0;

void (__thiscall* RGE_Map__set_terrain)(void* this_, void* obj_owner, void* gworld, __int16 mapcol1, __int16 maprow1, __int16 mapcol2, __int16 maprow2, unsigned __int8 terrain, unsigned __int8 set_flag, int delete_obj) =
    (void (__thiscall*)(void*, void*, void*, __int16, __int16, __int16, __int16, unsigned __int8, unsigned __int8, int))0x00495F80;

void* (__cdecl* calloc_internal)(size_t number, size_t size) =
    (void* (__cdecl*) (size_t, size_t))0x00632D33;

void (__cdecl* free_internal)(void* memory) =
    (void (__cdecl*) (void*))0x00632CCA;

int (__thiscall* unit_detach) (UNIT* unit) =
    (int (__thiscall*) (UNIT*))0x0055F350;

void (__fastcall* deflate_write) (void* outfile, void* buffer, unsigned int size) =
    (void (__fastcall*) (void*, void*, unsigned int))0x004D5790;

void (__fastcall* deflate_read) (void* infile, void* buffer, unsigned int size) =
    (void (__fastcall*) (void*, void*, unsigned int))0x004D5550;

int (__thiscall* RGE_View__display_object_selection)(void* this_, int id, int duration, int select_type, int reset_type) =
    (int (__thiscall*) (void*, int, int, int, int))0x0060A9A0;

int (__thiscall* TPanelSystem__destroyPanel)(void* this_, char* n) =
    (int (__thiscall*)(void*, char*))0x004B4D60;

void (__thiscall* TEasy_Panel__popupOKDialog)(void* this_, char* text, char* panel_title, int wid, int hgt, int centered) =
    (void (__thiscall*)(void*, char*, char*, int, int, int))0x004BB250;
