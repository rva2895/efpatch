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
		push    2978h	//giant
		call    esi
		mov     ecx, [edi]
		push	0
		push	2979h	//huge
		call	esi
		mov     ecx, [edi]
		push	0
		push	297Ah	//massive
		call	esi
		mov     ecx, [edi]
		push	0
		push	297Bh	//titanic
		call	esi
		pop		esi
		push	0052A131h
		ret
	}
}

__declspec(naked) void jmp_280()
{
	__asm
	{
		mov		esi, 280
		push	0052EC76h
		ret
	}
}

__declspec(naked) void jmp_336()
{
	__asm
	{
		mov		esi, 336
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
	(int)(&jmp_280),
	(int)(&jmp_336)
};

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

void setMapSizeHooks()
{
	setHook((void*)0x0052A123, &onEditorMapSize);

	setInt(0x0052EC29, (int)map_sizes_jmp);
	setByte(0x0052EC23, 8);

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
}

void setMapSizeHooks_legacy()
{
	setHook((void*)0x0052A123, &onEditorMapSize_legacy);
}
