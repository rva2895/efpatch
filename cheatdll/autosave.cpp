#include "stdafx.h"
#include "autosave.h"
#include "editorstatus.h"

bool isEditor = false;
extern HWND hWnd_main;
int autosave_interval;
bool autosave = false;

void __stdcall editor_enter()
{
	log("Editor opened");
	isEditor = true;
	if (autosave)
		SetTimer(hWnd_main, AUTOSAVE_TIMER, autosave_interval, 0);
}

void __stdcall editor_exit()
{
	log("Editor closed");
	isEditor = false;
	if (autosave)
	{
		log("Editor autosave: editor closed, killing timer");
		KillTimer(hWnd_main, AUTOSAVE_TIMER);
	}
}

char name[] = "autosave.sc1";

__declspec(naked) void saveScen()
{
	__asm
	{
		mov     ecx, 0x6A3684
		mov     ecx, [ecx]
		push    offset name
		mov     ecx, [ecx + 420h]
		mov     edx, [ecx]
		call    dword ptr [edx + 100h]
		ret
	}
}

void editor_autosave()
{
	if (isEditor)
	{
		log("Editor autosave: scenario -> autosave.sc1");
		saveScen();
		//paint_test();
	}
	else
	{
		log("Editor autosave: not in editor, killing timer");
		KillTimer(hWnd_main, AUTOSAVE_TIMER);
	}
}

void setAutosaveHooks(int interval)
{
	log("Editor autosave initialised, interval = %d seconds", interval);
	autosave_interval = interval * 1000;
	autosave = true;
}
