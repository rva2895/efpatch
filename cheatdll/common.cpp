#include "stdafx.h"
#include "common.h"

RGE_Base_Game** const base_game = (RGE_Base_Game** const)0x006A3684;
TPanelSystem* const panel_system = (TPanelSystem* const)0x006ADBB8;
int* const world_update_counter = (int* const)0x007A22F8;

TCommunications_Handler** const comm = (TCommunications_Handler** const)0x006A35E0;
TChat** const tchat = (TChat** const)0x006A35D8;

HINSTANCE* const hInstance_dll = (HINSTANCE* const)0x0078F81C;

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

errno_t __cdecl strcpy_safe(char* dest, size_t size, const char* source)
{
    if (size > 0)
    {
        dest[0] = '\0';
        return strncat_s(dest, size, source, size - 1);
    }
    else
        return 0;
}

unsigned int get_worldtime()
{
    TRIBE_World* world = (*base_game)->world;
    if (world)
        return world->world_time;
    else
        return 0;
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

std::string get_string(int id)
{
    char temp_str_buffer[0x400];
    (*base_game)->vfptr->get_string3(*base_game, id, temp_str_buffer, _countof(temp_str_buffer));
    return temp_str_buffer;
}

void __stdcall sendChat(const char* s, int p)
{
    if (*tchat)
        TChat__AddChatMsg(*tchat, NULL, s, p, 0, 0);
}

void __cdecl chat(const char* format, ...)
{
    char s[0x100];
    va_list ap;
    va_start(ap, format);
    vsnprintf(s, _countof(s), format, ap);
    sendChat(s, -1);
    va_end(ap);
}

void* (__cdecl* const calloc_internal)(size_t number, size_t size) =
    (void* (__cdecl* const) (size_t, size_t))0x00632D33;

void (__cdecl* const free_internal)(void* memory) =
    (void (__cdecl* const) (void*))0x00632CCA;

int (__cdecl* const rand_internal)() =
    (int (__cdecl* const)())0x00632BDD;

int (__cdecl* const read_internal)(int handle, void* buffer, int size) =
    (int (__cdecl* const)(int, void*, int))0x006340C7;

int (__cdecl* const write_internal)(int handle, void* buffer, int size) =
    (int (__cdecl* const)(int, void*, int))0x00634638;

int (__cdecl* const tell_internal)(int handle) =
    (int (__cdecl* const)(int))0x0063458D;

int (__cdecl* const lseek_internal)(int handle, int distance, int orig) =
    (int (__cdecl* const)(int, int, int))0x0063459E;
