#include "stdafx.h"

#include "rms.h"

char** filenames;
int nFiles;

RMS_ASSIGN ra [] =
{
	{38, 13544},
	{39, 13545},
	{40, 13546},
	{41, 13547},
	{42, 13548},
	{43, 13549},
	{44, 13550},
	{45, 13551},
	{46, 13552},
	{47, 13553},
	{48, 13554},
	{49, 13555},
	{50, 13556},
	//
	{55, 13563},
	{56, 13564},
	{57, 13565},
	{58, 13566},
	{59, 13567},
	{60, 13568}
};

const int nRa = (sizeof(ra) / sizeof(ra[0]));

__declspec(naked) void rmsListLoadHook() //0052A2BE
{
	__asm
	{
		mov		ecx, [esi]
		push	1Fh
		push	2A8Eh
		mov		eax, 4C82D0h
		call	eax

		push	edi
		push	ebx

		mov		edi, offset ra
		xor		ebx, ebx

cont1:
		mov		eax, nRa
		cmp		ebx, eax
		jz		end1
		mov		al, [edi]
		movsx	eax, al
		push	eax
		mov		ax, [edi + 2]
		movsx	eax, ax
		push	eax
		mov		ecx, [esi]
		mov		eax, 4C82D0h
		call	eax
		inc		ebx
		add		edi, 4
		jmp		cont1
end1:
		//
		call	rmsNameHookLoad
		mov		edi, 4C82D0h
		xor		ebx, ebx

cont:
		mov		eax, nFiles
		cmp		ebx, eax
		jz		end
		mov		ecx, [esi]
		push	0FFh
		push	ebx
		call	edi
		inc		ebx
		jmp		cont
end:
		call	rmsNameHookUnload
		pop		ebx
		pop		edi
		mov		ecx, 0052A2CCh
		jmp		ecx
	}
}

__declspec(naked) void rmsNameLoad() //004D392C
{
	__asm
	{
		//eax - char*
		//ecx - language dll id, change to array index
		mov		edx, filenames
		lea		edx, [edx + ecx * 4]
		mov		edx, [edx]
		push	100h
		push	edx
		push	eax
		call	ds:[strncpy]
		add		esp, 0Ch
		mov		ecx, 004D393Ah
		jmp		ecx
	}
}

void rmsNameHookLoad()
{
	setHook((void*)0x004D392C, &rmsNameLoad);
}

void rmsNameHookUnload()
{
	setInt(0x004D392C, 0x00010068);
	setInt(0x004D3930, 0x8B515000);
	setInt(0x004D3934, 0x3D86E8CE);
	setInt(0x004D3938, 0x848BFFFE);
}

void findFiles()
{
	filenames = 0;
	nFiles = 0;
	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFile("random\\*.rms", &fd);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		log("FindFirstFile returned INVALID_HANDLE_VALUE");
	}
	else
	{
		do
		{
#ifdef _DEBUG
			log("found %s", fd.cFileName);
#endif
			nFiles++;
			filenames = (char**)realloc(filenames, sizeof(char*)*nFiles);
			filenames[nFiles - 1] = (char*)malloc(strlen(fd.cFileName) + 1);
			strcpy(filenames[nFiles - 1], fd.cFileName);
			filenames[nFiles - 1][strstr(fd.cFileName, ".rms") - fd.cFileName] = 0;
		} while (FindNextFile(hFile, &fd));
		int err = GetLastError();
		if (err != ERROR_NO_MORE_FILES)
			log("WARNING: FindNextFile(): unrecognised error %d", err);
		else
			log("Finished listing files");
		FindClose(hFile);
	}
	log("Found %d .rms files", nFiles);
}

void setRmsEditorHooks()
{
	findFiles();
	setHook((void*)0x0052A2BE, &rmsListLoadHook);
}
