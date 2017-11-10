#include "stdafx.h"

#include "population.h"

__declspec(naked) int createPopList()
{
	__asm
	{
		mov     edi, 5h
		//imul    ecx, edi, 5h
		lea     ecx, [edi + edi * 4]
		push    ecx
		mov     eax, 00520279h
		jmp     eax
	}
}

__declspec(naked) int popListContCreation()
{
	__asm
	{
		jle     createPopListCont

		_emit	0x8B
		_emit	0x0D
		_emit	0x84
		_emit	0x36
		_emit	0x6A
		_emit	0x00
		mov     edx, 005202ADh
		jmp     edx

createPopListCont:
		//imul    ecx, edi, 05h
		lea     ecx, [edi + edi * 4]
		push    ecx
		mov     eax, 00520279h
		jmp     eax
	}
}

float f1000 = 1000.0;

__declspec(naked) void getPopCap_new() //005EF1B0
{
	__asm
	{
		xor     eax, eax
		mov     al, [ecx + 1446h]
		lea     eax, [eax + eax * 4]
		ret
	}
}

__declspec(naked) void getPopCap_fix1() //005202AD
{
	__asm
	{

		xor     eax, eax
		mov     al, [ecx + 1446h]
		mov     ecx, [esi + 0B28h]
		push    005202BDh
		ret
	}
}

__declspec(naked) void setPopCap_new() //005EF240
{
	__asm
	{
		mov     al, [esp + 4]
		mov     [ecx + 1446h], al
		ret     4
	}
}

char* __fastcall getPopCapString(int c)
{
	c /= 25;
	switch (c)
	{
	case 1: //25
		return (char*)0x00692E5C;
	case 2: //50
		return (char*)0x00692E48;
	case 3: //75
		return (char*)0x00692E34;
	case 4: //100
		return (char*)0x00692E20;
	case 5: //125
		return (char*)0x00692E0C;
	case 6: //150
		return (char*)0x00692DF8;
	case 7: //175
		return (char*)0x00692DE4;
	case 8: //200
		return (char*)0x00692DD0;
	case 9: //225
		return (char*)0x00692DBC;
	case 10: //250
		return (char*)0x00692DA8;
	default:
		return (char*)0x00692DA8;
	}
}

__declspec(naked) void onAIPopCap() //0057F13A
{
	__asm
	{
		mov     ecx, eax
		call    getPopCapString
		push    eax
		push    0057F197h
		ret
	}
}

void setPopulationHooks()
{
#ifdef _DEBUG
	log("Setting population cap hooks...");
#endif
	setHook((void*)0x00520273, createPopList);
	setHook((void*)0x005202A5, popListContCreation);
	setHook((void*)0x0057F13A, onAIPopCap);

	setHook((void*)0x005EF1B0, getPopCap_new);
	setHook((void*)0x005EF240, setPopCap_new);
	setHook((void*)0x005202AD, getPopCap_fix1);

	//fix 75 -> 15 (75/5)
	setByte(0x004F1ADE, 15);
	setByte(0x00508B8E, 15);
	setByte(0x00520846, 15);
	setByte(0x005E424E, 15);
	setByte(0x005EC382, 15);


	//getPopCap and eax, 0FFh
	//setInt(0x005202B9, 0xFFFFFFFF); //useless
	setInt(0x004412D8, 0xFFFFFFFF);
	setInt(0x0051E58E, 0xFFFFFFFF);
	setInt(0x00541D47, 0xFFFFFFFF);
	setInt(0x0057F136, 0xFFFFFFFF);
	setInt(0x005FECBA, 0xFFFFFFFF);
	setInt(0x00601366, 0xFFFFFFFF);
	setInt(0x0060270D, 0xFFFFFFFF);
	setInt(0x006031D1, 0xFFFFFFFF);

	//print new pop steps
	setByte(0x0052029E, 5);
	setByte(0x005202A1, 0xC8); //1000 / 5 = 200
	setByte(0x005202A2, 0x00);

	//editor pop limit
	setInt(0x005CF063, (int)&f1000);
	setInt(0x005CF074, *(int*)&f1000);
}
