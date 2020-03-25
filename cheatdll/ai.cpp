#include "stdafx.h"

#include "ai.h"
#include "aiunitcount.h"
#include "aibuilders.h"
#include "aisnx.h"
#include "aicommandunits.h"
#include "aigoals.h"

void setAIHooks()
{
	setAIBuildersCntrHooks();
	setAIUnitCountHooks();
	setAISNXHooks();

	setAICommandUnitsHooks();

	setAIGoalsHooks();
}
