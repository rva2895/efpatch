#include "stdafx.h"

void* ef_campaign_button = 0;

//create new button
__declspec(naked) void onCreateButtons() //0050BA3A
{
	__asm
	{
		mov     esi, offset ef_campaign_button
		mov     eax, [esi]
		test    eax, eax
		jnz     _continue_button_init
		mov     ecx, 0050B98Eh
		jmp     ecx
_continue_button_init:
		push    0Bh
		push    96h
		mov     ecx, 0050BA41h
		jmp     ecx
	}
}

//free button
__declspec(naked) void onDeleteButtons() //0050D13E
{
	__asm
	{
		mov     eax, offset ef_campaign_button
		push    eax
		mov     ecx, esi
		mov     eax, 00428520h
		call    eax
		lea     eax, [esi + 93Ch]
		mov     ecx, 0050D144h
		jmp     ecx
	}
}

//buttons release capture
__declspec(naked) void onReleaseCapture() //0050F9E8
{
	__asm
	{
		mov     ecx, ef_campaign_button
		push    0
		mov     edx, [ecx]
		call    dword ptr [edx + 14h]
		push    0
		mov     ecx, esi
		mov     eax, 0050CAA0h
		call    eax
		mov     eax, 0050F9F1h
		jmp     eax
	}
}

//ef campaign button set rect & text
__declspec(naked) void onButtonSet() //0050EEFF
{
	__asm
	{
		mov     ecx, ef_campaign_button
		push    28h     //height
		push    144h    //width
		push    400     //y
		mov     edx, [ecx]
		push    1B9h    //x
		call    dword ptr [edx + 28h]

		mov     ecx, ef_campaign_button
		//push    34612   //text
		push    240
		push    0
		mov     eax, 004C4F80h
		call    eax

		push    0
		push    14h
		mov     ecx, ef_campaign_button
		push    0
		mov     eax, 004C4D10h
		call    eax

		mov     ecx, ef_campaign_button
		push    1
		mov     eax, [ecx]
		call    dword ptr [eax + 14h]

		mov     ecx, [esi + 85Ch]
		mov     eax, 0050EF05h
		jmp     eax
	}
}

//button help text
__declspec(naked) void onHelpText() //0050F294
{
	__asm
	{
		mov     ecx, ef_campaign_button
		push    0FFFFFFFFh
		//push    31153
		push    241
		push    ecx
		mov     ecx, esi
		mov     eax, 004BB970h
		call    eax
		mov     ecx, [esi + 85Ch]
		mov     eax, 0050F29Ah
		jmp     eax
	}
}

//button press - restore default
__declspec(naked) void onButtonPress_default()
{
	__asm
	{
		push    2
		push    005106E1h
		call    writeByte
		push    0D0248C8Bh
		push    00511228h
		call    writeDword
		push    5F000002h
		push    0051122Ch
		call    writeDword
		add     esp, 18h
		mov     esi, 0051122Fh
		jmp     esi
	}
}

//button test
__declspec(naked) void onButtonTest()
{
	__asm
	{
		push    3
		push    005106E1h
		call    writeByte
		push    onButtonPress_default
		push    00511228h
		call    setHook
		add     esp, 10h
		mov     eax, 005105C4h
		jmp     eax
	}
}

//button press check
__declspec(naked) void onButtonPress() //0050E581
{
	__asm
	{
loc_50E581:
		cmp     [edx], eax
		jz      loc_50E5A6
		inc     ecx
		add     edx, 4
		cmp     ecx, 14h
		jl      loc_50E581
		cmp     ef_campaign_button, eax
		jz      loc_50E5A6
		mov     edx, 0050E5BCh
		jmp     edx
loc_50E5A6:
		mov     eax, 0050E5A6h
		jmp     eax
	}
}

char efcam[] = "2cam";

//get campaign mode implementation
__declspec(naked) void mode_1() //00507670
{
	__asm
	{
		cmp     eax, 3
		jz      _ef_mode_1
		mov     ecx, 6A3684h
		mov     ecx, [ecx]
		mov     eax, 00507676h
		jmp     eax
_ef_mode_1:
		push    1
		push    54000               //campaign menu definition
		push    offset efcam
		mov     eax, 00507682h
		jmp     eax
	}
}

char a2cam_pic_pos[] = "2cam_pic.pos";

__declspec(naked) void mode_2() //00507CC8
{
	__asm
	{
		cmp     eax, 3
		jz      _ef_mode_2
		lea     eax, [ebp - 28h]
		push    00690F98h
		push    eax
		mov     eax, 006330C4h
		call    eax
		add     esp, 8
		mov     edx, 0CFEFh
		mov     ecx, 00507CDEh
		jmp     ecx
_ef_mode_2:
		lea     eax, [ebp - 28h]
		push    offset a2cam_pic_pos
		push    eax
		mov     eax, 006330C4h
		call    eax
		add     esp, 8
		mov     edx, 54003              //campaign menu button positions
		mov     ecx, 00507CDEh
		jmp     ecx
	}
}

char a2cam[] = "2cam";

__declspec(naked) void mode_3() //00507F6E
{
	__asm
	{
		cmp     eax, 3
		jz      _ef_mode_3
		mov     edi, 690C80h
		or      ecx, 0FFFFFFFFh
		xor     eax, eax
		mov     [esp + 18h], 0C8h
		push    00507F80h
		ret
_ef_mode_3:
		mov     edi, offset a2cam
		or      ecx, 0FFFFFFFFh
		xor     eax, eax
		//mov     [esp + 18h], 0C8h     //34928 + this_n = language DLL offset of campaign and scen titles
		mov     [esp + 18h], 0ACh
		push    00507F80h
		ret
	}
}

char a2camd[] = "2cam%d";

__declspec(naked) void mode_4() //00508354
{
	__asm
	{
		lea     ecx, [esp + 4Ch]
		cmp     eax, 3
		jz      _ef_mode_4
		push    00690FD0h
		push    ecx
		mov     eax, 006330C4h
		call    eax
		add     esp, 0Ch
		lea     ecx, ds:0CFE4h[edi * 4]
		mov     eax, 0050836Dh
		jmp     eax
_ef_mode_4:
		push    offset a2camd
		push    ecx
		mov     eax, 006330C4h
		call    eax
		add     esp, 0Ch
		lea     ecx, [edi + 54003]    //scenario menu definitions, one per campaign (54004 - 54009)
		mov     eax, 0050836Dh
		jmp     eax
	}
}

int __stdcall mode_5_in(int mode, int v9) //returns offset, 34935 + this_n = campaign title ID
{
	int offset;
	switch (mode)
	{
	case 1:
		offset = 210;
		break;
	case 2:
		offset = 310;
		break;
	case 3:
		//offset = 210;
		offset = 211;
		break;
	default:
		offset = 0;
		break;
	}
	return offset + 30 * v9 + 165977;
}

__declspec(naked) void mode_5() //00508452
{
	__asm
	{
		push    edi
		push    eax
		call    mode_5_in
		lea     ecx, [esi + 824h]
		push    eax
		push    ecx
		mov     edx, 00508479h
		jmp     edx
	}
}

int __stdcall mode_6_in(int mode, int v13) //returns offset, 34935 + this_n = something ID???
{
	int offset;
	switch (mode)
	{
	case 1:
		offset = 200;
		break;
	case 2:
		offset = 300;
		break;
	case 3:
		offset = 200;
		break;
	default:
		offset = 0;
		break;
	}
	return offset + 30 * v13 + 165971;
}

__declspec(naked) void mode_6() //00508FA3
{
	__asm
	{
		push    eax
		push    esi
		call    mode_6_in
		mov     [esp + 18h], ebx
		mov     ebp, eax
		mov     edx, 00508FC6h
		jmp     edx
	}
}

__declspec(naked) void mode_7() //0050910D
{
	__asm
	{
		cmp     eax, 3
		jz      _ef_mode_7
		mov     edi, 00690C80h
		jz      _ef_mode_7_cont
_ef_mode_7:
		mov     edi, offset a2cam
_ef_mode_7_cont:
		or      ecx, 0FFFFFFFFh
		mov     eax, 00509115h
		jmp     eax
	}
}

__declspec(naked) void mode_8() //0050919F
{
	__asm
	{
		mov     ecx, 006A3684h
		mov     ecx, [ecx]
		mov     eax, 005E8000h
		call    eax
		cmp     eax, 3
		jz      _ef_mode_8
		lea     edx, [edi + esi * 4 - 4]
		jmp     _ef_mode_8_cont
_ef_mode_8:
		lea     edx, [esi + 54009]    //54010 - 54015: scenario button positions
_ef_mode_8_cont:
		mov     ecx, 62696E61h
		mov     eax, 005091A8h
		jmp     eax
	}
}

int __fastcall mode_9_in(int mode)		//num_scenarios + 6 ???
{
	switch (mode)
	{
	case 0:
	case 1:
		return 8;
	case 2:
		return 0xC;
	case 3:
		return 0xC;
	default:
		return 8;
	}
}

__declspec(naked) void mode_9() //0050928C
{
	__asm
	{
		mov     ecx, eax
		call    mode_9_in
		mov     ecx, 00509298h
		jmp     ecx
	}
}

char a2c[] = "2c";

__declspec(naked) void mode_A() //0052440E
{
	__asm
	{
		cmp     eax, 3
		jz      _ef_mode_A
		push    00691E00h
		jmp     _ef_mode_A_cont
_ef_mode_A:
		push    offset a2c
_ef_mode_A_cont:
		push    ecx
		mov     eax, 00524414h
		jmp     eax
	}
}

char a2b[] = "2b";

__declspec(naked) void mode_B() //0052459A
{
	__asm
	{
		cmp     eax, 3
		jz      _ef_mode_B
		push    00691D64h
		jmp     _ef_mode_B_cont
_ef_mode_B:
		push    offset a2b
_ef_mode_B_cont:
		push    ecx
		mov     eax, 005245A0h
		jmp     eax
	}
}

char a2[] = "2";

__declspec(naked) void mode_C() //0052461B
{
	__asm
	{
		cmp     eax, 3
		lea     eax, [esp + 10h]
		jz      _ef_mode_C
		push    00691D4Ch
		jmp     _ef_mode_C_cont
_ef_mode_C:
		push    offset a2
_ef_mode_C_cont:
		push    eax
		mov     eax, 00524625h
		jmp     eax
	}
}

void setCampaignHooks()
{
	writeDword(0x00511294, (DWORD)onButtonTest);
	//writeDword(0x00511294, 0x005105C4);
	writeByte(0x00510261, 0x14);
	setHook((void*)0x0050BA3A, onCreateButtons);
	setHook((void*)0x0050D13E, onDeleteButtons);
	setHook((void*)0x0050F9E8, onReleaseCapture);
	setHook((void*)0x0050EEFF, onButtonSet);
	setHook((void*)0x0050F294, onHelpText);
	setHook((void*)0x0050E581, onButtonPress);

	setHook((void*)0x00507670, mode_1);
	setHook((void*)0x00507CC8, mode_2);
	setHook((void*)0x00507F6E, mode_3);
	setHook((void*)0x00508354, mode_4);
	setHook((void*)0x00508452, mode_5);
	setHook((void*)0x00508FA3, mode_6);
	setHook((void*)0x0050910D, mode_7);
	setHook((void*)0x0050919F, mode_8);
	setHook((void*)0x0050928C, mode_9);
	setHook((void*)0x0052440E, mode_A);
	setHook((void*)0x0052459A, mode_B);
	setHook((void*)0x0052461B, mode_C);
}
