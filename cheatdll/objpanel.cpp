#include "stdafx.h"

#include "objpanel.h"

#include "resgenbldgs.h"

void* objPanelPtr;

int itemCounter;

void* langDllPopup;
char* is2ndCol;

int currentPlayer;

float* resources;

int(__thiscall* objPanelDrawItem)
	(void*, int level, int slpFrame, int displayType, int v1, int v2, int v3, int v4, int langDLLDesc) =
		(int (__thiscall*) (void*, int, int, int, int, int, int, int, int))0x005DB840;
//{
//	__asm
//	{
//		mov     ecx, objPanelPtr
//		push    005DB840h
//		ret
//	}
//}

__declspec(naked) void secondCol1 () //005DB8B4
{
	__asm
	{
		mov     eax, [esi+20h]
		add     edx, ebx
		push    ecx
		cmp     edi, 3
		jng     end1
		add     edx, 6Ch    //x offset for 2nd col
end1:
		push    005DB8BAh
		ret
	}
}

__declspec(naked) void secondCol2 () //005DB894
{
	__asm
	{
		mov     ecx, edi
		cmp     edi, 3
		jng     end2
		sub     ecx, 4
end2:
		lea     ecx, [ecx+ecx*8]
		shl     eax, 2
		push    005DB89Ah
		ret
	}
}

__declspec(naked) void secondColtxty () //005DBAC0
{
	__asm
	{
		mov     eax, [esp + 6Ch]   //level
		mov     ecx, eax
		cmp     eax, 3
		jng     endy
		sub     ecx, 4
endy:
		lea     ecx, [ecx+ecx*8]
		push    005DBAC7h
		ret
	}
}

__declspec(naked) void secondColtxtx () //005DBAE3
{
	__asm
	{
		mov     edx, [esp + 70h]   //level
		cmp     edx, 3
		jng     endx
		add     ecx, 6Ch
endx:
		mov     edx, [esi+20h]
		push    ecx
		mov     eax, [edx+38h]
		push    005DBAEAh
		ret
	}
}

__declspec(naked) void objPanelHook () //005DB3CE
{
	__asm
	{
		mov     ecx, [ebp + 858h]
		//push    ebp
		push    ecx
		call    objPanel
		add     esp, 4
		//pop     ebp
		mov     eax, 6A3684h
		mov     eax, [eax]
		mov     ecx, [eax+17B4h]
		push    005DB3D9h
		ret
	}
}

__declspec(naked) void obtainPtr () //005D98AE
{
	__asm
	{
		mov     objPanelPtr, ecx
		mov     ebp, ecx
		mov     eax, [ebp + 858h]
		test    eax, eax
		jz      curPend
		mov     eax, [eax + 18h]    //selected player
		mov     ecx, [ebp + 854h]   //current player
		cmp     eax, ecx
		jz      curP
		xor     eax, eax
		jmp     curPend
curP:
		xor     eax, eax
		inc     eax
		mov     ecx, [ecx + 0ACh]   //resources
		mov     resources, ecx
curPend:
		mov     currentPlayer, eax
		push    esi
		push    edi
		xor     edi, edi
		push    005D98B4h
		ret
	}
}

__declspec(naked) void langDllList () //005DBB13
{
	__asm
	{
		mov     ebx, langDllPopup
		mov     [ebx + eax*4], ecx
		pop     ebx
		push    005DBB1Bh
		ret
	}
}

__declspec(naked) void langDllRead1 () //005DF2D9
{
	__asm
	{
		mov     esi, langDllPopup
		mov     eax, [esi + eax*4]
		push    005DF2E0h
		ret
	}
}

__declspec(naked) void langDllRead2 () //005DF2F3
{
	__asm
	{
		mov     esi, langDllPopup
		mov     eax, [esi + ecx*4]
		push    005DF2FAh
		ret
	}
}

int secondColFlag = 0;

__declspec(naked) void popup1 () //005DF2B4
{
	__asm
	{
		inc     eax
		add     ebp, 15h
		xor     edx, edx
		cmp     ebp, 54h
		jmp     noflag1
		inc     edx
noflag1:
		mov     secondColFlag, edx
		cmp     eax, ecx
		push    005DF2BAh
		ret
	}
}

int startEbp;

__declspec(naked) void popup2 () //005DF279
{
	__asm
	{
		add     ecx, edx

		mov     edx, [esp + 28h]
		push    ebx
		mov     ebx, is2ndCol
		mov     bl, [ebx + edx]
		test    bl, bl
		pop     ebx
		jz      noflag2
		mov     edx, startEbp
		test    edx, edx
		jnz     alreadySet
		mov     startEbp, ebp
		xor     ebp, ebp
		jmp     popupCont
alreadySet:
		//sub     ebp, edx
popupCont:
		add     ecx, 6Ch
noflag2:
		add     eax, ebp
		mov     edx, [esi+10h]
		push    005DF280h
		ret
	}
}

__declspec(naked) void setItemCounter () //005D98A0
{
	__asm
	{
		push    ebp
		xor     ebp, ebp
		mov     itemCounter, ebp
		mov     ebp, esp
		and     esp, 0FFFFFFF8h
		sub     esp, 190h
		push    005D98ACh
		ret
	}
}

__declspec(naked) void incItemCounter () //005DB840
{
	__asm
	{
		sub     esp, 50h
		push    esi
		mov     esi, itemCounter
		inc     esi
		mov     itemCounter, esi
		mov     esi, ecx
		push    005DB846h
		ret
	}
}

__declspec(naked) void clearStartEbp () //005DF253
{
	__asm
	{
		xor     eax, eax
		mov     startEbp, eax
		mov     eax, [esi+838h]
		push    005DF259h
		ret
	}
}

void langDllAlloc ()
{
#ifdef _DEBUG
	log ("LangDLLAlloc()");
#endif

	langDllPopup = malloc (sizeof(void*)*0x10);
	is2ndCol = (char*) malloc (0x10);
	memset (is2ndCol, 0, 0x10);
	//is2ndCol [3] = 1;
	//is2ndCol [4] = 1;
}

void langDllFree ()
{
	free (langDllPopup);
}

void fixObjPanelDrawFunction ()
{
	setHook ((void*)0x005DB8B4, &secondCol1);
	setHook ((void*)0x005DB894, &secondCol2);

	setHook ((void*)0x005DBAC0, &secondColtxty);
	setHook ((void*)0x005DBAE3, &secondColtxtx);

	setHook ((void*)0x005DB3CE, &objPanelHook);

	setHook ((void*)0x005D98AE, &obtainPtr);
	setByte (0x005DB860, 7);

	setHook ((void*)0x005D98A0, &setItemCounter);
	setHook ((void*)0x005DB840, &incItemCounter);

	setByte (0x005DF2B7, 0x11);
	setByte (0x005DF298, 0x10);

	langDllAlloc ();

	//setHook ((void*)0x005DF2B4, &popup1);
	setHook ((void*)0x005DF279, &popup2);
	setHook ((void*)0x005DF253, &clearStartEbp);

	setHook ((void*)0x005DBB13, &langDllList);
	setHook ((void*)0x005DF2D9, &langDllRead1);
	setHook ((void*)0x005DF2F3, &langDllRead2);
}

void setObjectPanelHooks ()
{
	fixObjPanelDrawFunction ();
}
