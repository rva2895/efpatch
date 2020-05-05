#include "stdafx.h"

#pragma optimize( "s", on )
void setMenuHooks()
{
	//single player
	writeDword(0x0050B514, 283);
	writeDword(0x0050B50F, 195);
	writeDword(0x0050B50A, 125);
	writeByte(0x0050B508, 93);
	//multiplayer
	writeDword(0x0050B533, 206);
	writeDword(0x0050B52C, 363);
	writeDword(0x0050B527, 184);
	writeDword(0x0050B522, 110);
	//databank
	writeByte(0x0050B552, 37);
	writeDword(0x0050B54B, 212);
	writeDword(0x0050B546, 100);
	writeDword(0x0050B541, 90);
	//scenario editor
	writeDword(0x0050B56B, 183);
	writeDword(0x0050B564, 518);
	writeDword(0x0050B55F, 123);
	writeByte(0x0050B55D, 74);
	//logo: no changes
	//exit: no changes
	//basic training
	writeDword(0x0050B5AF, 282);
	writeByte(0x0050B5AB, 97);
	writeDword(0x0050B5A6, 122);
	writeByte(0x0050B5A4, 77);
	//options
	writeByte(0x0050B5CE, 31);
	writeDword(0x0050B5C7, 369);
	writeDword(0x0050B5C2, 111);
	writeDword(0x0050B5BD, 119);
	//zone
	writeByte(0x0050B5E4, 100);
	writeByte(0x0050B5E0, 91);
	writeDword(0x0050B5DB, 107);
	writeByte(0x0050B5D9, 80);

	//todo: text rects
}
#pragma optimize( "", on )
