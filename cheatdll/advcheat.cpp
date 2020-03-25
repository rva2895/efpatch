#include "stdafx.h"

#include "advcheat.h"
#include "advtriggereffect.h"
#include "effects.h"

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

int (__thiscall* unit_detach) (UNIT* unit) =
	(int (__thiscall*) (UNIT*))0x0055F350;

void prepareToEngageCheatCreateUnit(int unitId)
{
#ifdef _DEBUG
	log("Initiating creation of unit ID %d (0x%X)", unitId, unitId);
#endif
	unsigned long c;
	unsigned char movEax = 0xB8;
	DWORD cheatDetected = 0x005EDC41; //change to mov eax,1
	DWORD newUnitId = 0x00603E3C;
	DWORD one = 1;
	WriteProcessMemory(GetCurrentProcess(), (void*)cheatDetected, &movEax, 1, &c); //simulate "simonsays" cheat
	WriteProcessMemory(GetCurrentProcess(), (char*)cheatDetected + 1, &one, 4, &c);
	WriteProcessMemory(GetCurrentProcess(), (void*)newUnitId, &unitId, 4, &c);
}

void prepareToEngageCheatResearchTech(int researchId)
{
#ifdef _DEBUG
	log("Initiating researching of tech ID %d (0x%X)", researchId, researchId);
#endif
	unsigned long c;
	unsigned char movEax = 0xB8;
	DWORD cheatDetected = 0x005EDCD5; //change to mov eax,1
	DWORD newTechId = 0x00603DD3;
	DWORD one = 1;
	WriteProcessMemory(GetCurrentProcess(), (void*)cheatDetected, &movEax, 1, &c); //simulate "the most powerful jedi" cheat
	WriteProcessMemory(GetCurrentProcess(), (char*)cheatDetected + 1, &one, 4, &c);
	WriteProcessMemory(GetCurrentProcess(), (void*)newTechId, &researchId, 4, &c);
}

bool restoreCheatFlag = 0;

void restoreAll()
{
#ifdef _DEBUG
	log("Restoring all cheats");
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
	WriteProcessMemory(GetCurrentProcess(), (void*)cheatDetectedUnit, &call, 1, &c);
	WriteProcessMemory(GetCurrentProcess(), (char*)cheatDetectedUnit + 1, &unitCallStrStr, 4, &c);
	WriteProcessMemory(GetCurrentProcess(), (void*)cheatDetectedTech, &call, 1, &c);
	WriteProcessMemory(GetCurrentProcess(), (char*)cheatDetectedTech + 1, &techCallStrStr, 4, &c);
	WriteProcessMemory(GetCurrentProcess(), (void*)unitID, &unit, 4, &c);
	WriteProcessMemory(GetCurrentProcess(), (void*)techID, &tech, 4, &c);

	restoreCheatFlag = 0;
}

#pragma warning(push)
#pragma warning(disable:4100)
__declspec(naked) void __stdcall takeControl(int p)
{
	__asm
	{
		mov     eax, [esp + 4]
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
#pragma warning(pop)

//
bool time_collect = false;
bool time_stage_find = false;
extern volatile bool answer;
extern volatile bool answer_flag;
//

int __stdcall checkCheats(char* s2)
{
	char dummy[100];
	char s[0x100];
	strcpy(s, s2);
	_strupr(s);
	int id;
#ifdef _DEBUG
	log("Scanning chat...");
#endif

	if (restoreCheatFlag)
		restoreAll();

	if (strstr(s, "/CREATE-UNIT"))
	{
		restoreCheatFlag = 1;
		sscanf(s, "%s %d", dummy, &id);
		prepareToEngageCheatCreateUnit(id);
		return false;
	}
	if (strstr(s, "/RESEARCH-TECH"))
	{
		restoreCheatFlag = 1;
		sscanf(s, "%s %d", dummy, &id);
		prepareToEngageCheatResearchTech(id);
		return false;
	}
	if (strstr(s, "/TAKE-CONTROL"))
	{
		restoreCheatFlag = 1;
		sscanf(s, "%s %d", dummy, &id);
		if ((id >= 0) && (id <= 8))
		{
			player_clearSelection(getCurrentPlayer());
			takeControl(id);
			chat("Taking control of player %d", id);
		}
		return true;
	}
	if (strstr(s, "LUMINOUS BEINGS ARE WE"))
	{
		restoreCheatFlag = 1;
		player_clearSelection(getCurrentPlayer());
		takeControl(0);
		return true;
	}
	if (strstr(s, "HELP ME OBI-WAN"))
	{
		restoreCheatFlag = 1;
		prepareToEngageCheatResearchTech(798);
		return false;
	}
	if (strstr(s, "NOW THIS IS PODRACING"))
	{
		restoreCheatFlag = 1;
		prepareToEngageCheatResearchTech(799);
		return false;
	}
	if (strstr(s, "YOU HAVE FAILED ME FOR THE LAST TIME"))
	{
		sel_iterator i(getCurrentPlayer());
		UNIT* unit;
		for (; unit = *i, unit != 0; ++i)
			if (unit->player != getCurrentPlayer())
			{
				if (unit->prop_object->type > 30)
					unit->hp = 0;
			}
		return true;
	}
	if (strstr(s, "THE DEFLECTOR SHIELD IS TOO STRONG"))
	{
		sel_iterator i(getCurrentPlayer());
		UNIT* unit;
		for (; unit = *i, unit != 0; ++i)
			if (unit->player == getCurrentPlayer())
			{
				unit_detach(unit);
				unit->prop_object->unit_attribute |= 0x40;
			}
		return true;
	}
	if (strstr(s, "FORCEHEAL"))
	{
		sel_iterator i(getCurrentPlayer());
		UNIT* unit;
		for (; unit = *i, unit != 0; ++i)
			if (unit->player == getCurrentPlayer())
			{
				effectUnitVarActual(unit, "SET HPPercent 100");
			}
		return true;
	}
	if (strstr(s, "NO SHIP THAT SMALL HAS A CLOAKING DEVICE"))
	{
		sel_iterator i(getCurrentPlayer());
		UNIT* unit;
		for (; unit = *i, unit != 0; ++i)
			if (unit->player == getCurrentPlayer())
			{
				unit_detach(unit);
				unit->prop_object->unit_attribute |= 0x4;
			}
		return true;
	}
	if (strstr(s, "FORCEPROTECT"))
	{
		sel_iterator i(getCurrentPlayer());
		UNIT* unit;
		for (; unit = *i, unit != 0; ++i)
			if (unit->player == getCurrentPlayer())
			{
				unit_detach(unit);
				advTriggerEffectActual(unit->prop_object, "ADD Armor 3 1");
				advTriggerEffectActual(unit->prop_object, "ADD Armor 4 1");
			}
		return true;
	}
	if (strstr(s, "ULTIMATE POWER IN THE UNIVERSE"))
	{
		restoreCheatFlag = 1;
		prepareToEngageCheatCreateUnit(1802);
		return false;
	}
	if (strstr(s, "UNLIMITED POWER"))
	{
		restoreCheatFlag = 1;
		prepareToEngageCheatResearchTech(810);
		return false;
	}
	//
	/*if (strstr(s, "/START"))
	{
		time_collect = true;
		return true;
	}
	if (strstr(s, "/STOP"))
	{
		time_collect = false;
		time_stage_find = true;
		return true;
	}
	if (strstr(s, "/YES"))
	{
		answer = true;
		answer_flag = true;
		return true;
	}
	if (strstr(s, "/NO"))
	{
		answer = false;
		answer_flag = true;
		return true;
	}*/

	return false;
}

__declspec(naked) void scanChat() //put on sub at 0x005ED970
{
	__asm
	{
		mov     eax, [esp + 8] //2nd argument of the function we are intercepting is pointer to the chat string
		push    ecx

		push    eax
		call    checkCheats
		pop     ecx
		sub     esp, 300h
		push    ebx
		push    edi
		test    eax, eax
		jnz     _no_chat
		push    005ED978h
		ret
_no_chat:
		push    005EDD65h
		ret
	}
}

extern float const_f_zero;

__declspec(naked) void checkPowerResource() //0054BE05
{
	__asm
	{
		mov     ecx, [esi + 18h]
		mov     ecx, [ecx + 0ACh]
		fld     dword ptr [ecx + 211 * 4]
		push    eax
		fcomp   const_f_zero
		fnstsw  ax
		test    ah, 41h
		pop     eax
		jz      always_powered
		mov     esi, [esi + 18h]
		shl     edi, 5
		mov     edx, 0054BE0Bh
		jmp     edx
always_powered:
		mov     edx, 0054BE36h
		jmp     edx
	}
}

void setAdvCheatHooks()
{
#ifdef _DEBUG
	log("Setting adv cheat hooks...");
#endif

	setHook((void*)0x005ED970, scanChat);

	setHook((void*)0x0054BE05, checkPowerResource);
}

sel_iterator::sel_iterator(void* player_)
{
	player = player_;
	index = 0;
}

sel_iterator& sel_iterator::operator++()
{
	if (index < *(int*)((int)player + 0x26C))
		index++;
	return *this;
}

sel_iterator& sel_iterator::operator=(const sel_iterator& i)
{
	index = i.index;
	return *this;
}

bool sel_iterator::operator==(const sel_iterator& i)
{
	return index == i.index;
}

bool sel_iterator::operator!=(const sel_iterator& i)
{
	return index != i.index;
}

UNIT* sel_iterator::operator*()
{
	if (index < *(int*)((int)player + 0x26C))
		return *(UNIT**)((int)player + 0x1C8 + index * 4);
	else
		return nullptr;
}
