#include "stdafx.h"

#include "terrain.h"

#ifndef _CHEATDLL_CC
#define TERRAIN_COUNT 252
#else
#ifdef _CC_COMPATIBLE
#define TERRAIN_COUNT 53
#else
#define TERRAIN_COUNT 54
#endif
#endif

BYTE* terrain_array = 0;
/*BYTE terrain_array[] =
{
	4, 1, 2, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1,	//0
	1, 4, 4, 4, 4, 4, 1, 1, 4, 4, 4, 4, 4, 4, 4, 4,	//0x10
	4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,	//0x20
	4, 4, 4, 1, 4,									//CC terrains end here
	4, 4,											//terrains 53, 54
	//border terrains
	4, 1, 4, 4, 2, 4, 4, 4, 4,	//0x30
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,	//0x40
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,	//0x50
	4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0,	//0x60
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//0x70
	0, 4, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//0x80
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//0x90
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//0xA0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//0xB0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//0xC0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//0xD0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//0xE0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//0xF0
};*/

BYTE indirect_table_water[] =	//starts from 1 (TERR-WATER1)
{
	/*   1 */ 0, 1, 1, 0,
	/*     */ 1, 1, 1, 1,
	/*     */ 1, 1, 1, 1,
	/*     */ 1, 1, 1, 1,
	/*     */ 1, 1, 1, 1,
	/*     */ 1, 0, 0,				//WATER2 and WATER3
	/*     */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/*     */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/*     */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/*     */ 1, 1, 1, 1, 1, 0,		//MARSH
	/*  60 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/*  70 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/*  80 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/*  90 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* 100 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* 110 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* 120 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* 130 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
	/* 140 */ 0, 0, 0,                //TROPICAL SHALLOWS
	/* 143 */ 1, 1, 1, 1, 1, 1, 1,
	/* 150 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* 160 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* 170 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* 180 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* 190 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* 200 */ 1, 1, 0, 0, 1, 1, 1, 1, 1, 1,
	/* 210 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* 220 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* 230 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* 240 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* 250 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

#define TERR_MAX_CONST 240 //before TROPICALSHALLOWS, used for indirect table water

//BYTE indirect_table_water_2[TERR_MAX_CONST+1];

/*void terrain_transition_change(int state)
{
	//writeByte(0x005CBE20, state ? 0xFF : 2);
	indirect_table_water[0] = state;	//WATER1
	indirect_table_water[3] = state;	//WATERSWAMP
	indirect_table_water[21] = state;	//WATER2
	indirect_table_water[22] = state;	//WATER3
	indirect_table_water[58] = state;	//MARSH
	for (int i = 0; i < sizeof(indirect_table_water); i++)
		switch (i)
		{
		case 0:
		case 3:
		case 21:
		case 22:
		case 58:
			break;
		default:
			indirect_table_water[i] = !state;
		}
}
*/

__declspec(naked) void ice_terrain_fix() //005CBCCE
{
	__asm
	{
		cmp     ebx, 35
		jz      _ice1
		cmp     ebx, 37
		jz      _ice1
		cmp     ebx, 157
		jb      _not_ice1
		cmp     ebx, 160
		ja      _not_ice1
_ice1:
		mov     ebx, 005CBCDCh
		jmp     ebx
_not_ice1:
		mov     ebx, 005CBE1Eh
		jmp     ebx
	}
}

__declspec(naked) void snow_terrain_fix_1() //005CBE92
{
	__asm
	{
		cmp     dl, 20h
		jb      _not_snow1
		cmp     dl, 22h
		jbe     _snow1
		cmp     dl, 36
		jz      _snow1
		cmp     dl, 47
		jz      _snow1
		cmp     dl, 55
		jz      _snow1
		cmp     dl, 63
		jz      _snow1
		cmp     dl, 134
		jz      _snow1
		cmp     dl, 135
		jz      _snow1
		cmp     dl, 195
		jz      _snow1
_not_snow1:
		push    005CBEA1h
		ret
_snow1:
		push    005CBEBEh
		ret
	}
}

__declspec(naked) void snow_terrain_fix_2() //005CBEAF
{
	__asm
	{
		cmp     dl, 20h
		jb      _not_snow2
		cmp     dl, 22h
		jbe     _snow2
		cmp     dl, 36
		jz      _snow2
		cmp     dl, 47
		jz      _snow2
		cmp     dl, 55
		jz      _snow2
		cmp     dl, 63
		jz      _snow2
		cmp     dl, 134
		jz      _snow2
		cmp     dl, 135
		jz      _snow2
		cmp     dl, 195
		jz      _snow2
_not_snow2:
		push    005CBEC3h
		ret
_snow2:
		push    005CBEBEh
		ret
	}
}

__declspec(naked) void snow_terrain_fix_3() //005CBF42
{
	__asm
	{
		cmp     bl, 20h
		jb      _not_snow3
		cmp     bl, 22h
		jbe     _snow3
		cmp     bl, 36
		jz      _snow3
		cmp     bl, 47
		jz      _snow3
		cmp     bl, 55
		jz      _snow3
		cmp     bl, 63
		jz      _snow3
		cmp     bl, 134
		jz      _snow3
		cmp     bl, 135
		jz      _snow3
		cmp     bl, 195
		jz      _snow3
_not_snow3:
		push    005CBF51h
		ret
_snow3:
		push    005CBF89h
		ret
	}
}

__declspec(naked) void snow_terrain_fix_4() //005CBF5D
{
	__asm
	{
		cmp     dl, 20h
		jb      _not_snow4
		cmp     dl, 22h
		jbe     _snow4
		cmp     dl, 36
		jz      _snow4
		cmp     dl, 47
		jz      _snow4
		cmp     dl, 55
		jz      _snow4
		cmp     dl, 63
		jz      _snow4
		cmp     dl, 134
		jz      _snow4
		cmp     dl, 135
		jz      _snow4
		cmp     dl, 195
		jz      _snow4
_not_snow4:
		push    005CBF6Ch
		ret
_snow4:
		push    005CBF89h
		ret
	}
}

__declspec(naked) void snow_terrain_fix_5() //005CBF7A
{
	__asm
	{
		cmp     dl, 20h
		jb      _not_snow5
		cmp     dl, 22h
		jbe     _snow5
		cmp     dl, 36
		jz      _snow5
		cmp     dl, 47
		jz      _snow5
		cmp     dl, 55
		jz      _snow5
		cmp     dl, 63
		jz      _snow5
		cmp     dl, 134
		jz      _snow5
		cmp     dl, 135
		jz      _snow5
		cmp     dl, 195
		jz      _snow5
_not_snow5:
		push    005CBF8Eh
		ret
_snow5:
		push    005CBF89h
		ret
	}
}

__declspec(naked) void snow_terrain_fix_6() //005CC011
{
	__asm
	{
		cmp     bl, 20h
		jb      _not_snow6
		cmp     bl, 22h
		jbe     _snow6
		cmp     bl, 36
		jz      _snow6
		cmp     bl, 47
		jz      _snow6
		cmp     bl, 55
		jz      _snow6
		cmp     bl, 63
		jz      _snow6
		cmp     bl, 134
		jz      _snow6
		cmp     bl, 135
		jz      _snow6
		cmp     bl, 195
		jz      _snow6
_not_snow6:
		push    005CC020h
		ret
_snow6:
		push    005CC055h
		ret
	}
}

__declspec(naked) void snow_terrain_fix_7() //005CC02C
{
	__asm
	{
		cmp     dl, 20h
		jb      _not_snow7
		cmp     dl, 22h
		jbe     _snow7
		cmp     dl, 36
		jz      _snow7
		cmp     dl, 47
		jz      _snow7
		cmp     dl, 55
		jz      _snow7
		cmp     dl, 63
		jz      _snow7
		cmp     dl, 134
		jz      _snow7
		cmp     dl, 135
		jz      _snow7
		cmp     dl, 195
		jz      _snow7
_not_snow7:
		push    005CC03Bh
		ret
_snow7:
		push    005CC055h
		ret
	}
}

__declspec(naked) void snow_terrain_fix_8() //005CC049
{
	__asm
	{
		cmp     al, 20h
		jb      _not_snow8
		cmp     al, 22h
		jbe     _snow8
		cmp     al, 36
		jz      _snow8
		cmp     al, 47
		jz      _snow8
		cmp     al, 55
		jz      _snow8
		cmp     al, 63
		jz      _snow8
		cmp     al, 134
		jz      _snow8
		cmp     al, 135
		jz      _snow8
		cmp     al, 195
		jz      _snow8
_not_snow8:
		push    005CC05Ah
		ret
_snow8:
		push    005CC055h
		ret
	}
}

__declspec(naked) void snow_terrain_fix_9() //005CC095
{
	__asm
	{
		cmp     dl, 20h
		jb      _not_snow9
		cmp     dl, 22h
		jbe     _snow9
		cmp     dl, 36
		jz      _snow9
		cmp     dl, 47
		jz      _snow9
		cmp     dl, 55
		jz      _snow9
		cmp     dl, 63
		jz      _snow9
		cmp     dl, 134
		jz      _snow9
		cmp     dl, 135
		jz      _snow9
		cmp     dl, 195
		jz      _snow9
_not_snow9:
		push    005CC1F6h
		ret
_snow9:
		push    005CC0ACh
		ret
	}
}

//new

__declspec(naked) void snow_terrain_fix_10() //004E4519
{
	__asm
	{
		cmp     bl, 20h
		jb      _not_snow10
		cmp     bl, 22h
		jbe     _snow10
		cmp     bl, 36
		jz      _snow10
		cmp     bl, 47
		jz      _snow10
		cmp     bl, 55
		jz      _snow10
		cmp     bl, 63
		jz      _snow10
		cmp     bl, 134
		jz      _snow10
		cmp     bl, 135
		jz      _snow10
		cmp     bl, 195
		jz      _snow10
_not_snow10:
		push    004E4684h
		ret
_snow10:
		push    004E4530h
		ret
	}
}

__declspec(naked) void snow_terrain_fix_11() //004E6209
{
	__asm
	{
		cmp     bl, 20h
		jb      _not_snow11
		cmp     bl, 22h
		jbe     _snow11
		cmp     bl, 36
		jz      _snow11
		cmp     bl, 47
		jz      _snow11
		cmp     bl, 55
		jz      _snow11
		cmp     bl, 63
		jz      _snow11
		cmp     bl, 134
		jz      _snow11
		cmp     bl, 135
		jz      _snow11
		cmp     bl, 195
		jz      _snow11
_not_snow11:
		push    004E6374h
		ret
_snow11:
		push    004E6220h
		ret
	}
}

__declspec(naked) void snow_terrain_fix_12() //00554995
{
	__asm
	{
		cmp     cl, 20h
		jb      _not_snow12
		cmp     cl, 22h
		jbe     _snow12
		cmp     cl, 36
		jz      _snow12
		cmp     cl, 47
		jz      _snow12
		cmp     cl, 55
		jz      _snow12
		cmp     cl, 63
		jz      _snow12
		cmp     cl, 134
		jz      _snow12
		cmp     cl, 135
		jz      _snow12
		cmp     cl, 195
		jz      _snow12
_not_snow12:
		push    005549CCh
		ret
_snow12:
		push    005549AEh
		ret
	}
}

void(__thiscall* rmsDefine) (void* _this, char *str, int flag, int value, char, char, char, char) =
	(void(__thiscall*) (void*, char*, int, int, char, char, char, char))0x004E1C20;

void __stdcall mapsize_rms_define(int size, void* _this)
{
	switch (size)
	{
	case 320:
		rmsDefine(_this, "MAPSIZE_320", 1, 0, 0, 0, 0, 0);
		break;
	case 400:
		rmsDefine(_this, "MAPSIZE_400", 1, 0, 0, 0, 0, 0);
		break;
	case 480:
		rmsDefine(_this, "MAPSIZE_480", 1, 0, 0, 0, 0, 0);
		break;
	case 560:
		rmsDefine(_this, "MAPSIZE_560", 1, 0, 0, 0, 0, 0);
		break;
	case 640:
		rmsDefine(_this, "MAPSIZE_640", 1, 0, 0, 0, 0, 0);
		break;
	default:
		break;
	}
}

__declspec(naked) void onMapSizeRMSDefine() //004E7542
{
	__asm
	{
		push    eax
		mov     ecx, esi
		mov     eax, 004E1C20h
		call    eax

		push    esi
		mov     eax, [esi + 14h]
		push    eax
		call    mapsize_rms_define
		push    004E754Ah
		ret
	}
}

__declspec(naked) void terrain_asm_1() //004B0CC3
{
	__asm
	{
		add     edx, 4
		cmp     ecx, TERRAIN_COUNT
		mov     eax, 004B0CC9h
		jmp     eax
	}
}

__declspec(naked) void terrain_asm_2() //0049479F
{
	__asm
	{
		cmp     ebp, 129  //forbidden ID+1
		jg      short not_forbidden
		cmp     ebp, 105
		jge     short loc_494805
not_forbidden:
		cmp     [esi - 24h], bl
		jz      short loc_494805
		mov     edx, 004947A4h
		jmp     edx
loc_494805:
		mov     edx, 00494805h
		jmp     edx
	}
}

__declspec(naked) void terrain_forbidden_ids_2() //0060DC77
{
	__asm
	{
		cmp     ecx, 128
		jg      short not_forbidden_2
		cmp     ecx, 104
		jge     short forbidden_2
not_forbidden_2:
		mov     byte ptr[edx + eax + 0D9h], 0
forbidden_2:
		mov     eax, 0060DC7Fh
		jmp     eax
	}
}

int memory_temp = 0x40000000;

__declspec(naked) void terrain_memory_fix() //00497141
{
	__asm
	{
		//cmp     edx, 40000000h
		//jb      no_memory_fix
		//mov     edx, 40000000h
//no_memory_fix:
		mov     edx, memory_temp
		mov     ecx, 190h
		mov     eax, 00497146h
		jmp     eax
	}
}

__declspec(naked) void terrain_sound_fix() //005006F4
{
	__asm
	{
		cmp     eax, 128
		jg      not_forbidden_3
		cmp     eax, 104
		jge     forbidden_3
not_forbidden_3:
		movzx   cx, byte ptr [edx]
		cmp     cx, di
		push    005006FBh
		ret
forbidden_3:
		push    00500707h
		ret
	}
}

__declspec(naked) void terrain_asm_3() //004DE9DF
{
	__asm
	{
		push    TERRAIN_COUNT
		push    edx
		mov     ecx, [eax + 0BF7Ch]
		mov     eax, 004DE9E8h
		jmp     eax
	}
}

__declspec(naked) void terrain_asm_4() //004DE2E3
{
	__asm
	{
		push    TERRAIN_COUNT
		mov     ebp, [edx + ecx * 4]
		mov     edx, [esi + 38h]
		mov     eax, 004DE2EBh
		jmp     eax
	}
}

__declspec(naked) void terrain_asm_5() //004E5F90
{
	__asm
	{
		add     ecx, 4
		cmp     eax, TERRAIN_COUNT
		mov     edx, 004E5F96h
		jmp     edx
	}
}

__declspec(naked) void terrain_asm_6() //004E5FA6
{
	__asm
	{
		push    TERRAIN_COUNT
		push    eax
		mov     ecx, [ecx + 0BF7Ch]
		mov     edx, 004E5FAFh
		jmp     edx
	}
}

__declspec(naked) void terrain_asm_7() //004DE61B
{
	__asm
	{
		push    TERRAIN_COUNT
		mov     ecx, [eax + ecx * 4]
		push    ecx
		mov     eax, 004DE621h
		jmp     eax
	}
}

void __cdecl memcpy_new(void* dst, void* src, size_t count)
{
	memcpy(dst, src, count);
}

__declspec(naked) void terrain_shift_rge_data() //0048FB16
{
	__asm
	{
		push    0x2CC                                   //count
		mov     eax, esi
		add     eax, 0xC1E8-0x2CC-0x6C+136*0x1D4-0x2CC  //src
		push    eax
		mov     eax, esi
		add     eax, 0xC1E8-0x2CC                       //dest
		push    eax
		call    memcpy_new
		add     esp, 0Ch
		mov     eax, 006A3684h
		mov     eax, [eax]
		mov     ecx, 0048FB1Bh
		jmp     ecx
	}
}
#define RGE_BORDER_PADDING 0x6C
#define RGE_POST_BORDER_SIZE 0x2D0
#define TERRAIN_SIZE 0x1D4
#define OLD_RGE_SIZE 0xC1E8
#define NEW_RGE_SIZE (OLD_RGE_SIZE - RGE_BORDER_PADDING + 148 * TERRAIN_SIZE)
#define NEW_RGE_SIZE_2 (NEW_RGE_SIZE+0xEA10-0xC1E8)
#define NEW_RGE_STEP 0xE800

__declspec(naked) void terrain_read_dat_split() //0048FAFC
{
	__asm
	{
		push    NEW_RGE_STEP
		mov     edx, esi  //buffer
		mov     ecx, ebp
		mov     dword ptr [esi], 0065B5C0h
		mov     [esi + 0BF7Ch], ebx
		mov     eax, 004D5550h
		call    eax
		push    NEW_RGE_SIZE-NEW_RGE_STEP
		lea     edx, [esi+NEW_RGE_STEP]
		mov     ecx, ebp
		mov     eax, 004D5550h
		call    eax
		//shift RGE
		push    RGE_POST_BORDER_SIZE       //count
		mov     eax, esi
		add     eax, NEW_RGE_SIZE - RGE_POST_BORDER_SIZE  //src
		push    eax
		mov     eax, esi
		add     eax, OLD_RGE_SIZE - RGE_POST_BORDER_SIZE  //dest
		push    eax
		call    memcpy_new
		add     esp, 0Ch
		mov     eax, 006A3684h
		mov     eax, [eax]
		mov     ecx, 0048FB1Bh
		jmp     ecx
	}
}

int __fastcall isCarbon(char terrain)
{
	switch (terrain)
	{
	case 10:
	case 13:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 49:
	case 50:
	case 73:
	case 74:
	case 75:
	case 76:
	case 77:
	case 78:
	case 79:
	case 80:
	case 81:
	case 82:
	case 83:
	case 84:
	case 85:
	case 86:
	case 87:
	case 88:
	case 89:
	case 90:
	case 92:
	case 176:
	case 177:
	case 178:
	case 179:
		return true;
		break;
	default:
		return false;
		break;
	}
}

__declspec(naked) void onResourceFound_carbon() //005856DE
{
	__asm
	{
		mov     ecx, eax
		call    isCarbon
		test    eax, eax
		jnz     found_carbon
		mov     eax, 00585726h
		jmp     eax
found_carbon:
		mov     eax, 0058570Bh
		jmp     eax
	}
}

#pragma optimize( "s", on )
void setExtraTerrainHooks()
{
	//new terrain implementatio:
	setHook((void*)0x004B0CC3, terrain_asm_1);
	setHook((void*)0x0049479F, terrain_asm_2);
	setHook((void*)0x004DE9DF, terrain_asm_3);
	setHook((void*)0x004DE2E3, terrain_asm_4);
	setHook((void*)0x004E5F90, terrain_asm_5);
	setHook((void*)0x004E5FA6, terrain_asm_6);
	setHook((void*)0x004DE61B, terrain_asm_7);
	//writeDword(0x0048FAFD, NEW_RGE_SIZE); //replaced by step above
	writeDword(0x004946B5, NEW_RGE_SIZE);
	writeDword(0x0049473A, NEW_RGE_SIZE);
	writeDword(0x0061BFDA, NEW_RGE_SIZE);
	writeDword(0x0061CA3A, NEW_RGE_SIZE);
	//setHook((void*)0x0048FB16, terrain_shift_rge_data);
	setHook((void*)0x0048FAFC, terrain_read_dat_split);
	writeDword(0x005FE15A, NEW_RGE_SIZE_2);
	writeDword(0x005FE48A, NEW_RGE_SIZE_2);

	writeByte(0x0048FB72, 0xEB);	//skip border loading
	writeByte(0x0048FB73, 0x06);

	writeDword(0x004B0435, (DWORD)terrain_array);
	writeDword(0x004B0CBC, (DWORD)terrain_array);
	//writeByte(0x004B0CC8, TERRAIN_COUNT); //replace with asm.

	writeDword(0x0049479B, TERRAIN_COUNT); //asm, skip forbidden IDs.
	writeDword(0x00494796, 0x6368 + TERRAIN_SIZE * (TERRAIN_COUNT - 0x37));

	writeByte(0x00495063, TERRAIN_COUNT);
	writeByte(0x00495530, TERRAIN_COUNT);
	writeByte(0x00495B6C, TERRAIN_COUNT);

	writeByte(0x004901DE, 0xEB); //skip border unloading

	//memcpy(indirect_table_water_2, indirect_table_water, TERR_MAX_CONST + 1);

	//1
	writeDword(0x005CCA67, (DWORD)indirect_table_water);
	writeDword(0x005CCAB6, (DWORD)indirect_table_water);
	writeDword(0x005CCAED, (DWORD)indirect_table_water);
	writeDword(0x005CCB13, (DWORD)indirect_table_water);
	writeDword(0x005CCB3F, (DWORD)indirect_table_water);
	writeDword(0x005CCB78, (DWORD)indirect_table_water);
	writeDword(0x005CCBAD, (DWORD)indirect_table_water);
	writeDword(0x005CCBE7, (DWORD)indirect_table_water);
	writeDword(0x005CCC1D, (DWORD)indirect_table_water);
	writeDword(0x005CCC80, (DWORD)indirect_table_water);

	//2
	writeDword(0x005CBCF8, (DWORD)indirect_table_water);
	writeDword(0x005CBD1F, (DWORD)indirect_table_water);
	writeDword(0x005CBD45, (DWORD)indirect_table_water);
	writeDword(0x005CBD6A, (DWORD)indirect_table_water);
	writeDword(0x005CBD91, (DWORD)indirect_table_water);
	writeDword(0x005CBDBF, (DWORD)indirect_table_water);
	writeDword(0x005CBDE4, (DWORD)indirect_table_water);
	writeDword(0x005CBE13, (DWORD)indirect_table_water);
	writeDword(0x005CBE4D, (DWORD)indirect_table_water);
	writeDword(0x005CBE76, (DWORD)indirect_table_water);

	writeDword(0x005CBEE6, (DWORD)indirect_table_water);
	writeDword(0x005CBF0B, (DWORD)indirect_table_water);
	writeDword(0x005CBF32, (DWORD)indirect_table_water);

	writeDword(0x005CBFB5, (DWORD)indirect_table_water);
	writeDword(0x005CBFDA, (DWORD)indirect_table_water);
	writeDword(0x005CC001, (DWORD)indirect_table_water);

	//3
	writeDword(0x005CC0C8, (DWORD)indirect_table_water);
	writeDword(0x005CC0EF, (DWORD)indirect_table_water);
	writeDword(0x005CC115, (DWORD)indirect_table_water);
	writeDword(0x005CC13A, (DWORD)indirect_table_water);
	writeDword(0x005CC161, (DWORD)indirect_table_water);
	writeDword(0x005CC18B, (DWORD)indirect_table_water);
	writeDword(0x005CC1B0, (DWORD)indirect_table_water);
	writeDword(0x005CC1D7, (DWORD)indirect_table_water);

	//4
	writeDword(0x005CC200, (DWORD)indirect_table_water);
	writeDword(0x005CC227, (DWORD)indirect_table_water);
	writeDword(0x005CC24E, (DWORD)indirect_table_water);
	writeDword(0x005CC274, (DWORD)indirect_table_water);
	writeDword(0x005CC299, (DWORD)indirect_table_water);
	writeDword(0x005CC2C0, (DWORD)indirect_table_water);
	writeDword(0x005CC2EA, (DWORD)indirect_table_water);
	writeDword(0x005CC30F, (DWORD)indirect_table_water);
	writeDword(0x005CC336, (DWORD)indirect_table_water);

	//new
	//5
	writeDword(0x004E43DA, (DWORD)indirect_table_water);
	writeDword(0x004E4403, (DWORD)indirect_table_water);
	writeDword(0x004E4430, (DWORD)indirect_table_water);
	writeDword(0x004E4455, (DWORD)indirect_table_water);
	writeDword(0x004E447A, (DWORD)indirect_table_water);
	writeDword(0x004E44A7, (DWORD)indirect_table_water);
	writeDword(0x004E44CC, (DWORD)indirect_table_water);
	writeDword(0x004E44F1, (DWORD)indirect_table_water);
	writeDword(0x004E454C, (DWORD)indirect_table_water);
	writeDword(0x004E4573, (DWORD)indirect_table_water);
	writeDword(0x004E459D, (DWORD)indirect_table_water);
	writeDword(0x004E45C2, (DWORD)indirect_table_water);
	writeDword(0x004E45E9, (DWORD)indirect_table_water);
	writeDword(0x004E4617, (DWORD)indirect_table_water);
	writeDword(0x004E463C, (DWORD)indirect_table_water);
	writeDword(0x004E4663, (DWORD)indirect_table_water);
	writeDword(0x004E468E, (DWORD)indirect_table_water);
	writeDword(0x004E46B5, (DWORD)indirect_table_water);
	writeDword(0x004E46DC, (DWORD)indirect_table_water);
	writeDword(0x004E4706, (DWORD)indirect_table_water);
	writeDword(0x004E472B, (DWORD)indirect_table_water);
	writeDword(0x004E4752, (DWORD)indirect_table_water);
	writeDword(0x004E4780, (DWORD)indirect_table_water);
	writeDword(0x004E47A5, (DWORD)indirect_table_water);
	writeDword(0x004E47CC, (DWORD)indirect_table_water);

	//6
	writeDword(0x004E60CA, (DWORD)indirect_table_water);
	writeDword(0x004E60F3, (DWORD)indirect_table_water);
	writeDword(0x004E6120, (DWORD)indirect_table_water);
	writeDword(0x004E6145, (DWORD)indirect_table_water);
	writeDword(0x004E616A, (DWORD)indirect_table_water);
	writeDword(0x004E6197, (DWORD)indirect_table_water);
	writeDword(0x004E61BC, (DWORD)indirect_table_water);
	writeDword(0x004E61E1, (DWORD)indirect_table_water);
	writeDword(0x004E623C, (DWORD)indirect_table_water);
	writeDword(0x004E6263, (DWORD)indirect_table_water);
	writeDword(0x004E628D, (DWORD)indirect_table_water);
	writeDword(0x004E62B2, (DWORD)indirect_table_water);
	writeDword(0x004E62D9, (DWORD)indirect_table_water);
	writeDword(0x004E6307, (DWORD)indirect_table_water);
	writeDword(0x004E632C, (DWORD)indirect_table_water);
	writeDword(0x004E6353, (DWORD)indirect_table_water);
	writeDword(0x004E637E, (DWORD)indirect_table_water);
	writeDword(0x004E63A5, (DWORD)indirect_table_water);
	writeDword(0x004E63CC, (DWORD)indirect_table_water);
	writeDword(0x004E63F6, (DWORD)indirect_table_water);
	writeDword(0x004E641B, (DWORD)indirect_table_water);
	writeDword(0x004E6442, (DWORD)indirect_table_water);
	writeDword(0x004E6470, (DWORD)indirect_table_water);
	writeDword(0x004E6495, (DWORD)indirect_table_water);
	writeDword(0x004E64BC, (DWORD)indirect_table_water);

	//7
	writeDword(0x005CAA88, (DWORD)indirect_table_water);

	//8
	writeDword(0x005CC910, (DWORD)indirect_table_water);

	//1
	writeByte(0x005CCA60, TERR_MAX_CONST);
	writeByte(0x005CCAAF, TERR_MAX_CONST);
	writeByte(0x005CCAE6, TERR_MAX_CONST);
	writeByte(0x005CCB0C, TERR_MAX_CONST);
	writeByte(0x005CCB38, TERR_MAX_CONST);
	writeByte(0x005CCB71, TERR_MAX_CONST);
	writeByte(0x005CCBA6, TERR_MAX_CONST);
	writeByte(0x005CCBE0, TERR_MAX_CONST);
	writeByte(0x005CCC16, TERR_MAX_CONST);
	writeByte(0x005CCC75, TERR_MAX_CONST);

	//2
	writeByte(0x005CBCF1, TERR_MAX_CONST);
	writeByte(0x005CBD18, TERR_MAX_CONST);
	writeByte(0x005CBD3E, TERR_MAX_CONST);
	writeByte(0x005CBD63, TERR_MAX_CONST);
	writeByte(0x005CBD8A, TERR_MAX_CONST);
	writeByte(0x005CBDB8, TERR_MAX_CONST);
	writeByte(0x005CBDDD, TERR_MAX_CONST);
	writeByte(0x005CBE08, TERR_MAX_CONST);
	writeByte(0x005CBE46, TERR_MAX_CONST);
	writeByte(0x005CBE6F, TERR_MAX_CONST);

	writeByte(0x005CBEDF, TERR_MAX_CONST);
	writeByte(0x005CBF04, TERR_MAX_CONST);
	writeByte(0x005CBF2B, TERR_MAX_CONST);

	writeByte(0x005CBFAE, TERR_MAX_CONST);
	writeByte(0x005CBFD3, TERR_MAX_CONST);
	writeByte(0x005CBFFA, TERR_MAX_CONST);

	//3
	writeByte(0x005CC0C1, TERR_MAX_CONST);
	writeByte(0x005CC0E8, TERR_MAX_CONST);
	writeByte(0x005CC10E, TERR_MAX_CONST);
	writeByte(0x005CC133, TERR_MAX_CONST);
	writeByte(0x005CC15A, TERR_MAX_CONST);
	writeByte(0x005CC184, TERR_MAX_CONST);
	writeByte(0x005CC1A9, TERR_MAX_CONST);
	writeByte(0x005CC1D0, TERR_MAX_CONST);

	//4
	writeByte(0x005CC1F9, TERR_MAX_CONST);
	writeByte(0x005CC220, TERR_MAX_CONST);
	writeByte(0x005CC247, TERR_MAX_CONST);
	writeByte(0x005CC26D, TERR_MAX_CONST);
	writeByte(0x005CC292, TERR_MAX_CONST);
	writeByte(0x005CC2B9, TERR_MAX_CONST);
	writeByte(0x005CC2E3, TERR_MAX_CONST);
	writeByte(0x005CC308, TERR_MAX_CONST);
	writeByte(0x005CC32F, TERR_MAX_CONST);

	//new
	//5
	writeByte(0x004E43D3, TERR_MAX_CONST);
	writeByte(0x004E43FC, TERR_MAX_CONST);
	writeByte(0x004E4429, TERR_MAX_CONST);
	writeByte(0x004E444E, TERR_MAX_CONST);
	writeByte(0x004E4473, TERR_MAX_CONST);
	writeByte(0x004E44A0, TERR_MAX_CONST);
	writeByte(0x004E44C5, TERR_MAX_CONST);
	writeByte(0x004E44EA, TERR_MAX_CONST);
	writeByte(0x004E4545, TERR_MAX_CONST);
	writeByte(0x004E456C, TERR_MAX_CONST);
	writeByte(0x004E4596, TERR_MAX_CONST);
	writeByte(0x004E45BB, TERR_MAX_CONST);
	writeByte(0x004E45E2, TERR_MAX_CONST);
	writeByte(0x004E4610, TERR_MAX_CONST);
	writeByte(0x004E4635, TERR_MAX_CONST);
	writeByte(0x004E465C, TERR_MAX_CONST);
	writeByte(0x004E4687, TERR_MAX_CONST);
	writeByte(0x004E46AE, TERR_MAX_CONST);
	writeByte(0x004E46D5, TERR_MAX_CONST);
	writeByte(0x004E46FF, TERR_MAX_CONST);
	writeByte(0x004E4724, TERR_MAX_CONST);
	writeByte(0x004E474B, TERR_MAX_CONST);
	writeByte(0x004E4779, TERR_MAX_CONST);
	writeByte(0x004E479E, TERR_MAX_CONST);
	writeByte(0x004E47C5, TERR_MAX_CONST);

	//6
	writeByte(0x004E60C3, TERR_MAX_CONST);
	writeByte(0x004E60EC, TERR_MAX_CONST);
	writeByte(0x004E6119, TERR_MAX_CONST);
	writeByte(0x004E613E, TERR_MAX_CONST);
	writeByte(0x004E6163, TERR_MAX_CONST);
	writeByte(0x004E6190, TERR_MAX_CONST);
	writeByte(0x004E61B5, TERR_MAX_CONST);
	writeByte(0x004E61DA, TERR_MAX_CONST);
	writeByte(0x004E6235, TERR_MAX_CONST);
	writeByte(0x004E625C, TERR_MAX_CONST);
	writeByte(0x004E6286, TERR_MAX_CONST);
	writeByte(0x004E62AB, TERR_MAX_CONST);
	writeByte(0x004E62D2, TERR_MAX_CONST);
	writeByte(0x004E6300, TERR_MAX_CONST);
	writeByte(0x004E6325, TERR_MAX_CONST);
	writeByte(0x004E634C, TERR_MAX_CONST);
	writeByte(0x004E6377, TERR_MAX_CONST);
	writeByte(0x004E639E, TERR_MAX_CONST);
	writeByte(0x004E63C5, TERR_MAX_CONST);
	writeByte(0x004E63EF, TERR_MAX_CONST);
	writeByte(0x004E6414, TERR_MAX_CONST);
	writeByte(0x004E643B, TERR_MAX_CONST);
	writeByte(0x004E6469, TERR_MAX_CONST);
	writeByte(0x004E648E, TERR_MAX_CONST);
	writeByte(0x004E64B5, TERR_MAX_CONST);

	//7
	writeByte(0x005CAA81, TERR_MAX_CONST);

	//8
	writeByte(0x005CC909, TERR_MAX_CONST);

	setHook((void*)0x005CBE92, snow_terrain_fix_1);
	setHook((void*)0x005CBEAF, snow_terrain_fix_2);
	setHook((void*)0x005CBF42, snow_terrain_fix_3);
	setHook((void*)0x005CBF5D, snow_terrain_fix_4);
	setHook((void*)0x005CBF7A, snow_terrain_fix_5);
	setHook((void*)0x005CC011, snow_terrain_fix_6);
	setHook((void*)0x005CC02C, snow_terrain_fix_7);
	setHook((void*)0x005CC049, snow_terrain_fix_8);
	setHook((void*)0x005CC095, snow_terrain_fix_9);
	setHook((void*)0x004E4519, snow_terrain_fix_10);
	setHook((void*)0x004E6209, snow_terrain_fix_11);
	setHook((void*)0x00554995, snow_terrain_fix_12);

	setHook((void*)0x005CBCCE, ice_terrain_fix);

	//RGE stack array
	writeDword(0x004DE832, 0x13C + (TERRAIN_COUNT - 55) * 4);
	writeDword(0x004DE96F, 0x13C + (TERRAIN_COUNT - 55) * 4);
	writeDword(0x004DED39, 0x13C + (TERRAIN_COUNT - 55) * 4);
	//writeByte(0x004DE9E0, TERRAIN_COUNT); //replace with asm
	//
	writeDword(0x004DE83A, 0x144 + (TERRAIN_COUNT - 55) * 4);
	writeDword(0x004DE861, 0x154 + (TERRAIN_COUNT - 55) * 4);
	writeDword(0x004DE86E, 0x154 + (TERRAIN_COUNT - 55) * 4);
	writeDword(0x004DE889, 0x158 + (TERRAIN_COUNT - 55) * 4);
	writeDword(0x004DE8E7, 0x158 + (TERRAIN_COUNT - 55) * 4);
	writeDword(0x004DE9B2, 0x154 + (TERRAIN_COUNT - 55) * 4);
	writeDword(0x004DE9B9, 0x158 + (TERRAIN_COUNT - 55) * 4);
	writeDword(0x004DE854, 0x158 + (TERRAIN_COUNT - 55) * 4);
	writeDword(0x004DE8E0, 0x160 + (TERRAIN_COUNT - 55) * 4);
	writeDword(0x004DE980, 0x160 + (TERRAIN_COUNT - 55) * 4);
	writeDword(0x004DE908, 0x164 + (TERRAIN_COUNT - 55) * 4);

	//0x37 -> 104
	//NEW!!
	writeDword(0x004DE9A3, TERRAIN_COUNT);
	//
	//writeByte(0x004DE2E4, TERRAIN_COUNT); //replace with asm (replaced)
	//writeByte(0x004DE61C, TERRAIN_COUNT); //replace with asm (replaced)
	//writeByte(0x004E5F95, TERRAIN_COUNT); //NOT SURE (0x63), replace with asm? (replaced)
	writeDword(0x004E5F72, (TERRAIN_COUNT - 0x63) * 4 + 0x190);
	writeDword(0x004E602E, (TERRAIN_COUNT - 0x63) * 4 + 0x190);
	//writeByte(0x004E5FA7, TERRAIN_COUNT); //NOT SURE (0x63), replace with asm? (replaced)

	setHook((void*)0x004E7542, onMapSizeRMSDefine);

	//SUPER DUMB crash fix
	//writeByte(0x004977E1, 0xEB);

	//terrain crash fix
	setHook((void*)0x0060DC77, terrain_forbidden_ids_2);
	setHook((void*)0x00497141, terrain_memory_fix);
	setHook((void*)0x005006F4, terrain_sound_fix);

	setHook((void*)0x005856DE, onResourceFound_carbon);
}

void setExtraTerrainHooks_CC()
{
	writeByte(0x0048FB72, 0xEB);	//skip border loading
	writeByte(0x0048FB73, 0x06);

	writeDword(0x004B0435, (DWORD)terrain_array);
	writeDword(0x004B0CBC, (DWORD)terrain_array);
	writeByte(0x004B0CC8, TERRAIN_COUNT);

	writeDword(0x0049479B, TERRAIN_COUNT);

	writeByte(0x00495063, TERRAIN_COUNT);
	writeByte(0x00495530, TERRAIN_COUNT);
	writeByte(0x00495B6C, TERRAIN_COUNT);

	writeByte(0x004901DE, 0xEB); //skip border unloading
}
#pragma optimize( "", on )

//blend array: 0069B600
//reads here: 00610147
//blendomatic unload: 0060A240