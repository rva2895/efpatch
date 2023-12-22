#include "stdafx.h"
#include "hideineditor.h"

#pragma optimize( "s", on )
void setHideInEditorHooks()
{
    writeNops(0x0053A3E4, 6);
    writeNops(0x0053A3F6, 6);

    writeNops(0x0053A5B8, 6);
    writeNops(0x0053A5CA, 6);
}
#pragma optimize( "", on )
