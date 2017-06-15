#include "stdafx.h"

#include "population.h"

__declspec(naked) int createPopList ()
{
	__asm
	{
		mov     edi, 5h
		//imul    ecx, edi, 5h
		lea     ecx, [edi + edi*4]
		push    ecx
		//push    ebx
		//push    esi
		//push    edi
	//}
	//log ("Creating population caps list");
	//__asm
	//{
		//pop     edi
		//pop     esi
		//pop     ebx
		push    00520279h
		ret
	}
}

__declspec(naked) int popListContCreation ()
{
	__asm
	{
		jle     createPopListCont

		_emit   0x8B
		_emit   0x0D
		_emit   0x84
		_emit   0x36
		_emit   0x6A
		_emit   0x00
		push    005202ADh
		ret

createPopListCont:
		//imul    ecx, edi, 05h
		lea     ecx, [edi + edi*4]
		push    ecx
		push    00520279h
		ret
	}
}

__declspec(naked) int popFunc1 ()
{
	__asm
	{
		and     eax, 0FFh
		//imul    eax, eax, 05h
		lea     eax, [eax + eax*4]
		mov     ecx, ebp
		push    0051E594h
		ret
	}
}

__declspec(naked) int popFunc2 ()
{
	__asm
	{
		and     eax, 0FFh
		//imul    eax, eax, 05h
		lea     eax, [eax + eax*4]
		push    eax
		push    0060136Bh
		ret
	}
}

__declspec(naked) int popFuncMain ()
{
	__asm
	{
		mov     edx, [ebp+4Ch]
		and     eax, 0FFh
		//imul    eax, eax, 05h
		lea     eax, [eax + eax*4]
		push    00602711h
		ret
	}
}

char aPopCap275 [] = "POPULATION-CAP-275";
char aPopCap300 [] = "POPULATION-CAP-300";
char aPopCapEx  [] = "POPULATION-CAP-EX";

__declspec(naked) void popCap275 ()
{
	__asm
	{
		push    offset aPopCap275
		push    0057F197h
		ret
	}
}

__declspec(naked) void popCap300 ()
{
	__asm
	{
		push    offset aPopCap300
		push    0057F197h
		ret
	}
}

__declspec(naked) void popCapEx ()
{
	__asm
	{
		push    offset aPopCapEx
		push    0057F197h
		ret
	}
}

int popCapJmpTable [] =
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

__declspec(naked) int badPopAiFix ()
{
	__asm
	{
		and     eax, 0FFh
		cmp     eax, 25
		jnb     scenCap
		xor     edx, edx
		mov     ecx, 5
		div     ecx
		cmp     eax, 40
		ja      badPopCap
		cmp     eax, 12
		ja      extendedPopCap
		mov     edx, offset popCapJmpTable
		jmp     ds:popCapJmpTable[eax*4]
badPopCap:
		push    0057F1A2h
		ret
extendedPopCap:
		//jmp     popCapEx
		push    0057F192h
		ret
scenCap:
		xor     edx, edx
		mov     ecx, 25
		div     ecx
		mov     edx, offset popCapJmpTable
		jmp     ds:popCapJmpTable[eax*4]
	}
}

SET_HOOK_ASSIGN popHooks [] =
{
	{(void*)0x00520273, &createPopList},
	{(void*)0x005202A5, &popListContCreation},
	{(void*)0x0051E58D, &popFunc1},
	{(void*)0x00601365, &popFunc2},
	{(void*)0x00602709, &popFuncMain},
	{(void*)0x0057F135, &badPopAiFix}
};

float f1000 = 1000.0;

void setPopulationHooks ()
{
#ifdef _DEBUG
	log ("Setting population cap hooks...");
#endif
	for (int i = 0; i < (sizeof(popHooks)/sizeof(popHooks[0])); i++)
		setHook (popHooks[i].addr, popHooks[i].newAddr);

	//print new pop steps
	setByte (0x0052029E, 5);
	setByte (0x005202A1, 0xC8); //1000 / 5 = 200
	setByte (0x005202A2, 0x00);

	//editor pop limit
	setInt(0x005CF063, (int)&f1000);
	setInt(0x005CF074, *(int*)&f1000);
}
