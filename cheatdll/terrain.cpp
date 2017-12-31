#include "stdafx.h"

#include "terrain.h"

#define TERRAIN_COUNT 104

BYTE terrain_array[] =
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
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,	//0x60
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4	//0x70
};

BYTE indirect_table_water[] =	//starts from 1 (TERR-WATER1)
{
	0, 1, 1, 0,
	1, 1, 1, 1,
	1, 1, 1, 1,
	1, 1, 1, 1,
	1, 1, 1, 1,
	1, 0, 0,				//WATER2 and WATER3
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 0		//MARSH
};

#define TERR_MAX_CONST 58

//BYTE indirect_table_water_2[TERR_MAX_CONST+1];

void terrain_transition_change(int state)
{
	//setByte(0x005CBE20, state ? 0xFF : 2);
	indirect_table_water[0] = state;	//WATER1
	indirect_table_water[3] = state;	//WATERSWAMP
	indirect_table_water[21] = state;	//WATER2
	indirect_table_water[22] = state;	//WATER3
	indirect_table_water[58] = state;	//MARSH
	/*for (int i = 0; i < sizeof(indirect_table_water); i++)
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
		}*/
}

__declspec(naked) void snow_terrain_fix_1() //005CBE92
{
	__asm
	{
		cmp     dl, 20h
		jb      _not_snow1
		cmp     dl, 22h
		jbe     _snow1
		cmp     dl, 47
		jz      _snow1
		cmp     dl, 55
		jz      _snow1
		cmp     dl, 63
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
		cmp     dl, 47
		jz      _snow2
		cmp     dl, 55
		jz      _snow2
		cmp     dl, 63
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
		cmp     bl, 47
		jz      _snow3
		cmp     bl, 55
		jz      _snow3
		cmp     bl, 63
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
		cmp     dl, 47
		jz      _snow4
		cmp     dl, 55
		jz      _snow4
		cmp     dl, 63
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
		cmp     dl, 47
		jz      _snow5
		cmp     dl, 55
		jz      _snow5
		cmp     dl, 63
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
		cmp     bl, 47
		jz      _snow6
		cmp     bl, 55
		jz      _snow6
		cmp     bl, 63
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
		cmp     dl, 47
		jz      _snow7
		cmp     dl, 55
		jz      _snow7
		cmp     dl, 63
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
		cmp     al, 47
		jz      _snow8
		cmp     al, 55
		jz      _snow8
		cmp     al, 63
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
		cmp     dl, 47
		jz      _snow9
		cmp     dl, 55
		jz      _snow9
		cmp     dl, 63
		jz      _snow9
_not_snow9:
		push    005CC1F6h
		ret
_snow9:
		push    005CC0ACh
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
void setExtraTerrainHooks()
{
	setByte(0x0048FB72, 0xEB);	//skip border loading
	setByte(0x0048FB73, 0x06);

	setInt(0x004B0435, (DWORD)terrain_array);
	setInt(0x004B0CBC, (DWORD)terrain_array);
	setByte(0x004B0CC8, TERRAIN_COUNT);

	setInt(0x0049479B, TERRAIN_COUNT);
	setInt(0x00494796, 0x6368 + 0x1D4 * (TERRAIN_COUNT - 0x37));

	setByte(0x00495063, TERRAIN_COUNT);
	setByte(0x00495530, TERRAIN_COUNT);
	setByte(0x00495B6C, TERRAIN_COUNT);

	setByte(0x004901DE, 0xEB); //skip border unloading

	//memcpy(indirect_table_water_2, indirect_table_water, TERR_MAX_CONST + 1);

	//1
	setInt(0x005CCA67, (DWORD)indirect_table_water);
	setInt(0x005CCAB6, (DWORD)indirect_table_water);
	setInt(0x005CCAED, (DWORD)indirect_table_water);
	setInt(0x005CCB13, (DWORD)indirect_table_water);
	setInt(0x005CCB3F, (DWORD)indirect_table_water);
	setInt(0x005CCB78, (DWORD)indirect_table_water);
	setInt(0x005CCBAD, (DWORD)indirect_table_water);
	setInt(0x005CCBE7, (DWORD)indirect_table_water);
	setInt(0x005CCC1D, (DWORD)indirect_table_water);
	setInt(0x005CCC80, (DWORD)indirect_table_water);

	//2
	setInt(0x005CBCF8, (DWORD)indirect_table_water);
	setInt(0x005CBD1F, (DWORD)indirect_table_water);
	setInt(0x005CBD45, (DWORD)indirect_table_water);
	setInt(0x005CBD6A, (DWORD)indirect_table_water);
	setInt(0x005CBD91, (DWORD)indirect_table_water);
	setInt(0x005CBDBF, (DWORD)indirect_table_water);
	setInt(0x005CBDE4, (DWORD)indirect_table_water);
	setInt(0x005CBE13, (DWORD)indirect_table_water);
	setInt(0x005CBE4D, (DWORD)indirect_table_water);
	setInt(0x005CBE76, (DWORD)indirect_table_water);

	setInt(0x005CBEE6, (DWORD)indirect_table_water);
	setInt(0x005CBF0B, (DWORD)indirect_table_water);
	setInt(0x005CBF32, (DWORD)indirect_table_water);

	setInt(0x005CBFB5, (DWORD)indirect_table_water);
	setInt(0x005CBFDA, (DWORD)indirect_table_water);
	setInt(0x005CC001, (DWORD)indirect_table_water);

	//3
	setInt(0x005CC0C8, (DWORD)indirect_table_water);
	setInt(0x005CC0EF, (DWORD)indirect_table_water);
	setInt(0x005CC115, (DWORD)indirect_table_water);
	setInt(0x005CC13A, (DWORD)indirect_table_water);
	setInt(0x005CC161, (DWORD)indirect_table_water);
	setInt(0x005CC18B, (DWORD)indirect_table_water);
	setInt(0x005CC1B0, (DWORD)indirect_table_water);
	setInt(0x005CC1D7, (DWORD)indirect_table_water);

	//4
	setInt(0x005CC200, (DWORD)indirect_table_water);
	setInt(0x005CC227, (DWORD)indirect_table_water);
	setInt(0x005CC24E, (DWORD)indirect_table_water);
	setInt(0x005CC274, (DWORD)indirect_table_water);
	setInt(0x005CC299, (DWORD)indirect_table_water);
	setInt(0x005CC2C0, (DWORD)indirect_table_water);
	setInt(0x005CC2EA, (DWORD)indirect_table_water);
	setInt(0x005CC30F, (DWORD)indirect_table_water);
	setInt(0x005CC336, (DWORD)indirect_table_water);

	//1
	setByte(0x005CCA60, TERR_MAX_CONST);
	setByte(0x005CCAAF, TERR_MAX_CONST);
	setByte(0x005CCAE6, TERR_MAX_CONST);
	setByte(0x005CCB0C, TERR_MAX_CONST);
	setByte(0x005CCB38, TERR_MAX_CONST);
	setByte(0x005CCB71, TERR_MAX_CONST);
	setByte(0x005CCBA6, TERR_MAX_CONST);
	setByte(0x005CCBE0, TERR_MAX_CONST);
	setByte(0x005CCC16, TERR_MAX_CONST);
	setByte(0x005CCC75, TERR_MAX_CONST);

	//2
	setByte(0x005CBCF1, TERR_MAX_CONST);
	setByte(0x005CBD18, TERR_MAX_CONST);
	setByte(0x005CBD3E, TERR_MAX_CONST);
	setByte(0x005CBD63, TERR_MAX_CONST);
	setByte(0x005CBD8A, TERR_MAX_CONST);
	setByte(0x005CBDB8, TERR_MAX_CONST);
	setByte(0x005CBDDD, TERR_MAX_CONST);
	setByte(0x005CBE08, TERR_MAX_CONST);
	setByte(0x005CBE46, TERR_MAX_CONST);
	setByte(0x005CBE6F, TERR_MAX_CONST);

	setByte(0x005CBEDF, TERR_MAX_CONST);
	setByte(0x005CBF04, TERR_MAX_CONST);
	setByte(0x005CBF2B, TERR_MAX_CONST);

	setByte(0x005CBFAE, TERR_MAX_CONST);
	setByte(0x005CBFD3, TERR_MAX_CONST);
	setByte(0x005CBFFA, TERR_MAX_CONST);

	//3
	setByte(0x005CC0C1, TERR_MAX_CONST);
	setByte(0x005CC0E8, TERR_MAX_CONST);
	setByte(0x005CC10E, TERR_MAX_CONST);
	setByte(0x005CC133, TERR_MAX_CONST);
	setByte(0x005CC15A, TERR_MAX_CONST);
	setByte(0x005CC184, TERR_MAX_CONST);
	setByte(0x005CC1A9, TERR_MAX_CONST);
	setByte(0x005CC1D0, TERR_MAX_CONST);

	//4
	setByte(0x005CC1F9, TERR_MAX_CONST);
	setByte(0x005CC220, TERR_MAX_CONST);
	setByte(0x005CC247, TERR_MAX_CONST);
	setByte(0x005CC26D, TERR_MAX_CONST);
	setByte(0x005CC292, TERR_MAX_CONST);
	setByte(0x005CC2B9, TERR_MAX_CONST);
	setByte(0x005CC2E3, TERR_MAX_CONST);
	setByte(0x005CC308, TERR_MAX_CONST);
	setByte(0x005CC32F, TERR_MAX_CONST);

	setHook((void*)0x005CBE92, snow_terrain_fix_1);
	setHook((void*)0x005CBEAF, snow_terrain_fix_2);
	setHook((void*)0x005CBF42, snow_terrain_fix_3);
	setHook((void*)0x005CBF5D, snow_terrain_fix_4);
	setHook((void*)0x005CBF7A, snow_terrain_fix_5);
	setHook((void*)0x005CC011, snow_terrain_fix_6);
	setHook((void*)0x005CC02C, snow_terrain_fix_7);
	setHook((void*)0x005CC049, snow_terrain_fix_8);
	setHook((void*)0x005CC095, snow_terrain_fix_9);

	//RGE stack array
	setInt(0x004DE832, 0x13C + (TERRAIN_COUNT - 55) * 4);
	setInt(0x004DE96F, 0x13C + (TERRAIN_COUNT - 55) * 4);
	setInt(0x004DED39, 0x13C + (TERRAIN_COUNT - 55) * 4);
	setByte(0x004DE9E0, TERRAIN_COUNT);
	//
	setInt(0x004DE83A, 0x144 + (TERRAIN_COUNT - 55) * 4);
	setInt(0x004DE861, 0x154 + (TERRAIN_COUNT - 55) * 4);
	setInt(0x004DE86E, 0x154 + (TERRAIN_COUNT - 55) * 4);
	setInt(0x004DE889, 0x158 + (TERRAIN_COUNT - 55) * 4);
	setInt(0x004DE8E7, 0x158 + (TERRAIN_COUNT - 55) * 4);
	setInt(0x004DE9B2, 0x154 + (TERRAIN_COUNT - 55) * 4);
	setInt(0x004DE9B9, 0x158 + (TERRAIN_COUNT - 55) * 4);
	setInt(0x004DE854, 0x158 + (TERRAIN_COUNT - 55) * 4);
	setInt(0x004DE8E0, 0x160 + (TERRAIN_COUNT - 55) * 4);
	setInt(0x004DE980, 0x160 + (TERRAIN_COUNT - 55) * 4);
	setInt(0x004DE908, 0x164 + (TERRAIN_COUNT - 55) * 4);

	//0x37 -> 104
	setByte(0x004DE2E4, TERRAIN_COUNT);
	setByte(0x004DE61C, TERRAIN_COUNT);
	setByte(0x004E5F95, TERRAIN_COUNT); //NOT SURE (0x63)
	setInt(0x004E5F72, (TERRAIN_COUNT - 0x63) * 4 + 0x190);
	setInt(0x004E602E, (TERRAIN_COUNT - 0x63) * 4 + 0x190);
	setByte(0x004E5FA7, TERRAIN_COUNT); //NOT SURE (0x63)

	setHook((void*)0x004E7542, onMapSizeRMSDefine);
}
