#include "stdafx.h"

#include "test.h"
#include "log.h"

/*__declspec(naked) int someText()
{
	__asm
	{
		push    eax
		mov     ecx, [eax + 38h]
		push    ecx
		call    someTextOut
		pop     eax

		mov     edx, [esi + 100h]
		push    5E02BEh
		ret
	}
}*/

#ifdef _DEBUG

int effectsFired = 0;
int triggerBegun = 0;
char triggerText[256];

void __cdecl effectLog(int n, char* ef)
{
	if (triggerBegun)
	{
		log("%s", triggerText);
		triggerBegun = 0;
	}
	log("---> Effect %d (%d)", n, *(int*)(ef + 4));
}

__declspec(naked) int effectLogHook() //005F4A77
{
	__asm
	{
		mov     eax, [edx]
		mov     ecx, [ecx + edi * 4]
		push    eax
		push    ecx
		push    edi
		call    effectLog
		pop     edi
		pop     ecx
		push    005F4A7Dh
		ret
	}
}

void __cdecl triggerLog(int n, char* tr)
{
	triggerBegun = 1;

	char szName[] = "Untitled";
	//*szName = 0;
	char* name;
	if (*(char**)(tr + 0x14))
		name = *(char**)(tr + 0x14);
	else
		name = szName;

	if (*tr == 1)
		sprintf(triggerText, "Trigger %d (name: %s; %d conditions, %d effects", n,
			name, *(int*)(tr + 0x20), *(int*)(tr + 0x30));
}

__declspec(naked) int triggerLogHook() //005F54D2
{
	__asm
	{
		mov     ecx, [esi + 10h]
		push    ebx
		mov     ecx, [ecx + edi * 4]
		push    ecx
		push    edi
		call    triggerLog
		pop     edi
		pop     ecx
		push    005F54D9h
		ret
	}
}
#endif

int retSave;
int data;

char title[] = "DAT";
char text[] = "read";

__declspec(naked) int onReadDat()
{
	__asm
	{
		mov     ecx, data
		mov     ecx, [ecx]
		cmp     cx, 16548
		jnz     _end
		int     3
		push    eax
		push    0
		push    offset title
		push    offset text
		push    0
		call    ds:[MessageBoxA]
		pop     eax
_end:
		mov     ecx, retSave
		jmp     ecx
	}
}

__declspec(naked) int readDatHook() //004D5550
{
	__asm
	{
		mov     eax, [esp]
		mov     retSave, eax
		mov     data, edx
		mov     eax, onReadDat
		mov     [esp], eax
		push    ebp
		mov     ebp, esp
		sub     esp, 0Ch
		push    004D5556h
		ret
	}
}

void __cdecl log_internal(int unk1, char* fmt1, char* fmt2, ...)
{
	UNREFERENCED_PARAMETER(unk1);
	va_list ap;
	char* fmt;
	if ((unsigned long)fmt1 < 0x2000)
	{
		va_start(ap, fmt2);
		fmt = fmt2;
	}
	else
	{
		va_start(ap, fmt1);
		fmt = fmt1;
	}

	if (!fmt)
		return;

	if ((strlen(fmt) < 8) || (strlen(fmt) > 0x180))
		return;
	
	char s[0x200];
	vsprintf(s, fmt, ap);
	log("%s", s);
	va_end(ap);
}

void setTestHook()
{
	//str = (char*) malloc (2000);

	//setHook ((void*)0x5E02B8, &someText);

#ifdef _DEBUG
	setHook((void*)0x005F54D2, &triggerLogHook);
	setHook((void*)0x005F4A77, &effectLogHook);

	//setHook((void*)0x0060F920, &log_internal);
#endif

	//setHook ((void*)0x004D5550, &readDatHook);
}
