#include "stdafx.h"

#include "startupload.h"

char sz_ga1[] = ".GA1";
char sz_mg1[] = ".MG1";
char sz_sc1[] = ".SC1";

char cmdLine[255];
char* filename;

__declspec(naked) void afterLoadHook() //005EC3DF
{
	__asm
	{
		pop     edi
		pop     esi
		pop     ebp
		pop     ebx
		add     esp, 10Ch
		mov     eax, 1
		push    8Bh
		push    005EC3DFh
		call    setByte
		push    75EB3BE8h
		push    005EC3E0h
		call    setInt
		push    3Ch
		push    005EC3E4h
		call    setByte
		retn    20h
	}
}

char* getCmdFilename (char* s)
{
	char* p = s + strlen (s) - 1;
	bool k = (*p == '\"');
	if (k)
		*p = 0;
	while ( k?(*p!='\"'):(*p!=' ') )
		p--;
	p++;
	//if (k)
	//	p++;
	return p;
}

char* __stdcall checkCmdLine(char* ext)
{
	//MessageBox (0, "w", "w", 0);

	char* cmdLine_ = GetCommandLine();

	strcpy(cmdLine, cmdLine_);

	_strupr(cmdLine);

	char* ext_start = strstr(cmdLine, ext);
	if (ext_start)
	{
		filename = getCmdFilename(cmdLine);
		//MessageBox (0, filename, "asd", 0);
		return filename;
	}
	else
		return 0;
}

/*__declspec(naked) void cmdLineHook () //005E55DB
{
	__asm
	{
		push    esi
		call    checkCmdLine
		test    eax, eax
		jz      _noCmdLine
		push    eax
		mov     ecx, ebp
		mov     eax, 005EC2D0h
		push    005E55F4h
		jmp     eax
_noCmdLine:

	}
}*/

void __stdcall setAbsScen();
void __stdcall onLoadSave();

__declspec(naked) void hookLoadSave() //005E5652
{
	__asm
	{
		push    ebx
		push    offset sz_ga1
		call    checkCmdLine
		test    eax, eax
		jnz     _yes_cmd_line_save
		push    offset sz_mg1
		call    checkCmdLine
		test    eax, eax
		jz      _noCmdLine_save
		//mg1: push 3
		mov     [esp], 3
_yes_cmd_line_save:
		push    eax
	}
	setHook((void*)0x0061D928, &onLoadSave);
	__asm
	{
		mov     ecx, ebp
		mov     eax, 005EC580h
		push    005E55F4h
		jmp     eax
_noCmdLine_save:
		pop     ebx
		push    005E566Dh
		ret
	}
}

__declspec(naked) void hookLoadScen() //005E5636
{
	__asm
	{
		push    offset sz_sc1
		call    checkCmdLine
		test    eax, eax
		jz      _noCmdLine_save
		push    ebx
		push    ebx
		push    eax
		call    setAbsScen
		mov     ecx, ebp
		mov     eax, 005EE090h
		push    005E55F4h
		jmp     eax
_noCmdLine_save:
		push    005E5652h
		ret
	}
}

__declspec(naked) void scenAbsPath() //00620F07
{
	__asm
	{
		push    8Ah
		push    00620F07h
		call    setByte
		push    01502484h
		push    00620F08h
		call    setInt
		push    0
		push    00620F0Ch
		call    setByte
		add     esp, 18h
		mov     eax, 1
		push    00620F3Dh
		ret
	}
}

void __stdcall setAbsScen()
{
	setHook((void*)0x00620F07, &scenAbsPath);
}

__declspec(naked) void rec_filename_fix() //0061DACF
{
	__asm
	{
		add     eax, 9
		push    eax
		push    edx
		push    50h
		push    0061DACFh
		call    setByte
		push    16148A8Bh
		push    0061DAD0h
		call    setInt
		push    0A5E80000h
		push    0061DAD4h
		call    setInt
		add     esp, 4 * 6
		pop     edx
		mov     ecx, [edx + 1614h]
		mov     eax, 0061DAD6h
		jmp     eax
	}
}

__declspec(naked) void __stdcall onLoadSave() //0061D928
{
	setInt(0x0061D928, 0x008000BA);
	setInt(0x0061D92C, 0x248C8D00);
	setHook((void*)0x0061DACF, &rec_filename_fix);
	__asm
	{
		lea     ecx, [esp + 0B1h]
		mov     edx, 8000h
		push    0061D939h
		mov     eax, 004D5330h
		jmp     eax
	}
}

void setStartupLoadHooks(int ver)
{
	if (ver)	//EF
	{
		sz_ga1[3] = '2';
		sz_mg1[3] = '2';
	}
	setHook((void*)0x005E5652, &hookLoadSave);
	setHook((void*)0x005E5636, &hookLoadScen);
	//setByte(0x0061D930, 0xB1);
}
