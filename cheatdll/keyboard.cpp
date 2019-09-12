#include "stdafx.h"

bool key_state(int key)
{
	return GetKeyState(key) & 0x8000;
}

bool key_states[0x100];

int state_shift;
int state_menu;
int state_ctrl;

void __stdcall get_key_states(UINT msg)
{
	if (msg == WM_KEYDOWN)
	{
		
	}
}

__declspec(naked) void onCheckKeyStates() //004B6651
{
	__asm
	{
		mov     eax, [esp + 4]
		push    eax
		call    get_key_states
		mov     ecx, state_shift
		mov     edi, state_ctrl
		mov     ebp, state_menu
		mov     eax, 004B6681h
		jmp     eax
	}
}

void setKeyboardHooks()
{
	memset(key_states, 0, sizeof(key_states));
}