#include "stdafx.h"

#include "terrain.h"

/*void* (__thiscall* slpAlloc)(void* ptr, char* name, int id) =
	(void* (__thiscall*) (void*, char*, int))0x0542870;

int* map_current_x = (int*)0x006ACCE4;
int* map_current_y = (int*)0x006ACCE8;*/

#define TERRAIN_COUNT 93

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
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4	//0x60
};

void setExtraTerrainHooks()
{
	setByte(0x0048FB72, 0xEB);	//skip border loading
	setByte(0x0048FB73, 0x06);

	setInt(0x004B0435, (int)terrain_array);
	setInt(0x004B0CBC, (int)terrain_array);
	setByte(0x004B0CC8, TERRAIN_COUNT);

	setInt(0x0049479B, TERRAIN_COUNT);
	setInt(0x00494796, 0x6368 + 0x1D4 * (TERRAIN_COUNT - 0x37));

	setByte(0x00495063, TERRAIN_COUNT);
	setByte(0x00495530, TERRAIN_COUNT);
	setByte(0x00495B6C, TERRAIN_COUNT);

	setByte(0x004901DE, 0xEB); //skip border unloading

	//collision manager array
	/*(setByte(0x004A8998, TERRAIN_COUNT);			//common
	setByte(0x004A89E0, TERRAIN_COUNT);
	setInt(0x004A88E2, TERRAIN_COUNT * 4 + 4);
	setInt(0x004A89AA, TERRAIN_COUNT * 4 + 4);

	setInt(0x004B01EA, TERRAIN_COUNT * 4 + 8);	//land
	setInt(0x00414538, TERRAIN_COUNT * 4 + 8);	//air
	*/
}
