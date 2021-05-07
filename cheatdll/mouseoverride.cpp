#include "stdafx.h"
#include "mouseoverride.h"

__declspec(naked) void mouseoverride1() //00617B59
{
	__asm
	{
		mov     edx, [esi+110h]
		mov     ecx, [esi+154h]
		and     edx, edx
		jz      loc_41373F
		push    eax
		mov     eax, ecx
		mov     ecx, [edx+8]
		dec     ecx
		cmp     eax, ecx
		jbe     short loc_7C4E19
		test    eax, eax
		jge     short loc_7C4E17
		xor     eax, eax
		jmp     short loc_7C4E19

loc_7C4E17:
		mov     eax, ecx

loc_7C4E19:
		push    eax
		mov     eax, [esi+150h]
		cmp     eax, ecx
		jbe     short loc_7C4E2E
		test    eax, eax
		jge     short loc_7C4E2C
		xor     eax, eax
		jmp     short loc_7C4E2E

loc_7C4E2C:
		mov     eax, ecx

loc_7C4E2E:
		push    eax
		mov     eax, [esi+14Ch]
		cmp     eax, ecx
		jbe     short loc_7C4E43
		test    eax, eax
		jge     short loc_7C4E41
		xor     eax, eax
		jmp     short loc_7C4E43

loc_7C4E41:
		mov     eax, ecx

loc_7C4E43:
		push    eax
		mov     eax, [esi+148h]
		cmp     eax, ecx
		jbe     short loc_7C4E58
		test    eax, eax
		jge     short loc_7C4E56
		xor     eax, eax
		jmp     short loc_7C4E58

loc_7C4E56:
		mov     eax, ecx

loc_7C4E58:
		mov     edx, [esi]
		push    eax
		mov     ecx, esi
		call    dword ptr [edx+140h]
		mov     ecx, 00617B80h
		jmp     ecx
loc_41373F:
		mov     edx, 00617B5Fh
		jmp     edx
	}
}

DWORD dword_7A50C0 = 0;

__declspec(naked) void mouseoverride2() //00618A2B
{
	__asm
	{
		mov     edx, [esp+10h]
		or      edx, [esp+40h]
		mov     dword_7A50C0, edx
		call    dword ptr [eax+134h]
		xor     eax, eax
		pop     edi
		mov     dword_7A50C0, eax
		pop     esi
		inc     eax
		add     esp, 18h
		retn    10h
	}
}

__declspec(naked) void mouseoverride3() //00618C8B
{
	__asm
	{
		mov     eax, ebx
		pop     esi
		pop     ebx
		retn    14h
	}
}

__declspec(naked) void mouseoverride4() //006197D2
{
	__asm
	{
		call    dword ptr [edx+144h]
		test    eax, eax
		jz      loc_4150C4
		cmp     dword_7A50C0, 1
		jnz     loc_415096
		mov     edi, eax
		mov     eax, 006197DCh
		jmp     eax
loc_4150C4:
		mov     eax, 00619806h
		jmp     eax
loc_415096:
		mov     ecx, 006197D8h
		jmp     ecx
	}
}

__declspec(naked) void mouseoverride5() //0061A3B2
{
	__asm
	{
		mov     ebp, dword_7A50C0
		lea     edx, [esp+234h]
		dec     ebp
		jle     loc_415BF9
		cmp     ebp, 8
		jz      short loc_7C4C1D
		mov     esi, 7FFFFFFFh

loc_7C4BDE:
		mov     edi, [esp+ecx*4+34h]
		test    edi, edi
		jz      short loc_7C4C15
		cmp     edi, esi
		jg      short loc_7C4C15
		mov     eax, [esp+10h]
		mov     esi, [edx]
		mov     eax, [eax+108h]
		mov     eax, [eax+0ACh]
		mov     eax, [eax+esi*4]
		mov     esi, [esp+540h]
		test    esi, esi
		jz      short loc_7C4C13
		mov     ebp, [esp+ecx*4+134h]
		mov     [esi], ebp

loc_7C4C13:
		mov     esi, edi

loc_7C4C15:
		inc     ecx
		add     edx, 0Ch
		cmp     ecx, ebx
		jl      short loc_7C4BDE

loc_7C4C1D:
		pop     edi
		pop     esi
		pop     ebp
		pop     ebx
		add     esp, 524h
		retn    20h

loc_415BF9:
		//mov     eax, 0061A3B9h
		//jmp     eax
		push    0061A3B9h
		ret
	}
}

//174 -> 178
//178 -> 17C

__declspec(naked) void ftol_exe()
{
	__asm
	{
		mov     eax, 00632BACh
		jmp     eax
	}
}

extern const float const_f_zero = 0.0f;
const float const_f_half = 0.5f;
const float const_f_hundred = 100.0f;

__declspec(naked) void mouseoverride6() //0060BD51
{
	__asm
	{
		mov     ecx, [esi+110h]
		push    eax
		mov     eax, [ecx+8]
		dec     eax
		test    edx, edx
		jle     short loc_7C494F
		cmp     edx, eax
		jge     short loc_7C494F
		movsx   edx, word ptr [esi+12Eh]
		test    edx, edx
		jle     short loc_7C494F
		cmp     edx, eax
		jge     short loc_7C494F
		push    edi
		fld     st(1)
		fmul    const_f_hundred
		call    ftol_exe
		fld     st
		fmul    const_f_hundred
		mov     edi, eax
		call    ftol_exe
		cmp     eax, edi
		jz      short loc_7C494E
		mov     ecx, eax
		add     eax, edi
		shr     eax, 1
		cmp     eax, 32h
		jnz     short loc_7C4968

loc_7C494E:
		pop     edi

loc_7C494F:
		pop     eax
		mov     edx, [esp+14h]
		mov     [esi+13Ch], eax
		mov     ecx, [edi+17Ch]
		mov     eax, [esi+14h]
		jmp     loc_41B610

loc_7C4968:
		xor     edx, edx
		pop     edi
		cmp     edx, [edi+1A8h]
		jnz     short loc_7C494F
		fstp    st
		fstp    st
		fld     const_f_half
		fld     st
		fiadd   dword ptr [esp+14h]
		fcom    dword ptr [esi+13Ch]
		fnstsw  ax
		test    ah, 40h
		jz      short loc_7C4991
		inc     edx

loc_7C4991:
		fstp    dword ptr [edi+178h]
		fiadd   dword ptr [esp+18h]
		fcom    dword ptr [esi+140h]
		fnstsw  ax
		test    ah, 40h
		jz      short loc_7C49A9
		inc     edx

loc_7C49A9:
		fstp    dword ptr [edi+17Ch]
		cmp     edx, 2
		jnz     short loc_7C49BF
		add     esp, 4
		pop     ebp
		pop     ebx
		pop     edi
		pop     esi
		add     esp, 8
		retn

loc_7C49BF:
		mov     eax, [edi+178h]
		fld     const_f_half
		add     esp, 4
		fld     st
		mov     edx, [esp+14h]
		mov     [esi+13Ch], eax
		mov     ecx, [edi+17Ch]
		mov     eax, [esi+14h]
loc_41B610:
		mov     edx, 0060BD60h
		jmp     edx
	}
}

const char byte_7C4B70[] = {
	00, 00, 01, 00, 00, 00, 01, 00,  01, 00, 01, 00, 00, 00, 01, 01,
	01, 00, 00, 00, 00, 00, 00, 00,  00, 00, 00, 00, 00, 00, 00, 00,
	00, 00, 00, 00, 00, 00, 00, 00,  00, 00, 00, 00, 00, 00, 00, 00,
	00, 00, 00, 00, 00, 00, 00, 00,  00, 00, 00, 00, 00, 00, 00, 00,
	00, 00, 00, 00, 00, 00, 00, 00,  00, 00, 00, 00, 00, 00, 00, 00
};

__declspec(naked) void mouseoverride7() //0060BE53
{
	__asm
	{
		mov     bl, [ecx+4]
		mov     ah, [ecx+6]
		mov     al, byte_7C4B70[ebx]
		movsx   ecx, word ptr [ecx+2]
		test    al, al
		jz      short loc_7C4B57
		add     ecx, 18h

		loc_7C4B57:
		mov     bl, ah
		push    0060BE5Ah
		ret
	}
}

__declspec(naked) void mouseoverride8() //0060C1CB
{
	__asm
	{
		cmp     byte ptr [ecx + 42Dh], 1
		jz      loc_41C0AA
		fld     dword ptr [esp+68h]
		mov     [esp+40h], esi
		mov     eax, 0060C1D3h
		jmp     eax
loc_41C0AA:
		mov     ecx, 0060C765h
		jmp     ecx
	}
}

char dword_7A5F00 = 0;

__declspec(naked) void mouseoverride9() //00612114
{
	__asm
	{
		test    dword_7A5F00, 3
		jnz     short loc_421319
		mov     edx, [esi]
		push    1
		mov     ecx, esi
		call    dword ptr [edx + 2Ch]
loc_421319:
		mov     eax, 006120B9h
		jmp     eax
	}
}

//check edx+1E0, 1E4 below!!!
//1E0 -> 1DC
//1E4 -> 1E0 (?)

__declspec(naked) void mouseoverride10() //004F911A
{
	__asm
	{
		mov     edx, 006A3684h
		mov     edx, [edx]
		push    edi
		mov     bl, al
		mov     edi, eax
		and     al, 4
		cmp     dword ptr [edx+1E0h], 7
		jnz     loc_51C2F1
		cmp     dword ptr [edx+1DCh], 4
		jnz     loc_51C2F1
		movzx   ebp, word ptr [edx+428h]
		mov     esi, 318h
		cmp     ebp, esi
		jb      short loc_7C4D40
		test    eax, eax
		jl      short loc_7C4CF8
		sub     ebp, esi
		jnz     short loc_7C4CD4
		lea     eax, [esi+8]
		jmp     short loc_7C4D20

loc_7C4CD4:
		cmp     ebp, 4
		jnz     short loc_7C4CDE
		lea     eax, [esi+0Ch]
		jmp     short loc_7C4D20

loc_7C4CDE:
		cmp     ebp, 8
		jnz     short loc_7C4CE8
		lea     eax, [esi+4]
		jmp     short loc_7C4D20

loc_7C4CE8:
		cmp     ebp, 0Ch
		jnz     loc_51C2F1
		mov     eax, esi
		jmp     short loc_7C4D20

		//START OF FUNCTION CHUNK FOR sub_51C2E0
loc_7C4CF8:
		sub     ebp, esi
		jnz     short loc_7C4D01
		lea     eax, [esi+0Ch]
		jmp     short loc_7C4D20

loc_7C4D01:
		cmp     ebp, 4
		jnz     short loc_7C4D0B
		lea     eax, [esi+8]
		jmp     short loc_7C4D20

loc_7C4D0B:
		cmp     ebp, 8
		jnz     short loc_7C4D14
		mov     eax, esi
		jmp     short loc_7C4D20

loc_7C4D14:
		cmp     ebp, 0Ch
		jnz     loc_51C2F1
		lea     eax, [esi+4]

loc_7C4D20:
		pop     edi
		pop     esi
		mov     [edx+428h], eax
		pop     ebp
		mov     word ptr [edx+42Ch], 100h
		mov     eax, 1
		pop     ebx
		add     esp, 0Ch
		retn    0Ch

		//START OF FUNCTION CHUNK FOR sub_51C2E0
loc_7C4D40:
		mov     esi, 1E7h
		test    eax, eax
		jl      short loc_7C4D80
		sub     ebp, esi
		jnz     short loc_7C4D54
		mov     eax, 299h
		jmp     short loc_7C4DB0

loc_7C4D54:
		cmp     ebp, 3
		jnz     short loc_7C4D60
		mov     eax, 2A1h
		jmp     short loc_7C4DB0

loc_7C4D60:
		sub     ebp, 0B2h
		jnz     short loc_7C4D6D
		lea     eax, [esi+3]
		jmp     short loc_7C4DB0

loc_7C4D6D:
		cmp     ebp, 8
		jnz     loc_51C2F1
		mov     eax, esi
		jmp     short loc_7C4DB0

		//START OF FUNCTION CHUNK FOR sub_51C2E0
loc_7C4D80:
		sub     ebp, esi
		jnz     short loc_7C4D8B
		mov     eax, 2A1h
		jmp     short loc_7C4DB0

loc_7C4D8B:
		cmp     ebp, 3
		jnz     short loc_7C4D97
		mov     eax, 299h
		jmp     short loc_7C4DB0

loc_7C4D97:
		sub     ebp, 0B2h
		jnz     short loc_7C4DA4
		lea     eax, [esi+0]
		jmp     short loc_7C4DB0

loc_7C4DA4:
		cmp     ebp, 8
		jnz     loc_51C2F1
		lea     eax, [esi+3]

loc_7C4DB0:
		pop     edi
		pop     esi
		mov     [edx+428h], eax
		pop     ebp
		mov     word ptr [edx+42Ch], 100h
		mov     eax, 1
		pop     ebx
		add     esp, 0Ch
		retn    0Ch

loc_51C2F1:
		mov     edx, 004F9121h
		jmp     edx
	}
}

//430 -> 42C

__declspec(naked) void mouseoverride11() //0050138B
{
	__asm
	{
		mov     word ptr [eax + 42Ch], 0
		mov     ebx, 00501392h
		jmp     ebx
	}
}

DWORD dword_7A5FEC;
const float flt_7C4B60 = 4.0f;
const float flt_7C4B64 = 0.25f;
const float flt_7C4B68 = 0.0625f;

__declspec(naked) void sub_7C4C60()
{
	__asm
	{
		mov     edx, esp
		add     esp, 0FFFFFFF4h
		fstcw   word ptr [edx-2]
		wait
		mov     ax, [edx-2]
		or      ah, 0Ch
		mov     [edx-4], ax
		fldcw   word ptr [edx-4]
		fistp   qword ptr [edx-0Ch]
		fldcw   word ptr [edx-2]
		mov     esp, edx
		retn
	}
}

__declspec(naked) void mouseoverride12() //004C2050
{
	__asm
	{
		mov     dword_7A5FEC, edx
		fld     dword ptr [esp+8]
		fmul    flt_7C4B60
		fadd    const_f_half
		call    sub_7C4C60
		fild    dword ptr [esp-10h]
		fmul    flt_7C4B64
		fstp    dword ptr [esp+8]
		fld     dword ptr [esp+4]
		fmul    flt_7C4B60
		fadd    const_f_half
		call    sub_7C4C60
		fild    dword ptr [esp-10h]
		mov     edx, dword_7A5FEC
		fmul    flt_7C4B64
		mov     eax, [esp+8]
		fst     dword ptr [esp+4]
		cmp     eax, [edx+17Ch]
		mov     ecx, [esp+4]
		jnz     short loc_7C4A5A
		cmp     ecx, [edx+178h]
		jz      short loc_7C4A75

loc_7C4A5A:
		mov     eax, [esp+0Ch]
		xor     ecx, ecx
		test    eax, eax
		jnz     short loc_7C4A6A
		//mov     ds:dword_7A5F00, ecx
		mov     dword_7A5F00, cl

loc_7C4A6A:
		fcomp   const_f_zero
		mov     eax, 004C205Ah
		jmp     eax

loc_7C4A75:
		xor     eax, eax
		fstp    st
		retn    0Ch
	}
}

//174 -> 178
//178 -> 17C
//17C -> 180
//17E -> 182

const float const_f_16 = 16.0f;
const float const_f_three = 3.0f;

__declspec(naked) void sub_7C4A80() //
{
	__asm
	{
		mov     edx, [esi+8Ch]
		mov     ecx, [edx+34h]
		mov     edx, [ecx+8]
		mov     ecx, [esp+8]
		test    eax, eax
		jle     short loc_7C4AAB
		cmp     eax, edx
		jl      short loc_7C4AB7
		lea     eax, [edx-1]
		mov     [esp+4], eax
		fild    dword ptr [esp+4]
		fstp    dword ptr [esi+178h]
		jmp     short loc_7C4AB7

loc_7C4AAB:
		xor     eax, eax
		mov     [esp+4], eax
		mov     [esi+178h], eax

loc_7C4AB7:
		test    ecx, ecx
		jle     short loc_7C4AD2
		cmp     ecx, edx
		jl      short loc_7C4ADE
		lea     ecx, [edx-1]
		mov     [esp+8], ecx
		fild    dword ptr [esp+8]
		fstp    dword ptr [esi+17Ch]
		jmp     short loc_7C4ADE

loc_7C4AD2:
		xor     ecx, ecx
		mov     [esp+8], ecx
		mov     [esi+17Ch], ecx

loc_7C4ADE:
		mov     [esi+180h], ax
		mov     [esi+182h], cx
		fld     dword ptr [esi+17Ch]
		fmul    const_f_16
		fadd    const_f_half
		call    sub_7C4C60
		fild    dword ptr [esp-10h]
		fmul    flt_7C4B68
		fstp    dword ptr [esi+17Ch]
		fld     dword ptr [esi+178h]
		fmul    const_f_16
		fadd    const_f_half
		call    sub_7C4C60
		fild    dword ptr [esp-10h]
		fmul    flt_7C4B68
		fstp    dword ptr [esi+178h]
		retn    8
	}
}

__declspec(naked) void sub_5581D0() //
{
	__asm
	{
		push    ecx
		push    esi
		mov     esi, ecx
		fld     dword ptr [esi+1ACh]
		fcomp   const_f_zero
		fnstsw  ax
		test    ah, 41h
		jnz     loc_5582D8
		fld     dword ptr [esi+1ACh]
		fmul    const_f_half
		fcomp   dword ptr [esi+1B0h]
		fnstsw  ax
		test    ah, 1
		jz      short loc_558212
		fld     dword ptr [esi+1ACh]
		fsub    dword ptr [esi+1B0h]
		jmp     short loc_558218

loc_558212:
		fld     dword ptr [esi+1B0h]

loc_558218:
		fcom    const_f_zero
		fnstsw  ax
		test    ah, 1
		jz      short loc_55825F
		fstp    st
		fld     dword ptr [esi+19Ch]
		fst     dword ptr [esi+178h]
		fld     dword ptr [esi+1A0h]
		fst     dword ptr [esi+17Ch]
		call    ftol_exe
		push    eax
		call    ftol_exe
		push    eax
		mov     ecx, esi
		call    sub_7C4A80
		mov     dword ptr [esi+1ACh], 0
		pop     esi
		pop     ecx
		retn

loc_55825F:
		fdiv    dword ptr [esi+1ACh]
		mov     eax, [esi+8Ch]
		fmul    const_f_three
		fadd    const_f_half
		fld     dword ptr [eax+0A8h]
		fmul    dword ptr [esi+1ACh]
		fmulp   st(1), st
		fadd    dword ptr [esi+1B0h]
		fst     dword ptr [esi+1B0h]
		fdiv    dword ptr [esi+1ACh]
		fld     st
		fmul    dword ptr [esi+194h]
		fadd    dword ptr [esi+1A4h]
		fst     dword ptr [esp+4]
		fstp    dword ptr [esi+178h]
		fmul    dword ptr [esi+198h]
		fadd    dword ptr [esi+1A8h]
		fst     dword ptr [esi+17Ch]
		call    ftol_exe
		fld     dword ptr [esp+4]
		push    eax
		call    ftol_exe
		push    eax
		mov     ecx, esi
		call    sub_7C4A80

loc_5582D8:
		pop     esi
		pop     ecx
		retn
	}
}

#pragma optimize( "s", on )
void setMouseOverrideHooks()
{
	writeDword(0x006169AD, 0x9090C033);		//xor eax, eax
	writeByte(0x006169B1, 0x90);
	writeDword(0x00616A9D, 0x9090C033);		//xor eax, eax
	writeByte(0x00616AA1, 0x90);

	setHook((void*)0x004C4660, sub_5581D0);

	setHook((void*)0x00617B59, mouseoverride1);
	setHook((void*)0x00618A2B, mouseoverride2);
	setHook((void*)0x00618C8B, mouseoverride3);
	setHook((void*)0x006197D2, mouseoverride4);
	setHook((void*)0x0061A3B2, mouseoverride5);
	setHook((void*)0x0060BD51, mouseoverride6);
	setHook((void*)0x0060BE53, mouseoverride7);
	setHook((void*)0x0060C1CB, mouseoverride8);
	setHook((void*)0x00612114, mouseoverride9);
	setHook((void*)0x004F911A, mouseoverride10);
	setHook((void*)0x0050138B, mouseoverride11);
	setHook((void*)0x004C2050, mouseoverride12);

	writeDword(0x004B6F93, 0x90909090);
	writeWord(0x004B6F97, 0x9090);
	writeByte(0x004B6F99, 0x90);
}
#pragma optimize( "", on )
