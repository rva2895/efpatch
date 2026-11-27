#include "stdafx.h"
#include "shield.h"

extern int current_loaded_version;

// TOMBSTONED 2026-11-27: ADOPTED VERBATIM as RGE_Static_Object__is_shielded_new
// (src/engine/object/RGE_Static_Object.cpp, batch S4) per the collision naming
// rule - the handler was a full reimplementation hooked at the is_shielded
// entry 0x0054BC40 (setShieldHooks retired below). The adopted body carries
// the deliberate deltas: the version-gated master-id extension {4069,4778,
// 5904} at DAT v7+ (current_loaded_version >= 7), the group-59 partition
// restructure (59 -> the return-false case), and the int-truncation form.

#pragma optimize( "s", on )
void setShieldHooks()
{
    // retired 2026-11-27: the entry hook is superseded by the ADOPTED
    // RGE_Static_Object__is_shielded_new (the canonical _new took over; the
    // E9 here would fight it - the last installer wins and the adoption IS
    // the replacement)
    //setHook((void*)0x0054BC40, gbg_is_shielded_new);
}
#pragma optimize( "", on )
