#include "stdafx.h"

short* unconv;
int nUnconv;

__declspec(naked) void isUnconvertable() //ebx = ID
{
	__asm
	{
		mov     eax, nUnconv
		mov     edx, unconv
loopcont:
		test    eax, eax
		jz      loopend
		dec     eax
		cmp     word ptr [edx + eax*2], bx
		jnz     loopcont
		xor     eax, eax
		inc     eax
		ret
loopend:
		xor     eax, eax
		ret
	}
}

__declspec(naked) void unConvertHook() //00567EDC
{
	__asm
	{
		push    edx
		call    isUnconvertable
		pop     edx
		test    eax, eax
		jz      convertable
		mov     edx, 0056823Ch
		jmp     edx
convertable:
		mov     eax, 00567EF1h
		jmp     eax
	}
}

void setConvertHooks()
{
	log("Loading unconvertable unit list");
	FILE* f = fopen("data\\unconv.txt", "rt");
	if (f)
	{
		int id;
		nUnconv = 0;
		unconv = 0;

		while (fscanf(f, "%d", &id) > 0)
		{
			nUnconv++;
			unconv = (short*)realloc(unconv, nUnconv * sizeof(short));
			unconv[nUnconv - 1] = id;
		}

		fclose(f);

		setHook((void*)0x00567EDC, &unConvertHook);
	}
	else
		log("Warning: unconv.txt not found, using default settings");
}
