#include "stdafx.h"

#include "civletter.h"

BYTE_ASSIGN civLetterSet[] =
{
	{0x0053A543, 0xBE},            //new offset of (E), (N) etc
	{0x0053A544, 0x0A},            //strings for the editor
	{0x0053A554, CIV_COUNT + 1},   //civ count there

	{0x53A65C, 0xDC},
	{0x53A65D, 0x02},
	{0x53A6D0, 0xDC},
	{0x53A6D1, 0x02},
	{0x53A74E, 0xDC},
	{0x53A74F, 0x02},
	{0x53A7C1, 0xDC},
	{0x53A7C2, 0x02},
	{0x53A82D, 0xDC},
	{0x53A82E, 0x02},
	{0x53A859, 0xDC},
	{0x53A85A, 0x02},
	{0x53A8F4, 0xDC},
	{0x53A8F5, 0x02},
	{0x53A920, 0xDC},
	{0x53A921, 0x02},
	{0x53A9A7, 0xDC},
	{0x53A9A8, 0x02},
	{0x53A9D3, 0xDC},
	{0x53A9D4, 0x02},

	{0x53A4F9, 0x98},
	{0x53A4FA, 0x03},
	{0x53A587, 0xA8},
	{0x53A588, 0x03},
	{0x53AAA2, 0xA8},
	{0x53AAA3, 0x03},

	{0x53A599, 0xAC},
	{0x53A59A, 0x03},
	{0x53AA9B, 0xAC},
	{0x53AA9C, 0x03},

	{0x53A519, 0xA8},
	{0x53A51A, 0x03},

	{0x53A5F6, 0xB4},
	{0x53A5F7, 0x03},

	{0x53A4F2, 0x94},
	{0x53A4F3, 0x03},
	{0x53A50F, 0x94},
	{0x53A510, 0x03},
	{0x53A527, 0x94},
	{0x53A528, 0x03},
	{0x53AAC2, 0x94},
	{0x53AAC3, 0x03},

	/************/

	{0x0052A55B, 0xBE}, //
	{0x0052A55C, 0x0A}, //offset of (E) things for techs

	{0x52A56C, CIV_COUNT + 1},

	{0x529E52, 0xDC},
	{0x529E53, 0x04},
	{0x52A09A, 0x08},
	{0x52A09B, 0x05},
	{0x529E5E, 0xF0},
	{0x529E5F, 0x04},
	{0x52A0B0, 0xDC},
	{0x52A0B1, 0x04},
	{0x52AAF1, 0xDC},
	{0x52AAF2, 0x04},

	{0x52A5D8, 0x28},
	{0x52A5D9, 0x04},
	{0x52A5FB, 0x24},
	{0x52A5FC, 0x04}
};

void fixCivLetterFunction()
{
	for (int i = 0; i < (sizeof(civLetterSet) / sizeof(civLetterSet[0])); i++)
		writeByte(civLetterSet[i].addr, civLetterSet[i].val);
}

char ss1[] = "%s %s";
char ss2[] = "!%s %s";

__declspec (naked) void altLetter1() //0053A874
{
	__asm
	{
		mov     edx, [esp]
		mov     [esp], eax
		push    edx
		push    offset ss1
		push    ecx
		mov     eax, 0053A87Bh
		jmp     eax
	}
}

__declspec (naked) void altLetter2() //0053A93B
{
	__asm
	{
		mov     edi, [esp]
		mov     [esp], edx
		push    edi
		push    offset ss1
		push    eax
		mov     eax, 0053A942h
		jmp     eax
	}
}

__declspec (naked) void altLetter3() //0053A9EE
{
	__asm
	{
		mov     edi, [esp]
		mov     [esp], edx
		push    edi
		push    offset ss1
		push    eax
		mov     eax, 0053A9F5h
		jmp     eax
	}
}

__declspec (naked) void altLetter4() //0053AA46
{
	__asm
	{
		mov     edi, [esp]
		mov     [esp], ecx
		push    edi
		push    offset ss2
		push    edx
		mov     eax, 0053AA4Dh
		jmp     eax
	}
}

#pragma optimize( "s", on )
void setAltCivLetter()
{
	setHook((void*)0x0053A874, altLetter1);
	setHook((void*)0x0053A93B, altLetter2);
	setHook((void*)0x0053A9EE, altLetter3);
	setHook((void*)0x0053AA46, altLetter4);
}
#pragma optimize( "", on )
