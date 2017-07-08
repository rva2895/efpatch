#pragma once

void __cdecl setByte (int addr, unsigned char val);
void __cdecl setInt (int addr, int val);
void __cdecl setByteF (int addr, unsigned char val);
void __cdecl setIntF (int addr, int val);
void __cdecl setHook (void* addr, void* newAddr);
void __cdecl setHookRestorable (void* addr, void* newAddr, void* oldData);
void __cdecl restoreHook (void* addr, void* oldData);

int __cdecl readInt (int addr);

int __stdcall getWindowX();
int __stdcall getWindowY();

struct BYTE_ASSIGN
{
	int  addr;
	unsigned char val;
};

struct INT_ASSIGN
{
	int  addr;
	int  val;
};

struct SET_HOOK_ASSIGN
{
	void* addr;
	void* newAddr;
};

//void myCrtStartup ();


/*char* (__cdecl *strncpy__)(char *Dest, const char *Source, size_t Count);
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
char* (__cdecl *strupr__)(char *String);*/

/*char* __cdecl strncpy__(char *Dest, const char *Source, size_t Count);
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
char* __cdecl strupr__(char *String);*/

//#define _malloc malloc
//#define _free free
//#define _exit exit
