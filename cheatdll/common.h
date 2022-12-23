#pragma once
#include "stdafx.h"
#include "structs.h"

#ifdef VOOBLY_EF
#define DATA_FOLDER_PREFIX_FROM_ROOT "Voobly Mods\\SWBGCC\\Data Mods\\Expanding Fronts Data\\Data\\"
#define DATA_FOLDER_PREFIX_FROM_DATA "..\\Voobly Mods\\SWBGCC\\Data Mods\\Expanding Fronts Data\\Data\\"
#else
#define DATA_FOLDER_PREFIX_FROM_ROOT "data\\"
#define DATA_FOLDER_PREFIX_FROM_DATA ""
#endif

#ifdef TARGET_VOOBLY
#define DLL_NAME "userpatch.dll"
#else
#define DLL_NAME "efpatch.dll"
#endif

void __cdecl writeByte(DWORD addr, BYTE val);
void __cdecl writeWord(DWORD addr, WORD val);
void __cdecl writeDword(DWORD addr, DWORD val);
void __cdecl writeByteF(DWORD addr, BYTE val);
void __cdecl writeDwordF(DWORD addr, DWORD val);
void __cdecl setHook(void* addr, void* newAddr);
void __cdecl writeData(DWORD addr, const void* data, int len);
//void __cdecl setHookRestorable (void* addr, void* newAddr, void* oldData);
//void __cdecl restoreHook (void* addr, void* oldData);

RGE_Player* __fastcall get_player(int);

int __stdcall language_dll_load(UINT id, char* buf, int nmax);

errno_t __cdecl strcpy_safe(char* dest, size_t size, const char* source);

int __stdcall getWindowX();
int __stdcall getWindowY();

int __stdcall get_sn(RGE_Player* player, int sn);

struct BYTE_ASSIGN
{
    DWORD  addr;
    BYTE val;
};

struct INT_ASSIGN
{
    DWORD  addr;
    DWORD  val;
};

struct SET_HOOK_ASSIGN
{
    void* addr;
    void* newAddr;
};

void __stdcall sendChat(char* s, int p);
void __cdecl chat(char* format, ...);

void* __stdcall get_TRIBE_Command();

void* __stdcall get_main_view();


int __stdcall getMapSize();

extern RGE_Base_Game** const base_game;
extern TPanelSystem* const panel_system;

extern TCommunications_Handler** const comm;
extern TChat** const chat_p;

extern HINSTANCE* const hInstance_dll;

extern void* (__cdecl* const calloc_internal)(size_t number, size_t size);
extern void (__cdecl* const free_internal)(void* memory);
extern int (__cdecl* const rand_internal)();

extern void (__fastcall* const deflate_write)(void* outfile, void* buffer, unsigned int size);
extern void (__fastcall* const deflate_read)(void* infile, void* buffer, unsigned int size);
