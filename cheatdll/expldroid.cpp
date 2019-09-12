#include "stdafx.h"

#include "expldroid.h"

void __cdecl isExpl();

void expl1();
void expl2();
void expl3();
void expl4();

short* explIDs;
int nExplIDs;

#pragma optimize( "s", on )
void expl_droid_hooks()
{
	setHook((void*)0x0040215F, &expl1);
	setHook((void*)0x0055A881, &expl2);
	setHook((void*)0x0055BE40, &expl3);
	setHook((void*)0x005B6DF2, &expl4);
}
#pragma optimize( "", on )

void initExplDroid()
{
	log("Loading suicide attack unit list");
	FILE* f = fopen("data\\expl.txt", "rt");
	if (f)
	{
		short id;
		nExplIDs = 0;
		explIDs = 0;

		while (fscanf(f, "%hd", &id) > 0)
		{
			nExplIDs++;
			explIDs = (short*)realloc(explIDs, nExplIDs * sizeof(short));
			explIDs[nExplIDs - 1] = id;
		}

		fclose(f);

		expl_droid_hooks();
	}
	else
		log("Warning: expl.txt not found, using default settings");
}

__declspec(naked) void isExpl() //bx = ID
{
	__asm
	{
		push    ecx
		push    edx
		mov     ecx, nExplIDs
		mov     edx, explIDs
		xor     eax, eax
loopcont:
		test    ecx, ecx
		jz      loopend
		dec     ecx
		cmp     word ptr [edx + ecx * 2], bx
		jnz     loopcont
		inc     eax
loopend:
		pop     edx
		pop     ecx
		ret
	}
}

__declspec(naked) void expl1() //0040215F
{
	__asm
	{
		push    ebx
		push    eax
		mov     bx, word ptr [eax + 18h]
		call    isExpl
		cmp     eax, 1
		pop     eax
		pop     ebx
		push    00402165h
		ret
	}
}

__declspec(naked) void expl2() //0055A881
{
	__asm
	{
		push    ebx
		push    eax
		mov     bx, word ptr [ecx + 18h]
		call    isExpl
		cmp     eax, 1
		pop     eax
		pop     ebx
		push    0055A887h
		ret
	}
}

__declspec(naked) void expl3() //0055BE40
{
	__asm
	{
		push    ebx
		push    eax
		mov     bx, word ptr [ebp + 18h]
		call    isExpl
		cmp     eax, 1
		pop     eax
		pop     ebx
		push    0055BE46h
		ret
	}
}

__declspec(naked) void expl4() //005B6DF2
{
	__asm
	{
		push    ebx
		push    eax
		mov     bx, word ptr [eax + 18h]
		call    isExpl
		cmp     eax, 1
		pop     eax
		pop     ebx
		push    005B6DF8h
		ret
	}
}
