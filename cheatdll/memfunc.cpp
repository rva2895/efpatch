#include "stdafx.h"

#include "memfunc.h"

void __cdecl setByte(DWORD addr, BYTE val)
{
	WriteProcessMemory(GetCurrentProcess(), (void*)addr, &val, 1, 0);
}

void __cdecl setInt(DWORD addr, DWORD val)
{
	WriteProcessMemory(GetCurrentProcess(), (void*)addr, &val, 4, 0);
}

void __cdecl setByteF(DWORD addr, BYTE val)
{
	WriteProcessMemory(GetCurrentProcess(), (void*)(addr + 0x400000), &val, 1, 0);
}

void __cdecl setIntF(DWORD addr, DWORD val)
{
	WriteProcessMemory(GetCurrentProcess(), (void*)(addr + 0x400000), &val, 4, 0);
}

const unsigned char push = 0x68;
const unsigned char ret = 0xC3;

void __cdecl setHook(void* addr, void* newAddr)
{
	unsigned long c;
#ifdef _DEBUG
	log("Hook: 0x%X -> 0x%X", addr, newAddr);

	//m.insert(std::make_pair<int, int>((int)addr, 0));
#endif
	//WriteProcessMemory (GetCurrentProcess (), addr, &push, 1, &c);
	//WriteProcessMemory (GetCurrentProcess (), (char*)addr+1, &newAddr, 4, &c);
	//WriteProcessMemory (GetCurrentProcess (), (char*)addr+5, &ret, 1, &c);
	unsigned char j = 0xE9;
	WriteProcessMemory(GetCurrentProcess(), addr, &j, 1, &c);
	char* r = (char*)newAddr - (char*)addr + (char*)-5;
	WriteProcessMemory(GetCurrentProcess(), (char*)addr + 1, &r, 4, &c);
}

/*int __cdecl readInt(int addr)
{
	int val;
	ReadProcessMemory(GetCurrentProcess(), (void*)addr, &val, 4, 0);
	return val;
}*/

__declspec(naked) int __stdcall getWindowX()
{
	__asm
	{
		mov		eax, 006A3684h
		mov		eax, [eax]
		mov		eax, [eax + 24h]
		mov		eax, [eax + 8F4h]
		ret
	}
}

__declspec(naked) int __stdcall getWindowY()
{
	__asm
	{
		mov		eax, 006A3684h
		mov		eax, [eax]
		mov		eax, [eax + 24h]
		mov		eax, [eax + 8FCh]
		ret
	}
}
