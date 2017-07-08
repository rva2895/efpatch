#include "stdafx.h"

#include "memfunc.h"

void __cdecl setByte (int addr, unsigned char val)
{
	WriteProcessMemory (GetCurrentProcess (), (void*)addr, &val, 1, 0);
}

void __cdecl setInt (int addr, int val)
{
	WriteProcessMemory (GetCurrentProcess (), (void*)addr, &val, 4, 0);
}

void __cdecl setByteF (int addr, unsigned char val)
{
	WriteProcessMemory (GetCurrentProcess (), (void*)(addr+0x400000), &val, 1, 0);
}

void __cdecl setIntF (int addr, int val)
{
	WriteProcessMemory (GetCurrentProcess (), (void*)(addr+0x400000), &val, 4, 0);
}

const unsigned char push = 0x68;
const unsigned char ret = 0xC3;

void __cdecl setHook (void* addr, void* newAddr)
{
	unsigned long c;
#ifdef _DEBUG
	log ("Hook: 0x%X -> 0x%X", addr, newAddr);
#endif
	//WriteProcessMemory (GetCurrentProcess (), addr, &push, 1, &c);
	//WriteProcessMemory (GetCurrentProcess (), (char*)addr+1, &newAddr, 4, &c);
	//WriteProcessMemory (GetCurrentProcess (), (char*)addr+5, &ret, 1, &c);
	unsigned char j = 0xE9;
	WriteProcessMemory(GetCurrentProcess(), addr, &j, 1, &c);
	char* r = (char*)newAddr - (char*)addr + (char*)-5;
	WriteProcessMemory(GetCurrentProcess(), (char*)addr + 1, &r, 4, &c);
}

void __cdecl setHookRestorable (void* addr, void* newAddr, void* oldData)
{
	unsigned long c;
	ReadProcessMemory (GetCurrentProcess (), addr, oldData, 6, &c);
	setHook (addr, newAddr);
}

void __cdecl restoreHook (void* addr, void* oldData)
{
	unsigned long c;
	WriteProcessMemory (GetCurrentProcess (), addr, oldData, 6, &c);
}

int __cdecl readInt (int addr)
{
	int val;
	ReadProcessMemory (GetCurrentProcess (), (void*)addr, &val, 4, 0);
	return val;
}

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


/*__declspec(naked) char* __cdecl strncpy__ (char*, const char*, size_t)
{
	__asm
	{
		mov     eax, 00632DF0h
		jmp     eax
	}
}

__declspec(naked) char* __cdecl strstr__ (const char*, const char*)
{
	__asm
	{
		mov     eax, 00632F30h
		jmp     eax
	}
}

__declspec(naked) int __cdecl fclose__ (void*)
{
	__asm
	{
		mov     eax, 00632FB0h
		jmp     eax
	}
}

__declspec(naked) int __cdecl sscanf__ (const char*, const char*, ...)
{
	__asm
	{
		mov     eax, 00633006h
		jmp     eax
	}
}

__declspec(naked) void* __cdecl fopen__ (const char*, const char*)
{
	__asm
	{
		mov     eax, 006330B1h
		jmp     eax
	}
}

__declspec(naked) int __cdecl sprintf__ (char*, const char*, ...)
{
	__asm
	{
		mov     eax, 006330C4h
		jmp     eax
	}
}

__declspec(naked) void* __cdecl malloc__ (size_t)
{
	__asm
	{
		mov     eax, 00632B9Dh
		jmp     eax
	}
}

__declspec(naked) int __cdecl fscanf__ (void*, const char*, ...)
{
	__asm
	{
		mov     eax, 0063481Fh
		jmp     eax
	}
}

__declspec(naked) void* __cdecl memset__ (void*, int, size_t)
{
	__asm
	{
		mov     eax, 00634890h
		jmp     eax
	}
}

__declspec(naked) void* __cdecl memcpy__ (void*, const void*, size_t)
{
	__asm
	{
		mov     eax, 006348F0h
		jmp     eax
	}
}

__declspec(naked) char* __cdecl strcpy__(char*, const char*)
{
	__asm
	{
		mov     eax, 00635910h
		jmp     eax
	}
}

__declspec(naked) char* __cdecl strcat__ (char*, const char*)
{
	__asm
	{
		mov     eax, 00635920h
		jmp     eax
	}
}

__declspec(naked) int __cdecl strcmp__ (const char*, const char*)
{
	__asm
	{
		mov     eax, 00636460h
		jmp     eax
	}
}

__declspec(naked) size_t __cdecl strlen__ (const char*)
{
	__asm
	{
		mov     eax, 006364F0h
		jmp     eax
	}
}

__declspec(naked) char* __cdecl strupr__ (char*)
{
	__asm
	{
		mov     eax, 006404EFh
		jmp     eax
	}
}

__declspec(naked) void __cdecl free__ (void*)
{
	__asm
	{
		mov     eax, 00632CCAh
		jmp     eax
	}
}

__declspec(naked) void* __cdecl realloc__ (void*, size_t, size_t)
{
	__asm
	{
		jmp     realloc___
	}
}*/