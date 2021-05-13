#include "stdafx.h"

#pragma optimize( "s", on )
void setScrollHooks()
{
    writeByte(0x005FD5E5, 0xEB);
    writeByte(0x005FD61F, 0xEB);
    writeByte(0x005FD659, 0xEB);
    writeByte(0x005FD692, 0xEB);
}
#pragma optimize( "", on )
