#include "stdafx.h"
#include "tribe_command.h"
#include "mbs.h"
#include "hotkeys.h"
#include "tribe_command_ef.h"

const uint32_t dispatch_command_offsets[] =
{
    0x005B9EF3,
    0x005B9E1A,
    0x005B9E2A,
    0x005B9E3A,
    0x005B9EE3,
    0x005B9E5A,
    0x005B9E4A,
    0x005B9F45,
    0x005B9F8A,
    0x005B9F9A,
    0x005B9FAA,
    0x005B9FE0,
    0x005BA085,
    0x005B9FF0,
    0x005BA095,
    0x005BA0A5,
    0x005BA000,
    0x005BA055,
    0x005BA065,
    0x005BA075,
    0x005BA0B5,
    0x005BA0C5,
    0x005BA0D5,
    0x005BA11B,
    0x005BA161,
    0x005BA185,
    0x005BA195,
    0x005BA1A5,
    0x005BA1B5,
    0x005BA1D5,
    (uint32_t)TRIBE_Command__do_command_mbs,
    (uint32_t)TRIBE_Command__do_command_shift_delete,
    (uint32_t)TRIBE_Command__do_command_ef
};

void setTribeCommandHooks(int version)
{
    writeDword(0x005B9E16, (DWORD)dispatch_command_offsets);
    writeByte(0x005BA2DD, 0x1E); //dispatch table offset
    writeByte(0x005BA2DE, 0x1F);
    if (version == VER_EF)
        writeByte(0x005BA2DF, 0x20);
    if (version == VER_EF)
        writeByte(0x005B9E04, 0x7F);
    else
        writeByte(0x005B9E04, 0x7E); //max command id -> 0x7E

    //remove tribe create
    setHook((void*)0x005B9EF3, (void*)0x005B9F3D);
    
    if (version == VER_EF)
        setTribeCommandEFHooks();
}
