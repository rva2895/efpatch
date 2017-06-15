#include "stdafx.h"

#include "flare.h"

__declspec(naked) void __stdcall drawX (void* p, int y, int x, int color, int size)
{
	__asm
	{
		pop     eax
		mov     ecx, [esp]
		mov     [esp], eax
		mov     eax, 5F9670h
		jmp     eax
	}
}

void __stdcall drawFlare (void* p, int y, int x, int color, int size)
{
	drawX (p, y, x, color, size);

	//drawX (p, (y+1)|1, (x+1)|1, color, size);
	//drawX (p, y, x+1, color, size);
	//drawX (p, y-1, x, color, size);
	//drawX (p, y, x-1, color, size);
}

__declspec(naked) void onDrawX () //005F943B
{
	__asm
	{
		pop     eax    //y
		pop     ecx    //x
		pop     edx    //color
		add     esp, 4 //size - not used
		push    6
		push    edx
		push    ecx
		push    eax
		push    esi
		call    drawFlare
		push    005F9442h
		ret
	}
}

__declspec(naked) void onGetColor () //005F9416
{
	__asm
	{
		jns      _black
		movsx   ecx, byte ptr [edi+3Ch]
		mov     eax, [esi+0F8h]
		mov     eax, [eax+4Ch]
		mov     ecx, [eax+ecx*4]
		mov     edx, [ecx+164h]
		mov     eax, [edx+20h]
		push    005F9426h
		ret
_black:
		mov     eax, 0Bh
		push    005F9426h
		ret
	}
}

__declspec(naked) void onCreateFlare () //005BCAC0
{
	__asm
	{
		mov     cl, [ebp+1Ch]
		mov     [edi+3Ch], cl

		mov     ecx, [esp+10h]
		mov     edx, [ecx+4]
		push    005BCAC7h
		ret
	}
}

__declspec(naked) void onChangeState () //005F8C29
{
	__asm
	{
		sets    dl
		setz    dh
		sub     dl, dh         //3 states instead of 2
		movsx   edx, dl
		mov     [esi+17Ch], edx
		push    005F8C32h
		ret
	}
}

void setFlareHooks ()
{
	setHook ((void*)0x005F9416, &onGetColor);
	setHook ((void*)0x005F943B, &onDrawX);
	setHook ((void*)0x005BCAC0, &onCreateFlare);
	setHook ((void*)0x005F8C29, &onChangeState);
}
