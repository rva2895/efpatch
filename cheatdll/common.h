#pragma once
#include "stdafx.h"
#include "structs.h"

#define VOOBLY_NAME_SUFFIX ""
#define VOOBLY_DATA_MOD_NAME "Expanding Fronts Data" VOOBLY_NAME_SUFFIX
#define VOOBLY_ASSETS_MOD_NAME "EF Assets" VOOBLY_NAME_SUFFIX
#define VOOBLY_EXT_ASSETS_MOD_NAME "EF Extra Assets" VOOBLY_NAME_SUFFIX
#define VOOBLY_LOCAL_MOD_PATH "Voobly Mods\\SWBGCC\\Local Mods\\"
#define VOOBLY_ASSETS_ARCHIVE_NAME "ef_assets.zip.001"
#define VOOBLY_EXT_ASSETS_ARCHIVE_NAME "ef_assets.zip.002"
#ifdef VOOBLY_EF
#define VOOBLY_EXE_MOD_NAME "EF EXE Patch" VOOBLY_NAME_SUFFIX
#else
#define VOOBLY_EXE_MOD_NAME "EXE Patch" VOOBLY_NAME_SUFFIX
#endif

#ifdef VOOBLY_EF
#define DATA_FOLDER_PREFIX_FROM_ROOT "Voobly Mods\\SWBGCC\\Data Mods\\" VOOBLY_DATA_MOD_NAME "\\Data\\" 
#define DATA_FOLDER_PREFIX_FROM_DATA "..\\Voobly Mods\\SWBGCC\\Data Mods\\" VOOBLY_DATA_MOD_NAME "\\Data\\"
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
void __cdecl writeNops(DWORD addr, size_t len);
void __cdecl writeByteF(DWORD addr, BYTE val);
void __cdecl writeDwordF(DWORD addr, DWORD val);
void __cdecl setHook(void* addr, void* newAddr);
void __cdecl setHookCall(void* addr, void* newAddr);
void __cdecl writeData(DWORD addr, const void* data, size_t len);
//void __cdecl setHookRestorable (void* addr, void* newAddr, void* oldData);
//void __cdecl restoreHook (void* addr, void* oldData);

int __stdcall language_dll_load(UINT id, char* buf, int nmax);

errno_t __cdecl strcpy_safe(char* dest, size_t size, const char* source);

unsigned int get_worldtime();

struct BYTE_ASSIGN
{
    DWORD  addr;
    BYTE val;
};

std::string get_string(int id);

void __stdcall sendChat(const char* s, int p);
void __cdecl chat(const char* format, ...);

extern RGE_Base_Game** const base_game;
extern TPanelSystem* const panel_system;
extern int* const world_update_counter;

extern TCommunications_Handler** const comm;
extern TChat** const tchat;

extern HINSTANCE* const hInstance_dll;

extern void* (__cdecl* const calloc_internal)(size_t number, size_t size);
extern void* (__cdecl* const operator_new_internal)(size_t size);
extern void (__cdecl* const free_internal)(void* memory);
extern int (__cdecl* const rand_internal)();
extern int (__cdecl* const read_internal)(int handle, void* buffer, int size);
extern int (__cdecl* const write_internal)(int handle, void* buffer, int size);
extern int (__cdecl* const tell_internal)(int handle);
extern int (__cdecl* const lseek_internal)(int handle, int distance, int orig);
extern FILE* (__cdecl* const fopen_internal)(const char* Filename, const char* Mode);
extern __int32 (__cdecl* const ftell_internal)(FILE* File);
extern int (__cdecl* const fseek_internal)(FILE* File, __int32 Offset, int Origin);
extern size_t (__cdecl* const fread_internal)(void* DstBuf, size_t ElementSize, size_t Count, FILE* File);
extern size_t (__cdecl* const fwrite_internal)(const void* Str, size_t Size, size_t Count, FILE* File);
extern int (__cdecl* const fscanf_internal)(FILE* File, const char* Format, ...);
extern char* (__cdecl* const fgets_internal)(char* Buf, int MaxCount, FILE* File);
