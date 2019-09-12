#include "stdafx.h"

#include "rescheat.h"

#pragma optimize( "s", on )
void setResCheatHooks()
{
	int f10k = 0x461C4000;  //10000f
	writeDword(0x00604407, f10k);
	writeDword(0x00604433, f10k);
	writeDword(0x0060445F, f10k);
	writeDword(0x0060448B, f10k);
}
#pragma optimize( "", on )
