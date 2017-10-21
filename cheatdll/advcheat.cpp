#include "stdafx.h"

#include "advcheat.h"

#include "casts.h" //<- for /print-map command

/*extern char* (__cdecl *strncpy__)(char *Dest, const char *Source, size_t Count);
extern char* (__cdecl *strstr__)(const char *Str, const char *SubStr);
extern int (__cdecl *fclose__)(void* File);
extern int (__cdecl *sscanf__)(const char *Src, const char *Format, ...);
extern void* (__cdecl *fopen__)(const char *Filename, const char *Mode);
extern int (__cdecl *sprintf__)(char*, const char*, ...);
extern void* (__cdecl *malloc__)(size_t Size);
extern int (__cdecl *vsprintf__)(char *Dest, const char *Format, char* Args);
extern int (__cdecl *fprintf__)(void *File, const char *Format, ...);
extern int (__cdecl *fscanf__)(void *File, const char *Format, ...);
extern void* (__cdecl *memset__)(void *Dst, int Val, size_t Size);
extern void* (__cdecl *memcpy__)(void *Dst, const void *Src, size_t Size);
extern char* (__cdecl *strcpy__)(char *Dest, const char *Source);
extern char* (__cdecl *strcat__)(char *Dest, const char *Source);
extern int (__cdecl *strcmp__)(const char *Str1, const char *Str2);
extern size_t (__cdecl *strlen__)(const char *Str);
extern void (__cdecl *free__)(void* mem);
extern void* (__cdecl *realloc__)(void*, size_t, size_t);
extern char* (__cdecl *strupr__)(char *String);*/

void prepareToEngageCheatCreateUnit (int unitId)
{
#ifdef _DEBUG
	log ("Initiating creation of unit ID %d (0x%X)", unitId, unitId);
#endif
	unsigned long c;
	unsigned char movEax = 0xB8;
	DWORD cheatDetected = 0x005EDC41; //change to mov eax,1
	DWORD newUnitId = 0x00603E3C;
	DWORD one = 1;
	WriteProcessMemory (GetCurrentProcess(), (void*)cheatDetected, &movEax, 1, &c); //simulate "simonsays" cheat
	WriteProcessMemory (GetCurrentProcess(), (char*)cheatDetected+1, &one, 4, &c);
	WriteProcessMemory (GetCurrentProcess(), (void*)newUnitId, &unitId, 4, &c);
}

void prepareToEngageCheatResearchTech (int researchId)
{
#ifdef _DEBUG
	log ("Initiating researching of tech ID %d (0x%X)", researchId, researchId);
#endif
	unsigned long c;
	unsigned char movEax = 0xB8;
	DWORD cheatDetected = 0x005EDCD5; //change to mov eax,1
	DWORD newTechId = 0x00603DD3;
	DWORD one = 1;
	WriteProcessMemory (GetCurrentProcess(), (void*)cheatDetected, &movEax, 1, &c); //simulate "the most powerful jedi" cheat
	WriteProcessMemory (GetCurrentProcess(), (char*)cheatDetected+1, &one, 4, &c);
	WriteProcessMemory (GetCurrentProcess(), (void*)newTechId, &researchId, 4, &c);
}

bool restoreCheatFlag = 0;

void restoreAll ()
{
#ifdef _DEBUG
	log ("Restoring all cheats");
#endif
	unsigned long c;
	DWORD cheatDetectedUnit = 0x005EDC41;
	DWORD cheatDetectedTech = 0x005EDCD5;
	DWORD unitCallStrStr = 0x000452EA;
	DWORD techCallStrStr = 0x00045256;
	DWORD unitID = 0x00603E3C;
	DWORD techID = 0x00603DD3;
	DWORD call = 0xE8;
	int unit = 0x4B4;
	int tech = 0x24C;
	WriteProcessMemory (GetCurrentProcess(), (void*)cheatDetectedUnit, &call, 1, &c);
	WriteProcessMemory (GetCurrentProcess(), (char*)cheatDetectedUnit+1, &unitCallStrStr, 4, &c);
	WriteProcessMemory (GetCurrentProcess(), (void*)cheatDetectedTech, &call, 1, &c);
	WriteProcessMemory (GetCurrentProcess(), (char*)cheatDetectedTech+1, &techCallStrStr, 4, &c);
	WriteProcessMemory (GetCurrentProcess(), (void*)unitID, &unit, 4, &c);
	WriteProcessMemory (GetCurrentProcess(), (void*)techID, &tech, 4, &c);

	restoreCheatFlag = 0;
}

__declspec(naked) void __stdcall takeControl (int p)
{
	__asm
	{
		mov     eax, [esp+4]
		push    esi
		push    edi
		push    ebp
		push    ebx
		mov     ecx, 6A3684h
		mov     ecx, [ecx]
		push    eax
		mov     edx, 005E68B0h //takeControl
		call    edx
		pop     ebx
		pop     ebp
		pop     edi
		pop     esi
		ret     4
	}
}

//6A35D8 <- chat this

void __stdcall sendChat(char* s, int p)
{
	__asm
	{
		mov		eax, 006A35D8h
		mov		ecx, [eax]
		mov		edx, s
		mov		eax, p
		push	0
		push	0
		push	eax			//player
		push	edx			//str
		push	0			//int
		mov		eax, 0042D5E0h
		call	eax
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

int __stdcall checkCheats (char* s)
{
	char dummy[100];
	int id;
#ifdef _DEBUG
	log ("Scanning chat...");
#endif

	if (restoreCheatFlag)
		restoreAll ();

	if (strstr (s, "/create-unit"))
	{
		restoreCheatFlag = 1;
		sscanf (s, "%s %d", dummy, &id);
		prepareToEngageCheatCreateUnit (id);
		return false;
	}
	if (strstr (s, "/research-tech"))
	{
		restoreCheatFlag = 1;
		sscanf (s, "%s %d", dummy, &id);
		prepareToEngageCheatResearchTech (id);
		return false;
	}
	if (strstr (s, "/take-control"))
	{
		restoreCheatFlag = 1;
		sscanf (s, "%s %d", dummy, &id);
		if ((id >= 0) && (id <= 8))
		{
			takeControl(id);
			chat("Taking control of player %d", id);
		}
		return true;
	}

	return false;
}

void __declspec(naked) scanChat () //put on sub at 0x005ED970
{
	__asm
	{
		mov		eax, [esp+8] //2nd argument of the function we are intercepting is pointer to the chat string
		push	ecx

		push	eax
		call	checkCheats
		pop		ecx
		sub		esp, 300h
		push	ebx
		push	edi
		test	eax, eax
		jnz		_no_chat
		push	005ED978h
		ret
_no_chat:
		push	005EDD65h
		ret
	}
}

void setAdvCheatHooks ()
{
#ifdef _DEBUG
	log ("Setting adv cheat hooks...");
#endif

	setHook ((void*)0x005ED970, &scanChat);
}
