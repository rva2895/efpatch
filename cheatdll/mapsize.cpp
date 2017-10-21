#include "stdafx.h"
#include "mapsize.h"

#define MAP_MAX 0x300

__declspec(naked) void onEditorMapSize_legacy() //0052A123
{
	__asm
	{
		push	esi
		mov		esi, 4C82D0h
		mov     ecx, [edi]
		push    0
		push    2978h	//giant
		call    esi
		mov     ecx, [edi]
		push	0
		push	2979h	//huge
		call	esi
		pop		esi
		push	0052A131h
		ret
	}
}

__declspec(naked) void onEditorMapSize() //0052A123
{
	__asm
	{
		push	esi
		mov		esi, 4C82D0h
		mov     ecx, [edi]
		push    0
		push    2978h	//giant 240
		call    esi
		mov     ecx, [edi]
		push	0
		push	2979h	//huge 255
		call	esi
		mov     ecx, [edi]
		push	0
		push	297Ah	//massive 320
		call	esi
		mov     ecx, [edi]
		push	0
		push	297Bh	//enormous 400
		call	esi
		mov     ecx, [edi]
		push	0
		push	10620	//colossal 480
		call	esi
		mov     ecx, [edi]
		push	0
		push	10610	//titanic 560
		call	esi
		pop		esi
		push	0052A131h
		ret
	}
}

__declspec(naked) void jmp_320()
{
	__asm
	{
		mov		esi, 320
		push	0052EC76h
		ret
	}
}

__declspec(naked) void jmp_400()
{
	__asm
	{
		mov		esi, 400
		push	0052EC76h
		ret
	}
}

__declspec(naked) void jmp_480()
{
	__asm
	{
		mov		esi, 480
		push	0052EC76h
		ret
	}
}

__declspec(naked) void jmp_560()
{
	__asm
	{
		mov		esi, 560
		push	0052EC76h
		ret
	}
}

int map_sizes_jmp[] =
{
	0x52EC2D,
	0x52EC46,
	0x52EC4F,
	0x52EC58,
	0x52EC61,
	0x52EC6A,
	0x52EC71,
	(int)(&jmp_320),
	(int)(&jmp_400),
	(int)(&jmp_480),
	(int)(&jmp_560)
};

int mapsize_current_sel = 0;

__declspec(naked) void maplist_getSel() //00521B57
{
	__asm
	{
		cmp		eax, 5
		ja		_large_maps
		push	eax
		jmp		_large_maps_cont
_large_maps:
		push	5
_large_maps_cont:
		mov		mapsize_current_sel, eax
		mov		eax, 005EEC80h
		call	eax
		mov		ecx, 00521B5Dh
		jmp		ecx
	}
}

int __stdcall maplist_getMapSize(short old_size)
{
	switch (mapsize_current_sel)
	{
	case 6:
		return 255;
		break;
	case 7:
		return 320;
		break;
	case 8:
		return 400;
		break;
	case 9:
		return 480;
		break;
	case 10:
		return 560;
		break;
	default:
		return old_size;
	}
}

__declspec(naked) void maplist_interceptSel() //005ECC77
{
	__asm
	{
		push	eax
		call	maplist_getMapSize
		mov		ecx, ebp
		mov		[esp + 2FEh], ax
		mov		eax, 0042C320h
		call	eax
		push	eax
		call	maplist_getMapSize
		mov		[esp + 300h], ax
		mov		edx, 005ECC8Ch
		jmp		edx
	}
}

__declspec(naked) void maplist_make() //00520043
{
	__asm
	{
		push	edi
		mov		edi, 004C82D0h
		mov		ecx, [esi + 0B08h]
		push	5					//giant (240)
		push	10616
		call	edi
		mov		ecx, [esi + 0B08h]	//huge (255)
		push	6
		push	10617
		call	edi
		mov		ecx, [esi + 0B08h]	//320
		push	7
		push	10618
		call	edi
		mov		ecx, [esi + 0B08h]	//400
		push	8
		push	10619
		call	edi
		mov		ecx, [esi + 0B08h]	//480
		push	9
		push	10620
		call	edi
		mov		ecx, [esi + 0B08h]	//560
		push	10
		push	10610
		call	edi
		pop		edi
		mov		eax, 00520055h
		jmp		eax
	}
}

int* mapptr = (int*)0x7A1CBC;
int* mapptr_2 = (int*)0x7A1CB8;
int* mapptr_3 = (int*)0x7A1CC0;

//var_9 -> var_a
//var_a -> var_c
//arg_18+3 -> ebp + 22h

struct map_open_paths_
{
	WORD	x;
	WORD	y;
	int		n;
};

map_open_paths_* map_open_paths;
int* map_cost;

//offsets
#define cn_x 8
#define cn_y 10
#define cn_n 12

#define o_x 0
#define o_y 2
#define o_n 4

__declspec(naked) void p1() //004BDCDE
{
	__asm
	{
		mov		edi, map_open_paths
		//mov     dx, [esi + eax * 8 + 4F61Ch]
		mov		dx, [edi + eax * 8 + o_x]
		//mov		[esi + 4F624h], dx
		mov		[edi + cn_x], dx
		mov     cx, [edi + eax * 8 + o_y]
		mov		[edi + cn_y], cx
		mov     edx, [edi + eax * 8 + o_n]
		lea     ecx, [eax - 1]
		mov		[edi + cn_n], edx
		mov		[esi + 0CE62Ch], ecx
		mov     ax, [edi + cn_x]
		mov     dx, [edi + cn_y]
		mov     word ptr[ebp + 22h], ax
		mov     eax, [esi + cn_n]
		mov		[ebp - 0Ah], dx
		push	004BDD26h
		ret
	}
}

__declspec(naked) void p2() //004BDD97
{
	__asm
	{
		mov     eax, [ebp - 1Ch]
		mov     cx, word ptr[ebp + 22h]
		mov		edx, map_open_paths
		mov		[edx + ebx * 8 + o_x], cx
		mov     cx, [ebp - 0Ah]
		mov		[edx + ebx * 8 + o_y], cx
		mov		[edx + ebx * 8 + o_n], eax
		push	004BDDB5h
		ret
	}
}

//var_8+4

__declspec(naked) void p3() //004BE58C
{
	__asm
	{
		mov		edi, map_open_paths
		mov		[edi + eax * 8 + o_x], bx
		mov     edx, [esi + 0CE62Ch]
		mov     ax, word ptr[ebp - 4]
		mov		[edi + edx * 8 + o_y], ax
		mov     edx, [esi + 0CE62Ch]
		mov		[edi + edx * 8 + o_n], ecx
		mov     edi, [esi + 0CE62Ch]
		push	004BE5B6h
		ret
	}
}

__declspec(naked) void p4() //004BE5C6
{
	__asm
	{
		push	esi
		mov		esi, map_open_paths
		mov     dx, [esi + edi * 8 + o_y]
		mov     ebx, eax
		mov     ax, [esi + edi * 8 + o_x]
		sar     ebx, 1
		mov		[ebp - 0Ah], ax
		mov     eax, [esi + edi * 8 + o_n]
		mov		[ebp - 0Ch], dx
		mov     edx, [esi + ebx * 8 + o_n]
		pop		esi
		push	004BE5ECh
		ret
	}
}

__declspec(naked) void p5() //004BE631
{
	__asm
	{
		mov		edi, map_open_paths
		mov     dx, [ebp - 0Ah]
		mov     ax, [ebp - 0Ch]
		mov		[edi + ecx * 8 + o_x], dx
		mov     edx, [ebp - 28h]
		mov		[edi + ecx * 8 + o_y], ax
		mov		[edi + ecx * 8 + o_n], edx
		push	004BE64Fh
		ret
	}
}

//var_4C -> var_4C
//var_4B -> var_4A
//arg_2F -> ebp + 36h

__declspec(naked) void p6() //004BE73F
{
	__asm
	{
		mov		edi, map_open_paths
		mov     cx, [ebp - 4Ch]
		mov		[edi + eax * 8 + o_x], cx
		mov     edx, [esi + 0CE62Ch]
		mov     ax, [ebp - 4Ah]
		mov		[edi + edx * 8 + o_y], ax
		mov     ecx, [esi + 0CE62Ch]
		mov     dword ptr[edi + ecx * 8 + o_n], 0
		push	004BE76Ah
		ret
	}
}

//arg_8 + 3 -> ebp + 12h

__declspec(naked) void p7() //004BE787
{
	__asm
	{
		push	esi
		mov		esi, map_open_paths
		mov     dx, [esi + eax * 8 + o_x]
		mov		[ebp + 36h], dx
		mov     dx, [esi + eax * 8 + o_y]
		mov     eax, [esi + eax * 8 + o_n]
		mov     word ptr[ebp + 12h], dx
		mov     edx, [esi + edi * 8 + o_n]
		pop		esi
		push	004BE7A9h
		ret
	}
}

__declspec(naked) void p8() //004BE7EF
{
	__asm
	{
		mov		edi, map_open_paths
		mov     dx, [ebp + 36h]
		mov     ax, word ptr[ebp + 12h]
		mov		[edi + ecx * 8 + o_x], dx
		mov     edx, [ebp + 14h]
		mov		[edi + ecx * 8 + o_y], ax
		mov		[edi + ecx * 8 + o_n], edx
		push	004BE80Dh
		ret
	}
}

//

__declspec(naked) void p9() //004BDAEF
{
	__asm
	{
		mov		eax, map_open_paths
		mov		[eax + cn_x], bx
		mov		[ebp + 20h], ecx
		mov     dword ptr[eax + o_n], 88CA6C00h
		fild	[ebp + 20h]
		mov		[ebp - 4Ch], bx
		push	004BDB08h
		ret
	}
}

__declspec(naked) void p10() //004BDB9E
{
	__asm
	{
		mov		edi, map_open_paths
		mov     ax, [edi + cn_x]
		mov     cx, [edi + cn_y]
		push	004BDBAAh
		ret
	}
}

__declspec(naked) void p11() //004BDCB6
{
	__asm
	{
		mov		ebx, map_open_paths
		mov     dx, [ebx + cn_x]
		mov     cx, [ebx + cn_y]
		mov		[ebp - 4Ch], dx
		mov		[ebp - 4Ah], cx
		push	004BDCC8h
		ret
	}
}

__declspec(naked) void p12() //004BDAC8
{
	__asm
	{
		push	esi
		mov		esi, map_cost
		mov		[ebp - 4Ah], cx
		mov		[esi + edx * 4], eax //map_cost
		mov		esi, map_open_paths
		mov		[esi + cn_n], eax
		mov     eax, [ebp + 10h]
		mov		[esi + cn_y], cx
		pop		esi
		push	004BDAE1h
		ret
	}
}

__declspec(naked) void p13() //004BE48D
{
	__asm
	{
		xor     cx, cx
		inc     eax
		cmp     edi, eax
		mov		[ebp - 0Ch], cx
		push	004BE495h
		ret
	}
}

__declspec(naked) void p14() //004BDFB0
{
	__asm
	{
		mov     word ptr[ebp + 22h], 0
		cmp     ebx, ecx
		push	004BDFB6h
		ret
	}
}

__declspec(naked) void p15() //004BE103
{
	__asm
	{
		xor     cx, cx
		cmp     ebx, edx
		mov     word ptr[ebp + 22h], cx
		push	004BE10Ah
		ret
	}
}

__declspec(naked) void p16() //004BE543
{
	__asm
	{
		cmp     word ptr[ebp + 22h], 1
		jnz     loc_4BE54B
		xor     ecx, ecx
loc_4BE54B:
		push	004BE54Bh
		ret
	}
}

__declspec(naked) void p17() //004BE64F
{
	__asm
	{
		cmp     word ptr[ebp + 22h], 1
		jz		loc_4BE68A
		push	004BE655h
		ret
loc_4BE68A:
		push	004BE68Ah
		ret
	}
}

void fix_sub_4bd6a0()
{
	setByte(0x004BDFD3, 0x22);
	setByte(0x004BE129, 0x22);

}

// ****************************************** */

//+1C -> +1C
//+1D -> +1E

__declspec(naked) void p1_() //004BE9A5
{
	__asm
	{
		mov		edi, map_open_paths
		mov     dx, [edi + cn_x]
		mov     ax, [edi + cn_y]
		mov     word ptr[esp + 1Ch], dx
		mov     word ptr[esp + 1Eh], ax
		or		cl, 0FFh
		mov     edi, [esp + 1Ch]
		mov     ebp, [esp + 1Eh]
		and     edi, 0FFFFh
		and     ebp, 0FFFFh
		mov     edx, edi
		mov		[esp + 14h], edi
		shl     edx, 8
		lea		edx, [edx + edx * 2]
		push	004BE9DBh
		ret
	}
}

__declspec(naked) void p2_() //004BEB56
{
	__asm
	{
		mov     cx, word ptr[esp + 1Ch]
		mov     ax, word ptr[esp + 1Eh]
		inc     cx
		inc     ax
		mov     word ptr[esp + 1Ch], cx
		mov     word ptr[esp + 1Eh], ax
		push	004BEBA8h
		ret
	}
}

__declspec(naked) void p3_() //004BEB6C
{
	__asm
	{
		dec     word ptr[esp + 1Ch]
		inc     word ptr[esp + 1Eh]
		push	004BEBA8h
		ret
	}
}

__declspec(naked) void p3_x() //-none-
{
	__asm
	{
		inc     word ptr[esp + 1Eh]
		push	004BEBA8h
		ret
	}
}

__declspec(naked) void p4_() //004BEB76
{
	__asm
	{
		mov     ax, word ptr[esp + 1Ch]
		dec     ax
		mov     word ptr[esp + 1Ch], ax
		push	004BEBA8h
		ret
	}
}

__declspec(naked) void p5_() //004BEB7E
{
	__asm
	{
		mov     cx, word ptr[esp + 1Ch]
		mov     ax, word ptr[esp + 1Eh]
		dec     cx
		dec     ax
		mov     word ptr[esp + 1Ch], cx
		mov     word ptr[esp + 1Eh], ax
		push	004BEBA8h
		ret
	}
}

__declspec(naked) void p6_() //004BEB94
{
	__asm
	{
		inc     word ptr[esp + 1Ch]
		dec     word ptr[esp + 1Eh]
		push	004BEBA8h
		ret
	}
}

__declspec(naked) void p6_x() //-none-
{
	__asm
	{
		dec     word ptr[esp + 1Eh]
		push	004BEBA8h
		ret
	}
}

__declspec(naked) void p7_() //004BEB9E
{
	__asm
	{
		mov     ax, word ptr[esp + 1Ch]
		inc     ax
		mov     word ptr[esp + 1Ch], ax
		push	004BEBA8h
		ret
	}
}

//__declspec(naked) void p8_() //
//{
//	__asm
//	{
//	}
//}

void fix_sub_4be980()
{
	setInt(0x004BEC4C, (int)&p3_x);
	setInt(0x004BEC5C, (int)&p6_x);
	setByte(0x004BEBAF, 0x1E);
	setInt(0x4BEBB2, 0xFFFF);
	setInt(0x4BEBB8, 0xFFFF);
	
	//setByte(0x004BEBC4, 16);
}

void __stdcall map_init()
{
	memset(map_cost, 0xFF, sizeof(int)*MAP_MAX*MAP_MAX);
	memset(map_open_paths, 0xFF, sizeof(map_open_paths_)*MAP_MAX*MAP_MAX);
}

__declspec(naked) void map_init_hook() //004BD63E
{
	__asm
	{
		mov     ecx, 24000h
		or		eax, 0FFFFFFFFh
		lea     edi, [edx + 14h]
		rep stosd
		push	edx
		call	map_init
		pop		edx
		push	004BD66Ch
		ret
	}
}

__declspec(naked) void facet_clear_hook() //004BDA3B
{
	__asm
	{
		mov     ecx, 24000h
		or		eax, 0FFFFFFFFh
		lea     edi, [esi + 14h]
		mov     byte ptr[esi + 0CE634h], 0
		rep stosd
		push	004BDA5Ah
		ret
	}
}

//facet references
//implements multiplication by 0x300
//facet array is assumed to be [0x300][0x300] instead of [255][255]

__declspec(naked) void facet1() //004BDDBA
{
	__asm
	{
		mov     edx, eax
		mov		[ebp - 18h], ecx
		shl     edx, 8
		lea		edx, [edx + edx * 2]
		push	004BDDC4h
		ret
	}
}

__declspec(naked) void facet2() //004BDF60
{
	__asm
	{
		shl     ecx, 8
		lea		ecx, [ecx + ecx * 2]
		add     eax, edi
		push	004BDF67h
		ret
	}
}

__declspec(naked) void facet3() //004BEBC2
{
	__asm
	{
		shl     edx, 8
		lea     edx, [edx + edx * 2]
		mov		[esp + 14h], edi
		push	004BEBCBh
		ret
	}
}

__declspec(naked) void onPathFind_shift() //004B0987
{
	__asm
	{
		and     edx, 3FFh
		mov		ebx, ecx
		shr		ebx, 10
		and		ebx, 3FFh
		mov		ecx, 004B0991h
		jmp		ecx
	}
}

//ebp struct: 4 byte, 1 byte, 1 byte (unused), 1 byte x, 1 byte y
//ebp+5 = unused var, 4 low bits = x, 4 high bits = y

__declspec(naked) void findres_onX() //006150CB
{
	__asm
	{
		mov		al, [ebp + 5]
		shl		eax, 8
		mov		al, [ebp + 6]
		and		eax, 0FFFh
		mov		esi, 006150D0h
		jmp		esi
	}
}

__declspec(naked) void findres_onY() //006150E0
{
	__asm
	{
		mov		al, [ebp + 5]
		shl		eax, 4
		mov		al, [ebp + 7]
		and		eax, 0FFFh
		mov		ecx, 006150E5h
		jmp		ecx
	}
}

__declspec(naked) void findres_onPlayerUpdate() //00614FD4
{
	__asm
	{
		mov		ecx, [esp + 20h]
		mov		[esi], edx
		mov     word ptr[esi + 4], 0
		mov		[esi + 6], cl
		mov		[esi + 7], al
		shl		eax, 4
		and		eax, 0F000h
		or		ecx, eax
		mov		[esi + 5], ch
		//mov		esi, 00614FE8h
		//jmp		esi
		push	00614FE8h
		ret
	}
}

//+5: player, use 4 lower bits, then 2: x, next 2: y

__declspec(naked) void visible_unit_managerUpdate() //00614966
{
	__asm
	{
		mov		ebx, [esp + 30h]	//y
		shl		ebx, 10
		mov		bx, [esp + 2Ch]		//x
		shr		ebx, 8
		and		ebx, 0C03h
		or		bl, bh
		shl		ebx, 12
		mov		bl, [esp + 28h]		//player
		and		bl, 0Fh
		or		bl, bh
		//or		bl, 0F0h				//TEST, REMOVE
		//and		bl, 5Fh					//TEST, REMOVE
		mov		[eax + 5], bl
		mov		ebx, [esp + 2Ch]	//x
		mov		[eax + 6], bl
		mov		ebx, [esp + 30h]	//y
		mov		[eax + 7], bl
		mov		ebx, [esp + 34h]	//radius
		mov		[eax + 4], bl
		//mov		ebx, 00614982h
		//jmp		ebx
		push	00614982h
		ret
	}
}

__declspec(naked) void visible_unit_managerOnX() //00614AC1
{
	__asm
	{
		xor		eax, eax
		mov		al, [esi + 5]
		shl		eax, 4
		mov		al, [esi + 6]
		and		eax, 3FFh
		//and		eax, 0FFh			//TEST, REMOVE
		mov		edx, 00614AC9h
		jmp		edx
	}
}

__declspec(naked) void visible_unit_managerOnY() //00614AEC
{
	__asm
	{
		xor		eax, eax
		and		edx, 1Fh
		mov		al, [esi + 5]
		shl		eax, 2
		mov		al, [esi + 7]
		//and		eax, 0FFh			//TEST, REMOVE
		mov		ecx, 00614AF7h
		jmp		ecx
	}
}

__declspec(naked) void visible_unit_managerOnPlayer() //00614B43
{
	__asm
	{
		mov		edx, eax
		mov		al, [esi + 5]
		and		eax, 0Fh
		mov		edi, 00614B48h
		jmp		edi
	}
}

//references to visible unit ptr at 007A1CA0
__declspec(naked) void visible_unit_ptr_fix_1() //0041BCC6
{
	__asm
	{
		mov     dl, [ecx + edi * 8 + 5]
		and		dl, 0Fh
		lea     eax, [ecx + edi * 8]
		mov		ecx, 0041BCCDh
		jmp		ecx
	}
}

__declspec(naked) void ai_findres() //005873A4
{
	__asm
	{
		xor		edx, edx
		xor		eax, eax
		mov		dh, [ebx + 7]	//padding byte
		mov		ah, dh
		shr		edx, 2
		mov		dl, [ebx + 5]	//y
		mov		al, [ebx + 4]	//x
		and		eax, 3FFh
		push	005873AEh
		ret
	}
}

__declspec(naked) void ai_findres_under_attack() //00591D51
{
	__asm
	{
		lea		edx, [eax + edi]
		mov		ch, [edx + 7]	//padding byte
		mov		cl, [edx + 4]	//x
		and		ecx, 3FFh
		mov		eax, [esp + 1Ch]
		sub		eax, ecx
		xor		ecx, ecx
		mov		ch, [edx + 7]
		shr		ecx, 2
		mov		cl, [edx + 5]	//y
		push	00591D63h
		ret
	}
}

__declspec(naked) void ai_findres_init_1() //00590B86
{
	__asm
	{
		lea		ecx, [ecx + ecx * 2]
		mov		[edx + ecx * 8 + 4], al
		and		eax, 300h
		mov		[edx + ecx * 8 + 7], ah		//low 2 bits
		push	00590B8Dh
		ret
	}
}

__declspec(naked) void ai_findres_init_2() //00590BA3
{
	__asm
	{
		lea		ecx, [ecx + ecx * 2]
		mov		[edx + ecx * 8 + 5], al
		shr		eax, 6
		and		eax, 0Ch
		or		byte ptr [edx + ecx * 8 + 7], al		//high 2 bits
		push	00590BAAh
		ret
	}
}

__declspec(naked) void ai_findres_onCopy() //00590A43
{
	__asm
	{
		mov		ecx, [esi + ebx * 4 + 1F24h]
		mov		cl, [eax + ecx - 12h]
		mov		[edi - 17h], cl
		mov		ecx, [esi + ebx * 4 + 1F24h]
		mov		cl, [eax + ecx - 11h]
		mov		[edi - 16h], cl
		push	00590A5Ch
		ret
	}
}

__declspec(naked) void ai_findres_save() //0058670C
{
	__asm
	{
		mov		ecx, [edi - 20h]
		push	1
		lea     edx, [ecx + ebp + 7]
		mov		ecx, esi
		mov		eax, 004D5790h
		call	eax
		mov		edx, [edi - 20h]
		push	4
		mov		ecx, esi
		push	00586713h
		ret
	}
}

__declspec(naked) void ai_findres_load() //00584A5A
{
	__asm
	{
		mov		eax, [edi - 20h]
		push	1
		mov		ecx, ebp
		lea		edx, [eax + ebx + 7]
		mov		eax, 004D5550h
		call	eax
		mov		ecx, [edi - 20h]
		push	4
		lea		edx, [ecx + ebx + 8]
		push	00584A63h
		ret
	}
}

//collision manager array extend: 32 -> 128
//LAND
__declspec(naked) void land_1() //004B0192
{
	__asm
	{
		mov		edi, [esi + 9Ch]
		push	004B0198h
		ret
	}
}

__declspec(naked) void land_1_() //004B01B5
{
	__asm
	{
		add		edi, 4
		cmp		edx, 128
		push	004B01BBh
		ret
	}
}

__declspec(naked) void land_2() //004B0A6D
{
	__asm
	{
		mov		eax, [ebx + 9Ch]
		mov		eax, [eax]
		push	004B0A73h
		ret
	}
}

__declspec(naked) void land_3() //004B063C
{
	__asm
	{
		mov		ecx, [edi + 9Ch]
		mov		eax, [ecx + eax * 4]
		push	004B0643h
		ret
	}
}

__declspec(naked) void land_4() //004B0651
{
	__asm
	{
		mov		ecx, [edi + 9Ch]
		mov		ecx, [ecx + eax * 4]
		push	004B0658h
		ret
	}
}

__declspec(naked) void land_5() //004B112F
{
	__asm
	{
		mov		ecx, [edx + 9Ch]
		mov		eax, [ecx + eax * 4]
		push	004B1136h
		ret
	}
}

__declspec(naked) void land_6() //004B114B
{
	__asm
	{
		push	esi
		mov		esi, [eax + 9Ch]
		mov		ecx, [esi + ecx * 4]
		pop		esi
		push	004B1152h
		ret
	}
}

__declspec(naked) void land_7() //004AFD7F
{
	__asm
	{
		push	128*4
		call	ds:[malloc]
		add		esp, 4
		mov		[esi + 9Ch], eax
		mov		edi, eax
		xor		eax, eax
		mov		ecx, 128
		push	004AFD85h
		ret
	}
}

//collision check functions:
__declspec(naked) void land_8() //004B207D
{
	__asm
	{
		mov		eax, [esi + 9Ch]
		lea		eax, [eax + ebx * 4]
		push	004B2084h
		ret
	}
}

__declspec(naked) void land_9() //004B2093
{
	__asm
	{
		mov		ebx, [esi + 9Ch]
		lea		ecx, [ebx + ecx * 4]
		mov		ebx, [ecx]
		push	004B20A1h
		ret
	}
}

__declspec(naked) void land_10() //004B1614
{
	__asm
	{
		push	edi
		mov		edi, [esi + 9Ch]
		lea		edx, [edi + ecx * 4]
		mov		eax, [edx]
		lea		ecx, [edi + ebx * 4]
		pop		edi
		push	004B1629h
		ret
	}
}

__declspec(naked) void land_destructor() //004B0260
{
	__asm
	{
		push    ebx
		push    ebp
		push    esi
		mov     esi, ecx
		xor     ebp, ebp
		mov		eax, [esi + 9Ch]
		push	eax
		call	ds:[free]
		add		esp, 4
		push	004B0267h
		ret
	}
}

//AIR
__declspec(naked) void air_1() //004144E0
{
	__asm
	{
		mov		edi, [esi + 9Ch]
		push	004144E6h
		ret
	}
}

__declspec(naked) void air_1_() //00414503
{
	__asm
	{
		add		edi, 4
		cmp		edx, 128
		push	00414509h
		ret
	}
}

__declspec(naked) void air_2() //00414808
{
	__asm
	{
		mov		edx, [ecx + 9Ch]
		mov		edx, [edx + eax * 4]
		push	0041480Fh
		ret
	}
}

__declspec(naked) void air_3() //00414823
{
	__asm
	{
		mov		edx, [eax + 9Ch]
		mov		edx, [edx + ecx * 4]
		push	0041482Ah
		ret
	}
}

__declspec(naked) void air_4() //00414E1F
{
	__asm
	{
		mov		edx, [edx + 9Ch]
		mov		eax, [edx + eax * 4]
		push	00414E26h
		ret
	}
}

__declspec(naked) void air_5() //00414E3B
{
	__asm
	{
		push	edi
		mov		edi, [eax + 9Ch]
		mov		ecx, [edi + ecx * 4]
		pop		edi
		push	00414E42h
		ret
	}
}

__declspec(naked) void air_6() //004141BF
{
	__asm
	{
		push	128*4
		call	ds : [malloc]
		add		esp, 4
		mov		[esi + 9Ch], eax
		mov		edi, eax
		xor		eax, eax
		mov		ecx, 128
		push	004141C5h
		ret
	}
}

//collision check functions:
__declspec(naked) void air_7() //00415B91
{
	__asm
	{
		mov		eax, [esi + 9Ch]
		lea		eax, [eax + ebx * 4]
		push	00415B98h
		ret
	}
}

__declspec(naked) void air_8() //00415BA7
{
	__asm
	{
		mov		ebx, [esi + 9Ch]
		lea		ecx, [ebx + ecx * 4]
		mov		ebx, [ecx]
		push	00415BB5h
		ret
	}
}

__declspec(naked) void air_9() //004152F4
{
	__asm
	{
		push	edi
		mov		edi, [esi + 9Ch]
		lea		edx, [edi + ecx * 4]
		mov		eax, [edx]
		lea		ecx, [edi + ebx * 4]
		pop		edi
		push	00415309h
		ret
	}
}

__declspec(naked) void air_destructor() //004145B0
{
	__asm
	{
		push    ebx
		push    ebp
		push    esi
		mov     esi, ecx
		xor     ebp, ebp
		mov		eax, [esi + 9Ch]
		push	eax
		call	ds : [free]
		add		esp, 4
		push	004145B7h
		ret
	}
}

__declspec(naked) void waypoint_task() //004C06B0
{
	__asm
	{
		xor		eax, eax
		mov		ch, [esi + 1]
		shr		ecx, 6
		mov		cl, [esi]
		mov		[esp + 14h], eax
		fild	dword ptr [esp + 14h]
		mov		[esp + 14h], ecx
		mov		edx, [edi]
		xor		eax, eax
		fstp	dword ptr [esp]
		fild	dword ptr [esp + 14h]
		mov		ah, [esi + 1]
		shr		eax, 4
		mov     al, [esi - 1]
		and		eax, 3FFh
		push	004C06CCh
		ret
	}
}

__declspec(naked) void waypoint_call() //0044C002
{
	__asm
	{
		mov		al, [edi + 1]
		mov		[esp + 1Eh], al
		push	0044C007h
		ret
	}
}

__declspec(naked) void waypoint_set() //0044E3AD
{
	__asm
	{
		mov		ecx, [esp + 20h]
		mov		edx, [esp + 24h]
		lea		ebx, [esi + 2]
		mov		[esi + 3], cl
		mov		[esi + 4], dl
		shr		ecx, 8
		shr		edx, 6
		and		edx, 0Ch
		or		cl, dl
		shl		ecx, 4
		or		[esi + 1], cl
		push	0044E3BEh
		ret
	}
}

__declspec(naked) void waypoint_fix() //0044BDD0
{
	__asm
	{
		mov		al, [edi + 1]
		and		eax, 0Fh
		lea		esi, [edi + 2]
		push	0044BDD6h
		ret
	}
}

void setMapSizeHooks()
{
	//setHook((void*)0x004B1639, &collision_test_1);
	//setHook((void*)0x004B20B5, &collision_test_2);

	setHook((void*)0x0052A123, &onEditorMapSize);

	setInt(0x0052EC29, (int)map_sizes_jmp);
	setByte(0x0052EC23, sizeof(map_sizes_jmp) / sizeof(map_sizes_jmp[0]) - 1);

	mapptr_2 = (int*)malloc(sizeof(int*) * (MAP_MAX+1));
	mapptr = mapptr_2 + 1;
	mapptr_3 = mapptr + 1;

	map_open_paths = (map_open_paths_*)malloc(sizeof(map_open_paths_)*MAP_MAX*MAP_MAX);
	map_cost = (int*)malloc(sizeof(int)*MAP_MAX*MAP_MAX);

	int map_offset = (int)map_open_paths - 0x006AEBB0;
	int map_cost_offset = (int)map_cost - 0x006AEBB0;

	//

	setHook((void*)0x004BD63E, &map_init_hook);
	setHook((void*)0x004BDA3B, &facet_clear_hook);

	//

	setInt(0x004BDD45, map_offset + o_n);
	setInt(0x004BDD4C, map_offset + cn_n);

	setInt(0x004BDD5B, map_offset + o_n);
	setInt(0x004BDD66, map_offset + o_x);
	setInt(0x004BDD6D, map_offset + o_x);
	setInt(0x004BDD74, map_offset + o_n);
	setInt(0x004BDD7B, map_offset + o_n);

	setInt(0x004BE5F6, map_offset + o_x);
	setInt(0x004BE5FD, map_offset + o_x);
	setInt(0x004BE604, map_offset + o_n);
	setInt(0x004BE60D, map_offset + o_n);

	setInt(0x004BE627, map_offset + o_n);

	setInt(0x004BE7B3, map_offset + o_x);
	setInt(0x004BE7BA, map_offset + o_x);
	setInt(0x004BE7C1, map_offset + o_n);
	setInt(0x004BE7CA, map_offset + o_n);

	setInt(0x004BE7E4, map_offset + o_n);

	setInt(0x004BDEBB, map_cost_offset);
	setInt(0x004BDF8F, map_cost_offset);
	setInt(0x004BE558, map_cost_offset);
	setInt(0x004BE56B, map_cost_offset);
	setInt(0x004BE6D8, map_cost_offset);

	//edit all references to array:
	setInt(0x41C30E, (int)mapptr);
	setInt(0x41E3DA, (int)mapptr);
	setInt(0x420EB0, (int)mapptr);
	setInt(0x444E45, (int)mapptr);
	setInt(0x444E9D, (int)mapptr);
	setInt(0x444ED5, (int)mapptr);
	setInt(0x444F25, (int)mapptr);
	setInt(0x4464F7, (int)mapptr);
	setInt(0x446518, (int)mapptr);
	setInt(0x456566, (int)mapptr);

	setInt(0x456CBD, (int)mapptr);
	setInt(0x46FCC4, (int)mapptr);
	setInt(0x46FF43, (int)mapptr);
	setInt(0x4701A7, (int)mapptr);
	setInt(0x47026F, (int)mapptr);
	setInt(0x4702CE, (int)mapptr);
	setInt(0x47046B, (int)mapptr);
	setInt(0x55079D, (int)mapptr);
	setInt(0x5508C8, (int)mapptr);
	setInt(0x55094A, (int)mapptr);

	setInt(0x553396, (int)mapptr);
	setInt(0x553602, (int)mapptr);
	setInt(0x5537F2, (int)mapptr);
	setInt(0x555DCF, (int)mapptr);
	setInt(0x556468, (int)mapptr);
	setInt(0x5F9825, (int)mapptr);
	setInt(0x5F9DFA, (int)mapptr);
	setInt(0x60DE9F, (int)mapptr);
	setInt(0x60FD1B, (int)mapptr);
	setInt(0x60FD70, (int)mapptr);

	setInt(0x60FD9C, (int)mapptr);
	setInt(0x610AC4, (int)mapptr);
	setInt(0x614072, (int)mapptr);
	setInt(0x61543A, (int)mapptr);
	setInt(0x6154BE, (int)mapptr);
	setInt(0x6154FB, (int)mapptr);
	setInt(0x61557A, (int)mapptr);
	setInt(0x615714, (int)mapptr);
	setInt(0x61595F, (int)mapptr);
	setInt(0x615AD1, (int)mapptr);

	setInt(0x615C69, (int)mapptr);
	setInt(0x615DF2, (int)mapptr);
	setInt(0x615F2B, (int)mapptr);
	setInt(0x615F5A, (int)mapptr);
	setInt(0x61601B, (int)mapptr);

	//

	setInt(0x419FA8, (int)mapptr_2);
	setInt(0x41A027, (int)mapptr_2);
	setInt(0x443B7A, (int)mapptr_2);
	setInt(0x443E62, (int)mapptr_2);
	setInt(0x443EC5, (int)mapptr_2);
	setInt(0x444D81, (int)mapptr_2);
	setInt(0x444F7A, (int)mapptr_2);
	setInt(0x4464F0, (int)mapptr_2);
	setInt(0x446559, (int)mapptr_2);
	setInt(0x60FD06, (int)mapptr_2);

	setInt(0x60FDB6, (int)mapptr_2);
	setInt(0x60FDEF, (int)mapptr_2);
	setInt(0x60FE1F, (int)mapptr_2);
	setInt(0x614647, (int)mapptr_2);
	setInt(0x61470E, (int)mapptr_2);

	//

	setInt(0x60FD2B, (int)mapptr_3);
	setInt(0x60FDD2, (int)mapptr_3);
	setInt(0x60FE03, (int)mapptr_3);
	setInt(0x60FE37, (int)mapptr_3);

	//

	setInt(0x58355C, MAP_MAX);
	setInt(0x583561, MAP_MAX);

	setInt(0x581923, MAP_MAX);
	setInt(0x581928, MAP_MAX);

	//

	setHook((void*)0x004BDCDE, &p1);
	setHook((void*)0x004BDD97, &p2);
	setHook((void*)0x004BE58C, &p3);
	setHook((void*)0x004BE5C6, &p4);
	setHook((void*)0x004BE631, &p5);
	setHook((void*)0x004BE73F, &p6);
	setHook((void*)0x004BE787, &p7);
	setHook((void*)0x004BE7EF, &p8);
	setHook((void*)0x004BDAEF, &p9);
	setHook((void*)0x004BDB9E, &p10);
	setHook((void*)0x004BDCB6, &p11);
	setHook((void*)0x004BDAC8, &p12);
	setHook((void*)0x004BE48D, &p13);
	setHook((void*)0x004BDFB0, &p14);
	setHook((void*)0x004BE103, &p15);
	setHook((void*)0x004BE543, &p16);
	setHook((void*)0x004BE64F, &p17);

	setHook((void*)0x004BE9A5, &p1_);
	setHook((void*)0x004BEB56, &p2_);
	setHook((void*)0x004BEB6C, &p3_);
	setHook((void*)0x004BEB76, &p4_);
	setHook((void*)0x004BEB7E, &p5_);
	setHook((void*)0x004BEB94, &p6_);
	setHook((void*)0x004BEB9E, &p7_);

	fix_sub_4bd6a0();
	fix_sub_4be980();

	setHook((void*)0x004BDDBA, &facet1);
	setHook((void*)0x004BDF60, &facet2);
	setHook((void*)0x004BEBC2, &facet3);

	//fix path finding on the right side: 8 to 10 bit shift
	setByte(0x004B0A41, 10);
	setHook((void*)0x004B0987, &onPathFind_shift);

	//100h consts
	setInt(0x00615F4C, MAP_MAX);
	setInt(0x00615F53, MAP_MAX);

	//setInt(0x00609999, MAP_MAX);	//check!
	//setInt(0x00609A46, MAP_MAX);	//
	//setInt(0x0060A10C, MAP_MAX);

	//RM
	//setInt(0x005CB15B, MAP_MAX);

	//FFFCFF00h constant -> FFFCFC00h
	setInt(0x004B19E6, 0xFFFCFC00);
	setInt(0x004B1ACB, 0xFFFCFC00);
	setByte(0x004B19EC, 10);
	setInt(0x004B1ACB, 0xFFFCFC00);
	setByte(0x004B1AD1, 10);
	setInt(0x004B1BA9, 0xFFFCFC00);
	setByte(0x004B1BAF, 10);
	setInt(0x004B1CB2, 0xFFFCFC00);
	setByte(0x004B1CC3, 10);

	setByte(0x004A9163, 10);
	setByte(0x004A9B4F, 10);

	//movsx -> movzx
	setByte(0x0060BE42, 0xB7);
	setByte(0x0060DE66, 0xB7);
	setByte(0x00611290, 0xB7);
	setByte(0x00491D34, 0xB7);
	setByte(0x0060D09F, 0xB7);
	setByte(0x0042B584, 0xB7);

	/*setByte(0x0060BE57, 0xB7);
	setByte(0x0060DE6B, 0xB7);
	setByte(0x006112AD, 0xB7);
	setByte(0x00491D52, 0xB7);
	setByte(0x0060D0B5, 0xB7);
	setByte(0x0042B588, 0xB7);*/

	//search for resources
	setHook((void*)0x006150CB, findres_onX);
	setHook((void*)0x006150E0, findres_onY);
	setHook((void*)0x00614FD4, findres_onPlayerUpdate);

	setHook((void*)0x005873A4, ai_findres);
	setHook((void*)0x00591D51, ai_findres_under_attack);
	setHook((void*)0x00590B86, ai_findres_init_1);
	setHook((void*)0x00590BA3, ai_findres_init_2);
	setHook((void*)0x00590A43, ai_findres_onCopy);

	setHook((void*)0x0058670C, ai_findres_save);
	setHook((void*)0x00584A5A, ai_findres_load);

	//search for targets
	setHook((void*)0x00614AC1, &visible_unit_managerOnX);
	setHook((void*)0x00614AEC, &visible_unit_managerOnY);
	setHook((void*)0x00614B43, &visible_unit_managerOnPlayer);
	setHook((void*)0x00614966, &visible_unit_managerUpdate);

	//visible unit ptr
	setHook((void*)0x0041BCC6, &visible_unit_ptr_fix_1);

	//save load fix
	setInt(0x00583BF2, MAP_MAX);
	setInt(0x00583BE7, MAP_MAX);

	//standard game list
	setHook((void*)0x00520043, maplist_make);
	setHook((void*)0x005ECC77, maplist_interceptSel);
	setHook((void*)0x00521B57, maplist_getSel);

	//collision manager (land)
	setHook((void*)0x004B0192, land_1);
	setHook((void*)0x004B01B5, land_1_);
	setHook((void*)0x004B0A6D, land_2);
	setHook((void*)0x004B063C, land_3);
	setHook((void*)0x004B0651, land_4);
	setHook((void*)0x004B112F, land_5);
	setHook((void*)0x004B114B, land_6);
	setHook((void*)0x004AFD7F, land_7);
	setHook((void*)0x004B207D, land_8);
	setHook((void*)0x004B2093, land_9);
	setHook((void*)0x004B1614, land_10);
	setHook((void*)0x004B0260, land_destructor);

	//collision manager (air)
	setHook((void*)0x004144E0, air_1);
	setHook((void*)0x00414503, air_1_);
	setHook((void*)0x00414808, air_2);
	setHook((void*)0x00414823, air_3);
	setHook((void*)0x00414E1F, air_4);
	setHook((void*)0x00414E3B, air_5);
	setHook((void*)0x004141BF, air_6);
	setHook((void*)0x00415B91, air_7);
	setHook((void*)0x00415BA7, air_8);
	setHook((void*)0x004152F4, air_9);
	setHook((void*)0x004145B0, air_destructor);

	//waypoints
	setHook((void*)0x004C06B0, waypoint_task);
	setHook((void*)0x0044C002, waypoint_call);
	setHook((void*)0x0044E3AD, waypoint_set);
	setHook((void*)0x0044BDD0, waypoint_fix);
}

void setMapSizeHooks_legacy()
{
	setHook((void*)0x0052A123, &onEditorMapSize_legacy);

	setHook((void*)0x0058670C, ai_findres_save);
	setHook((void*)0x00584A5A, ai_findres_load);

	//save load fix
	setInt(0x00583BF2, MAP_MAX);
	setInt(0x00583BE7, MAP_MAX);
}
