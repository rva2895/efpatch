#include "stdafx.h"

void setScrollHooks()
{
	setByte(0x005FD5E5, 0xEB);
	setByte(0x005FD61F, 0xEB);
	setByte(0x005FD659, 0xEB);
	setByte(0x005FD692, 0xEB);
}
