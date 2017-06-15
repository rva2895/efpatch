#pragma once

void initBldgResProdList ();
int resGenHook ();
void setResGenHooks ();

typedef struct resGen
{
	int unitID;
	int useControlRes;
	int controlResID;
	int constantResAmount;
	int resID;
} resGen;
