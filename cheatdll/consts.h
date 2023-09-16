#pragma once

#ifdef _CHEATDLL_CC
#define UNIT_COUNT 1596
#define CIV_COUNT 8
#else
//#define UNIT_COUNT 5000
#define CIV_COUNT 15
#endif

#define _BALANCE_PATCH_VER "1.2"

#define AUTOSAVE_TIMER 0xF0

#define VER_CC 0
#define VER_EF 1

#ifndef _CHEATDLL_CC
#define TERRAIN_COUNT 252
#else
#ifdef _CC_COMPATIBLE
#define TERRAIN_COUNT 53
#else
#define TERRAIN_COUNT 54
#endif
#endif

//-1 undefined
//0 old. 1.4.0
//1 1.4.1 rc1
//2 1.4.1
//3 1.4.2
//4 1.5.0
//5 1.5.1
//6 1.5.2

#define CURRENT_VERSION 6
