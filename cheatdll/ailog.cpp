#include "stdafx.h"

int current_player;

void __stdcall on_do_ai_report(int player)
{
	log("Executing AI rules for player %d", player+1);
	current_player = player+1;
}

__declspec(naked) void on_do_ai() //0040F910
{
	__asm
	{
		push    ecx
		mov     eax, [esp + 8]
		push    edi
		push    ecx
		push    eax
		push    eax
		call    on_do_ai_report
		pop     eax
		pop     ecx
		mov     edx, 0040F916h
		jmp     edx
	}
}

int current_rule;

void __stdcall on_rule_enable_check_report(int rule, int status)
{
	log("  Rule %d is %s", rule, status ? "ENABLED" : "DISABLED");
	current_rule = rule;
	//if (rule == 822)
	//	__debugbreak();
}

__declspec(naked) void on_rule_enable_check() //0040F994
{
	__asm
	{
		add     esi, ebx
		push    edx
		push    eax
		push    eax
		push    ebp
		call    on_rule_enable_check_report
		pop     eax
		pop     edx
		cmp     eax, edx
		jz      short loc_40FA19
		mov     eax, 0040F99Ah
		jmp     eax
loc_40FA19:
		mov     eax, 0040FA19h
		jmp     eax
	}
}

void __stdcall on_condition_check_report(DWORD function, int argc, int arg0, int arg1, int arg2, int arg3)
{
	log("    Condition: 0x%X, argc=%d, args=%d %d %d %d", function, argc, arg0, arg1, arg2, arg3);
}

void __stdcall on_condition_check_status_report(int status)
{
	log("      => %s", status ? "TRUE" : "FALSE");
}

__declspec(naked) void on_condition_check() //00413070
{
	__asm
	{
		push    ecx
		mov     eax, [esp + 14h]
		push    eax
		mov     eax, [esp + 14h]
		push    eax
		mov     eax, [esp + 14h]
		push    eax
		mov     eax, [esp + 14h]
		push    eax
		movsx   eax, byte ptr [ecx + 0Ch]
		push    eax
		mov     eax, [ecx + 10h]
		push    eax
		call    on_condition_check_report
		pop     ecx
		push    esi
		mov     esi, ecx
		mov     eax, [esi]
		dec     eax
		mov     edx, 00413076h
		jmp     edx
	}
}

__declspec(naked) void on_condition_check_status() //0040FB85
{
	__asm
	{
		push    eax
		push    eax
		call    on_condition_check_status_report
		pop     eax
		pop     edi
		pop     esi
		pop     ebp
		pop     ebx
		retn    8
	}
}

void __stdcall on_action_report(DWORD function, int argc, int arg0, int arg1, int arg2, int arg3)
{
	log("    Action: 0x%X, argc=%d, args=%d %d %d %d", function, argc, arg0, arg1, arg2, arg3);
	if ((function == 0x00577380) && (arg0 == -17))
	{
		//__debugbreak();
		chat("Train pummel, player %d", current_player);
		log("      !!! train pummel!");
	}
}

__declspec(naked) void on_action() //0040B170
{
	__asm
	{
		push    ecx
		mov     eax, [esp + 14h]
		push    eax
		mov     eax, [esp + 14h]
		push    eax
		mov     eax, [esp + 14h]
		push    eax
		mov     eax, [esp + 14h]
		push    eax
		movsx   eax, byte ptr [ecx]
		push    eax
		mov     eax, [ecx + 4]
		push    eax
		call    on_action_report
		pop     ecx
		mov     eax, ecx
		xor     ecx, ecx
		mov     cl, [eax]
		mov     edx, 0040B176h
		jmp     edx
	}
}

#pragma optimize( "s", on )
void setAILogHooks()
{
	setHook((void*)0x0040FB85, on_condition_check_status);
	setHook((void*)0x00413070, on_condition_check);
	setHook((void*)0x0040F994, on_rule_enable_check);
	setHook((void*)0x0040F910, on_do_ai);
	setHook((void*)0x0040B170, on_action);
}
#pragma optimize( "", on )
