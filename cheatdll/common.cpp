#include "stdafx.h"

#include "common.h"

#ifdef TARGET_VOOBLY
extern IVoobly *g_pVoobly;
#endif

void __cdecl writeByte(DWORD addr, BYTE val)
{
#ifndef TARGET_VOOBLY
	WriteProcessMemory(GetCurrentProcess(), (void*)addr, &val, 1, 0);
#else
	g_pVoobly->Write(addr, &val, sizeof(BYTE));
#endif
}

void __cdecl writeWord(DWORD addr, WORD val)
{
#ifndef TARGET_VOOBLY
	WriteProcessMemory(GetCurrentProcess(), (void*)addr, &val, 2, 0);
#else
	g_pVoobly->Write(addr, &val, sizeof(WORD));
#endif
}

void __cdecl writeDword(DWORD addr, DWORD val)
{
#ifndef TARGET_VOOBLY
	WriteProcessMemory(GetCurrentProcess(), (void*)addr, &val, 4, 0);
#else
	g_pVoobly->Write(addr, val);
#endif
}

void __cdecl writeByteF(DWORD addr, BYTE val)
{
#ifndef TARGET_VOOBLY
	WriteProcessMemory(GetCurrentProcess(), (void*)(addr + 0x400000), &val, 1, 0);
#else
	g_pVoobly->Write(addr + 0x400000, &val, sizeof(BYTE));
#endif
}

void __cdecl writeDwordF(DWORD addr, DWORD val)
{
#ifndef TARGET_VOOBLY
	WriteProcessMemory(GetCurrentProcess(), (void*)(addr + 0x400000), &val, 4, 0);
#else
	g_pVoobly->Write(addr + 0x400000, val);
#endif
}

void __cdecl writeData(DWORD addr, void* data, int len)
{
#ifndef TARGET_VOOBLY
	WriteProcessMemory(GetCurrentProcess(), (void*)addr, data, len, 0);
#else
	g_pVoobly->Write(addr, data, len);
#endif
}

//const unsigned char push = 0x68;
//const unsigned char ret = 0xC3;

void __cdecl setHook(void* addr, void* newAddr)
{
#ifndef TARGET_VOOBLY
	unsigned long c;
	unsigned char j = 0xE9;
	WriteProcessMemory(GetCurrentProcess(), addr, &j, 1, &c);
	char* r = (char*)newAddr - (char*)addr + (char*)-5;
	WriteProcessMemory(GetCurrentProcess(), (char*)addr + 1, &r, 4, &c);
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

void* (__thiscall* global_getCurrentPlayer) (void* globalPtr) =
	(void* (__thiscall*) (void*))0x00428750;

void (__thiscall* player_clearSelection) (void* player) =
	(void(__thiscall*) (void*))0x004C3050;

int (__thiscall* player_scrollView)(void *player, float x, float y, int unk) =
	(int(__thiscall*) (void*, float, float, int))0x004C2010;

void* __stdcall getCurrentPlayer()
{
	return global_getCurrentPlayer(*(void**)0x006A3684);
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
		push    eax			//player
		push    edx			//str
		push    0			//int
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
