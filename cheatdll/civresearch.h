#pragma once

#include "consts.h"

struct ID_ASSIGNR
{
#ifdef _CHEATDLL_CC
	short IDs[10];
#else
	short IDs[CIV_COUNT];
#endif
};

void initCivResearchTable();
