#include "stdafx.h"

#include "rescheat.h"

void setResCheatHooks ()
{
	int f10k = 0x461C4000;  //10000f
	setInt (0x00604407, f10k);
	setInt (0x00604433, f10k);
	setInt (0x0060445F, f10k);
	setInt (0x0060448B, f10k);
}
