#include "stdafx.h"

#include "population.h"

__declspec(naked) int createPopList()
{
	__asm
	{
		mov		edi, 5h
		//imul    ecx, edi, 5h
		lea		ecx, [edi + edi * 4]
		push	ecx
		mov		eax, 00520279h
		jmp		eax
	}
}

__declspec(naked) int popListContCreation()
{
	__asm
	{
		jle		createPopListCont

		_emit	0x8B
		_emit	0x0D
		_emit	0x84
		_emit	0x36
		_emit	0x6A
		_emit	0x00
		mov		edx, 005202ADh
		jmp		edx

createPopListCont:
		//imul    ecx, edi, 05h
		lea		ecx, [edi + edi * 4]
		push	ecx
		mov		eax, 00520279h
		jmp		eax
	}
}

__declspec(naked) int popFunc1()
{
	__asm
	{
		and		eax, 0FFh
		//imul    eax, eax, 05h
		lea		eax, [eax + eax * 4]
		mov		ecx, ebp
		mov		edx, 0051E594h
		jmp		edx
	}
}

__declspec(naked) int popFunc2()
{
	__asm
	{
		and		eax, 0FFh
		//imul    eax, eax, 05h
		lea		eax, [eax + eax * 4]
		push	eax
		mov		edx, 0060136Bh
		jmp		edx
	}
}

__declspec(naked) int popFuncMain()
{
	__asm
	{
		mov		edx, [ebp + 4Ch]
		and		eax, 0FFh
		//imul    eax, eax, 05h
		lea		eax, [eax + eax * 4]
		mov		[esp + 20h], eax
		mov		eax, 00602715h
		jmp		eax
	}
}

char aPopCap275[] = "POPULATION-CAP-275";
char aPopCap300[] = "POPULATION-CAP-300";
char aPopCapEx[] = "POPULATION-CAP-EX";

__declspec(naked) void popCap275()
{
	__asm
	{
		push	offset aPopCap275
		mov		ecx, 0057F197h
		jmp		ecx
	}
}

__declspec(naked) void popCap300()
{
	__asm
	{
		push	offset aPopCap300
		mov		ecx, 0057F197h
		jmp		ecx
	}
}

__declspec(naked) void popCapEx()
{
	__asm
	{
		push	offset aPopCapEx
		mov		ecx, 0057F197h
		jmp		ecx
	}
}

int popCapJmpTable[] =
{
	0,
	0x0057F153,
	0x0057F15A,
	0x0057F161,
	0x0057F168,
	0x0057F16F,
	0x0057F176,
	0x0057F17D,
	0x0057F184,
	0x0057F18B,
	0x0057F192,
	0x0057F192, //(int)&popCap275,
	0x0057F192  //(int)&popCap300
};

extern bool isEditor;

__declspec(naked) int badPopAiFix()
{
	__asm
	{
		and		eax, 0FFh
		mov		dl, isEditor
		test	dl, dl
		jnz		scenCap
		xor		edx, edx
		mov		ecx, 5
		div		ecx
		cmp		eax, 40
		ja		badPopCap
		cmp		eax, 12
		ja		extendedPopCap
		mov		edx, offset popCapJmpTable
		jmp		ds:popCapJmpTable[eax * 4]
badPopCap:
		mov		eax, 0057F1A2h
		jmp		eax
extendedPopCap:
		//jmp     popCapEx
		mov		ecx, 0057F192h
		jmp		ecx
scenCap:
		cmp		eax, 75
		jnz		test_sc
		xor		edx, edx
		mov		ecx, 25
		div		ecx
		mov		edx, offset popCapJmpTable
		jmp		ds:popCapJmpTable[eax * 4]
test_sc:
		int		3
	}
}

float f1000 = 1000.0;

void setPopulationHooks()
{
#ifdef _DEBUG
	log("Setting population cap hooks...");
#endif
	setHook((void*)0x00520273, &createPopList);
	setHook((void*)0x005202A5, &popListContCreation);
	setHook((void*)0x0051E58D, &popFunc1);
	setHook((void*)0x00601365, &popFunc2);
	setHook((void*)0x00602709, &popFuncMain);
	setHook((void*)0x0057F135, &badPopAiFix);

	//print new pop steps
	setByte(0x0052029E, 5);
	setByte(0x005202A1, 0xC8); //1000 / 5 = 200
	setByte(0x005202A2, 0x00);

	//editor pop limit
	setInt(0x005CF063, (int)&f1000);
	setInt(0x005CF074, *(int*)&f1000);
}
