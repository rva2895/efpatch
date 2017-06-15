#include "stdafx.h"

#include "advtriggereffect.h"
#include "editpropertyobject.h"
#include "effectUnitVar.h"

__declspec(naked) int triggerTableLoad ()
{
	__asm
	{
		mov     ebx, [edx+04Ch]
		//mov     [ebx+4], al
		//mov     [ebx+6], al
		//mov     [ebx+7], al
		//mov     [ebx+10h], al
		//mov     [ebx+11h], al
		//mov     [ebx+12h], al
		//mov     [ebx+13h], al
		//mov     [ebx+14h], al
		//mov     [ebx+15h], al

		mov     ebx, [edx+8h]
		mov     [ebx+1h], al

		mov     ebx, [edx+0B4h]
		//mov     [ebx+4], al
		//mov     [ebx+6], al
		//mov     [ebx+7], al
		//mov     [ebx+9], al
		//mov     [ebx+10h], al
		//mov     [ebx+11h], al
		//mov     [ebx+12h], al
		//mov     [ebx+13h], al
		//mov     [ebx+14h], al
		mov     [ebx+15h], al

		mov     eax, [esi+8]
		mov     cl, 1
		push    eax
		push    ecx
		push    ebx
		push    esi
		push    edi
	}
#ifdef _DEBUG
	log ("Editing trigger load table");
#endif
	__asm
	{
		pop     edi
		pop     esi
		pop     ebx
		pop     ecx
		pop     eax
		push    005F5580h
		ret
	}
}

__declspec(naked) void triggerInputTableHook ()
{
	__asm
	{
		mov     [ebx+6], al
		mov     [ebx+7], al
		mov     [ebx+0Ah], al

		mov     ebx, [edx+0B0h]       //explore effect
		mov     [ebx+7], al           //player
		mov     [ebx+10h], al         //area
		mov     [ebx+11h], al
		mov     [ebx+12h], al
		mov     [ebx+13h], al

		mov     ebx, [edx+0B4h]       //unit var effect
		mov     [ebx+4], al           //obj
		mov     [ebx+5], al
		mov     [ebx+6], al           //obj list
		mov     [ebx+7], al           //player
		mov     [ebx+0Ah], al         //message
		mov     [ebx+10h], al         //area
		mov     [ebx+11h], al
		mov     [ebx+12h], al
		mov     [ebx+13h], al
		mov     [ebx+14h], al         //obj group
		mov     [ebx+15h], al         //obj type

		mov		ebx, [edx + 78h]	  //snap view
		mov		[ebx + 0Eh], al		  //x
		mov		[ebx + 0Fh], al		  //y

		push	007B2ADDh
		ret
	}
}

char aChangeGlobalUnit [] = "Change Unit Property Object";
char aExplore [] = "Explore Area";
char aUnitVar [] = "Change Unit Variable";

__declspec(naked) void triggerDisplayHook ()
{
	__asm
	{
		mov     ecx, [edi+0E24h]
		push    2Bh
		push    offset aChangeGlobalUnit
		mov     eax, 4C82A0h
		call    eax

		mov     ecx, [edi+0E24h]
		push    2Ch
		push    offset aExplore
		mov     eax, 4C82A0h
		call    eax

		mov     ecx, [edi+0E24h]
		push    2Dh
		push    offset aUnitVar
		mov     eax, 4C82A0h
		call    eax

		push    007B23ACh
		ret
	}
}//2B02FC

__declspec(naked) void effectExploreArea ()
{
	__asm
	{
		mov     ebp, 7A1CBCh
		mov     edx, [edi+50h]    //y1
		mov     esi, [edi+58h]    //y2
		//sub     ebx, edx          //rect x size - 1

		shl     edx, 2
		shl     esi, 2
		add     edx, ebp          //y1 coloumn, x1 to x2
		add     esi, ebp          //y2 coloumn, x1 to x2
		//mov     esi, edx

x_cont:
		//edx: current coloumn
		mov     ebp, [edx]

		mov     eax, [edi+4Ch]
		//mov     eax, [esi+ecx]
y_cont:
		mov     ecx, [edi+28h]
		mov     ebx, 10000h
		shl     ebx, cl
		or      [ebp+eax*4], ebx
		inc     eax
		cmp     eax, [edi+54h]
		jle     y_cont


		cmp     edx, esi
		jge     x_end
		add     edx, 4
		jmp     x_cont
x_end:


		push    005F3DB1h
		ret
	}
}

__declspec(naked) void effectUnitVar ()
{
	__asm
	{
		mov     eax, [esp+10h]       // ; jumptable 005F2B53 case 23
		xor     ebp, ebp
		cmp     eax, ebx
		jle     endLoc                     // ; jumptable 005F2B53 default case
		lea     esi, [esp+134h]
		
		loc_5F3AAD:                             // CODE XREF: ProcessTriggerEffect+1061j

		mov     eax, [edi+6Ch]
		push    eax
		mov     eax, [esi]
		push    eax
		call    effectUnitVarActual


		//loc_5F3B37:                            // ; CODE XREF: ProcessTriggerEffect+FD1j
		                                       // ; ProcessTriggerEffect+1045j ...
		mov     eax, [esp+10h]
		inc     ebp
		add     esi, 4
		cmp     ebp, eax
		jl      loc_5F3AAD

endLoc:
		push    005F3DB1h
		ret
	}
}

void setEffectHooks ()
{
	int adrBuf = ((int)&advTriggerEffect - 0x7B3199);

	setByte(0x007B3188, 0x90); //change unit data - remove type check
	setByte(0x007B3189, 0x90);

	int adrChangePropertyObjectEffect = (int)&changePropertyObjectHook;
	//int adrSetVarEffect = (int)&setVarEffect;

	//int patrolAddr = (int)&patrol;

	setHook ((void*)0x007B2ADD, &triggerTableLoad);

	setByte (0x0048BB21, 3);

	setHook ((void*)0x007B2A9B, &triggerInputTableHook);
	setHook ((void*)0x007B2388, &triggerDisplayHook);

	//snap view
	setByte(0x005F5B10, 0x42);
	setByte(0x005F5B19, 0x42);

	//setHook ((void*)0x007B2ABF, &setVarHook);

	setByte (0x005F2B4C, 0x2E); //effect count, old = 2d
	//setByte (0x0053BD37, 0x28);

	setByte (0x005F5575, 0xBC);
	setByte (0x005F550C, 0xBC);

	setByte (0x005F53AF, 0xB4);

	WriteProcessMemory (GetCurrentProcess (), (void*)0x7B22F8, &adrChangePropertyObjectEffect, 4, 0); //changepropobj effect
	//WriteProcessMemory (GetCurrentProcess (), (void*)0x7B22FC, &adrSetVarEffect, 4, 0); //setvar effect

	WriteProcessMemory (GetCurrentProcess (), (void*)0x7B3195, &adrBuf, 4, 0); //wtf???

	setInt (0x7B22FC, (int)&effectExploreArea);
	setInt (0x7B2300, (int)&effectUnitVar);

	//WriteProcessMemory (GetCurrentProcess (), (void*)(0x7B2250+(18*4)), &patrolAddr, 4, 0); //new patrol effect addr
}
