#include "stdafx.h"
#include "core.h"

__declspec(naked) void core1() //00606920
{
	__asm
	{
		mov     eax, [esp + 4]
		fld     [esp + 8]
		fmul    dword ptr [ecx]
		fld     [esp + 8]
		fmul    dword ptr [ecx + 4]
		fld     [esp + 8]
		fmul    dword ptr [ecx + 8]
		fxch    st(2)
		fstp    dword ptr [eax]
		fstp    dword ptr [eax + 4]
		fstp    dword ptr [eax + 8]
		ret     8
	}
}

__declspec(naked) void core2() //006069C0
{
	__asm
	{
		mov     eax, [esp + 4]
		fld     dword ptr [ecx]
		fdiv    [esp + 8]
		fld     dword ptr [ecx + 4]
		fdiv    [esp + 8]
		fld     dword ptr [ecx + 8]
		fdiv    [esp + 8]
		fxch    st(2)
		fstp    dword ptr [eax]
		fstp    dword ptr [eax + 4]
		fstp    dword ptr [eax + 8]
		ret     8
	}
}

__declspec(naked) void core3() //00606A10
{
	__asm
	{
		mov     edx, [esp + 8]
		mov     eax, [esp + 4]
		fld     dword ptr [edx]
		fadd    dword ptr [ecx]
		fld     dword ptr [edx + 4]
		fadd    dword ptr [ecx + 4]
		fld     dword ptr [edx + 8]
		fadd    dword ptr [ecx + 8]
		fxch    st(2)
		fstp    dword ptr [eax]
		fstp    dword ptr [eax + 4]
		fstp    dword ptr [eax + 8]
		ret     8
	}
}

__declspec(naked) void core4() //00606A60
{
	__asm
	{
		mov     edx, [esp+8]
		mov     eax, [esp+4]
		fld     dword ptr [ecx]
		fsub    dword ptr [edx]
		fld     dword ptr [ecx+4]
		fsub    dword ptr [edx+4]
		fld     dword ptr [ecx+8]
		fsub    dword ptr [edx+8]
		fxch    st(2)
		fstp    dword ptr [eax]
		fstp    dword ptr [eax+4]
		fstp    dword ptr [eax+8]
		ret     8
	}
}


__declspec(naked) void core5() //00606AB0
{
	__asm
	{
		mov     ebx, [ecx]
		mov     edx, [ecx+4]
		mov     ecx, [ecx+8]
		mov     eax, [esp+4]
		xor     ebx, 80000000h
		xor     edx, 80000000h
		xor     ecx, 80000000h
		mov     [eax], ebx
		mov     [eax+4], edx
		mov     [eax+8], ecx
		retn    4
	}
}

__declspec(naked) void core6() //00606B00
{
	__asm
	{
		mov     eax, [esp+4]
		mov     edx, [esp+8]
		fld     dword ptr [ecx+4]
		fmul    dword ptr [edx+8]
		fld     dword ptr [edx+4]
		fmul    dword ptr [ecx+8]
		fsubp   st(1), st
		fstp    dword ptr [eax]
		fld     dword ptr [edx]
		fmul    dword ptr [ecx+8]
		fld     dword ptr [ecx]
		fmul    dword ptr [edx+8]
		fsubp   st(1), st
		fstp    dword ptr [eax+4]
		fld     dword ptr [edx+4]
		fmul    dword ptr [ecx]
		fld     dword ptr [ecx+4]
		fmul    dword ptr [edx]
		fsubp   st(1), st
		fstp    dword ptr [eax+8]
		retn    8
	}
}

__declspec(naked) void core7() //00606970
{
	__asm
	{
		mov     eax, ecx
		mov     ecx, edx
		fld     [esp+4]
		fmul    dword ptr [ecx]
		fld     [esp+4]
		fmul    dword ptr [ecx+4]
		fld     [esp+4]
		fmul    dword ptr [ecx+8]
		fxch    st(2)
		fstp    dword ptr [eax]
		fstp    dword ptr [eax+4]
		fstp    dword ptr [eax+8]
		retn    4
	}
}

//

#pragma optimize( "s", on )
void setCoreHooks()
{
	setHook((void*)0x00606920, core1);
	setHook((void*)0x006069C0, core2);
	setHook((void*)0x00606A10, core3);
	setHook((void*)0x00606A60, core4);
	setHook((void*)0x00606AB0, core5);
	setHook((void*)0x00606B00, core6);
	setHook((void*)0x00606970, core7);

	setHook((void*)0x005FD030, (void*)0x00619FB0);
}
#pragma optimize( "", on )
