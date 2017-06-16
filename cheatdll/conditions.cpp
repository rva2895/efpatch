#include "stdafx.h"

#include "conditions.h"

#define NEW_COND 3

#define ALLIANCE_ALLY 0
#define ALLIANCE_NEUTRAL 1
#define ALLIANCE_ENEMY 3

int condJMPTable [] =
{
	0x5F1E53,
	0x5F1EA8,
	0x5F1F25,
	0x5F1F9B,
	0x5F2054,
	0x5F22C5,
	0x5F22E4,
	0x5F2312,
	0x5F2343,
	0x5F2374,
	0x5F2396,
	0x5F23DB,
	0x5F2411,
	0x5F2435,
	0x5F24DB,
	0x5F2540,
	0x5F25A5,
	0x5F25D2,
	0x5F25FE,
	0x5F1FE6,
	0x5F20F5,
	0x5F2227,
	0x5F1E25,
	0x90909090,
	0x90909090,
	0x90909090
};

void conditionAreaExplored();
void conditionAlliance();

const char sss [] = "wew lad";

__declspec(naked) void condNotMet ()
{
	__asm
	{
		push    005F2626h
		ret
	}
}

__declspec(naked) void condMet ()
{
	__asm
	{
		push    005F1E95h
		ret
	}
}

__declspec(naked) void cond1 ()
{
	__asm
	{
		//call    ds:[rand]
		mov		edx, 00632BDDh
		call	edx				//rand
		xor     edx, edx
		mov     edi, 3E8h
		idiv    edi
		mov     edi, [esi + 0Ch]
		cmp     edi, edx
		jg      cYes
		jmp     condNotMet
cYes:
		jmp     condMet
	}
}

void setConditionNumbers ()
{
	setByte (0x005F538A, 0x60+4*NEW_COND);
	setByte (0x005F5505, 0x60+4*NEW_COND);
	setByte (0x005F5548, 0x60+4*NEW_COND);
	setByte (0x0053BD77, 0x18+NEW_COND);

	setInt (0x005F1E21, (int)condJMPTable);
	setByte (0x005F1E17, 0x16+NEW_COND);

	condJMPTable[0x17] = (int)&cond1;
	condJMPTable[0x18] = (int)&conditionAreaExplored;
	condJMPTable[0x19] = (int)&conditionAlliance;
}

__declspec(naked) void condParams () //005F55AA
{
	__asm
	{
		mov     eax, [esi + 8]
		mov     edx, [eax + 60h]    //new condition 1
		mov		[edx + 0], cl

		mov     eax, [esi + 8]
		mov     edx, [eax + 64h]    //new condition 2
		mov		[edx + 0], cl
		mov		[edx + 5], cl
		mov		[edx + 9], cl
		mov		[edx + 0Ah], cl
		mov		[edx + 0Bh], cl
		mov		[edx + 0Ch], cl

		mov     eax, [esi + 8]
		mov     edx, [eax + 68h]    //new condition 2
		mov		[edx + 0], cl
		mov		[edx + 5], cl
		mov		[edx + 7], cl

		mov     eax, [esi+8]
		mov     edx, [eax+8]
		push    005F55B0h
		ret
	}
}

int invBtnPtr;

__declspec(naked) void funcCreateWrapper ()
{
	__asm
	{
		mov     eax, [esp+4]
		push    esi
		push    edi
		mov     edi, [esp+10h]
		push    0
		push    1
		push    0
		push    0
		push    0
		push    0
		push    edi
		mov     esi, ecx
		push    eax
		mov     eax, 4b9520h
		call    eax
		test    eax, eax
		jnz     short loc_4ED558
		pop     edi
		pop     esi
		retn    8
		//---------------------------------------------------------------------------
		
loc_4ED558:                      //       ; CODE XREF: sub_4ED530+21j
		mov     ecx, [edi]
		push    0
		push    1
		mov     eax, 4B7450h
		call    eax
		mov     ecx, [edi]
		push    0
		mov     edx, [ecx]
		call    dword ptr [edx+14h]
		mov     eax, [esi+320h]
		test    eax, eax
		jz      short loc_4ED5B5
		xor     eax, eax
		xor     ecx, ecx
		mov     al, [esi+32Dh]
		mov     cl, [esi+32Ch]
		xor     edx, edx
		push    eax
		mov     dl, [esi+32Bh]
		push    ecx
		xor     eax, eax
		push    edx
		mov     al, [esi+32Ah]
		xor     ecx, ecx
		mov     cl, [esi+329h]
		xor     edx, edx
		mov     dl, [esi+328h]
		push    eax
		push    ecx
		mov     ecx, [edi]
		push    edx
		push    3
		mov     eax, 4C5190h
		call    eax
		
loc_4ED5B5:             //                ; CODE XREF: sub_4ED530+44j
		pop     edi
		mov     eax, 1
		pop     esi
		retn    8
	}
}

__declspec(naked) void inv1 () //00529A06
{
	__asm
	{
		mov     ecx, esi
		mov     eax, 531E80h
		call    eax
		mov     eax, [esi+918h]
		lea     edi, invBtnPtr
		push    edi
		push    eax
		mov     ecx, esi
		call    funcCreateWrapper
		test    eax, eax
		jz      short loc_7E22C4
		mov     ecx, ds:invBtnPtr
		push    12h
		push    12h
		push    2
		mov     edx, [ecx]
		push    0BAh
		call    dword ptr [edx+28h]

loc_7E22C4:
		push    00529A0Dh
		ret
	}
}

__declspec(naked) void inv2 () //0052ABD7
{
	__asm
	{
		lea     eax, invBtnPtr
		mov     ecx, esi
		push    eax
		mov     eax, 428520h
		call    eax
		lea     eax, [esi+964h]
		push    0052ABDDh
		ret
	}
}

__declspec(naked) void inv3 () //0053BDA0
{
	__asm
	{
		mov     ecx, invBtnPtr
		push    0
		mov     edx, [ecx]
		call    dword ptr [edx + 14h]

		mov     ecx, [esi+0E1Ch]
		push    0053BDA6h
		ret
	}
}

__declspec(naked) void inv4 () //0053C37C
{
	__asm
	{
		movsx   eax, byte ptr [edi+2Ch]
		neg     eax
		mov     ecx, ds:invBtnPtr
		push    eax
		mov     edx, [ecx]
		call    dword ptr [edx+104h]
		mov     ecx, ds:invBtnPtr
		push    1
		mov     edx, [ecx]
		call    dword ptr [edx+14h]
		mov     ecx, edi
		//call    sub_553EA0 -> 419150
		mov     eax, [ecx+4]

		mov     ecx, edi
		mov     eax, 419150h
		call    eax
		push    0053C383h
		ret
	}
}

__declspec(naked) void inv5 () //0053E014
{
	__asm
	{
		mov     ecx, ds:invBtnPtr
		mov     eax, 4C5280h
		call    eax
		neg     eax
		mov     [ebx+2Ch], al

		mov     ecx, ebx
		mov     eax, 419150h
		call    eax
		push    0053E01Bh
		ret
	}
}

__declspec(naked) void inv6 () //005F2756
{
	__asm
	{
		test    cl, cl
		jnz     cont
		cmp     ebp, 8
		jz      short loc_7E236B
		mov     dword ptr [ebx], 0FFFFFFFFh
		//
loc_7E236B:                     //        ; CODE XREF: sub_437120+3AB243j
		inc     ebp
		add     ebx, 4
		cmp     ebp, 10h
		jge     jmpFar
		xor     edx, edx
		push    5F274Bh
		ret
jmpFar:
		push    005F2817h
		ret
cont:
		push    005F275Eh
		ret
	}
}

char invCond;

__declspec(naked) void invProcessCond () //005F4A0F
{
	__asm
	{
		mov     ecx, [ecx+edi*4]
		mov     al, [ecx+2Ch]
		mov     ds:invCond, al
		mov     eax, 005F1DE0h
		call    eax
		cmp     al, 2
		jz      loc_5F4A2D
		add     al, ds:invCond
		jnz     loc_5F4A63
		push    5F4A23h
		ret
loc_5F4A63:
		push    5F4A63h
		ret
loc_5F4A2D:
		push    5F4A2Dh
		ret
	}
}

__declspec(naked) void conditionAreaExplored()
{
	__asm
	{
		mov     ebp, 7A1CBCh
		mov     edx, [esi + 34h]    //y1
		mov     edi, [esi + 3Ch]    //y2
		//sub     ebx, edx          //rect x size - 1

		shl     edx, 2
		shl     edi, 2
		add     edx, ebp          //y1 coloumn, x1 to x2
		add     edi, ebp          //y2 coloumn, x1 to x2

		xor		ecx, ecx
		//mov     esi, edx

x_cont :
		//edx: current coloumn
		mov     ebp, [edx]

		mov     eax, [esi + 30h]
		//mov     eax, [esi+ecx]
		push	edx
y_cont :
		shl		ecx, 10h
		mov     cl, [esi + 20h]
		mov     ebx, 10000h
		shl     ebx, cl
		shr		ecx, 10h
		test	[ebp + eax * 4], ebx
		setnz	bl
		movzx	ebx, bl
		add		ecx, ebx
		inc     eax
		cmp     eax, [esi + 38h]
		jle     y_cont
		pop		edx
		cmp     edx, edi
		jge     x_end
		add     edx, 4
		jmp     x_cont
x_end :
		//ecx = n tiles
		mov		eax, [esi + 38h]
		sub		eax, [esi + 30h]
		inc		eax			//eax = xl
		mov		edx, [esi + 3Ch]
		sub		edx, [esi + 34h]
		inc		edx			//edx = yl
		mul		edx
		mov		ebx, [esi + 0Ch]
		mul		ebx
		mov		ebx, 100
		div		ebx
		//eax = n tiles required
		cmp		ecx, eax
		jl		_not_enough_tiles
		jmp		condMet
_not_enough_tiles:
		jmp		condNotMet
	}
}

__declspec(naked) void conditionAlliance()
{
	__asm
	{
		mov		ecx, [esp + 38h]	//player ptr
		mov		eax, [esi + 28h]	//timer = target player
		push	eax
		mov		eax, 004C2C60h
		call	eax
		mov		cl, [esi + 0Ch]
		cmp		al, cl
		jz		_alliance_same
		jmp		condNotMet
_alliance_same:
		jmp		condMet
	}
}

void setConditionHooks ()
{
	setConditionNumbers ();
	setHook ((void*)0x005F55AA, &condParams);
	setHook ((void*)0x00529A06, &inv1);
	setHook ((void*)0x0052ABD7, &inv2);
	setHook ((void*)0x0053BDA0, &inv3);
	setHook ((void*)0x0053C37C, &inv4);
	setHook ((void*)0x0053E014, &inv5);
	setHook ((void*)0x005F2756, &inv6);

	setHook ((void*)0x005F4A0F, &invProcessCond);
}
