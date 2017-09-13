#include "stdafx.h"
#include "aicommandunits.h"

#define SHIFT_1 1200
#define SHIFT_2 600

void setAICommandUnitsHooks()
{
	setInt(0x005AE8C2, SHIFT_1 + 0x1C4);
	setInt(0x005AF33F, SHIFT_1 + 0x1C4);
	setInt(0x005AF3F5, SHIFT_1 + 0x1C4);

	setInt(0x005AE965, SHIFT_2 + 0x10C);
	setInt(0x005AE97C, SHIFT_2 + 0x114);
	setInt(0x005AE9CC, SHIFT_2 + 0x10C);
	setInt(0x005AF061, SHIFT_2 + 0x114);

	setInt(0x005AE8EC, SHIFT_1 + 0x1D8);
	setInt(0x005AEA2B, SHIFT_1 + 0x1D8);
	setInt(0x005AF15B, SHIFT_1 + 0x1D8);
	setInt(0x005AE925, SHIFT_1 + 0x1DC);
	setInt(0x005AF172, SHIFT_1 + 0x1DC);
	setInt(0x005AF1EC, SHIFT_1 + 0x1DC);
}
