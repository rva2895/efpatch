#include "stdafx.h"
#include "common.h"

RGE_Base_Game** const base_game = (RGE_Base_Game** const)0x006A3684;
TPanelSystem* const panel_system = (TPanelSystem* const)0x006ADBB8;
int* const world_update_counter = (int* const)0x007A22F8;

TCommunications_Handler** const comm = (TCommunications_Handler** const)0x006A35E0;
TChat** const tchat = (TChat** const)0x006A35D8;

RGE_Obstruction_Manager** const OBSystem_Land = (RGE_Obstruction_Manager** const)0x006ACCDC;
RGE_Airspace_Manager** const OBSystem_Air = (RGE_Airspace_Manager** const)0x006ACCE0;

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

void __cdecl writeData(DWORD addr, const void* data, size_t len)
{
#ifndef TARGET_VOOBLY
    WriteProcessMemory(GetCurrentProcess(), (void*)addr, data, len, NULL);
#else
    g_pVoobly->Write(addr, data, len);
#endif
}

const std::array<BYTE, 9> nop_bytes[] =
{
    { 0x90 },
    { 0x66, 0x90 },
    { 0x0F, 0x1F, 0x00 },
    { 0x0F, 0x1F, 0x40, 0x00 },
    { 0x0F, 0x1F, 0x44, 0x00, 0x00 },
    { 0x66, 0x0F, 0x1F, 0x44, 0x00, 0x00 },
    { 0x0F, 0x1F, 0x80, 0x00, 0x00, 0x00, 0x00 },
    { 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x66, 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00}
};

void __cdecl write_nop_seq(DWORD addr, size_t len)
{
    if (len >= 1 && len <= 9)
        writeData(addr, nop_bytes[len - 1].data(), len);
}

void __cdecl writeNops(DWORD addr, size_t len)
{
    while (len > 0)
    {
        size_t nop_len = len > 9 ? 9 : len;
        write_nop_seq(addr, nop_len);
        len -= nop_len;
        addr += nop_len;
    }
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

void __cdecl setHookCall(void* addr, void* newAddr)
{
#ifndef TARGET_VOOBLY
    BYTE data[8];
    data[3] = 0xE8;
    *((DWORD*)data + 1) = (DWORD)newAddr - (DWORD)addr - 5;
    WriteProcessMemory(GetCurrentProcess(), addr, data + 3, 5, NULL);
#else
    g_pVoobly->WriteJump((DWORD)addr, newAddr);
    g_pVoobly->Write((DWORD)addr, "E8");
#endif
}

size_t strlcpy(char* dst, const char* src, size_t siz)
{
    char*       d = dst;
    const char* s = src;
    size_t      n = siz;

    /* Copy as many bytes as will fit */
    if (n != 0)
    {
        while (--n != 0)
        {
            if ((*d++ = *s++) == '\0')
                break;
        }
    }

    /* Not enough room in dst, add NUL and traverse rest of src */
    if (n == 0)
    {
        if (siz != 0)
            *d = '\0';          /* NUL-terminate dst */
        while (*s++)
            ;
    }

    return (s - src - 1);       /* count does not include NUL */
}

char* make_str_copy(const char* src)
{
    size_t size = strlen(src) + 1;
    char* s = (char*)malloc(size);
    memcpy(s, src, size);
    return s;
}

void trySetProcessDPIAware()
{
    BOOL (__stdcall* SetProcessDPIAware_p)();
    SetProcessDPIAware_p = GetProcAddress(GetModuleHandle("user32.dll"), "SetProcessDPIAware");
    if (SetProcessDPIAware_p)
    {
        SetProcessDPIAware_p();
        log("SetProcessDPIAware() ok");
    }
    else
    {
        log("SetProcessDPIAware() not available");
    }
}

unsigned int get_worldtime()
{
    TRIBE_World* world = (*base_game)->world;
    if (world)
        return world->world_time;
    else
        return 0;
}

bool __stdcall file_exists(const char* filename)
{
    FILE* f = fopen(filename, "rb");
    if (f)
    {
        fclose(f);
        return true;
    }
    else
        return false;
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

std::string __stdcall get_string(int id)
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

void __stdcall load_ids_from_txt(
    const char* prefix,
    const char* filename,
    __int16** list,
    int* n,
    bool* flag,
    void (__cdecl *install_func)(),
    const char* desc)
{
    log(desc);
    char full_filename[MAX_PATH];
    snprintf(full_filename, _countof(full_filename), "%s%s", prefix, filename);
    FILE* f = fopen(full_filename, "rt");
    if (f)
    {
        __int16 id;
        *n = 0;
        if (*list)
        {
            free(*list);
            *list = NULL;
        }

        while (fscanf(f, "%hd", &id) > 0)
        {
            (*n)++;
            *list = (__int16*)realloc(*list, *n * sizeof(__int16));
            (*list)[*n - 1] = id;
        }

        fclose(f);
        if (!*flag)
        {
            install_func();
            *flag = true;
        }
    }
    else
        log("Warning: %s not found, using default settings", full_filename);
}

bool __stdcall is_id_in_list(__int16 id, __int16* list, int n)
{
    for (int i = 0; i < n; i++)
        if (id == list[i])
            return true;
    return false;
}

void* (__cdecl* const calloc_internal)(size_t number, size_t size) =
    (void* (__cdecl* const) (size_t, size_t))0x00632D33;

void* (__cdecl* const operator_new_internal)(size_t size) =
    (void* (__cdecl* const) (size_t))0x00632B9D;

void (__cdecl* const operator_delete_internal)(void* ptr) =
    (void (__cdecl* const) (void* ptr))0x00632B42;

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

FILE* (__cdecl* const fopen_internal)(const char* Filename, const char* Mode) =
    (FILE* (__cdecl* const)(const char*, const char*))0x006330B1;

__int32 (__cdecl* const ftell_internal)(FILE* File) =
    (__int32 (__cdecl* const) (FILE*))0x006361B6;

int (__cdecl* const fseek_internal)(FILE* File, __int32 Offset, int Origin) =
    (int (__cdecl* const) (FILE*, __int32, int))0x00635E6D;

size_t (__cdecl* const fread_internal)(void* DstBuf, size_t ElementSize, size_t Count, FILE* File) =
    (size_t (__cdecl* const) (void*, size_t, size_t, FILE*))0x006360CE;

size_t (__cdecl* const fwrite_internal)(const void* Str, size_t Size, size_t Count, FILE* File) =
    (size_t (__cdecl* const)(const void*, size_t, size_t, FILE*))0x00635EF9;

int (__cdecl* const fscanf_internal)(FILE* File, const char* Format, ...) =
    (int (__cdecl* const) (FILE*, const char*, ...))0x0063481F;

char* (__cdecl* const fgets_internal)(char* Buf, int MaxCount, FILE* File) =
    (char* (__cdecl* const) (char*, int, FILE*))0x0063303A;

char* (__cdecl* const mbsupr_internal)(char* str) =
    (char* (__cdecl* const)(char*))0x0063658F;
