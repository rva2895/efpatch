#include "stdafx.h"

#include "ai.h"
#include "aiunitcount.h"
#include "aibuilders.h"
#include "aisnx.h"
#include "aicommandunits.h"

void setAIHooks()
{
#ifdef _DEBUG
	log("Setting AI hooks:");
#endif

	setAIBuildersCntrHooks();
	setAIUnitCountHooks();
	setAISNXHooks();

	setAICommandUnitsHooks();
}
