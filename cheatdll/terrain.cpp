#include "stdafx.h"

#include "terrain.h"

struct slpParam
{
	char baseTerrain;
	unsigned char color1;
	unsigned char color2;
	unsigned char color3;
	int slpId;
	void* slpPtr;
};

slpParam* extraTerrains;
int nExtraTerrains;

char* extraTerrainMap;

void* (__thiscall* slpAlloc)(void* ptr, char* name, int id) =
	(void* (__thiscall*) (void*, char*, int))0x0542870;

int* map_current_x = (int*)0x006ACCE4;
int* map_current_y = (int*)0x006ACCE8;

void initExtraTerrain()
{
	extraTerrainMap = (char*)malloc(256 * 256);
	memset(extraTerrainMap, 0, 256 * 256);
	nExtraTerrains = 1;
	extraTerrains = new slpParam;
	extraTerrains->baseTerrain = 3;
	extraTerrains->color1 = 0x37;
	extraTerrains->color2 = 0xEC;
	extraTerrains->color3 = 0x36;
	extraTerrains->slpId = 15043;
	extraTerrains->slpPtr = 0;
}

void __stdcall slpAllocations()
{
	extraTerrains->slpPtr = malloc(0x1C);
	slpAlloc(extraTerrains->slpPtr, "None", extraTerrains->slpId);
}

__declspec(naked) void onSlpAlloc() //004947D3
{
	__asm
	{
		mov		ecx, eax
		mov		eax, 542870h
		call	eax
		push    eax
		call	slpAllocations
		pop		eax
		push	4947DEh
		ret
	}
}

void* __stdcall getExtraSlp()
{
	char extra = extraTerrainMap[*map_current_y * 256 + *map_current_x];
	if (extra)
		return extraTerrains[extra - 1].slpPtr;
	else
		return 0;
}

__declspec(naked) void terrainSlp1() //0061024C
{
	__asm
	{
		push	ecx
		mov		edi, [edx + eax*4 + 0B4h]
		call	getExtraSlp
		test	eax, eax
		jz		_noExtraSlp1
		mov		edi, eax
_noExtraSlp1:
		pop		ecx
		push	610253h
		ret
	}
}

__declspec(naked) void terrainSlp2() //00610A89
{
	__asm
	{
		push	ecx
		mov		edi, [edx + eax * 4 + 0B4h]
		call	getExtraSlp
		test	eax, eax
		jz		_noExtraSlp2
		mov		edi, eax
_noExtraSlp2:
		pop		ecx
		push	610A90h
		ret
	}
}

__declspec(naked) void terrainSlp3() //00610123
{
	__asm
	{
		mov     ecx, [eax + edi + 0B4h]
		push	ecx
		call	getExtraSlp
		test	eax, eax
		jz		_noExtraSlp3
		pop		ecx
		mov		ecx, eax
		push	61012Ah
		ret
_noExtraSlp3:
		pop		ecx
		push	61012Ah
		ret
	}
}

__declspec(naked) void terrainSlp4() //006100D9
{
	__asm
	{
		mov     edx, [ecx + eax + 0B4h]
		push	edx
		call	getExtraSlp
		test	eax, eax
		jz		_noExtraSlp4
		pop		edx
		mov		edx, eax
		push	6100E0h
		ret
_noExtraSlp4:
		pop		edx
		push	6100E0h
		ret
	}
}

void __stdcall clearExtra(int x, int y, int size)
{
	int ystart = y - size / 2;
	if (ystart < 0)
		ystart = 0;
	int xstart = x - size / 2;
	if (xstart < 0)
		xstart = 0;

	for (int i = xstart; i <= x + size / 2; i++)
		for (int j = ystart; j <= y + size / 2; j++)
			extraTerrainMap[j * 256 + i] = 0;
}

int __stdcall setExtra(int id, int x, int y, int size) //returns base terrain id
{
	int ystart = y - size / 2;
	if (ystart < 0)
		ystart = 0;
	int xstart = x - size / 2;
	if (xstart < 0)
		xstart = 0;

	for (int i = xstart; i <= x + size / 2; i++)
		for (int j = ystart; j <= y + size / 2; j++)
			extraTerrainMap[j * 256 + i] = 1;

	return 3;
}

__declspec(naked) void onPaint() //00612904
{
	__asm
	{
		cmp     ax, 0FFFFh
		jz		loc_6129CA

		and		eax, 0FFh
		cmp		ax, 55
		jae		_extra
		push	eax
		push	ecx
		push	edx
		movsx	edx, dx
		push	edx				//size
		push	edi				//y
		push	ebx				//x
		call	clearExtra
		pop		edx
		pop		ecx
		pop		eax
		push	61290Eh
		ret
_extra:
		push	eax
		push	ecx
		push	edx
		movsx	edx, dx
		push	edx				//size
		push	edi				//y
		push	ebx				//x
		push	eax				//id
		call	setExtra
		pop		edx
		pop		ecx
		add		esp, 4			//discard eax
		push	61290Eh
		ret
loc_6129CA:
		push	6129CAh
		ret
	}
}

void setExtraTerrainHooks()
{
	initExtraTerrain();

	setExtra(55, 2, 2, 1);

	setHook((void*)0x004947D3, &onSlpAlloc);

	setHook((void*)0x0061024C, &terrainSlp1);
	setHook((void*)0x00610A89, &terrainSlp2);
	setHook((void*)0x00610123, &terrainSlp3);
	setHook((void*)0x006100D9, &terrainSlp4);

	setHook((void*)0x00612904, &onPaint);
}
