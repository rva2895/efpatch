#include "stdafx.h"

#include "resgenbldgs.h"

int numberOfResProducers;
resGen* resProducersData;

void initBldgResProdList ()
{
	char c;
	int value;
	int id;
	int res;
	log ("Loading resource generating buildings list");
	FILE* f = fopen ("data\\resgen.txt", "rt");
	if (f)
	{
		numberOfResProducers = 0;
		resProducersData = 0;
		while (int ss = fscanf (f, "%d %d %c %d", &id, &res, &c, &value) > 0)
		{
#ifdef _DEBUG
			log ("fscanf: %d, val = %d, %d, %c, %d", ss, id, res, c, value);
#endif
			numberOfResProducers++;
			resProducersData = (resGen*) realloc (resProducersData, numberOfResProducers*sizeof(resGen));
			resProducersData[numberOfResProducers-1].unitID = id;
			resProducersData[numberOfResProducers-1].resID = res;
			if (c == 'f')
			{
				resProducersData[numberOfResProducers-1].useControlRes = 0;
				resProducersData[numberOfResProducers-1].constantResAmount = value;
			}
			else
			{
				resProducersData[numberOfResProducers-1].useControlRes = 1;
				resProducersData[numberOfResProducers-1].controlResID = value;
			}
		}

		setHook ((void*)0x005553C6, &resGenHook);
	}
	else
		log ("Warning: resgen.txt not found, resource generating buildings disabled");
}

void __stdcall doResGen (int unitID, float* resources)
{
	for (int i = 0; i < numberOfResProducers; i++)
	{
		if (unitID == resProducersData[i].unitID)
		{
			if (resProducersData[i].useControlRes)
				resources[resProducersData[i].resID] += (resources[resProducersData[i].controlResID] / 60);
			else
				resources[resProducersData[i].resID] += ((float)resProducersData[i].constantResAmount / 60);
		}
	}
}

__declspec(naked) int resGenHook ()
{
	__asm
	{
		push    esi
		push    ecx
		mov     eax, [ecx+0ACh]
		push    eax //resources
		mov     eax, [esi+14h]
		xor     ecx, ecx
		mov     cx, word ptr [eax+18h]
		push    ecx //unit id
			
		call    doResGen

		pop     ecx
		pop     esi
		mov     eax, [esi+14h];
		cmp     word ptr [eax+18h], 68h
		push    005553CEh
		ret
	}
}

void setResGenHooks ()
{
	initBldgResProdList ();
}
