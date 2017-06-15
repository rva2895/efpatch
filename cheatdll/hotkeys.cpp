#include "stdafx.h"

char hotkeys [] = {
	1,  7,  6, //monitor
	2,  8,  4, //interceptor
	3,  8,  5, //attacker
	4, 10,  4, //transport mech
	5, 18,  3, //vornskyr
	6,  5,  7, //aa battery
	7,  5,  8  //mine
};

int hotkeyTestRet;
int hotkeyTestCont;

__declspec(naked) void hotkeyTest () //00563010, 005625D0
{
	__asm
	{
		push    ecx
		mov     eax, [esp + 8]
		mov     byte ptr [esp + 3], 0
		cmp     eax, 7
		ja      _noHK
		test    eax, eax
		jz      _noHK
		lea     edx, [esp+3]
		push    edx
		lea     edx, [esp+3]
		push    edx
		lea     edx, [esp+3]
		push    edx
		dec     eax
		lea     eax, [eax + eax*2]
		mov     edx, offset hotkeys
		add     edx, eax
		add     edx, 2
		mov     al, byte ptr [edx]
		push    eax                 //id
		dec     edx
		mov     al, byte ptr [edx]
		push    eax                 //group
		
		mov     eax, hotkeyTestRet
		push    eax
		push    486B20h
		ret
_noHK:
		mov     edx, hotkeyTestCont
		push    edx
		ret
	}
}

__declspec(naked) void hotkeyTestUnit () //00563010
{
	__asm
	{
		mov     eax, 005631F0h
		mov     hotkeyTestRet, eax
		mov     eax, 0056301Ah
		mov     hotkeyTestCont, eax
		jmp     hotkeyTest
	}
}

__declspec(naked) void hotkeyTestBldg () //005625D0
{
	__asm
	{
		mov     eax, 00562620h
		mov     hotkeyTestRet, eax
		mov     eax, 005625DAh
		mov     hotkeyTestCont, eax
		jmp     hotkeyTest
	}
}

void setGroupNumbers ()
{
	setByte (0x00561462, 9);  //5
	setByte (0x00561478, 7);  //7
	setByte (0x00561483, 6);  //8
	setByte (0x00561499, 5);  //10
	setByte (0x005614F1, 4);  //18
}

__declspec(naked) void hotkeyOptionsLoad () //005625C1
{
	__asm
	{
		push    edi
		mov     edi, 486DA0h

		push    17499        //monitor
		push    6            //id
		push    7            //group
		mov     ecx, esi
		call    edi
		push    17508        //interceptor
		push    4            //id
		push    8            //group
		mov     ecx, esi
		call    edi
		push    10241        //attacker
		push    5            //id
		push    8            //group
		mov     ecx, esi
		call    edi
		push    17553        //transport mech
		push    4            //id
		push    10           //group
		mov     ecx, esi
		call    edi
		push    17482        //vornskyr
		push    3            //id
		push    18           //group
		mov     ecx, esi
		call    edi
		push    973          //aa battery
		push    7            //id
		push    5            //group
		mov     ecx, esi
		call    edi
		push    17563        //mine
		push    8            //id
		push    5            //group
		mov     ecx, esi
		call    edi

		pop     edi
		pop     esi
		ret     4
	}
}

__declspec(naked) void hotkeyDefaultSet () //00561C72
{
	__asm
	{
		push    edi
		mov     edi, 563BB0h

		push    2
		push    12h
		mov     ecx, esi
		call    edi

		push    6            //monitor
		push    7
		mov     ecx, esi
		call    edi
		
		push    4            //interceptor
		push    8
		mov     ecx, esi
		call    edi

		push    5            //attacker
		push    8
		mov     ecx, esi
		call    edi

		push    4            //transport mech
		push    10
		mov     ecx, esi
		call    edi

		push    7            //aa battery
		push    5
		mov     ecx, esi
		call    edi

		push    8            //mine
		push    5
		mov     ecx, esi
		call    edi

		push    3            //vornskr
		push    18
		mov     ecx, esi
		call    edi

		pop     edi
		pop     esi
		ret
	}
}

__declspec(naked) void hotkeyDefaultMonitor ()
{
	__asm
	{
		push    16378
		push    0
		push    0
		push    0
		push    45h
		push    6
		push    7
		mov     eax, 486BC0h
		call    eax
		retn    8
	}
}

__declspec(naked) void hotkeyDefaultInterceptor ()
{
	__asm
	{
		push    16379
		push    0
		push    0
		push    0
		push    57h
		push    4
		push    8
		mov     eax, 486BC0h
		call    eax
		retn    8
	}
}

__declspec(naked) void hotkeyDefaultAttacker ()
{
	__asm
	{
		push    16380
		push    0
		push    0
		push    0
		push    46h
		push    5
		push    8
		mov     eax, 486BC0h
		call    eax
		retn    8
	}
}

__declspec(naked) void hotkeyDefaultTransportMech ()
{
	__asm
	{
		push    16381
		push    0
		push    0
		push    0
		push    57h
		push    4
		push    10
		mov     eax, 486BC0h
		call    eax
		retn    8
	}
}

__declspec(naked) void hotkeyDefaultAABattery ()
{
	__asm
	{
		push    16382
		push    0
		push    0
		push    0
		push    52h
		push    7
		push    5
		mov     eax, 486BC0h
		call    eax
		retn    8
	}
}

__declspec(naked) void hotkeyDefaultMine ()
{
	__asm
	{
		push    16383
		push    0
		push    0
		push    0
		push    43h
		push    8
		push    5
		mov     eax, 486BC0h
		call    eax
		retn    8
	}
}

__declspec(naked) void hotkeyDefaultVornskr ()
{
	__asm
	{
		push    16384
		push    0
		push    0
		push    0
		push    57h
		push    3
		push    18
		mov     eax, 486BC0h
		call    eax
		retn    8
	}
}

int hotkeyDefaultsGroup5 [] = {  //build defense
	0x5644CA,
	0x5644E3,
	0x5644FC,
	0x564515,
	0x56452E,
	0x564547,
	0x564560,
	(int)&hotkeyDefaultAABattery,
	(int)&hotkeyDefaultMine
};

int hotkeyDefaultsGroup7 [] = {  //shipyard
	0x56461E,
	0x564637,
	0x564650,
	0x564669,
	0x564682,
	0x56469B,
	(int)&hotkeyDefaultMonitor
};

int hotkeyDefaultsGroup8 [] = {  //airbase
	0x5646C8,
	0x5646E1,
	0x5646FA,
	0x564713,
	(int)&hotkeyDefaultInterceptor,
	(int)&hotkeyDefaultAttacker
};

int hotkeyDefaultsGroup10 [] = {  //mech factory
	0x5647B8,
	0x5647D1,
	0x5647EA,
	0x564803,
	(int)&hotkeyDefaultTransportMech
};

__declspec(naked) void group18_fix () //00564ACF
{
	__asm
	{
		sub     eax, 0
		jz      _id_0
		dec     eax
		jz      _id_1
		dec     eax
		jz      _id_2
		dec     eax
		jnz     _no_id
		jmp     hotkeyDefaultVornskr
_id_0:
		mov     eax, 564B0Ch
		jmp     eax
_id_1:
		mov     eax, 564AF3h
		jmp     eax
_id_2:
		mov     eax, 564ADAh
		jmp     eax
_no_id:
		ret     8
	}
}

void setHotkeyHooks ()
{
	setGroupNumbers ();

	setHook ((void*)0x005625C1, &hotkeyOptionsLoad);
	setHook ((void*)0x00563010, &hotkeyTestUnit);
	setHook ((void*)0x005625D0, &hotkeyTestBldg);

	setHook ((void*)0x00561C72, &hotkeyDefaultSet);

	setByte (0x005644BC, 8);
	setInt (0x005644C6, (int)hotkeyDefaultsGroup5);

	setByte (0x00564610, 6);
	setInt (0x0056461A, (int)hotkeyDefaultsGroup7);

	setByte (0x005646BA, 5);
	setInt (0x005646C4, (int)hotkeyDefaultsGroup8);
	
	setByte (0x005647AA, 4);
	setInt (0x005647B4, (int)hotkeyDefaultsGroup10);
	
	setHook ((void*)0x00564ACF, &group18_fix);
}
