#pragma once

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
void __cdecl writeData(DWORD addr, void* data, int len);
//void __cdecl setHookRestorable (void* addr, void* newAddr, void* oldData);
//void __cdecl restoreHook (void* addr, void* oldData);

//int __cdecl readInt (int addr);

void* __stdcall getCurrentPlayer();
void* __fastcall get_player(int);

int __stdcall language_dll_load(UINT id, char* buf, int nmax);

int __stdcall getWindowX();
int __stdcall getWindowY();

int __stdcall get_sn(void* player, int sn);

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

/*void myCrtStartup ();

char* (__cdecl *strncpy__)(char *Dest, const char *Source, size_t Count);
char* (__cdecl *strstr__)(const char *Str, const char *SubStr);
int (__cdecl *fclose__)(void* File);
int (__cdecl *sscanf__)(const char *Src, const char *Format, ...);
void* (__cdecl *fopen__)(const char *Filename, const char *Mode);
int (__cdecl *sprintf__)(char*, const char*, ...);
void* (__cdecl *malloc__)(size_t Size);
int (__cdecl *vsprintf__)(char *Dest, const char *Format, char* Args);
int (__cdecl *fprintf__)(void *File, const char *Format, ...);
int (__cdecl *fscanf__)(void *File, const char *Format, ...);
void* (__cdecl *memset__)(void *Dst, int Val, size_t Size);
void* (__cdecl *memcpy__)(void *Dst, const void *Src, size_t Size);
char* (__cdecl *strcpy__)(char *Dest, const char *Source);
char* (__cdecl *strcat__)(char *Dest, const char *Source);
int (__cdecl *strcmp__)(const char *Str1, const char *Str2);
size_t (__cdecl *strlen__)(const char *Str);
void (__cdecl *free__)(void* mem);
void* (__cdecl *realloc__)(void*, size_t, size_t);
char* (__cdecl *strupr__)(char *String);

char* __cdecl strncpy__(char *Dest, const char *Source, size_t Count);
char* __cdecl strstr__(const char *Str, const char *SubStr);
int __cdecl fclose__(void* File);
int __cdecl sscanf__(const char *Src, const char *Format, ...);
void* __cdecl fopen__(const char *Filename, const char *Mode);
int __cdecl sprintf__(char*, const char*, ...);
void* __cdecl malloc__(size_t Size);
int __cdecl vsprintf__(char *Dest, const char *Format, char* Args);
int __cdecl fprintf__(void *File, const char *Format, ...);
int __cdecl fscanf__(void *File, const char *Format, ...);
void* __cdecl memset__(void *Dst, int Val, size_t Size);
void* __cdecl memcpy__(void *Dst, const void *Src, size_t Size);
char* __cdecl strcpy__(char *Dest, const char *Source);
char* __cdecl strcat__(char *Dest, const char *Source);
int __cdecl strcmp__(const char *Str1, const char *Str2);
size_t __cdecl strlen__(const char *Str);
void __cdecl free__(void* mem);
void* __cdecl realloc__(void*, size_t, size_t);
char* __cdecl strupr__(char *String);

//#define _malloc malloc
//#define _free free
//#define _exit exit*/

int __stdcall getMapSize();

extern void** BaseGame_bg;

extern void* (__thiscall* BaseWorld__object)(void* this_, int oID);

extern void* (__thiscall* BaseGame__get_player)(void* globalPtr);

extern void* (__thiscall* GameScreen__find_next_idle_unit)(void* this_, int last_object_id);
extern void* (__thiscall* GameScreen__find_next_idle_military_unit)(void* this_, int last_object_id);

extern void* (__thiscall* WorldPlayer__find_obj)(void* this_, __int16 obj_id, void* after_obj, __int16 obj_id_2);

extern int (__thiscall* WorldPlayerBase__select_object)(void* this_, void* unit, int play_sound);
extern int (__thiscall* WorldPlayerBase__unselect_object)(void* this_);
extern int (__thiscall* WorldPlayerBase__set_view_loc)(void* player, float x, float y, int spectatingView);

extern int (__thiscall* GameSoundEffectsManager__playSound)(void* this_, int soundId, int pan, int volume);
