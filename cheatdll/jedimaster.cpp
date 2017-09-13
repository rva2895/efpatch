#include "stdafx.h"

#include "jedimaster.h"

short* masters;
int nMasters;

__declspec(naked) void isMaster () //ax = ID
{
	__asm
	{
		push    ecx
		push    edx
		mov     ecx, nMasters
		mov     edx, masters
loopcont:
		test    ecx, ecx
		jz      loopend
		dec     ecx
		cmp     word ptr [edx + ecx*2], ax
		jnz     loopcont
		xor		eax, eax
		inc		eax
		pop     edx
		pop     ecx
		ret
loopend:
		xor		eax, eax
		pop     edx
		pop     ecx
		ret
	}
}

//int calledNtimes = 0;

__declspec(naked) void jediMasterHook () //0054B1D3
{
	//calledNtimes++;
	//log ("jedi master called %d times", calledNtimes);
	__asm
	{
		mov     ax, [ecx + 1Eh]
		cmp     ax, 32h
		jnz     nonjedi
		mov     ax, [ecx + 18h]
		call    isMaster
		ret
nonjedi:
		xor     eax, eax
		ret
	}
}

void setJediMasterHooks()
{
	log("Loading jedi master unit list");
	FILE* f = fopen("data\\master.txt", "rt");
	if (f)
	{
		int id;
		nMasters = 0;
		masters = 0;

		while (fscanf(f, "%d", &id) > 0)
		{
			nMasters++;
			masters = (short*)realloc(masters, nMasters * sizeof(short));
			masters[nMasters - 1] = id;
		}

		fclose(f);

		setHook((void*)0x0054B1D3, &jediMasterHook);
	}
	else
		log("Warning: master.txt not found, using default settings");
}
