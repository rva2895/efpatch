#include "stdafx.h"
#include "overlay.h"

void* overlay_ptr = NULL;

int pos_x = 400;
int pos_y = 600;
int size_x = 1000;
int size_y = 300;

HBRUSH br_black;
bool brushes_loaded = false;

bool overlay_state = false;

__declspec(naked) void onOverlayInit() //004F583C
{
	__asm
	{
		//set rect
		mov     ecx, overlay_ptr
		push    size_y
		push    size_x
		push    pos_y
		push    pos_x
		mov     eax, 004B5F60h
		call    eax
		//
		mov     ecx, overlay_ptr
		mov     eax, 005DF850h
		call    eax
		mov     ecx, overlay_ptr
		push    0
		push    0
		mov     eax, 005DF880h
		call    eax
		//
		lea     edi, [esi + 11A0h]
		mov     ecx, 004F5842h
		jmp     ecx
	}
}

__declspec(naked) int __fastcall get_n_players()
{
	__asm
	{
		mov     ecx, 006A3684h
		mov     ecx, [ecx]
		mov     ecx, [ecx + 17B4h]
		mov     ecx, [ecx + 126Ch]
		movsx   eax, byte ptr [ecx + 48h]
		ret
	}
}

__declspec(naked) void* __fastcall get_player(int)
{
	__asm
	{
		mov     edx, ecx
		mov     ecx, 006A3684h
		mov     ecx, [ecx]
		mov     ecx, [ecx + 17B4h]
		mov     ecx, [ecx + 126Ch]
		mov     ecx, [ecx + 4Ch]
		mov     eax, [ecx + edx * 4]
		ret
	}
}

__declspec(naked) float* __fastcall get_player_resources(void*)
{
	__asm
	{
		mov     eax, [ecx + 0ACh]
		ret
	}
}

__declspec(naked) int __fastcall get_player_units_count(void*)
{
	__asm
	{
		mov     ecx, [ecx + 78h]
		mov     eax, [ecx + 8]
		ret
	}
}

__declspec(naked) UNIT* __fastcall get_player_units(void*)
{
	__asm
	{
		mov     ecx, [ecx + 78h]
		mov     eax, [ecx + 4]
		ret
	}
}

__declspec(naked) int __fastcall get_gametime()
{
	__asm
	{
		mov     ecx, 006A3684h
		mov     ecx, [ecx]
		mov     ecx, [ecx + 17B4h]
		mov     ecx, [ecx + 126Ch]
		mov     eax, [ecx + 10h]
		ret
	}
}

void print_graph(HDC hdc, std::vector<std::vector<std::pair<int, int>>> data)
{

}

void __stdcall window_overlay_draw2(HDC hdc)
{
	if (!brushes_loaded)
	{
		br_black = CreateSolidBrush(RGB(32, 32, 32));
		brushes_loaded = true;
	}

	//get_player_units(get_player(1));

	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = size_x;
	r.bottom = size_y;
	FillRect(hdc, &r, br_black);
	
	SetTextColor(hdc, RGB(255, 255, 255));
	char b[0x100];
	//sprintf(b, "%d players", );
	//TextOut(hdc, 0, 0, b, strlen(b));

	int n_players = get_n_players();
	sprintf(b, "Time: %d", get_gametime());
	TextOut(hdc, 0, 0, b, strlen(b));
	for (int i = 1; i < n_players; i++)
	{
		float* res = get_player_resources(get_player(i));
		sprintf(b, "Player %d: carbon gathered: %d", i, (int)res[167]);
		TextOut(hdc, 0, 20 * i, b, strlen(b));
	}
}

__declspec(naked) void window_overlay_render()
{
	__asm
	{
		sub     esp, 154h
		push    ebp
		push    esi
		mov     esi, ecx
		xor     ebp, ebp
		push    edi
		mov     ecx, [esi + 138h]
		cmp     ecx, ebp
		jnz     short loc_5E0023
		pop     edi
		pop     esi
		xor     eax, eax
		pop     ebp
		add     esp, 154h
		retn

loc_5E0023:
		lea     edi, [esi + 13Ch]
		push    1
		push    edi
		mov     [esp + 14h], ebp
		mov     eax, 00473090h
		call    eax
		mov     ecx, [esi + 138h]
		push    0
		mov     eax, 00472B60h
		call    eax
		test    eax, eax
		jnz     short loc_5E0053
		pop     edi
		pop     esi
		pop     ebp
		add     esp, 154h
		retn

loc_5E0053:
		mov     ecx, [esi + 138h]
		push    ebx
		push    edi
		mov     eax, 00473370h
		call    eax
		mov     ecx, [esi + 138h]
		mov     eax, [esi + 134h]
		push    eax
		mov     edx, [ecx + 38h]
		push    edx
		call    ds:SelectClipRgn
		mov     ecx, [esi + 138h]
		mov     eax, [esi + 0F8h]
		push    eax
		mov     edx, [ecx + 38h]
		push    edx
		call    ds:SelectObject
		mov     [esp + 24h], eax
		mov     eax, [esi + 138h]
		push    1
		mov     ecx, [eax + 38h]
		push    ecx
		call    ds:SetBkMode
		//
		mov     eax, [esi + 138h]
		mov     ecx, [eax + 38h]
		push    ecx
		call    window_overlay_draw2
		//
		mov     ecx, [esi + 138h]
		mov     eax, [esp + 24h]
		push    eax
		mov     edx, [ecx + 38h]
		push    edx
		call    ds:SelectObject
		mov     eax, [esi + 138h]
		xor     edi, edi
		push    edi
		mov     ecx, [eax + 38h]
		push    ecx
		call    ds:SelectClipRgn
		mov     ecx, [esi + 138h]
		push    0
		mov     eax, 00472BB0h
		call    eax
		mov     eax, [esi + 14h]
		mov     edx, [esi + 90h]
		mov     ecx, [esi + 18h]
		mov     [esi + 14Ch], edi
		mov     [esi + 150h], edi
		mov     edi, [esi + 94h]
		dec     eax
		mov     [esi + 15Ch], edx
		dec     ecx
		add     edx, eax
		mov     [esi + 160h], edi
		mov     [esi + 154h], eax
		mov     eax, [esi + 104h]
		mov     [esi + 158h], ecx
		mov     [esi + 164h], edx
		mov     edx, [esi + 12Ch]
		add     edi, ecx
		mov     ecx, [esi + 10Ch]
		mov     [esi + 168h], edi
		pop     ebx
		mov     [esi + 130h], edx
		mov     [esi + 108h], eax
		mov     [esi + 110h], ecx
		mov     dword ptr [esi + 16Ch], 1
		pop     edi
		pop     esi
		mov     eax, 1
		pop     ebp
		add     esp, 154h
		retn
	}
}

__declspec(naked) void window_overlay_draw()
{
	__asm
	{
		push    ebp
		mov     ebp, esp
		sub     esp, 12Ch
		push    esi
		mov     esi, ecx
		mov     eax, [esi + 20h]
		test    eax, eax
		jz      loc_5DFE21
		mov     eax, [esi + 70h]
		test    eax, eax
		jz      loc_5DFE21
		mov     eax, [esi + 6Ch]
		test    eax, eax
		jz      loc_5DFE21
		mov     edx, [esi + 0E4h]
		mov     eax, [esi]
		inc     edx
		push    0
		mov     [esi + 0E4h], edx
		call    dword ptr [eax + 34h]
		mov     ecx, [esi + 10Ch]
		mov     edx, [esi + 124h]
		cmp     ecx, 1
		jnz     short loc_5DF9FA
		mov     [esi + 12Ch], edx
		jmp     short loc_5DFA10

loc_5DF9FA:
		add     edx, 4
		mov     eax, 0CCCCCCCDh
		mul     edx
		shr     edx, 2
		lea     eax, [edx + edx * 4]
		mov     [esi + 12Ch], eax

loc_5DFA10:
		mov     eax, [esi + 138h]
		test    eax, eax
		jz      short loc_5DFA70
		mov     edx, [esi + 12Ch]
		mov     eax, [esi + 130h]
		cmp     edx, eax
		jnz     short loc_5DFA4C
		mov     eax, [esi + 104h]
		mov     edx, [esi + 108h]
		cmp     eax, edx
		jnz     short loc_5DFA4C
		cmp     ecx, [esi + 110h]
		jnz     short loc_5DFA4C
		mov     al, [esi + 0F7h]
		test    al, al
		jz      short loc_5DFA70

loc_5DFA4C:
		mov     ecx, esi
		call    window_overlay_render		//new func
		mov     al, [esi + 0F7h]
		test    al, al
		jz      short loc_5DFA70
		mov     edx, [esi]
		mov     ecx, esi
		mov     byte ptr [esi + 0F7h], 0
		call    dword ptr [edx + 38h]
		pop     esi
		mov     esp, ebp
		pop     ebp
		retn

loc_5DFA70:
		mov     eax, [esi + 16Ch]
		test    eax, eax
		jz      short loc_5DFACE
		mov     ecx, [esi + 138h]
		test    ecx, ecx
		jz      short loc_5DFACE
		push    1
		push    1
		mov     eax, 00473BD0h
		call    eax
		mov     edx, [esi + 160h]
		mov     eax, [esi + 15Ch]
		lea     ecx, [esi + 14Ch]
		push    1
		push    ecx
		mov     ecx, [esi + 20h]
		push    edx
		push    eax
		push    ecx
		mov     ecx, [esi + 138h]
		mov     eax, 00473590h
		call    eax
		mov     ecx, [esi + 138h]
		push    1
		push    0
		mov     eax, 00473BD0h
		call    eax
		mov     edx, [esi]
		mov     ecx, esi
		call    dword ptr [edx + 38h]

loc_5DFACE:
loc_5DFE21:
		pop     esi
		mov     esp, ebp
		pop     ebp
		retn
	}
}

void __stdcall replace_overlay_vtable()
{
	void* old_vtable = *(void**)overlay_ptr;
	void* new_vtable = malloc(0x184);
	memcpy(new_vtable, old_vtable, 0x184);
	((DWORD*)new_vtable)[15] = (DWORD)&window_overlay_draw;
	*(void**)overlay_ptr = new_vtable;

	overlay_state = false;
}

void __stdcall delete_vtable()
{
	free(*(void**)overlay_ptr);
	*(void**)overlay_ptr = (void*)0x00668478;

	overlay_state = false;
}

__declspec(naked) void onOverlayCreate() //004F45C1
{
	__asm
	{
		mov     eax, 005DF500h
		call    eax
		push    eax
		//
		push    178h
		mov     eax, 00632B9Dh
		call    eax
		add     esp, 4
		mov     overlay_ptr, eax
		mov     ecx, [esi + 10BCh]
		mov     edx, [esi + 20h]
		push    ecx
		push    edx
		mov     ecx, eax
		mov     eax, 005DF500h
		call    eax
		call    replace_overlay_vtable
		//
		pop     eax
		mov     ecx, 004F45CAh
		jmp     ecx
	}
}

__declspec(naked) void onOverlayDelete() //004F5F52
{
	__asm
	{
		mov     ecx, overlay_ptr
		test    ecx, ecx
		jz      _no_overlay_delete
		push    ecx
		call    delete_vtable
		pop     ecx
		mov     eax, [ecx]
		push    1
		call    dword ptr [eax]
		xor     eax, eax
		mov     overlay_ptr, eax
_no_overlay_delete:
		//
		mov     dword ptr [esi + 119Ch], 0
		mov     edi, 004F5F5Ch
		jmp     edi
	}
}

#pragma optimize( "s", on )
void setOverlayHooks()
{
	setHook((void*)0x004F45C1, onOverlayCreate);
	setHook((void*)0x004F5F52, onOverlayDelete);
	setHook((void*)0x004F583C, onOverlayInit);
}
#pragma optimize( "", on )

__declspec(naked) void __stdcall overlay_on()
{
	__asm
	{
		mov     ecx, overlay_ptr
		push    0
		push    2
		mov     eax, 005DF880h
		call    eax
		ret
	}
}

__declspec(naked) void __stdcall overlay_off()
{
	__asm
	{
		mov     ecx, overlay_ptr
		push    0
		push    0
		mov     eax, 005DF880h
		call    eax
		ret
	}
}

void overlay_switch()
{
	if (overlay_ptr)
	{
		if (overlay_state)
		{
			overlay_off();
			overlay_state = false;
		}
		else
		{
			overlay_on();
			overlay_state = true;
		}
	}
}
