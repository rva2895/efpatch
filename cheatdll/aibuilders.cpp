#include "stdafx.h"

#include "ai.h"
#include "aibuilders.h"

int buildersCntr;
int buildPtrFirst;

__declspec(naked) void cntrSet() //00413870
{
	__asm
	{
		xor     eax, eax
		mov     buildersCntr, eax
		sub     esp, 8
		push    ebx
		push    ebp
		push    esi
		mov     esi, ecx
		mov     eax, 00413878h
		jmp     eax
	}
}

__declspec(naked) void checkCntr() //00413ACE
{
	__asm
	{
		mov     eax, buildersCntr
		test    eax, eax
		jz      _cntrEnd;
		pop     edi
		mov     eax, buildPtrFirst
		pop     esi
		pop     ebp
		pop     ebx
		add     esp, 8
		retn    28h
_cntrEnd:
		pop     edi
		pop     esi
		pop     ebp
		xor     eax, eax
		pop     ebx
		add     esp, 8
		retn    28h
	}
}

__declspec(naked) void cntrContinue() //00413ADA
{
	__asm
	{
		mov     eax, buildersCntr;
		test    eax, eax
		jnz     _cntrNotFirst
		mov     buildPtrFirst, ebp
_cntrNotFirst:
		inc     eax
		mov     buildersCntr, eax
		mov     ecx, 00413AADh
		jmp     ecx
	}
}

__declspec(naked) void queryCntr() //0059F3EF
{
	__asm
	{
		//test    eax, eax
		//jz      _skipCntr
		push    eax
		movsx   ecx, word ptr [ecx + 18h]
		mov     edx, [edi + 18h]
		call    getUnitTypeBuilders
		mov     ecx, buildersCntr
		cmp     ecx, eax
		pop     eax
		jge     _noBuilders
		//_skipCntr:
		//test    eax,eax

		test    bl, bl
		jz      _builders
		cmp     dword ptr [esp + 1Ch], 2
		jge     _noBuilders
_builders:
		mov     eax, 0059F3FEh
		jmp     eax
_noBuilders:
		mov     eax, 0059F435h
		jmp     eax
	}
}

void setAIBuildersCntrHooks()
{
#ifdef _DEBUG
	log("Setting AI builders hooks...");
#endif

	setHook((void*)0x00413870, cntrSet);
	setHook((void*)0x00413ACE, checkCntr);
	setHook((void*)0x00413ADA, cntrContinue);
	setHook((void*)0x0059F3EF, queryCntr);
}
