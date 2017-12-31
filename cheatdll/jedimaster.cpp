#include "stdafx.h"

#include "jedimaster.h"

short* masters;
int nMasters;

short* padawans;
int nPadawans;

__declspec(naked) void isMaster() //ax = ID
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
		cmp     word ptr [edx + ecx * 2], ax
		jnz     loopcont
		xor     eax, eax
		inc     eax
		pop     edx
		pop     ecx
		ret
loopend:
		xor     eax, eax
		pop     edx
		pop     ecx
		ret
	}
}

__declspec(naked) void isPadawan() //ax = ID
{
	__asm
	{
		push    ecx
		push    edx
		mov     ecx, nPadawans
		mov     edx, padawans
loopcont_p:
		test    ecx, ecx
		jz      loopend_p
		dec     ecx
		cmp     word ptr [edx + ecx * 2], ax
		jnz     loopcont_p
		xor     eax, eax
		inc     eax
		pop     edx
		pop     ecx
		ret
loopend_p:
		xor     eax, eax
		pop     edx
		pop     ecx
		ret
	}
}

__declspec(naked) void jediMasterHook () //0054B1D0
{
	__asm
	{
		mov     ecx, [ecx + 14h]
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

__declspec(naked) void jediPadawanHook() //0054B170
{
	__asm
	{
		mov     ecx, [ecx + 14h]
		mov     ax, [ecx + 1Eh]
		cmp     ax, 32h
		jnz     nonjedi_p
		mov     ax, [ecx + 1Ah]
		call    isPadawan
		ret
nonjedi_p:
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

		setHook((void*)0x0054B1D0, &jediMasterHook);
	}
	else
		log("Warning: master.txt not found, using default settings");

	log("Loading jedi padawan unit list");
	f = fopen("data\\padawan.txt", "rt");
	if (f)
	{
		int id;
		nPadawans = 0;
		padawans = 0;

		while (fscanf(f, "%d", &id) > 0)
		{
			nPadawans++;
			padawans = (short*)realloc(padawans, nPadawans * sizeof(short));
			padawans[nPadawans - 1] = id;
		}

		fclose(f);

		setHook((void*)0x0054B170, &jediPadawanHook);
	}
	else
		log("Warning: padawan.txt not found, using default settings");
}
