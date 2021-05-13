#pragma once

#include "consts.h"

struct ID_ASSIGNU
{
#ifdef _CHEATDLL_CC
    short IDs[10];
#else
    short IDs[CIV_COUNT];
#endif
};

void initCivUnitTable();
